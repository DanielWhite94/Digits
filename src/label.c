#include <assert.h>
#include <stdlib.h>

#include "label.h"
#include "util.h"

DWidget *dLabelNew(const char *text) {
	assert(text!=NULL);

	// Create widget instance
	DWidget *label=dWidgetNew(DWidgetTypeLabel);

	// Set text
	dLabelSetText(label, text);

	return label;
}

void dLabelSetText(DWidget *label, const char *text) {
	assert(label!=NULL);
	assert(text!=NULL);

	DWidgetObjectData *data=dWidgetGetObjectDataNoFail(label, DWidgetTypeLabel);

	size_t newSize=strlen(text)+1;
	data->d.label.text=reallocNoFail(data->d.label.text, newSize);
	memcpy(data->d.label.text, text, newSize);
}
