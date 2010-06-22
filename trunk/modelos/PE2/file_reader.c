#include "file_reader.h"


/* Funcion que lee un archivo de texto y los carga en un arreglo de tamaño
* fijo.
* REQUIRES:
* 	n	size of array
* 	arr	array != NULL
* 	fname	fileName != NULL
* RETURNS:
* 	< 0 		on error
* 	lines readed	if success
*/
int read_double_file(double *arr, int size, const char *fname)
{
	int result = 0;
	FILE *file = NULL;
	int i = 0;
	
	assert(fname != NULL);
	assert(arr != NULL);
	
	file = fopen(fname, "r");
	if (file == NULL)
		return -1;
	
	result = fscanf(file, "%lf\n", &arr[i]);
	while (result > 0 && result != EOF && i < size) {
		i++;
		result = fscanf(file, "%lf\n", &arr[i]);
	}
	
	
	fclose(file);
	
	return i;
}
