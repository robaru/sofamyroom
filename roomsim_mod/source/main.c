/*********************************************************************//**
 * @file main.c
 * @brief Basic room simulator in C
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

#include <stdio.h>
#include <stdlib.h>

#include "libroomsim.h"
#include "interface.h"
#include "setup.h"
#include "msg.h"
#include "build.h"

/* disable warnings about unsafe CRT functions */
#ifdef _MSC_VER
#  pragma warning( disable : 4996)
#endif

#define PI 3.14159265358979323846

void Roomsetup(CRoomSetup *par)
{
	static const double surfacefrequency[]  = { 125, 250, 500, 1000, 2000, 4000 };
	static double surfaceabsorption[] = 
			{ 0.4, 0.4, 0.4, 0.4, 0.4, 0.4, /*  125 Hz */
			  0.4, 0.4, 0.4, 0.4, 0.4, 0.4, /*  250 Hz */
			  0.4, 0.4, 0.4, 0.4, 0.4, 0.4, /*  500 Hz */
			  0.4, 0.4, 0.4, 0.4, 0.4, 0.4, /* 1000 Hz */
			  0.4, 0.4, 0.4, 0.4, 0.4, 0.4, /* 2000 Hz */
			  0.4, 0.4, 0.4, 0.4, 0.4, 0.4, /* 4000 Hz */
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
		{{3,2,1}, {PI,0,0}, "cardioid"}
	};
	static const CSensor receiver[] = {
		/*{{1,2,1}, {0,PI/2,0}, "cardioid"} */
		//{{5.0,7.0,1.0}, {0,0,0}, "MIT ../../../../data/MIT/KEMARsmall.hrtf"}
		//{{5.0,7.0,1.0}, {0,0,0}, "SOFA ../../../../data/SOFA/MIT_KEMAR_normal_pinna.sofa", TRUE}
		{{1.0,2.0,1.0}, {0,0,0}, "SOFA ../../../../data/SOFA/MIT_KEMAR_large_pinna.sofa"}
		//{{1.0,2.0,1.0}, {0,0,0}, "SOFA ../../../../data/SOFA/MIT_KEMAR_COMPACT_FULL.sofa"}

	};
	FILE *fid;
	int i;

	par->options.fs					  = 44100;
	par->options.responseduration	  = 0.5;
	par->options.bandsperoctave		  = 1;
	par->options.referencefrequency   = 125;
	par->options.airabsorption		  = TRUE;
	par->options.distanceattenuation  = TRUE;
	par->options.subsampleaccuracy    = TRUE;
	par->options.highpasscutoff		  = 0;
	par->options.verbose			  = TRUE;

	par->options.simulatespecular	  = TRUE;
	par->options.reflectionorder[0]   = 3;
	par->options.reflectionorder[1]   = 3;
	par->options.reflectionorder[2]   = 3;

#define RAYORDER 10
	par->options.simulatediffuse      = TRUE;
	par->options.numberofrays         = 20*RAYORDER*RAYORDER;
	par->options.rayenergyfloordB     = -80;
	par->options.diffusetimestep      = 0.010;

	par->room.dimension[0]			  = 10;
	par->room.dimension[1]			  = 15;
	par->room.dimension[2]			  = 3;
	par->room.humidity				  = 0.42;
	par->room.temperature			  = 20;

	par->room.surface.frequency       = surfacefrequency;
	par->room.surface.nBands		  = sizeof(surfacefrequency) / sizeof(surfacefrequency[0]);
	par->room.surface.absorption      = surfaceabsorption;
	par->room.surface.nRowsAbsorption = par->room.surface.nBands;
	par->room.surface.nColsAbsorption = sizeof(surfaceabsorption) / sizeof(surfaceabsorption[0]) / par->room.surface.nBands;
	par->room.surface.diffusion       = surfacediffusion;
	par->room.surface.nRowsDiffusion  = par->room.surface.nBands;
	par->room.surface.nColsDiffusion  = sizeof(surfacediffusion) / sizeof(surfacediffusion[0]) / par->room.surface.nBands;

	/* Source(s) */
	par->source		= source;
	par->nSources	= sizeof(source)/sizeof(source[0]);

	/* Receiver(s) */
	par->receiver	= receiver;
	par->nReceivers = sizeof(receiver)/sizeof(receiver[0]);

	/* read absorption and diffusion data if exists */
	fid = fopen("abscoeff.txt","r");
	if (fid)
	{
		printf("using absorption coefficients from 'abscoeff.txt'\n");
		for (i=0; i<par->room.surface.nBands*6; i++)
			fscanf(fid,"%lf", &surfaceabsorption[i]);
		fclose(fid);
	}
	fid = fopen("difcoeff.txt","r");
	if (fid)
	{
		printf("using diffusion coefficients from 'difcoeff.txt'\n");
		for (i=0; i<par->room.surface.nBands*6; i++)
			fscanf(fid,"%lf", &surfacediffusion[i]);
		fclose(fid);
	}
}

#if !defined(UNITTEST)

int main(int argc, char **argv)
{
	CRoomSetup setup;
    BRIR	   *response;
    FILE	   *fid;
	int		   i;
	CFileSetup filesetup;

	printf("ROOMSIM v" ROOMSIM_VERSION ", built %s %s\n", builddate, buildtime);

	/*if (argc<=1)
	{
		MsgPrintf("Usage: roomsim setup [output]\n");
		return 0;
	}

	MsgPrintf("Reading setup file '%s'...\n", argv[1]);
	if (ReadSetup(argv[1],&filesetup) < 0)
	{
		char msg[256];
		sprintf(msg,"error reading setup file '%s'\n",argv[1]);
		MsgErrorExit(msg);
	}*/

	/* PrintSetup(&filesetup.root); */
	/*LoadCRoomSetup(&filesetup.root,&setup);*/
	Roomsetup(&setup);
	ValidateSetup(&setup);

	/* run the simulator */
    response = Roomsim(&setup);

#define FWVAR(v) fwrite(&(v),sizeof(v),1,fid)
#define FWDBLARR(v,c) fwrite(v,sizeof((v)[0]),c,fid)

	fid = NULL;
	if (argc>2)
	{
		fid = fopen(argv[2],"wb");
		if (fid) MsgPrintf("Writing output file '%s'\n", argv[2]);
	}

	if (!fid)
	{
		fid = fopen("output.brir","wb");
		if (fid) MsgPrintf("Writing output file 'output.brir'\n");
	}

	if (fid)
	{
		FWVAR(setup.nSources);
		FWVAR(setup.nReceivers);

		for (i=0; i<setup.nSources*setup.nReceivers; i++)
		{
			FWVAR(response[i].fs);
			FWVAR(response[i].nChannels);
			FWVAR(response[i].nSamples);
			FWDBLARR(response[i].sample,response[i].nChannels * response[i].nSamples);
		}
		fclose(fid);
	}
    
	/* release BRIR memory */
	ReleaseBRIR(response);

	/* release sensors */
	ClearAllSensors();

#ifdef DEBUG
	printf("Press return to exit...\n");
	getchar();
#endif

    return 0;
}

#else /* defined(UNITTEST) */

/* import unit test functions */
#include "unittest.c"

/* unit test main function */
int main(void)
{
    int i;
    for (i=0; i<nUnittests; i++)
    {
        MsgPrintf("testing %s... ", unittest[i].name);
        unittest[i].run();
        MsgPrintf("passed.\n");
    }

	printf("Press return to exit...\n");
	getchar();
    return 0;
}
#endif /* defined(UNIT_TEST) */
