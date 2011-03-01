#include <stdlib.h>
#include <stdio.h>
#include <math.h>



double  _a   = 0.0,	/* Estimation lower bound */
	_b   = 0.0,	/* Estimation upper bound */
	_h   = 0.1,	/* Time step size */
	_tau = 17.0,	/* Time delay for the differential equation */
	_ya  = 1.2;	/* Series first point value */

double *y = NULL;	/* Results array */




/* # of places to overlook when printing */
#define  MARGIN   0
/* Initial values before the first point */
#define  _y0	 0.0

/* Little trick to access results array correctly */
#define  time(t)	lround ((t) * (1.0/_h))




/* Runge-Kutta method for DE series estimation
 *
 * PARAMETERS:	h -------> time step size
 *		t -------> previous time value
 *		y_t -----> function's value in time 't'
 *		f(t,y) --> differential equation
 *
 * POS: returned value is the RK approximation
 *	to the funtion's value in time t+h
 */
static double RungeKutta (double t, double y_t, double h,
			  double (*f) (double, double))
{
	double	K1 = 0.0, K2 = 0.0, K3 = 0.0, K4 = 0.0;
	
	K1 = f ( t         , y_t            );
	K2 = f ( t + 0.5*h , y_t + 0.5*h*K1 );
	K3 = f ( t + 0.5*h , y_t + 0.5*h*K2 );
	K4 = f ( t + h     , y_t + h*K3     );
	
	return y_t + (h/6.0) * (K1 + 2.0*K2 + 2.0*K3 + K4);
}



/* Mackey-Glass differential delay equation */
static double f (double t, double y_t)
{
	double y_tau = y [time(t)-time(_tau)];
	
	return ((0.2 * y_tau) / (1.0 + pow (y_tau, 10.0))) - 0.1 * y_t;
}






static void
parse_input (int argc, char **argv,
	     double *a, double *b, double *h, double *tau, double *ya)
{
	char *err = NULL;
	
	if (argc != 6) {
		fprintf (stderr, "Must be called with following arguments:\n"
				 "\t1) Lower estimation bound\n"
				 "\t2) Upper estimation bound\n"
				 "\t3) Time step size\n"
				 "\t4) Delay size (smaller than arg #1)\n"
				 "\t5) Series first value, ie: y(a)\n");
		exit (EXIT_FAILURE);
	}
	
	/* Retrieving estimation bounds */
	*a = strtod (argv[1], &err);
	if (err[0] != '\0') {
		fprintf (stderr, "Error at '%s' while parsing input\n"
				 "Must give lower bound of estimation "
				 "as first argument\n", err);
		exit (EXIT_FAILURE);
	}
	
	*b = strtod (argv[2], &err);
	if (err[0] != '\0') {
		fprintf (stderr, "Error at '%s' while parsing input\n"
				 "Must give upper bound of estimation "
				 "as second argument\n", err);
		exit (EXIT_FAILURE);
	}
	
	*h = strtod (argv[3], &err);
	if (err[0] != '\0') {
		fprintf (stderr, "Error at '%s' while parsing input\nMust give "
				 "time step size as third argument\n", err);
		exit (EXIT_FAILURE);
	}
	
	*tau = strtod (argv[4], &err);
	if (err[0] != '\0') {
		fprintf (stderr, "Error at '%s' while parsing input\nMust give "
				 "delay size as fourth argument\n", err);
		exit (EXIT_FAILURE);
	} else if (*tau > *a) {
		fprintf (stderr, "Delay size must be smaller than arg #1\n");
		exit (EXIT_FAILURE);
	}
	
	*ya = strtod (argv[5], &err);
	if (err[0] != '\0') {
		fprintf (stderr, "Error at '%s' while parsing input\nMust give "
				 "series first value as fifth argument\n", err);
		exit (EXIT_FAILURE);
	}
	
	if (*a > *b) {	/* swap values */
		*a += *b;
		*b  = *a - *b;
		*a -= *b;
	}
	
	return;
}



int main (int argc, char **argv)
{
	int i = 0;
	size_t  a = 0,    b = 0;
	double  t = 0.0, new_val = 0.0;
	
	
	parse_input (argc, argv, &_a, &_b, &_h, &_tau, &_ya);
	
	
	/* Creating work environment  */
	a = time(_a);
	b = time(_b + MARGIN);
	
	y = (double *) calloc (b, sizeof (double));
	if (y == NULL) {
		perror ("Mackey-Glass: insufficient memory to execute");
		exit (EXIT_FAILURE);
	}
	
	/* Setting initial values */
	for (i=0 ; i < a ; i++) {
		y[i] = _y0;
	}
	y[a] = _ya;
	
	
	/* Estimating series */
	t = _a;
	while (t < _b + MARGIN) {
		new_val = RungeKutta (t, y[time(t)], _h, f);
		t += _h;
		y[time(t)] = new_val;
	}
	
	/* Printing results */
	t = _a + MARGIN;
	a += time(MARGIN);
	for (i = a ; i < b ; i++) {
		if (!(i % ((int)(1.0/_h))))
			printf ("%.1f\t%f\n", t, y[time(t)]);
		t += _h;
	}
	
	free (y);
	y = NULL;
	
	return EXIT_SUCCESS;
}
