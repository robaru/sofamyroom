/*********************************************************************//**
 * @file roomsim.c
 * @brief Room simulation routines.
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


/****** NOTES ************************************************************/
/**

 @todo Make sure that all frequency band weighting uses the same base for log. 
    => Done. They all use the LOGDOMAIN macro now.

 @todo Make roomsim.c independent of mex.h.
 
 @todo Have Roomsim return the BRIR properly (currently returns pointer to
       internal.brir, without knowledge of number of brir's in the array).
 
 @todo Make separation of mexmain.c and roomsim.c complete, by removing
       interdependencies on global roomsim and internal structures.
    => Done. Converted from global variables to function arguments as part
	   of the development of the C codebase.
 
 @todo Loading of sensors: must match Fs, and how to deal with sensor v. simulation frequency bands
    => Partly done. Fs is matched on running a simulation.

 @todo Receiver ypr should be negated when receiver YPRT computed.
	=> Done. yprt matrices are now for room->source and room->receiver 
	   coordinate transformations, and source->receiver vector is
	   negated to obtain receiver->source vector.

 @todo Should let sensor init functions return void *, which is passed to sensor probe functions (should I?)
    => Done. Sensor init functions fill a sensor definition structure, which includes a void * for private use. 
       This void * is passed to sensor probe.

 @todo Convert roomsim attenuation and diffusion coefficients to log domain.
       Also, determine correct interpretation of surface attenuation coefficients
       in material databases. (Is sqrt(1-a) appropriate? It is a little confusing
       when manually setting attenuation coefficients.)

************************************************ @file *******************/
 
/* C library includes */
/* math.h defines sin, cos, tan, asin, acos, atan, atan2, sinh, cosh, tanh, 
                  exp, log (=ln), log10, pow, sqrt, ceil, floor, fabs */
#include <math.h>     
#include <string.h>

/* NOTE: I was unable to make the FFTW DLL work with LCC. Version 2.4 of LCC 
   that is shipped with MATLAB does not support __declspec(dllimport), hence 
   it does not know how to call functions inside a DLL. Attempts to upgrade 
   LCC to version 4.2 and linking to a static FFTW library both were unsuccessful.
   The only solution I found was to compile/link using the Visual C compiler */
#include "fftw3.h"

/* local includes */
#include "3D.h"
#include "defs.h"
#include "dsp.h"
#include "interface.h"
#include "interp.h"
#include "mem.h"
#include "msg.h"
#include "rng.h"
#include "sensor.h"
#include "types.h"

/* disable warnings about unreferenced inline functions and unsafe CRT functions */
#ifdef _MSC_VER
#  pragma warning( disable : 4514 4996)
#endif

#define NFFT_SIZE 512

/* Note: global variables are persistent across calls, but cleared when mex-function cleared
   mxMalloc'ed memory pointed to by global variables is released after each call, unless
   made persistent. then, it needs a call to mxFree in mexAtExit
 */

/** Internal data structure holding data for a sensor. */
typedef struct {
    const YPRT           r2s_yprt;      /**< Room-to-sensor coordinate transformation matrix. */
    const YPRT           s2r_yprt;      /**< Sensor-to-room coordinate transformation matrix. */
    CSensorDefinition    *definition;   /**< Sensor definition */

	double				 *TFShist;
	double				 *FirstTOA;
	int					 nTbin;
	int					 nFbin;
	int					 nSbin;
} CSensorInternal;

/* Spatial bins center directions: front,back,left,right,up,down */
static const XYZ SpaceBinCenter[6] = {{1,0,0},{-1,0,0},{0,1,0},{0,-1,0},{0,0,1},{0,0,-1}};

/* #define RECV_TFS_BIN(r,t,f,s) (r).TFShist[(t) + ((r).nTbin) * ( (f) + ((r).nFbin) * (s) ) ] */
#define RECV_TFS_BIN(r,t,f,s) (r).TFShist[(f) + ((r).nFbin) * ( (t) + ((r).nTbin) * (s) ) ]


/** Internal simulation data structure. */
typedef struct {

	/* general simulation variables */
	double  fs;
	double  duration;
	int		length;
	double  diffusetimestep;
    double  c;          /**< speed of sound (m/s) */
    double  csample;    /**< speed of sound (m/sample) */

	/* simulation frequency bands */
    int     nBands;					/**< Number of frequency bands in simulation. */
    double  *frequency;				/**< Center frequencies of simulation frequency bands. */

	/* absorption, reflection, and attenuation coefficients */
    double  *logairattenuation;		/**< Frequency dependent air attenuation coefficients. */
    double  *logreflection;			/**< Frequency dependent surface reflection coefficients. */
    double  *logabsorption;			/**< Frequency dependent surface absorption coefficients. */
    double  *logdiffusion;			/**< Frequency dependent surface diffusion coefficients.  */
    double  *logdiffusereflection;
	double  *logspecularreflection;
	double  *diffusioncoefficient;
    
	/* image source method fields */
    double  *surfaceattenuation;	/**< Attenuation of surfaces on virtual-to-real room. */
    double  *attenuation;			/**< Total attenuation of image source to receiver path. */
    double  *h;						/**< Buffer for impulse responses. */
    double  *convbuf;				/**< Convolution buffer. */

	/* diffuse rain algorithm fields */
	double	*htv;
	int		*htvidx;
	unsigned int *noise;
	double  *shapednoise;
	double  *directionalshapednoise;

    CMinPhaseFIRplan *minphaseplan;	/**< Design plan for minimum phase FIR filter from attenuation. */

    /* sources */
    int     nSources;
    CSensorInternal *source;
    
	/* receivers */
    int     nReceivers;
    CSensorInternal *receiver;

	/* output */
    BRIR    *brir;
    
} CRoomsimInternal;

/* Convenience macro's to access individual surface absorption and diffusion coefficients */
#define SURFACELOGREFLECTION(v,s,b)			((v)->logreflection		  [(b) + (s) * ((v)->nBands)])
#define SURFACELOGABSORPTION(v,s,b)			((v)->logabsorption		  [(b) + (s) * ((v)->nBands)])
#define SURFACELOGDIFFUSION(v,s,b)			((v)->logdiffusion	      [(b) + (s) * ((v)->nBands)])
#define SURFACEDIFFUSIONCOEFFICIENT(v,s,b)	((v)->diffusioncoefficient[(b) + (s) * ((v)->nBands)])

/* Convenience macro to negate an XYZ structure */
#define XYZ_NEGATE(v) ( (v).x = -(v).x; (v).y = -(v).y; (v).z = -(v).z; )

void ComputeBandFrequencies(const CRoomSetup *pSetup, CRoomsimInternal *pSimulation)
{
    int p0, p1;
    int i, idx;
    double base = pow(2,1.0/pSetup->options.bandsperoctave);
    
    p0 = (int) ceil(LOG2(30.0/pSetup->options.referencefrequency)  * pSetup->options.bandsperoctave);
    p1 = (int) floor(LOG2(pSetup->options.fs / 2.22 / pSetup->options.referencefrequency) * pSetup->options.bandsperoctave);
    
    pSimulation->nBands = (p1-p0+1+2);
    pSimulation->frequency = (double *) MemMalloc(pSimulation->nBands * sizeof(double));

    /* assign frequencies to bands */
    pSimulation->frequency[0] = 0.0;
    for (idx=1,i=p0; i<=p1; i++,idx++)
        pSimulation->frequency[idx] = ROUND(pSetup->options.referencefrequency * pow(base,i));
    pSimulation->frequency[idx] = pSetup->options.fs / 2.0;
}

void PrintAbsorptionAndDiffusion(const CRoomSetup *pSetup, CRoomsimInternal *pSimulation)
{
    int b, s;
    
    MsgPrintf("\nNumber of surface bands: %d\n", pSetup->room.surface.nBands);
    for (b=0; b<pSetup->room.surface.nBands; b++)
        MsgPrintf("%7.f ", pSetup->room.surface.frequency[b]);
    
    MsgPrintf("\n\nAbsorption\n");
    for (s=0; s<6; s++)
    {
        for (b=0; b<pSetup->room.surface.nBands; b++)
            MsgPrintf("%7.4f ", pSetup->room.surface.absorption[b+s*pSetup->room.surface.nBands]);
        MsgPrintf("\n");
    }
    MsgPrintf("\n");
    
    MsgPrintf("Diffusion\n");
    for (s=0; s<6; s++)
    {
        for (b=0; b<pSetup->room.surface.nBands; b++)
            MsgPrintf("%7.4f ", pSetup->room.surface.diffusion[b+s*pSetup->room.surface.nBands]);
        MsgPrintf("\n");
    }
    MsgPrintf("\n");
    
    MsgPrintf("\nNumber of simulation bands: %d\n", pSimulation->nBands);
    for (b=0; b<pSimulation->nBands; b++)
        MsgPrintf("%7.f ", pSimulation->frequency[b]);


    MsgPrintf("\n\nAbsorption\n");
    for (s=0; s<6; s++)
    {
        for (b=0; b<pSimulation->nBands; b++)
            MsgPrintf("%7.4f ", LINDOMAIN(pSimulation->logabsorption[b+s*pSimulation->nBands]));
        MsgPrintf("\n");
    }
    MsgPrintf("\n\nDiffusion\n");
    for (s=0; s<6; s++)
    {
        for (b=0; b<pSimulation->nBands; b++)
            MsgPrintf("%7.4f ", LINDOMAIN(pSimulation->logdiffusion[b+s*pSimulation->nBands]));
        MsgPrintf("\n");
    }
    MsgPrintf("\n");
}

