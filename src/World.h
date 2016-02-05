#ifndef WORLD_H
#define WORLD_H

#include <gc.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define GRIDCELL_SIZE 64

struct Canvas;
struct LinkedList;
struct Resources;
struct Mouse;
struct Keyboard;

struct World
{
  struct Keyboard *keyboard;
  struct Camera *camera;
  struct Mouse *mouse;
  struct Resources *resources;
  struct Canvas *canvas;
  struct LinkedList *grid;
  struct GcContext *gc_ctx;
};

#endif