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
#include "global.h"

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
			if (sensor->probe.response(sensor, xyz) < 0)
				return 0;
			response->type = SR_IMPULSERESPONSE;
			response->data.impulseresponse = sensor->responsedata;
			return 1;
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

/***** SOFA *******************************/
int sensor_SOFA_probe_nointerp(const CSensorDefinition* sensor, const XYZ* xyz)
{
	unsigned int i;

	UNREFERENCED_PARAMETER(sensor);

	float x = (float)xyz->x;
	float y = (float)xyz->y;
	float z = (float)xyz->z;

	mysofa_getfilter_float_nointerp(sensor->sofahandle, x, y, z, sensor->responsedatafloat, sensor->responsedatafloat + sensor->sofahandle->hrtf->N,
		&sensor->delays[0], &sensor->delays[1]);

	for (i = 0; i < sensor->sofahandle->hrtf->R * sensor->sofahandle->hrtf->N; ++i)
	{
		sensor->responsedata[i] = (double)sensor->responsedatafloat[i];
	}

	return 0;
}

int sensor_SOFA_probe(const CSensorDefinition *sensor, const XYZ *xyz)
{
	unsigned int i;

	UNREFERENCED_PARAMETER(sensor);

	float x = (float)xyz->x;
	float y = (float)xyz->y;
	float z = (float)xyz->z;

	mysofa_getfilter_float(sensor->sofahandle, x, y, z, sensor->responsedatafloat, sensor->responsedatafloat + sensor->sofahandle->hrtf->N,
		&sensor->delays[0], &sensor->delays[1]);

	for (i = 0; i < sensor->sofahandle->hrtf->R * sensor->sofahandle->hrtf->N; ++i)
	{
		sensor->responsedata[i] = (double)sensor->responsedatafloat[i];
	}

	return 0;
}

void getOptions(char *options, CSensorDefinition *definition)
{
	char *option, msg[256];
	char *stringOptions[] = { "interp=", "norm=", "resampling=" };

	option = strtok(options, " ");
	
	while (option)
	{
		if (!strnicmp(option, stringOptions[0], strlen(stringOptions[0])))
		{
			switch (*(option + strlen(stringOptions[0])))
			{
			case 'f':
			case 'F':
			case '0': 
				definition->interpolation = false;
				break;
			case 't':
			case 'T':
			case '1':
				definition->interpolation = true;
				break;
			default: 
				sprintf(msg, "invalid interpolation value, setting it to false\n");
				MsgPrintf("%s", msg);
				definition->interpolation = false;
			}
		}
		else if (!strnicmp(option, stringOptions[1], strlen(stringOptions[1])))
		{
			switch (*(option + strlen(stringOptions[1])))
			{
			case 'f':
			case 'F':
			case '0':
				definition->normalization = false;
				break;
			case 't':
			case 'T':
			case '1':
				definition->normalization = true;
				break;
			default:
				sprintf(msg, "invalid normalization value, setting it to false\n");
				MsgPrintf("%s", msg);
				definition->normalization = false;
			}
		}
		else if (!strnicmp(option, stringOptions[2], strlen(stringOptions[2])))
		{
			switch (*(option + strlen(stringOptions[2])))
			{
			case 'f':
			case 'F':
			case '0':
				definition->resampling = false;
				break;
			case 't':
			case 'T':
			case '1':
				definition->resampling = true;
				break;
			default:
				sprintf(msg, "invalid resempling value, setting it to false\n");
				MsgPrintf("%s", msg);
				definition->resampling = false;
			}
		}
		else
		{
			sprintf(msg, "%s, not a valid option\n", option);
			MsgPrintf("%s", msg);
		}

		option = strtok(NULL, " ");
	}
}

