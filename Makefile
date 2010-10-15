main: main.cpp
	g++ -Wall -g main.cpp -o main `pkg-config clutter-1.0 --cflags --libs`
clean:
	rm main *.o

