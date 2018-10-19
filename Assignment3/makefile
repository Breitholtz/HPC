.PHONY: all

all: cell_distance

cell_distance: cell_distance.o
	gcc -O2 -o $@ cell_distance.o -lgomp -lm

cell_distance.o: cell_distance.c
	gcc -O2 -fopenmp -c cell_distance.c -lm

clean:
	rm -f *.o cell_distance
