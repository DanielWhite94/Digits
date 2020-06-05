#include <assert.h>
#include <stdio.h>

#include "util.h"

void *mallocNoFail(size_t size) {
	return reallocNoFail(NULL, size);
}

void *reallocNoFail(void *ptr, size_t size) {
	ptr=realloc(ptr, size);
	if (ptr==NULL)
		fatalError("memory allocation failure\n");
	return ptr;
}

void fatalError(const char *format, ...) {
	assert(format!=NULL);

	va_list ap;
	va_start(ap, format);
	fatalErrorV(format, ap);
	va_end(ap);
}

void fatalErrorV(const char *format, va_list ap) {
	assert(format!=NULL);

	vfprintf(stderr, format, ap);

	abort();
}
