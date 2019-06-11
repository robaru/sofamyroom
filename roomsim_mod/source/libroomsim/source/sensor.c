/*********************************************************************//**
 * @file sensor.c
 * @brief Sensor init, probe, and exit routines.
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
#include <stdio.h>

#include "3D.h"
#include "defs.h"
#include "mem.h"
#include "msg.h"
#include "types.h"
#include "sensor.h"
#include "mysofa.h"

/* disable warnings about unsafe CRT functions */
#ifdef _MSC_VER
#  pragma warning( disable : 4996)
#endif

/* function prototypes */
void MexAtExitCallback(void);

int SensorGetResponse(const CSensorDefinition *sensor, const XYZ *xyz, CSensorResponse *response)
{
	int idx;
	switch (sensor->type)
	{
		case ST_LOGGAIN:
		{
			response->type = SR_LOGGAIN;
			response->data.loggain = sensor->probe.loggain(sensor, xyz);
			return response->data.loggain != EMPTY_GAIN;
		}

		case ST_LOGWEIGHTS:
		{
			if ((idx = sensor->probe.xyz2idx(sensor, xyz)) < 0)
				return 0;
			response->type = SR_LOGWEIGHTS;
			response->data.logweights = &sensor->simulationlogweights[idx * sensor->nBands * sensor->nChannels];
			return 1;
		}
		
		case ST_IMPULSERESPONSE:
		{
#			ifdef MYSOFA_H_INCLUDED
			if (!sensor->interpolation)
			{
#			endif
				if ((idx = sensor->probe.xyz2idx(sensor, xyz)) < 0)
					return 0;
				response->type = SR_IMPULSERESPONSE;
				response->data.impulseresponse = &sensor->responsedata[idx * sensor->nSamples * sensor->nChannels];
				return 1;
#			ifdef MYSOFA_H_INCLUDED
			}
			else
			{
				sensor->probe.xyz2idx(sensor, xyz);
				response->type = SR_IMPULSERESPONSE;
				response->data.impulseresponse = sensor->interpResponseDataDouble;
				return 1;
			}
#			endif
		}
	}
	return 0;
}

double SensorGetLogGain(CSensorDefinition *sensor, XYZ *xyz, int band)
{
	int idx;

	if (sensor->type == ST_LOGGAIN)
		return sensor->probe.loggain(sensor, xyz);

	idx = sensor->probe.xyz2idx(sensor, xyz);
	if (idx<0)
		return LOGMINIMUM;

	return sensor->simulationlogweights[idx * sensor->nSimulationBands * sensor->nChannels + band];
}


void SensorInitDefault(CSensorDefinition *definition)
{
	memset((void *)definition, 0, sizeof(*definition));
    definition->nChannels	 = 1;
    definition->fs			 = ANY_FS;
}

/********************************************** 
 * sensor functions
 **********************************************/

/***** bidirectional **************************/
double sensor_bidirectional_probe(const CSensorDefinition *sensor, const XYZ *xyz)
{
    AZEL vp;

	UNREFERENCED_PARAMETER(sensor);

    /* convert vector to vertical-polar azimuth,elevation */
    azel_vp(xyz,&vp);

    if ((fabs(vp.el) <= PI/6) && ((fabs(vp.az)<PI/18) || (fabs(vp.az-PI) < PI/18)))
        return 0.0; /* = LOGDOMAIN(1.0) */
    
    return EMPTY_GAIN;
}

void sensor_bidirectional_init(const char *datafile, CSensorDefinition *definition)
{
	UNREFERENCED_PARAMETER(datafile);

    definition->type = ST_LOGGAIN;
    definition->probe.loggain = sensor_bidirectional_probe;
}

/***** cardioid *******************************/
double sensor_cardioid_probe(const CSensorDefinition *sensor, const XYZ *xyz)
{
    AZEL vp;
	double loggain;
    
	UNREFERENCED_PARAMETER(sensor);

    /* convert vector to vertical-polar azimuth,elevation */
    azel_vp(xyz,&vp);

    /* generate cardioid directivity */
	loggain = LOGDOMAIN(fabs(0.5 + 0.5*cos(vp.az)*cos(vp.el)));

	if (loggain < LOGMINIMUM) 
		return LOGMINIMUM;
	else
		return loggain;
}

