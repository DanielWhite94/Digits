#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "util.h"
#include "widget.h"

DWidgetObjectData *dWidgetObjectDataNew(DWidgetType type);
void dWidgetObjectDataFree(DWidgetObjectData *data);

DWidget *dWidgetNew(DWidgetType type) {
	assert(dWidgetTypeIsValid(type));

	// Allocate widget memory and init fields
	DWidget *widget=mallocNoFail(sizeof(DWidget));

	widget->base=NULL;
	widget->parent=NULL;
	memset(widget->signalsCount, 0, sizeof(widget->signalsCount[0])*DWidgetSignalTypeNB);

	// Initialise base object (and any other it derives from)
	widget->base=dWidgetObjectDataNew(type);

	return widget;
}

void dWidgetFree(DWidget *widget) {
	// NULL check
	if (widget==NULL)
		return;

	// Free base object (and any others it derives from)
	dWidgetObjectDataFree(widget->base);

	// Free memory
	free(widget);
}

DWidget *dWidgetGetParent(DWidget *widget) {
	assert(widget!=NULL);

	return widget->parent;
}

DWidgetType dWidgetGetBaseType(const DWidget *widget) {
	assert(widget!=NULL);

	return widget->base->type;
}

bool dWidgetGetHasType(const DWidget *widget, DWidgetType type) {
	assert(widget!=NULL);
	assert(dWidgetTypeIsValid(type));

	const DWidgetObjectData *data;
	for(data=widget->base; data!=NULL; data=data->super)
		if (data->type==type)
			return true;
	return false;
}

bool dWidgetSignalConnect(DWidget *widget, DWidgetSignalType type, DWidgetSignalHandler *handler, void *userData) {
	assert(widget!=NULL);
	assert(dWidgetSignalTypeIsValid(type));
	assert(handler!=NULL);

	// Is this a valid type of signal for this widget?
	if (!dWidgetGetHasType(widget, dWidgetSignalTypeToWidgetType(type))) {
		warning("warning: can not add signal handler of type %s to widget %p - unsuitable widget type %s\n", dWidgetSignalTypeToString(type), widget, dWidgetTypeToString(dWidgetGetBaseType(widget)));
		return false;
	}

	// Have we reached the limit of handlers for this widget and sub class?
	if (widget->signalsCount[type]>=DWidgetSignalDataMax) {
		warning("warning: can not add signal handler of type %s to widget %p - max of %u already reached\n", dWidgetSignalTypeToString(type), widget, DWidgetSignalDataMax);
		return false;
	}

	// Add this handler to array of handlers
	widget->signals[type][widget->signalsCount[type]].handler=handler;
	widget->signals[type][widget->signalsCount[type]].userData=userData;
	widget->signalsCount[type]++;

	return true;
}

void dWidgetSignalInvoke(const DWidgetSignalEvent *event) {
	assert(event!=NULL);
	assert(dWidgetSignalTypeIsValid(event->type));
	assert(event->widget!=NULL);

	// Loop over registered handlers calling each one in turn (stopping early if any handlers request this)
	for(size_t i=0; i<event->widget->signalsCount[event->type]; ++i) {
		const DWidgetSignalData *signalData=&event->widget->signals[event->type][i];
		if (signalData->handler(event, signalData->userData)==DWidgetSignalReturnStop)
			break;
	}
}

bool dWidgetTypeIsValid(DWidgetType type) {
	return (type>=0 && type<DWidgetTypeNB);
}

static const char *dWidgetTypeStrings[DWidgetTypeNB]={
	[DWidgetTypeBin]="Bin",
	[DWidgetTypeButton]="Button",
	[DWidgetTypeLabel]="Label",
	[DWidgetTypeTextButton]="TextButton",
	[DWidgetTypeWindow]="Window",
	[DWidgetTypeWidget]="Widget",
};
const char *dWidgetTypeToString(DWidgetType type) {
	assert(dWidgetTypeIsValid(type));

	return dWidgetTypeStrings[type];
}

bool dWidgetSignalTypeIsValid(DWidgetSignalType type) {
	return (type>=0 && type<DWidgetSignalTypeNB);
}

