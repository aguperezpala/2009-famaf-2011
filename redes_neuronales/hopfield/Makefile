DET=deterministic_Hopfield
STOC=stochastic_Hopfield
SMPL=example_Hopfield

CC=gcc
CFLAGS = -O3 -fopenmp -g -std=c99 -D_XOPEN_SOURCE=600 \
	 -Wall -ansi -pedantic -Wmissing-declarations \
	 -Wcast-qual -Wstrict-prototypes -Wdeclaration-after-statement \
	 -Wmissing-prototypes -Wno-unused-parameter -Werror
#SOURCES=$(shell echo *.c)
#OBJECTS=$(SOURCES:.c=.o)

LIBRARIES = -lm
INCLUDES=.depend

all:$(STOC)

example:$(SMPL)
$(SMPL): Hopfield_example.o mzran13.o
	$(CC) $(CFLAGS) $^ $(LIBRARIES) -o $@

det:$(DET)
$(DET): Hopfield_deterministico.o mzran13.o
	$(CC) $(CFLAGS) $^ -o $@

stoc:$(STOC)
$(STOC): Hopfield_estocastico.o mzran13.o
	$(CC) $(CFLAGS) $^ -o $@

test:
	make -C tests test2

clean:
	rm -f .depend $(TARGET) $(DET) $(STOC) $(SMPL) *.o *~
	make -C tests clean

.depend: *.[ch]
	$(CC) -MM $(SOURCES) >.depend

-include .depend

.PHONY: clean all
