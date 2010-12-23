TARGET=mg
TEST=test
CC=gcc

# For debugging include flag:	-D_DEBUG
# Otherwise include flag:	-DNDEBUG
CFLAGS = -Wall -ansi -pedantic -D_DEBUG \
	 -O3 -fopenmp -g -std=c99 -D_XOPEN_SOURCE=600 -Wmissing-prototypes \
	 -Wmissing-declarations -Winline -Wcast-qual -Wstrict-prototypes \
	 -Wdeclaration-after-statement -Wno-unused-parameter -Werror

LIBRARIES = -lgsl -lgslcblas -lm
INCLUDES = .depend

all: help
help:
	echo "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"\
	"\n\nElija una opción de compilación e ingrese \"make <opción>\"\n"\
	"\rOpciones de compilación: mg\n\t\t\t test\n"


test: $(TEST)
$(TEST): mzran13.o anfis.o test.o
	$(CC) $(CFLAGS) $^ $(LIBRARIES) -o $@; \
	echo "\n\nPara ejecutar el programa ingrese \"./$(TEST)\"\n"


mg: $(TARGET)
$(TARGET): mzran13.o anfis.o
	$(CC) $(CFLAGS) $^ $(LIBRARIES) -o $@; \
	echo "\n\nPara ejecutar la simulación de Mackey-Glass " \
 		"ingrese \"./$(TARGET)\"\n"

clean:
	rm -f .depend $(TEST) $(TARGET) *.o *~

.depend: *.[ch]
	$(CC) -MM $(SOURCES) >.depend

-include .depend

.PHONY: clean all
