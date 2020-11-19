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
#include "mem.h"

/* disable warnings about unsafe CRT functions */
#ifdef _MSC_VER
#  pragma warning( disable : 4996)
#endif

#if !defined(UNITTEST)

int main(int argc, char **argv)
{
	CRoomSetup setup;
    BRIR	   **response;
	int		   i, j, k, r;
	CFileSetup filesetup;
	char	   filename[256];
	Wave	   w;
	float      **samples;

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
	samples = (float**)MemMalloc(setup.nRooms * sizeof(float*));

	if (!samples)
	{
		MsgErrorExit("Unable to allocate memory for writing the WAVE file\n");
	}

	for (r = 0; r < setup.nRooms; r++)
	{
		BRIR* currentBRIR = response[r];
		samples[r] = (float*)MemMalloc(currentBRIR[0].nChannels * sizeof(float));

		if (!samples[r])
		{
			MsgPrintf("Unable to allocate memory for writing the WAVE file for Room %d\n", r);
			continue;
		}

		for (i = 0; i < setup.nSources * setup.nReceivers; i++)
		{
			sprintf(filename, "%s_room_%d_receiver_%d.wav", setup.options.outputname, r, i);

			MsgPrintf("Writing output file '%s'\n", filename);

			w = makeWave(3, (int)currentBRIR[i].fs, (short int)currentBRIR[i].nChannels, (short int)32);
			waveSetDuration(&w, (float)(currentBRIR[i].nSamples / currentBRIR[i].fs));

			for (j = 0; j < currentBRIR[i].nSamples; ++j)
			{
				for (k = 0; k < currentBRIR[i].nChannels; ++k)
				{
					samples[r][k] = (float)currentBRIR[i].sample[j + currentBRIR[i].nSamples * k];
				}
				waveAddSampleFloat(&w, samples[r]);
			}
			waveToFile(&w, filename);
			waveDestroy(&w);
		}

		/* release BRIR memory */
		ReleaseBRIR(currentBRIR);

		/* release WAVE memory */
		MemFree(samples[i]);
	}
	
	/* release Wave pointers array */
	MemFree(samples);

	/* release BRIR pointers array */
	MemFree(response);

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
