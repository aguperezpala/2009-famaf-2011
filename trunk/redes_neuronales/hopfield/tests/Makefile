TEST=test
CC=gcc
CFLAGS = -O3 -fopenmp -g -std=c99 -D_XOPEN_SOURCE=600 \
	 -Wall -ansi -pedantic -Wmissing-declarations \
	 -Wcast-qual -Wstrict-prototypes -Wdeclaration-after-statement \
	 -Wmissing-prototypes -Wno-unused-parameter -Werror

INCLUDES=.depend

all: test2

test1: ../mzran13.o test1.o
	$(CC) $(CFLAGS) $^ -o $@
	./test1 ; rm test1 test1.o ; echo "\nFin de las pruebas"

test2: ../mzran13.o test2.o
	$(CC) $(CFLAGS) $^ -o $@
	./test2 ; rm test2 test2.o ; echo "\nFin de las pruebas"


clean:
	rm -f .depend test1 test2 *.o *~

.depend: *.[ch]
	$(CC) -MM $(SOURCES) >.depend

-include .depend

.PHONY: clean all
