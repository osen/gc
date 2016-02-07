#include "Canvas.h"
#include "World.h"
#include "Resources.h"
#include "Mouse.h"
#include "Keyboard.h"

struct Canvas
{
  SDL_Surface *screen;
  void (*drawFunc)(struct Canvas*);
  void *userData;
  struct World *world;
};

void screen_finalizer(void* unused)
{
  SDL_Quit();
}

void CanvasMainLoop(struct Canvas *ctx, struct GcContext *gc)
{
  SDL_Event event = {0};
  int quit = 0;
  int gcTimeout = 100;
  int keyState = 0;

  while(quit == 0)
  {
    ctx->world->mouse->clicked = 0;

    while(SDL_PollEvent(&event))
    {
      if(event.type == SDL_QUIT)
      {
        quit = 1;
      }
      else if(event.type == SDL_VIDEORESIZE)
      {
        ctx->screen = SDL_SetVideoMode(event.resize.w, event.resize.h, 32,
          SDL_SWSURFACE | SDL_RESIZABLE);

        ResourcesRegenerateScaledImages(ctx->world->resources);
      }
      else if(event.type == SDL_MOUSEMOTION)
      {
        ctx->world->mouse->x = event.motion.x;
        ctx->world->mouse->y = event.motion.y;
      }
      else if(event.type == SDL_MOUSEBUTTONDOWN)
      {
        ctx->world->mouse->clicked = 1;
      }
      else if(event.type == SDL_KEYDOWN || event.type == SDL_KEYUP)
      {
        keyState = 0;

        if(event.type == SDL_KEYDOWN)
        {
          keyState = 1;
        }

        if(event.key.keysym.sym == SDLK_LEFT) ctx->world->keyboard->left = keyState;
        else if(event.key.keysym.sym == SDLK_UP) ctx->world->keyboard->up = keyState;
        else if(event.key.keysym.sym == SDLK_RIGHT) ctx->world->keyboard->right = keyState;
        else if(event.key.keysym.sym == SDLK_DOWN) ctx->world->keyboard->down = keyState;
      }
    }

    if(ctx->drawFunc != NULL)
    {
      ctx->drawFunc(ctx);
    }

    SDL_Flip(ctx->screen);

    gcTimeout --;

    if(gcTimeout <= 0)
    {
      gc_collect(gc);
      gcTimeout = 100;
    }

    SDL_Delay(1000.0f / 60.0f);
  }
}

struct Canvas *CanvasCreate(struct World *world)
{
  struct Canvas *rtn = NULL;

  rtn = gc_alloc(world->gc_ctx, sizeof(*rtn));

  if(SDL_Init(SDL_INIT_EVERYTHING) == -1)
  {
    return NULL;
  }

  rtn->screen = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, 32, SDL_SWSURFACE | SDL_RESIZABLE);

  if(rtn->screen == NULL)
  {
    return NULL;
  }

  gc_finalizer(world->gc_ctx, rtn->screen, screen_finalizer);

  rtn->world = world;

  SDL_WM_SetCaption("Pathfinding Sandbox", NULL);

  return rtn;
}

void CanvasClear(struct Canvas *ctx, int r, int g, int b)
{
  SDL_FillRect(ctx->screen, &ctx->screen->clip_rect, SDL_MapRGB(ctx->screen->format, r, g, b));
}

void CanvasSetUserData(struct Canvas *ctx, void *userData)
{
  ctx->userData = userData;
}

void *CanvasUserData(struct Canvas *ctx)
{
  return ctx->userData;
}

void CanvasSetDrawFunc(struct Canvas *ctx, void (*drawFunc)(struct Canvas*))
{
  ctx->drawFunc = drawFunc;
}

void CanvasDrawImage(struct Canvas *ctx, struct Image *img, int x, int y)
{
  SDL_Rect rect = {0};

  CanvasModify(ctx, &x, &y);
  rect.x = x;
  rect.y = y;

  SDL_BlitSurface(ImageSurface(img), NULL, ctx->screen, &rect);
}

void CanvasDrawRect(struct Canvas *ctx, int x, int y, int w, int h, int r, int g, int b, int a)
{
  SDL_Rect rect = {0};

  rect.x = x;
  rect.y = y;
  rect.w = x;
  rect.h = y;
  CanvasModifyRect(ctx, &rect);
  SDL_FillRect(ctx->screen, &rect, SDL_MapRGBA(ctx->screen->format, r, g, b, a));
}

SDL_Surface *CanvasScreen(struct Canvas *ctx)
{
  return ctx->screen;
}

int CanvasWidth(struct Canvas *ctx)
{
  return ctx->screen->clip_rect.w;
}

int CanvasHeight(struct Canvas *ctx)
{
  return ctx->screen->clip_rect.h;
}

void CanvasModifyRect(struct Canvas *ctx, SDL_Rect* rect)
{
  int x = 0;
  int y = 0;

  x = rect->x;
  y = rect->y;
  CanvasModify(ctx, &x, &y);

  rect->x = x;
  rect->y = y;

  x = rect->w;
  y = rect->h;
  CanvasModify(ctx, &x, &y);

  rect->w = x;
  rect->h = y;
}

void CanvasUnmodify(struct Canvas *ctx, int* x, int* y)
{
  double widthPercent = 0;
  double heightPercent = 0;
  double canvasWidth = 0;
  double canvasHeight = 0;

  canvasWidth = CanvasWidth(ctx);
  canvasHeight = CanvasHeight(ctx);

  if(canvasWidth / canvasHeight >= (double)RELATIVE_WIDTH / (double)RELATIVE_HEIGHT)
  {
    canvasWidth = ((double)RELATIVE_WIDTH / (double)RELATIVE_HEIGHT) * canvasHeight;
  }
  else
  {
    canvasHeight = ((double)RELATIVE_HEIGHT / (double)RELATIVE_WIDTH) * canvasWidth;
  }

  if(x != NULL)
  {
    widthPercent = canvasWidth / (double)RELATIVE_WIDTH;
    *x = *x / widthPercent + 1;
  }

  if(y != NULL)
  {
    heightPercent = canvasHeight / (double)RELATIVE_HEIGHT;
    *y = *y / heightPercent + 1;
  }
}

void CanvasModify(struct Canvas *ctx, int* x, int* y)
{
  double widthPercent = 0;
  double heightPercent = 0;
  double canvasWidth = 0;
  double canvasHeight = 0;

  canvasWidth = CanvasWidth(ctx);
  canvasHeight = CanvasHeight(ctx);

  if(canvasWidth / canvasHeight >= (double)RELATIVE_WIDTH / (double)RELATIVE_HEIGHT)
  {
    canvasWidth = ((double)RELATIVE_WIDTH / (double)RELATIVE_HEIGHT) * canvasHeight;
  }
  else
  {
    canvasHeight = ((double)RELATIVE_HEIGHT / (double)RELATIVE_WIDTH) * canvasWidth;
  }

  if(x != NULL)
  {
    widthPercent = canvasWidth / (double)RELATIVE_WIDTH;
    *x = *x * widthPercent + 1;
  }

  if(y != NULL)
  {
    heightPercent = canvasHeight / (double)RELATIVE_HEIGHT;
    *y = *y * heightPercent + 1;
  }
}
