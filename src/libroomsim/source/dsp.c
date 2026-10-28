/*********************************************************************//**
 * @file dsp.c
 * @brief Digital Signal Processing routines.
 **********************************************************************//*
 * Author: Steven Schimmel, stevenmschimmel@gmail.com
 * Copyright 2009, University of Zurich
 *************************************************************************
 * This file is part of ROOMSIM.
 *
 * ROOMSIM is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * ROOMSIM is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with ROOMSIM. If not, see <http://www.gnu.org/licenses/>.
 *************************************************************************
 * Modified for SofaMyRoom (https://github.com/robaru/sofamyroom).
 * Copyright 2019-2026, Roberto Barumerli and Daniele Bianchi
 *
 * Changes with respect to the original ROOMSIM file:
 *   - FFTW replaced by PocketFFT (BSD) for the real FFTs of the minimum
 *     phase filter design; the half-complex buffer layout of FFTW is kept
 *     and converted in ExecuteR2HC()/ExecuteHC2R().
 *   - Conv() and FIRfilter() iterate over the filter taps in the outer loop
 *     and over the samples in the inner loop, so that the inner loop is a
 *     plain multiply-add the compiler can vectorize. Every output sample
 *     still accumulates its products in the same order as the direct form.
 *
 * The modifications are released under the same GNU General Public
 * License, version 3 or later.
 *************************************************************************/

#include <math.h>
#include <string.h>

#include "pocketfft.h"
#include "defs.h"
#include "dsp.h"
#include "interp.h"
#include "mem.h"
#include "msg.h"

/* Pointers that do not alias let the compiler vectorize the inner loops. */
#if defined(_MSC_VER)
#  define DSP_RESTRICT __restrict
#elif defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L
#  define DSP_RESTRICT restrict
#elif defined(__GNUC__)
#  define DSP_RESTRICT __restrict__
#else
#  define DSP_RESTRICT
#endif

/** Convolution. The sequence \a h[0...\a hlen - 1] is convolved with
 *  the sequence \a x[0...\a xlen - 1], and the result is stored in 
 *  \a y[0...\a hlen +\a xlen - 1].
 *
 *	@param[in]  h	  First input sequence.
 *	@param[in]  hlen  Length of first input sequence.
 *  @param[in]  x     Second input sequence.
 *  @param[in]  xlen  Length of second input sequence.
 *  @param[out] y     Output sequence
 *
 *  @warning
 *     The output sequence \a y is expected to hold 
 *     \a hlen + \a xlen - 1 elements.
 *  @note
 *     Both the case that \a hlen <= \a xlen and \a hlen > \a xlen 
 *     are handled correctly. Internally, if \a hlen > \a xlen then 
 *     \a h,\a hlen and \a x,\a xlen are swapped.
 */
void Conv(const double *DSP_RESTRICT h, int hlen, const double *DSP_RESTRICT x, int xlen, double *DSP_RESTRICT y)
{
    const double *tmp;
    double hk;
    int    i, k;
    
    /* make sure hlen <= xlen */
    if (hlen>xlen) 
    { 
        /* swap h and x */
        tmp = h; h = x; x = tmp;
        i = hlen; hlen = xlen; xlen = i;
    }
    
    /* Tap-outer, sample-inner form. Output sample y[n] receives its
       products in the order h[0]x[n], h[1]x[n-1], ..., exactly as the 
       direct form does, so the result is bit-identical; the inner loop
       is a multiply-add over contiguous memory that vectorizes. */
    memset(y, 0, (size_t)(hlen+xlen-1)*sizeof(double));
    for (k=0; k<hlen; k++)
    {
        hk = h[k];
        for (i=0; i<xlen; i++)
            y[k+i] += hk * x[i];
    }
}

/** Finite impulse response filter. The sequence \a x[0...\a xlen - 1] 
 *  is filtered by the sequence \a h[0...\a hlen - 1], and the result
 *  is stored in the output sequence \a y[0...\a xlen-1].
 *  When \a state is not NULL, the filter's internal state is initialized
 *  to \a state, and \a state is updated after filtering.
 * 
 *  @param[in]      h      Filter coefficients
 *  @param[in]      hlen   Filter length.
 *  @param[in]      x      Input sequence.
 *  @param[in]      xlen   Length of input sequence.
 *  @param[out]     y      Output sequence.
 *  @param[in,out]  state  Filter's internal state.
 *
 *  @warning
 *     The output sequence \a y is expected to hold \a xlen elements
 *  @warning
 *     The \a state variable must contain \a hlen - 1 elements.
 *  @warning
 *     Requires that \a xlen > \a hlen.
 */
