terminal_image: src/main.c
	gcc `pkg-config --libs --cflags MagickWand` -lmagic src/main.c -o terminal_image -Wall -Werror -pedantic -std=c99 -O0

release: src/main.c
	gcc `pkg-config --libs --cflags MagickWand` -lmagic src/main.c -o terminal_image -Wall -Werror -pedantic -std=c99 -O3
