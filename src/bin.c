#include <assert.h>

#include "bin.h"

bool dBinAdd(DWidget *bin, DWidget *child) {
	assert(bin!=NULL);
	assert(child!=NULL);

	// Check child does not already have a parent
	if (dWidgetGetParent(child)!=NULL)
		return false;

	// Check bin does not already have a child
	if (dBinGetChild(bin)!=NULL)
		return false;

	// Add child to bin
	DWidgetObjectData *data=dWidgetGetObjectDataNoFail(bin, DWidgetTypeBin);
	data->d.bin.child=child;

	// Set child's parent to bin
	child->parent=bin;

	return true;
}

DWidget *dBinGetChild(DWidget *bin) {
	assert(bin!=NULL);

	DWidgetObjectData *data=dWidgetGetObjectDataNoFail(bin, DWidgetTypeBin);

	return data->d.bin.child;
}