void FIRfilter(const double *DSP_RESTRICT h, int hlen, const double *DSP_RESTRICT x, int xlen, double *DSP_RESTRICT y, double *DSP_RESTRICT state)
{
    double hk;
    int    i, k;
    
    /* initial value of each output sample: the previous state, or zero */
    if (state)
    {
        memcpy(y, state, (size_t)(hlen-1)*sizeof(double));
        memset(&y[hlen-1], 0, (size_t)(xlen-hlen+1)*sizeof(double));
    }
    else
    {
        memset(y, 0, (size_t)xlen*sizeof(double));
    }
    
    /* tap-outer, sample-inner form; see Conv() */
    for (k=0; k<hlen; k++)
    {
        hk = h[k];
        for (i=0; i<xlen-k; i++)
            y[k+i] += hk * x[i];
    }
    
    if (state)
    {
        /* update state: state[i] = sum_{k>i} h[k] x[xlen-k+i], products in ascending k */
        memset(state, 0, (size_t)(hlen-1)*sizeof(double));
        for (k=1; k<hlen; k++)
        {
            hk = h[k];
            for (i=0; i<k; i++)
                state[i] += hk * x[xlen-k+i];
        }
    }
}
/** Log-magnitude frequency response of FIR filter h.
 *
 *  @param[in]       h        FIR filter samples.
 *  @param[in]       hlen     Number of samples in h.
 *  @param[in]       w        Normalized frequencies (radians/sample) at which to evaluate H(e^{jw}).
 *  @param[in]       wlen     Number of frequency samples in w.
 *  @param[in,out]   logmag   Log-magnitude, log |H(e^{jw})|, at w.
 *
 *  @note The in/out array \a logmag is expected to hold \a wlen elements.
 *
 */
void FreqzLogMagnitude(double *h, int hlen, double *w, int wlen, double *logmag)
{
	double cossum, sinsum, wij;
	int i, j;

	for (i=0; i<wlen; i++)
	{
		cossum = 0;
		sinsum = 0;
		for (j=0; j<hlen; j++)
		{
			wij     = w[i] * j;
			cossum += cos(wij) * h[j];
			sinsum += sin(wij) * h[j];
		}
		logmag[i] = LOGDOMAIN(sqrt(cossum*cossum + sinsum*sinsum));
	}
}

/** Plan for creating minimum phase FIR filters.
  */
struct CMinPhaseFIRplan {
    unsigned int nFFT;				/**< Full FFT size. */
	unsigned int nFFThalf;			/**< Half FFT size. */

    unsigned int *idx0;				/**< Freq. interpolation: left index.   */ 
    unsigned int *idx1;				/**< Freq. interpolation: right index.  */
    double *weight0;				/**< Freq. interpolation: left weight.  */
    double *weight1;				/**< Freq. interpolation: right weight. */
    /*double *righthalfhannwin; */

	rfft_plan fftplan;				/**< PocketFFT real FFT plan. */
    double    *fftwork;				/**< PocketFFT in-place work buffer. */
    double    *bufhc;				/**< Half-complex FFT buffer (FFTW layout). */
	double	  *bufr;				/**< Real FFT buffer. */
    
    double       *lifterzerostart;	/**< Pointer to lifter's first 0. */
    unsigned int lifterzerosize;	/**< Number of 0's in lifter. */
    unsigned int liftermul2end;		/**< Index of last 2 in lifter. */
    
    unsigned int complexexpend;		/**< Index. */
    unsigned int realexpnyquist;	/**< Index. */
};

/** Forward real FFT of plan->bufr into plan->bufhc (unnormalized).
 *
 *  PocketFFT returns r0, r1, i1, r2, i2, ...; the result is stored in the
 *  half-complex order of FFTW: r0, r1, ..., r_{n/2}, i_{(n+1)/2-1}, ..., i1.
 */
static void ExecuteR2HC(CMinPhaseFIRplan *plan)
{
    const unsigned int n = plan->nFFT;
    double *w = plan->fftwork, *hc = plan->bufhc;
    unsigned int k;

    memcpy(w, plan->bufr, n * sizeof(double));
    rfft_forward(plan->fftplan, w, 1.0);
    hc[0] = w[0];
    for (k=1; k<(n+1)/2; k++)
    {
        hc[k]   = w[2*k-1];
        hc[n-k] = w[2*k];
    }
    if (n % 2 == 0) hc[n/2] = w[n-1];
}

