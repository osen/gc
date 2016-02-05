#include "Canvas.h"
#include "GridCell.h"
#include "LinkedList.h"
#include "World.h"
#include "Resources.h"
#include "Mouse.h"
#include "Camera.h"
#include "Keyboard.h"

#include <gc.h>
#include <SDL.h>

#include <stdio.h>
#include <stdlib.h>

void draw(struct Canvas *canvas)
{
  struct World *world = NULL;
  struct GridCell *cell = NULL;
  struct LinkedListItem *cellItem = NULL;

  CanvasClear(canvas, 100, 149, 237);
  world = CanvasUserData(canvas);
  CameraUpdate(world->camera);
  cellItem = LinkedListFirst(world->grid);

  while(cellItem != NULL)
  {
    cell = LinkedListItemGet(cellItem);
    GridCellDraw(cell);

    cellItem = LinkedListItemNext(cellItem);
  }
}

void prepare_grid(struct World *world)
{
  int y = 0;
  int x = 0;
  int gridWidth = 18;
  int gridHeight = 13;
  struct LinkedListItem *cellItem;
  struct GridCell *cell;

  world->grid = LinkedListCreate(world->gc_ctx);

  for(y = 0; y < gridHeight; y++)
  {
    for(x = 0; x < gridWidth; x++)
    {
      cell = GridCellCreate(world);
      cellItem = LinkedListAdd(world->grid, cell);
      GridCellSetPosition(cell, x * GRIDCELL_SIZE, y * GRIDCELL_SIZE);
    }
  }
}

int main(int argc, char* argv[])
{
  struct GcContext *gc_ctx = NULL;
  struct World *world = NULL;

  gc_ctx = gc_context();
  world = gc_root(gc_ctx, sizeof(*world));

  if(!world)
  {
    printf("Failed to allocate GC root\n");

    return 1;
  }

  world->gc_ctx = gc_ctx;
  world->camera = CameraCreate(world);
  world->resources = ResourcesCreate(world);
  world->canvas = CanvasCreate(world);
  world->mouse = MouseCreate(world);
  world->keyboard = KeyboardCreate(world);

  CanvasSetUserData(world->canvas, world);
  CanvasSetDrawFunc(world->canvas, draw);

  prepare_grid(world);

  CanvasMainLoop(world->canvas, gc_ctx);

  memset(world, 0, sizeof(*world));
  gc_collect(gc_ctx);

  return 0;
}

