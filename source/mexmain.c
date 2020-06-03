/*********************************************************************//**
 * @file mexmain.c
 * @brief Main file for MEX interface with MATLAB.
 **********************************************************************//*


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

/* local includes */
#include "build.h"
#include "interface.h"
#include "libroomsim.h"
#include "msg.h"
#include "sensor.h"
#include "types.h"
#include "fftw3.h"
#include "wavwriter.h"


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
    
    /* accept call sofamyroom(par) */
    if (nrhs==1 && mxIsStruct(prhs[0]))
    {
        if (mxGetNumberOfElements(prhs[0])!=1) mexErrMsgTxt("structure array unexpected as first input argument");

        /* load parameters from first argument into sofamyroom structure */
        LoadCRoomSetup(prhs[0],0,&roomsetup);
        
        ValidateSetup(&roomsetup);
        brir = Roomsim(&roomsetup);

        if (roomsetup.options.saveaswav)
        {
            char	   filename[256];
            Wave	   w;
            float      *sample;
            
            sample = (float*)malloc(brir[0].nChannels * sizeof(float));

            if (!sample)
            {
                MsgPrintf("Unable to allocate memory for writing the WAVE file\n");
                return;
            }

            for (int i = 0; i < roomsetup.nSources*roomsetup.nReceivers; i++)
            {
                sprintf(filename, "%s-receiver_%d.wav", roomsetup.options.outputname, i);

                MsgPrintf("Writing output file '%s'\n", filename);

                w = makeWave(3, (int)brir[i].fs, (short int)brir[i].nChannels, (short int)32);
                waveSetDuration(&w, (float)brir[i].nSamples / brir[i].fs);
                for (int j = 0; j < brir[i].nSamples; ++j)
                {
                    for (int k = 0; k < brir[i].nChannels; ++k)
                    {
                        sample[k] = (float)brir[i].sample[j + brir[i].nSamples * k];
                    }
                    waveAddSampleFloat(&w, sample);
                }
                waveToFile(&w, filename);
                waveDestroy(&w);
            }

            free(sample);
        }
        else {
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
            mexPrintf("SofaMyRoom - built %s %s\n", builddate, buildtime);
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
        mexPrintf("SofaMyRoom - built %s %s\n", builddate, buildtime);
        mexPrintf("No argument provided!\n");
        mexPrintf("Arguments available: version, list, load, clear, whos\n");
        mexPrintf("i.e. sofamyroom list\n");
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

