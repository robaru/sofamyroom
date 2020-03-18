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
 *************************************************************************/

#include <math.h>
#include <string.h>

#ifdef __GNUC__
#	include <fftw3.h>
#else
#	include "fftw3.h"
#endif
#include "defs.h"
#include "dsp.h"
#include "interp.h"
#include "mem.h"

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
void Conv(const double *h, int hlen, const double *x, int xlen, double *y)
{
    const double *hp, *xp;
    int   count, i;
    
    /* make sure hlen <= xlen */
    if (hlen>xlen) 
    { 
        /* swap h and x */
        hp = h; h = x; x = hp;
        i = hlen; hlen = xlen; xlen = i;
    }
    
    /* compute head */
    for (i=0; i<hlen-1; i++)
    {
        y[i] = 0.0;
        hp = h;
        xp = &(x[i]);
        count = i+1;
        while (count--)
            y[i] += (*xp--) * (*hp++);
    }

    /* compute mid-section */
    for (i=hlen-1; i<=xlen-1; i++)
    {
        y[i] = 0.0;
        hp = h;
        xp = &(x[i]);
        count = hlen;
        while (count--)
            y[i] += (*xp--) * (*hp++);
    }
    
    /* compute tail */
    for (i=xlen; i<xlen+hlen-1; i++)
    {
        y[i] = 0.0;
        hp = &(h[(i-xlen) + 1]);
        xp = &(x[xlen-1]);
        count = hlen-1 - (i-xlen);
        while (count--)
            y[i] += (*xp--) * (*hp++);
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
void FIRfilter(const double *h, int hlen, const double *x, int xlen, double *y, double *state)
{
    const double *hp, *xp;
    int          count, i;
    
    if (state)
    {
        /* compute initial output using previous state */
        for (i=0; i<hlen-1; i++)
        {
            y[i]  = state[i];
            hp    = h;
            xp    = &(x[i]);
            count = i+1;
            while (count--)
                y[i] += (*xp--) * (*hp++);
        }
        
        /* compute mid-section  */
        for (i=hlen-1; i<xlen; i++)
        {
            y[i]  = 0.0;
            hp    = h;
            xp    = &(x[i]);
            count = hlen;
            while (count--)
                y[i] += (*xp--) * (*hp++);
        }
        
        /* update state */
        for (i=0; i<hlen-1; i++)
        {
            state[i] = 0.0;
            hp       = &(h[i+1]);
            xp       = &(x[xlen-1]);
            count    = hlen - 1 - i;
            while (count--)
                state[i] += (*xp--) * (*hp++);
        }
    }
    else
    {
        /* compute initial output without a previous state */
        for (i=0; i<hlen-1; i++)
        {
            y[i]  = 0.0;
            hp    = h;
            xp    = &(x[i]);
            count = i+1;
            while (count--)
                y[i] += (*xp--) * (*hp++);
        }
        
        /* compute mid-section */
        for (i=hlen-1; i<xlen; i++)
        {
            y[i]  = 0.0;
            hp    = h;
            xp    = &(x[i]);
            count = hlen;
            while (count--)
                y[i] += (*xp--) * (*hp++);
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

	fftw_plan fftwplanr2hc;			/**< Real to half-complex forward FFTW plan. */
    fftw_plan fftwplanhc2r;			/**< Half-complex to real inverse FFTW plan. */
    double    *fftwbufhc;			/**< FFTW buffer for half-complex FFT output. */
	double	  *fftwbufr;			/**< FFTW buffer for real FFT output. */
    
    double       *lifterzerostart;	/**< Pointer to lifter's first 0. */
    unsigned int lifterzerosize;	/**< Number of 0's in lifter. */
    unsigned int liftermul2end;		/**< Index of last 2 in lifter. */
    
    unsigned int complexexpend;		/**< Index. */
    unsigned int realexpnyquist;	/**< Index. */
};

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

    /* allocate FFTW memory and prepare FFTW plans */
    plan->fftwbufhc  = (double *) fftw_malloc(nFFT * sizeof(double));
    plan->fftwbufr   = (double *) fftw_malloc(nFFT * sizeof(double));
    plan->fftwplanhc2r = fftw_plan_r2r_1d(nFFT, plan->fftwbufhc, plan->fftwbufr,  FFTW_HC2R, FFTW_ESTIMATE);
    plan->fftwplanr2hc = fftw_plan_r2r_1d(nFFT, plan->fftwbufr,  plan->fftwbufhc, FFTW_R2HC, FFTW_ESTIMATE);
    
    plan->liftermul2end = (nFFT+1)>>1;
    plan->lifterzerostart = &plan->fftwbufr[(nFFT>>1)+1];
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
    ExecuteLinearInterpolate(logmag, plan->fftwbufhc, plan->nFFThalf+1, plan->idx0, plan->idx1, plan->weight0, plan->weight1);
    /* set imaginary part to 0 */
    memset(&plan->fftwbufhc[plan->nFFThalf+1], 0, (plan->nFFT - plan->nFFThalf - 1) * sizeof(double));
    
    /* compute real cepstrum by taking real output of ifft  */
    fftw_execute(plan->fftwplanhc2r);
    
    /** @todo Normalization should be integrated with multiplication step 
              below, or with complex exp step further below. */
    
    /* normalize ifft output by dividing by N */
    for (i=0; i<plan->nFFT; i++) 
		plan->fftwbufr[i] /= plan->nFFT;
    
    /* lifter positive quefrencies */
    for (i=1; i<plan->liftermul2end; i++) 
		plan->fftwbufr[i] *= 2;
    memset(plan->lifterzerostart, 0, plan->lifterzerosize);

    /* take fft */
    fftw_execute(plan->fftwplanr2hc);
    
    /* compute complex exp of half-complex buffer */
    plan->fftwbufhc[0] = exp(plan->fftwbufhc[0]); /* DC */
    for (i=1; i<plan->complexexpend; i++)
    {
        tmp1 = exp(plan->fftwbufhc[i]);
        tmp2 = plan->fftwbufhc[plan->nFFT-i];
        plan->fftwbufhc[i]            = tmp1 * cos(tmp2);
        plan->fftwbufhc[plan->nFFT-i] = tmp1 * sin(tmp2);
    }
    if (plan->realexpnyquist) 
		plan->fftwbufhc[plan->realexpnyquist] = exp(plan->fftwbufhc[plan->realexpnyquist]); 
    
    /* take ifft and real => minphase sequence */
    fftw_execute(plan->fftwplanhc2r);
    for (i=0; i<plan->nFFT; i++) 
		plan->fftwbufr[i] /= plan->nFFT;
    
    /** @todo When truncation desired, apply right-half hann window to min phase respone. */
    /** @todo Or should window always be applied to smooth the freq. resp? */
    
    /* copy min phase sequence to output */
    memcpy(h, plan->fftwbufr, plan->nFFT*sizeof(double));
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

    /* destroy FFTW plans and free FFTW memory */
    fftw_destroy_plan(plan->fftwplanhc2r);
    fftw_destroy_plan(plan->fftwplanr2hc);
    fftw_free(plan->fftwbufr);
    fftw_free(plan->fftwbufhc);
    
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
