/*********************************************************************//**
 * @file unittest.c
 * @brief Unit test routines.
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

#include "defs.h"
#include "dsp.h"
#include "interp.h"
#include "msg.h"
#include "sensor.h"
#include "libroomsim.h"

/* disable warnings about depricated unsafe CRT functions */
#ifdef _MSC_VER
#  pragma warning( disable : 4996)
#endif

#define EPSILON     1e-10
#define EPSEQ(x,y)  (fabs((x)-(y))<EPSILON)

#define CLEAROUTPUT memset(out,0,sizeof(out))
#define ASSERTOUTPUT(out,r) \
    for (i=0; i<LENGTH(r); i++) if (!EPSEQ(out[i],r[i])) { char msg[64]; sprintf(msg,"incorrect output (%d,%.10f,%.10f)",i,out[i],r[i]); ERROR(msg); } \
    for (i=LENGTH(r); i<LENGTH(out); i++) if (out[i]!=0.0) ERROR("output beyond array bounds");

#define ERROR(MSG) uterror(MSG,__FILE__,__LINE__)
void uterror(char *msg, char *file, unsigned int line)
{
    char errmsg[256];
    sprintf(errmsg,"\nERROR AT %s(%d):\n%s", file, line, msg);
    MsgErrorExit(errmsg);
}

void PrintConv(double *h, int hlen, double *x, int xlen, double *y)
{
    int i;
    MsgPrintf("[ ");
    for (i=0; i<hlen; i++)
        MsgPrintf("%.2f ", h[i]);
    MsgPrintf("] * [ ");
    for (i=0; i<xlen; i++)
        MsgPrintf("%.2f ", x[i]);
    MsgPrintf("] = [ ");
    for (i=0; i<hlen+xlen-1; i++)
        MsgPrintf("%.2f ", y[i]);
    MsgPrintf("]\n");
}

#define LENGTH(x) (sizeof(x)/sizeof(x[0]))
#define DBLLEN(x) (sizeof(x)/sizeof(double))
#define INTLEN(x) (sizeof(x)/sizeof(int))
#define DISPDBL(x) mexPrintf("%s:\n",#x);for (i=0; i<DBLLEN(x); i++) mexPrintf("  %d = %f\n",i,(x)[i]);
#define DISPINT(x) mexPrintf("%s:\n",#x);for (i=0; i<INTLEN(x); i++) mexPrintf("  %d = %d\n",i,(x)[i]);

/*******************************************************************************/
void testConvolution(void)
{
    double x1[] = {1,1,1};
    double h1[] = {1,1,1};
    double r1[] = {1,2,3,2,1};
    
    double x2[] = {1,-2,3,-4,5};
    double h2[] = {0,1};
    double r2[] = {0,1,-2,3,-4,5};
    
    double x3[] = {1,-2,-3,3,3,4,-5,-1,2,3,-3,-3};
    double h3[] = {0.5,-0.25,-2,1.3};
    double r3[] = {0.50,-1.25,-3.00,7.55,4.15,-8.65,-5.60,-3.35,16.45,-3.50,-7.55,-4.15,10.65,2.10,-3.90};

    double y[20];
    int i;

    memset(y,0,sizeof(y));
    Conv(h1,DBLLEN(h1),x1,DBLLEN(x1),y);
    /* PrintConv(h1,DBLLEN(h1),x1,DBLLEN(x1),y); */
    for (i=0; i<DBLLEN(h1)+DBLLEN(x1)-1; i++) if (!EPSEQ(y[i],r1[i])) ERROR("incorrect output");
    for (i=DBLLEN(h1)+DBLLEN(x1)-1; i<DBLLEN(y); i++) if (y[i]!=0.0) ERROR("output out of bounds");
    
    memset(y,0,sizeof(y));
    Conv(h2,DBLLEN(h2),x2,DBLLEN(x2),y);
    /* PrintConv(h2,DBLLEN(h2),x2,DBLLEN(x2),y); */
    for (i=0; i<DBLLEN(h2)+DBLLEN(x2)-1; i++) if (!EPSEQ(y[i],r2[i])) ERROR("incorrect output");
    for (i=DBLLEN(h2)+DBLLEN(x2)-1; i<DBLLEN(y); i++) if (y[i]!=0.0) ERROR("output out of bounds");
    
    memset(y,0,sizeof(y));
    Conv(x2,DBLLEN(x2),h2,DBLLEN(h2),y);
    /* PrintConv(x2,DBLLEN(x2),h2,DBLLEN(h2),y); */
    for (i=0; i<DBLLEN(x2)+DBLLEN(h2)-1; i++) if (!EPSEQ(y[i],r2[i])) ERROR("incorrect output");
    for (i=DBLLEN(x2)+DBLLEN(h2)-1; i<DBLLEN(y); i++) if (y[i]!=0.0) ERROR("output out of bounds");
    
    memset(y,0,sizeof(y));
    Conv(x3,DBLLEN(x3),h3,DBLLEN(h3),y);
    /* PrintConv(x3,DBLLEN(x3),h3,DBLLEN(h3),y); */
    for (i=0; i<DBLLEN(x3)+DBLLEN(h3)-1; i++) if (!EPSEQ(y[i],r3[i])) ERROR("incorrect output");
    for (i=DBLLEN(x3)+DBLLEN(h3)-1; i<DBLLEN(y); i++) if (y[i]!=0.0) ERROR("output out of bounds");
}

