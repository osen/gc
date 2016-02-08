#include "Resources.h"
#include "World.h"
#include "LinkedList.h"
#include "Canvas.h"
#include "lodepng.h"

#include <SDL.h>

struct Resources
{
  struct World *world;
  struct LinkedList *images;
};

struct Image
{
  char *path;
  SDL_Surface *raw;
  SDL_Surface *scaled;
  struct Resources *resources;
};

SDL_Surface *ImageSurface(struct Image *ctx)
{
  return ctx->scaled;
}

struct Resources *ResourcesCreate(struct World *world)
{
  struct Resources *rtn = NULL;

  rtn = gc_alloc(world->gc_ctx, sizeof(*rtn));
  rtn->world = world;
  rtn->images = LinkedListCreate(world->gc_ctx);

  return rtn;
}

void _SDL_FreeSurface(void *ptr)
{
  SDL_FreeSurface((SDL_Surface *)ptr);
}

/*
 * Set the pixel at (x, y) to the given value
 * NOTE: The surface must be locked before calling this!
 */
void putpixel(SDL_Surface *surface, int x, int y, Uint32 pixel)
{
    int bpp = surface->format->BytesPerPixel;
    /* Here p is the address to the pixel we want to set */
    Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

    switch(bpp) {
    case 1:
        *p = pixel;
        break;

    case 2:
        *(Uint16 *)p = pixel;
        break;

    case 3:
        if(SDL_BYTEORDER == SDL_BIG_ENDIAN) {
            p[0] = (pixel >> 16) & 0xff;
            p[1] = (pixel >> 8) & 0xff;
            p[2] = pixel & 0xff;
        } else {
            p[0] = pixel & 0xff;
            p[1] = (pixel >> 8) & 0xff;
            p[2] = (pixel >> 16) & 0xff;
        }
        break;

    case 4:
        *(Uint32 *)p = pixel;
        break;
    }
}

/*
 * Return the pixel value at (x, y)
 * NOTE: The surface must be locked before calling this!
 */
Uint32 getpixel(SDL_Surface *surface, int x, int y)
{
    int bpp = surface->format->BytesPerPixel;
    /* Here p is the address to the pixel we want to retrieve */
    Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

    switch(bpp) {
    case 1:
        return *p;

    case 2:
        return *(Uint16 *)p;

    case 3:
        if(SDL_BYTEORDER == SDL_BIG_ENDIAN)
            return p[0] << 16 | p[1] << 8 | p[2];
        else
            return p[0] | p[1] << 8 | p[2] << 16;

    case 4:
        return *(Uint32 *)p;

    default:
        return 0;       /* shouldn't happen, but avoids warnings */
    }
}

void _ResourcesGenerateScaledImage(struct Image *ctx)
{
/*
  double widthPercent = 0;
  double heightPercent = 0;
  SDL_Surface *Surface = ctx->raw;
  int Width = 0;
  int Height = 0;
  double _stretch_factor_x = 0;
  double _stretch_factor_y = 0;
  double y = 0;
  double x = 0;
  double o_y = 0;
  double o_x = 0;
  SDL_Surface *_ret = NULL;
  double canvasWidth = CanvasWidth(ctx->resources->world->canvas);
  double canvasHeight = CanvasHeight(ctx->resources->world->canvas);

  if(canvasWidth / canvasHeight > 800.0f / 600.0f)
  {
    canvasWidth = (800.0f / 600.0f) * canvasHeight;
  }
  else
  {
    canvasHeight = (600.0f / 800.0f) * canvasWidth;
  }

  //canvasHeight = (600.0f / 800.0f) * canvasWidth;
  //canvasWidth = (800.0f / 600.0f) * canvasHeight;

  widthPercent = canvasWidth / 800.0f;
  heightPercent = canvasHeight / 600.0f;
  //heightPercent =  (800.0f / 600.0f) * canvasHeight;

  Width = Surface->w * widthPercent;
  Height = Surface->h * heightPercent;
*/


  int Height = 0;
  int Width = 0;
  SDL_Surface *Surface = ctx->raw;
  SDL_Surface *_ret = NULL;
  double _stretch_factor_x = 0;
  double _stretch_factor_y = 0;
  double y = 0;
  double x = 0;
  double o_y = 0;
  double o_x = 0;

  Width = Surface->clip_rect.w;
  Height = Surface->clip_rect.h;
  CanvasModify(ctx->resources->world->canvas, &Width, &Height);
    
  _ret = SDL_CreateRGBSurface(Surface->flags, Width, Height, Surface->format->BitsPerPixel,
    Surface->format->Rmask, Surface->format->Gmask, Surface->format->Bmask, Surface->format->Amask);

  if(_ret == NULL)
  {
    exit(1);
  }

  gc_finalizer(ctx->resources->world->gc_ctx, _ret, _SDL_FreeSurface);

  _stretch_factor_x = (double)Width  / (double)Surface->w,
  _stretch_factor_y = (double)Height / (double)Surface->h;

  for(y = 0; y < Surface->h; y++)
  {
    for(x = 0; x < Surface->w; x++)
    {
      for(o_y = 0; o_y < _stretch_factor_y; ++o_y)
      {
        for(o_x = 0; o_x < _stretch_factor_x; ++o_x)
        {
          putpixel(_ret, _stretch_factor_x * x + o_x, 
            _stretch_factor_y * y + o_y, getpixel(Surface, x, y));
        }
      }
    }
  }

  ctx->scaled = _ret;
}

