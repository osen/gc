#ifndef CAMERA_H
#define CAMERA_H

struct World;

struct Camera
{
  struct World *world;
  int x;
  int y;
};

struct Camera *CameraCreate(struct World *world);
void CameraUpdate(struct Camera *ctx);
void CameraDrawImage(struct Camera *ctx, struct Image *img, int x, int y);
void CameraTranslate(struct Camera *ctx, int* x, int* y);

#endif