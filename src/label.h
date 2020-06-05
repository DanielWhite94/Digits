#ifndef LABEL_H
#define LABEL_H

#include "widget.h"

DWidget *dLabelNew(const char *text);

void dLabelSetText(DWidget *label, const char *text);

#endif