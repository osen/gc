#include "Mouse.h"
#include "World.h"
#include "Canvas.h"

struct Mouse *MouseCreate(struct World *world)
{
  struct Mouse *ctx = NULL;

  ctx = gc_alloc(world->gc_ctx, sizeof(*ctx));
  ctx->world = world;

  return ctx;
}

void MousePosition(struct Mouse *ctx, int *x, int *y)
{
  *x = ctx->x;
  *y = ctx->y;
  CanvasUnmodify(ctx->world->canvas, x, y);
}

int MouseClicked(struct Mouse *ctx)
{
  return ctx->clicked;
}