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
 *************************************************************************
 * Modified for SofaMyRoom (https://github.com/robaru/sofamyroom).
 * Copyright 2019-2026, Roberto Barumerli and Daniele Bianchi
 *
 * Changes with respect to the original ROOMSIM file:
 *   - Turned into the SofaMyRoom command line front end: the hard-coded
 *     Roomsetup() example was removed and the simulated responses are
 *     written to WAVE files through wavwriter.
 *
 * The modifications are released under the same GNU General Public
 * License, version 3 or later.
 *************************************************************************/

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
		snprintf(msg, sizeof(msg),"error reading setup file '%s'\n",argv[1]);
		MsgErrorExit(msg);
	}

	//PrintSetup(&filesetup.root); 
	LoadCRoomSetup(&filesetup.root,&setup);
	//Roomsetup(&setup);
	ValidateSetup(&setup);

	/* run the simulator */
    response = Roomsim(&setup);

	/* scratch buffer sized for the receiver with the most channels */
	k = 0;
	for (i = 0; i < setup.nSources*setup.nReceivers; i++)
		if (response[i].nChannels > k) k = response[i].nChannels;
	sample = (float*)malloc(k * sizeof(float));

	if (!sample)
	{
		MsgPrintf("Unable to allocate memory for writing the WAVE file\n");
		return 1;
	}

	for (i = 0; i < setup.nSources*setup.nReceivers; i++)
	{
		snprintf(filename, sizeof(filename), "%s_receiver_%d.wav", setup.options.outputname, i);

		MsgPrintf("Writing output file '%s'\n", filename);

		w = makeWave(3, (int)response[i].fs, (short int)response[i].nChannels, (short int)32);
		waveSetDuration(&w, (float)(response[i].nSamples / response[i].fs));
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

    MsgPrintf("all %d tests passed.\n", nUnittests);
    return 0;
}
#endif /* defined(UNIT_TEST) */
