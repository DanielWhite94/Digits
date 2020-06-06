#ifndef WIDGETPRIVATE_H
#define WIDGETPRIVATE_H

#include <SDL2/SDL.h>

#include "widget.h"

typedef struct {
	DWidget *child;
} DWidgetObjectDataBin;

typedef struct {
	char *text;
} DWidgetObjectDataLabel;

typedef struct {
	SDL_Window *sdlWindow;
} DWidgetObjectDataWindow;

typedef struct DWidgetObjectData DWidgetObjectData;
struct DWidgetObjectData {
	DWidgetType type;
	DWidgetObjectData *super;
	union {
		DWidgetObjectDataBin bin;
		DWidgetObjectDataLabel label;
		DWidgetObjectDataWindow window;
	} d;
};

struct DWidget {
	DWidgetObjectData *base;
	DWidget *parent;
};

DWidget *dWidgetNew(DWidgetType type);

DWidgetObjectData *dWidgetGetObjectData(DWidget *widget, DWidgetType subType);
DWidgetObjectData *dWidgetGetObjectDataNoFail(DWidget *widget, DWidgetType subType); // return will never be NULL, and type will always match that given (otherwise program is aborted)

const DWidgetObjectData *dWidgetGetObjectDataConst(const DWidget *widget, DWidgetType subType);
const DWidgetObjectData *dWidgetGetObjectDataConstNoFail(const DWidget *widget, DWidgetType subType); // return will never be NULL, and type will always match that given (otherwise program is aborted)

#endif
