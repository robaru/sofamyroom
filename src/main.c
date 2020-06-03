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

	printf(SOFAMYROOM_NAME " v" SOFAMYROOM_VERSION ", built %s %s\n", builddate, buildtime);

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
