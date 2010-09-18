TEST=test
CC=gcc
CFLAGS = -Wall -ansi -pedantic \
	 -O3 -fopenmp -g -std=c99 -D_XOPEN_SOURCE=600 -Wmissing-prototypes \
	 -Wmissing-declarations -Winline -Wcast-qual -Wstrict-prototypes \
	 -Wdeclaration-after-statement -Wno-unused-parameter -Werror

INCLUDES=.depend

all: test2

test1: ../mzran13.o test1.o
	$(CC) $(CFLAGS) $^ -o $@
	echo "\nPrueba de velocidades de aleatorizacion" ; \
	./test1 ; echo "\nFin de la prueba"
	
test2: ../mzran13.o test2.o
	$(CC) $(CFLAGS) $^ -o $@
	echo "\nPrueba de velocidades: long vs (64)bit network-state 'S'\n" ; \
	./test2 ; echo "\nFin de la prueba"
	
test3: ../mzran13.o test3.o
	$(CC) $(CFLAGS) $^ -o $@
	echo "\nPrueba de operacion bitxbit\n" ; \
	./test3 ; echo "\nFin de la prueba"
	
clean:
	rm -f .depend test? *.o *~

.depend: *.[ch]
	$(CC) -MM $(SOURCES) >.depend

-include .depend

.PHONY: clean all
