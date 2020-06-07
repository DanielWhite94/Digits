#include <assert.h>

#include "bin.h"
#include "binprivate.h"
#include "container.h"

bool dBinAdd(DWidget *bin, DWidget *child) {
	assert(bin!=NULL);
	assert(child!=NULL);

	// Already have a child?
	if (dContainerGetChildCount(bin)>0)
		return false;

	// Otherwise attempt to add child to container
	return dContainerAdd(bin, child);
}

DWidget *dBinGetChild(DWidget *bin) {
	assert(bin!=NULL);

	// Simply return first child from container (if exists)
	return dContainerGetChildN(bin, 0);
}

const DWidget *dBinGetChildConst(const DWidget *bin) {
	assert(bin!=NULL);

	// Simply return first child from container (if exists)
	return dContainerGetChildNConst(bin, 0);
}

int dBinVTableGetWidth(const DWidget *widget) {
	assert(widget!=NULL);

	// Simply use childs width (or 0 if empty)
	const DWidget *child=dBinGetChildConst(widget);
	return (child!=NULL ? dWidgetGetWidth(child) : 0);
}

int dBinVTableGetHeight(const DWidget *widget) {
	assert(widget!=NULL);

	// Simply use childs height (or 0 if empty)
	const DWidget *child=dBinGetChildConst(widget);
	return (child!=NULL ? dWidgetGetHeight(child) : 0);
}

int dBinVTableGetChildXOffset(const DWidget *parent, const DWidget *child) {
	assert(parent!=NULL);
	assert(child!=NULL);
	assert(dWidgetGetParentConst(child)==parent);

	// Single child so never any offset
	return 0;
}

int dBinVTableGetChildYOffset(const DWidget *parent, const DWidget *child) {
	assert(parent!=NULL);
	assert(child!=NULL);
	assert(dWidgetGetParentConst(child)==parent);

	// Single child so never any offset
	return 0;
}
