#include <assert.h>
#include <stdlib.h>

#include <SDL2/SDL.h>

#include "window.h"

DWidget *dWindowNew(const char *title, int width, int height) {
	assert(title!=NULL);
	assert(width>=0);
	assert(height>=0);

	// Create widget instance
	DWidget *window=dWidgetNew(DWidgetTypeWindow);

	// Create SDL backing window
	window->base->d.window.sdlWindow=SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_RESIZABLE);
	if (window->base->d.window.sdlWindow==NULL) {
		dWidgetFree(window);
		return NULL;
	}

	return window;
}
