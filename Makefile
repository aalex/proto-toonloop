all: run

run: main.c
	gcc -Wall -Werror --pedantic --std=gnu99 `pkg-config --cflags --libs clutter-1.0` -o run main.c
