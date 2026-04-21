#include <SDL.h>
#include <stdio.h>
#include "grid.h"
#include "render.h"
#include "patterns.h"

#define DEFAULT_SPS 10

int main(void) {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        fprintf(stderr, "SDL_Init: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Window *win = SDL_CreateWindow(
        "Conway's Game of Life",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        WINDOW_W, WINDOW_H, 0
    );
    if (!win) {
        fprintf(stderr, "SDL_CreateWindow: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Renderer *sdl_r = SDL_CreateRenderer(win, -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!sdl_r) {
        fprintf(stderr, "SDL_CreateRenderer: %s\n", SDL_GetError());
        return 1;
    }

    // Init grid with toroidal wrapping enabled
    Grid     *grid = grid_create(1);
    Renderer *rend = renderer_create(sdl_r);

    srand(time(NULL));
    int randPattern = rand() % 6; 
    pattern_load(grid, randPattern , -1, -1);

    int running   = 1;
    int paused    = 0;
    int sps       = DEFAULT_SPS;        // simulation steps per second
    Uint32 last_step = SDL_GetTicks();

    printf("Controls:\n");
    printf("  Space      — pause / resume\n");
    printf("  Right      — step one generation (while paused)\n");
    printf("  R          — randomise\n");
    printf("  C          — clear\n");
    printf("  G          — toggle gridlines\n");
    printf("  W          — toggle wrap\n");
    printf("  1-6        — load pattern\n");
    printf("  +/-        — speed up / slow down\n");
    printf("  Left click — draw cells\n");
    printf("  Escape     — quit\n");

    // Main event + game loop
    // this loop runs once per frame for the lifetime of the program
    while (running) {
        SDL_Event e;

        // drain the SDL event queue each frame
        while (SDL_PollEvent(&e)) {
            switch (e.type) {
                case SDL_QUIT:
                    running = 0;
                    break;

                case SDL_KEYDOWN:
                    switch (e.key.keysym.sym) {
                        case SDLK_ESCAPE: running = 0;               break;
                        case SDLK_SPACE:  paused ^= 1;               break;
                        case SDLK_r:      grid_randomise(grid, 0.3f); break;
                        case SDLK_c:      grid_clear(grid);           break;
                        case SDLK_g:      rend->show_grid ^= 1;       break;
                        case SDLK_w:
                            grid->wrap ^= 1;
                            printf("wrap: %s\n", grid->wrap ? "on" : "off");
                            break;
                        case SDLK_RIGHT:
                            if (paused) grid_step(grid);
                            break;
                        case SDLK_PLUS:
                        case SDLK_EQUALS:
                            sps = (sps < 60) ? sps + 1 : 60;
                            printf("sps: %d\n", sps);
                            break;
                        case SDLK_MINUS:
                            sps = (sps > 1) ? sps - 1 : 1;
                            printf("sps: %d\n", sps);
                            break;
                        // Pattern hotkeys 1–6
                        case SDLK_1: grid_clear(grid); pattern_load(grid, PATTERN_GLIDER,     -1,-1); break;
                        case SDLK_2: grid_clear(grid); pattern_load(grid, PATTERN_BLINKER,    -1,-1); break;
                        case SDLK_3: grid_clear(grid); pattern_load(grid, PATTERN_PULSAR,     -1,-1); break;
                        case SDLK_4: grid_clear(grid); pattern_load(grid, PATTERN_RPENTOMINO, -1,-1); break;
                        case SDLK_5: grid_clear(grid); pattern_load(grid, PATTERN_DIEHARD,    -1,-1); break;
                        case SDLK_6: grid_clear(grid); pattern_load(grid, PATTERN_GOSPER_GUN, -1,-1); break;
                    }
                    break;

                case SDL_MOUSEBUTTONDOWN:
                case SDL_MOUSEMOTION:
                    // Draw cells by clicking or dragging
                    if (e.type == SDL_MOUSEMOTION && !(e.motion.state & SDL_BUTTON_LMASK))
                        break;
                    {
                        int mx = (e.type == SDL_MOUSEMOTION) ? e.motion.x : e.button.x;
                        int my = (e.type == SDL_MOUSEMOTION) ? e.motion.y : e.button.y;
                        int cx = mx / CELL_PX;
                        int cy = my / CELL_PX;
                        if (e.type == SDL_MOUSEBUTTONDOWN)
                            grid_toggle(grid, cx, cy);
                        else
                            grid_set(grid, cx, cy, 1); // drag = paint alive
                    }
                    break;
            }
        }

        // Advance simulation at the chosen steps-per-second rate
        // time-based step trigger inside the frame loop
        if (!paused) {
            Uint32 now = SDL_GetTicks();
            Uint32 interval = 1000 / sps;
            if (now - last_step >= interval) {
                grid_step(grid);   // one generation forward
                last_step = now;

                // Print stats to terminal every 50 generations
                if (grid->generation % 50 == 0)
                    printf("gen %d  pop %d\n", grid->generation, grid->population);
            }
        }

        renderer_draw(rend, grid);
    }

    // Cleanup
    renderer_destroy(rend);
    grid_destroy(grid);
    SDL_DestroyRenderer(sdl_r);
    SDL_DestroyWindow(win);
    SDL_Quit();
    return 0;
}
