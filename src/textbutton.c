#include <assert.h>
#include <stdlib.h>

#include "bin.h"
#include "buttonprivate.h"
#include "label.h"
#include "textbutton.h"
#include "textbuttonprivate.h"
#include "util.h"
#include "widgetprivate.h"

DWidget *dTextButtonGetLabel(DWidget *button); // never fails

DWidget *dTextButtonNew(const char *text) {
	assert(text!=NULL);

	// Create widget instance
	DWidget *button=dWidgetNew(DWidgetTypeTextButton);

	// Call constructor
	dTextButtonConstructor(button, button->base, text);

	return button;
}

void dTextButtonConstructor(DWidget *widget, DWidgetObjectData *data, const char *text) {
	assert(widget!=NULL);
	assert(data!=NULL);
	assert(data->type==DWidgetTypeTextButton);
	assert(text!=NULL);

	// Call super constructor first
	// Also might as well add label at the same time
	dButtonConstructor(widget, data->super, dLabelNew(text));
}

void dTextButtonSetText(DWidget *button, const char *text) {
	assert(button!=NULL);
	assert(text!=NULL);

	DWidget *label=dTextButtonGetLabel(button);
	dLabelSetText(label, text);
}

DWidget *dTextButtonGetLabel(DWidget *button) {
	assert(button!=NULL);

	DWidget *label=dBinGetChild(button);

	if (label==NULL) {
		dFatalError("error: TextButton %p has no child (expected Label)\n", button);
		return NULL;
	}

	return label;
}
