TARGET = terminal_image

SRC = src/main.c

LDFLAGS = `pkg-config --libs --cflags MagickWand` -lmagic

OPTIMIZATION_LEVEL = 3

CFLAGS = -Wall -Werror -pedantic -std=c99 -lm -O$(OPTIMIZATION_LEVEL)

$(TARGET): $(SRC)
	$(CC) -o $@ $(LDFLAGS) $^ $(CFLAGS)
