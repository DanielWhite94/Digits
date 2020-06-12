#ifndef BIN_H
#define BIN_H

#include <stdbool.h>

#include "widget.h"

DWidget *dBinNew(DWidget *child); // child can be NULL and added later with dBinAdd

bool dBinAdd(DWidget *bin, DWidget *child); // fails if bin already has child, or child already has parent

DWidget *dBinGetChild(DWidget *bin);
const DWidget *dBinGetChildConst(const DWidget *bin);

#endif
