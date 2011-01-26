#-ansi -Wall -Wextra -Wdeclaration-after-statement -Wbad-function-cast 
#	 -Wcast-qual -Wstrict-prototypes -Wmissing-declarations 
#	 -Wmissing-prototypes -Wno-unused-parameter -Werror -pedantic -g 
TARGET=main
CC=gcc
CFLAGS = -I../../common 
LIBRARIES = -lm
SOURCES=$(shell echo *.c ../../common/*.c)
OBJECTS=$(SOURCES:.c=.o)
INCLUDES=.depend

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) $(LIBRARIES) $^ -o $@

clean:
	rm -f $(TARGET) $(OBJECTS) .depend *~

.depend: *.[ch]
	$(CC) $(CFLAGS) $(LIBRARIES) -MM $(SOURCES) >.depend


-include $(INCLUDES)

.PHONY: clean all
