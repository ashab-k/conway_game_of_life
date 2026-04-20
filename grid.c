#include "grid.h"
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* ─── Helpers ─────────────────────────────────────────────────────────────── */

Grid *grid_create(int wrap) {
    Grid *g = calloc(1, sizeof(Grid));
    if (!g) return NULL;
    g->wrap = wrap;
    return g;
}

void grid_destroy(Grid *g) {
    free(g);
}

void grid_clear(Grid *g) {
    memset(g->current, 0, sizeof(g->current));
    memset(g->next,    0, sizeof(g->next));
    g->generation = 0;
    g->population  = 0;
}

void grid_set(Grid *g, int x, int y, uint8_t alive) {
    if (x < 0 || x >= COLS || y < 0 || y >= ROWS) return;
    g->current[y * COLS + x].alive = alive;
    g->current[y * COLS + x].age   = 0;
}

void grid_toggle(Grid *g, int x, int y) {
    if (x < 0 || x >= COLS || y < 0 || y >= ROWS) return;
    Cell *c = &g->current[y * COLS + x];
    c->alive ^= 1;
    c->age    = 0;
}

void grid_randomise(Grid *g, float density) {
    srand((unsigned)time(NULL));
    grid_clear(g);
    /* ITERATION: visit every cell and probabilistically bring it alive */
    for (int y = 0; y < ROWS; y++) {
        for (int x = 0; x < COLS; x++) {
            g->current[y * COLS + x].alive =
                ((float)rand() / RAND_MAX) < density ? 1 : 0;
        }
    }
}

/* ─── Neighbour count ─────────────────────────────────────────────────────────
 * ITERATION: loop over the 3x3 Moore neighbourhood around (cx, cy),
 * skipping the centre cell itself.
 *
 * Toroidal wrapping uses modulo so opposite edges are adjacent:
 *   nx = (cx - 1 + COLS) % COLS   ← never negative                         */
static int count_neighbours(Grid *g, int cx, int cy) {
    int count = 0;

    for (int dy = -1; dy <= 1; dy++) {          /* ITERATION over neighbourhood */
        for (int dx = -1; dx <= 1; dx++) {
            if (dx == 0 && dy == 0) continue;   /* skip self */

            int nx, ny;
            if (g->wrap) {
                /* Toroidal: wrap around edges with modulo */
                nx = (cx + dx + COLS) % COLS;
                ny = (cy + dy + ROWS) % ROWS;
            } else {
                /* Bounded: out-of-bounds cells treated as dead */
                nx = cx + dx;
                ny = cy + dy;
                if (nx < 0 || nx >= COLS || ny < 0 || ny >= ROWS)
                    continue;
            }

            count += g->current[ny * COLS + nx].alive;
        }
    }
    return count;
}

/* ─── Simulation step ─────────────────────────────────────────────────────────
 * ITERATION: outer double-loop visits every cell exactly once per generation.
 *
 * Conway's rules (B3/S23):
 *   - A dead  cell with exactly 3 neighbours becomes alive  (birth)
 *   - A live  cell with 2 or 3 neighbours stays alive       (survival)
 *   - Everything else dies or stays dead                    (death)
 *
 * We write results into `next`, then memcpy back to `current`.
 * This is the double-buffer swap — reads and writes never alias.            */
void grid_step(Grid *g) {
    int pop = 0;

    /* ITERATION: every cell in the grid */
    for (int y = 0; y < ROWS; y++) {
        for (int x = 0; x < COLS; x++) {
            int   n      = count_neighbours(g, x, y);   /* ITERATION inside */
            Cell *cur    = &g->current[y * COLS + x];
            Cell *nxt    = &g->next   [y * COLS + x];

            /* Apply B3/S23 rules */
            if (cur->alive) {
                nxt->alive = (n == 2 || n == 3) ? 1 : 0;
            } else {
                nxt->alive = (n == 3) ? 1 : 0;
            }

            /* Age tracking: increment if still alive, reset on death */
            if (nxt->alive) {
                nxt->age = (cur->age < 255) ? cur->age + 1 : 255;
                pop++;
            } else {
                nxt->age = 0;
            }
        }
    }

    /* Double-buffer swap: next becomes current for the next generation */
    memcpy(g->current, g->next, sizeof(g->current));
    g->generation++;
    g->population = pop;
}
