
LIBS= -lpthread -lm
#INCLUDE_DIRS = -I$(HOME)/local_arb/include -I$(HOME)/local_flint/include
#LIB_DIRS = -L$(HOME)/local_arb/lib -L$(HOME)/local_flint/lib

.PHONY: all

all: newton

newton: newton.o
	gcc -o $@ -O2 newton.o $(LIBS) 
newton.o: newton.c
	gcc -c -O2 newton.c
clean:
	rm -f *.o newton *.ppm 
