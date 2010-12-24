#include <stdlib.h>
#include <stdio.h>
#include <math.h>


/* Estimation interval bounds */
#define  _a	 118
#define  _b	1118
/* # of places to overlook when printing */
#define  margin   50
/* Time step size */
#define  _h	 0.1
/* Time delay for the differential equation */
#define  _tau	  17
/* Initial values */
#define  _y0	 0.0	/* Before the first point */
#define  _ya	 1.2	/* First point */

/* Results array */
double *y = NULL;
/* Little trick to access results array correctly */
#define  time(t)	lround (t * (1.0/_h))




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



int main (void)
{
	int i = 0;
	double t = 0.0, new_val = 0.0;
	size_t a = 0, b = 0;
	
	/* Creating work environment  */
	a = lround (ceil (_a * (1.0/_h)));
	b = lround (ceil (_b * (1.0/_h)));
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
	while (t < _b) {
		new_val = RungeKutta (t, y[time(t)], _h, f);
		t += _h;
		y[time(t)] = new_val;
	}
	
	/* Printing results */
	t = _a + margin;
	i = a + lround (margin * (1.0/_h));
	for (; i < b ; i++) {
		printf ("%.1f\t%f\n", t, y[time(t)]);
		t += _h;
	}
	
	free (y);
	y = NULL;
	
	return EXIT_SUCCESS;
}
