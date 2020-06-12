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
	[DWidgetTypeBox]=DWidgetTypeContainer,
	[DWidgetTypeButton]=DWidgetTypeBin,
	[DWidgetTypeContainer]=DWidgetTypeWidget,
	[DWidgetTypeLabel]=DWidgetTypeWidget,
	[DWidgetTypeTextButton]=DWidgetTypeButton,
	[DWidgetTypeWindow]=DWidgetTypeBin,
	[DWidgetTypeWidget]=DWidgetTypeNB,
};

int dWidgetVTableGetMinWidth(DWidget *widget);
int dWidgetVTableGetMinHeight(DWidget *widget);
int dWidgetVTableGetWidth(DWidget *widget);
int dWidgetVTableGetHeight(DWidget *widget);

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

	// Init fields
	data->d.widget.paddingTop=0;
	data->d.widget.paddingBottom=0;
	data->d.widget.paddingLeft=0;
	data->d.widget.paddingRight=0;
	data->d.widget.orientation=DWidgetOrientationHorizontal;
	data->d.widget.hexpand=false;
	data->d.widget.vexpand=false;

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

SDL_Renderer *dWidgetGetRenderer(DWidget *widget) {
	assert(widget!=NULL);

	DWidget *window=dWidgetGetWindow(widget);
	if (window==NULL)
		return NULL;

	return dWindowGetRenderer(window);
}

void dWidgetSetDirty(DWidget *widget) {
	assert(widget!=NULL);

	DWidget *window=dWidgetGetWindow(widget);
	if (window==NULL)
		return;

	dWindowSetDirty(window);
}

