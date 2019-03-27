/*********************************************************************//**
 * @file rng.c
 * @brief Random number generation routines.
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

#include "types.h"
#include "SFMT.h"

/* disable warning about unreferenced inline functions */
#ifdef _MSC_VER
#  pragma warning( disable : 4514 )
#endif
/** @todo This probably needs to become thread-safe at some point */

/*
typedef struct 
{
	uint32_t *sea;
	int       seaidx;
	int       seasize;
} CRNG;

uint32_t RandomGet(CRNG *rng)
{
	if (rng->seaidx >= rng->seasize)
	{
	}
	return rng->sea[rng->seaidx++];
}

*/

void RngInit(void)
{
	init_gen_rand(0xdeaf0bad);
}

void RngSeed(void)
{
}

/*#define UNIFORM(a,b) (RandomUniform() * ((b)-(a)) + (a)) */
#define UNIFORM(a,b) ((double)(gen_rand32() * ( ((b)-(a)) / 4294967295.0 ) + (a)))
#define UNIFORM01	 ((double)(gen_rand32() * ( 1.0 / 4294967295.0 )))

void RngLambert(XYZ *xyz)
{
	double s;
	do
	{
		xyz->x = UNIFORM(-0.7,0.7);
		xyz->y = UNIFORM(-0.7,0.7);
		xyz->z = UNIFORM01;
		s = xyz->x*xyz->x + xyz->y*xyz->y + xyz->z*xyz->z;
	} while (s*s > xyz->z);
}
