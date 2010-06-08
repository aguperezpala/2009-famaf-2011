# MODO DE USO
# Para generar el ejecutable del programa correspondiente al j-esimo problema
# se debe ejecutar por consola el comando:
# make i=j
# El programa a ejecutar por defecto es el del problema #1
# 
# EJEMPLO DE USO
# Supongamos que queremos generar el ejecutable del problema 4.
# Entonces debemos abrir una terminal y ejecutar el siguiente comando:
# make i=4
# Esto generara un archivo ejecutable "p4" que es el programa deseado

CC=gcc
CFLAGS = -ansi -Wall -Wextra -Wdeclaration-after-statement -Wbad-function-cast \
	 -Wcast-qual -Wstrict-prototypes -Wmissing-declarations \
	 -Wmissing-prototypes -Wno-unused-parameter -Werror -pedantic \
	 -g #-D_VERBOSE
LIBRARIES = -lm
RANDOMIZERS=mzran13.c ran2.c
INCLUDES=.depend
i=1

$(i):p$(i)
p$(i): p_$(i).o $(RANDOMIZERS)
	$(CC) $(CFLAGS) $^ $(LIBRARIES) -o $@


normal: normal.o $(RANDOMIZERS)
	$(CC) $(CFLAGS) $^ $(LIBRARIES) -o $@


polar: polar.o $(RANDOMIZERS)
	$(CC) $(CFLAGS) $^ $(LIBRARIES) -o $@


pph: PPh.o $(RANDOMIZERS)
	$(CC) $(CFLAGS) $^ $(LIBRARIES) -o $@


ppnh: PPnh.o $(RANDOMIZERS)
	$(CC) $(CFLAGS) $^ $(LIBRARIES) -o $@
	
p3 :  parcial_2_inciso_3.o $(RANDOMIZERS)
	$(CC) $(CFLAGS) $^ $(LIBRARIES) -o $@
	
p4 :  parcial_2_inciso_4.o $(RANDOMIZERS)
	$(CC) $(CFLAGS) $^ $(LIBRARIES) -o $@
	


clean:
	rm -f $(TARGET) .depend *.o p? *~ pph polar normal

.depend: *.[ch]
	$(CC) -MM $(SOURCES) >.depend

-include $(INCLUDES)

.PHONY: clean all