/** Resamples the surface absorption and diffusion coefficients 
 *  at the simulation band frequencies.
 *
 *  @note
 *     Requires that the \a pSetup structure contains meaningful values,
 *     and that the \a pSimulation->nBands and \a pSimulation->frequency 
 *     fields are initialized.
 */
#define LOGLIMIT(v)  if((v)<LOGMINIMUM) (v)=LOGMINIMUM;

void InterpolateAbsorptionAndDiffusion(const CRoomSetup *pSetup, CRoomsimInternal *pSimulation)
{
    int    i, j, Nin, Nout;
	double *alpha, *d, r;
    
	Nin  = pSetup->room.surface.nBands;
	Nout = pSimulation->nBands;
	
	/* allocate arrays in simulation structure */
    pSimulation->logreflection         = (double *) MemMalloc(Nout * 6 * sizeof(double));
    pSimulation->logabsorption         = (double *) MemMalloc(Nout * 6 * sizeof(double));
    pSimulation->logdiffusion          = (double *) MemMalloc(Nout * 6 * sizeof(double));
    pSimulation->logdiffusereflection  = (double *) MemMalloc(Nout * 6 * sizeof(double));
    pSimulation->logspecularreflection = (double *) MemMalloc(Nout * 6 * sizeof(double));
    pSimulation->diffusioncoefficient  = (double *) MemMalloc(Nout * 6 * sizeof(double));
    
	/* allocate local arrays */
	alpha = (double *)MemMalloc(pSimulation->nBands * sizeof(double));
	d     = (double *)MemMalloc(pSimulation->nBands * sizeof(double));
	for (i=0; i<6; i++)
	{
		/* interpolate absorption and diffusion coefficients to simulation bands */
        LinearInterpolate(pSetup->room.surface.frequency,          /* xin */
                          &pSetup->room.surface.absorption[i*Nin], /* yin */
                          Nin,                                     /* Nin */
                          pSimulation->frequency,                  /* xout */
                          alpha,								   /* yout */
                          Nout);								   /* Nout   */
        LinearInterpolate(pSetup->room.surface.frequency,          /* xin */
                          &pSetup->room.surface.diffusion[i*Nin],  /* yin */
                          Nin,                                     /* Nin */
                          pSimulation->frequency,                  /* xout */
                          d,								       /* yout */
                          Nout);								   /* Nout   */

		for (j=0; j<Nout; j++)
		{
			r = sqrt(1.0 - alpha[j]);
			/* r = 1.0 - alpha[j]; */
			pSimulation->logreflection        [i*Nout + j] = LOGDOMAIN(r);
			LOGLIMIT(pSimulation->logreflection        [i*Nout + j]);

			pSimulation->logabsorption        [i*Nout + j] = LOGDOMAIN(1.0 - r);
			LOGLIMIT(pSimulation->logabsorption        [i*Nout + j]);

			pSimulation->logdiffusion         [i*Nout + j] = LOGDOMAIN(d[j]);
			LOGLIMIT(pSimulation->logdiffusion         [i*Nout + j]);

			pSimulation->logspecularreflection[i*Nout + j] = LOGDOMAIN((1.0 - d[j]) * r);
			LOGLIMIT(pSimulation->logspecularreflection[i*Nout + j]);

			pSimulation->logdiffusereflection [i*Nout + j] = LOGDOMAIN(d[j] * r);
			LOGLIMIT(pSimulation->logdiffusereflection [i*Nout + j]);

			pSimulation->diffusioncoefficient [i*Nout + j] = d[j];
		}
	}

	/* free local arrays */
    MemFree(alpha);
    MemFree(d);
}

#if 0
void ComputeAirAbsorption(const CRoomSetup *pSetup, CRoomsimInternal *pSimulation)
{
	double tmp, m;
	int i;

    /* compute the frequency dependent pressure absorption coeff for air */
    tmp = 5.5e-4 * (0.50 / pSetup->room.humidity);
    pSimulation->logairattenuation = (double *) MemMalloc(pSimulation->nBands * sizeof(double));
    for (i=0; i<pSimulation->nBands; i++)
    {
		m = tmp * pow(pSimulation->frequency[i] * 1e-3, 1.7);
        pSimulation->logairattenuation[i] = LOGDOMAIN(exp(-0.5 * m));
        /* LOG( ("m[%2d] = %10lf, att[%2d] = %10lf\n", i, pSimulation->m[i], i, pSimulation->logairattenuation[i]) ) */
    }
}
#endif

/* compute frequency dependent pressure absorption coefficients for air 
   according to the ISO 9613-1:1993 standard */
void ComputeAirAbsorption(const CRoomSetup *pSetup, CRoomsimInternal *pSimulation)
{
	double pressure = 101325.0 / 101325.0;
	double temp = pSetup->room.temperature + 273.15;
	double Chumid = 4.6151 - 6.8346 * pow((273.15/temp),1.261);
	double hum = (pSetup->room.humidity * 100) * pow(10,Chumid) * pressure;
	double tempr = temp / 293.15;
	double frO = pressure * (24 + 4.04e4 * hum * (0.02 + hum)/(0.391 + hum));
	double frN = pressure * pow(tempr,-0.5) * (9 + 280*hum*exp(-4.17*(pow(tempr,-1/3)-1)));
	double alpha, f2;
	int i;

    pSimulation->logairattenuation = (double *) MemMalloc(pSimulation->nBands * sizeof(double));

	for (i=0; i<pSimulation->nBands; i++)
	{
		f2 = pSimulation->frequency[i]; f2*=f2;
		alpha = 8.686*f2 * 
			( 1.84e-11 * (1/pressure) * sqrt(tempr) + pow(tempr,-2.5) * 
			     ( 0.01275 * (exp(-2239.1/temp) / (frO + f2/frO)) + 
			       0.1068  * (exp(-3352  /temp) / (frN + f2/frN))
			     )
		    );
		pSimulation->logairattenuation[i] = LOGDOMAIN(pow(10,alpha/-20));
		/*printf("F=%.0f Hz, Air attenuation=%.5f\n", pSimulation->frequency[i],pow(10,alpha/-20)); */
	}
}

/* local convenience macros */
#define IMGF(x) (((x)+1)&(-2))
#define IMGS(x) (-(((x)&1)*2-1))

typedef struct {
	int order;
	int rx, ry, rz;
	int surfacecount[6];
	const CRoomSetup *pSetup;
	CRoomsimInternal *pSimulation;
} CRoomCallbackArg;

