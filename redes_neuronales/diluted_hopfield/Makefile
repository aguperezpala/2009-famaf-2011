DIL=diluted_Hopfield
TEST=test
CC=gcc

# For debugging include flag:	-D_DEBUG
# Otherwise include flag:	-DNDEBUG
CFLAGS = -Wall -ansi -pedantic -DNDEBUG \
	 -O3 -fopenmp -g -std=c99 -D_XOPEN_SOURCE=600 -Wmissing-prototypes \
	 -Wmissing-declarations -Winline -Wcast-qual -Wstrict-prototypes \
	 -Wdeclaration-after-statement -Wno-unused-parameter -Werror

LIBRARIES = -lm
INCLUDES = .depend

all: help
help:
	echo "\n\nElija una opción 'OP' de compilación e ingrese \"make OP\"\n"\
		"\rOpciones de compilación: dil\n\t\t\t test\n"

dil: $(DIL)
$(DIL): mzran13.o ssv.o sdhn.o Hopfield_diluido.o
	$(CC) $(CFLAGS) $^ $(LIBRARIES) -o $@; \
	echo "\n\nPara ejecutar el programa ingrese \"./$(DIL)\"\n"

test: $(TEST)
$(TEST): mzran13.o sdhn.o test.o
	$(CC) $(CFLAGS) $^ $(LIBRARIES) -o $@; \
	echo "\n\nPara ejecutar el programa ingrese \"./$(TEST)\"\n"

clean:
	rm -f .depend $(DIL) $(TEST) *.o *~

.depend: *.[ch]
	$(CC) -MM $(SOURCES) >.depend

-include .depend

.PHONY: clean all
