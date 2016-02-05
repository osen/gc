#ifndef MOUSE_H
#define MOUSE_H

struct World;

struct Mouse
{
  struct World *world;
  int x;
  int y;
  int clicked;
};

struct Mouse *MouseCreate(struct World *world);
void MousePosition(struct Mouse *ctx, int *x, int *y);
int MouseClicked(struct Mouse *ctx);

#endif