void roomcallback(const CRoomCallbackArg *arg) /*(int order, int rx, int ry, int rz, int *surfacecount) */
{
    XYZ				 S, V, W, xyz;
    double			 distance, delay, tmp;
    double			 *y;
    const double     *sourceimpulse, *receiverimpulse;
	CSensorResponse  sourceresponse, receiverresponse;
    const double	 *x, *h;
    int				 b, s, si, ri;
    int				 i, sr, ofs, lim;
    int				 xlen, ylen, hlen, nChannels;

    /* compute surface absorption/diffusion for this virtual room */
	i = arg->pSimulation->nBands;
    for (b=0; b<i; b++)
    {
        arg->pSimulation->surfaceattenuation[b] = 0;
        for (s=0; s<6; s++)
            arg->pSimulation->surfaceattenuation[b] += arg->pSimulation->logspecularreflection[b+s*i] * arg->surfacecount[s];
    }
    
    /* loop over all sources */
    for (si=0; si<arg->pSetup->nSources; si++)
    {
        /* compute virtual source position */
        S.x = IMGF(arg->rx) * arg->pSetup->room.dimension[0] + IMGS(arg->rx) * arg->pSetup->source[si].location[0];
        S.y = IMGF(arg->ry) * arg->pSetup->room.dimension[1] + IMGS(arg->ry) * arg->pSetup->source[si].location[1];
        S.z = IMGF(arg->rz) * arg->pSetup->room.dimension[2] + IMGS(arg->rz) * arg->pSetup->source[si].location[2];
        
        /* loop over all receivers */
        for (ri=0; ri<arg->pSetup->nReceivers; ri++)
        {
	        /* compute virtual source to receiver vector */
            V.x = arg->pSetup->receiver[ri].location[0] - S.x;
            V.y = arg->pSetup->receiver[ri].location[1] - S.y;
            V.z = arg->pSetup->receiver[ri].location[2] - S.z;

			/* derive receiver to virtual source vector */
			W.x = -V.x; 
			W.y = -V.y; 
			W.z = -V.z;
                                
            /* compute distance, delay */
            distance = sqrt(V.x*V.x + V.y*V.y + V.z*V.z);
            delay = distance / arg->pSimulation->c;
            
            /* jump out of loop if delay > max delay */
            if (delay > arg->pSetup->options.responseduration) 
                continue;

            /* copy virtual room surface attenuation to source/receiver attenuation */
            memcpy(arg->pSimulation->attenuation,arg->pSimulation->surfaceattenuation,arg->pSimulation->nBands*sizeof(double));
            
            /* apply attenuation from distance and air absorption */
            if (arg->pSetup->options.distanceattenuation)
            {
                tmp = LOGDOMAIN(distance);
                for (b=0; b<arg->pSimulation->nBands; b++)
                    arg->pSimulation->attenuation[b] -= tmp;
            }
            if (arg->pSetup->options.airabsorption)
            {
                for (b=0; b<arg->pSimulation->nBands; b++)
                    arg->pSimulation->attenuation[b] += distance*arg->pSimulation->logairattenuation[b];
            }
                  
            /** @todo Add test for maximum attenuation, something like 
                      "if max(Attenuation) < MinReflection, continue; end;". */

            /* flip source vector component depending on reflection order */
			V.x *= IMGS(arg->rx);
			V.y *= IMGS(arg->ry);
			V.z *= IMGS(arg->rz);

			/* convert source vector from room to sensor coordinates */
            YawPitchRoll(&V,&arg->pSimulation->source[si].r2s_yprt,&xyz);

            /* determine source response to this direction */
			if (!SensorGetResponse(arg->pSimulation->source[si].definition,&xyz,&sourceresponse))
				break;	/* skip receiver if no source response defined for this direction */

            sourceimpulse = NULL;
			switch (sourceresponse.type)
			{
			case SR_LOGGAIN:
                for (b=0; b<arg->pSimulation->nBands; b++)
                    arg->pSimulation->attenuation[b] += sourceresponse.data.loggain;
                break;

			case SR_LOGWEIGHTS:
                for (b=0; b<arg->pSimulation->nBands; b++)
                    arg->pSimulation->attenuation[b] += sourceresponse.data.logweights[b];
                break;

			case SR_IMPULSERESPONSE:
				sourceimpulse = sourceresponse.data.impulseresponse;
			}

#if 0
            switch (arg->pSimulation->source[si].definition->type)
            {
                case ST_GAIN:
                    gain = arg->pSimulation->source[si].definition->probe.gain(&xyz,arg->pSimulation->source[si].definition->data);
                    if (gain==EMPTY_GAIN) 
						continue;
                    for (b=0; b<arg->pSimulation->nBands; b++)
                        arg->pSimulation->attenuation[b] += gain;
                    break;
                    
                case ST_WEIGHTS:
                    weights = arg->pSimulation->source[si].definition->probe.weights(&xyz,arg->pSimulation->source[si].definition->data);
                    if (!weights) 
						continue;
                    /** @todo Interpolate source weights to simulation freq. bands. */
                    for (b=0; b<arg->pSimulation->nBands; b++)
                        arg->pSimulation->attenuation[b] += weights[b];
                    break;
                    
                case ST_RESPONSE:
                    sourceresponse = arg->pSimulation->source[si].definition->probe.response(&xyz,arg->pSimulation->source[si].definition->data);
                    if (!sourceresponse) 
						continue;
            }
#endif
            
			/* convert receiver vector from room to sensor coordinates */
            YawPitchRoll(&W,&arg->pSimulation->receiver[ri].r2s_yprt,&xyz);

            /* determine receiver response to this direction */
			if (!SensorGetResponse(arg->pSimulation->receiver[ri].definition,&xyz,&receiverresponse))
				break;	/* skip receiver if no receiver response defined for this direction */

            receiverimpulse = NULL;
			switch (receiverresponse.type)
			{
			case SR_LOGGAIN:
                for (b=0; b<arg->pSimulation->nBands; b++)
                    arg->pSimulation->attenuation[b] += receiverresponse.data.loggain;
                break;

			case SR_LOGWEIGHTS:
                for (b=0; b<arg->pSimulation->nBands; b++)
                    arg->pSimulation->attenuation[b] += receiverresponse.data.logweights[b];
                break;

			case SR_IMPULSERESPONSE:
				receiverimpulse = receiverresponse.data.impulseresponse;
			}

#if 0
            /* determine receiver response to this direction  */
            receiverresponse = NULL;
            YawPitchRoll(&W,&arg->pSimulation->receiver[ri].r2s_yprt,&xyz);
            switch (arg->pSimulation->receiver[ri].definition->type)
            {
                case ST_GAIN:
                    gain = arg->pSimulation->receiver[ri].definition->probe.gain(&xyz,arg->pSimulation->receiver[ri].definition->data);
                    if (gain==EMPTY_GAIN) continue;
                    for (b=0; b<arg->pSimulation->nBands; b++)
                        arg->pSimulation->attenuation[b] += gain;
                    break;
                    
                case ST_WEIGHTS:
                    weights = arg->pSimulation->receiver[ri].definition->probe.weights(&xyz,arg->pSimulation->receiver[ri].definition->data);
                    if (!weights) continue;
                    /** @todo Interpolate receiver weights to simulation freq. bands. */
                    for (b=0; b<arg->pSimulation->nBands; b++)
                        arg->pSimulation->attenuation[b] += weights[b];
                    break;
                    
                case ST_RESPONSE:
                    receiverresponse = arg->pSimulation->receiver[ri].definition->probe.response(&xyz,arg->pSimulation->receiver[ri].definition->data);
                    if (!receiverresponse) continue;
            }
#endif

            /* combine surfaces, air, distance, source, and receiver weights into single impulse response */
            LogMagFreqResp2MinPhaseFIR(arg->pSimulation->attenuation, arg->pSimulation->h, arg->pSimulation->minphaseplan);
            
            x = arg->pSimulation->h; xlen = NFFT_SIZE;
            y = arg->pSimulation->convbuf;
            nChannels = 1;
            
            /** @todo Apply subsample filter if required. */
            
            /* Convolve arg->pSimulation->h with sourceimpulse and receiverimpulse, if any. */
			/* Note that receiverimpulse could contain 2 channels. */
            if (sourceimpulse)
            {
                h = sourceimpulse; hlen = arg->pSimulation->source[si].definition->nSamples;
                Conv(h, hlen, x, xlen, y);
                ylen = hlen + xlen - 1;
                x = y; xlen = ylen;
                y += ylen;
            }
            
            if (receiverimpulse)
            {
                h = receiverimpulse; hlen = arg->pSimulation->receiver[ri].definition->nSamples;
                Conv(h, hlen, x, xlen, y);
                ylen = hlen + xlen - 1;
                if (arg->pSimulation->receiver[ri].definition->nChannels == 2)
                {
                    Conv(&h[hlen], hlen, x, xlen, &y[ylen]);
                    nChannels = 2;
                }
                x = y; xlen = ylen;
                y += nChannels * ylen;
            }
            
            /* add final impulse response to output room impulse response */
            sr  = ri*arg->pSimulation->nSources + si;
            ofs = ROUND(distance/arg->pSimulation->csample);
            lim = MIN(xlen,arg->pSimulation->brir[sr].nSamples-ofs);
            for (i=0; i<lim; i++)
                arg->pSimulation->brir[sr].sample[ofs+i] += x[i];
            if (nChannels == 2)
                for (i=0; i<lim; i++)
                    arg->pSimulation->brir[sr].sample[arg->pSimulation->brir[sr].nSamples+ofs+i] += x[xlen+i];
            
        } /* next receiver */

    } /* next source */

}

typedef void (*CVirtualRoomCallback)(const CRoomCallbackArg *);

void EnumerateVirtualRooms(const CRoomSetup *pSetup, CRoomsimInternal *pSimulation,
						   int maxx, int maxy, int maxz, CVirtualRoomCallback callback)
{
    int maxorder;
    int x,sx,y,sy,z,sz;
	CRoomCallbackArg arg;

	arg.pSetup = pSetup;
	arg.pSimulation = pSimulation;
    
    maxorder = MAX(maxx,MAX(maxy,maxz));
    
    for (arg.order=0; arg.order<=maxorder; arg.order++)
    {
        for (x=0; x<=arg.order && x<=maxx; x++)
        {
            for (sx=1; sx>-2; sx-=2)
            {
                if ((sx==-1) && (x==0)) break;
                
                /* determine surface counts in X dimension */
                arg.surfacecount[0] = (x>>1) + ((sx<0) & (x&1));
                arg.surfacecount[1] = x - arg.surfacecount[0];
                
                for (y=0; y<=arg.order-x && y<=maxy; y++)
                {
                    for (sy=1; sy>-2; sy-=2)
                    {
                        if ((sy==-1) && (y==0)) break;
                        
                        /* determine surface counts in Y dimension */
                        arg.surfacecount[2] = (y>>1) + ((sy<0) & (y&1));
                        arg.surfacecount[3] = y - arg.surfacecount[2];
                        
                        z = arg.order-x-y;
                        if (z>maxz) break;
                        
                        for (sz=1; sz>-2; sz-=2)
                        {
                            if ((sz==-1) && (z==0)) break;
                            
                            /* determine surface counts in Z dimension */
                            arg.surfacecount[4] = (z>>1) + ((sz<0) & (z&1));
                            arg.surfacecount[5] = z - arg.surfacecount[4];
                            
							arg.rx = sx*x;
							arg.ry = sy*y;
							arg.rz = sz*z;

                            /* invoke callback */
                            callback(&arg);
                            
                        } /* for sz */
                    } /* for sy */
                } /* for y */
            } /* for sx */
        } /* for x */
    } /* order */
}

