#include <assert.h>

#include "bin.h"
#include "binprivate.h"
#include "button.h"
#include "buttonprivate.h"
#include "util.h"
#include "widgetprivate.h"

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

}

