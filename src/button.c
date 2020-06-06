#include "bin.h"
#include "button.h"

DWidget *dButtonNew(DWidget *child) {
	// Create widget instance
	DWidget *button=dWidgetNew(DWidgetTypeButton);

	// Add child if given
	if (child!=NULL)
		dBinAdd(button, child);

	return button;
}
