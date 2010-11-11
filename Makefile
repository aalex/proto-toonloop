all: run

run: main.cpp property.h
	g++ -Wall -Werror -o run main.cpp

clean:
	rm run
