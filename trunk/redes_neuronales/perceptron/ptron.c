/*
 * Neural Networks - Multi-layer perceptron
 * Network ADT
 *
 * $ Author: Carlos E. Budde $
 * $ Date: 09/12/2010 11:35 $
 * $ License: GPL v3 $
 *
 */


#include <stdlib.h>
#include <assert.h>

#include "ptron.h"

struct _ptron_s {
	int A;			/* # of layers (counting input & output) */
	int *N;			/* # of neurons per layer*/
	double **V;		/* neurons */
	double ***w;		/* sinaptic weights matrix */
	double (*g) (double);	/* sinaptic update function */
	double ***dw;		/* for back-propagation updates */
};