void InitSimulationWeights(CRoomsimInternal *pSimulation, CSensorDefinition *pSensor)
{
	int i, icount;

	if (pSensor->type == ST_LOGGAIN)
		return;

	/* check existing simulation weights, if any */
		
	if ( (pSensor->nSimulationBands == pSimulation->nBands) /* number of simulation bands same as current simulation? */
         && pSensor->simulationfrequency				    /* and simulation frequency information available? */
		 && pSensor->simulationlogweights					/* and simulation log-weights available? */
	   )
	{
		/* compare simulation band frequencies */
		for (i=0; i<pSensor->nSimulationBands; i++)
			if (pSensor->simulationfrequency[i] != pSimulation->frequency[i])
				break;

		/* all frequencies identical? */
		if (i == pSensor->nSimulationBands)
		{
			/* then simulation weights are properly initialized */
			return;
		}
	}

	/* release existing simulation weights (if any), because they are invalid for current simulation */
	if (pSensor->simulationfrequency)
		MemFree(pSensor->simulationfrequency);
	if (pSensor->simulationlogweights)
		MemFree(pSensor->simulationlogweights);

	/* allocate data */
	pSensor->nSimulationBands	  = pSimulation->nBands;
	pSensor->simulationfrequency  = MemMalloc(pSimulation->nBands * sizeof(double));
	pSensor->simulationlogweights = MemMalloc(pSimulation->nBands * pSensor->nChannels * pSensor->nEntries * sizeof(double));

	/* copy simulation band frequencies to sensor */
	memcpy(pSensor->simulationfrequency, pSimulation->frequency, pSensor->nSimulationBands * sizeof(double));

	icount = pSensor->nEntries * pSensor->nChannels;
	if (pSensor->type == ST_LOGWEIGHTS)
	{
		for (i=0; i<icount; i++)
		{
			/* interpolate from sensor weights */
			LinearInterpolate(
				pSensor->frequency,
				&pSensor->responsedata[i * pSensor->nBands],
				pSensor->nBands,
				pSensor->simulationfrequency,
				&pSensor->simulationlogweights[i * pSensor->nSimulationBands],
				pSensor->nSimulationBands
			);
		}
	}
	else /* pSensor->type == ST_IMPULSERESPONSE */
	{
		/* normalize simulation frequencies for freqz */
		for (i=0; i<pSensor->nSimulationBands; i++)
			pSensor->simulationfrequency[i] *= 2 * PI / pSimulation->fs;

		/* evaluate sensor impulse response at simulation band frequencies */
		for (i=0; i<icount; i++)
		{
			FreqzLogMagnitude(
				&pSensor->responsedata[i * pSensor->nSamples], pSensor->nSamples, 
				pSensor->simulationfrequency, pSensor->nSimulationBands,
				&pSensor->simulationlogweights[i * pSensor->nSimulationBands]
			);
		}

		/* restore simulation band frequencies */
		memcpy(pSensor->simulationfrequency, pSimulation->frequency, pSensor->nSimulationBands * sizeof(double));
	}

#ifdef MEX    
    /* make memory persistent */
    mexMakeMemoryPersistent(pSensor->simulationfrequency);
    mexMakeMemoryPersistent(pSensor->simulationlogweights);
#endif

}

CRoomsimInternal *RoomsimInit(const CRoomSetup *pSetup)
{
    char msg[256];
    int  i, s, r;
	int  nTimebin, nFreqbin, nSpacebin, nBins;
	int  length;

	/* allocate memory for internal simulation data structure */
	CRoomsimInternal *pSimulation = (CRoomsimInternal *) MemMalloc(sizeof(CRoomsimInternal));

	/* copy setup variables to simulation structure */
	pSimulation->fs				 = pSetup->options.fs;
	pSimulation->duration		 = pSetup->options.responseduration;
	length						 = (int) ceil(pSetup->options.responseduration * pSetup->options.fs);
	pSimulation->length			 = length;
	pSimulation->diffusetimestep = pSetup->options.diffusetimestep;

    /* compute speed of sound at given room temperature */
    pSimulation->c       = 331 * sqrt(1 + 0.0036 * pSetup->room.temperature);
    pSimulation->csample = pSimulation->c / pSetup->options.fs;

    /* prepare simulation band frequencies */
    ComputeBandFrequencies(pSetup, pSimulation);
    
	/* compute air absorption coefficients */
	ComputeAirAbsorption(pSetup, pSimulation);

	/* interpolate absorption/diffusion coefficients to simulation frequencies */
    InterpolateAbsorptionAndDiffusion(pSetup, pSimulation);
    /*PrintAbsorptionAndDiffusion(pSetup, pSimulation); */

	/* init local variables */
	nTimebin  = (int) ceil( pSetup->options.responseduration / pSetup->options.diffusetimestep);
	nFreqbin  = pSimulation->nBands;
	nSpacebin = 6; /* front,back,left,right,up,down */
	nBins     = nTimebin * nFreqbin * nSpacebin;

    /* allocate memory for internal source and receiver data */
    pSimulation->nSources   = pSetup->nSources;
    pSimulation->source     = (CSensorInternal *) MemMalloc(pSimulation->nSources * sizeof(CSensorInternal));
    pSimulation->nReceivers = pSetup->nReceivers;
    pSimulation->receiver   = (CSensorInternal *) MemMalloc(pSimulation->nReceivers * sizeof(CSensorInternal));
    
    /* load sources, filling probe callback functions and associated data, and  */
    /* prepare yaw-pitch-roll transformation matrices */
    for (s=0; s<pSetup->nSources; s++)
    {
		pSimulation->source[s].definition = LoadSensor(pSetup->source[s].description);

		/* verify that source sampling frequency matches simulation sampling frequency */
        if (!(pSimulation->source[s].definition->fs == ANY_FS 
			|| pSimulation->source[s].definition->fs == pSimulation->fs))
        {
            sprintf(msg,"sampling frequency mismatch for source '%s'\n(simulation Fs=%.f Hz, source Fs=%.f Hz)", 
                            pSetup->source[s].description, pSetup->options.fs, pSimulation->source[s].definition->fs);
            MsgErrorExit(msg);
        }

		/* compute coordinate transformation matrices */
        ComputeRoom2SensorYPRT((YPR *)pSetup->source[s].orientation, (YPRT *)&pSimulation->source[s].r2s_yprt);
        ComputeSensor2RoomYPRT((YPR *)pSetup->source[s].orientation, (YPRT *)&pSimulation->source[s].s2r_yprt);

		/* prepare source's simulation frequency band weights */
		InitSimulationWeights(pSimulation, pSimulation->source[s].definition);
    }

    /* load receivers, filling probe callback functions and associated data, and  */
    /* prepare yaw-pitch-roll transformation matrices */
    for (r=0; r<pSetup->nReceivers; r++)
    {
        pSimulation->receiver[r].definition = LoadSensor(pSetup->receiver[r].description);

		/* verify that receiver sampling frequency matches simulation sampling frequency */
        if (!(pSimulation->receiver[r].definition->fs == ANY_FS 
			|| pSimulation->receiver[r].definition->fs == pSimulation->fs))
        {
            sprintf(msg,"sampling frequency mismatch for receiver '%s'\n(simulation Fs=%.f Hz, receiver Fs=%.f Hz)", 
                            pSetup->receiver[r].description, pSetup->options.fs, pSimulation->receiver[r].definition->fs);
            MsgErrorExit(msg);
        }

		/* compute coordinate transformation matrices */
        ComputeRoom2SensorYPRT((YPR *)pSetup->receiver[r].orientation, (YPRT *)&pSimulation->receiver[r].r2s_yprt);
        ComputeSensor2RoomYPRT((YPR *)pSetup->receiver[r].orientation, (YPRT *)&pSimulation->receiver[r].s2r_yprt);

		/* prepare receiver's simulation frequency band weights */
		InitSimulationWeights(pSimulation, pSimulation->receiver[r].definition);

		/* allocate and initialize time-frequency-space histogram */
		pSimulation->receiver[r].nTbin = nTimebin;
		pSimulation->receiver[r].nFbin = nFreqbin;
		pSimulation->receiver[r].nSbin = nSpacebin;
		pSimulation->receiver[r].TFShist = (double *) MemCalloc(nBins, sizeof(double));

		/* allocate and initialize first time-of-arrival array */
		pSimulation->receiver[r].FirstTOA = (double *) MemMalloc(nSpacebin * sizeof(double));
		for (i=0; i<nSpacebin; i++)
			pSimulation->receiver[r].FirstTOA[i] = 10000.0;
    }
    
    /* allocate plan for converting log magnitude frequency response to minimum phase filter */
    pSimulation->minphaseplan = AllocMinPhaseFIRplan(NFFT_SIZE, pSimulation->frequency, pSimulation->nBands);
    pSimulation->h = MemMalloc(NFFT_SIZE * sizeof(double));
    
    /* allocate internal attenuation vectors */
    pSimulation->surfaceattenuation = (double *)MemMalloc(pSimulation->nBands * sizeof(double));
    pSimulation->attenuation		= (double *)MemMalloc(pSimulation->nBands * sizeof(double));

    /* allocate memory for convolution results */
    { 
        int maxslen = 0, maxrlen = 0;
        int len   = NFFT_SIZE;
        int total = 0;
        
        for (s=0; s<pSimulation->nSources; s++)
            if (pSimulation->source[s].definition->type == ST_IMPULSERESPONSE)
                maxslen = MAX(maxslen,pSimulation->source[s].definition->nSamples);

        for (r=0; r<pSimulation->nReceivers; r++)
            if (pSimulation->receiver[r].definition->type == ST_IMPULSERESPONSE)
                maxrlen = MAX(maxrlen,pSimulation->receiver[r].definition->nSamples);
        
        if (maxslen > 0) { len += maxslen; total += len;   }
        if (maxrlen > 0) { len += maxrlen; total += len*2; }
        
        pSimulation->convbuf = MemMalloc(total * sizeof(double));
    }

	/* allocate memory for time-varying filter */
	pSimulation->htv = (double *)MemMalloc(nTimebin * NFFT_SIZE * sizeof(double));

	/* setup time-varying index array */
	pSimulation->htvidx = (int *)MemMalloc(nTimebin * sizeof(int));
	for (i=0; i<nTimebin; i++)
		pSimulation->htvidx[i] = ROUND(i * pSimulation->diffusetimestep * pSimulation->fs);

	/* allocate memory for noise signal and processed versions */
	/* uses factor of 2 to accomodate stereo signals */
	pSimulation->noise                  = MemMalloc(((2*length+3)&-4) * sizeof(unsigned int));
	pSimulation->shapednoise            = MemMalloc(2*length * sizeof(double));
	pSimulation->directionalshapednoise = MemMalloc(2*length * sizeof(double));

    /* allocate memory for BRIR matrix */
    pSimulation->brir = (BRIR *)MemCalloc(pSimulation->nSources * pSimulation->nReceivers + 1, sizeof(BRIR));
    
    /* initialize structure and allocate memory for all source/receiver combinations */
    for (s=0; s<pSimulation->nSources; s++)
    {        
        for (r=0; r<pSimulation->nReceivers; r++)
        {
            i = r * pSimulation->nSources + s; /* s*pSimulation->nReceivers + r; */
            
            pSimulation->brir[i].fs = pSetup->options.fs;
                        
            pSimulation->brir[i].nChannels = pSimulation->receiver[r].definition->nChannels; 
            pSimulation->brir[i].nSamples  = pSimulation->length;
            
            /* allocate memory for impulse response and set to zero */
            pSimulation->brir[i].sample = (double *) MemCalloc(pSimulation->brir[i].nChannels * pSimulation->brir[i].nSamples, sizeof(double));
        }
    }

	/* initialize random number generator */
	RngInit();

	return pSimulation;
}

