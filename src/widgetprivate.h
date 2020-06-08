#ifndef WIDGETPRIVATE_H
#define WIDGETPRIVATE_H

#include <SDL2/SDL.h>

#include "widget.h"

#define DWidgetSignalDataMax 16

typedef void (DWidgetVTableDestructor)(DWidget *widget);
typedef int (DWidgetVTableGetMinWidth)(const DWidget *widget);
typedef int (DWidgetVTableGetMinHeight)(const DWidget *widget);
typedef int (DWidgetVTableGetWidth)(const DWidget *widget);
typedef int (DWidgetVTableGetHeight)(const DWidget *widget);
typedef int (DWidgetVTableGetChildXOffset)(const DWidget *parent, const DWidget *child);
typedef int (DWidgetVTableGetChildYOffset)(const DWidget *parent, const DWidget *child);

typedef struct {
	DWidgetVTableDestructor *destructor;
	DWidgetVTableGetMinWidth *getMinWidth;
	DWidgetVTableGetMinHeight *getMinHeight;
	DWidgetVTableGetWidth *getWidth;
	DWidgetVTableGetHeight *getHeight;
	DWidgetVTableGetChildXOffset *getChildXOffset;
	DWidgetVTableGetChildYOffset *getChildYOffset;
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
void dWidgetConstructor(DWidget *widget, DWidgetObjectData *data);
void dWidgetDestructor(DWidget *widget, DWidgetObjectData *data); // starts from data sub class when searching for vtable entries (if data is NULL then function does nothing)

DWidgetObjectData *dWidgetGetObjectData(DWidget *widget, DWidgetType subType);
DWidgetObjectData *dWidgetGetObjectDataNoFail(DWidget *widget, DWidgetType subType); // return will never be NULL, and type will always match that given (otherwise program is aborted)

const DWidgetObjectData *dWidgetGetObjectDataConst(const DWidget *widget, DWidgetType subType);
const DWidgetObjectData *dWidgetGetObjectDataConstNoFail(const DWidget *widget, DWidgetType subType); // return will never be NULL, and type will always match that given (otherwise program is aborted)

#endif