void ResourcesRegenerateScaledImages(struct Resources *ctx)
{
  struct LinkedListItem *imageItem = NULL;
  struct Image *image = NULL;

  imageItem = LinkedListFirst(ctx->images);

  while(imageItem != NULL)
  {
    image = LinkedListItemGet(imageItem);
    _ResourcesGenerateScaledImage(image);

    imageItem = LinkedListItemNext(imageItem);
  }
}

SDL_Surface *LoadPng(struct GcContext *gc, char *path)
{
  SDL_Surface *rtn = NULL;
  unsigned error = 0;
  unsigned char* image = NULL;
  unsigned width = 0;
  unsigned height = 0;
  int x = 0;
  int y = 0;
  int pixel = 0;
  Uint32 rmask = 0;
  Uint32 gmask = 0;
  Uint32 bmask = 0;
  Uint32 amask = 0;

  error = lodepng_decode32_file(&image, &width, &height, path);

  if(error) 
  {
    printf("Error %u: %s\n", error, lodepng_error_text(error));
    exit(1);
  }

  gc_finalizer(gc, image, free);

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
    rmask = 0xff000000;
    gmask = 0x00ff0000;
    bmask = 0x0000ff00;
    amask = 0x000000ff;
#else
    rmask = 0x000000ff;
    gmask = 0x0000ff00;
    bmask = 0x00ff0000;
    amask = 0xff000000;
#endif

  rtn = SDL_CreateRGBSurface(0, width, height, 32,
    rmask, gmask, bmask, amask);

  if(rtn == NULL)
  {
    exit(1);
  }

  gc_finalizer(gc, rtn, _SDL_FreeSurface);

  for(y = 0; y < height; y++)
  {
    for(x = 0; x < width; x++)
    {
      putpixel(rtn, x, y, SDL_MapRGBA(rtn->format, image[pixel], image[pixel + 1], image[pixel + 2], image[pixel + 3]));
      pixel += 4;
    }
  }

  return rtn;
}

struct Image *ResourcesLoadImage(struct Resources *ctx, char *path)
{
  struct LinkedListItem *imageItem = NULL;
  struct Image *image = NULL;
  SDL_Surface *loadedImage = NULL;

  imageItem = LinkedListFirst(ctx->images);

  while(imageItem != NULL)
  {
    image = LinkedListItemGet(imageItem);

    if(strcmp(image->path, path) == 0)
    {
      return image;
    }

    imageItem = LinkedListItemNext(imageItem);
  }

  image = gc_alloc(ctx->world->gc_ctx, sizeof(*image));

  if(image == NULL)
  {
    exit(1);
  }

  image->resources = ctx;

  image->path = strdup(path);
  gc_finalizer(ctx->world->gc_ctx, image->path, free);

  //loadedImage = SDL_LoadBMP(path);
  loadedImage = LoadPng(ctx->world->gc_ctx, path);

  image->raw = SDL_DisplayFormatAlpha(loadedImage);
  gc_finalizer(ctx->world->gc_ctx, image->raw, _SDL_FreeSurface);

  _ResourcesGenerateScaledImage(image);
  LinkedListAdd(ctx->images, image);

  return image;
}
