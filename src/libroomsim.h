/*********************************************************************//**
 * @file libroomsim.h
 * @brief Room simulation function prototypes.
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
 *************************************************************************
 * Modified for SofaMyRoom (https://github.com/robaru/sofamyroom).
 * Copyright 2019-2026, Roberto Barumerli and Daniele Bianchi
 *
 * Changes with respect to the original ROOMSIM file:
 *   - Documentation comments added to the prototypes (the code is
 *     unchanged).
 *
 * The modifications are released under the same GNU General Public
 * License, version 3 or later.
 *************************************************************************/

#ifndef _LIBROOMSIM_H_51635172653123019823
#define _LIBROOMSIM_H_51635172653123019823

#include "types.h"
#include "interface.h"

/**
 * Check the ranges and sizes of the setup (six surfaces, one column per
 * frequency band, absorption and diffusion in [0,1], ...). An invalid
 * setup stops the program with the message ``invalid setup: ...``; in the
 * embedded library the message is returned by smr_last_error() instead.
 */
void  ValidateSetup   ( const CRoomSetup *pSetup );

/**
 * Run the simulation and return an array of nSources*nReceivers
 * responses, indexed ``r*nSources + s`` for source s and receiver r,
 * followed by a terminating element whose sample pointer is NULL.
 */
BRIR *Roomsim         ( const CRoomSetup *pSetup );

/** Free an array returned by Roomsim(). */
void  ReleaseBRIR     ( BRIR *brir );

/**
 * Free the sources and receivers loaded so far (SOFA files are kept in
 * memory between simulations).
 */
void  ClearAllSensors ( void );

#endif /* #ifndef _LIBROOMSIM_H_51635172653123019823 */
