#ifndef WIDGETPRIVATE_H
#define WIDGETPRIVATE_H

#include <SDL2/SDL.h>

#include "widget.h"

#define DWidgetSignalDataMax 16

typedef void (DWidgetVTableDestructor)(DWidget *widget);
typedef void (DWidgetVTableRedraw)(DWidget *widget, SDL_Renderer *renderer);
typedef int (DWidgetVTableGetMinWidth)(DWidget *widget);
typedef int (DWidgetVTableGetMinHeight)(DWidget *widget);
typedef int (DWidgetVTableGetWidth)(DWidget *widget);
typedef int (DWidgetVTableGetHeight)(DWidget *widget);
typedef int (DWidgetVTableGetChildXOffset)(DWidget *parent, DWidget *child);
typedef int (DWidgetVTableGetChildYOffset)(DWidget *parent, DWidget *child);

typedef struct {
	DWidgetVTableDestructor *destructor;
	DWidgetVTableRedraw *redraw;
	DWidgetVTableGetMinWidth *getMinWidth;
	DWidgetVTableGetMinHeight *getMinHeight;
	DWidgetVTableGetWidth *getWidth;
	DWidgetVTableGetHeight *getHeight;
	DWidgetVTableGetChildXOffset *getChildXOffset;
	DWidgetVTableGetChildYOffset *getChildYOffset;
} DWidgetVTable;

typedef struct {
	bool pressed; // true if currently held down (i.e. mid click)
} DWidgetObjectDataButton;

typedef struct {
	DWidget **children;
	size_t childCount;
} DWidgetObjectDataContainer;

typedef struct {
	char *text;

	SDL_Texture *texture;
} DWidgetObjectDataLabel;

typedef struct {
	int paddingTop;
	int paddingBottom;
	int paddingLeft;
	int paddingRight;

	DWidgetOrientation orientation; // not all widgets will use this value, it affects things such a Box and Separator

	bool hexpand, vexpand; // horizontal and vertical expand flags
} DWidgetObjectDataWidget;

typedef struct {
	SDL_Window *sdlWindow;
	SDL_Renderer *renderer;

	bool dirty; // true if need to redraw

	DWidget *mouseFocusWidget; // widget under the mouse (can be NULL if mouse not inside window)
} DWidgetObjectDataWindow;

typedef struct DWidgetObjectData DWidgetObjectData;
struct DWidgetObjectData {
	DWidgetType type;
	DWidgetObjectData *super;
	union {
		DWidgetObjectDataButton button;
		DWidgetObjectDataContainer container;
		DWidgetObjectDataLabel label;
		DWidgetObjectDataWidget widget;
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

SDL_Renderer *dWidgetGetRenderer(DWidget *widget); // returns NULL if not a Window or descendant of a Window
void dWidgetSetDirty(DWidget *widget); // sets dirty flag of containing window

void dWidgetRedraw(DWidget *widget, DWidgetObjectData *data, SDL_Renderer *renderer); // starts from data sub class when searching for vtable entries (if data is NULL then function does nothing)

DWidgetObjectData *dWidgetGetObjectData(DWidget *widget, DWidgetType subType);
DWidgetObjectData *dWidgetGetObjectDataNoFail(DWidget *widget, DWidgetType subType); // return will never be NULL, and type will always match that given (otherwise program is aborted)

const DWidgetObjectData *dWidgetGetObjectDataConst(const DWidget *widget, DWidgetType subType);
const DWidgetObjectData *dWidgetGetObjectDataConstNoFail(const DWidget *widget, DWidgetType subType); // return will never be NULL, and type will always match that given (otherwise program is aborted)

#endif
