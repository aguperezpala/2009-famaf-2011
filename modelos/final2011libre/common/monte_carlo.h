#ifndef MONTE_CARLO_H
#define MONTE_CARLO_H

#include "gen_continuas.h"

typedef double (*FUNC_PTR)(float);

typedef double (*FUNC_PTR_COMP)(float,float);
/* TODO: cambiar el valor de la constante si tomamos mas de 2 args */
#define MONTE_CARLO_COMP_N	2

/* Caso @ = Integrate (g(x) dx) from 0 to 1:
 * => @ = E[g(U)] = Sum (g(Ui) / k) from 1 to k (con Ui ~ U(0,1)
*/
double monte_carlo_normal(int k, FUNC_PTR g);


/* Caso @ = Integrate (g(x) dx) from a to b:
* => cambio de variable: y = (x-a)/(b-a), dy=dx/(b-a) =>
* @ = integrate (g(a + (b-a) * y) * (b-a) dy) from 0 to 1 ==
* integrate h(y) dy  from 0 to 1
* Luego usamos la montecarlo normal
*/
double monte_carlo_ab(int k, FUNC_PTR g, double a, double b);


/* Caso @ = Integrate (g(x) dx) from 0 to inf:
* => cambio de variable: y = 1/(x+1), dy=-dx/pow((x+1),2)=-pow(y,2)dx =>
* @ = integrate (g((1/y)-1) / pow(y,2) dy) from 0 to 1 ==
* integrate h(y) dy  from 0 to 1
* Luego usamos la montecarlo normal
*/
double monte_carlo_0toInf(int k, FUNC_PTR g);


/* Caso de integracion multiple (DEBEMOS cambiar las definiciones de los tipos
 * en el caso de que la funcion tome mas de 2 argumentos).
 * @ = Integrate (integrate (g(x1,x2)) dx1 from 0 to 1) dx2 from 0 to 1 ==
 * Sum (g(U1, U2)/k) from 1 to k
 */
double monte_carlo_multiple(int k, FUNC_PTR_COMP g);


#endif
