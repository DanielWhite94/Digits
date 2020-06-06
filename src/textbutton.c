#include <assert.h>
#include <stdlib.h>

#include "bin.h"
#include "label.h"
#include "textbutton.h"
#include "util.h"

DWidget *dTextButtonGetLabel(DWidget *button); // never fails

DWidget *dTextButtonNew(const char *text) {
	assert(text!=NULL);

	// Create widget instance
	DWidget *button=dWidgetNew(DWidgetTypeTextButton);

	// Add a label with given text
	dBinAdd(button, dLabelNew(text));

	return button;
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
		fatalError("error: TextButton %p has no child (expected Label)\n", button);
		return NULL;
	}

	return label;
}
