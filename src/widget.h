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
	DWidgetSignalTypeWindowClose,
	DWidgetSignalTypeNB,
} DWidgetSignalType;

typedef struct {
	DWidgetSignalType type;
	DWidget *widget;
	union {
	} d;
} DWidgetSignalEvent;

typedef enum {
	DWidgetSignalReturnContinue,
	DWidgetSignalReturnStop,
} DWidgetSignalReturn;

typedef DWidgetSignalReturn (DWidgetSignalHandler)(const DWidgetSignalEvent *event, void *userData);

#include "widgetprivate.h"

void dWidgetFree(DWidget *widget);

DWidget *dWidgetGetParent(DWidget *widget);
const DWidget *dWidgetGetParentConst(const DWidget *widget);
DWidgetType dWidgetGetBaseType(const DWidget *widget);
bool dWidgetGetHasType(const DWidget *widget, DWidgetType type); // does the base type, or any of the derived types match the type given?
int dWidgetGetMinWidth(const DWidget *widget);
int dWidgetGetMinHeight(const DWidget *widget);
int dWidgetGetWidth(const DWidget *widget);
int dWidgetGetHeight(const DWidget *widget);
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
