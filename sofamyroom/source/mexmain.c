/*********************************************************************//**
 * @file mexmain.c
 * @brief Main file for MEX interface with MATLAB.
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


/****** NOTES *********************************************************//**

@todo Make sure that all frequency band weighting uses the same base for log. 
  => Done. They all use the LOGDOMAIN macro now.

@todo Loading of sensors: must match Fs, and how to deal with sensor v. simulation frequency bands
  => Partly done. Fs is matched on running a simulation.

@todo Receiver ypr should be negated when receiver YPRT computed.

@todo Should let sensor init functions return void *, which is passed to sensor probe functions (should I?)
  => Done. Sensor init functions fill a sensor definition structure, which includes a void * for private use. 
     This void * is passed to sensor probe.

@todo Convert roomsim attenuation and diffusion coefficients to log domain.
      Also, determine correct interpretation of surface attenuation coefficients
      in material databases. (Is sqrt(1-a) appropriate? It is a little confusing
      when manually setting attenuation coefficients.)

************************************************ @file *******************/
 
/* C library includes */
#include <math.h>     
#include <string.h>

/* MATLAB includes */
#include <mex.h>
#include <matrix.h>

/* NOTE: I was unable to make the FFTW DLL work with LCC. Version 2.4 of LCC 
   that is shipped with MATLAB does not support __declspec(dllimport), hence 
   it can not generate calls to functions in one DLL from another DLL. Attempts 
   to upgrade to LCC version 4.2 and linking to a static FFTW library were both 
   unsuccessful. The only solution I found was to compile/link using a different
   compiler, such as GCC (from MinGW or Cygwin, see http://gnumex.sourceforge.net),
   Microsoft Visual Studio 6.0 or Visual Studio 2008 Express.
 */
#include "fftw3.h"

/* local includes */
#include "build.h"
#include "interface.h"
#include "libroomsim.h"
#include "msg.h"
#include "sensor.h"
#include "types.h"

/* disable warnings about depricated unsafe CRT functions */
#ifdef _MSC_VER
#  pragma warning( disable : 4996)
#endif

/* NOTE: Global variables are persistent across calls, but cleared when 
   mex-function are cleared. Memory allocated by mxMalloc that is pointed 
   to by a global variable is released after each call, unless
   made persistent. When made persistent, it should be freed with a call 
   to mxFree in mexAtExit.
 */

/* system-dependent functions */
#ifdef __GNUC__
#		define stricmp     strcasecmp
#endif

bool IsMexString(const mxArray *pm)
{
    const size_t *dimensions = mxGetDimensions(pm);
    
    return mxIsChar(pm) && 
           mxGetNumberOfDimensions(pm)==2 &&
           dimensions[0]==1 && 
           dimensions[1]>0;
}

#if !defined(UNIT_TEST)
/* gateway function */
void mexFunction(
        int nlhs, mxArray *plhs[], 
        int nrhs, const mxArray *prhs[])
{
    CRoomSetup roomsetup;
    BRIR       *brir;
    
    /* accept call ROOMSIM(PAR) */
    if (nrhs==1 && mxIsStruct(prhs[0]))
    {
        if (mxGetNumberOfElements(prhs[0])!=1) mexErrMsgTxt("structure array unexpected as first input argument");

        /* load parameters from first argument into roomsim structure */
        LoadCRoomSetup(prhs[0],0,&roomsetup);
        
        /** @todo Implement VerifyRoomsim(), verifying parameter validity. */

        brir = Roomsim(&roomsetup);
        
        /** @todo Create proper output (cell) array */
        if (nlhs>0)
        {
            if ((roomsetup.nSources == 1) && (roomsetup.nReceivers == 1))
            {
                if (roomsetup.options.verbose)
                {
                    MsgPrintf("Creating output: %dx%d array\n", brir[0].nSamples, brir[0].nChannels);
                    MsgRelax;
                }
                
                plhs[0] = mxCreateDoubleMatrix(brir[0].nSamples, brir[0].nChannels, mxREAL);
                mxFree(mxGetPr(plhs[0]));
                mxSetPr(plhs[0],brir[0].sample);
            }
            else
            {
                int i;
                mxArray *h;
                
                mexPrintf("Creating output: %dx%d cell matrix\n", roomsetup.nSources, roomsetup.nReceivers);
                plhs[0] = mxCreateCellMatrix(roomsetup.nSources, roomsetup.nReceivers);
                for (i=0; i<roomsetup.nSources*roomsetup.nReceivers; i++)
                {
                    h = mxCreateDoubleMatrix(brir[i].nSamples, brir[i].nChannels, mxREAL);
                    mxFree(mxGetPr(h)); 
                    mxSetPr(h,brir[i].sample);
                    mxSetCell(plhs[0],i,h);
                }
            }
        }
    }
    else if (nrhs>0 && IsMexString(prhs[0]))
    {
        char cmd[256];
        int  i, n;
        
        /* retrieve command string from input argument */
        mxGetString(prhs[0],cmd,sizeof(cmd)-1);
        
        /* parse command string */
        if (stricmp(cmd,"version")==0)
        {
            mexPrintf("ROOMSIM v" ROOMSIM_VERSION ", built %s %s\n", builddate, buildtime);
        }
        else if (stricmp(cmd,"list")==0)
        {
            if (nrhs>1) 
                mexErrMsgTxt("extra characters after LIST command");
            
            CmdListSensors();
        }
        else if (stricmp(cmd,"load")==0)
        {
            if (nrhs<2)
                mexErrMsgTxt("LOAD syntax error");
            if (!IsMexString(prhs[1]))
                mexErrMsgTxt("argument to LOAD must be a string");
            
            /* get sensor type from input */
            mxGetString(prhs[1],cmd,sizeof(cmd)-1);
            
            /* append all remaining strings from the input */
            i = 2; n = strlen(cmd);
            while ((i<nrhs) && (n<sizeof(cmd)-2) && IsMexString(prhs[i]))
            {
                cmd[n++] = ' ';
                mxGetString(prhs[i],cmd+n,sizeof(cmd)-1-n);
                i++;
                n += strlen(cmd+n);
            }
            CmdLoadSensor(cmd);
        }
        else if (stricmp(cmd,"whos")==0)
        {
            if (nrhs>1) 
                mexErrMsgTxt("extra characters after WHOS command");
            
            CmdWhosSensors();
        }
        else if (stricmp(cmd,"clear")==0)
        {
            if (nrhs==1)
                CmdClearAllSensors();
            else if (nrhs>2)
                mexErrMsgTxt("too many arguments for CLEAR command");
            else if (!IsMexString(prhs[1]))
                mexErrMsgTxt("argument to CLEAR must be a string");
            else
            {
                mxGetString(prhs[1],cmd,sizeof(cmd)-1);
                CmdClearSensor(cmd);
            }
        }
        else
        {
            mexErrMsgTxt("unrecognized command");
        }
    }
    else
    {
        mexErrMsgTxt("syntax error");
    }
}

#else /* defined(UNITTEST) */

/* import unit test functions */
#include "unittest.c"

/* unit test gateway function */
void mexFunction(
        int nlhs, mxArray *plhs[], 
        int nrhs, const mxArray *prhs[])
{
    int i;
    for (i=0; i<nUnittests; i++)
    {
        mexPrintf("testing %s... ", unittest[i].name);
        unittest[i].run();
        mexPrintf("passed.\n");
    }
}
#endif /* defined(UNIT_TEST) */
