#include <assert.h>
#include <stdlib.h>

#include <SDL2/SDL.h>

#include "binprivate.h"
#include "digitsprivate.h"
#include "util.h"
#include "utilprivate.h"
#include "widgetprivate.h"
#include "window.h"
#include "windowprivate.h"

const DColour dWindowBackgroundColour={.r=32, .g=32, .b=32, .a=255};

void dWindowVTableDestructor(DWidget *widget);
void dWindowVTableRedraw(DWidget *widget, SDL_Renderer *renderer);
int dWindowVTableGetWidth(DWidget *widget);
int dWindowVTableGetHeight(DWidget *widget);

DWidget *dWindowNew(const char *title, int width, int height) {
	assert(title!=NULL);
	assert(width>=0);
	assert(height>=0);

	// Create widget instance
	DWidget *widget=dWidgetNew(DWidgetTypeWindow);

	// Call constructor
	dWindowConstructor(widget, widget->base, title, width, height);

	return widget;
}

void dWindowConstructor(DWidget *widget, DWidgetObjectData *data, const char *title, int width, int height) {
	assert(widget!=NULL);
	assert(data!=NULL);
	assert(data->type==DWidgetTypeWindow);
	assert(title!=NULL);
	assert(width>=0);
	assert(height>=0);

	// Call super constructor first
	dBinConstructor(widget, data->super, NULL);

	// Init fields
	data->d.window.sdlWindow=NULL;
	data->d.window.renderer=NULL;
	data->d.window.dirty=true;
	data->d.window.mouseFocusWidget=NULL;

	// Create SDL backing window and add some custom data to point back to our widget
	data->d.window.sdlWindow=SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_RESIZABLE);
	if (data->d.window.sdlWindow==NULL)
		dFatalError("error: could not create SDL window for widget %p\n", widget);

	SDL_SetWindowData(data->d.window.sdlWindow, "widget", widget);

	// Create SDL renderer for widget drawing
	data->d.window.renderer=SDL_CreateRenderer(data->d.window.sdlWindow, -1, SDL_RENDERER_ACCELERATED);
	if (data->d.window.renderer==NULL)
		dFatalError("error: could not create SDL renderer for widget %p\n", widget);

	// Setup vtable
	data->vtable.destructor=&dWindowVTableDestructor;
	data->vtable.redraw=&dWindowVTableRedraw;
	data->vtable.getWidth=&dWindowVTableGetWidth;
	data->vtable.getHeight=&dWindowVTableGetHeight;

	// Register window so we can keep track of it
	digitsRegisterWindow(widget);
}

const char *dWindowGetTitle(const DWidget *widget) {
	assert(widget!=NULL);

	const DWidgetObjectData *data=dWidgetGetObjectDataConstNoFail(widget, DWidgetTypeWindow);

	return SDL_GetWindowTitle(data->d.window.sdlWindow);
}

SDL_Renderer *dWindowGetRenderer(DWidget *widget) {
	assert(widget!=NULL);

	DWidgetObjectData *data=dWidgetGetObjectDataNoFail(widget, DWidgetTypeWindow);

	return data->d.window.renderer;
}


void dWindowSetDirty(DWidget *window) {
	assert(window!=NULL);

	DWidgetObjectData *data=dWidgetGetObjectDataNoFail(window, DWidgetTypeWindow);

	data->d.window.dirty=true;
}

