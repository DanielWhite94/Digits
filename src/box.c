#include <assert.h>
#include <stdlib.h>

#include "box.h"
#include "boxprivate.h"
#include "container.h"
#include "containerprivate.h"

int dBoxVTableGetMinWidth(DWidget *widget);
int dBoxVTableGetMinHeight(DWidget *widget);
int dBoxVTableGetWidth(DWidget *widget);
int dBoxVTableGetHeight(DWidget *widget);
int dBoxVTableGetChildXOffset(DWidget *parent, DWidget *child);
int dBoxVTableGetChildYOffset(DWidget *parent, DWidget *child);

DWidget *dBoxNew(DWidgetOrientation orientation) {
	assert(dWidgetOrientationIsValid(orientation));

	// Create widget instance
	DWidget *box=dWidgetNew(DWidgetTypeBox);

	// Call constructor
	dBoxConstructor(box, box->base, orientation);

	return box;
}

void dBoxConstructor(DWidget *widget, DWidgetObjectData *data, DWidgetOrientation orientation) {
	assert(widget!=NULL);
	assert(data!=NULL);
	assert(data->type==DWidgetTypeBox);
	assert(dWidgetOrientationIsValid(orientation));

	// Call super constructor first
	dContainerConstructor(widget, data->super);

	// Setup vtable
	data->vtable.getMinWidth=&dBoxVTableGetMinWidth;
	data->vtable.getMinHeight=&dBoxVTableGetMinHeight;
	data->vtable.getWidth=&dBoxVTableGetWidth;
	data->vtable.getHeight=&dBoxVTableGetHeight;
	data->vtable.getChildXOffset=&dBoxVTableGetChildXOffset;
	data->vtable.getChildYOffset=&dBoxVTableGetChildYOffset;

	// Set orientation
	dWidgetSetOrientation(widget, orientation);
}

int dBoxVTableGetMinWidth(DWidget *widget) {
	assert(widget!=NULL);

	int width=0;
	size_t childCount=dContainerGetChildCount(widget);

	switch(dWidgetGetOrientation(widget)) {
		case DWidgetOrientationHorizontal:
			// Sum of all child min widths
			for(size_t i=0; i<childCount; ++i) {
				DWidget *child=dContainerGetChildN(widget, i);
				width+=dWidgetGetMinWidth(child);
			}
		break;
		case DWidgetOrientationVertical:
			// Max of all child min widths
			for(size_t i=0; i<childCount; ++i) {
				DWidget *child=dContainerGetChildN(widget, i);
				int childWidth=dWidgetGetMinWidth(child);
				if (childWidth>width)
					width=childWidth;
			}
		break;
	}

	// Add padding
	width+=dWidgetGetPaddingLeft(widget)+dWidgetGetPaddingRight(widget);

	return width;
}

int dBoxVTableGetMinHeight(DWidget *widget) {
	assert(widget!=NULL);

	int height=0;
	size_t childCount=dContainerGetChildCount(widget);

	switch(dWidgetGetOrientation(widget)) {
		case DWidgetOrientationHorizontal:
			// Max of all child min heights
			for(size_t i=0; i<childCount; ++i) {
				DWidget *child=dContainerGetChildN(widget, i);
				int childHeight=dWidgetGetMinHeight(child);
				if (childHeight>height)
					height=childHeight;
			}
		break;
		case DWidgetOrientationVertical:
			// Sum of all child min heights
			for(size_t i=0; i<childCount; ++i) {
				DWidget *child=dContainerGetChildN(widget, i);
				height+=dWidgetGetMinHeight(child);
			}
		break;
	}

	// Add padding
	height+=dWidgetGetPaddingTop(widget)+dWidgetGetPaddingBottom(widget);

	return height;
}

int dBoxVTableGetWidth(DWidget *widget) {
	assert(widget!=NULL);

	int width=0;
	size_t childCount=dContainerGetChildCount(widget);

	switch(dWidgetGetOrientation(widget)) {
		case DWidgetOrientationHorizontal:
			// Sum of all child widths
			for(size_t i=0; i<childCount; ++i) {
				DWidget *child=dContainerGetChildN(widget, i);
				width+=dWidgetGetWidth(child);
			}
		break;
		case DWidgetOrientationVertical:
			// Max of all child widths
			for(size_t i=0; i<childCount; ++i) {
				DWidget *child=dContainerGetChildN(widget, i);
				int childWidth=dWidgetGetWidth(child);
				if (childWidth>width)
					width=childWidth;
			}
		break;
	}

	// Add padding
	width+=dWidgetGetPaddingLeft(widget)+dWidgetGetPaddingRight(widget);

	return width;
}

int dBoxVTableGetHeight(DWidget *widget) {
	assert(widget!=NULL);

	int height=0;
	size_t childCount=dContainerGetChildCount(widget);

	switch(dWidgetGetOrientation(widget)) {
		case DWidgetOrientationHorizontal:
			// Max of all child heights
			for(size_t i=0; i<childCount; ++i) {
				DWidget *child=dContainerGetChildN(widget, i);
				int childHeight=dWidgetGetHeight(child);
				if (childHeight>height)
					height=childHeight;
			}
		break;
		case DWidgetOrientationVertical:
			// Sum of all child heights
			for(size_t i=0; i<childCount; ++i) {
				DWidget *child=dContainerGetChildN(widget, i);
				height+=dWidgetGetHeight(child);
			}
		break;
	}

	// Add padding
	height+=dWidgetGetPaddingTop(widget)+dWidgetGetPaddingBottom(widget);

	return height;
}

int dBoxVTableGetChildXOffset(DWidget *parent, DWidget *child) {
	assert(parent!=NULL);
	assert(child!=NULL);

	int offset=dWidgetGetPaddingLeft(parent);

	switch(dWidgetGetOrientation(parent)) {
		case DWidgetOrientationHorizontal: {
			// In horizontal case need to sum all previous child widths
			size_t childCount=dContainerGetChildCount(parent);
			for(size_t i=0; i<childCount; ++i) {
				DWidget *loopChild=dContainerGetChildN(parent, i);
				if (loopChild==child)
					break;
				offset+=dWidgetGetWidth(loopChild);
			}
		} break;
		case DWidgetOrientationVertical:
			// In vertical case all widgets are aligned at the left hand edge
		break;
	}

	return offset;
}

int dBoxVTableGetChildYOffset(DWidget *parent, DWidget *child) {
	assert(parent!=NULL);
	assert(child!=NULL);

	int offset=dWidgetGetPaddingTop(parent);

	switch(dWidgetGetOrientation(parent)) {
		case DWidgetOrientationHorizontal:
			// In horizontal case all widgets are aligned at the top edge
		break;
		case DWidgetOrientationVertical: {
			// In vertical case need to sum all previous child heights
			size_t childCount=dContainerGetChildCount(parent);
			for(size_t i=0; i<childCount; ++i) {
				DWidget *loopChild=dContainerGetChildN(parent, i);
				if (loopChild==child)
					break;
				offset+=dWidgetGetHeight(loopChild);
			}
		} break;
	}

	return offset;
}
