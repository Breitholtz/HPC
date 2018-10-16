.PHONY: all

all: cell_distance

cell_distance: cell_distance.o
	gcc -O2 -o $@ cell_distance.o -lgomp

cell_distance.o: cell_distance.c
	gcc -O2 -fopenmp -c cell_distance.c

clean:
	rm -f *.o cell_distance