void sensor_cardioid_init(const char *datafile, CSensorDefinition *definition)
{
	UNREFERENCED_PARAMETER(datafile);

    definition->type = ST_LOGGAIN;
    definition->probe.loggain = sensor_cardioid_probe;
}

/***** dipole *******************************/
double sensor_dipole_probe(const CSensorDefinition *sensor, const XYZ *xyz)
{
    AZEL vp;
    double loggain;

	UNREFERENCED_PARAMETER(sensor);

    /* convert vector to vertical-polar azimuth,elevation */
    azel_vp(xyz,&vp);

    /* generate dipole (figure 8) directivity */
	loggain = LOGDOMAIN(fabs(cos(vp.az)*cos(vp.el)));

	if (loggain < LOGMINIMUM) 
		return LOGMINIMUM;
	else
		return loggain;
}

void sensor_dipole_init(const char *datafile, CSensorDefinition *definition)
{
	UNREFERENCED_PARAMETER(datafile);

    definition->type = ST_LOGGAIN;
    definition->probe.loggain = sensor_dipole_probe;
}

/***** hemisphere *******************************/
double sensor_hemisphere_probe(const CSensorDefinition *sensor, const XYZ *xyz)
{
	UNREFERENCED_PARAMETER(sensor);

    if (xyz->x >= 0)
        return 0.0; /* = LOGDOMAIN(1.0) */
    
    return EMPTY_GAIN;
}

void sensor_hemisphere_init(const char *datafile, CSensorDefinition *definition)
{
	UNREFERENCED_PARAMETER(datafile);

    definition->type = ST_LOGGAIN;
    definition->probe.loggain = sensor_hemisphere_probe;
}

/***** hypercardioid *******************************/
double sensor_hypercardioid_probe(const CSensorDefinition *sensor, const XYZ *xyz)
{
    AZEL vp;
    double loggain;

	UNREFERENCED_PARAMETER(sensor);

    /* convert vector to vertical-polar azimuth,elevation */
    azel_vp(xyz,&vp);

    /* generate hypercardioid directivity */
	loggain = LOGDOMAIN(fabs(0.25 + 0.75*cos(vp.az)*cos(vp.el)));

	if (loggain < LOGMINIMUM) 
		return LOGMINIMUM;
	else
		return loggain;
}

void sensor_hypercardioid_init(const char *datafile, CSensorDefinition *definition)
{
	UNREFERENCED_PARAMETER(datafile);

    definition->type = ST_LOGGAIN;
    definition->probe.loggain = sensor_hypercardioid_probe;
}

/***** omnidirectional *******************************/
double sensor_omnidirectional_probe(const CSensorDefinition *sensor, const XYZ *xyz)
{
	UNREFERENCED_PARAMETER(sensor);
	UNREFERENCED_PARAMETER(xyz);

    return 0.0; /* = LOGDOMAIN(1.0) */
}

void sensor_omnidirectional_init(const char *datafile, CSensorDefinition *definition)
{
    UNREFERENCED_PARAMETER(datafile);

	definition->type = ST_LOGGAIN;
    definition->probe.loggain = sensor_omnidirectional_probe;
}

/***** subcardioid *******************************/
double sensor_subcardioid_probe(const CSensorDefinition *sensor, const XYZ *xyz)
{
    AZEL vp;
    double loggain;

	UNREFERENCED_PARAMETER(sensor);

    /* convert vector to vertical-polar azimuth,elevation */
    azel_vp(xyz,&vp);

    /* generate subcardioid directivity */
	loggain = LOGDOMAIN(fabs(0.7 + 0.3*cos(vp.az)*cos(vp.el)));

	if (loggain < LOGMINIMUM) 
		return LOGMINIMUM;
	else
		return loggain;

}

