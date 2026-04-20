#ifndef RENDER_H
#define RENDER_H

#include <SDL.h>
#include "grid.h"

// One cell maps to CELL_PX x CELL_PX pixels on screen
#define CELL_PX 6

#define WINDOW_W (COLS * CELL_PX)
#define WINDOW_H (ROWS * CELL_PX)

typedef struct {
    SDL_Renderer *renderer;
    int           show_grid; // draw faint gridlines when 1
} Renderer;

Renderer *renderer_create(SDL_Renderer *r);
void      renderer_destroy(Renderer *rr);
void      renderer_draw(Renderer *rr, Grid *g);

#endif
