#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "binprivate.h"
#include "container.h"
#include "containerprivate.h"
#include "labelprivate.h"
#include "util.h"
#include "widget.h"
#include "widgetprivate.h"
#include "windowprivate.h"

// This is an array showing what widgets are derived from
// DWidgetTypeNB indicates does not extend anything, which should only be the case for DWidgetTypeWidget
static const DWidgetType dWidgetTypeExtends[DWidgetTypeNB]={
	[DWidgetTypeBin]=DWidgetTypeContainer,
	[DWidgetTypeButton]=DWidgetTypeBin,
	[DWidgetTypeContainer]=DWidgetTypeWidget,
	[DWidgetTypeLabel]=DWidgetTypeWidget,
	[DWidgetTypeTextButton]=DWidgetTypeButton,
	[DWidgetTypeWindow]=DWidgetTypeBin,
	[DWidgetTypeWidget]=DWidgetTypeNB,
};

int dWidgetVTableGetMinWidth(const DWidget *widget);
int dWidgetVTableGetMinHeight(const DWidget *widget);
int dWidgetVTableGetWidth(const DWidget *widget);
int dWidgetVTableGetHeight(const DWidget *widget);

DWidgetObjectData *dWidgetObjectDataNew(DWidgetType type);
void dWidgetObjectDataFree(DWidgetObjectData *data);

DWidget *dWidgetNew(DWidgetType type) {
	assert(dWidgetTypeIsValid(type));

	// Allocate widget memory and init fields
	DWidget *widget=dMallocNoFail(sizeof(DWidget));

	widget->base=NULL;
	widget->parent=NULL;
	memset(widget->signalsCount, 0, sizeof(widget->signalsCount[0])*DWidgetSignalTypeNB);

	// Initialise all sub classes - base one and any others it derives from
	widget->base=dWidgetObjectDataNew(type);

	return widget;
}

void dWidgetConstructor(DWidget *widget, DWidgetObjectData *data) {
	assert(widget!=NULL);
	assert(data!=NULL);
	assert(data->type==DWidgetTypeWidget);

	// Note: this class does not derive from any and so does not need to call another constructor
	assert(data->super==NULL);

	// Setup vtable
	data->vtable.getMinWidth=&dWidgetVTableGetMinWidth;
	data->vtable.getMinHeight=&dWidgetVTableGetMinHeight;
	data->vtable.getWidth=&dWidgetVTableGetWidth;
	data->vtable.getHeight=&dWidgetVTableGetHeight;
}

void dWidgetDestructor(DWidget *widget, DWidgetObjectData *data) {
	assert(widget!=NULL);
	// data can be NULL

	// Call first destructor we find (if any),
	// starting from the given sub class
	while(data!=NULL) {
		if (data->vtable.destructor!=NULL) {
			data->vtable.destructor(widget);
			break;
		}

		data=data->super;
	}
}

void dWidgetFree(DWidget *widget) {
	// NULL check
	if (widget==NULL)
		return;

	// Call first destructor we find (if any), starting with the base class
	dWidgetDestructor(widget, widget->base);

	// Free base object (and any others it derives from)
	dWidgetObjectDataFree(widget->base);

	// Free memory
	free(widget);
}

DWidget *dWidgetGetParent(DWidget *widget) {
	assert(widget!=NULL);

	return widget->parent;
}

