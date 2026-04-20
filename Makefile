CC      = gcc
CFLAGS  = -Wall -Wextra -O2 -I/opt/homebrew/include/SDL2 -D_THREAD_SAFE
LDFLAGS = -L/opt/homebrew/lib -lSDL2
SRC     = main.c grid.c render.c patterns.c
TARGET  = build/gol

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET) $(LDFLAGS)

clean:
	rm -f $(TARGET)

.PHONY: all clean

