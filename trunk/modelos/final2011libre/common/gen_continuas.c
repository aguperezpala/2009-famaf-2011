#include "gen_continuas.h"


/******************************************************************************/
double rg_gen_uni_cont(double a, double b)
{
	double result = (double)mzran13()/(double)ULONG_MAX;
	
	result = result * (b-a) + a;
	
	return result;
}