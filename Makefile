all: run

run: main.cpp
	g++ -Wall -Werror `pkg-config glib-2.0 --cflags --libs` -o run main.cpp
