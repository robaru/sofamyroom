/*********************************************************************//**
 * @file mem.h
 * @brief Defines for memory allocation routines.
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

#ifndef _MEM_H_19237197329817461287
#define _MEM_H_19237197329817461287

#ifdef MEX
#  include <stdlib.h>
#  include <matrix.h>
#  define MemMalloc(s)   mxMalloc(s)
#  define MemCalloc(n,s) mxCalloc(n,s)
#  define MemFree(p)     mxFree(p)
#else
#  include <stdlib.h>
#  define MemMalloc(s)   malloc(s)
#  define MemCalloc(n,s) calloc(n,s)
#  define MemFree(p)     free(p)
#endif

#endif /* _MEM_H_19237197329817461287 */
