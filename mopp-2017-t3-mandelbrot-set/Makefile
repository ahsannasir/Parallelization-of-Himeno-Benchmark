FLAGS=-O3 -std=c++11

RM=rm -f

EXEC=mandelbrot

all: $(EXEC)

$(EXEC):
	$(CXX) $(FLAGS) $(EXEC).cpp -c -o $(EXEC).o -lpthread
	$(CXX) $(FLAGS) $(EXEC).o -o $(EXEC) -lpthread

clean:
	$(RM) $(EXEC).o $(EXEC)
