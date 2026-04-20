#include "patterns.h"

// Stamp a list of {dx,dy} pairs onto the grid at offset (ox,oy).
// List is terminated by a {-1,-1} sentinel.
// ITERATION: loops over every coordinate pair and calls grid_set.
static void stamp(Grid *g, const int cells[][2], int ox, int oy) {
    for (int i = 0; cells[i][0] != -1; i++) {
        int x = ox + cells[i][0];
        int y = oy + cells[i][1];
        grid_set(g, x, y, 1);
    }
}

// Compute bounding box of a pattern and return offsets to centre it on grid.
static void centre_offset(const int cells[][2], int *ox, int *oy) {
    int minx = 9999, miny = 9999, maxx = -9999, maxy = -9999;
    for (int i = 0; cells[i][0] != -1; i++) {
        if (cells[i][0] < minx) minx = cells[i][0];
        if (cells[i][0] > maxx) maxx = cells[i][0];
        if (cells[i][1] < miny) miny = cells[i][1];
        if (cells[i][1] > maxy) maxy = cells[i][1];
    }
    *ox = (COLS - (maxx - minx)) / 2 - minx;
    *oy = (ROWS - (maxy - miny)) / 2 - miny;
}

// Glider — travels diagonally, period 4
static const int glider[][2] = {
    {1,0},{2,1},{0,2},{1,2},{2,2},{-1,-1}
};

// Blinker — simplest oscillator, period 2
static const int blinker[][2] = {
    {0,0},{1,0},{2,0},{-1,-1}
};

// Pulsar — period 3, one of the most common large oscillators
static const int pulsar[][2] = {
    {2,0},{3,0},{4,0},{8,0},{9,0},{10,0},
    {0,2},{5,2},{7,2},{12,2},
    {0,3},{5,3},{7,3},{12,3},
    {0,4},{5,4},{7,4},{12,4},
    {2,5},{3,5},{4,5},{8,5},{9,5},{10,5},
    {2,7},{3,7},{4,7},{8,7},{9,7},{10,7},
    {0,8},{5,8},{7,8},{12,8},
    {0,9},{5,9},{7,9},{12,9},
    {0,10},{5,10},{7,10},{12,10},
    {2,12},{3,12},{4,12},{8,12},{9,12},{10,12},
    {-1,-1}
};

// R-pentomino — small seed that runs for 1103 generations before stabilising
static const int rpentomino[][2] = {
    {1,0},{2,0},{0,1},{1,1},{1,2},{-1,-1}
};

// Diehard — vanishes completely after exactly 130 generations
static const int diehard[][2] = {
    {6,0},{0,1},{1,1},{1,2},{5,2},{6,2},{7,2},{-1,-1}
};

// Gosper Glider Gun — first discovered infinite-growth pattern
// Emits a new glider every 30 generations
static const int gosper_gun[][2] = {
    {24,0},
    {22,1},{24,1},
    {12,2},{13,2},{20,2},{21,2},{34,2},{35,2},
    {11,3},{15,3},{20,3},{21,3},{34,3},{35,3},
    {0,4},{1,4},{10,4},{16,4},{20,4},{21,4},
    {0,5},{1,5},{10,5},{14,5},{16,5},{17,5},{22,5},{24,5},
    {10,6},{16,6},{24,6},
    {11,7},{15,7},
    {12,8},{13,8},
    {-1,-1}
};

void pattern_load(Grid *g, PatternID id, int ox, int oy) {
    const int (*cells)[2];

    // Select the pattern data
    switch (id) {
        case PATTERN_GLIDER:     cells = glider;     break;
        case PATTERN_BLINKER:    cells = blinker;    break;
        case PATTERN_PULSAR:     cells = pulsar;     break;
        case PATTERN_RPENTOMINO: cells = rpentomino; break;
        case PATTERN_DIEHARD:    cells = diehard;    break;
        case PATTERN_GOSPER_GUN: cells = gosper_gun; break;
        default: return;
    }

    // Auto-centre if caller passed -1
    if (ox == -1 || oy == -1)
        centre_offset(cells, &ox, &oy);

    stamp(g, cells, ox, oy);
}