void sensor_subcardioid_init(const char *datafile, CSensorDefinition *definition)
{
	UNREFERENCED_PARAMETER(datafile);

    definition->type = ST_LOGGAIN;
    definition->probe.loggain = sensor_subcardioid_probe;
}

/***** supercardioid *******************************/
double sensor_supercardioid_probe(const CSensorDefinition *sensor, const XYZ *xyz)
{
    AZEL vp;
    double loggain;

	UNREFERENCED_PARAMETER(sensor);

    /* convert vector to vertical-polar azimuth,elevation */
    azel_vp(xyz,&vp);

    /* generate supercardioid directivity */
	loggain = LOGDOMAIN(fabs(0.37 + 0.63*cos(vp.az)*cos(vp.el)));

	if (loggain < LOGMINIMUM) 
		return LOGMINIMUM;
	else
		return loggain;
}

void sensor_supercardioid_init(const char *datafile, CSensorDefinition *definition)
{
	UNREFERENCED_PARAMETER(datafile);

    definition->type = ST_LOGGAIN;
    definition->probe.loggain = sensor_supercardioid_probe;
}

/***** unidirectional *******************************/
double sensor_unidirectional_probe(const CSensorDefinition *sensor, const XYZ *xyz)
{
    AZEL vp;
    
	UNREFERENCED_PARAMETER(sensor);

    /* convert vector to vertical-polar azimuth,elevation */
    azel_vp(xyz,&vp);

    /* unity gain in +/- 30 degree elevation and +/- 10 degree azimuth */
    /* (front only), otherwise zero gain */
    if ((fabs(vp.el) <= PI/6) && (fabs(vp.az) < PI/18))
        return 0.0; /* = LOGDOMAIN(1.0) */
    
    return EMPTY_GAIN;
}

void sensor_unidirectional_init(const char *datafile, CSensorDefinition *definition)
{
	UNREFERENCED_PARAMETER(datafile);

    definition->type = ST_LOGGAIN;
    definition->probe.loggain = sensor_unidirectional_probe;
}

#ifndef MYSOFA_H_INCLUDED
/***** MIT KEMAR *******************************/

#define SENSORMIT_NHRTFS  710

#if 0
typedef struct {
    const double *data;
	int hrtflen;
} CMITdata;

void sensor_MIT_exit(void *arg)
{
    /* MsgPrintf("MIT exit: array=%p\n", ((CMITdata *)arg)->data); */
    MemFree((void *)((CMITdata *)arg)->data);
    MemFree(arg);
}
#endif

int sensor_MIT_probe(const CSensorDefinition *sensor, const XYZ *xyz)
{
	static const int MITprobe_nazs[]   = {56, 60, 72, 72, 72, 72, 72, 60, 56, 45, 36, 24, 12, 1};
	static const int MITprobe_cumidx[] = {0, 56, 116, 188, 260, 332, 404, 476, 536, 592, 637, 673, 697, 709};

    int elidx, naz, azidx, idx;
    AZEL vp;

	UNREFERENCED_PARAMETER(sensor);

    /* MsgPrintf("MIT probe [%f,%f,%f],%p\n", xyz->x, xyz->y, xyz->z, arg); */
    
    /* convert vector to vertical-polar azimuth,elevation */
    azel_vp(xyz,&vp);

	/* validate elevation  */
    if (vp.el<DEG2RAD(-45) || vp.el>DEG2RAD(90))
        return -1;

	/* negate azimuth and convert to 0...2 pi */
	vp.az = -vp.az; if (vp.az<0) vp.az += TWOPI;
    /* NOTE: negating required due to difference in azimuth convention */
    /* MIT: positive azimuth = right; ROOMSIM: positive azimuth = left. */

    /* determine idx of nearest response */
    elidx = (int) floor((vp.el + DEG2RAD(45.0))/DEG2RAD(10.0));
    naz   = MITprobe_nazs[elidx];
    azidx = (int) (ROUND(naz * vp.az / TWOPI) % naz);
    idx   = MITprobe_cumidx[elidx]+azidx;

    /*MsgPrintf("MIT probe: az=%7.3f, el=%7.3f [%d,%d,%d]\n", vp.az/PI, vp.el/PI, azidx,elidx,idx); */

    /* return nearest HRTF */
	/*return &(((const double *)definition->responsedata)[idx * definition->nSamples * 2]); */
    /*return &(((CMITdata *)arg)->data[idx * (((CMITdata *)arg)->hrtflen) * 2]); */
	return idx;
}