const DWidget *dWidgetGetParentConst(const DWidget *widget) {
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

DWidget *dWidgetGetWidgetByXY(DWidget *widget, int globalX, int globalY) {
	assert(widget!=NULL);

	// Not even inside this widget?
	int widgetX=dWidgetGetGlobalX(widget);
	int widgetY=dWidgetGetGlobalY(widget);
	if (globalX<widgetX || globalX>=widgetX+dWidgetGetWidth(widget) || globalY<widgetY || globalY>=widgetY+dWidgetGetHeight(widget))
		return NULL;

	// If this widget is a container, check if pointing at a child
	if (dWidgetGetHasType(widget, DWidgetTypeContainer)) {
		size_t childCount=dContainerGetChildCount(widget);
		for(size_t i=0; i<childCount; ++i) {
			// Call ourselves recursively on this child to test for a hit
			DWidget *child=dContainerGetChildN(widget, i);
			DWidget *result=dWidgetGetWidgetByXY(child, globalX, globalY);
			if (result!=NULL)
				return result;
		}
	}

	// If no children, or does not point at any children, then simple return this widget itself.
	return widget;
}

int dWidgetGetMinWidth(const DWidget *widget) {
	assert(widget!=NULL);

	DWidgetObjectData *data;
	for(data=widget->base; data!=NULL; data=data->super)
		if (data->vtable.getMinWidth!=NULL)
			return data->vtable.getMinWidth(widget);

	dFatalError("error: widget %p (%s) has no getMinWidth vtable entry\n", widget, dWidgetTypeToString(dWidgetGetBaseType(widget)));
	return 0;
}

int dWidgetGetMinHeight(const DWidget *widget) {
	assert(widget!=NULL);

	DWidgetObjectData *data;
	for(data=widget->base; data!=NULL; data=data->super)
		if (data->vtable.getMinHeight!=NULL)
			return data->vtable.getMinHeight(widget);

	dFatalError("error: widget %p (%s) has no getMinHeight vtable entry\n", widget, dWidgetTypeToString(dWidgetGetBaseType(widget)));
	return 0;
}

int dWidgetGetWidth(const DWidget *widget) {
	assert(widget!=NULL);

	DWidgetObjectData *data;
	for(data=widget->base; data!=NULL; data=data->super)
		if (data->vtable.getWidth!=NULL)
			return data->vtable.getWidth(widget);

	dFatalError("error: widget %p (%s) has no getWidth vtable entry\n", widget, dWidgetTypeToString(dWidgetGetBaseType(widget)));
	return 0;
}

int dWidgetGetHeight(const DWidget *widget) {
	assert(widget!=NULL);

	DWidgetObjectData *data;
	for(data=widget->base; data!=NULL; data=data->super)
		if (data->vtable.getHeight!=NULL)
			return data->vtable.getHeight(widget);

	dFatalError("error: widget %p (%s) has no getHeight vtable entry\n", widget, dWidgetTypeToString(dWidgetGetBaseType(widget)));
	return 0;
}

int dWidgetGetGlobalX(const DWidget *widget) {
	assert(widget!=NULL);

	// If we have no parent, then global offset is 0
	// (this should only really apply to the root Window widget)
	const DWidget *parent=dWidgetGetParentConst(widget);
	if (parent==NULL)
		return 0;

	// Grab parents position and our offset within, and return sum
	int parentPos=dWidgetGetGlobalX(parent);
	int offset=dWidgetGetChildXOffset(parent, widget);
	return parentPos+offset;
}

int dWidgetGetGlobalY(const DWidget *widget) {
	assert(widget!=NULL);

	// If we have no parent, then global offset is 0
	// (this should only really apply to the root Window widget)
	const DWidget *parent=dWidgetGetParentConst(widget);
	if (parent==NULL)
		return 0;

	// Grab parents position and our offset within, and return sum
	int parentPos=dWidgetGetGlobalY(parent);
	int offset=dWidgetGetChildYOffset(parent, widget);
	return parentPos+offset;
}

int dWidgetGetChildXOffset(const DWidget *parent, const DWidget *child) {
	assert(parent!=NULL);
	assert(child!=NULL);
	assert(dWidgetGetParentConst(child)==parent);

	DWidgetObjectData *data;
	for(data=parent->base; data!=NULL; data=data->super)
		if (data->vtable.getChildXOffset!=NULL)
			return data->vtable.getChildXOffset(parent, child);

	dFatalError("error: widget %p (%s) has no getChildXOffset vtable entry\n", parent, dWidgetTypeToString(dWidgetGetBaseType(parent)));
	return 0;
}

int dWidgetGetChildYOffset(const DWidget *parent, const DWidget *child) {
	assert(parent!=NULL);
	assert(child!=NULL);
	assert(dWidgetGetParentConst(child)==parent);

	DWidgetObjectData *data;
	for(data=parent->base; data!=NULL; data=data->super)
		if (data->vtable.getChildYOffset!=NULL)
			return data->vtable.getChildYOffset(parent, child);

	dFatalError("error: widget %p (%s) has no getChildYOffset vtable entry\n", parent, dWidgetTypeToString(dWidgetGetBaseType(parent)));
	return 0;
}

bool dWidgetSignalConnect(DWidget *widget, DWidgetSignalType type, DWidgetSignalHandler *handler, void *userData) {
	assert(widget!=NULL);
	assert(dWidgetSignalTypeIsValid(type));
	assert(handler!=NULL);

	// Is this a valid type of signal for this widget?
	if (!dWidgetGetHasType(widget, dWidgetSignalTypeToWidgetType(type))) {
		dWarning("warning: can not add signal handler of type %s to widget %p - unsuitable widget type %s\n", dWidgetSignalTypeToString(type), widget, dWidgetTypeToString(dWidgetGetBaseType(widget)));
		return false;
	}

	// Have we reached the limit of handlers for this widget and sub class?
	if (widget->signalsCount[type]>=DWidgetSignalDataMax) {
		dWarning("warning: can not add signal handler of type %s to widget %p - max of %u already reached\n", dWidgetSignalTypeToString(type), widget, DWidgetSignalDataMax);
		return false;
	}

	// Add this handler to array of handlers
	widget->signals[type][widget->signalsCount[type]].handler=handler;
	widget->signals[type][widget->signalsCount[type]].userData=userData;
	widget->signalsCount[type]++;

	return true;
}

DWidgetSignalReturn dWidgetSignalInvoke(const DWidgetSignalEvent *event) {
	assert(event!=NULL);
	assert(dWidgetSignalTypeIsValid(event->type));
	assert(event->widget!=NULL);

	// Loop over registered handlers calling each one in turn (stopping early if any handlers request this)
	for(size_t i=0; i<event->widget->signalsCount[event->type]; ++i) {
		const DWidgetSignalData *signalData=&event->widget->signals[event->type][i];
		if (signalData->handler(event, signalData->userData)==DWidgetSignalReturnStop)
			return DWidgetSignalReturnStop;
	}
	return DWidgetSignalReturnContinue;
}

void dWidgetDebug(const DWidget *widget, int indentation) {
	assert(widget!=NULL);
	assert(indentation>=0);

	// Debug this widget itself
	for(int i=0; i<indentation; ++i)
		printf(" ");
	printf("%s %p", dWidgetTypeToString(dWidgetGetBaseType(widget)), widget);
	printf(" (x,y,w,h)=(%i,%i,%i,%i)", dWidgetGetGlobalX(widget), dWidgetGetGlobalY(widget), dWidgetGetWidth(widget), dWidgetGetHeight(widget));
	printf("\n");

	// If this is a container, recurse to handle children
	if (dWidgetGetHasType(widget, DWidgetTypeContainer)) {
		size_t childCount=dContainerGetChildCount(widget);
		for(size_t i=0; i<childCount; ++i)
			dWidgetDebug(dContainerGetChildNConst(widget, i), indentation+2);
	}
}

bool dWidgetTypeIsValid(DWidgetType type) {
	return (type>=0 && type<DWidgetTypeNB);
}

static const char *dWidgetTypeStrings[DWidgetTypeNB]={
	[DWidgetTypeBin]="Bin",
	[DWidgetTypeButton]="Button",
	[DWidgetTypeContainer]="Container",
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
	[DWidgetSignalTypeWidgetButtonPress]="WidgetButtonPress",
	[DWidgetSignalTypeWidgetButtonRelease]="WidgetButtonRelease",
	[DWidgetSignalTypeWidgetEnter]="WidgetEnter",
	[DWidgetSignalTypeWidgetLeave]="WidgetLeave",
	[DWidgetSignalTypeWindowClose]="WindowClose",
};
const char *dWidgetSignalTypeToString(DWidgetSignalType type) {
	assert(dWidgetSignalTypeIsValid(type));

	return dWidgetSignalTypeStrings[type];
}

DWidgetType dWidgetSignalTypeToWidgetType(DWidgetSignalType type) {
	assert(dWidgetSignalTypeIsValid(type));

	switch(type) {
		case DWidgetSignalTypeWidgetButtonPress:
		case DWidgetSignalTypeWidgetButtonRelease:
		case DWidgetSignalTypeWidgetEnter:
		case DWidgetSignalTypeWidgetLeave:
			return DWidgetTypeWidget;
		break;
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
		dFatalError("error: widget %p does not derive from %s as expected\n", widget, dWidgetTypeToString(subType));
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
		dFatalError("error: widget %p does not derive from %s as expected\n", widget, dWidgetTypeToString(subType));
		return NULL;
	}

	return data;
}

int dWidgetVTableGetMinWidth(const DWidget *widget) {
	assert(widget!=NULL);

	return 0;
}

int dWidgetVTableGetMinHeight(const DWidget *widget) {
	assert(widget!=NULL);

	return 0;
}

int dWidgetVTableGetWidth(const DWidget *widget) {
	assert(widget!=NULL);

	return 0;
}

int dWidgetVTableGetHeight(const DWidget *widget) {
	assert(widget!=NULL);

	return 0;
}

DWidgetObjectData *dWidgetObjectDataNew(DWidgetType type) {
	// Allocate memory and set basic fields
	DWidgetObjectData *data=dMallocNoFail(sizeof(DWidgetObjectData));

	data->type=type;
	data->super=NULL;

	data->vtable.destructor=NULL;
	data->vtable.getMinWidth=NULL;
	data->vtable.getMinHeight=NULL;
	data->vtable.getWidth=NULL;
	data->vtable.getHeight=NULL;
	data->vtable.getChildXOffset=NULL;
	data->vtable.getChildYOffset=NULL;

	// Create and init super class if needed
	// note: this recurses until we hit DWidgetTypeWidget
	DWidgetType superType=dWidgetTypeExtends[type];
	if (superType!=DWidgetTypeNB)
		data->super=dWidgetObjectDataNew(superType);

	return data;
}

void dWidgetObjectDataFree(DWidgetObjectData *data) {
	// NULL check
	if (data==NULL)
		return;

	// Free super object (if any)
	dWidgetObjectDataFree(data->super);

	// Free object memory itself
	free(data);
}
