/*********************************************************************//**
 * @file dsp.h
 * @brief Digital Signal Processing routines.
 **********************************************************************/

#ifndef _DSP_H_123795791719246514351
#define _DSP_H_123795791719246514351

void FIRfilter(const double *h, int hlen, const double *x, int xlen, double *y, double *state);
void Conv(const double *h, int hlen, const double *x, int xlen, double *y);

void FreqzLogMagnitude(double *h, int hlen, double *w, int wlen, double *logmag);

/** Opague type for minimum-phase FIR conversion routine. */
typedef struct CMinPhaseFIRplan CMinPhaseFIRplan;

CMinPhaseFIRplan *AllocMinPhaseFIRplan(unsigned int nFFT, double *F, unsigned int nF);
void LogMagFreqResp2MinPhaseFIR(const double *logmag, double *h, CMinPhaseFIRplan *plan);
void FreeMinPhaseFIRplan(CMinPhaseFIRplan *plan);

void TimeVaryingConv(const double *hh, int hlen, 
					 const int *idx, int nidx, 
					 const unsigned int *x, int xlen,
					 int xstart, unsigned int xthreshold,
					 double *y);

#endif /* #ifndef _DSP_H_123795791719246514351 */
