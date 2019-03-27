/*********************************************************************//**
 * @file sensor.h
 * @brief Sensor load function and interactive functions prototypes.
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

#ifndef _SENSOR_H_123791268461368123821821683
#define _SENSOR_H_123791268461368123821821683

#include "types.h"

extern CSensorDefinition *LoadSensor(const char *description);
/*extern void LoadSensor(char *description, CSensorProbeFunction *probe, CSensorData **data); */

int SensorGetResponse(const CSensorDefinition *sensor, const XYZ *xyz, CSensorResponse *response);
double SensorGetLogGain(CSensorDefinition *sensor, XYZ *xyz, int band);

extern void CmdListSensors(void);
extern void CmdLoadSensor(const char *description);
extern void CmdWhosSensors(void);
extern void CmdClearSensor(const char *description);
extern void CmdClearAllSensors(void);

#endif /* #ifndef _SENSOR_H_123791268461368123821821683 */
