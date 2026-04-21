#include <SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "grid.h"
#include "render.h"
#include "patterns.h"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#define DEFAULT_SPS 10

typedef struct {
    Grid        *grid;
    Renderer    *rend;
    SDL_Window  *win;
    SDL_Renderer *sdl_r;
    int          paused;
    int          sps;
    int          running;
    Uint32       last_step;
} AppState;

static AppState app;

static void handle_events(void) {
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        switch (e.type) {
            case SDL_QUIT:
                app.running = 0;
                break;

            case SDL_KEYDOWN:
                switch (e.key.keysym.sym) {
                    case SDLK_ESCAPE: app.running = 0;                        break;
                    case SDLK_SPACE:  app.paused ^= 1;                        break;
                    case SDLK_r:      grid_randomise(app.grid, 0.3f);         break;
                    case SDLK_c:      grid_clear(app.grid);                   break;
                    case SDLK_g:      app.rend->show_grid ^= 1;               break;
                    case SDLK_w:
                        app.grid->wrap ^= 1;
                        printf("wrap: %s\n", app.grid->wrap ? "on" : "off");
                        break;
                    case SDLK_RIGHT:
                        if (app.paused) grid_step(app.grid);
                        break;
                    case SDLK_PLUS:
                    case SDLK_EQUALS:
                        app.sps = (app.sps < 60) ? app.sps + 1 : 60;
                        printf("sps: %d\n", app.sps);
                        break;
                    case SDLK_MINUS:
                        app.sps = (app.sps > 1) ? app.sps - 1 : 1;
                        printf("sps: %d\n", app.sps);
                        break;
                    case SDLK_1: grid_clear(app.grid); pattern_load(app.grid, PATTERN_GLIDER,     -1,-1); break;
                    case SDLK_2: grid_clear(app.grid); pattern_load(app.grid, PATTERN_BLINKER,    -1,-1); break;
                    case SDLK_3: grid_clear(app.grid); pattern_load(app.grid, PATTERN_PULSAR,     -1,-1); break;
                    case SDLK_4: grid_clear(app.grid); pattern_load(app.grid, PATTERN_RPENTOMINO, -1,-1); break;
                    case SDLK_5: grid_clear(app.grid); pattern_load(app.grid, PATTERN_DIEHARD,    -1,-1); break;
                    case SDLK_6: grid_clear(app.grid); pattern_load(app.grid, PATTERN_GOSPER_GUN, -1,-1); break;
                }
                break;

            case SDL_MOUSEBUTTONDOWN:
            case SDL_MOUSEMOTION:
                if (e.type == SDL_MOUSEMOTION && !(e.motion.state & SDL_BUTTON_LMASK))
                    break;
                {
                    int mx = (e.type == SDL_MOUSEMOTION) ? e.motion.x : e.button.x;
                    int my = (e.type == SDL_MOUSEMOTION) ? e.motion.y : e.button.y;
                    int cx = mx / CELL_PX;
                    int cy = my / CELL_PX;
                    if (e.type == SDL_MOUSEBUTTONDOWN)
                        grid_toggle(app.grid, cx, cy);
                    else
                        grid_set(app.grid, cx, cy, 1);
                }
                break;
        }
    }
}

static void step_simulation(void) {
    if (app.paused) return;

    Uint32 now      = SDL_GetTicks();
    Uint32 interval = 1000 / app.sps;
    if (now - app.last_step >= interval) {
        grid_step(app.grid);
        app.last_step = now;

        if (app.grid->generation % 50 == 0)
            printf("gen %d  pop %d\n", app.grid->generation, app.grid->population);
    }
}

static void frame(void) {
    handle_events();
    step_simulation();
    renderer_draw(app.rend, app.grid);

#ifndef __EMSCRIPTEN__
    if (!app.running) {
        renderer_destroy(app.rend);
        grid_destroy(app.grid);
        SDL_DestroyRenderer(app.sdl_r);
        SDL_DestroyWindow(app.win);
        SDL_Quit();
        exit(0);
    }
#endif
}

int main(void) {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        fprintf(stderr, "SDL_Init: %s\n", SDL_GetError());
        return 1;
    }

    app.win = SDL_CreateWindow(
        "Conway's Game of Life",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        WINDOW_W, WINDOW_H, 0
    );
    if (!app.win) {
        fprintf(stderr, "SDL_CreateWindow: %s\n", SDL_GetError());
        return 1;
    }

    app.sdl_r = SDL_CreateRenderer(app.win, -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!app.sdl_r) {
        fprintf(stderr, "SDL_CreateRenderer: %s\n", SDL_GetError());
        return 1;
    }

    app.grid      = grid_create(1);
    app.rend      = renderer_create(app.sdl_r);
    app.paused    = 0;
    app.sps       = DEFAULT_SPS;
    app.running   = 1;
    app.last_step = SDL_GetTicks();

    srand((unsigned)time(NULL));
    pattern_load(app.grid, rand() % 6, -1, -1);

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

#ifdef __EMSCRIPTEN__
    emscripten_set_main_loop(frame, 0, 1);
#else
    while (app.running)
        frame();

    renderer_destroy(app.rend);
    grid_destroy(app.grid);
    SDL_DestroyRenderer(app.sdl_r);
    SDL_DestroyWindow(app.win);
    SDL_Quit();
#endif

    return 0;
}
