#ifndef GRIDCELL_H
#define GRIDCELL_H

struct GridCell;
struct World;

struct GridCell *GridCellCreate(struct World *world);
void GridCellDraw(struct GridCell *ctx);
void GridCellSetPosition(struct GridCell *ctx, float x, float y);

#endif