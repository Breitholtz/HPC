
LIBS= -larb -lflint -lmpfr -lpthread -lm
INCLUDE_DIRS = -I$(HOME)/local_arb/include -I$(HOME)/local_flint/include
LIB_DIRS = -L$(HOME)/local_arb/lib -L$(HOME)/local_flint/lib

.PHONY: all

all: newton

newton: newton.o
	gcc -o $@ -O2 newton.o $(LIB_DIRS) $(LIBS) $(INCLUDE_DIRS)
newton.o: newton.c
	gcc -c -O2 newton.c $(INCLUDE_DIRS)
clean:
	rm -f *.o newton *.ppm
