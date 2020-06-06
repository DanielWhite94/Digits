#ifndef DIGITS_H
#define DIGITS_H

#include <stdbool.h>

#include "bin.h"
#include "button.h"
#include "label.h"
#include "textbutton.h"
#include "widget.h"
#include "window.h"

bool digitsInit(void); // also returns true if already initialised
void digitsQuit(void); // this should only be called once, regardless of how many times init was called

void digitsLoop(void); // enters the main event loop
void digitsLoopStop(void); // exits the loop

#endif
