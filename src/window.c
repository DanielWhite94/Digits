#include <assert.h>
#include <stdlib.h>

#include <SDL2/SDL.h>

#include "binprivate.h"
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
}

void dWindowVTableRedraw(DWidget *widget, SDL_Renderer *renderer) {
	assert(widget!=NULL);
	assert(renderer!=NULL);

	DWidgetObjectData *data=dWidgetGetObjectDataNoFail(widget, DWidgetTypeWindow);

	// Clear entire window to background colour
	dSetRenderDrawColour(renderer, &dWindowBackgroundColour);
	SDL_RenderClear(renderer);

	// Call super redraw
	dWidgetRedraw(widget, data->super, renderer);

	// Update screen
	SDL_RenderPresent(renderer);
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
