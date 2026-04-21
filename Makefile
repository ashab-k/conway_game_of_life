CC      = gcc
CFLAGS  = -Wall -Wextra -O2 -I/opt/homebrew/include/SDL2 -D_THREAD_SAFE
LDFLAGS = -L/opt/homebrew/lib -lSDL2
SRC     = main.c grid.c render.c patterns.c
TARGET  = build/gol

EMCC     = emcc
EMFLAGS  = -Wall -O2 -s USE_SDL=2 -s ALLOW_MEMORY_GROWTH=1
EMTARGET = docs/index.html

all: $(TARGET)

$(TARGET): $(SRC)
	mkdir -p build
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET) $(LDFLAGS)

web: $(SRC)
	mkdir -p docs
	$(EMCC) $(EMFLAGS) $(SRC) -o $(EMTARGET)

clean:
	rm -f $(TARGET) docs/index.html docs/index.js docs/index.wasm

.PHONY: all web clean
