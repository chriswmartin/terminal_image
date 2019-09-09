TARGET = terminal_image

SRC = src/main.c

LDFLAGS = `pkg-config --libs --cflags MagickWand` -lmagic -L /usr/local/lib

OPTIMIZATION_LEVEL = 3

CFLAGS = -Wall -Werror -pedantic -std=c99 -lm -I /usr/local/include -O$(OPTIMIZATION_LEVEL)

$(TARGET): $(SRC)
	$(CC) -o $@ $(LDFLAGS) $^ $(CFLAGS)
