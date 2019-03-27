/*********************************************************************//**
 * @file msg.h
 * @brief Defines for messaging routines.
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

#ifndef _MSG_H_85179461704471264112
#define _MSG_H_85179461704471264112

#ifdef MEX
#   include <mex.h>
#	define MsgPrintf		 mexPrintf
#   define MsgRelax          mexEvalString("drawnow;")
#	define MsgErrorExit(msg) mexErrMsgTxt(msg)
#else
#	include <stdio.h>
#   include <stdlib.h>
#	define MsgPrintf		 printf
#   define MsgRelax          
#	define MsgErrorExit(msg) { fprintf(stderr,"error: %s\nPress return to exit...\n", msg); getchar(); exit(1); }
#endif

#endif /* _MSG_H_85179461704471264112 */
