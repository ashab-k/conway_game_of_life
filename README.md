# Conway's Game of Life

A terminal-free, interactive implementation of [Conway's Game of Life](https://en.wikipedia.org/wiki/Conway%27s_Game_of_Life) written in C with SDL2. Features color trails, preset patterns, toroidal wrapping, and live mouse drawing.


## Features

- Color trail system — cell color reflects how long it has been alive
- 6 built-in patterns — Glider, Blinker, Pulsar, R-pentomino, Diehard, Gosper Glider Gun
- Toroidal wrapping — toggle whether edges connect to each other
- Mouse drawing — click or drag to paint cells live
- Variable speed — adjust simulation steps per second at runtime
- Step mode — advance one generation at a time while paused

---

## Requirements

- C compiler (gcc or clang)
- SDL2

---

## Building

### macOS (Homebrew)

```bash
brew install sdl2
make
./build/gol
```

### Linux (apt)

```bash
sudo apt install libsdl2-dev
make
./build/gol
```

---

## Controls

| Key | Action |
|-----|--------|
| `Space` | Pause / resume |
| `→` | Step one generation (while paused) |
| `R` | Randomise grid |
| `C` | Clear grid |
| `G` | Toggle gridlines |
| `W` | Toggle toroidal wrapping |
| `1` | Load Glider |
| `2` | Load Blinker |
| `3` | Load Pulsar |
| `4` | Load R-pentomino |
| `5` | Load Diehard |
| `6` | Load Gosper Glider Gun |
| `+` / `-` | Speed up / slow down |
| Left click | Toggle cell |
| Left drag | Paint cells alive |
| `Esc` | Quit |

---

## How It Works

The grid is a flat `ROWS * COLS` array of cells. Each step:

1. For every cell, count its 8 neighbours (Moore neighbourhood)
2. Apply B3/S23 rules — a cell is alive next generation if it has exactly 3 neighbours, or is currently alive with 2 or 3
3. Write results into a second buffer, then swap — this is the double-buffer pattern that ensures every cell sees the same generation's state during computation

Each cell also tracks an `age` counter (consecutive generations alive), which drives the color trail in the renderer.

---


## Rules

Conway's Game of Life follows four rules, which collapse into one condition:

> A cell is alive next generation if and only if it has exactly 3 neighbours, **or** it is currently alive with exactly 2 neighbours.

Everything else dies or stays dead.

---

## License

MIT
