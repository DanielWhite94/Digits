#include <assert.h>

#include "bin.h"
#include "binprivate.h"
#include "button.h"
#include "buttonprivate.h"
#include "util.h"
#include "widgetprivate.h"

DWidgetSignalReturn dButtonHandlerWidgetButtonPress(const DWidgetSignalEvent *event, void *userData);
DWidgetSignalReturn dButtonHandlerWidgetButtonRelease(const DWidgetSignalEvent *event, void *userData);
DWidgetSignalReturn dButtonHandlerWidgetLeave(const DWidgetSignalEvent *event, void *userData);

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
	if (!dWidgetSignalConnect(widget, DWidgetSignalTypeWidgetButtonPress, &dButtonHandlerWidgetButtonPress, NULL) ||
	    !dWidgetSignalConnect(widget, DWidgetSignalTypeWidgetButtonRelease, &dButtonHandlerWidgetButtonRelease, NULL) ||
	    !dWidgetSignalConnect(widget, DWidgetSignalTypeWidgetLeave, &dButtonHandlerWidgetLeave, NULL)) {
		// This shouldn't really happen - there is no reason the handlers can fail to connect
		dFatalError("error: could not connect internal signals for Button %p\n", widget);
	}
}

DWidgetSignalReturn dButtonHandlerWidgetButtonPress(const DWidgetSignalEvent *event, void *userData) {
	assert(event!=NULL);
	assert(userData==NULL);

	DWidgetObjectData *data=dWidgetGetObjectDataNoFail(event->widget, DWidgetTypeButton);

	// Set pressed flag
	data->d.button.pressed=true;

	// Indicate we have handled this event
	return DWidgetSignalReturnStop;
}

DWidgetSignalReturn dButtonHandlerWidgetButtonRelease(const DWidgetSignalEvent *event, void *userData) {
	assert(event!=NULL);
	assert(userData==NULL);

	DWidgetObjectData *data=dWidgetGetObjectDataNoFail(event->widget, DWidgetTypeButton);

	// Not pressed? (can happen if mouse is moved over button while held)
	if (!data->d.button.pressed)
		return DWidgetSignalReturnContinue;

	// Clear pressed flag
	data->d.button.pressed=false;

	// Invoke button click signal
	DWidgetSignalEvent dEvent;
	dEvent.type=DWidgetSignalTypeButtonClick;
	dEvent.widget=event->widget;
	dWidgetSignalInvoke(&dEvent);

	// Indicate we have handled this event
	return DWidgetSignalReturnStop;
}

DWidgetSignalReturn dButtonHandlerWidgetLeave(const DWidgetSignalEvent *event, void *userData) {
	assert(event!=NULL);
	assert(userData==NULL);

	DWidgetObjectData *data=dWidgetGetObjectDataNoFail(event->widget, DWidgetTypeButton);

	// Clear pressed flag (but do not invoke clicked signal - consider process aborted)
	data->d.button.pressed=false;

	return DWidgetSignalReturnStop;
}
