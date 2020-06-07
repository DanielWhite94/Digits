#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "label.h"
#include "labelprivate.h"
#include "util.h"

// TODO: improve this - it is just a hacky way to get width/height functions working without worrying about height-for-width logic
const int dLabelFontWidth=20;
const int dLabelFontHeight=10;

DWidget *dLabelNew(const char *text) {
	assert(text!=NULL);

	// Create widget instance
	DWidget *label=dWidgetNew(DWidgetTypeLabel);

	// Set text
	dLabelSetText(label, text);

	return label;
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
	data->d.label.text=reallocNoFail(data->d.label.text, newSize);
	memcpy(data->d.label.text, text, newSize);
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
