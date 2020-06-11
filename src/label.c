#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include <SDL2/SDL_ttf.h>

#include "label.h"
#include "labelprivate.h"
#include "util.h"
#include "widgetprivate.h"

const int dLabelFontSize=26;
const SDL_Color dLabelTextColour={255,255,255};
const char *dLabelFontPath="./fonts/Montserrat-Regular.ttf";

bool dLabelGenerateTexture(DWidget *label); // attempts to render texture (if not already renderer)
void dLabelClearTexture(DWidget *label); // clears cached texture (if any)

void dLabelVTableDestructor(DWidget *widget);
void dLabelVTableRedraw(DWidget *widget, SDL_Renderer *renderer);
int dLabelVTableGetMinWidth(const DWidget *widget);
int dLabelVTableGetMinHeight(const DWidget *widget);
int dLabelVTableGetWidth(DWidget *widget);
int dLabelVTableGetHeight(DWidget *widget);

int dLabelGetTextureWidth(DWidget *widget);
int dLabelGetTextureHeight(DWidget *widget);

DWidget *dLabelNew(const char *text) {
	assert(text!=NULL);

	// Create widget instance
	DWidget *label=dWidgetNew(DWidgetTypeLabel);

	// Call constructor
	dLabelConstructor(label, label->base, text);

	return label;
}

void dLabelConstructor(DWidget *widget, DWidgetObjectData *data, const char *text) {
	assert(widget!=NULL);
	assert(data!=NULL);
	assert(data->type==DWidgetTypeLabel);
	assert(text!=NULL);

	// Call super constructor first
	dWidgetConstructor(widget, data->super);

	// Init fields
	data->d.label.text=dMallocNoFail(1);
	data->d.label.text[0]='\0';
	data->d.label.texture=NULL;

	// Setup vtable
	data->vtable.destructor=&dLabelVTableDestructor;
	data->vtable.redraw=&dLabelVTableRedraw;
	data->vtable.getMinWidth=&dLabelVTableGetMinWidth;
	data->vtable.getMinHeight=&dLabelVTableGetMinHeight;
	data->vtable.getWidth=&dLabelVTableGetWidth;
	data->vtable.getHeight=&dLabelVTableGetHeight;

	// Set text
	dLabelSetText(widget, text);
}

const char *dLabelGetText(const DWidget *label) {
	assert(label!=NULL);

	const DWidgetObjectData *data=dWidgetGetObjectDataConstNoFail(label, DWidgetTypeLabel);

	return data->d.label.text;
}

void dLabelSetText(DWidget *label, const char *text) {
	assert(label!=NULL);
	assert(text!=NULL);

	DWidgetObjectData *data=dWidgetGetObjectDataNoFail(label, DWidgetTypeLabel);

	// No change?
	if (strcmp(text, data->d.label.text)==0)
		return;

	// Update text field
	size_t newSize=strlen(text)+1;
	data->d.label.text=dReallocNoFail(data->d.label.text, newSize);
	memcpy(data->d.label.text, text, newSize);

	// Clear cached texture
	dLabelClearTexture(label);

	// Mark window as dirty to redraw
	dWidgetSetDirty(label);
}

bool dLabelGenerateTexture(DWidget *label) {
	assert(label!=NULL);

	DWidgetObjectData *data=dWidgetGetObjectDataNoFail(label, DWidgetTypeLabel);

	// Already generated?
	if (data->d.label.texture!=NULL)
		return true;

	// Grab renderer from parent window
	SDL_Renderer *renderer=dWidgetGetRenderer(label);
	if (renderer==NULL)
		return false;

	// Open font
	TTF_Font *font=TTF_OpenFont(dLabelFontPath, dLabelFontSize);
	if (font==NULL) {
		dWarning("warning: could not generate label texture for widget %p (%s) - could not open font at '%s'\n", label, dWidgetTypeToString(dWidgetGetBaseType(label)), dLabelFontPath);
		return false;
	}

	// Render text to surface
	SDL_Surface *surface=TTF_RenderText_Blended(font, data->d.label.text, dLabelTextColour);
	if (surface==NULL) {
		dWarning("warning: could not generate label texture for widget %p (%s) - could not render to surface\n", label, dWidgetTypeToString(dWidgetGetBaseType(label)));
		TTF_CloseFont(font);
		return false;
	}

	// Convert surface to texture for rendering to window later
	data->d.label.texture=SDL_CreateTextureFromSurface(renderer, surface);
	if (data->d.label.texture==NULL) {
		dWarning("warning: could not generate label texture for widget %p (%s) - could not create texture\n", label, dWidgetTypeToString(dWidgetGetBaseType(label)));
		SDL_FreeSurface(surface);
		TTF_CloseFont(font);
		return false;
	}

	// Tidy up
	SDL_FreeSurface(surface);
	TTF_CloseFont(font);

	return true;
}

