TARGET=simulate
CC=gcc
CFLAGS = -ansi -Wall -Wextra -Wdeclaration-after-statement -Wbad-function-cast \
	 -Wcast-qual -Wstrict-prototypes -Wmissing-declarations \
	 -Wmissing-prototypes -Wno-unused-parameter -Werror -pedantic \
	 -g #-D_VERBOSE
LIBS = -lm
SOURCES = $(shell echo *.c)
OBJECTS = $(SOURCES:%.c=%.o)
INCLUDES=.depend

all: $(TARGET)

$(TARGET): $(OBJECTS) $(BSTRING_OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

clean:
	rm -f $(TARGET) .depend *.o p? *~ alias

.depend: *.[ch]
	$(CC) -MM $(SOURCES) >.depend

-include $(INCLUDES)

.PHONY: clean all
