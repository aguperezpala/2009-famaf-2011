#include <stdio.h>
#include "../actividad3.h"


/* Nombre del archivo donde estan los valores muestrales */
#define  SAMPLE_FILE	"results/sample.dat"
/* # de valores muestrales */
#define  SAMPLE_SIZE	500


int main (void)
{
	/* Parametros de la normal */
	act3_normal_params (SAMPLE_FILE, SAMPLE_SIZE);
	
	/* Parametros de la normal */
	act3_lnNormal_params (SAMPLE_FILE, SAMPLE_SIZE);
	
	/* Indicadores de la Gamma para calcular sus parametros */
	act3_gamma_params (SAMPLE_FILE, SAMPLE_SIZE);
	
	
	return 0;
}
