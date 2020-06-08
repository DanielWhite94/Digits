#ifndef WIDGET_H
#define WIDGET_H

#include <stdbool.h>

typedef enum {
	DWidgetTypeBin,
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
DWidgetType dWidgetGetBaseType(const DWidget *widget);
bool dWidgetGetHasType(const DWidget *widget, DWidgetType type); // does the base type, or any of the derived types match the type given?
DWidget *dWidgetGetWidgetByXY(DWidget *widget, int globalX, int globalY); // x and y are relative to the top left of the root Window
int dWidgetGetMinWidth(const DWidget *widget);
int dWidgetGetMinHeight(const DWidget *widget);
int dWidgetGetWidth(const DWidget *widget);
int dWidgetGetHeight(const DWidget *widget);
// These two functions return values relative to the top left corner of the parent Window
int dWidgetGetGlobalX(const DWidget *widget);
int dWidgetGetGlobalY(const DWidget *widget);
// These two functions return values relatlive to the top left corner of the parent container
int dWidgetGetChildXOffset(const DWidget *parent, const DWidget *child);
int dWidgetGetChildYOffset(const DWidget *parent, const DWidget *child);

bool dWidgetSignalConnect(DWidget *widget, DWidgetSignalType type, DWidgetSignalHandler *handler, void *userData);
DWidgetSignalReturn dWidgetSignalInvoke(const DWidgetSignalEvent *event); // returns DWidgetSignalReturnStop if any handlers do, otherwise returns DWidgetSignalReturnContinue

void dWidgetDebug(const DWidget *widget, int indentation);

bool dWidgetTypeIsValid(DWidgetType type);
const char *dWidgetTypeToString(DWidgetType type);

bool dWidgetSignalTypeIsValid(DWidgetSignalType type);
const char *dWidgetSignalTypeToString(DWidgetSignalType type);
DWidgetType dWidgetSignalTypeToWidgetType(DWidgetSignalType type);

#endif
