#include "Camera.h"
#include "World.h"
#include "Keyboard.h"
#include "Canvas.h"

struct Camera *CameraCreate(struct World *world)
{
  struct Camera *rtn = NULL;

  rtn = gc_alloc(world->gc_ctx, sizeof(*rtn));
  rtn->world = world;
  //rtn->x = -23;
  //rtn->y = -30;
  rtn->x = 576;
  rtn->y = 416;

  return rtn;
}

void CameraUpdate(struct Camera *ctx)
{
  if(KeyboardKeyDown(ctx->world->keyboard, KEY_DOWN) == 1)
  {
    ctx->y +=5;
  }
  if(KeyboardKeyDown(ctx->world->keyboard, KEY_UP) == 1)
  {
    ctx->y -=5;
  }
  if(KeyboardKeyDown(ctx->world->keyboard, KEY_LEFT) == 1)
  {
    ctx->x -=5;
  }
  if(KeyboardKeyDown(ctx->world->keyboard, KEY_RIGHT) == 1)
  {
    ctx->x +=5;
  }
}

void CameraTranslate(struct Camera *ctx, int* x, int* y)
{
  int offsetX = 0;
  int offsetY = 0;
  int cWidth = 0;
  int cHeight = 0;

  cWidth = CanvasWidth(ctx->world->canvas) / 2.0f;
  cHeight = CanvasHeight(ctx->world->canvas) / 2.0f;
  CanvasUnmodify(ctx->world->canvas, &cWidth, &cHeight);

  offsetX = ctx->x - cWidth;
  offsetY = ctx->y - cHeight;

  *x += offsetX;
  *y += offsetY;
}

void CameraDrawImage(struct Camera *ctx, struct Image *img, int x, int y)
{
  int canvasWidth = 0;
  int canvasHeight = 0;

  canvasWidth = CanvasWidth(ctx->world->canvas);
  canvasHeight = CanvasHeight(ctx->world->canvas);
  CanvasUnmodify(ctx->world->canvas, &canvasWidth, &canvasHeight);

  CanvasDrawImage(ctx->world->canvas, img, x + (canvasWidth / 2.0f - ctx->x), y + (canvasHeight / 2.0f) - ctx->y);
  //CanvasDrawImage(ctx->world->canvas, img, x - ctx->x, y - ctx->y);
}