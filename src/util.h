#ifndef UTIL_H
#define UTIL_H

#include <stdarg.h>
#include <stdint.h>
#include <stdlib.h>

typedef uint64_t DTimeMs;

typedef struct {
	uint8_t r, g, b, a; // values in range 0-255
} DColour;

void *dMallocNoFail(size_t size);
void *dReallocNoFail(void *ptr, size_t size);

void dFatalError(const char *format, ...);
void dFatalErrorV(const char *format, va_list ap);

void dWarning(const char *format, ...);
void dWarningV(const char *format, va_list ap);

void dDelayMs(DTimeMs delay);

#endif
