#include "render.h"
#include <stdlib.h>

Renderer *renderer_create(SDL_Renderer *r) {
    Renderer *rr = malloc(sizeof(Renderer));
    if (!rr) return NULL;
    rr->renderer   = r;
    rr->show_grid  = 0;
    return rr;
}

void renderer_destroy(Renderer *rr) {
    free(rr);
}

// Map cell age to an RGB color for the trail effect.
// Young cells (age 1) are bright white-green.
// Older cells shift toward deep teal then purple.
// ITERATION: called once per live cell inside renderer_draw.
static void age_to_color(uint8_t age, uint8_t *r, uint8_t *g, uint8_t *b) {
    if (age < 8) {
        // Fresh — bright white-green
        *r = 200 + age * 5;
        *g = 255;
        *b = 200 + age * 5;
    } else if (age < 40) {
        // Settling — fade toward cyan
        uint8_t t = age - 8;
        *r = 50  + (205 - t * 4);
        *g = 220;
        *b = 255;
    } else if (age < 120) {
        // Stable — deep teal
        uint8_t t = age - 40;
        *r = 20;
        *g = 180 - t;
        *b = 200 - t / 2;
    } else {
        // Ancient — dim purple
        *r = 80;
        *g = 40;
        *b = 120;
    }
}

// Draw the entire grid each frame.
// ITERATION: nested loop visits every cell exactly once.
void renderer_draw(Renderer *rr, Grid *g) {
    SDL_Renderer *r = rr->renderer;

    // Clear to near-black background
    SDL_SetRenderDrawColor(r, 10, 10, 15, 255);
    SDL_RenderClear(r);

    // ITERATION: draw every cell
    for (int y = 0; y < ROWS; y++) {
        for (int x = 0; x < COLS; x++) {
            Cell *c = grid_at(g, x, y);

            if (c->alive) {
                uint8_t cr, cg, cb;
                age_to_color(c->age, &cr, &cg, &cb);
                SDL_SetRenderDrawColor(r, cr, cg, cb, 255);

                SDL_Rect rect = {
                    .x = x * CELL_PX,
                    .y = y * CELL_PX,
                    .w = CELL_PX - (rr->show_grid ? 1 : 0),
                    .h = CELL_PX - (rr->show_grid ? 1 : 0),
                };
                SDL_RenderFillRect(r, &rect);
            }
        }
    }

    SDL_RenderPresent(r);
}
