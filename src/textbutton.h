#ifndef TEXTBUTTON_H
#define TEXTBUTTON_H

#include "widget.h"

DWidget *dTextButtonNew(const char *text);

void dTextButtonSetText(DWidget *button, const char *text);

#endif