void dWindowSetMouseFocusWidget(DWidget *window, DWidget *newWidget) {
	assert(window!=NULL);
	assert(newWidget==NULL || newWidget==window || dWidgetIsAncestor(window, newWidget));

	DWidgetObjectData *data=dWidgetGetObjectDataNoFail(window, DWidgetTypeWindow);

	// No change?
	DWidget *oldWidget=data->d.window.mouseFocusWidget;
	if (newWidget==oldWidget)
		return;

	// Keep track of newWidget lineage to help generate Enter events
	DWidget *newWidgetLineageStack[64];
	size_t newWidgetLineageStackCount=0;

	DWidget *loopWidget=newWidget;
	while(loopWidget!=NULL) {
		if (newWidgetLineageStackCount==64)
			dFatalError("error: widget %p has too many ancestors (while generating leave enter events)\n", newWidget);
		newWidgetLineageStack[newWidgetLineageStackCount++]=loopWidget;
		loopWidget=dWidgetGetParent(loopWidget);
	}

	// Generate Leave events from oldWidget upwards, stopping if we hit a common ancestor of newWidget
	size_t commonAncestorIndex=newWidgetLineageStackCount;
	while(oldWidget!=NULL) {
		// Search through new widget lineage to see if this is a common ancestor
		// (if it is, it must be the lowest common ancestor)
		for(size_t i=0; i<newWidgetLineageStackCount; ++i) {
			if (oldWidget==newWidgetLineageStack[i]) {
				commonAncestorIndex=i;
			}
		}
		if (commonAncestorIndex!=newWidgetLineageStackCount)
			break;

		// Invoke widget Leave signal
		DWidgetSignalEvent dEvent;
		dEvent.type=DWidgetSignalTypeWidgetLeave;
		dEvent.widget=oldWidget;
		dWidgetSignalInvoke(&dEvent);

		// Move up the tree to look for next wiget mouse may have left
		oldWidget=dWidgetGetParent(oldWidget);
	}

	// Generate Enter events based on common ancestor found in previous step (if any)
	while(commonAncestorIndex>0) {
		--commonAncestorIndex;

		// Invoke widget Enter signal
		DWidgetSignalEvent dEvent;
		dEvent.type=DWidgetSignalTypeWidgetEnter;
		dEvent.widget=newWidgetLineageStack[commonAncestorIndex];
		dWidgetSignalInvoke(&dEvent);
	}

	// Update cached mouseFocusWidget for next call
	data->d.window.mouseFocusWidget=newWidget;
}

void dWindowVTableDestructor(DWidget *widget) {
	assert(widget!=NULL);

	DWidgetObjectData *data=dWidgetGetObjectDataNoFail(widget, DWidgetTypeWindow);

	// Destry SDL window and renderer
	if (data->d.window.renderer!=NULL)
		SDL_DestroyRenderer(data->d.window.renderer);
	if (data->d.window.sdlWindow!=NULL)
		SDL_DestroyWindow(data->d.window.sdlWindow);

	// Call super destructor
	dWidgetDestructor(widget, data->super);

	// Deregister to remove from list of windows
	digitsDeregisterWindow(widget);
}

void dWindowVTableRedraw(DWidget *widget, SDL_Renderer *renderer) {
	assert(widget!=NULL);
	assert(renderer!=NULL);

	DWidgetObjectData *data=dWidgetGetObjectDataNoFail(widget, DWidgetTypeWindow);

	// Not even dirty?
	if (!data->d.window.dirty)
		return;

	// Clear entire window to background colour
	dSetRenderDrawColour(renderer, &dWindowBackgroundColour);
	SDL_RenderClear(renderer);

	// Call super redraw
	dWidgetRedraw(widget, data->super, renderer);

	// Update screen
	SDL_RenderPresent(renderer);

	// Clear dirty flag
	data->d.window.dirty=false;
}

int dWindowVTableGetWidth(DWidget *widget) {
	assert(widget!=NULL);

	const DWidgetObjectData *data=dWidgetGetObjectDataConstNoFail(widget, DWidgetTypeWindow);

	int width;
	SDL_GetWindowSize(data->d.window.sdlWindow, &width, NULL);
	return width;
}

int dWindowVTableGetHeight(DWidget *widget) {
	assert(widget!=NULL);

	const DWidgetObjectData *data=dWidgetGetObjectDataConstNoFail(widget, DWidgetTypeWindow);

	int height;
	SDL_GetWindowSize(data->d.window.sdlWindow, NULL, &height);
	return height;
}
