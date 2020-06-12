#include <assert.h>
#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>

#include "digits.h"

DWidgetSignalReturn windowCloseHandler(const DWidgetSignalEvent *event, void *userData);
DWidgetSignalReturn buttonClickHandler(const DWidgetSignalEvent *event, void *userData);

int main(int argc, char **argv) {
	// Initialise widget system
	if (!digitsInit()) {
		printf("error: could not init digits\n");
		return 0;
	}

	// Create test windows
	DWidget *window=dWindowNew("My Window", 640, 480);

	DWidget *box=dBoxNew(DWidgetOrientationVertical);
	dBinAdd(window, box);

	DWidget *label;
	label=dLabelNew("lolwut noob");
	dWidgetSetVExpand(label, true);
	dContainerAdd(box, label);
	label=dLabelNew("2");
	dWidgetSetVExpand(label, true);
	dContainerAdd(box, label);
	label=dLabelNew("three");
	dWidgetSetVExpand(label, true);
	dContainerAdd(box, label);

	DWidget *box2=dBoxNew(DWidgetOrientationHorizontal);
	DWidget *spacer=dLabelNew("(spacer)");
	dWidgetSetHExpand(spacer, true);
	dContainerAdd(box2, spacer);
	dContainerAdd(box2, dTextButtonNew("Ok"));
	dContainerAdd(box2, dTextButtonNew("Cancel"));
	dContainerAdd(box, box2);

	DWidget *window2=dWindowNew("New Win", 160, 100);

	DWidget *button=dTextButtonNew("click me!");
	dBinAdd(window2, button);

	// Connect signals
	dWidgetSignalConnect(window, DWidgetSignalTypeWindowClose, &windowCloseHandler, NULL);
	dWidgetSignalConnect(window2, DWidgetSignalTypeWindowClose, &windowCloseHandler, NULL);

	dWidgetSignalConnect(button, DWidgetSignalTypeButtonClick, &buttonClickHandler, NULL);

	// Debugging
	dWidgetDebug(window, 0);
	dWidgetDebug(window2, 0);

	// Main loop
	digitsLoop();

	// Tidy up
	dWidgetFree(window);
	dWidgetFree(window2);
	digitsQuit();

	return 0;
}

DWidgetSignalReturn windowCloseHandler(const DWidgetSignalEvent *event, void *userData) {
	assert(event!=NULL);
	assert(event->type==DWidgetSignalTypeWindowClose);
	assert(userData==NULL);

	printf("windowCloseHandler for %p - %s\n", event->widget, dWindowGetTitle(event->widget));

	digitsLoopStop();

	return DWidgetSignalReturnContinue;
}

DWidgetSignalReturn buttonClickHandler(const DWidgetSignalEvent *event, void *userData) {
	assert(event!=NULL);
	assert(event->type==DWidgetSignalTypeButtonClick);
	assert(userData==NULL);

	printf("button click for %p\n", event->widget);

	return DWidgetSignalReturnStop;
}
