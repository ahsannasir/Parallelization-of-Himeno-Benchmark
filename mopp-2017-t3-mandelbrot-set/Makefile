FLAGS=-O3 -std=c++11

RM=rm -f

all: mandelbrot

mandelbrot-set:
	$(CXX) $(FLAGS) mandelbrot.cpp -o mandelbrot-set -pthread

clean:
	$(RM) mandelbrot
