/*********************************************************************//**
 * @file interp.h
 * @brief Interpolation function prototypes.
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

#ifndef _INTERP_H_6482163816451836213987120382146
#define _INTERP_H_6482163816451836213987120382146

void ExecuteLinearInterpolate(const double *yi, double *yo, int olen,
                                 const unsigned int *idx0, const unsigned int *idx1, const double *weight0, const double *weight1);
void PrepareLinearInterpolate(const double *xi, int ilen, const double *xo, int olen, 
                                 unsigned int *idx0, unsigned int *idx1, double *weight0, double *weight1);

void LinearInterpolate(const double *xi, const double *yi, int ilen, const double *xo, double *yo, int olen);

#endif /* #ifndef _INTERP_H_6482163816451836213987120382146 */