void sensor_MIT_init(const char *datafile, CSensorDefinition *definition)
{
    char msg[256];
	unsigned int hrtflen;
	long len;
    FILE *fid;
	size_t res;

	/* test that a datafile name is provided */
	if (!datafile)
		MsgErrorExit("no MIT datafile specified\n");

    /* open HRTF file */
    fid = fopen(datafile,"rb");
    if (!fid)
    {
        sprintf(msg,"unable to open MIT KEMAR data file '%s'", datafile);
        MsgErrorExit(msg);
    }

    /* determine file length */
	if ( (fseek(fid,0,SEEK_END)!=0) || ((len=ftell(fid))==-1L) || (fseek(fid,0,SEEK_SET)!=0) )
	{
        sprintf(msg,"unable to determine length of MIT KEMAR data file '%s'", datafile);
        MsgErrorExit(msg);
	}

	/* determine length of HRTFs */
	hrtflen = (len/(SENSORMIT_NHRTFS*2*sizeof(double)));

	/* verify that data file has the proper size */
	if ( hrtflen * SENSORMIT_NHRTFS * 2 * sizeof(double) != (unsigned long) len )
	{
        sprintf(msg,"invalid size for MIT KEMAR data file '%s'", datafile);
        MsgErrorExit(msg);
	}

    /* allocate memory for sensor response data */
    definition->responsedata = MemMalloc(len);

    /* read HRTF data and close HRTF file */
	res = fread(definition->responsedata, sizeof(double), 2 * hrtflen * SENSORMIT_NHRTFS, fid);
	fclose(fid);

    /* if read unsuccessful, terminate */
	if (res != 2 * hrtflen * SENSORMIT_NHRTFS)
    {
        sprintf(msg,"unable to read data from MIT KEMAR data file '%s' (error code %d)", datafile, res);
        MsgErrorExit(msg);
    }
        
    /* fill sensor definition structure */
    definition->type          = ST_IMPULSERESPONSE;
    definition->probe.xyz2idx = sensor_MIT_probe;
    definition->fs            = 44100.0;
    definition->nChannels     = 2;
	definition->nEntries      = SENSORMIT_NHRTFS;
    definition->nSamples      = hrtflen;

	/* provide some feedback */
	MsgPrintf("Successfully loaded MIT HRTFs (#pos=%d, #samples=%d, #ch=2, fs=44100)\n", 
		SENSORMIT_NHRTFS, hrtflen);
    MsgRelax;
    
#ifdef MEX
    /* make response data memory persistent */
    mexMakeMemoryPersistent(definition->responsedata);
#endif
}
#else
/***** SOFA *******************************/
int sensor_SOFA_probe(const CSensorDefinition *sensor, const XYZ *xyz)
{
	float c[3];
	int nearest;

	UNREFERENCED_PARAMETER(sensor);

	c[0] = (float)xyz->x;
	c[1] = (float)xyz->y;
	c[2] = (float)xyz->z;

	nearest = mysofa_lookup(sensor->sofaHandle->lookup, c);

	return nearest;
}

int sensor_SOFA_probe_interp(const CSensorDefinition *sensor, const XYZ *xyz)
{
	unsigned int i;

	UNREFERENCED_PARAMETER(sensor);

	mysofa_getfilter_float(sensor->sofaHandle, (float)xyz->x, (float)xyz->y, (float)xyz->z, sensor->interpResponseDataFloat, sensor->interpResponseDataFloat + sensor->sofaHandle->hrtf->N,
		&sensor->delays[0], &sensor->delays[1]);

	for (i = 0; i < sensor->sofaHandle->hrtf->R * sensor->sofaHandle->hrtf->N; ++i)
	{
		sensor->interpResponseDataDouble[i] = (double)sensor->interpResponseDataFloat[i];
	}

	return 0;
}

