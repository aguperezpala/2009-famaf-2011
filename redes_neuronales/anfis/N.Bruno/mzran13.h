#ifndef MZRAN13_H
#define MZRAN13_H

#include <inttypes.h> /* To have access to uint64_t */

/* Another random number generator, proposed by Marsaglia and Zaman in 1994
 * It's period is around 2^125 ~ 10^36... MUCH greater than ran2's 10^18
 * Initialization is done using auxiliary routine ran13set. If it's not used
 * default seed values (strongly recommended) are used.
 */
extern uint64_t mzran13 (void);

/* Seeding routine for mzran13 */
void ran13set (uint64_t  xx, uint64_t  yy, uint64_t  zz, long nn);

#endif
