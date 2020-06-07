#ifndef WIDGETPRIVATE_H
#define WIDGETPRIVATE_H

#include <SDL2/SDL.h>

#include "widget.h"

#define DWidgetSignalDataMax 16

typedef int (DWidgetVTableGetMinWidth)(const DWidget *widget);
typedef int (DWidgetVTableGetMinHeight)(const DWidget *widget);
typedef int (DWidgetVTableGetWidth)(const DWidget *widget);
typedef int (DWidgetVTableGetHeight)(const DWidget *widget);

typedef struct {
	DWidgetVTableGetMinWidth *getMinWidth;
	DWidgetVTableGetMinHeight *getMinHeight;
	DWidgetVTableGetWidth *getWidth;
	DWidgetVTableGetHeight *getHeight;
} DWidgetVTable;

typedef struct {
	DWidget **children;
	size_t childCount;
} DWidgetObjectDataContainer;

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
		DWidgetObjectDataContainer container;
		DWidgetObjectDataLabel label;
		DWidgetObjectDataWindow window;
	} d;

	DWidgetVTable vtable;
};

typedef struct {
	DWidgetSignalHandler *handler;
	void *userData;
} DWidgetSignalData;

struct DWidget {
	DWidgetObjectData *base;
	DWidget *parent;

	DWidgetSignalData signals[DWidgetSignalTypeNB][DWidgetSignalDataMax];
	size_t signalsCount[DWidgetSignalTypeNB];
};

DWidget *dWidgetNew(DWidgetType type);

DWidgetObjectData *dWidgetGetObjectData(DWidget *widget, DWidgetType subType);
DWidgetObjectData *dWidgetGetObjectDataNoFail(DWidget *widget, DWidgetType subType); // return will never be NULL, and type will always match that given (otherwise program is aborted)

const DWidgetObjectData *dWidgetGetObjectDataConst(const DWidget *widget, DWidgetType subType);
const DWidgetObjectData *dWidgetGetObjectDataConstNoFail(const DWidget *widget, DWidgetType subType); // return will never be NULL, and type will always match that given (otherwise program is aborted)

int dWidgetVTableGetMinWidth(const DWidget *widget);
int dWidgetVTableGetMinHeight(const DWidget *widget);
int dWidgetVTableGetWidth(const DWidget *widget);
int dWidgetVTableGetHeight(const DWidget *widget);

#endif
