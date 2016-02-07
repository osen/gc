#include "GridCell.h"
#include "World.h"
#include "Canvas.h"
#include "Resources.h"
#include "Mouse.h"
#include "Camera.h"

struct GridCell
{
  struct World *world;
  float x;
  float y;
  int type;
  struct Image *image;
  struct Image *blockImage;
  struct Image *cursorImage;
};

struct GridCell *GridCellCreate(struct World *world)
{
  struct GridCell *rtn = NULL;

  rtn = gc_alloc(world->gc_ctx, sizeof(*rtn));
  rtn->world = world;
  rtn->image = ResourcesLoadImage(rtn->world->resources, "images/grass.bmp");
  rtn->blockImage = ResourcesLoadImage(rtn->world->resources, "images/block.bmp");
  rtn->cursorImage = ResourcesLoadImage(rtn->world->resources, "images/cursor.bmp");

  return rtn;
}

int check_collision( SDL_Rect A, SDL_Rect B )
{
    //The sides of the rectangles
    int leftA, leftB;
    int rightA, rightB;
    int topA, topB;
    int bottomA, bottomB;

    //Calculate the sides of rect A
    leftA = A.x;
    rightA = A.x + A.w;
    topA = A.y;
    bottomA = A.y + A.h;

    //Calculate the sides of rect B
    leftB = B.x;
    rightB = B.x + B.w;
    topB = B.y;
    bottomB = B.y + B.h;

     //If any of the sides from A are outside of B
    if( bottomA <= topB )
    {
        return 0;
    }

    if( topA >= bottomB )
    {
        return 0;
    }

    if( rightA <= leftB )
    {
        return 0;
    }

    if( leftA >= rightB )
    {
        return 0;
    }

    //If none of the sides from A are outside B
    return 1;
}

void GridCellDraw(struct GridCell *ctx)
{
  SDL_Rect rect = {0};
  struct Camera *camera = NULL;
  int mouseX = 0;
  int mouseY = 0;
  SDL_Rect mouseRect = {0};

  camera = ctx->world->camera;

  rect.x = ctx->x;
  rect.y = ctx->y;
  rect.w = GRIDCELL_SIZE;
  rect.h = GRIDCELL_SIZE;

  MousePosition(ctx->world->mouse, &mouseX, &mouseY);
  CameraTranslate(camera, &mouseX, &mouseY);
  //mouseRect.x = mouseX + ctx->world->camera->x;
  //mouseRect.y = mouseY + ctx->world->camera->y;
  mouseRect.x = mouseX;
  mouseRect.y = mouseY;
  mouseRect.w = 1;
  mouseRect.h = 1;

  if(ctx->type == 0)
  {
    CameraDrawImage(camera, ctx->image, ctx->x, ctx->y);
  }
  else
  {
    CameraDrawImage(camera, ctx->blockImage, ctx->x, ctx->y);
  }

  if(check_collision(rect, mouseRect) == 1)
  {
    if(MouseClicked(ctx->world->mouse) == 1)
    {
      ctx->type = 1;
    }

    CameraDrawImage(camera, ctx->cursorImage, ctx->x, ctx->y);
  }
}

void GridCellSetPosition(struct GridCell *ctx, float x, float y)
{
  ctx->x = x;
  ctx->y = y;
}
