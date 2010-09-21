DET=deterministic_Hopfield
STOC=stochastic_Hopfield
SMPL=example_Hopfield

CC=gcc
CFLAGS = -Wall -ansi -pedantic \
	 -O3 -fopenmp -g -std=c99 -D_XOPEN_SOURCE=600 -Wmissing-prototypes \
	 -Wmissing-declarations -Winline -Wcast-qual -Wstrict-prototypes \
	 -Wdeclaration-after-statement -Wno-unused-parameter -Werror
#SOURCES=$(shell echo *.c)
#OBJECTS=$(SOURCES:.c=.o)

LIBRARIES = -lm
INCLUDES=.depend

all:help
help:
	echo "\n\nChoose a compiling option 'OP' and then type \"make OP\"\n"\
		"\rCompiling options:\n\t\t   example\n\t\t   det\n\t\t   stoc"\
		"\n\t\t   test1\n\t\t   test2\n\t\t   test3\n"

example:$(SMPL)
$(SMPL): Hopfield_example.o mzran13.o
	$(CC) $(CFLAGS) $^ $(LIBRARIES) -o $@; \
	echo "\n\nTo execute program type command \"./$(SMPL)\" and press ENTER\n"

det:$(DET)
$(DET): Hopfield_deterministico.o mzran13.o ssv.o ndf.o
	$(CC) $(CFLAGS) $^ $(LIBRARIES) -o $@ ; \
	echo "\n\nTo execute program type command \"./$(DET)\" and press ENTER\n"

stoc:$(STOC)
$(STOC): Hopfield_estocastico.o mzran13.o ssv.o ndf.o
	$(CC) $(CFLAGS) $^ $(LIBRARIES) -o $@; \
	echo "\n\nTo execute program type command \"./$(STOC)\" and press ENTER\n"

test1:
	make -C tests test1

test2:test
test:
	make -C tests test2

test3:
	make -C tests test3
	

clean:
	rm -f .depend $(TARGET) $(DET) $(STOC) $(SMPL) *.o *~
	make -C tests clean

.depend: *.[ch]
	$(CC) -MM $(SOURCES) >.depend

-include .depend

.PHONY: clean all