/*******************************************************************************/
void testLinearInterpolation(void)
{
    double x0[] = {3, 4, 5, 6, 7};
    double y0[] = {3, 4, 5, 6, 7};
    
    double x1[] = {0.4, 1.4, 2.3, 3.2, 3.8, 4.1, 6.7, 7, 8.4, 9.5};
    double r1[] = {3  , 3  , 3  , 3.2, 3.8, 4.1, 6.7, 7, 7  , 7  };
    
    double x2[] = {3, 4, 5, 6, 7};
    double r2[] = {3, 4, 5, 6, 7};

    double y3[] = {6, 8, 10, 12, 14};
    double x3[] = {3.5, 4.5, 5.5, 6.5, 6.9};
    double r3[] = {7, 9, 11, 13, 13.8};
    
    double out[32];
    unsigned idx0[32], idx1[32];
    double weight0[32], weight1[32];
    int    i;

	CLEAROUTPUT;
    LinearInterpolate(x0,y0,DBLLEN(x0),x1,out,DBLLEN(x1));
	ASSERTOUTPUT(out,r1);

	CLEAROUTPUT;
    LinearInterpolate(x0,y0,DBLLEN(x0),x2,out,DBLLEN(x2));
	ASSERTOUTPUT(out,r2);

    CLEAROUTPUT;
    LinearInterpolate(x0, y3, DBLLEN(x0), x3, out, DBLLEN(x3));
    ASSERTOUTPUT(out, r3);
    
    /* repeat test using prepare/execute linear interpolation */
    memset(idx0,0,sizeof(idx0));        memset(idx1,0,sizeof(idx1));
    memset(weight0,0,sizeof(weight0));  memset(weight1,0,sizeof(weight1));
    PrepareLinearInterpolate(x0,DBLLEN(x0),x1,DBLLEN(x1),idx0,idx1,weight0,weight1);
    for (i=DBLLEN(x1); i<INTLEN(idx0); i++)     if (idx0[i]!=0.0) ERROR("output out of bounds");
    for (i=DBLLEN(x1); i<INTLEN(idx1); i++)     if (idx1[i]!=0.0) ERROR("output out of bounds");
    for (i=DBLLEN(x1); i<DBLLEN(weight0); i++)  if (weight0[i]!=0.0) ERROR("output out of bounds");
    for (i=DBLLEN(x1); i<DBLLEN(weight1); i++)  if (weight1[i]!=0.0) ERROR("output out of bounds");
	CLEAROUTPUT;
    ExecuteLinearInterpolate(y0,out,DBLLEN(x1),idx0,idx1,weight0,weight1);
	ASSERTOUTPUT(out,r1);

    memset(idx0,0,sizeof(idx0));        memset(idx1,0,sizeof(idx1));
    memset(weight0,0,sizeof(weight0));  memset(weight1,0,sizeof(weight1));
    PrepareLinearInterpolate(x0,DBLLEN(x0),x2,DBLLEN(x2),idx0,idx1,weight0,weight1);
    for (i=DBLLEN(x1); i<INTLEN(idx0); i++)     if (idx0[i]!=0.0) ERROR("output out of bounds");
    for (i=DBLLEN(x1); i<INTLEN(idx1); i++)     if (idx1[i]!=0.0) ERROR("output out of bounds");
    for (i=DBLLEN(x1); i<DBLLEN(weight0); i++)  if (weight0[i]!=0.0) ERROR("output out of bounds");
    for (i=DBLLEN(x1); i<DBLLEN(weight1); i++)  if (weight1[i]!=0.0) ERROR("output out of bounds");
	CLEAROUTPUT;
    ExecuteLinearInterpolate(y0,out,DBLLEN(x2),idx0,idx1,weight0,weight1);
	ASSERTOUTPUT(out,r2);
}

