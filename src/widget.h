#ifndef WIDGET_H
#define WIDGET_H

typedef enum {
	DWidgetTypeBin,
	DWidgetTypeLabel,
	DWidgetTypeWindow,
} DWidgetType;

typedef struct DWidget DWidget;

#include "widgetprivate.h"

void dWidgetFree(DWidget *widget);

DWidget *dWidgetGetParent(DWidget *widget);
DWidgetType dWidgetGetBaseType(const DWidget *widget);

const char *dWidgetTypeToString(DWidgetType type);

#endif
