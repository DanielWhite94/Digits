#include <assert.h>
#include <stdio.h>

#include <SDL2/SDL.h>

#include "util.h"
#include "utilprivate.h"

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

void dDelayMs(DTimeMs delay) {
	// TODO: fix this if given delay does not fit in 32 bit unsigned int
	SDL_Delay(delay);
}

void dSetRenderDrawColour(SDL_Renderer *renderer, const DColour *colour) {
	assert(renderer!=NULL);
	assert(colour!=NULL);

	SDL_SetRenderDrawColor(renderer, colour->r, colour->g, colour->b, colour->a);
}
