#ifndef DISTRIB_H
#define DISTRIB_H

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

/*! para gcf */
/* Maximum allowed number of iterations. */
#define ITMAX 100
/* Relative accuracy. */
#define EPS 3.0e-7
/* Number near the smallest representable */
#define FPMIN 1.0e-30


/* generadora de chi-cuadrada */
double chi_cuadrada(int gradosLibertad, double value);








#endif