void sensor_SOFA_init(const char *datafile, CSensorDefinition *definition)
{
	char msg[256], *datafilecopy, *path, *options;
	int err;
	unsigned int i;

	/* test that a datafile name is provided */
	if (!datafile)
	{
		MsgErrorExit("no SOFA datafile specified\n");
	}
    
    /* getting options */
	datafilecopy = (char *)MemMalloc((strlen(datafile) + 1) * sizeof(char));
	if (!datafilecopy)
	{
		sprintf(msg, "unable to allocate memory space for datafile string\n");
		MsgPrintf("%s", msg);
	}
    strcpy(datafilecopy, datafile);

	/* setting options to default values */
	definition->interpolation = false;
	definition->normalization = false;
	definition->resampling = false;

	path = strtok(datafilecopy, " ");
	if (strlen(path) == strlen(datafile))
	{
		sprintf(msg, "no options provided in sensor description, using default values\n");
		MsgPrintf("%s", msg);
	}
	else
	{
		options = path + strlen(path) + 1;
		getOptions(options, definition);
	}

	/* memory allocation for SOFA data */
	definition->sofahandle = MemMalloc(sizeof(struct MYSOFA_EASY));
	if (!definition->sofahandle)
	{
		err = MYSOFA_NO_MEMORY;
		sprintf(msg, "unable to allocate memory for SOFA data file '%s' (error %d)", path, err);
		MemFree(datafilecopy);
		MsgErrorExit(msg);
	}

	/* set all values of struct to their default "0" (to avoid freeing unallocated
	 values in mysofa_free)*/
	*(definition->sofahandle) = (struct MYSOFA_EASY){ 0 };

	/* SOFA structure fields initialization */
	definition->sofahandle->lookup = NULL;
	definition->sofahandle->neighborhood = NULL;
	definition->sofahandle->fir = NULL;

	/* open SOFA file */
	definition->sofahandle->hrtf = mysofa_load(path, &err);
	if (!definition->sofahandle->hrtf) 
	{
		mysofa_close(definition->sofahandle);
		sprintf(msg, "unable to load SOFA data file '%s' (error %d)", path, err);
		MemFree(datafilecopy);
		MsgErrorExit(msg);
	}

	/* check SOFA data */
	err = mysofa_check(definition->sofahandle->hrtf);
	if (err != MYSOFA_OK) 
	{
		mysofa_close(definition->sofahandle);
		sprintf(msg, "error in SOFA hrtf data '%s' (error %d)", path, err);
		MemFree(datafilecopy);
		MsgErrorExit(msg);
	}

	/* free datafilecopy, not needed anymore */
	MemFree(datafilecopy);

	/* SOFA data resampling */
	if (definition->resampling && g_fs > 0)
	{
		sprintf(msg, "resampling HRTF data... ");
		MsgPrintf("%s", msg);
		err = mysofa_resample(definition->sofahandle->hrtf, (float)g_fs);
		if (err != MYSOFA_OK) 
		{
			mysofa_close(definition->sofahandle);
			sprintf(msg, "an error occurred during the resampling of HRTF data\n (error %d)", err);
			MsgErrorExit(msg);
		}
		sprintf(msg, "complete\n");
		MsgPrintf("%s", msg);
	}

	/* SOFA data normalization */
	if (definition->normalization)
	{
		sprintf(msg, "normalizing HRTF data... ");
		MsgPrintf("%s", msg);
		mysofa_loudness(definition->sofahandle->hrtf);
		sprintf(msg, "complete\n");
		MsgPrintf("%s", msg);
	}

	/* SOFA lookup initialization */
	mysofa_tocartesian(definition->sofahandle->hrtf);

	definition->sofahandle->lookup = mysofa_lookup_init(definition->sofahandle->hrtf);
	if (!definition->sofahandle->lookup) 
	{
		err = MYSOFA_INTERNAL_ERROR;
		mysofa_close(definition->sofahandle);
		sprintf(msg, "unable to initialize lookup (error %d)", err);
		MsgErrorExit(msg);
	}

	/* SOFA neighborhood initialization */
	definition->sofahandle->neighborhood = mysofa_neighborhood_init(definition->sofahandle->hrtf, definition->sofahandle->lookup);
	if (!definition->sofahandle->neighborhood) 
	{
		err = MYSOFA_INTERNAL_ERROR;
		mysofa_close(definition->sofahandle);
		sprintf(msg, "unable to initialize neighborhood (error %d)", err);
		MsgErrorExit(msg);
	}

	/* SOFA FIR initialization */
	definition->sofahandle->fir = MemMalloc(definition->sofahandle->hrtf->N * definition->sofahandle->hrtf->R * sizeof(float));
	if (!definition->sofahandle->fir) 
	{
		err = MYSOFA_INTERNAL_ERROR;
		mysofa_close(definition->sofahandle);
		sprintf(msg, "unable to initialize fir filter memory (error %d)", err);
		MsgErrorExit(msg);
	}

	/* allocate memory for sensor temporary float response data */
	definition->responsedatafloat = MemMalloc(definition->sofahandle->hrtf->N * definition->sofahandle->hrtf->R * sizeof(float));
	if (!definition->responsedatafloat)
	{
		mysofa_close(definition->sofahandle);
		sprintf(msg, "unable to allocate memory for impulse response");
		MsgErrorExit(msg);
	}

	memset(definition->responsedatafloat, 0, definition->sofahandle->hrtf->N * definition->sofahandle->hrtf->R * sizeof(float));

	/* allocate memory for delays */
	definition->delays = MemMalloc(definition->sofahandle->hrtf->R * sizeof(float));
	if (!definition->delays)
	{
		mysofa_close(definition->sofahandle);
		sprintf(msg, "unable to allocate memory for delays");
		MsgErrorExit(msg);
	}
	
	/* allocate memory for sensor response data */
	definition->responsedata = MemMalloc(definition->sofahandle->hrtf->N * definition->sofahandle->hrtf->R * sizeof(double));
	if (!definition->responsedata)
	{
		mysofa_close(definition->sofahandle);
		sprintf(msg, "unable to allocate memory for impulse response");
		MsgErrorExit(msg);
	}

	memset(definition->responsedata, 0, definition->sofahandle->hrtf->N* definition->sofahandle->hrtf->R * sizeof(double));

	/* fill sensor definition structure */
	definition->type = ST_IMPULSERESPONSE;
	if (definition->interpolation)
	{
		definition->probe.response = sensor_SOFA_probe;
	}
	else
	{
		definition->probe.response = sensor_SOFA_probe_nointerp;
	}
	if (definition->resampling && g_fs > 0)
	{
		definition->fs = g_fs;
	}
	else
	{
		definition->fs = definition->sofahandle->hrtf->DataSamplingRate.values[0];
	}
	definition->nChannels = definition->sofahandle->hrtf->R;
	definition->nEntries = definition->sofahandle->hrtf->M;
	definition->nSamples = definition->sofahandle->hrtf->N;

	/* provide some feedback */
	MsgPrintf("Successfully loaded SOFA HRTFs (#pos=%d, #samples=%d, #ch=%d, fs=%f)\n",
		definition->nEntries, definition->nSamples, definition->nChannels, definition->fs);
	MsgRelax;
    	
#ifdef MEX
	/* make response data memory persistent */
	mexMakeMemoryPersistent(definition->sofahandle);
	mexMakeMemoryPersistent(definition->sofahandle->fir);
	mexMakeMemoryPersistent(definition->responsedatafloat);
	mexMakeMemoryPersistent(definition->responsedata);
	mexMakeMemoryPersistent(definition->delays);
#endif
}

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
    {"SOFA",            sensor_SOFA_init            }
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

	if (item->definition.sofahandle)
	{
		if (item->definition.responsedatafloat)
			MemFree(item->definition.responsedatafloat);
		if (item->definition.delays)
			MemFree(item->definition.delays);
		if (item->definition.sofahandle->fir)
			MemFree(item->definition.sofahandle->fir);
		if (item->definition.sofahandle->neighborhood)
			mysofa_neighborhood_free(item->definition.sofahandle->neighborhood);
		if (item->definition.sofahandle->lookup)
			mysofa_lookup_free(item->definition.sofahandle->lookup);
		if (item->definition.sofahandle->hrtf)
			mysofa_free(item->definition.sofahandle->hrtf);
		MemFree(item->definition.sofahandle);
	}
    
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
	g_fs = -1;
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
