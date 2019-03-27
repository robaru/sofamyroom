/*********************************************************************//**
 * @file 3D.h
 * @brief Three-dimensional transformation function prototypes.
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

#ifndef _3D_H_75161827156451929812731
#define _3D_H_75161827156451929812731

#include "types.h"

void ComputeSensor2RoomYPRT(const YPR *ypr, YPRT *yprt);
void ComputeRoom2SensorYPRT(const YPR *ypr, YPRT *yprt);

void YawPitchRoll(const XYZ *xyz, const YPRT *yprt, XYZ *xyzhat);
void YawPitchRoll_InPlace(XYZ *xyz, const YPRT *yprt);

void azel_vp(const XYZ *xyz, AZEL *vp);

#endif /* #ifndef _3D_H_75161827156451929812731 */