/*******************************************************************************/
void testMinPhaseFIR(void)
{
    CMinPhaseFIRplan *plan;
    /*double F[]      = {0,63,125,250,500,1000,2000,4000,8000,16000,22050}; */
    /*double logmag[] = {0, 1,  2,  3,  4,   5,   6,   7,   8,    9,   10}; */
    double logmag[] = {5.2149, 3.7137, 3.0400, 2.6672, 2.4260, 2.2640, 2.1586, 2.0988, 2.0794};
    double F[]      = {0,1,2,3,4,5,6,7,8};
    double h[16];
/*    int i; */
        
    plan = AllocMinPhaseFIRplan(DBLLEN(h),F,DBLLEN(F));
    
	LogMagFreqResp2MinPhaseFIR(logmag, h, plan);
    
/*     for (i=0; i<DBLLEN(h); i++) mexPrintf("h[%2d] = %9.4f\n", i, h[i]); */
    
    FreeMinPhaseFIRplan(plan);
}

#define NORMFREQ (2*PI/44100.0)


/*******************************************************************************/
void testFreqzLogMagnitude(void)
{
	double h1[] = { 1, 2, 3, 4 };
	double w1[] = { 125, 250, 500, 1000 };
	double r1[] = { 2.30242650081318, 2.30195068402885, 2.30004681321544, 2.29242166814533 };
	double out[256];
 	int i;

	/* normalize frequencies */
	for (i=0; i<LENGTH(w1); i++) w1[i] *= NORMFREQ;

	CLEAROUTPUT;
	FreqzLogMagnitude(h1,LENGTH(h1),w1,LENGTH(w1),out);
	ASSERTOUTPUT(out,r1);
}

/*******************************************************************************/
#define PI 3.14159265358979323846

void Roomsetup(CRoomSetup* par)
{
    static double surfacefrequency[] = { 125, 250, 500, 1000, 2000, 4000 };
    static double surfaceabsorption[] =
    { 1.0, 1.0, 1.0, 1.0, 1.0, 1.0,/*  125 Hz */
      1.0, 1.0, 1.0, 1.0, 1.0, 1.0, /*  250 Hz */
      1.0, 1.0, 1.0, 1.0, 1.0, 1.0, /*  500 Hz */
      1.0, 1.0, 1.0, 1.0, 1.0, 1.0, /* 1000 Hz */
      1.0, 1.0, 1.0, 1.0, 1.0, 1.0, /* 2000 Hz */
      1.0, 1.0, 1.0, 1.0, 1.0, 1.0, /* 4000 Hz */
    };
    static double surfacediffusion[] =
    { 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, /*  125 Hz */
      0.5, 0.5, 0.5, 0.5, 0.5, 0.5, /*  250 Hz */
      0.5, 0.5, 0.5, 0.5, 0.5, 0.5, /*  500 Hz */
      0.5, 0.5, 0.5, 0.5, 0.5, 0.5, /* 1000 Hz */
      0.5, 0.5, 0.5, 0.5, 0.5, 0.5, /* 2000 Hz */
      0.5, 0.5, 0.5, 0.5, 0.5, 0.5, /* 4000 Hz */
    };
    static const CSensor source[] = {
        {{500,500,500}, {180,0,0}, "unidirectional"}
    };
    static const CSensor receiver[] = {
        {{500,500,500}, {0,0,0}, 
#	ifndef MEX
		"SOFA ../../data/SOFA/MIT_KEMAR_normal_pinna.sofa"
#	else
		"SOFA data/SOFA/MIT_KEMAR_normal_pinna.sofa"
#	endif
		}
    };
    FILE* fid;
    int i;

    par->options.fs = 44100;
    par->options.responseduration = 0.5;
    par->options.bandsperoctave = 1;
    par->options.referencefrequency = 125;
    par->options.airabsorption = false;
    par->options.distanceattenuation = false;
    par->options.subsampleaccuracy = true;
    par->options.highpasscutoff = 0;
    par->options.verbose = true;

    par->options.simulatespecular = true;
    par->options.reflectionorder[0] = 10;
    par->options.reflectionorder[1] = 10;
    par->options.reflectionorder[2] = 10;

#define RAYORDER 10
    par->options.simulatediffuse = false;
    par->options.numberofrays = 20 * RAYORDER * RAYORDER;
    par->options.rayenergyfloordB = -80;
    par->options.diffusetimestep = 0.010;

    par->room.dimension[0] = 1000;
    par->room.dimension[1] = 1000;
    par->room.dimension[2] = 1000;
    par->room.humidity = 0.42;
    par->room.temperature = 20;

    par->room.surface.frequency = surfacefrequency;
    par->room.surface.nBands = sizeof(surfacefrequency) / sizeof(surfacefrequency[0]);
    par->room.surface.absorption = surfaceabsorption;
    par->room.surface.nRowsAbsorption = par->room.surface.nBands;
    par->room.surface.nColsAbsorption = sizeof(surfaceabsorption) / sizeof(surfaceabsorption[0]) / par->room.surface.nBands;
    par->room.surface.diffusion = surfacediffusion;
    par->room.surface.nRowsDiffusion = par->room.surface.nBands;
    par->room.surface.nColsDiffusion = sizeof(surfacediffusion) / sizeof(surfacediffusion[0]) / par->room.surface.nBands;

    /* Source(s) */
    par->source = source;
    par->nSources = sizeof(source) / sizeof(source[0]);

    /* Receiver(s) */
    par->receiver = receiver;
    par->nReceivers = sizeof(receiver) / sizeof(receiver[0]);

    /* Output */
    par->options.outputname = "brir";
    par->options.saveaswav = true;

    /* read absorption and diffusion data if exists */
    fid = fopen("abscoeff.txt", "r");
    if (fid)
    {
        printf("using absorption coefficients from 'abscoeff.txt'\n");
        for (i = 0; i < par->room.surface.nBands * 6; i++)
            fscanf(fid, "%lf", &surfaceabsorption[i]);
        fclose(fid);
    }
    fid = fopen("difcoeff.txt", "r");
    if (fid)
    {
        printf("using diffusion coefficients from 'difcoeff.txt'\n");
        for (i = 0; i < par->room.surface.nBands * 6; i++)
            fscanf(fid, "%lf", &surfacediffusion[i]);
        fclose(fid);
    }
}

