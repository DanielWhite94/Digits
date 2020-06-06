#ifndef WINDOW_H
#define WINDOW_H

#include "widget.h"

DWidget *dWindowNew(const char *title, int width, int height);

const char *dWindowGetTitle(const DWidget *window);

#endif
