#ifndef WINDOWPRIVATE_H
#define WINDOWPRIVATE_H

#include <SDL2/SDL.h>

#include "widget.h"
#include "widgetprivate.h"

void dWindowConstructor(DWidget *widget, DWidgetObjectData *data, const char *title, int width, int height);

SDL_Renderer *dWindowGetRenderer(DWidget *widget);

void dWindowSetDirty(DWidget *window);
void dWindowSetMouseFocusWidget(DWidget *window, DWidget *newWidget);

#endif
