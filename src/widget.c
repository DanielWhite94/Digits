#include <assert.h>
#include <stdlib.h>

#include "util.h"
#include "widget.h"

DWidgetObjectData *dWidgetObjectDataNew(DWidgetType type);
void dWidgetObjectDataFree(DWidgetObjectData *data);

DWidget *dWidgetNew(DWidgetType type) {
	// Allocate widget memory and init fields
	DWidget *widget=mallocNoFail(sizeof(DWidget));

	widget->base=NULL;
	widget->parent=NULL;

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

static const char *dWidgetTypeStrings[]={
	[DWidgetTypeBin]="Bin",
	[DWidgetTypeLabel]="Label",
	[DWidgetTypeWindow]="Window",
};
const char *dWidgetTypeToString(DWidgetType type) {
	return dWidgetTypeStrings[type];
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
		fatalError("dWidgetGetObjectDataNoFail no data or bad type\n");
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
			data->d.bin.child=NULL;
		break;
		case DWidgetTypeLabel:
			data->d.label.text=malloc(1);
			data->d.label.text[0]='\0';
		break;
		case DWidgetTypeWindow:
			data->super=dWidgetObjectDataNew(DWidgetTypeBin);

			data->d.window.sdlWindow=NULL;
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
		case DWidgetTypeLabel:
			free(data->d.label.text);
		break;
		case DWidgetTypeWindow:
			if (data->d.window.sdlWindow!=NULL)
				SDL_DestroyWindow(data->d.window.sdlWindow);
		break;
	}

	// Free super object (if any)
	dWidgetObjectDataFree(data->super);

	// Free object memory itself
	free(data);
}