BRIR *RoomsimRelease(CRoomsimInternal *pSimulation)
{
	int i;
	BRIR *retval = pSimulation->brir;	/* save brir pointer  */

    /* free minimum phase plan and all allocated memory */
    FreeMinPhaseFIRplan(pSimulation->minphaseplan);

	/* free simulation memory  */
    MemFree(pSimulation->h);
    MemFree(pSimulation->convbuf);
    MemFree(pSimulation->frequency);
    MemFree(pSimulation->logreflection);
    MemFree(pSimulation->logabsorption);
    MemFree(pSimulation->logdiffusion);
    MemFree(pSimulation->logdiffusereflection);
    MemFree(pSimulation->logspecularreflection);
    MemFree(pSimulation->diffusioncoefficient);
    MemFree(pSimulation->logairattenuation);
	MemFree(pSimulation->surfaceattenuation);
    MemFree(pSimulation->attenuation);
	MemFree(pSimulation->htv);
	MemFree(pSimulation->htvidx);
	MemFree(pSimulation->noise);
	MemFree(pSimulation->shapednoise);
	MemFree(pSimulation->directionalshapednoise);

	/* free receiver histogram memory */
	for (i=0; i<pSimulation->nReceivers; i++)
	{
		MemFree(pSimulation->receiver[i].TFShist);
		MemFree(pSimulation->receiver[i].FirstTOA);
	}

	/* free simulation structure */
	MemFree(pSimulation);

    return retval;
}

void ReleaseBRIR(BRIR *brir)
{
	int i=0;

	while (brir[i].sample)
		MemFree(brir[i++].sample);

	MemFree(brir);
}

/** Generates unit vectors equally distributed around a sphere. 
 *
 *	@param[in]  nDesiredRays	Number of desired rays.
 *	@param[out] pnActualRays	Number of rays actually generated.
 *	@return						XYZ array of unit vectors.
 *
 *  @note
 *     The number of rays N that is actually generated satisfies N = 20*k*k, where
 *     k is the smallest integer such that N >= nDesiredRays.
 *  @note
 *     The generating algorithm subdivides each face of a regular icosahedron into 
 *     a triangular grid, whose granularity is determined by the number of desired 
 *     rays. The algorithm then enumerates the triangular cells of the grid and 
 *     takes the center of the grid cell as a ray direction.
 * 
 *                C             Triangle ABC is one face of the icosahedron, which 
 *              ./_\.           is subdivided into a triangular grid. The grid is 
 *            ./_\./_\          enumerated from left-to-right, bottom-to-top, 
 *          ./_\./_\./_\.       starting at A.
 *         A             B
 */
XYZ *GenerateRays(int nDesiredRays, int *pnActualRays)
{
	/** @todo Rotate vertices over small yaw,pitch,roll to avoid alignment with room axes. */

	/* xyz-coordinates of vertices of regular icosahedron */
	static const XYZ ico_vert[12] = 
	{
		{ 0.0000000000,  0.0000000000,  1.0000000000},
		{ 0.8944271910,  0.0000000000,  0.4472135955},
		{ 0.2763932023,  0.8506508084,  0.4472135955},
		{-0.7236067977,  0.5257311121,  0.4472135955},
		{-0.7236067977, -0.5257311121,  0.4472135955},
		{ 0.2763932023, -0.8506508084,  0.4472135955},
		{ 0.7236067977,  0.5257311121, -0.4472135955},
		{-0.2763932023,  0.8506508084, -0.4472135955},
		{-0.8944271910,  0.0000000000, -0.4472135955},
		{-0.2763932023, -0.8506508084, -0.4472135955},
		{ 0.7236067977, -0.5257311121, -0.4472135955},
		{ 0.0000000000,  0.0000000000, -1.0000000000}
	};
	/* vertex indices of faces of regular icosahedron */
	static const int ico_face[20][3] = 
	{
		{ 0,  1,  2}, { 0,  2,  3}, { 0,  3,  4}, { 0,  4,  5},
		{ 0,  5,  1}, { 1,  2,  6}, { 2,  6,  7}, { 2,  3,  7},
		{ 3,  7,  8}, { 3,  4,  8}, { 4,  8,  9}, { 4,  5,  9},
		{ 5,  9, 10}, { 5,  1, 10}, { 1, 10,  6}, { 6,  7, 11},
		{ 7,  8, 11}, { 8,  9, 11}, { 9, 10, 11}, {10,  6, 11}
	};

	int    rayorder = (int) ceil(sqrt(nDesiredRays / 20.0));
	int    nRays = rayorder * rayorder * 20;
	XYZ    *rayxyz;
	XYZ    A, B, C, dAB, dAC;
	XYZ    p1, p2, p3;
	int	   f, nSteps, i1, i2, idx=0;
	double norm;

	/* allocate memory for ray vectors */
	rayxyz = MemMalloc(nRays * sizeof(XYZ));
	*pnActualRays = nRays;

	/* loop over all faces of the icosahedron */
	for (f=0; f<20; f++)
	{
		/* get xyz-coords of vertices of current face */
		A = ico_vert[ico_face[f][0]];
		B = ico_vert[ico_face[f][1]];
		C = ico_vert[ico_face[f][2]];

		/* A, B, and C are the coordinates of a triangle in 3D space */

		/* determine grid spacing in A-B direction */
		dAB.x = (B.x - A.x) / rayorder;
		dAB.y = (B.y - A.y) / rayorder;
		dAB.z = (B.z - A.z) / rayorder;

		/* determine grid spacing in A-C direction */
		dAC.x = (C.x - A.x) / rayorder;
		dAC.y = (C.y - A.y) / rayorder;
		dAC.z = (C.z - A.z) / rayorder;

		/* initialize number of grid cells for enumeration */
		nSteps = 2*rayorder - 1;

		/** @todo Speed up algorithm by updating p = (p1+p2+p3)/3 
		rather than updating p1, p2, and p3 individually */

		for (i1=0; i1<rayorder; i1++)
		{
			/* initialize points p1, p2, and p3 */
			p1.x = A.x + i1 * dAC.x;
			p1.y = A.y + i1 * dAC.y;
			p1.z = A.z + i1 * dAC.z;

			p2.x = p1.x + dAB.x;
			p2.y = p1.y + dAB.y;
			p2.z = p1.z + dAB.z;

			p3.x = p1.x + dAC.x;
			p3.y = p1.y + dAC.y;
			p3.z = p1.z + dAC.z;

			/* p1, p2, and p3 are coordinates of grid cell in 3D space */

			for (i2=0; i2<nSteps; i2++)
			{
				/* use center of triangular grid cell as ray's direction  */
				rayxyz[idx].x = (p1.x + p2.x + p3.x) / 3;
				rayxyz[idx].y = (p1.y + p2.y + p3.y) / 3;
				rayxyz[idx].z = (p1.z + p2.z + p3.z) / 3;
            
				/* normalize length to unity */
				norm = sqrt(rayxyz[idx].x * rayxyz[idx].x + 
							rayxyz[idx].y * rayxyz[idx].y + 
							rayxyz[idx].z * rayxyz[idx].z);

				rayxyz[idx].x /= norm;
				rayxyz[idx].y /= norm;
				rayxyz[idx].z /= norm;

				idx = idx + 1;
            
				/* update p1, p2, and p3 to move to next grid cell */
				if ((i2 % 2) == 0)
				{
					p1 = p2;
					p2.x = p3.x + dAB.x;
					p2.y = p3.y + dAB.y;
					p2.z = p3.z + dAB.z;
				}
				else
				{
					p3 = p2;
					p2.x = p1.x + dAB.x;
					p2.y = p1.y + dAB.y;
					p2.z = p1.z + dAB.z;
				}
			}

			/* for face's next level, reduce number of grid cells by 2 */
			nSteps = nSteps - 2;
		}

	} /* next face */

#if 0
/* DEBUG OUTPUT  */
	{ 
		int i;
		char filename[256];
		FILE *fid;

		sprintf(filename,"rays%04d.txt", nRays);
		fid = fopen(filename,"w");

		/* print vertices */
		fprintf(fid,"12 12 12\n");
		for (i=0; i<12; i++)
		{
			fprintf(fid,"%13.10f %13.10f %13.10f\n", ico_vert[i].x, ico_vert[i].y, ico_vert[i].z);
		}

		/* print faces */
		fprintf(fid,"20 20 20\n");
		for (i=0; i<20; i++)
		{
			fprintf(fid,"%2d %2d %2d\n", ico_face[i][0]+1, ico_face[i][1]+1, ico_face[i][2]+1);
		}

		/* print rays */
		fprintf(fid,"%d %d 0\n", idx, nRays);
		for (i=0; i<nRays; i++)
		{
			fprintf(fid,"%13.10f %13.10f %13.10f\n", rayxyz[i].x, rayxyz[i].y, rayxyz[i].z);
		}
		fclose(fid);
	}
/* END DEBUG OUTPUT */
#endif

	return rayxyz;
}

