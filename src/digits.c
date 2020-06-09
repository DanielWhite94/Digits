#include <assert.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include "digits.h"
#include "digitsprivate.h"
#include "util.h"
#include "windowprivate.h"

bool digitsInitFlag=false;
bool digitsQuitFlag=false;

DWidget **digitsWindows=NULL;
size_t digitWindowCount=0;

void digitsLoopHandleSdlEvents(void);

DWidget *digitsGetWidgetFromSdlWindowId(unsigned id);

ssize_t digitsGetWindowIndex(const DWidget *widget); // find index of given window widget in the windows array. returns -1 on failure

bool digitsInit(void) {
	// Already initialised?
	if (digitsInitFlag)
		return true;

	// Update fields
	digitsQuitFlag=false;
	digitsWindows=NULL;
	digitWindowCount=0;

	// Initialise SDL
	if(SDL_Init(SDL_INIT_VIDEO)<0)
		return false;
	if (TTF_Init()!=0) {
		SDL_Quit();
		return false;
	}

	// Initialisation complete
	digitsInitFlag=true;

	return true;
}

void digitsQuit(void) {
	// Not even initialised?
	if (!digitsInitFlag)
		return;

	// Close open windows and free memory
	free(digitsWindows);
	digitsWindows=NULL;

	// Quit SDL
	TTF_Quit();
	SDL_Quit();

	// Clear init and quit flags
	digitsInitFlag=false;
	digitsQuitFlag=false;
}

void digitsLoop(void) {
	digitsQuitFlag=false;
	while(!digitsQuitFlag) {
		// Check SDL events
		digitsLoopHandleSdlEvents();

		// Delay to avoid high CPU usage
		dDelayMs(10);
	}
}

void digitsLoopStop(void) {
	digitsQuitFlag=true;
}


void digitsRegisterWindow(DWidget *widget) {
	assert(widget!=NULL);
	assert(dWidgetGetHasType(widget, DWidgetTypeWindow));

	// Already registered?
	if (digitsGetWindowIndex(widget)!=-1)
		return;

	// Add window to array
	digitsWindows=dReallocNoFail(digitsWindows, sizeof(DWidget *)*(digitWindowCount+1));
	digitsWindows[digitWindowCount++]=widget;
}

void digitsDeregisterWindow(DWidget *widget) {
	assert(widget!=NULL);
	assert(dWidgetGetHasType(widget, DWidgetTypeWindow));

	// Find index in array
	ssize_t index=digitsGetWindowIndex(widget);
	if (index==-1)
		return;

	// Remove from array by shifting others down and reducing the count
	memmove(digitsWindows+index, digitsWindows+index+1, (--digitWindowCount)-index);
}

