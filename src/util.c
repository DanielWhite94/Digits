#include <assert.h>
#include <stdio.h>

#include "util.h"

void *dMallocNoFail(size_t size) {
	return dReallocNoFail(NULL, size);
}

void *dReallocNoFail(void *ptr, size_t size) {
	ptr=realloc(ptr, size);
	if (ptr==NULL)
		dFatalError("error: memory allocation failure\n");
	return ptr;
}

void dFatalError(const char *format, ...) {
	assert(format!=NULL);

	va_list ap;
	va_start(ap, format);
	dFatalErrorV(format, ap);
	va_end(ap);
}

void dFatalErrorV(const char *format, va_list ap) {
	assert(format!=NULL);

	dWarningV(format, ap);
	abort();
}

void dWarning(const char *format, ...) {
	assert(format!=NULL);

	va_list ap;
	va_start(ap, format);
	dWarningV(format, ap);
	va_end(ap);
}

void dWarningV(const char *format, va_list ap) {
	assert(format!=NULL);

	vfprintf(stderr, format, ap);
}

