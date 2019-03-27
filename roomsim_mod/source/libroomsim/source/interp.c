/*********************************************************************//**
 * @file interp.c
 * @brief Interpolation routines.
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

#include "interp.h"

void ExecuteLinearInterpolate(const double *yi, double *yo, int olen,
                                 const unsigned int *idx0, const unsigned int *idx1, const double *weight0, const double *weight1)
{
    int i;
    for (i=0; i<olen; i++)
        yo[i] = yi[idx0[i]] * weight0[i] + yi[idx1[i]] * weight1[i];
}

void PrepareLinearInterpolate(const double *xi, int ilen, const double *xo, int olen, 
                                 unsigned int *idx0, unsigned int *idx1, double *weight0, double *weight1)
{
    double xb = xi[0], xe = xi[ilen-1];
    int i = 0, o = 0;
        
    /* left extrapolation */
    while (xo[o] <= xb)
    {
        idx0[o] = 0;
        idx1[o] = 0;
        weight0[o] = 1;
        weight1[o] = 0;
        o++;
    }
    
    /* interpolation */
    while (xo[o] < xe)
    {
        while (xo[o] > xi[i])
            i++;
    
        idx1[o] = i;
        idx0[o] = i-1;
        weight1[o] = ((xo[o] - xi[i-1]) / (xi[i] - xi[i-1]));
        weight0[o] = 1 - weight1[o];
        o++;
    }
    
    /* right extrapolation */
    while (o<olen)
    {
        idx0[o] = ilen-1;
        idx1[o] = ilen-1;
        weight0[o] = 0;
        weight1[o] = 1;
        o++;
    }
}

/** Resample a piecewise-linear function. The piecewise linear function 
 *  (xi[0...ilen-1], yi[0...ilen-1]) is sampled at the points 
 *  xo[0...olen-1]. If xo[i]<xi[0] then yo[i]=yi[0]. If xo[i]>xi[ilen-1],
 *  then yo[i]=yi[ilen-1]. The result is returned in yo[0...olen-1].
 *
 *  @param[in]  xi    x-coordinates of the piecewise linear function.
 *  @param[in]  yi    y-coordinates of the piecewise linear function.
 *  @param[in]  ilen  number of elements in \a xi and \a yi.
 *  @param[in]  xo    x-coordinates of the new sample points.
 *  @param[out] yo    y-coordinates of the resampled function.
 *  @param[in]  olen  number of elements in \a xo and \a yo.
 *
 *  @warning The values of \a xi and \a xo are assumed to be strictly increasing. 
 *  @warning The output array yo is assumed to be able to hold \a olen elements.
 */
void LinearInterpolate(const double *xi, const double *yi, int ilen, const double *xo, double *yo, int olen)
{
    double xb = xi[0], xe = xi[ilen-1];
    double yb = yi[0], ye = yi[ilen-1];
    int ii = 0, oi = 0;
    
    /* left extrapolation */
    while (xo[oi] < xb)
        yo[oi++] = yb;
    
    /* interpolation */
    while (xo[oi] < xe)
    {
        while (xo[oi] > xi[ii])
            ii++;
        
        yo[oi] = ((xo[oi] - xi[ii-1]) / (xi[ii] - xi[ii-1])) * (yi[ii] - yi[ii-1]) + yi[ii-1];
        oi++;
    }
    
    /* right extrapolation */
    while (oi<olen)
        yo[oi++] = ye;
}

