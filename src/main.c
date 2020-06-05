#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>

#include "digits.h"

int main(int argc, char **argv) {
	// Initialise widget system
	if (!digitsInit()) {
		printf("error: could not init digits\n");
		return 0;
	}

	// Create test window
	DWidget *window=dWindowNew("My Window", 640, 480);

	DWidget *label=dLabelNew("lolwut noob");
	dBinAdd(window, label);

	// Main loop
	digitsLoop();

	// Tidy up
	dWidgetFree(window);
	digitsQuit();

	return 0;
}