bool getInterpolation(char *datafile)
{
	int length;
	bool interpolation;

	length = strlen(datafile);

	if (datafile[length - 2] == ' ')
	{
		switch (datafile[length - 1])
		{
			case '0': {
				interpolation = false;
				break;
			}
			case '1': {
				interpolation = true;
				break;
			}
			default: {
				MsgPrintf("invalid interpolation option, setting interpolation to false\n");
				interpolation = false;
			}
		}

		datafile[length - 2] = '\0';

	}
	else
	{
		MsgPrintf("no interpolation option provided, setting interpolation to false\n");
		interpolation = false;
	}

	return interpolation;
}

void sensor_SOFA_init(char *datafile, CSensorDefinition *definition)
{
	char msg[256];
	int err;
	unsigned int i, length;

	/* test that a datafile name is provided */
	if (!datafile)
		MsgErrorExit("no SOFA datafile specified\n");

	definition->interpolation = getInterpolation(datafile);

	/* memory allocation for SOFA data */
	definition->sofaHandle = MemMalloc(sizeof(struct MYSOFA_EASY));
	
	/* check memory allocation error*/
	if (!definition->sofaHandle)
	{
		sprintf(msg, "unable to allocate memory for SOFA data file '%s'", datafile);
		MsgErrorExit(msg);
	}

	/* SOFA structure fields initialization */
	definition->sofaHandle->lookup = NULL;
	definition->sofaHandle->neighborhood = NULL;
	definition->sofaHandle->fir = NULL;

	/* open SOFA file */
	definition->sofaHandle->hrtf = mysofa_load(datafile, &err);
	if (!definition->sofaHandle->hrtf) {
		mysofa_close(definition->sofaHandle);
		sprintf(msg, "unable to load SOFA data file '%s' (error %d)", datafile, err);
		MsgErrorExit(msg);
	}

	/* check SOFA data */
	err = mysofa_check(definition->sofaHandle->hrtf);
	if (err != MYSOFA_OK) {
		mysofa_close(definition->sofaHandle);
		sprintf(msg, "error in SOFA hrtf data '%s' (error %d)", datafile, err);
		MsgErrorExit(msg);
	}

	/* SOFA lookup initialization */
	mysofa_tocartesian(definition->sofaHandle->hrtf);

	definition->sofaHandle->lookup = mysofa_lookup_init(definition->sofaHandle->hrtf);
	if (definition->sofaHandle->lookup == NULL) {
		err = MYSOFA_INTERNAL_ERROR;
		mysofa_close(definition->sofaHandle);
		sprintf(msg, "unable to initialize lookup (error %d)", err);
		MsgErrorExit(msg);
	}

	/* SOFA neighborhood initialization */
	definition->sofaHandle->neighborhood = mysofa_neighborhood_init(definition->sofaHandle->hrtf, definition->sofaHandle->lookup);
	if (definition->sofaHandle->neighborhood == NULL) {
		err = MYSOFA_INTERNAL_ERROR;
		mysofa_close(definition->sofaHandle);
		sprintf(msg, "unable to initialize neighborhood (error %d)", err);
		MsgErrorExit(msg);
	}

	/* SOFA FIR initialization */
	definition->sofaHandle->fir = MemMalloc(definition->sofaHandle->hrtf->N * definition->sofaHandle->hrtf->R * sizeof(float));
	if (definition->sofaHandle->fir == NULL) {
		err = MYSOFA_INTERNAL_ERROR;
		mysofa_close(definition->sofaHandle);
		sprintf(msg, "unable to initialize fir filter memory (error %d)", err);
		MsgErrorExit(msg);
	}

	if (definition->interpolation)
	{
		/* allocate memory for sensor temporary interpolated response data */
		definition->interpResponseDataFloat = MemMalloc(definition->sofaHandle->hrtf->N * definition->sofaHandle->hrtf->R * sizeof(float));

		if (!definition->interpResponseDataFloat)
		{
			mysofa_close(definition->sofaHandle);
			sprintf(msg, "unable to allocate memory for impulse responses");
			MsgErrorExit(msg);
		}

		/* allocate memory for sensor interpolated response data */
		definition->interpResponseDataDouble = MemMalloc(definition->sofaHandle->hrtf->N * definition->sofaHandle->hrtf->R * sizeof(double));

		if (!definition->interpResponseDataDouble)
		{
			mysofa_close(definition->sofaHandle);
			sprintf(msg, "unable to allocate memory for impulse responses");
			MsgErrorExit(msg);
		}

		/* allocate memory for delays */
		definition->delays = MemMalloc(definition->sofaHandle->hrtf->R * sizeof(float));

		if (!definition->delays)
		{
			mysofa_close(definition->sofaHandle);
			sprintf(msg, "unable to allocate memory for delays");
			MsgErrorExit(msg);
		}
	}
	
	/* allocate memory for sensor response data */
	definition->responsedata = MemMalloc(definition->sofaHandle->hrtf->DataIR.elements * sizeof(double));

	/* check memory allocation error */
	if (!definition->responsedata)
	{
		mysofa_close(definition->sofaHandle);
		sprintf(msg, "unable to allocate memory for impulse response");
		MsgErrorExit(msg);
	}

	/* read HRTF data */
	for (i = 0; i < definition->sofaHandle->hrtf->DataIR.elements; ++i)
		definition->responsedata[i] = (double)definition->sofaHandle->hrtf->DataIR.values[i];

	/* fill sensor definition structure */
	definition->type = ST_IMPULSERESPONSE;
	if (!definition->interpolation)
		definition->probe.xyz2idx = sensor_SOFA_probe;
	else
		definition->probe.xyz2idx = sensor_SOFA_probe_interp;
	definition->fs = definition->sofaHandle->hrtf->DataSamplingRate.values[0];
	definition->nChannels = definition->sofaHandle->hrtf->R;
	definition->nEntries = definition->sofaHandle->hrtf->M;
	definition->nSamples = definition->sofaHandle->hrtf->N;

	/* provide some feedback */
	MsgPrintf("Successfully loaded SOFA HRTFs (#pos=%d, #samples=%d, #ch=%d, fs=%f)\n",
		definition->nEntries, definition->nSamples, definition->nChannels, definition->fs);
	MsgRelax;
	
#ifdef MEX
	/* make response data memory persistent */
	mexMakeMemoryPersistent(definition->responsedata);
	mexMakeMemoryPersistent(definition->sofaHandle);
	mexMakeMemoryPersistent(definition->sofaHandle->fir);
	mexMakeMemoryPersistent(definition->interpResponseDataFloat);
	mexMakeMemoryPersistent(definition->interpResponseDataDouble);
	mexMakeMemoryPersistent(definition->delays);
#endif
}
#endif