void dLabelClearTexture(DWidget *label) {
	assert(label!=NULL);

	DWidgetObjectData *data=dWidgetGetObjectDataNoFail(label, DWidgetTypeLabel);

	// No texture to clear?
	if (data->d.label.texture==NULL)
		return;

	// Free memory and set pointer to NULL
	SDL_DestroyTexture(data->d.label.texture);
	data->d.label.texture=NULL;
}

void dLabelVTableDestructor(DWidget *widget) {
	assert(widget!=NULL);

	DWidgetObjectData *data=dWidgetGetObjectDataNoFail(widget, DWidgetTypeLabel);

	// Clear cached texture
	dLabelClearTexture(widget);

	// Free memory
	free(data->d.label.text);

	// Call super destructor
	dWidgetDestructor(widget, data->super);
}

void dLabelVTableRedraw(DWidget *widget, SDL_Renderer *renderer) {
	assert(widget!=NULL);
	assert(renderer!=NULL);

	DWidgetObjectData *data=dWidgetGetObjectDataNoFail(widget, DWidgetTypeLabel);

	// Call super redraw
	dWidgetRedraw(widget, data->super, renderer);

	// Render texture to window
	if (dLabelGenerateTexture(widget)) {
		SDL_Rect destRect={
		    .x=dWidgetGetGlobalX(widget)+dWidgetGetPaddingLeft(widget),
		    .y=dWidgetGetGlobalY(widget)+dWidgetGetPaddingTop(widget),
		    .w=dLabelGetTextureWidth(widget),
		    .h=dLabelGetTextureHeight(widget),
		};
		SDL_RenderCopy(renderer, data->d.label.texture, NULL, &destRect);
	}
}

int dLabelVTableGetMinWidth(const DWidget *widget) {
	assert(widget!=NULL);

	// TODO: improve this (should probably be width of widest individual character in the text - i.e. using as much height as needed for one letter per line)
	// HACK: for now simply use dLabelVTableGetWidth, with a const hack
	return dWidgetGetWidth((DWidget *)widget);
}

int dLabelVTableGetMinHeight(const DWidget *widget) {
	assert(widget!=NULL);

	// Minimum is a single line plus padding
	return dLabelFontSize+dWidgetGetPaddingTop(widget)+dWidgetGetPaddingBottom(widget);
}


int dLabelVTableGetWidth(DWidget *widget) {
	assert(widget!=NULL);

	int width=dLabelGetTextureWidth(widget);
	width+=dWidgetGetPaddingLeft(widget)+dWidgetGetPaddingRight(widget);
	return width;
}

int dLabelVTableGetHeight(DWidget *widget) {
	assert(widget!=NULL);

	int height=dLabelGetTextureHeight(widget);
	height+=dWidgetGetPaddingTop(widget)+dWidgetGetPaddingBottom(widget);
	return height;
}

int dLabelGetTextureWidth(DWidget *widget) {
	assert(widget!=NULL);

	DWidgetObjectData *data=dWidgetGetObjectDataNoFail(widget, DWidgetTypeLabel);

	if (!dLabelGenerateTexture(widget))
		return 0;

	int width;
	SDL_QueryTexture(data->d.label.texture, NULL, NULL, &width, NULL);

	return width;
}

int dLabelGetTextureHeight(DWidget *widget) {
	assert(widget!=NULL);

	DWidgetObjectData *data=dWidgetGetObjectDataNoFail(widget, DWidgetTypeLabel);

	if (!dLabelGenerateTexture(widget))
		return 0;

	int height;
	SDL_QueryTexture(data->d.label.texture, NULL, NULL, NULL, &height);

	return height;
}
