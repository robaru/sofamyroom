/*********************************************************************//**
 * @file rng.h
 * @brief Random number generation function prototypes.
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

#ifndef RNG_H_19239561826387126831623
#define RNG_H_19239561826387126831623

#include "types.h"
#include "SFMT.h"

/*double RandomUniform(void); */
void RngInit(sfmt_t *sfmt);
void RngLambert(sfmt_t *sfmt, XYZ *xyz);

#define RngFill_uint32(sfmt,array,size) RngInit(sfmt);sfmt_fill_array32(sfmt,array,size)

#endif /* RNG_H_19239561826387126831623 */