/********************************************** 
 * sensor list
 **********************************************/
CSensorListItem sensor[] = {
    {"bidirectional",   sensor_bidirectional_init   },
    {"cardioid",        sensor_cardioid_init        },
    {"dipole",          sensor_dipole_init          },
    {"hemisphere",      sensor_hemisphere_init      },
    {"hypercardioid",   sensor_hypercardioid_init   },
    {"omnidirectional", sensor_omnidirectional_init },
    {"subcardioid",     sensor_subcardioid_init     },
    {"supercardioid",	sensor_supercardioid_init   },
    {"unidirectional",  sensor_unidirectional_init  },
#	ifndef MYSOFA_H_INCLUDED
    {"MIT",             sensor_MIT_init             },
#	else
	{"SOFA",            sensor_SOFA_init            }
#	endif
};
int nSensors = sizeof(sensor) / sizeof(CSensorListItem);

int FindSensor(const char *description)
{
    int i, n;
    char *pch = strchr(description,' ');

    /* determine index of first space in name */
    if (pch) n = pch-description; else n = strlen(description);
    /* name should be at least 2 characters */
    if (n<2) return -1;
    
    /* scan through sensor list, pick one that matches name */
    for (i=0; i<nSensors; i++)
        if (strnicmp(description,sensor[i].name,n)==0)
            return i;
    
    /* return negative number when no match found */
    return -1;
}

