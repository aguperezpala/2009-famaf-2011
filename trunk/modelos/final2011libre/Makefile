TARGET=main
CC=gcc
CFLAGS = -ansi -Wall -Wextra -Wdeclaration-after-statement -Wbad-function-cast \
	 -Wcast-qual -Wstrict-prototypes -Wmissing-declarations \
	 -Wmissing-prototypes -Wno-unused-parameter -Werror -pedantic -g
LIBRARIES = -lm
SOURCES=$(shell echo *.c)
OBJECTS=$(SOURCES:.c=.o)
INCLUDES=.depend

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $^ $(LIBRARIES) -o $@

clean:
	rm -f $(TARGET) $(OBJECTS) .depend *~

.depend: *.[ch]
	$(CC) -MM $(SOURCES) >.depend


-include $(INCLUDES)

.PHONY: clean all
