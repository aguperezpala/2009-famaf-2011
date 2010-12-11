P1=parity
P2=functions
P3=encoding
TEST=test
CC=gcc

# For debugging include flag:	-D_DEBUG
# Otherwise include flag:	-DNDEBUG
CFLAGS = -Wall -ansi -pedantic -D_DEBUG \
	 -fopenmp -g -std=c99 -D_XOPEN_SOURCE=600 -Wmissing-prototypes \
	 -Wmissing-declarations -Winline -Wcast-qual -Wstrict-prototypes \
	 -Wdeclaration-after-statement -Wno-unused-parameter -Werror

LIBRARIES = -lm
INCLUDES = .depend

all: help
help:
	echo "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"\
	"\n\nElija una opción de compilación e ingrese \"make <opción>\"\n"\
	"\rOpciones de compilación: p1\n\t\t\t p2\n\t\t\t p3\n\t\t\t test\n"

# dil: $(DIL)
# $(DIL): mzran13.o ssv.o sdhn.o Hopfield_diluido.o
# 	$(CC) $(CFLAGS) $^ $(LIBRARIES) -o $@; \
# 	echo "\n\nPara ejecutar el programa ingrese \"./$(DIL)\"\n"

test: $(TEST)
$(TEST): mzran13.o ptron.o test.o
	$(CC) $(CFLAGS) $^ $(LIBRARIES) -o $@; \
	echo "\n\nPara ejecutar el programa ingrese \"./$(TEST)\"\n"

clean:
	rm -f .depend $(P1) $(P2) $(P3) $(TEST) *.o *~

.depend: *.[ch]
	$(CC) -MM $(SOURCES) >.depend

-include .depend

.PHONY: clean all
