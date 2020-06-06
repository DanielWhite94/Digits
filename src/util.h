#ifndef UTIL_H
#define UTIL_H

#include <stdarg.h>
#include <stdlib.h>

void *mallocNoFail(size_t size);
void *reallocNoFail(void *ptr, size_t size);

void fatalError(const char *format, ...);
void fatalErrorV(const char *format, va_list ap);

void warning(const char *format, ...);
void warningV(const char *format, va_list ap);

#endif
