#ifndef WIDGET_H
#define WIDGET_H

#include <stdbool.h>

typedef enum {
	DWidgetOrientationHorizontal,
	DWidgetOrientationVertical,
} DWidgetOrientation;

typedef enum {
	DWidgetTypeBin,
	DWidgetTypeBox,
	DWidgetTypeButton,
	DWidgetTypeContainer,
	DWidgetTypeLabel,
	DWidgetTypeTextButton,
	DWidgetTypeWindow,
	DWidgetTypeWidget, // common base widget
	DWidgetTypeNB,
} DWidgetType;

typedef struct DWidget DWidget;

typedef enum {
	DWidgetSignalTypeButtonClick,
	DWidgetSignalTypeWidgetButtonPress, // note: this and WidgetButtonRelease may not come in pairs if the mouse is dragged while pressed
	DWidgetSignalTypeWidgetButtonRelease,
	DWidgetSignalTypeWidgetEnter, // cursor has entered this widget
	DWidgetSignalTypeWidgetLeave, // cursor has left this widget
	DWidgetSignalTypeWindowClose,
	DWidgetSignalTypeNB,
} DWidgetSignalType;

typedef enum {
	DWidgetMouseButtonLeft,
	DWidgetMouseButtonMiddle,
	DWidgetMouseButtonRight,
	DWidgetMouseButtonX1,
	DWidgetMouseButtonX2,
} DWidgetMouseButton;

typedef struct {
	DWidgetMouseButton button;
} DWidgetSignalEventWidgetButtonPress;

typedef struct {
	DWidgetMouseButton button;
} DWidgetSignalEventWidgetButtonRelease;

typedef struct {
	DWidgetSignalType type;
	DWidget *widget;
	union {
		DWidgetSignalEventWidgetButtonPress widgetButtonPress;
		DWidgetSignalEventWidgetButtonRelease widgetButtonRelease;
	} d;
} DWidgetSignalEvent;

typedef enum {
	DWidgetSignalReturnContinue,
	DWidgetSignalReturnStop,
} DWidgetSignalReturn;

typedef DWidgetSignalReturn (DWidgetSignalHandler)(const DWidgetSignalEvent *event, void *userData);

void dWidgetFree(DWidget *widget);

DWidget *dWidgetGetParent(DWidget *widget);
const DWidget *dWidgetGetParentConst(const DWidget *widget);
bool dWidgetIsAncestor(const DWidget *widget, const DWidget *child); // returns true if child has widget among its parents
DWidgetType dWidgetGetBaseType(const DWidget *widget);
bool dWidgetGetHasType(const DWidget *widget, DWidgetType type); // does the base type, or any of the derived types match the type given?
// These two functions return the root Window containing the widget (or NULL if not part of a Window)
DWidget *dWidgetGetWindow(DWidget *widget);
const DWidget *dWidgetGetWindowConst(const DWidget *widget);
DWidget *dWidgetGetWidgetByXY(DWidget *widget, int globalX, int globalY); // x and y are relative to the top left of the root Window
int dWidgetGetMinWidth(DWidget *widget);
int dWidgetGetMinHeight(DWidget *widget);
int dWidgetGetWidth(DWidget *widget);
int dWidgetGetHeight(DWidget *widget);
// These two functions return values relative to the top left corner of the parent Window
int dWidgetGetGlobalX(DWidget *widget);
int dWidgetGetGlobalY(DWidget *widget);
// These two functions return values relatlive to the top left corner of the parent container
int dWidgetGetChildXOffset(DWidget *parent, DWidget *child);
int dWidgetGetChildYOffset(DWidget *parent, DWidget *child);
int dWidgetGetPaddingTop(const DWidget *widget);
int dWidgetGetPaddingBottom(const DWidget *widget);
int dWidgetGetPaddingLeft(const DWidget *widget);
int dWidgetGetPaddingRight(const DWidget *widget);
int dWidgetGetOrientation(const DWidget *widget);
int dWidgetGetHExpand(const DWidget *widget);
int dWidgetGetVExpand(const DWidget *widget);

void dWidgetSetPadding(DWidget *widget, int padding); // equivalent to calling each individual function with the same padding value
void dWidgetSetPaddingTop(DWidget *widget, int padding);
void dWidgetSetPaddingBottom(DWidget *widget, int padding);
void dWidgetSetPaddingLeft(DWidget *widget, int padding);
void dWidgetSetPaddingRight(DWidget *widget, int padding);
void dWidgetSetOrientation(DWidget *widget, DWidgetOrientation orientation);
void dWidgetSetHExpand(DWidget *widget, bool hexpand);
void dWidgetSetVExpand(DWidget *widget, bool vexpand);

bool dWidgetSignalConnect(DWidget *widget, DWidgetSignalType type, DWidgetSignalHandler *handler, void *userData);
DWidgetSignalReturn dWidgetSignalInvoke(const DWidgetSignalEvent *event); // returns DWidgetSignalReturnStop if any handlers do, otherwise returns DWidgetSignalReturnContinue

void dWidgetDebug(DWidget *widget, int indentation);

bool dWidgetOrientationIsValid(DWidgetOrientation orientation);

bool dWidgetTypeIsValid(DWidgetType type);
const char *dWidgetTypeToString(DWidgetType type);

bool dWidgetSignalTypeIsValid(DWidgetSignalType type);
const char *dWidgetSignalTypeToString(DWidgetSignalType type);
DWidgetType dWidgetSignalTypeToWidgetType(DWidgetSignalType type);

#endif
