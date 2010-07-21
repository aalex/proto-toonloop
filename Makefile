all: dummy

dummy: dummy.c
	gcc -Wall `pkg-config --libs --cflags jack` -I/usr/include -o dummy dummy.c
