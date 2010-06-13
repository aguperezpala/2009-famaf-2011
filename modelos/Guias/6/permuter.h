#ifndef PERM_H
#define PERM_H

/* Devuelve la permutacion aleatoria de un arreglo "a" de longitud "n" cuyos
 * elementos ocupan "size" bytes en memoria
 * Si el arreglo tiene punteros a otros bloques, no se copiaran los bloques en
 * si sino sus direcciones de memoria.
 * PRE: length(a) <= n
 *	"a tiene al menos un elemento, en la posicion 0"
 * POS: "los elementos de a fueron permutados aleatoriamente"
 * FIXME: NO FUNCIONA !!!
 */
void perm (void **a, unsigned int n, size_t size);

/* Devuelve la permutacion aleatoria de un arreglo de ints "a" de longitud "n"
 * PRE: 0 < length(a) < n
 * POS: "los elementos de a fueron permutados aleatoriamente"
 */
void perm_int (int *a, unsigned int n);


#endif
