#include <SDL2/SDL.h>

#include "digits.h"

DWidget *digitsGetWidgetFromSdlWindowId(unsigned id);

bool digitsInit(void) {
	// Initialise SDL
	if(SDL_Init(SDL_INIT_VIDEO)<0)
		return false;

	return true;
}

void digitsQuit(void) {
	SDL_Quit();
}

void digitsLoop(void) {
	bool quit=false;

	while(!quit) {
		// Check SDL events
		SDL_Event sdlEvent;
		while(!quit && SDL_PollEvent(&sdlEvent)) {
			switch(sdlEvent.type) {
				case SDL_QUIT:
					quit=true;
				break;
			}
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