/** Inverse real FFT of plan->bufhc (half-complex, FFTW order) into plan->bufr (unnormalized). */
static void ExecuteHC2R(CMinPhaseFIRplan *plan)
{
    const unsigned int n = plan->nFFT;
    double *w = plan->fftwork, *hc = plan->bufhc;
    unsigned int k;

    w[0] = hc[0];
    for (k=1; k<(n+1)/2; k++)
    {
        w[2*k-1] = hc[k];
        w[2*k]   = hc[n-k];
    }
    if (n % 2 == 0) w[n-1] = hc[n/2];
    rfft_backward(plan->fftplan, w, 1.0);
    memcpy(plan->bufr, w, n * sizeof(double));
}

/** Allocate and initialize a minimum phase FIR filter design plan. 
 *
 *  @param[in]	nFFT	number of FFT samples, also number of filter taps.
 *  @param[in]	F		array of frequencies of log-magnitude response.
 *  @param[in]	nF		number of frequencies in array F.
 * 
 *  @note
 *     F[nF-1] is assumed to be the Nyquist frequency F_N, i.e., F_s = 2 F_N. 
 */
CMinPhaseFIRplan *AllocMinPhaseFIRplan(unsigned int nFFT, double *F, unsigned int nF)
{
    CMinPhaseFIRplan *plan;
    double *FFTfreq, dFFTfreq;
    unsigned int i, nFFThalf = (nFFT>>1);
    
    plan = (CMinPhaseFIRplan *) MemMalloc(sizeof(CMinPhaseFIRplan));
    FFTfreq = (double *) MemMalloc((nFFThalf+1) * sizeof(double));

    plan->nFFT = nFFT;
    plan->nFFThalf = nFFThalf;
    
    /* allocate interpolation memory */
    plan->idx0    = (unsigned int *) MemMalloc((nFFThalf+1)*sizeof(unsigned int));
    plan->idx1    = (unsigned int *) MemMalloc(nFFT*sizeof(unsigned int));
    plan->weight0 = (double *) MemMalloc(nFFT*sizeof(double));
    plan->weight1 = (double *) MemMalloc(nFFT*sizeof(double));

    /* prepare interpolation data */
    dFFTfreq = 2*F[nF-1]/nFFT;
    for (i=0; i<=nFFThalf; i++) 
        FFTfreq[i] = dFFTfreq * i;

    PrepareLinearInterpolate(F,nF,FFTfreq,nFFThalf+1, plan->idx0, plan->idx1, plan->weight0, plan->weight1);
    MemFree(FFTfreq);

    /* allocate FFT buffers and prepare the FFT plan */
    plan->bufhc   = (double *) MemMalloc(nFFT * sizeof(double));
    plan->bufr    = (double *) MemMalloc(nFFT * sizeof(double));
    plan->fftwork = (double *) MemMalloc(nFFT * sizeof(double));
    plan->fftplan = make_rfft_plan(nFFT);
    if (!plan->fftplan)
    {
        MsgErrorExit("unable to create FFT plan");
    }
    
    plan->liftermul2end = (nFFT+1)>>1;
    plan->lifterzerostart = &plan->bufr[(nFFT>>1)+1];
    plan->lifterzerosize = ((nFFT-1)>>1) * sizeof(double);
    
    plan->complexexpend = (nFFT+1)>>1;
    plan->realexpnyquist = (1 - (nFFT % 2)) * (nFFT>>1);
    
    return plan;
}

/** Design minimum phase FIR filter with desired log-magnitude frequency response. 
 *
 *  @param[in]		logmag	array containing desired log-magnitude frequency response.
 *  @param[in,out]	h		memory location for filter coefficients.
 *  @param[in]		plan	filter design plan, obtained from \a AllocMinPhaseFIRplan.
 *
 */
