TARGET=simulate
CC=gcc
CFLAGS = -ansi -Wall -Wextra -Wdeclaration-after-statement -Wbad-function-cast \
	 -Wcast-qual -Wstrict-prototypes -Wmissing-declarations \
	 -Wmissing-prototypes -Wno-unused-parameter -Werror -pedantic \
	 -g --std=c99 #-D_DEBUG 
LIBS = -lm
SOURCES = $(shell echo *.c)
OBJECTS = $(SOURCES:%.c=%.o)
INCLUDES=.depend

all: $(TARGET)

$(TARGET): $(OBJECTS) $(BSTRING_OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)


3: estim_param
estim_param: file_reader.o actividad2.o actividad3.o results/estim_param.o
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

4: calc_pval
calc_pval: file_reader.o rdg.o ssv.o distrib.o actividad4.o results/calc_pvalues.o
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

clean:
	rm -f $(TARGET) .depend *.o calc_pval estim_param *~ \
		results/*.o results/*~ alias

.depend: *.[ch]
	$(CC) -MM $(SOURCES) >.depend

-include $(INCLUDES)

.PHONY: clean all
