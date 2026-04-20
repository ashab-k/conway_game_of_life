#ifndef GRID_H
#define GRID_H

#include <stdint.h>

#define COLS 160
#define ROWS 120

typedef struct {
    uint8_t alive;
    uint8_t age;
} Cell;

// Double-buffer: we READ from `current` and WRITE into `next`, then swap.
// This ensures every cell sees the same generation's state during a step
typedef struct {
    Cell current[ROWS * COLS];
    Cell next   [ROWS * COLS];
    int  generation;
    int  population;   // live cell count, updated each step
    int  wrap;         // 1 = toroidal (edges connect), 0 = bounded
} Grid;

Grid *grid_create(int wrap);
void  grid_destroy(Grid *g);
void  grid_clear(Grid *g);
void  grid_step(Grid *g);
void  grid_toggle(Grid *g, int x, int y);
void  grid_set(Grid *g, int x, int y, uint8_t alive);
void  grid_randomise(Grid *g, float density);

static inline Cell *grid_at(Grid *g, int x, int y) {
    return &g->current[y * COLS + x];
}

#endif 
