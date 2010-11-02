all: run

run: main.c
	gcc -Wall -Werror --std=gnu99 --pedantic `pkg-config glib-2.0 --cflags --libs` -o run main.c