void dWidgetRedraw(DWidget *widget, DWidgetObjectData *data, SDL_Renderer *renderer) {
	assert(widget!=NULL);
	// data can be NULL
	assert(renderer!=NULL);

	// Call first redraw functor we find (if any),
	// starting from the given sub class
	while(data!=NULL) {
		if (data->vtable.redraw!=NULL) {
			data->vtable.redraw(widget, renderer);
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

bool dWidgetIsAncestor(const DWidget *widget, const DWidget *child) {
	assert(widget!=NULL);
	assert(child!=NULL);

	const DWidget *childParent=dWidgetGetParentConst(child);

	// Does child simply have no ancestors?
	if (childParent==NULL)
		return false;

	// Is widget simply the direct parent of child?
	if (childParent==widget)
		return true;

	// Otherwise work up through parents recursively
	return dWidgetIsAncestor(widget, childParent);
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

DWidget *dWidgetGetWindow(DWidget *widget) {
	assert(widget!=NULL);

	// Is this widget a window?
	if (dWidgetGetHasType(widget, DWidgetTypeWindow))
		return widget;

	// Ask parent widget recursively
	if (dWidgetGetParent(widget)!=NULL)
		return dWidgetGetWindow(dWidgetGetParent(widget));

	return NULL;
}

const DWidget *dWidgetGetWindowConst(const DWidget *widget) {
	assert(widget!=NULL);

	// Is this widget a window?
	if (dWidgetGetHasType(widget, DWidgetTypeWindow))
		return widget;

	// Ask parent widget recursively
	if (dWidgetGetParentConst(widget)!=NULL)
		return dWidgetGetWindowConst(dWidgetGetParentConst(widget));

	return NULL;
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

int dWidgetGetMinWidth(DWidget *widget) {
	assert(widget!=NULL);

	DWidgetObjectData *data;
	for(data=widget->base; data!=NULL; data=data->super)
		if (data->vtable.getMinWidth!=NULL)
			return data->vtable.getMinWidth(widget);

	dFatalError("error: widget %p (%s) has no getMinWidth vtable entry\n", widget, dWidgetTypeToString(dWidgetGetBaseType(widget)));
	return 0;
}

int dWidgetGetMinHeight(DWidget *widget) {
	assert(widget!=NULL);

	DWidgetObjectData *data;
	for(data=widget->base; data!=NULL; data=data->super)
		if (data->vtable.getMinHeight!=NULL)
			return data->vtable.getMinHeight(widget);

	dFatalError("error: widget %p (%s) has no getMinHeight vtable entry\n", widget, dWidgetTypeToString(dWidgetGetBaseType(widget)));
	return 0;
}

int dWidgetGetWidth(DWidget *widget) {
	assert(widget!=NULL);

	DWidgetObjectData *data;
	for(data=widget->base; data!=NULL; data=data->super)
		if (data->vtable.getWidth!=NULL)
			return data->vtable.getWidth(widget);

	dFatalError("error: widget %p (%s) has no getWidth vtable entry\n", widget, dWidgetTypeToString(dWidgetGetBaseType(widget)));
	return 0;
}

int dWidgetGetHeight(DWidget *widget) {
	assert(widget!=NULL);

	DWidgetObjectData *data;
	for(data=widget->base; data!=NULL; data=data->super)
		if (data->vtable.getHeight!=NULL)
			return data->vtable.getHeight(widget);

	dFatalError("error: widget %p (%s) has no getHeight vtable entry\n", widget, dWidgetTypeToString(dWidgetGetBaseType(widget)));
	return 0;
}

int dWidgetGetGlobalX(DWidget *widget) {
	assert(widget!=NULL);

	// If we have no parent, then global offset is 0
	// (this should only really apply to the root Window widget)
	DWidget *parent=dWidgetGetParent(widget);
	if (parent==NULL)
		return 0;

	// Grab parents position and our offset within, and return sum
	int parentPos=dWidgetGetGlobalX(parent);
	int offset=dWidgetGetChildXOffset(parent, widget);
	return parentPos+offset;
}

int dWidgetGetGlobalY(DWidget *widget) {
	assert(widget!=NULL);

	// If we have no parent, then global offset is 0
	// (this should only really apply to the root Window widget)
	DWidget *parent=dWidgetGetParent(widget);
	if (parent==NULL)
		return 0;

	// Grab parents position and our offset within, and return sum
	int parentPos=dWidgetGetGlobalY(parent);
	int offset=dWidgetGetChildYOffset(parent, widget);
	return parentPos+offset;
}

int dWidgetGetChildXOffset(DWidget *parent, DWidget *child) {
	assert(parent!=NULL);
	assert(child!=NULL);
	assert(dWidgetGetParent(child)==parent);

	DWidgetObjectData *data;
	for(data=parent->base; data!=NULL; data=data->super)
		if (data->vtable.getChildXOffset!=NULL)
			return data->vtable.getChildXOffset(parent, child);

	dFatalError("error: widget %p (%s) has no getChildXOffset vtable entry\n", parent, dWidgetTypeToString(dWidgetGetBaseType(parent)));
	return 0;
}

int dWidgetGetChildYOffset(DWidget *parent, DWidget *child) {
	assert(parent!=NULL);
	assert(child!=NULL);
	assert(dWidgetGetParent(child)==parent);

	DWidgetObjectData *data;
	for(data=parent->base; data!=NULL; data=data->super)
		if (data->vtable.getChildYOffset!=NULL)
			return data->vtable.getChildYOffset(parent, child);

	dFatalError("error: widget %p (%s) has no getChildYOffset vtable entry\n", parent, dWidgetTypeToString(dWidgetGetBaseType(parent)));
	return 0;
}

int dWidgetGetPaddingTop(const DWidget *widget) {
	assert(widget!=NULL);

	const DWidgetObjectData *data=dWidgetGetObjectDataConstNoFail(widget, DWidgetTypeWidget);

	return data->d.widget.paddingTop;
}

int dWidgetGetPaddingBottom(const DWidget *widget) {
	assert(widget!=NULL);

	const DWidgetObjectData *data=dWidgetGetObjectDataConstNoFail(widget, DWidgetTypeWidget);

	return data->d.widget.paddingBottom;
}

int dWidgetGetPaddingLeft(const DWidget *widget) {
	assert(widget!=NULL);

	const DWidgetObjectData *data=dWidgetGetObjectDataConstNoFail(widget, DWidgetTypeWidget);

	return data->d.widget.paddingLeft;
}

int dWidgetGetPaddingRight(const DWidget *widget) {
	assert(widget!=NULL);

	const DWidgetObjectData *data=dWidgetGetObjectDataConstNoFail(widget, DWidgetTypeWidget);

	return data->d.widget.paddingRight;
}

int dWidgetGetOrientation(const DWidget *widget) {
	assert(widget!=NULL);

	const DWidgetObjectData *data=dWidgetGetObjectDataConstNoFail(widget, DWidgetTypeWidget);

	return data->d.widget.orientation;
}

int dWidgetGetHExpand(const DWidget *widget) {
	assert(widget!=NULL);

	const DWidgetObjectData *data=dWidgetGetObjectDataConstNoFail(widget, DWidgetTypeWidget);

	return data->d.widget.hexpand;
}

int dWidgetGetVExpand(const DWidget *widget) {
	assert(widget!=NULL);

	const DWidgetObjectData *data=dWidgetGetObjectDataConstNoFail(widget, DWidgetTypeWidget);

	return data->d.widget.vexpand;
}

void dWidgetSetPadding(DWidget *widget, int padding) {
	assert(widget!=NULL);
	assert(padding>=0);

	dWidgetSetPaddingTop(widget, padding);
	dWidgetSetPaddingBottom(widget, padding);
	dWidgetSetPaddingLeft(widget, padding);
	dWidgetSetPaddingRight(widget, padding);
}

void dWidgetSetPaddingTop(DWidget *widget, int padding) {
	assert(widget!=NULL);
	assert(padding>=0);

	DWidgetObjectData *data=dWidgetGetObjectDataNoFail(widget, DWidgetTypeWidget);

	// Update padding field
	data->d.widget.paddingTop=padding;

	// Mark window as dirty to redraw
	dWidgetSetDirty(widget);
}

void dWidgetSetPaddingBottom(DWidget *widget, int padding) {
	assert(widget!=NULL);
	assert(padding>=0);

	DWidgetObjectData *data=dWidgetGetObjectDataNoFail(widget, DWidgetTypeWidget);

	// Update padding field
	data->d.widget.paddingBottom=padding;

	// Mark window as dirty to redraw
	dWidgetSetDirty(widget);
}

void dWidgetSetPaddingLeft(DWidget *widget, int padding) {
	assert(widget!=NULL);
	assert(padding>=0);

	DWidgetObjectData *data=dWidgetGetObjectDataNoFail(widget, DWidgetTypeWidget);

	// Update padding field
	data->d.widget.paddingLeft=padding;

	// Mark window as dirty to redraw
	dWidgetSetDirty(widget);
}

void dWidgetSetPaddingRight(DWidget *widget, int padding) {
	assert(widget!=NULL);
	assert(padding>=0);

	DWidgetObjectData *data=dWidgetGetObjectDataNoFail(widget, DWidgetTypeWidget);

	// Update padding field
	data->d.widget.paddingRight=padding;

	// Mark window as dirty to redraw
	dWidgetSetDirty(widget);
}

void dWidgetSetOrientation(DWidget *widget, DWidgetOrientation orientation) {
	assert(widget!=NULL);
	assert(dWidgetOrientationIsValid(orientation));

	DWidgetObjectData *data=dWidgetGetObjectDataNoFail(widget, DWidgetTypeWidget);

	// Update field
	data->d.widget.orientation=orientation;

	// Mark window as dirty to redraw
	dWidgetSetDirty(widget);
}

void dWidgetSetHExpand(DWidget *widget, bool hexpand) {
	assert(widget!=NULL);

	DWidgetObjectData *data=dWidgetGetObjectDataNoFail(widget, DWidgetTypeWidget);

	// Update field
	data->d.widget.hexpand=hexpand;

	// Mark window as dirty to redraw
	dWidgetSetDirty(widget);
}

void dWidgetSetVExpand(DWidget *widget, bool vexpand) {
	assert(widget!=NULL);

	DWidgetObjectData *data=dWidgetGetObjectDataNoFail(widget, DWidgetTypeWidget);

	// Update field
	data->d.widget.vexpand=vexpand;

	// Mark window as dirty to redraw
	dWidgetSetDirty(widget);
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

void dWidgetDebug(DWidget *widget, int indentation) {
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
			dWidgetDebug(dContainerGetChildN(widget, i), indentation+2);
	}
}

bool dWidgetOrientationIsValid(DWidgetOrientation orientation) {
	return (orientation==DWidgetOrientationHorizontal || orientation==DWidgetOrientationVertical);
}

bool dWidgetTypeIsValid(DWidgetType type) {
	return (type>=0 && type<DWidgetTypeNB);
}

static const char *dWidgetTypeStrings[DWidgetTypeNB]={
	[DWidgetTypeBin]="Bin",
	[DWidgetTypeBox]="Box",
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
	[DWidgetSignalTypeButtonClick]="ButtonClick",
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
		case DWidgetSignalTypeButtonClick:
			return DWidgetTypeButton;
		break;
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

int dWidgetVTableGetMinWidth(DWidget *widget) {
	assert(widget!=NULL);

	return dWidgetGetPaddingLeft(widget)+dWidgetGetPaddingRight(widget);
}

int dWidgetVTableGetMinHeight(DWidget *widget) {
	assert(widget!=NULL);

	return dWidgetGetPaddingTop(widget)+dWidgetGetPaddingBottom(widget);
}

int dWidgetVTableGetWidth(DWidget *widget) {
	assert(widget!=NULL);

	return dWidgetGetPaddingLeft(widget)+dWidgetGetPaddingRight(widget);
}

int dWidgetVTableGetHeight(DWidget *widget) {
	assert(widget!=NULL);

	return dWidgetGetPaddingTop(widget)+dWidgetGetPaddingBottom(widget);
}

DWidgetObjectData *dWidgetObjectDataNew(DWidgetType type) {
	// Allocate memory and set basic fields
	DWidgetObjectData *data=dMallocNoFail(sizeof(DWidgetObjectData));

	data->type=type;
	data->super=NULL;

	data->vtable.destructor=NULL;
	data->vtable.redraw=NULL;
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
