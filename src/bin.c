#include <assert.h>
#include <stdlib.h>

#include "bin.h"
#include "binprivate.h"
#include "container.h"
#include "containerprivate.h"

int dBinVTableGetMinWidth(DWidget *widget);
int dBinVTableGetMinHeight(DWidget *widget);
int dBinVTableGetWidth(DWidget *widget);
int dBinVTableGetHeight(DWidget *widget);
int dBinVTableGetChildXOffset(DWidget *parent, DWidget *child);
int dBinVTableGetChildYOffset(DWidget *parent, DWidget *child);

DWidget *dBinNew(DWidget *child) {
	// Create widget instance
	DWidget *bin=dWidgetNew(DWidgetTypeBin);

	// Call constructor
	dBinConstructor(bin, bin->base, child);

	return bin;
}

bool dBinAdd(DWidget *bin, DWidget *child) {
	assert(bin!=NULL);
	assert(child!=NULL);

	// Already have a child?
	if (dContainerGetChildCount(bin)>0)
		return false;

	// Otherwise attempt to add child to container
	return dContainerAdd(bin, child);
}

void dBinConstructor(DWidget *widget, DWidgetObjectData *data, DWidget *child) {
	assert(widget!=NULL);
	assert(data!=NULL);
	assert(data->type==DWidgetTypeBin);

	// Call super constructor first
	dContainerConstructor(widget, data->super);

	// Setup vtable
	data->vtable.getMinWidth=&dBinVTableGetMinWidth;
	data->vtable.getMinHeight=&dBinVTableGetMinHeight;
	data->vtable.getWidth=&dBinVTableGetWidth;
	data->vtable.getHeight=&dBinVTableGetHeight;
	data->vtable.getChildXOffset=&dBinVTableGetChildXOffset;
	data->vtable.getChildYOffset=&dBinVTableGetChildYOffset;

	// Add child if given
	if (child!=NULL)
		dBinAdd(widget, child);
}
// TODO: consider if we need a destructor to remove data->d.bin.child

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

int dBinVTableGetMinWidth(DWidget *widget) {
	assert(widget!=NULL);

	// Simply use child's min width (or 0 if empty)
	DWidget *child=dBinGetChild(widget);
	int width=(child!=NULL ? dWidgetGetMinWidth(child) : 0);

	// Add padding
	width+=dWidgetGetPaddingLeft(widget)+dWidgetGetPaddingRight(widget);

	return width;
}

int dBinVTableGetMinHeight(DWidget *widget) {
	assert(widget!=NULL);

	// Simply use child's min height (or 0 if empty)
	DWidget *child=dBinGetChild(widget);
	int height=(child!=NULL ? dWidgetGetMinHeight(child) : 0);

	// Add padding
	height+=dWidgetGetPaddingTop(widget)+dWidgetGetPaddingBottom(widget);

	return height;
}

int dBinVTableGetWidth(DWidget *widget) {
	assert(widget!=NULL);

	// Simply use childs width (or 0 if empty)
	DWidget *child=dBinGetChild(widget);
	int width=(child!=NULL ? dWidgetGetWidth(child) : 0);

	// Add padding
	width+=dWidgetGetPaddingLeft(widget)+dWidgetGetPaddingRight(widget);

	return width;
}

int dBinVTableGetHeight(DWidget *widget) {
	assert(widget!=NULL);

	// Simply use childs height (or 0 if empty)
	DWidget *child=dBinGetChild(widget);
	int height=(child!=NULL ? dWidgetGetHeight(child) : 0);

	// Add padding
	height+=dWidgetGetPaddingTop(widget)+dWidgetGetPaddingBottom(widget);

	return height;
}

int dBinVTableGetChildXOffset(DWidget *parent, DWidget *child) {
	assert(parent!=NULL);
	assert(child!=NULL);
	assert(dWidgetGetParentConst(child)==parent);

	// Single child so offset is simply our left padding
	return dWidgetGetPaddingLeft(parent);
}

int dBinVTableGetChildYOffset(DWidget *parent, DWidget *child) {
	assert(parent!=NULL);
	assert(child!=NULL);
	assert(dWidgetGetParentConst(child)==parent);

	// Single child so offset is simply our top padding
	return dWidgetGetPaddingTop(parent);
}
