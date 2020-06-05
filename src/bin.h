#ifndef BIN_H
#define BIN_H

#include <stdbool.h>

#include "widget.h"

bool dBinAdd(DWidget *bin, DWidget *child); // fails if bin already has child, or child already has parent

DWidget *dBinGetChild(DWidget *bin);

#endif
