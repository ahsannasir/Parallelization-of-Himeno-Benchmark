FLAGS=-O3 -std=c++11

RM=rm -f

all: mandelbrot-set

mandelbrot-set:
	$(CXX) $(FLAGS) mandelbrot.cpp -o mandelbrot-set -lpthread

clean:
	$(RM) mandelbrot-set
