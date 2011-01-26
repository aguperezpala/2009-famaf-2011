TARGET = anfis_mg
SAMPLE = Mackey_Glass
GTEST  = gen_test
OTEST  = op_test
CC=gcc

# For debugging include flag:	-D_DEBUG
# Otherwise include flag:	-DNDEBUG
CFLAGS = -Wall -ansi -pedantic -D_DEBUG -D_BSD_SOURCE \
	 -O3 -fopenmp -g -std=c99 -D_XOPEN_SOURCE=600 -Wmissing-prototypes \
	 -Wmissing-declarations -Winline -Wcast-qual -Wstrict-prototypes \
	 -Wdeclaration-after-statement -Wno-unused-parameter -Werror

LIBRARIES = -lgsl -lgslcblas -lm
INCLUDES = .depend

all: help
help:
	echo "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"\
	"\n\nElija una opción de compilación e ingrese \"make <opción>\"\n"\
	"\rOpciones de compilación: anfis_mg\n\t\t\t mg_sample\n\t\t\t"\
	" gen_test   (test muy general, pruebas ligeras)\n\t\t\t"\
 	"op_test    (test pesado, prueba la dinámica de red)\n\n"


gen_test: $(GTEST)
$(GTEST): mzran13.o anfis.o gen_test.o
	$(CC) $(CFLAGS) $^ $(LIBRARIES) -o $@; \
	echo "\n\nPara ejecutar el programa ingrese \"./$(GTEST)\"\n"


op_test: $(OTEST)
$(OTEST): mzran13.o anfis.o op_test.o
	$(CC) $(CFLAGS) $^ $(LIBRARIES) -o $@; \
	echo "\n\nPara ejecutar el programa ingrese \"./$(OTEST)\"\n"


mg_sample: $(SAMPLE)
$(SAMPLE): Mackey_Glass.o
	$(CC) $(CFLAGS) $^ $(LIBRARIES) -o $@; \
	echo "\n\nPara ejecutar el programa ingrese \"./$(SAMPLE)\"\n"


anfis_mg: $(TARGET)
$(TARGET): mzran13.o anfis.o main.o
	$(CC) $(CFLAGS) $^ $(LIBRARIES) -o $@; \
	echo "\n\nPara ejecutar el aprendizaje ANFIS de Mackey-Glass " \
		"ingrese \"./$(TARGET)\"\n"


clean:
	rm -f .depend $(GTEST) $(OTEST) $(SAMPLE) $(TARGET) *.o *~

deep_clean:
	make clean && rm -f anfis.log *.dat *.png

.depend: *.[ch]
	$(CC) -MM $(SOURCES) >.depend

-include .depend

.PHONY: clean all