void AddDiffuseEnergy(CRoomsimInternal *pSimulation, int iReceiver, 
					  double recv_timeofarrival, 
					  XYZ    *recvrayvector, 
					  int    iBand, 
					  double recv_logenergy)
{
	int    sbin, tbin;
	double x2y2, z2;

	/* quantize time of arrival to temporal receiver histogram bin */
	tbin = (int) floor(recv_timeofarrival / pSimulation->diffusetimestep + 0.5);
	/* ignore energy contributions that fall outside the histogram */
	if (tbin >= pSimulation->receiver[iReceiver].nTbin)
		return;

	/* quantize ray direction to spatial receiver histogram bin */
	z2   = recvrayvector->z * recvrayvector->z;
	x2y2 = recvrayvector->x * recvrayvector->x + recvrayvector->y * recvrayvector->y;
	if (z2 > (4.0/9.0) * (x2y2 + z2))
	{
		/* pick vertical bin */
		/* 4=lower, 5=upper */
		sbin = 4 + (recvrayvector->z > 0);
	}
	else
	{
		/* pick horizontal bin */
		/* 0=back, 1=left, 2=right, 3=front; */
		sbin = 2*(recvrayvector->x > recvrayvector->y) + (recvrayvector->x > -recvrayvector->y);
	}

	/* keep track of first arrival in each spatial bin */
	if (recv_timeofarrival < pSimulation->receiver[iReceiver].FirstTOA[sbin])
		pSimulation->receiver[iReceiver].FirstTOA[sbin] = recv_timeofarrival;

	/* add energy to receiver histogram bin */
	RECV_TFS_BIN(pSimulation->receiver[iReceiver],tbin,iBand,sbin) += LINDOMAIN(recv_logenergy);
}

void MakeUnitVector(XYZ *xyz)
{
	double norm = 
		sqrt(xyz->x * xyz->x + 
			 xyz->y * xyz->y + 
			 xyz->z * xyz->z);
	if (norm == 0.0) return;
	xyz->x /= norm;
	xyz->y /= norm;
	xyz->z /= norm;
}

