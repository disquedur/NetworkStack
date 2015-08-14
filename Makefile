# -Weffc++

all:
	g++ -g -I. -shared -fPIC -lpcap -pthread -std=c++11 -pedantic -Wall -Wextra -Wold-style-cast -Woverloaded-virtual -Wfloat-equal -Wwrite-strings -Wpointer-arith -Wcast-qual -Wcast-align -Wshadow -Wredundant-decls -Wdouble-promotion -Winit-self -Wswitch-default -Wswitch-enum -Wundef -Wlogical-op -Winline -o ./libns.so src/libns.h src/*/*.cpp src/*/*/*.cpp src/*/*/*/*.cpp