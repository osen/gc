#ifndef CANVAS_H
#define CANVAS_H

#include <SDL.h>

#define RELATIVE_WIDTH 1200
#define RELATIVE_HEIGHT 900

struct World;
struct Canvas;

struct Canvas *CanvasCreate(struct World *world);
void CanvasRefresh(struct Canvas *ctx);
void CanvasMainLoop(struct Canvas *ctx, struct GcContext *gc);
void CanvasSetUserData(struct Canvas *ctx, void *userData);
void *CanvasUserData(struct Canvas *ctx);
void CanvasSetDrawFunc(struct Canvas *ctx, void (*drawFunc)(struct Canvas*));
void CanvasClear(struct Canvas *ctx, int r, int g, int b);
void CanvasDrawImage(struct Canvas *ctx, struct Image *img, int x, int y);
void CanvasDrawRect(struct Canvas *ctx, int x, int y, int w, int h, int r, int g, int b, int a);
SDL_Surface *CanvasScreen(struct Canvas *ctx);

int CanvasWidth(struct Canvas *ctx);
int CanvasHeight(struct Canvas *ctx);

void CanvasUnmodify(struct Canvas *ctx, int* x, int* y);
void CanvasModify(struct Canvas *ctx, int* x, int* y);
void CanvasModifyRect(struct Canvas *ctx, SDL_Rect* rect);

#endif