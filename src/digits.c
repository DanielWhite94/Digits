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
		warning("warning: no SDL window with id %u\n", id);
		return NULL;
	}

	DWidget *widget=SDL_GetWindowData(sdlWindow, "widget");
	if (widget==NULL) {
		warning("warning: no widget data in SDL window %p (id %u)\n", sdlWindow, id);
		return NULL;
	}

	// As an additonal safety check, verify widget is a window
	if (dWidgetGetBaseType(widget)!=DWidgetTypeWindow) {
		warning("warning: expected widget of type %s but got %s (widget %p, SDL window %p, id %u)\n", dWidgetTypeToString(DWidgetTypeWindow), dWidgetTypeToString(dWidgetGetBaseType(widget)), widget, sdlWindow, id);
		return NULL;
	}

	return widget;
}
