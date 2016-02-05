#ifndef RESOURCES_H
#define RESOURCES_H

#include <SDL.h>

struct Resources;
struct World;

struct Image;

SDL_Surface *ImageSurface(struct Image *ctx);

struct Resources *ResourcesCreate(struct World *world);
struct Image *ResourcesLoadImage(struct Resources *ctx, char *path);
void ResourcesRegenerateScaledImages(struct Resources *ctx);

#endif