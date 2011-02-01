all: dv

dv: dv.cpp
	$(CXX) $^ -o $@ `pkg-config --cflags --libs gstreamer-0.10 gtk+-2.0`
