#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

void findVar (char * var, char * buff)
{
	if (strstr (buff, var) != NULL)
		/* Encontramos */
		printf ("La cadena %s SE ENCUENTRA\n", var);
	else 
		printf ("La cadena %s NO SE ENCUENTRA\n", var);
}


int main (int argc, char ** argv)
{
	char buff[4096] = { 0 };
	char value[500] = { 0 };
	int c = 0;
	int pos = 0;
	bool begin = false;
	FILE * f1 = NULL, * f2 = NULL;
	
	
	if (argc < 3) {
		printf ("chequer archivo1 archivo2\n donde archivo1 es "
		"el archivo donde se encuentran las cadenas a buscar en \n"
		"el archivo2\n");
		return 1;
	}
	
	f1 = fopen (argv[1], "r");
	f2 = fopen (argv[2], "r");
	
	if (!f1 || !f2) {
		printf ("Error abriendo los archivos\n");
		return 1;
	}
	
	fread (buff, 1, 4096,f2);
	
	printf ("Archivo 1 \n %s \n", buff);
	
	while ((c = getc(f1)) != EOF) {
		if (c == '\'') {
			if (begin) {
				/* es porque termianmos */
				value[pos] = '\0';
				findVar (value, buff);
				pos = 0;
				begin = false;
				
			} else
				/* empezamos a encontrar una var */
				begin = true;
		} else if (begin) {
			value[pos] = c;
			pos++;
		}
	}
	
	fclose (f1);
	fclose (f2);
	return 0;
}
	