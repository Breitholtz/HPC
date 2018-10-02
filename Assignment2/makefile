.PHONY: all

all: newton

newton: newton.o
	gcc -o $@ -O0 newton.o
newton.o: newton.c
	gcc -c -O0 newton.c
clean:
	rm -f *.o; rm -f ./newton