char *FindSubID(const char *description)
{
    char *pch = strchr(description,' ');
    if (pch) 
        while (*pch==' ') 
            pch++;
    return pch;
}

typedef struct CSensorDefinitionListItem CSensorDefinitionListItem;
struct CSensorDefinitionListItem {
    CSensorDefinitionListItem *next, *prev;
    char                sensorid[32];
    char                subid[256];
    CSensorDefinition   definition;
};

CSensorDefinitionListItem *g_pSensordefinitionlist = NULL;

CSensorDefinitionListItem *FindSensorData(const char *name, const char *subid)
{
    CSensorDefinitionListItem *pItem = g_pSensordefinitionlist;
    
    while (pItem)
    {
        if (strnicmp(name,pItem->sensorid,strlen(pItem->sensorid))==0)
            if (!subid || strnicmp(subid,pItem->subid,strlen(pItem->subid))==0)
                break;

        pItem = pItem->next;
    }
    
    return pItem;
}

CSensorDefinition *LoadSensor(const char *description)
{
    CSensorDefinitionListItem *sensordefinitionlistitem;
    char *subid, msg[256];
    int  s;
    
    s = FindSensor(description);

    /* when no matching sensor found, terminate */
    if (s<0) 
    {
        sprintf(msg,"unknown source/receiver '%s'", description);
        MsgErrorExit(msg);
    }

    /* determine datafile for this sensor */
    subid = FindSubID(description);
    
    /* see if sensor is already loaded  */
    sensordefinitionlistitem = FindSensorData(sensor[s].name, subid);
    if (sensordefinitionlistitem)
        return &sensordefinitionlistitem->definition;
    
    /* provide feedback because this may take a while */
    /* (esp. loading HRTF data over a network) */
    if (subid)
        sprintf(msg,"Loading source/receiver %s (%s)...\n", sensor[s].name, subid);
    else
        sprintf(msg,"Loading source/receiver %s...\n", sensor[s].name);
    MsgPrintf("%s", msg);
    MsgRelax;

    /* allocate new SensorDefinitionListItem */
    sensordefinitionlistitem = (CSensorDefinitionListItem *) MemMalloc(sizeof(CSensorDefinitionListItem));
    
    /* invoke sensor initializer */
	SensorInitDefault(&sensordefinitionlistitem->definition);
    sensor[s].init(subid,&sensordefinitionlistitem->definition);

    /* populate SensorDefinitionListItem fields */
    memset(sensordefinitionlistitem->sensorid,0,sizeof(sensordefinitionlistitem->sensorid));
    strncpy(sensordefinitionlistitem->sensorid, sensor[s].name, sizeof(sensordefinitionlistitem->sensorid)-1);
    memset(sensordefinitionlistitem->subid, 0, sizeof(sensordefinitionlistitem->subid));
    if (subid)
        strncpy(sensordefinitionlistitem->subid, subid, sizeof(sensordefinitionlistitem->subid)-1);
    
    /* put sensor definition first in linked list */
    sensordefinitionlistitem->prev = NULL;
    sensordefinitionlistitem->next = g_pSensordefinitionlist;
    if (sensordefinitionlistitem->next)
        sensordefinitionlistitem->next->prev = sensordefinitionlistitem;
    g_pSensordefinitionlist = sensordefinitionlistitem;

#ifdef MEX    
    /* make memory persistent, and set mexatexit callback */
    mexMakeMemoryPersistent(sensordefinitionlistitem);
    mexAtExit(MexAtExitCallback);
#endif
    
    /* return pointer to sensordefinition field */
    return &sensordefinitionlistitem->definition;
}

