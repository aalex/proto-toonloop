proto: main.cpp
	g++ -Wall -g main.cpp -o proto `pkg-config clutter-1.0 --cflags --libs`
clean:
	rm -f proto *.o

