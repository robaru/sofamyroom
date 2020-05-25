/*********************************************************************//**
 * @file 3D.h
 * @brief Three-dimensional transformation function prototypes.
 **********************************************************************/

#ifndef _3D_H_75161827156451929812731
#define _3D_H_75161827156451929812731

#include "types.h"

void ComputeSensor2RoomYPRT(const YPR *ypr, YPRT *yprt);
void ComputeRoom2SensorYPRT(const YPR *ypr, YPRT *yprt);

void YawPitchRoll(const XYZ *xyz, const YPRT *yprt, XYZ *xyzhat);
void YawPitchRoll_InPlace(XYZ *xyz, const YPRT *yprt);

void azel_vp(const XYZ *xyz, AZEL *vp);

#endif /* #ifndef _3D_H_75161827156451929812731 */
