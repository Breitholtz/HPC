.PHONY: all

all: cell_distance cell_distance2

cell_distance: cell_distance.o
	gcc -O2 -o $@ cell_distance.o -lgomp -lm

cell_distance.o: cell_distance.c
	gcc -O2 -fopenmp -c cell_distance.c -lm

#cell_distance2: cell_distance_alt.o
#	gcc -O2 -o $@ cell_distance_alt.o -lgomp -lm

#cell_distance_alt.o: cell_distance_alt.c
#	gcc -O2 -fopenmp -c cell_distance_alt.c -lm

clean:
	rm -f *.o cell_distance