void RoomsimDiffuse(const CRoomSetup *pSetup, CRoomsimInternal *pSimulation)
{
	XYZ     *ray;

	/* loop counters */
	int	iSource, iBand, iRay, iReceiver;
	int iTimebin, iDirection;
	int	nRays;

	/* ray-tracing variables */
	XYZ		ray_xyz, ray_dxyz, impact_xyz;
	XYZ		rayrecvvector, recvrayvector, rs, rd;
	double	ray_time, timetoimpact, t, recv_timeofarrival, endtime;
	double	ray_logenergy, ray_logenergymin, rayrecv_logenergy, recv_logenergy;
	double  distance, d, vn=0.0, vf=0.0, v1, v2, v3, wd, ws, temp;
	int		surfaceofimpact;

	/* diffuse generation variables */
	unsigned int noisethreshold;
	double	*TFSbase;
	int		nTimebin, nFreqbin, nBins, nRecvCh;
	int		i, SRidx, length=0, receiverimpulselength;
	const double *receiverimpulse;
	double	gain;
	CSensorResponse receiverresponse;

#ifdef LOGRAYS
	FILE *fid, *fidrecv;
#endif

#if 0
	/* prepare internal room simulation data structure */
	CRoomsimInternal *pSimulation = RoomsimInit(pSetup);
#endif

	/* generate rays */
	ray = GenerateRays(pSetup->options.numberofrays,&nRays);
	if (nRays != pSetup->options.numberofrays && pSetup->options.verbose)
    {
		MsgPrintf("ray tracing: number of rays changed to %d\n", nRays);
        MsgRelax;
    }

/* TEMP 
	{
		CMinPhaseFIRplan *plan;
		double F[] = {0,125,250,500,1000,2000,4000,8000,16000,22050};
		double A[] = {0,  0,  0,  0,   0,   0,   0,   0,    0,    0};
		unsigned int nF = sizeof(F)/sizeof(F[0]);
		double h[512];
		int i;
		FILE *fid;

		plan = AllocMinPhaseFIRplan(512, F, nF);
		LogMagFreqResp2MinPhaseFIR(A, h, plan);
		fid = fopen("hflat.txt","w");
		for (i=0; i<512; i++)
			fprintf(fid,"%13.10f\n", h[i]);
		fclose(fid);
		FreeMinPhaseFIRplan(plan);
	}
TEMP */

	ray_logenergymin = -LOGDOMAIN(nRays) + LOGDOMAIN(pow(10,pSetup->options.rayenergyfloordB/20));
	endtime = pSetup->options.responseduration;
	noisethreshold = (unsigned int) ((10000.0 / pSimulation->fs) * 4294967295.0);


#ifdef LOGRAYS
	fid = fopen("raylog.txt","w");
	fidrecv = fopen("recvlog.txt","w");
	fprintf(fidrecv,"%%ray   recv   ray->recv vector                factor      time        recv->ray vector                logenergy \n"
		            "%%---   ----   -----------------------------   ---------   ---------   -----------------------------   ----------\n");
#endif

	/* loop over all sound sources */
    for (iSource=0; iSource<pSimulation->nSources; iSource++)
	{
		/* clear receivers' TFS histogram */
		for (iReceiver=0; iReceiver<pSetup->nReceivers; iReceiver++)
		{
			nBins = pSimulation->receiver[iReceiver].nTbin * 
					pSimulation->receiver[iReceiver].nFbin * 
					pSimulation->receiver[iReceiver].nSbin;
			memset(pSimulation->receiver[iReceiver].TFShist,0,nBins * sizeof(double));
		}

		/************************
		 * STAGE 1: RAY TRACING *
		 ************************/

		/* loop over all frequency bands */
		for (iBand=0; iBand<pSimulation->nBands; iBand++)
		{
			/* loop over all sound rays */
			for (iRay=0; iRay<nRays; iRay++)
			{
				/* load initial ray position */
				ray_xyz.x = pSetup->source[iSource].location[0];
				ray_xyz.y = pSetup->source[iSource].location[1];
				ray_xyz.z = pSetup->source[iSource].location[2];

				/* load initial ray direction */
				ray_dxyz = ray[iRay];

				/* initialize ray time */
				ray_time = 0;

				/* initialize ray energy */
				ray_logenergy = -LOGDOMAIN(nRays);

				/* apply source directivity to ray energy. */
				ray_logenergy += SensorGetLogGain(pSimulation->source[iSource].definition, &ray_dxyz, iBand);

				/* convert ray direction from source coords to room coords */
				YawPitchRoll_InPlace(&ray_dxyz, &(pSimulation->source[iSource].s2r_yprt));

				/* inifinite loop, terminates when ray time exceeds */
				/* response duration, or when ray energy drops below threshold */
				for (;;)
				{
				/*
				 * determine time and surface of impact
				 */
					timetoimpact = 1000.0;

                    surfaceofimpact = -1;

					/* compute time to intersection with x-surfaces */
					if (ray_dxyz.x < 0)
					{
						timetoimpact = -ray_xyz.x / ray_dxyz.x; 
						surfaceofimpact = 0;
					}
					else if (ray_dxyz.x > 0)
					{
						timetoimpact = (pSetup->room.dimension[0] - ray_xyz.x) / ray_dxyz.x;
						surfaceofimpact = 1;
					}
					/* compute time to intersection with y-surfaces */
					if (ray_dxyz.y < 0)
					{
						t = -ray_xyz.y / ray_dxyz.y; 
						if (t < timetoimpact)
						{
							surfaceofimpact = 2;
							timetoimpact = t;
						}
					}
					else if (ray_dxyz.y > 0)
					{
						t = (pSetup->room.dimension[1] - ray_xyz.y) / ray_dxyz.y;
						if (t < timetoimpact)
						{
							surfaceofimpact = 3;
							timetoimpact = t;
						}
					}
					/* compute time to intersection with z-surfaces */
					if (ray_dxyz.z < 0)
					{
						t = -ray_xyz.z / ray_dxyz.z; 
						if (t < timetoimpact)
						{
							surfaceofimpact = 4;
							timetoimpact = t;
						}
					}
					else if (ray_dxyz.z > 0)
					{
						t = (pSetup->room.dimension[2] - ray_xyz.z) / ray_dxyz.z;
						if (t < timetoimpact)
						{
							surfaceofimpact = 5;
							timetoimpact = t;
						}
					}

                    if (surfaceofimpact==-1)
                        MsgErrorExit("INTERNAL ERROR: no surface of impact found for current ray");

					/* determine length of ray segment */
					rs.x = timetoimpact * ray_dxyz.x;
					rs.y = timetoimpact * ray_dxyz.y;
					rs.z = timetoimpact * ray_dxyz.z;
					distance = sqrt(rs.x*rs.x + rs.y*rs.y+rs.z*rs.z);

					/* determine location of impact */
					impact_xyz.x = ray_xyz.x + rs.x;
					impact_xyz.y = ray_xyz.y + rs.y;
					impact_xyz.z = ray_xyz.z + rs.z;

#ifdef LOGRAYS
					if (iSource==0 && iBand==0)
					{
						fprintf(fid,"%4d    %9.6f %9.6f %9.6f    %9.6f %9.6f %9.6f    %9.6f    %10.6f\n",
							iRay, ray_xyz.x, ray_xyz.y, ray_xyz.z, ray_dxyz.x, ray_dxyz.y, ray_dxyz.z,
							ray_time, ray_logenergy);
#  ifdef LOGRAYS_EXTRA
						fprintf(fid,"                                        %% soi %d   rs %9.6f %9.6f %9.6f   d %9.6f   imp %9.6f %9.6f %9.6f\n",
							surfaceofimpact, rs.x, rs.y, rs.z, distance, impact_xyz.x, impact_xyz.y, impact_xyz.z);
#  endif
					}
#  ifdef LOGRAYS_EXTRA
					if (distance==0.0) 
					{
						fprintf(fid,"%% DISTANCE FELL TO ZERO, BREAKING\n");
						break;
					}
#  endif
#endif

					/** @todo Apply air absorption? */

					/* update ray location */
					ray_xyz = impact_xyz;

					/* update ray time */
					ray_time += distance / pSimulation->c;

					/* quit ray when simulation time exceeded */
					if (ray_time > endtime)
					{
#ifdef LOGRAYS
						if (iSource==0 && iBand==0)
							fprintf(fid,"%% ray time exceeded: %9.6f\n", ray_time);
#endif
						break;
					}

					/* apply surface absorption to ray's energy */
					ray_logenergy += SURFACELOGREFLECTION(pSimulation,surfaceofimpact,iBand);

					/* quit ray when energy drops below threshold */
					if (ray_logenergy < ray_logenergymin)
					{
#ifdef LOGRAYS
						if (iSource==0 && iBand==0)
							fprintf(fid,"%% ray energy depleted: %9.6f\n", ray_logenergy);
#endif
						break;
					}

					/* apply diffuse reflection to ray energy */
					rayrecv_logenergy = ray_logenergy + SURFACELOGDIFFUSION(pSimulation,surfaceofimpact,iBand);

					/* extend ray to all receivers */
					for (iReceiver=0; iReceiver<pSetup->nReceivers; iReceiver++)
					{
						/* determine ray->receiver vector */
						rayrecvvector.x = pSetup->receiver[iReceiver].location[0] - impact_xyz.x;
						rayrecvvector.y = pSetup->receiver[iReceiver].location[1] - impact_xyz.y;
						rayrecvvector.z = pSetup->receiver[iReceiver].location[2] - impact_xyz.z;

						/* determine ray's time of arrival at receiver */
						distance = sqrt(rayrecvvector.x * rayrecvvector.x + 
										rayrecvvector.y * rayrecvvector.y + 
										rayrecvvector.z * rayrecvvector.z); 
						recv_timeofarrival = ray_time + distance / pSimulation->c;

						/* skip this receiver if ray arrives too late */
						if (recv_timeofarrival > endtime)
							continue;

						/* determine amount of diffuse energy that reaches the receiver */
						switch (surfaceofimpact)
						{
						case 0:
							vn = rayrecvvector.x; 
							vf = rayrecvvector.y*rayrecvvector.y + rayrecvvector.z*rayrecvvector.z;
							break;
						case 1:
							vn = -rayrecvvector.x; 
							vf = rayrecvvector.y*rayrecvvector.y + rayrecvvector.z*rayrecvvector.z;
							break;
						case 2:
							vn = rayrecvvector.y; 
							vf = rayrecvvector.x*rayrecvvector.x + rayrecvvector.z*rayrecvvector.z;
							break;
						case 3:
							vn = -rayrecvvector.y; 
							vf = rayrecvvector.x*rayrecvvector.x + rayrecvvector.z*rayrecvvector.z;
							break;
						case 4:
							vn = rayrecvvector.z; 
							vf = rayrecvvector.x*rayrecvvector.x + rayrecvvector.y*rayrecvvector.y;
							break;
						case 5:
							vn = -rayrecvvector.z; 
							vf = rayrecvvector.x*rayrecvvector.x + rayrecvvector.y*rayrecvvector.y;
							break;
						}
						v1 = vn*vn;
						v2 = v1 + vf;
						v3 = v2 * sqrt(v2);
						d = (distance < 1.0 ? 1.0 : distance);
						recv_logenergy = rayrecv_logenergy + LOGDOMAIN(v1 * vn / v3 / (d * d));

						/* apply air absorption if requested */
						if (pSetup->options.airabsorption)
						{
							recv_logenergy += (recv_timeofarrival * pSimulation->c) * pSimulation->logairattenuation[iBand];
						}

						/* convert ray-receiver vector to receiver-ray vector */
						recvrayvector.x = -rayrecvvector.x;
						recvrayvector.y = -rayrecvvector.y;
						recvrayvector.z = -rayrecvvector.z;

						/* convert recv-ray vector from room coords to receiver coords */
						YawPitchRoll_InPlace(&recvrayvector, &pSimulation->receiver[iReceiver].r2s_yprt);

#ifdef LOGRAYS
					if (iSource==0 && iBand==0)
					{
						fprintf(fidrecv,"%4d   %4d   %9.6f %9.6f %9.6f   %9.6f   %9.6f   %9.6f %9.6f %9.6f    %10.6f\n",
							iRay, iReceiver, 
							rayrecvvector.x, rayrecvvector.y, rayrecvvector.z, 
							LOGDOMAIN(v1 * vn / v3 / (d * d)), recv_timeofarrival, 
							recvrayvector.x, recvrayvector.y, recvrayvector.z, 
							recv_logenergy);
					}
#endif
						/* add ray energy to receiver histogram */
						AddDiffuseEnergy(pSimulation, iReceiver, recv_timeofarrival, &recvrayvector, iBand, recv_logenergy);
					}

				/*
				 * Pick new direction for current ray
				 */
					
					/* select random unit vector from lambert distribution */
					RngLambert(&rd);
					switch (surfaceofimpact)
					{
					case 0: temp = rd.x; rd.x =  rd.z; rd.z = temp; rs.x = -rs.x; break;
					case 1: temp = rd.x; rd.x = -rd.z; rd.z = temp; rs.x = -rs.x; break;
					case 2: temp = rd.y; rd.y =  rd.z; rd.z = temp; rs.y = -rs.y; break;
					case 3: temp = rd.y; rd.y = -rd.z; rd.z = temp; rs.y = -rs.y; break;
					case 4:											rs.z = -rs.z; break;
					case 5: rd.z = -rd.z;							rs.z = -rs.z; break;
					}
					MakeUnitVector(&rd);
					MakeUnitVector(&rs);

					/* mix random/specular vectors using diffuse weighting */
					wd = SURFACEDIFFUSIONCOEFFICIENT(pSimulation,surfaceofimpact,iBand);
					ws = 1.0 - wd;
					ray_dxyz.x = wd * rd.x + ws * rs.x;
					ray_dxyz.y = wd * rd.y + ws * rs.y;
					ray_dxyz.z = wd * rd.z + ws * rs.z;

#ifdef LOGRAYS_EXTRA
					if (iSource==0 && iBand==0)
					{
						fprintf(fid,"                                        "
							"%% wd %9.6f   rd %9.6f %9.6f %9.6f\n", wd, rd.x, rd.y, rd.z);
						fprintf(fid,"                                        "
							"%% ws %9.6f   rs %9.6f %9.6f %9.6f\n", ws, rs.x, rs.y, rs.z);
					}
#endif
				} /* continue tracing current ray */

			} /* next ray */

		} /* next frequency band */

#ifdef LOGTFS
		if (iSource==0)
		{
			int i,j,k;
			FILE *fidtfs;
			fidtfs = fopen("tfslog.txt","w");
			fprintf(fidtfs,"%d %d %d\n", 
				pSimulation->receiver[0].nTbin,
				pSimulation->receiver[0].nFbin,
				pSimulation->receiver[0].nSbin);
			for (i=0; i<pSimulation->receiver[0].nSbin; i++)
			{
				for (j=0; j<pSimulation->receiver[0].nFbin; j++)
				{
					for (k=0; k<pSimulation->receiver[0].nTbin; k++)
					{
						fprintf(fidtfs, "%13.10f ", RECV_TFS_BIN(pSimulation->receiver[0],k,j,i));
					}
					fprintf(fidtfs,"\n");
				}
			}
			fclose(fidtfs);
		}
#endif

		/****************************************
		 * STAGE 2: REVERBERANT TAIL GENERATION *
		 ****************************************/

#ifdef LOGTAIL
		{
			FILE *fidtail = fopen("tail.bin", "wb");
#endif
		/* loop over receivers */
		for (iReceiver=0; iReceiver<pSetup->nReceivers; iReceiver++)
		{
            if (pSetup->options.verbose)
            {
                MsgPrintf("Generating reverberant tail from source %d to receiver %d...\n",
                    iSource+1, iReceiver+1);
                MsgRelax;
            }
            
			SRidx = iReceiver * pSimulation->nSources + iSource;

			/* loop over spatial groups */
			for (iDirection=0; iDirection<6; iDirection++)
			{
				/* determine receiver response to current direction */
				if (!SensorGetResponse(pSimulation->receiver[iReceiver].definition,
					&SpaceBinCenter[iDirection],&receiverresponse))
				{
					/* skip direction if no receiver response defined */
					continue;	
				}

				TFSbase  = &(RECV_TFS_BIN(pSimulation->receiver[iReceiver],0,0,iDirection));
				nTimebin = pSimulation->receiver[iReceiver].nTbin;
				nFreqbin = pSimulation->receiver[iReceiver].nFbin;
				length   = pSimulation->length;
				nRecvCh  = pSimulation->receiver[iReceiver].definition->nChannels;

#ifdef LOGTAIL
				fwrite(TFSbase,sizeof(double),nTimebin*nFreqbin,fidtail);
#endif /* LOGTAIL */

				/* convert TFS histogram to log domain */
				for (i=0; i<nTimebin*nFreqbin; i++)
				{
					TFSbase[i] = LOGDOMAINSAFE(TFSbase[i]);
				}

#ifdef LOGTAIL
				fwrite(TFSbase,sizeof(double),nTimebin*nFreqbin,fidtail);
#endif /* LOGTAIL */

				/* convert TFS histogram to time-varying filter */
				for (iTimebin=0; iTimebin<nTimebin; iTimebin++)
				{
					LogMagFreqResp2MinPhaseFIR(TFSbase + iTimebin * nFreqbin,
						&pSimulation->htv[iTimebin*NFFT_SIZE], pSimulation->minphaseplan);
				}
				
#ifdef LOGTAIL
				fwrite(pSimulation->htv,sizeof(double),nTimebin*NFFT_SIZE,fidtail);
#endif /* LOGTAIL */

				/* generate noise signal */
				RngFill_uint32(pSimulation->noise, (nRecvCh*length+3)&-4);

#ifdef LOGTAIL
				fwrite(pSimulation->noise,sizeof(pSimulation->noise[0]),nRecvCh*length,fidtail);
#endif /* LOGTAIL */

				/* apply time-varying filter to noise signal */
				TimeVaryingConv(pSimulation->htv, NFFT_SIZE, 
					 pSimulation->htvidx, nTimebin, 
					 pSimulation->noise, length, 
					 ROUND(pSimulation->receiver[iReceiver].FirstTOA[iDirection] * pSimulation->fs),
					 noisethreshold, pSimulation->shapednoise);

				if (nRecvCh==2 && pSetup->options.uncorrelatednoise)
				{
					/* apply time-varying filter to second channel of noise signal */
					TimeVaryingConv(pSimulation->htv, NFFT_SIZE, 
						 pSimulation->htvidx, nTimebin, 
						 pSimulation->noise+length, length, 
						 ROUND(pSimulation->receiver[iReceiver].FirstTOA[iDirection] * pSimulation->fs),
						 noisethreshold,pSimulation->shapednoise+length);
				}

#ifdef LOGTAIL
				fwrite(pSimulation->shapednoise,sizeof(pSimulation->shapednoise[0]),length,fidtail);
#endif /* LOGTAIL */

				receiverimpulse = NULL;
				receiverimpulselength = 0;
				switch (receiverresponse.type)
				{
				case SR_LOGGAIN:
					gain = LINDOMAIN(receiverresponse.data.loggain);

					for (i=0; i<length; i++)
						pSimulation->directionalshapednoise[i] = pSimulation->shapednoise[i] * gain;

					if (nRecvCh==2 && pSetup->options.uncorrelatednoise)
					{
						for (i=length; i<2*length; i++)
							pSimulation->directionalshapednoise[i] = pSimulation->shapednoise[i] * gain;
					}
					break;

				case SR_LOGWEIGHTS:
					/* convert receiver weights to impulse response */
					LogMagFreqResp2MinPhaseFIR(receiverresponse.data.logweights,
						pSimulation->h, pSimulation->minphaseplan);
					receiverimpulse = pSimulation->h;
					receiverimpulselength = NFFT_SIZE;
					break;

				case SR_IMPULSERESPONSE:
					receiverimpulse = receiverresponse.data.impulseresponse;
					receiverimpulselength = pSimulation->receiver[iReceiver].definition->nSamples;
					break;
				}

				if (receiverimpulse)
				{
					/* apply receiver directional filter to shaped noise signal */
					FIRfilter(
						receiverimpulse, receiverimpulselength,			/* filter */
						pSimulation->shapednoise, length,				/* signal */
						pSimulation->directionalshapednoise,			/* output */
						NULL											/* state */
					);

					/* handle binaural receiver */
					if (nRecvCh==2)
					{
						receiverimpulse += receiverimpulselength;
						if (pSetup->options.uncorrelatednoise)
						{
							FIRfilter(
								receiverimpulse, receiverimpulselength,			/* filter */
								pSimulation->shapednoise+length, length,	    /* signal */
								pSimulation->directionalshapednoise + length,	/* output */
								NULL											/* state */
							);
						}
						else
						{
							FIRfilter(
								receiverimpulse, receiverimpulselength,			/* filter */
								pSimulation->shapednoise, length,				/* signal */
								pSimulation->directionalshapednoise + length,	/* output */
								NULL											/* state */
							);
						}

						/* double length to account for second channel */
						length *= 2;
					}
				}

#ifdef LOGTAIL
				fwrite(&length,sizeof(length),1,fidtail);
				fwrite(pSimulation->directionalshapednoise,sizeof(pSimulation->directionalshapednoise[0]),length,fidtail);
#endif /* LOGTAIL */

				/* add directional shaped noise signal to (B)RIR */
				for (i=0; i<length; i++)
					pSimulation->brir[SRidx].sample[i] += pSimulation->directionalshapednoise[i];

			} /* next direction */

#ifdef LOGTAIL
			fwrite(pSimulation->brir[SRidx].sample,sizeof(pSimulation->brir[SRidx].sample[0]),length,fidtail);
#endif /* LOGTAIL */

		} /* next receiver */

#ifdef LOGTAIL
	fclose(fidtail);
	}
#endif
	} /* next source */

#ifdef LOGRAYS
	fclose(fid);
	fclose(fidrecv);
#endif

	MemFree(ray);
}


