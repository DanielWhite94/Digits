#ifndef CONTAINER_H
#define CONTAINER_H

#include <stdbool.h>

#include "widget.h"

bool dContainerAdd(DWidget *container, DWidget *child); // fails if child already has parent

DWidget *dContainerGetChildN(DWidget *container, size_t n);
size_t dContainerGetChildCount(const DWidget *container);

#endif
