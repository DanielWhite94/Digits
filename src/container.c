#include <assert.h>

#include "container.h"
#include "containerprivate.h"
#include "util.h"
#include "widgetprivate.h"

void dContainerVTableDestructor(DWidget *widget);
void dContainerVTableRedraw(DWidget *widget, SDL_Renderer *renderer);
int dContainerVTableGetMinWidth(const DWidget *widget);
int dContainerVTableGetMinHeight(const DWidget *widget);

void dContainerConstructor(DWidget *widget, DWidgetObjectData *data) {
	assert(widget!=NULL);
	assert(data!=NULL);
	assert(data->type==DWidgetTypeContainer);

	// Call super constructor first
	dWidgetConstructor(widget, data->super);

	// Init fields
	data->d.container.children=NULL;
	data->d.container.childCount=0;

	// Setup vtable
	data->vtable.destructor=&dContainerVTableDestructor;
	data->vtable.redraw=&dContainerVTableRedraw;
	data->vtable.getMinWidth=&dContainerVTableGetMinWidth;
	data->vtable.getMinHeight=&dContainerVTableGetMinHeight;
}

bool dContainerAdd(DWidget *container, DWidget *child) {
	assert(container!=NULL);
	assert(child!=NULL);

	// Check child does not already have a parent
	if (dWidgetGetParent(child)!=NULL)
		return false;

	// Add child to container
	DWidgetObjectData *data=dWidgetGetObjectDataNoFail(container, DWidgetTypeContainer);

	data->d.container.children=dReallocNoFail(data->d.container.children, sizeof(DWidget *)*(data->d.container.childCount+1));
	data->d.container.children[data->d.container.childCount++]=child;

	// Set child's parent to container
	child->parent=container;

	// Mark window as dirty to redraw
	dWidgetSetDirty(container);

	return true;
}

DWidget *dContainerGetChildN(DWidget *container, size_t n) {
	assert(container!=NULL);

	DWidgetObjectData *data=dWidgetGetObjectDataNoFail(container, DWidgetTypeContainer);

	if (n>=data->d.container.childCount)
		return NULL;

	return data->d.container.children[n];
}

const DWidget *dContainerGetChildNConst(const DWidget *container, size_t n) {
	assert(container!=NULL);

	const DWidgetObjectData *data=dWidgetGetObjectDataConstNoFail(container, DWidgetTypeContainer);

	if (n>=data->d.container.childCount)
		return NULL;

	return data->d.container.children[n];
}

size_t dContainerGetChildCount(const DWidget *container) {
	assert(container!=NULL);

	const DWidgetObjectData *data=dWidgetGetObjectDataConstNoFail(container, DWidgetTypeContainer);

	return data->d.container.childCount;
}

void dContainerVTableDestructor(DWidget *widget) {
	assert(widget!=NULL);

	DWidgetObjectData *data=dWidgetGetObjectDataNoFail(widget, DWidgetTypeContainer);

	// Free memory
	free(data->d.container.children);

	// Call super destructor
	dWidgetDestructor(widget, data->super);
}

void dContainerVTableRedraw(DWidget *widget, SDL_Renderer *renderer) {
	assert(widget!=NULL);
	assert(renderer!=NULL);

	DWidgetObjectData *data=dWidgetGetObjectDataNoFail(widget, DWidgetTypeContainer);

	// Call super redraw
	dWidgetRedraw(widget, data->super, renderer);

	// Loop to draw children
	for(size_t i=0; i<data->d.container.childCount; ++i) {
		DWidget *child=data->d.container.children[i];
		dWidgetRedraw(child, child->base, renderer);
	}
}

int dContainerVTableGetMinWidth(const DWidget *widget) {
	assert(widget!=NULL);

	int sum=0;

	// Sum child min widths
	size_t childCount=dContainerGetChildCount(widget);
	for(size_t i=0; i<childCount; ++i) {
		const DWidget *child=dContainerGetChildNConst(widget, i);
		sum+=dWidgetGetMinWidth(child);
	}

	return sum;
}

int dContainerVTableGetMinHeight(const DWidget *widget) {
	assert(widget!=NULL);

	int sum=0;

	// Sum child min heights
	size_t childCount=dContainerGetChildCount(widget);
	for(size_t i=0; i<childCount; ++i) {
		const DWidget *child=dContainerGetChildNConst(widget, i);
		sum+=dWidgetGetMinHeight(child);
	}

	return sum;
}
