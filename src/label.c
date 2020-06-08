#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "label.h"
#include "labelprivate.h"
#include "util.h"
#include "widgetprivate.h"

// TODO: improve this - it is just a hacky way to get width/height functions working without worrying about height-for-width logic
const int dLabelFontWidth=20;
const int dLabelFontHeight=10;

void dLabelVTableDestructor(DWidget *widget);
int dLabelVTableGetMinWidth(const DWidget *widget);
int dLabelVTableGetMinHeight(const DWidget *widget);
int dLabelVTableGetWidth(const DWidget *widget);
int dLabelVTableGetHeight(const DWidget *widget);

DWidget *dLabelNew(const char *text) {
	assert(text!=NULL);

	// Create widget instance
	DWidget *label=dWidgetNew(DWidgetTypeLabel);

	// Call constructor
	dLabelConstructor(label, label->base, text);

	return label;
}

void dLabelConstructor(DWidget *widget, DWidgetObjectData *data, const char *text) {
	assert(widget!=NULL);
	assert(data!=NULL);
	assert(data->type==DWidgetTypeLabel);
	assert(text!=NULL);

	// Call super constructor first
	dWidgetConstructor(widget, data->super);

	// Init fields
	data->d.label.text=dMallocNoFail(1);
	data->d.label.text[0]='\0';

	// Setup vtable
	data->vtable.destructor=&dLabelVTableDestructor;
	data->vtable.getMinWidth=&dLabelVTableGetMinWidth;
	data->vtable.getMinHeight=&dLabelVTableGetMinHeight;
	data->vtable.getWidth=&dLabelVTableGetWidth;
	data->vtable.getHeight=&dLabelVTableGetHeight;

	// Set text
	dLabelSetText(widget, text);
}

const char *dLabelGetText(const DWidget *label) {
	assert(label!=NULL);

	const DWidgetObjectData *data=dWidgetGetObjectDataConstNoFail(label, DWidgetTypeLabel);

	return data->d.label.text;
}

void dLabelSetText(DWidget *label, const char *text) {
	assert(label!=NULL);
	assert(text!=NULL);

	DWidgetObjectData *data=dWidgetGetObjectDataNoFail(label, DWidgetTypeLabel);

	size_t newSize=strlen(text)+1;
	data->d.label.text=dReallocNoFail(data->d.label.text, newSize);
	memcpy(data->d.label.text, text, newSize);
}

void dLabelVTableDestructor(DWidget *widget) {
	assert(widget!=NULL);

	DWidgetObjectData *data=dWidgetGetObjectDataNoFail(widget, DWidgetTypeLabel);

	// Free memory
	free(data->d.label.text);

	// Call super destructor
	dWidgetDestructor(widget, data->super);
}

int dLabelVTableGetMinWidth(const DWidget *widget) {
	assert(widget!=NULL);

	// Minimum is one character per line (using as many lines as needed)
	return dLabelFontWidth;
}

int dLabelVTableGetMinHeight(const DWidget *widget) {
	assert(widget!=NULL);

	// Minimum is a single line
	return dLabelFontHeight;
}

int dLabelVTableGetWidth(const DWidget *widget) {
	assert(widget!=NULL);

	// For now assume we have a single line
	return dLabelFontWidth*strlen(dLabelGetText(widget));
}

int dLabelVTableGetHeight(const DWidget *widget) {
	assert(widget!=NULL);

	// For now assume we have a single line
	return dLabelFontHeight;
}
