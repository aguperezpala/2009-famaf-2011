#include "analisis_estadistico.h"


/******************************************************************************/
double estimar_media_muestral(int j, double Xpromj, double Xj1)
{
	return (Xpromj + (Xj1 - Xpromj)/(j+1));
}

/******************************************************************************/
double estimar_varianza_muestral(int j, double Sj, double Xpromj1, double Xpromj)
{
	return (((j - 1.0)/(double)j)*Sj + ((double)(j+1.0))*pow(Xpromj1-Xpromj,2));
}
/******************************************************************************/

/******************************************************************************/








