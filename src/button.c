#include <assert.h>

#include "bin.h"
#include "binprivate.h"
#include "button.h"
#include "buttonprivate.h"
#include "util.h"
#include "utilprivate.h"
#include "widgetprivate.h"

const DColour dButtonPressedColour={.r=192, .g=192, .b=192, .a=255};
const DColour dButtonReleasedColour={.r=128, .g=128, .b=128, .a=255};

void dButtonVTableRedraw(DWidget *widget, SDL_Renderer *renderer);

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

	// Setup vtable
	data->vtable.redraw=&dButtonVTableRedraw;

	// Connect signals to handle clicking logic
	if (!dWidgetSignalConnect(widget, DWidgetSignalTypeWidgetButtonPress, &dButtonHandlerWidgetButtonPress, NULL) ||
	    !dWidgetSignalConnect(widget, DWidgetSignalTypeWidgetButtonRelease, &dButtonHandlerWidgetButtonRelease, NULL) ||
	    !dWidgetSignalConnect(widget, DWidgetSignalTypeWidgetLeave, &dButtonHandlerWidgetLeave, NULL)) {
		// This shouldn't really happen - there is no reason the handlers can fail to connect
		dFatalError("error: could not connect internal signals for Button %p\n", widget);
	}
}

void dButtonVTableRedraw(DWidget *widget, SDL_Renderer *renderer) {
	assert(widget!=NULL);
	assert(renderer!=NULL);

	DWidgetObjectData *data=dWidgetGetObjectDataNoFail(widget, DWidgetTypeButton);

	// Draw rectangle to represent body of button
	dSetRenderDrawColour(renderer, (data->d.button.pressed ? &dButtonPressedColour : &dButtonReleasedColour));
	SDL_Rect rect;
	rect.x=dWidgetGetGlobalX(widget);
	rect.y=dWidgetGetGlobalY(widget);
	rect.w=dWidgetGetWidth(widget);
	rect.h=dWidgetGetHeight(widget);
	SDL_RenderFillRect(renderer, &rect);

	// Call super redraw
	dWidgetRedraw(widget, data->super, renderer);
}

DWidgetSignalReturn dButtonHandlerWidgetButtonPress(const DWidgetSignalEvent *event, void *userData) {
	assert(event!=NULL);
	assert(userData==NULL);

	DWidgetObjectData *data=dWidgetGetObjectDataNoFail(event->widget, DWidgetTypeButton);

	// Already pressed?
	if (data->d.button.pressed)
		return DWidgetSignalReturnContinue;

	// Set pressed flag
	data->d.button.pressed=true;

	// Mark window as dirty to redraw
	dWidgetSetDirty(event->widget);

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

	// Mark window as dirty to redraw
	dWidgetSetDirty(event->widget);

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

	// Not pressed?
	if (!data->d.button.pressed)
		return DWidgetSignalReturnContinue;

	// Clear pressed flag (but do not invoke clicked signal - consider process aborted)
	data->d.button.pressed=false;

	// Mark window as dirty to redraw
	dWidgetSetDirty(event->widget);

	return DWidgetSignalReturnStop;
}