BRIR *Roomsim(const CRoomSetup *pSetup)
{
	/* prepare internal room simulation data structure */
	CRoomsimInternal *pSimulation = RoomsimInit(pSetup);
    
	if (pSetup->options.simulatespecular)
	{
        if (pSetup->options.verbose)
        {
            MsgPrintf("Simulating specular reflections (xyz-order %d,%d,%d)...\n", 
                pSetup->options.reflectionorder[0],
                pSetup->options.reflectionorder[1],
                pSetup->options.reflectionorder[2]);
            MsgRelax; /* let MATLAB process events */
        }
        
		/* generate specular reflections */
		EnumerateVirtualRooms(pSetup, pSimulation,
				pSetup->options.reflectionorder[0],
				pSetup->options.reflectionorder[1],
				pSetup->options.reflectionorder[2],
				roomcallback);
	}

	if (pSetup->options.simulatediffuse)
	{
        if (pSetup->options.verbose)
        {
    		MsgPrintf("Simulating diffuse reflections (%d rays)...\n", pSetup->options.numberofrays);
            MsgRelax; /* let MATLAB process events */
        }
		RoomsimDiffuse(pSetup, pSimulation);
	}

	/* release internal data structure, return BRIR */
	return RoomsimRelease(pSimulation);
}

#define VALIDATE(a,s) if (!(a)) { MsgErrorExit("invalid setup: " s); }
 
void ValidateSetup(CRoomSetup *pSetup)
{
	VALIDATE(pSetup->room.surface.nRowsAbsorption == 6, "surface absorption not defined for 6 surfaces");
	VALIDATE(pSetup->room.surface.nRowsDiffusion == 6, "surface diffusion not defined for 6 surfaces");
	VALIDATE(pSetup->room.surface.nColsAbsorption == pSetup->room.surface.nBands,
		"surface absorption not defined for all surface frequency bands");
	VALIDATE(pSetup->room.surface.nColsDiffusion == pSetup->room.surface.nBands,
		"surface diffusion not defined for all surface frequency bands");
}
