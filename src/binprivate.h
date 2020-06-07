#ifndef BINPRIVATE_H
#define BINPRIVATE_H

#include "widget.h"

int dBinVTableGetWidth(const DWidget *widget);
int dBinVTableGetHeight(const DWidget *widget);
int dBinVTableGetChildXOffset(const DWidget *parent, const DWidget *child);
int dBinVTableGetChildYOffset(const DWidget *parent, const DWidget *child);

#endif
