#include <SDL2/SDL.h>

#include "digits.h"

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
