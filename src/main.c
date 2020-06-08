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

	DWidget *label=dLabelNew("lolwut noob");
	dBinAdd(window, label);

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
