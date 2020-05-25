/*********************************************************************//**
 * @file sensor.h
 * @brief Sensor load function and interactive functions prototypes.
 **********************************************************************/

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
