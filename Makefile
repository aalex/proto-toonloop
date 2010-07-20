all: hello
	@echo DONE
hello: main.c
	gcc -Wall -Werror `pkg-config --cflags --libs freealut` -o hello main.c 

