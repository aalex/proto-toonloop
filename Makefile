all: run check_glist

run: main.cpp
	g++ -Wall -Werror `pkg-config glib-2.0 --cflags --libs` -o run main.cpp

check_glist: check_glist.cpp
	g++ -Wall -Werror `pkg-config glib-2.0 --cflags --libs` -o check_glist check_glist.cpp