void digitsLoopHandleSdlEvents(void) {
	// Handle events until none remain
	SDL_Event sdlEvent;
	while(SDL_PollEvent(&sdlEvent)) {
		switch(sdlEvent.type) {
			case SDL_MOUSEBUTTONDOWN: {
				// Find widget represented by this event's SDL window ID
				DWidget *windowWidget=digitsGetWidgetFromSdlWindowId(sdlEvent.button.windowID);
				if (windowWidget==NULL) {
					dWarning("warning: could not get window widget for SDL_MOUSEBUTTONDOWN event, ignoring\n");
					break;
				}

				// Find widget within the window which is actually under the mouse
				DWidget *targetWidget=dWidgetGetWidgetByXY(windowWidget, sdlEvent.button.x, sdlEvent.button.y);
				if (targetWidget==NULL) {
					dWarning("warning: could not get target widget for SDL_MOUSEBUTTONDOWN event at (%i,%i), ignoring\n", sdlEvent.button.x, sdlEvent.button.y);
					break;
				}

				// Invoke widget button press signal
				// Do this recursively up the widget tree until a handler 'accepts' it by returning Stop
				DWidgetSignalEvent dEvent;
				dEvent.type=DWidgetSignalTypeWidgetButtonPress;
				dEvent.d.widgetButtonPress.button=sdlEvent.button.button;
				while(targetWidget!=NULL) {
					dEvent.widget=targetWidget;
					if (dWidgetSignalInvoke(&dEvent)==DWidgetSignalReturnStop)
						break;

					targetWidget=dWidgetGetParent(targetWidget);
				}
			} break;
			case SDL_MOUSEBUTTONUP: {
				// Find widget represented by this event's SDL window ID
				DWidget *windowWidget=digitsGetWidgetFromSdlWindowId(sdlEvent.button.windowID);
				if (windowWidget==NULL) {
					dWarning("warning: could not get window widget for SDL_MOUSEBUTTONUP event, ignoring\n");
					break;
				}

				// Find widget within the window which is actually under the mouse
				DWidget *targetWidget=dWidgetGetWidgetByXY(windowWidget, sdlEvent.button.x, sdlEvent.button.y);
				if (targetWidget==NULL) {
					dWarning("warning: could not get target widget for SDL_MOUSEBUTTONUP event at (%i,%i), ignoring\n", sdlEvent.button.x, sdlEvent.button.y);
					break;
				}

				// Invoke widget button release signal
				// Do this recursively up the widget tree until a handler 'accepts' it by returning Stop
				DWidgetSignalEvent dEvent;
				dEvent.type=DWidgetSignalTypeWidgetButtonRelease;
				dEvent.d.widgetButtonPress.button=sdlEvent.button.button;
				while(targetWidget!=NULL) {
					dEvent.widget=targetWidget;
					if (dWidgetSignalInvoke(&dEvent)==DWidgetSignalReturnStop)
						break;

					targetWidget=dWidgetGetParent(targetWidget);
				}
			} break;
			case SDL_WINDOWEVENT: {
				// Find widget represented by this event's SDL window ID
				DWidget *windowWidget=digitsGetWidgetFromSdlWindowId(sdlEvent.window.windowID);
				if (windowWidget==NULL) {
					dWarning("warning: could not get window widget for SDL_WINDOWEVENT event, ignoring\n");
					break;
				}

				// Event specific logic
				switch(sdlEvent.window.event) {
					case SDL_WINDOWEVENT_EXPOSED:
						// Call redraw on window
						dWidgetRedraw(windowWidget, windowWidget->base, dWindowGetRenderer(windowWidget));
					break;
					case SDL_WINDOWEVENT_CLOSE: {
						// Invoke window close signal
						DWidgetSignalEvent dEvent;
						dEvent.type=DWidgetSignalTypeWindowClose;
						dEvent.widget=windowWidget;
						dWidgetSignalInvoke(&dEvent);
					} break;
				}
			} break;
			case SDL_QUIT:
				// TODO: remove this once we have a better way of terminating (as otherwise closing all windows will cause this to fire)
				digitsLoopStop();
			break;
		}
	}
}

DWidget *digitsGetWidgetFromSdlWindowId(unsigned id) {
	SDL_Window *sdlWindow=SDL_GetWindowFromID(id);
	if (sdlWindow==NULL) {
		dWarning("warning: no SDL window with id %u\n", id);
		return NULL;
	}

	DWidget *widget=SDL_GetWindowData(sdlWindow, "widget");
	if (widget==NULL) {
		dWarning("warning: no widget data in SDL window %p (id %u)\n", sdlWindow, id);
		return NULL;
	}

	// As an additonal safety check, verify widget is a window
	if (dWidgetGetBaseType(widget)!=DWidgetTypeWindow) {
		dWarning("warning: expected widget of type %s but got %s (widget %p, SDL window %p, id %u)\n", dWidgetTypeToString(DWidgetTypeWindow), dWidgetTypeToString(dWidgetGetBaseType(widget)), widget, sdlWindow, id);
		return NULL;
	}

	return widget;
}

ssize_t digitsGetWindowIndex(const DWidget *widget) {
	assert(widget!=NULL);

	for(size_t i=0; i<digitWindowCount; ++i)
		if (widget==digitsWindows[i])
			return i;
	return -1;
}

