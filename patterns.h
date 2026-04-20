#ifndef PATTERNS_H
#define PATTERNS_H

#include "grid.h"

typedef enum {
    PATTERN_GLIDER,
    PATTERN_BLINKER,
    PATTERN_PULSAR,
    PATTERN_RPENTOMINO,
    PATTERN_DIEHARD,
    PATTERN_GOSPER_GUN,
} PatternID;

// Place pattern on grid. Pass -1 for ox/oy to auto-centre.
void pattern_load(Grid *g, PatternID id, int ox, int oy);

#endif
