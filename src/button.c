#include <assert.h>

#include "bin.h"
#include "binprivate.h"
#include "button.h"
#include "buttonprivate.h"
#include "util.h"
#include "widgetprivate.h"

DWidgetSignalReturn dButtonHandlerButtonPress(const DWidgetSignalEvent *event, void *userData);
DWidgetSignalReturn dButtonHandlerButtonRelease(const DWidgetSignalEvent *event, void *userData);

DWidget *dButtonNew(DWidget *child) {
	// Create widget instance
	DWidget *button=dWidgetNew(DWidgetTypeButton);

	// Call constructor
	dButtonConstructor(button, button->base, child);

	return button;
}

void dButtonConstructor(DWidget *widget, DWidgetObjectData *data, DWidget *child) {
	assert(widget!=NULL);
	assert(data!=NULL);
	assert(data->type==DWidgetTypeButton);

	// Call super constructor first
	dBinConstructor(widget, data->super, child);

	// Init fields
	data->d.button.pressed=false;

	// Connect signals to handle clicking logic
	if (!dWidgetSignalConnect(widget, DWidgetSignalTypeWidgetButtonPress, &dButtonHandlerButtonPress, NULL) ||
	    !dWidgetSignalConnect(widget, DWidgetSignalTypeWidgetButtonRelease, &dButtonHandlerButtonRelease, NULL)) {
		// This shouldn't really happen - there is no reason the handlers can fail to connect
		dFatalError("error: could not connect internal signals for Button %p\n", widget);
	}
}

DWidgetSignalReturn dButtonHandlerButtonPress(const DWidgetSignalEvent *event, void *userData) {
	assert(event!=NULL);
	assert(userData==NULL);

	DWidgetObjectData *data=dWidgetGetObjectDataNoFail(event->widget, DWidgetTypeButton);

	// Set pressed flag
	data->d.button.pressed=true;

	// Indicate we have handled this event
	return DWidgetSignalReturnStop;
}

DWidgetSignalReturn dButtonHandlerButtonRelease(const DWidgetSignalEvent *event, void *userData) {
	assert(event!=NULL);
	assert(userData==NULL);

	DWidgetObjectData *data=dWidgetGetObjectDataNoFail(event->widget, DWidgetTypeButton);

	// clear pressed flag
	data->d.button.pressed=false;

	// Indicate we have handled this event
	return DWidgetSignalReturnStop;
}