static const char *dWidgetSignalTypeStrings[]={
	[DWidgetSignalTypeWindowClose]="WindowClose",
};
const char *dWidgetSignalTypeToString(DWidgetSignalType type) {
	assert(dWidgetSignalTypeIsValid(type));

	return dWidgetSignalTypeStrings[type];
}

DWidgetType dWidgetSignalTypeToWidgetType(DWidgetSignalType type) {
	assert(dWidgetSignalTypeIsValid(type));

	switch(type) {
		case DWidgetSignalTypeWindowClose:
			return DWidgetTypeWindow;
		break;
		case DWidgetSignalTypeNB:
			assert(false);
			return DWidgetTypeNB;
		break;
	}

	assert(false);
	return DWidgetTypeNB;
}

DWidgetObjectData *dWidgetGetObjectData(DWidget *widget, DWidgetType subType) {
	assert(widget!=NULL);

	DWidgetObjectData *data;
	for(data=widget->base; data!=NULL; data=data->super)
		if (data->type==subType)
			break;
	return data;
}

DWidgetObjectData *dWidgetGetObjectDataNoFail(DWidget *widget, DWidgetType subType) {
	assert(widget!=NULL);

	DWidgetObjectData *data=dWidgetGetObjectData(widget, subType);

	if (data==NULL || data->type!=subType) {
		fatalError("error: widget %p does not derive from %s as expected\n", widget, dWidgetTypeToString(subType));
		return NULL;
	}

	return data;
}

const DWidgetObjectData *dWidgetGetObjectDataConst(const DWidget *widget, DWidgetType subType) {
	assert(widget!=NULL);

	const DWidgetObjectData *data;
	for(data=widget->base; data!=NULL; data=data->super)
		if (data->type==subType)
			break;
	return data;
}

const DWidgetObjectData *dWidgetGetObjectDataConstNoFail(const DWidget *widget, DWidgetType subType) {
	assert(widget!=NULL);

	const DWidgetObjectData *data=dWidgetGetObjectDataConst(widget, subType);

	if (data==NULL || data->type!=subType) {
		fatalError("error: widget %p does not derive from %s as expected\n", widget, dWidgetTypeToString(subType));
		return NULL;
	}

	return data;
}

DWidgetObjectData *dWidgetObjectDataNew(DWidgetType type) {
	// Allocate memory and set basic fields
	DWidgetObjectData *data=mallocNoFail(sizeof(DWidgetObjectData));

	data->type=type;
	data->super=NULL;

	// Type specific logic
	switch(data->type) {
		case DWidgetTypeBin:
			data->super=dWidgetObjectDataNew(DWidgetTypeWidget);

			data->d.bin.child=NULL;
		break;
		case DWidgetTypeButton:
			data->super=dWidgetObjectDataNew(DWidgetTypeBin);
		break;
		case DWidgetTypeLabel:
			data->super=dWidgetObjectDataNew(DWidgetTypeWidget);

			data->d.label.text=malloc(1);
			data->d.label.text[0]='\0';
		break;
		case DWidgetTypeTextButton:
			data->super=dWidgetObjectDataNew(DWidgetTypeBin);
		break;
		case DWidgetTypeWindow:
			data->super=dWidgetObjectDataNew(DWidgetTypeBin);

			data->d.window.sdlWindow=NULL;
		break;
		case DWidgetTypeWidget:
			// This is the only type which does not inherit from any other
		break;
		case DWidgetTypeNB:
			assert(false);
		break;
	}

	return data;
}

void dWidgetObjectDataFree(DWidgetObjectData *data) {
	// NULL check
	if (data==NULL)
		return;

	// Type-specific logic
	switch(data->type) {
		case DWidgetTypeBin:
			// TODO: consider data->d.bin.child - should be removed before this point anyway?
		break;
		case DWidgetTypeButton:
		break;
		case DWidgetTypeLabel:
			free(data->d.label.text);
		break;
		case DWidgetTypeTextButton:
		break;
		case DWidgetTypeWindow:
			if (data->d.window.sdlWindow!=NULL)
				SDL_DestroyWindow(data->d.window.sdlWindow);
		break;
		case DWidgetTypeWidget:
		break;
		case DWidgetTypeNB:
			assert(false);
		break;
	}

	// Free super object (if any)
	dWidgetObjectDataFree(data->super);

	// Free object memory itself
	free(data);
}
