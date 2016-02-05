#include "Keyboard.h"
#include "World.h"

struct Keyboard *KeyboardCreate(struct World *world)
{
  struct Keyboard *rtn = NULL;

  rtn = gc_alloc(world->gc_ctx, sizeof(*rtn));

  return rtn;
}

int KeyboardKeyDown(struct Keyboard *ctx, int keyCode)
{
  if(keyCode == KEY_LEFT)
  {
    return ctx->left;
  }
  else if(keyCode == KEY_RIGHT)
  {
    return ctx->right;
  }
  else if(keyCode == KEY_UP)
  {
    return ctx->up;
  }
  else if(keyCode == KEY_DOWN)
  {
    return ctx->down;
  }

  return 0;
}