CC=gcc
# For debugging include flag:	-D_DEBUG
# Otherwise include flag:	-DNDEBUG
CFLAGS = -Wall -ansi -pedantic -D_DEBUG \
	 -O3 -fopenmp -g -std=c99 -D_XOPEN_SOURCE=600 -Wmissing-prototypes \
	 -Wmissing-declarations -Winline -Wcast-qual -Wstrict-prototypes \
	 -Wdeclaration-after-statement -Wno-unused-parameter -Werror
#SOURCES=$(shell echo *.c)
#OBJECTS=$(SOURCES:.c=.o)

LIBRARIES = -lm
INCLUDES=.depend

all: help
help:
	echo "\n\nChoose a compiling option 'OP' and then type \"make OP\"\n"\
		"\rCompiling options:\n\t\t   example\n\t\t   det\n\t\t   stoc"\
		"\n\t\t   test1\n\t\t   test2\n\t\t   test3\n"


test: mzran13.o sdhn.o test.o
	$(CC) $(CFLAGS) $^ $(LIBRARIES) -o $@;

clean:
	rm -f .depend test *.o *~

.depend: *.[ch]
	$(CC) -MM $(SOURCES) >.depend

-include .depend

.PHONY: clean all
