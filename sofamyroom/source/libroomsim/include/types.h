/*********************************************************************//**
 * @file types.h
 * @brief Project-wide typedefs.
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

#ifndef _TYPES_H_3019823578647120321803
#define _TYPES_H_3019823578647120321803

#include "mysofa.h"

/* yaw-pitch-roll data types */
/* typedef double YPR[3]; */
typedef struct {
    double yaw, pitch, roll;
} YPR;
typedef double YPRT[3][3];

/* three-dimensional coordinates type */
/* typedef double XYZ[3]; */
typedef struct {
    double x, y, z;
} XYZ;

/* azimuth-elevation type */
typedef struct {
    double az, el;
} AZEL;

typedef struct
{
    double  fs;
    int     nChannels;
    int     nSamples;
    double  *sample;
} BRIR;

/* forward declaration of CSensorDefinition */
typedef struct CSensorDefinition CSensorDefinition;

typedef void (*CSensorInitFunction)(const char *, CSensorDefinition*);
/*typedef void (*CSensorExitFunction)(void *); */

typedef double (*CSensorProbeLogGainFunction)(const CSensorDefinition*, const XYZ*);
typedef int (*CSensorProbeXyz2IdxFunction)(const CSensorDefinition*, const XYZ*);
/*typedef const double *(*CSensorProbeWeightsFunction)(const XYZ*, void *); */
typedef const int (*CSensorProbeResponseFunction)(const XYZ*, void *);

typedef union CSensorProbeFunction
{
    CSensorProbeLogGainFunction  loggain;
	CSensorProbeXyz2IdxFunction	 xyz2idx;
    /*CSensorProbeWeightsFunction  weights; */
    CSensorProbeResponseFunction response;
} CSensorProbeFunction;

typedef struct CSensorResponse
{
    enum {
		SR_LOGGAIN, 
		SR_LOGWEIGHTS, 
		SR_IMPULSERESPONSE
	} type;
	union {
		double loggain;
		double *logweights;
		double *impulseresponse;
	} data;
} CSensorResponse;

struct CSensorDefinition {
    enum {
		ST_LOGGAIN, 
		ST_LOGWEIGHTS, 
		ST_IMPULSERESPONSE
	} type;

    CSensorProbeFunction probe;
    /* CSensorExitFunction  exit; */

    double fs;
    int    nChannels;
	int	   nEntries;
    
    /* for impulse response */
    int	   nSamples;

    /* for weights */
    int    nBands;
    double *frequency;

    double *responsedata;

	/* optional opaque field for sensor's spatial sampling algorithm */
	void   *sensordata;

	/* log weights at simulation frequency bands */
	/* (interpolated or evaluated from responsedata) */
	int    nSimulationBands;
	double *simulationfrequency;
	double *simulationlogweights;

	/* for SOFA HRTFs */
	struct MYSOFA_EASY *sofahandle;
	float  *responsedatafloat;
	float  *delays; //delays[0] -> left
	bool   interpolation, normalization, resampling;
} ;

typedef struct 
{
    char                 *name;
    CSensorInitFunction  init;
} CSensorListItem;

#endif /* ifndef _TYPES_H_3019823578647120321803 */
