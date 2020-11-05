/*********************************************************************//**
 * @file rng.h
 * @brief Random number generation function prototypes.
 **********************************************************************/

#ifndef RNG_H_19239561826387126831623
#define RNG_H_19239561826387126831623

#include "types.h"
#include "SFMT.h"

/*double RandomUniform(void); */
void RngInit(sfmt_t *sfmt);
void RngLambert(sfmt_t *sfmt, XYZ *xyz);

#define RngFill_uint32(sfmt,array,size) RngInit(sfmt);sfmt_fill_array32(sfmt,array,size)

#endif /* RNG_H_19239561826387126831623 */
