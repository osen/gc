#ifndef GRIDCELL_H
#define GRIDCELL_H

#define GRIDCELL_TOP 0
#define GRIDCELL_BOTTOM 1
#define GRIDCELL_LEFT 2
#define GRIDCELL_RIGHT 3
#define GRIDCELL_TOPLEFT 4
#define GRIDCELL_BOTTOMRIGHT 5
#define GRIDCELL_TOPRIGHT 6
#define GRIDCELL_BOTTOMLEFT 7

struct GridCell;
struct World;

struct GridCell *GridCellCreate(struct World *world);
void GridCellDraw(struct GridCell *ctx);
void GridCellSetPosition(struct GridCell *ctx, float x, float y);
void GridCellSetNeighbour(struct GridCell *ctx, int position, struct GridCell *neighbour);

#endif