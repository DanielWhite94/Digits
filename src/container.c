#include <assert.h>

#include "container.h"
#include "util.h"

bool dContainerAdd(DWidget *container, DWidget *child) {
	assert(container!=NULL);
	assert(child!=NULL);

	// Check child does not already have a parent
	if (dWidgetGetParent(child)!=NULL)
		return false;

	// Add child to container
	DWidgetObjectData *data=dWidgetGetObjectDataNoFail(container, DWidgetTypeContainer);

	data->d.container.children=reallocNoFail(data->d.container.children, sizeof(DWidget *)*(data->d.container.childCount+1));
	data->d.container.children[data->d.container.childCount++]=child;

	// Set child's parent to container
	child->parent=container;

	return true;
}

DWidget *dContainerGetChildN(DWidget *container, size_t n) {
	assert(container!=NULL);

	DWidgetObjectData *data=dWidgetGetObjectDataNoFail(container, DWidgetTypeContainer);

	if (n>=data->d.container.childCount)
		return NULL;

	return data->d.container.children[n];
}

size_t dContainerGetChildCount(const DWidget *container) {
	assert(container!=NULL);

	const DWidgetObjectData *data=dWidgetGetObjectDataConstNoFail(container, DWidgetTypeContainer);

	return data->d.container.childCount;
}
