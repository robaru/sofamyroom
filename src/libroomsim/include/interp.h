/*********************************************************************//**
 * @file interp.h
 * @brief Interpolation function prototypes.
 **********************************************************************/

#ifndef _INTERP_H_6482163816451836213987120382146
#define _INTERP_H_6482163816451836213987120382146

void ExecuteLinearInterpolate(const double *yi, double *yo, int olen,
                                 const unsigned int *idx0, const unsigned int *idx1, const double *weight0, const double *weight1);
void PrepareLinearInterpolate(const double *xi, int ilen, const double *xo, int olen, 
                                 unsigned int *idx0, unsigned int *idx1, double *weight0, double *weight1);

void LinearInterpolate(const double *xi, const double *yi, int ilen, const double *xo, double *yo, int olen);

#endif /* #ifndef _INTERP_H_6482163816451836213987120382146 */