void LogMagFreqResp2MinPhaseFIR(const double *logmag, double *h, CMinPhaseFIRplan *plan)
{
    unsigned int i;
    double tmp1, tmp2;
    
    /* resample log mag onto FFT grid */
    ExecuteLinearInterpolate(logmag, plan->bufhc, plan->nFFThalf+1, plan->idx0, plan->idx1, plan->weight0, plan->weight1);
    /* set imaginary part to 0 */
    memset(&plan->bufhc[plan->nFFThalf+1], 0, (plan->nFFT - plan->nFFThalf - 1) * sizeof(double));
    
    /* compute real cepstrum by taking real output of ifft  */
    ExecuteHC2R(plan);
    
    /** @todo Normalization should be integrated with multiplication step 
              below, or with complex exp step further below. */
    
    /* normalize ifft output by dividing by N */
    for (i=0; i<plan->nFFT; i++) 
		plan->bufr[i] /= plan->nFFT;
    
    /* lifter positive quefrencies */
    for (i=1; i<plan->liftermul2end; i++) 
		plan->bufr[i] *= 2;
    memset(plan->lifterzerostart, 0, plan->lifterzerosize);

    /* take fft */
    ExecuteR2HC(plan);
    
    /* compute complex exp of half-complex buffer */
    plan->bufhc[0] = exp(plan->bufhc[0]); /* DC */
    for (i=1; i<plan->complexexpend; i++)
    {
        tmp1 = exp(plan->bufhc[i]);
        tmp2 = plan->bufhc[plan->nFFT-i];
        plan->bufhc[i]            = tmp1 * cos(tmp2);
        plan->bufhc[plan->nFFT-i] = tmp1 * sin(tmp2);
    }
    if (plan->realexpnyquist) 
		plan->bufhc[plan->realexpnyquist] = exp(plan->bufhc[plan->realexpnyquist]); 
    
    /* take ifft and real => minphase sequence */
    ExecuteHC2R(plan);
    for (i=0; i<plan->nFFT; i++) 
		plan->bufr[i] /= plan->nFFT;
    
    /** @todo When truncation desired, apply right-half hann window to min phase respone. */
    /** @todo Or should window always be applied to smooth the freq. resp? */
    
    /* copy min phase sequence to output */
    memcpy(h, plan->bufr, plan->nFFT*sizeof(double));
}

/** Release memory associated with minimum phase FIR filter design plan. 
 *
 *  @param[in]	plan	filter design plan, obtained from \a AllocMinPhaseFIRplan.
 */
void FreeMinPhaseFIRplan(CMinPhaseFIRplan *plan)
{
    /* free interpolation memory */
    MemFree(plan->idx0);
    MemFree(plan->idx1);
    MemFree(plan->weight0);
    MemFree(plan->weight1);

    /* destroy FFT plan and free FFT buffers */
    destroy_rfft_plan(plan->fftplan);
    MemFree(plan->fftwork);
    MemFree(plan->bufr);
    MemFree(plan->bufhc);
    
    /* free plan memory     */
    MemFree(plan);
}

void TimeVaryingConv(const double *hh, int hlen, 
					 const int *idx, int nidx, 
					 const unsigned int *x, int xlen, 
					 int xstart, unsigned int xthreshold,
					 double *y)
{
	const double *h0, *h1;
	double		 len, w0, w1;
	int			 idx0, idx1;
	int			 i, j, jmin, k, n=0;

	/* initialize pointers, indices, and length */
	h0   = hh;
	h1   = h0 + hlen;
	idx0 = idx[n];
	idx1 = idx[n+1];
	len  = idx1 - idx0;

	/* clear output */
	memset(y,0,xlen*sizeof(y[0]));

	/* loop over input samples */
	for (i=0; i<xlen; i++)
	{
		/* is current sample a unit pulse? */
		if (i>=xstart && x[i] < xthreshold)
		{
			/* compute relative weights for h0 and h1 */
			w1 = (i - idx0) / len;
			w0 = 1.0 - w1;

			/* determine lower extent of convolution */
			jmin = i - hlen + 1;
			if (jmin<0) jmin = 0;

			/* add two weighted impulse responses to current output sample */
			for (j=i, k=0; j>=jmin; k++, j--)
				y[j] += w0 * h0[k] + w1 * h1[k];
		}

		/* move to next pair of impulse responses? */
		if (i>=idx1)
		{
			/* update pointers */
			h0   = h1;
			h1   = h0 + hlen;
			idx0 = idx1;

			/* determine next update sample */
			n    = n + 1;
			if (n<nidx-1)
			{
				idx1 = idx[n+1];
			}
			else
			{
				idx1 = xlen-1;
				h1   = h0;
			}

			/* set len to length of interval */
			len  = idx1 - idx0;
		}

	} /* next input sample */
}
