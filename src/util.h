#ifndef UTIL_H
#define UTIL_H

#include <stdarg.h>
#include <stdlib.h>

void *dMallocNoFail(size_t size);
void *dReallocNoFail(void *ptr, size_t size);

void dFatalError(const char *format, ...);
void dFatalErrorV(const char *format, va_list ap);

void dWarning(const char *format, ...);
void dWarningV(const char *format, va_list ap);

#endif