void testEmptyRoom(void)
{
    CRoomSetup setup;
    BRIR* brir;
    CSensorDefinition *definition;
    CSensorResponse response;
    int error;
    XYZ xyz = { 0 };
    int i;

    double brirEnergy = 0;
    double hrtfEnergy = 0;

    double  brirLogmag[] = { 0, 0, 0, 0, 0, 0 };
    double  hrtfLogmag[] = { 0, 0, 0, 0, 0, 0 };

    double frequencies[] = { 125, 250, 500, 1000, 2000, 4000 };

    MsgPrintf("Running simulator...\n");
    Roomsetup(&setup);
    ValidateSetup(&setup);
    brir = Roomsim(&setup);

    for (i = 0; i < setup.room.surface.nBands; i++)
        frequencies[i] *= 2 * PI / setup.options.fs;

    FreqzLogMagnitude (brir->sample, brir->nSamples, frequencies, setup.room.surface.nBands, &brirLogmag[0]);

    MsgPrintf("Extracting HRTF from SOFA file...");
    definition = LoadSensor(
#	ifndef MEX
		"SOFA ../../data/SOFA/MIT_KEMAR_normal_pinna.sofa"
#	else
		"SOFA data/SOFA/MIT_KEMAR_normal_pinna.sofa"
#	endif
    );
    SensorGetResponse(definition, &xyz, &response);
    FreqzLogMagnitude (response.data.impulseresponse, definition->nSamples, frequencies, setup.room.surface.nBands, &hrtfLogmag[0]);

    for (int i = 0; i < setup.room.surface.nBands; ++i)
    {
        if (!EPSEQ(brirLogmag[i], hrtfLogmag[i]))
         {
             char msg[64];
             sprintf(msg, "incorrect logmag output (%d,%.10f,%.10f)", i, brirLogmag[i], hrtfLogmag[i]);
             ERROR(msg);
         }
    }

    for (int i = 0; i < brir->nChannels * brir->nSamples; ++i)
    {
        brirEnergy += (brir->sample[i] * brir->sample[i]);
        hrtfEnergy += (response.data.impulseresponse[i] * response.data.impulseresponse[i]);
    }

    if (!EPSEQ(brirEnergy, hrtfEnergy))
    {
        char msg[64];
        sprintf(msg, "incorrect energy output (%.10f,%.10f)", brirEnergy, hrtfEnergy);
        ERROR(msg);
    }

    //Release Memory
}

typedef struct {
    char *name;
    void (*run)(void);
} CUnittest;

CUnittest unittest[] = {
    { "convolution",	                        testConvolution         },
    { "linear interpolation",                   testLinearInterpolation },
    { "minimum phase FIR filter design",        testMinPhaseFIR         },
	{ "freqz log magnitude frequency response", testFreqzLogMagnitude   },
    { "empty room",                             testEmptyRoom   },
};
int nUnittests = sizeof(unittest) / sizeof(CUnittest);