void ClearSensor(CSensorDefinitionListItem *item)
{
    char msg[512];
    
    /* make sure item is not NULL */
    if (!item) return;
    
    /* isolate item from list */
    if (item->prev)
        item->prev->next = item->next;
    if (item->next)
        item->next->prev = item->prev;
    if (item == g_pSensordefinitionlist)
        g_pSensordefinitionlist = item->next;
    
    /* provide user feedback */
    if (item->subid[0])
        sprintf(msg,"Clearing %s (%s)...\n", item->sensorid, item->subid);
    else
        sprintf(msg,"Clearing %s...\n", item->sensorid);
    MsgPrintf("%s", msg);

    /* release allocated memory, if any */
	if (item->definition.frequency)
		MemFree(item->definition.frequency);
	if (item->definition.responsedata)
		MemFree(item->definition.responsedata);
	if (item->definition.sensordata)
		MemFree(item->definition.sensordata);
	if (item->definition.simulationlogweights)
		MemFree(item->definition.simulationlogweights);
#	ifdef MYSOFA_H_INCLUDED
	mysofa_close(item->definition.sofaHandle);
	if (item->definition.interpolation)
	{
		if (item->definition.interpResponseDataDouble)
			MemFree(item->definition.interpResponseDataDouble);
		if (item->definition.interpResponseDataFloat)
			MemFree(item->definition.interpResponseDataFloat);
		if (item->definition.delays)
			MemFree(item->definition.delays);
	}
#	endif
    
    /* release memory associated with item */
    MemFree(item);
}

void ClearAllSensors(void)
{
    while (g_pSensordefinitionlist)
        ClearSensor(g_pSensordefinitionlist);
}

void MexAtExitCallback(void)
{
    MsgPrintf("ROOMSIM exiting...\n");
    ClearAllSensors();
}

/* 
 * sensor commands 
 */

void CmdListSensors(void)
{
    int i;
    MsgPrintf("Recognized sources/receivers:\n\n");
    MsgPrintf("  Name\n\n");
    for (i=0; i<nSensors; i++)
        MsgPrintf("  %-30s\n", sensor[i].name);
    MsgPrintf("\n");
}

void CmdLoadSensor(const char *description)
{
    LoadSensor(description);
}

void CmdWhosSensors(void)
{
    CSensorDefinitionListItem *sensordefinitionlistitem = g_pSensordefinitionlist;
    
    if (!sensordefinitionlistitem)
    {
        MsgPrintf("No sources/receivers loaded\n");
        return;
    }
    
    MsgPrintf("Sources/receivers loaded:\n");
    while (sensordefinitionlistitem)
    {
        if (strlen(sensordefinitionlistitem->subid))
            MsgPrintf("   %s (%s)\n", sensordefinitionlistitem->sensorid,sensordefinitionlistitem->subid);
        else
            MsgPrintf("   %s\n", sensordefinitionlistitem->sensorid);
        
        sensordefinitionlistitem = sensordefinitionlistitem->next;
    }
}

void CmdClearSensor(const char *description)
{
    char *subid, msg[256];
    CSensorDefinitionListItem *sensordefinitionlistitem;
    int s;
    
    s = FindSensor(description);
    if (s<0)
    {
        sprintf(msg,"unrecognized source/receiver '%s'", description);
        MsgErrorExit(msg);
    }
    
    /* see if sensor is loaded  */
    subid = FindSubID(description);
    sensordefinitionlistitem = FindSensorData(sensor[s].name, subid);
    if (!sensordefinitionlistitem)
    {
        if (subid)
            sprintf(msg, "source/receiver %s (%s) not loaded", sensor[s].name, subid);
        else
            sprintf(msg, "source/receiver %s not loaded", sensor[s].name);
        MsgErrorExit(msg);
    }
    
    ClearSensor(sensordefinitionlistitem);
}

void CmdClearAllSensors(void)
{
    /* remove all sensors from (persistent) memory */
    ClearAllSensors();
}
