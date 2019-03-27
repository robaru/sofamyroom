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
 *************************************************************************/

#ifndef _LIBROOMSIM_H_51635172653123019823
#define _LIBROOMSIM_H_51635172653123019823

#include "types.h"
#include "interface.h"

void  ValidateSetup   ( const CRoomSetup *pSetup );
BRIR *Roomsim         ( const CRoomSetup *pSetup );
void  ReleaseBRIR     ( BRIR *brir );
void  ClearAllSensors ( void );

#endif /* #ifndef _LIBROOMSIM_H_51635172653123019823 */
