#include <assert.h>
#include <stdlib.h>

#include <SDL2/SDL.h>

#include "widgetprivate.h"
#include "window.h"
#include "windowprivate.h"

DWidget *dWindowNew(const char *title, int width, int height) {
	assert(title!=NULL);
	assert(width>=0);
	assert(height>=0);

	// Create widget instance
	DWidget *widget=dWidgetNew(DWidgetTypeWindow);

	// Create SDL backing window and add some custom data to point back to our widget
	widget->base->d.window.sdlWindow=SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_RESIZABLE);
	if (widget->base->d.window.sdlWindow==NULL) {
		dWidgetFree(widget);
		return NULL;
	}

	SDL_SetWindowData(widget->base->d.window.sdlWindow, "widget", widget);

	return widget;
}

const char *dWindowGetTitle(const DWidget *widget) {
	assert(widget!=NULL);

	const DWidgetObjectData *data=dWidgetGetObjectDataConstNoFail(widget, DWidgetTypeWindow);

	return SDL_GetWindowTitle(data->d.window.sdlWindow);
}

int dWindowVTableGetWidth(const DWidget *widget) {
	assert(widget!=NULL);

	const DWidgetObjectData *data=dWidgetGetObjectDataConstNoFail(widget, DWidgetTypeWindow);

	int width;
	SDL_GetWindowSize(data->d.window.sdlWindow, &width, NULL);
	return width;
}

int dWindowVTableGetHeight(const DWidget *widget) {
	assert(widget!=NULL);

	const DWidgetObjectData *data=dWidgetGetObjectDataConstNoFail(widget, DWidgetTypeWindow);

	int height;
	SDL_GetWindowSize(data->d.window.sdlWindow, NULL, &height);
	return height;
}
