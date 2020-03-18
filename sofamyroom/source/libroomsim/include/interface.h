/*********************************************************************//**
 * @file interface.h
 * @brief MATLAB-C interface data types and function prototypes.
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

#ifndef _INTERFACE_H_94718466154761251723
#define _INTERFACE_H_94718466154761251723

#include "mysofa.h"

/* type declarations */
#define MSTRUCT_DECLARE
#undef MSTRUCT_PROTOTYPE
#undef MSTRUCT_LOAD
#include "mstruct.h"

/* function prototypes */
#undef MSTRUCT_DECLARE
#define MSTRUCT_PROTOTYPE
#undef MSTRUCT_LOAD
#include "mstruct.h"

#endif /* _INTERFACE_H_94718466154761251723 */
