#include <unistd.h>

#include <SDL2/SDL.h>

#include "digits.h"
#include "util.h"

bool digitsInitFlag=false;
bool digitsQuitFlag=false;

void digitsLoopHandleSdlEvents(void);

DWidget *digitsGetWidgetFromSdlWindowId(unsigned id);

bool digitsInit(void) {
	// Already initialised?
	if (digitsInitFlag)
		return true;

	// Update flags
	digitsQuitFlag=false;

	// Initialise SDL
	if(SDL_Init(SDL_INIT_VIDEO)<0)
		return false;

	// Initialisation complete
	digitsInitFlag=true;

	return true;
}

void digitsQuit(void) {
	// Not even initialised?
	if (!digitsInitFlag)
		return;

	// Quit SDL
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
		usleep(10);
	}
}

void digitsLoopStop(void) {
	digitsQuitFlag=true;
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
