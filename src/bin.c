#include <assert.h>

#include "bin.h"
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
