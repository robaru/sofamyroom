/*********************************************************************//**
 * @file main.c
 * @brief Basic room simulator in C
 **********************************************************************/

#include <stdio.h>
#include <stdlib.h>

#include "libroomsim.h"
#include "interface.h"
#include "setup.h"
#include "msg.h"
#include "build.h"
#include "wavwriter.h"

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
		{{1.0,2.0,1.0}, {0,0,0}, "SOFA ../../data/SOFA/MIT_KEMAR_normal_pinna.sofa interp=1"}
	};
	FILE *fid;
	int i;

	par->options.fs					  = 44100;
	par->options.responseduration	  = 0.5;
	par->options.bandsperoctave		  = 1;
	par->options.referencefrequency   = 125;
	par->options.airabsorption		  = true;
	par->options.distanceattenuation  = true;
	par->options.subsampleaccuracy    = true;
	par->options.highpasscutoff		  = 0;
	par->options.verbose			  = true;

	par->options.simulatespecular	  = true;
	par->options.reflectionorder[0]   = 3;
	par->options.reflectionorder[1]   = 3;
	par->options.reflectionorder[2]   = 3;

#define RAYORDER 10
	par->options.simulatediffuse      = true;
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

	/* Output */
	par->options.outputname = "brir";
	par->options.saveaswav = true;

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
	int		   i, j, k;
	CFileSetup filesetup;
	char	   filename[256];
	Wave	   w;
	float      *sample;

	printf("ROOMSIM v" ROOMSIM_VERSION ", built %s %s\n", builddate, buildtime);

	if (argc<=1)
	{
		MsgPrintf("Usage: sofamyroom setup\n");
		return 0;
	}

	MsgPrintf("Reading setup file '%s'...\n", argv[1]);
	if (ReadSetup(argv[1],&filesetup) < 0)
	{
		char msg[256];
		sprintf(msg,"error reading setup file '%s'\n",argv[1]);
		MsgErrorExit(msg);
	}

	//PrintSetup(&filesetup.root); 
	LoadCRoomSetup(&filesetup.root,&setup);
	//Roomsetup(&setup);
	ValidateSetup(&setup);

	/* run the simulator */
    response = Roomsim(&setup);

#define FWVAR(v) fwrite(&(v),sizeof(v),1,fid)
#define FWDBLARR(v,c) fwrite(v,sizeof((v)[0]),c,fid)

	fid = NULL;

	if (!setup.options.saveaswav)
	{
		sprintf(filename, "%s.brir", setup.options.outputname);

		fid = fopen(filename, "wb");

		if (fid) 
		{ 
			MsgPrintf("Writing output file '%s'\n", filename);

			FWVAR(setup.nSources);
			FWVAR(setup.nReceivers);

			for (i = 0; i < setup.nSources*setup.nReceivers; i++)
			{
				FWVAR(response[i].fs);
				FWVAR(response[i].nChannels);
				FWVAR(response[i].nSamples);
				FWDBLARR(response[i].sample, response[i].nChannels * response[i].nSamples);
			}
			fclose(fid);
		}
		else
			MsgPrintf("unable to open '%s'\n", filename);
	}
	else
	{
		sample = (float*)malloc(response[0].nChannels * sizeof(float));

		if (!sample)
		{
			MsgPrintf("Unable to allocate memory for writing the WAVE file\n");
			return 1;
		}

		for (i = 0; i < setup.nSources*setup.nReceivers; i++)
		{
			sprintf(filename, "%s - receiver_%d.wav", setup.options.outputname, i);

			MsgPrintf("Writing output file '%s'\n", filename);

			w = makeWave(3, (int)response[i].fs, (short int)response[i].nChannels, (short int)32);
			waveSetDuration(&w, (float)response[i].nSamples / response[i].fs);
			for (j = 0; j < response[i].nSamples; ++j)
			{
				for (k = 0; k < response[i].nChannels; ++k)
				{
					sample[k] = (float)response[i].sample[j + response[i].nSamples * k];
				}
				waveAddSampleFloat(&w, sample);
			}
			waveToFile(&w, filename);
			waveDestroy(&w);

		}

		free(sample);
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
