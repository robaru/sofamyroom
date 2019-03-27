/*********************************************************************//**
 * @file dsp.h
 * @brief Digital Signal Processing routines.
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

#ifndef _DSP_H_123795791719246514351
#define _DSP_H_123795791719246514351

void FIRfilter(const double *h, int hlen, const double *x, int xlen, double *y, double *state);
void Conv(const double *h, int hlen, const double *x, int xlen, double *y);

void FreqzLogMagnitude(double *h, int hlen, double *w, int wlen, double *logmag);

/** Opague type for minimum-phase FIR conversion routine. */
typedef struct CMinPhaseFIRplan CMinPhaseFIRplan;

CMinPhaseFIRplan *AllocMinPhaseFIRplan(unsigned int nFFT, double *F, unsigned int nF);
void LogMagFreqResp2MinPhaseFIR(const double *logmag, double *h, CMinPhaseFIRplan *plan);
void FreeMinPhaseFIRplan(CMinPhaseFIRplan *plan);

void TimeVaryingConv(const double *hh, int hlen, 
					 const int *idx, int nidx, 
					 const unsigned int *x, int xlen,
					 int xstart, unsigned int xthreshold,
					 double *y);

#endif /* #ifndef _DSP_H_123795791719246514351 */
