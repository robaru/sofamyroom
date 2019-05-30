/*********************************************************************//**
 * @file defs.h
 * @brief Project-wide preprocessor defines.
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

#ifndef _DEFS_H_9834798749126316831231
#define _DEFS_H_9834798749126316831231

/*************
 * Constants *
 *************/
#if 0
#  define EPS0    (4.9407e-324)           /**< Smallest double >0 (2^(-1074)). */
#endif
#define EPS0    (1e-40)                  /**< Reasonably small double >0. */
#define PI      3.14159265358979323846  /**< Mathematical constant, \\pi. */
#define TWOPI   (2*PI)                  /**< Mathematical constant, 2\\pi. */
#define ANY_FS  0.0                     /**< Value signaling that a sensor can be used at any sampling frequency. */

#define EMPTY_GAIN  (1e42)              /**< Gain sensor return value signaling that the response at the specified \a XYZ is not defined. */

/*************
 * Functions *
 *************/
#if defined(MEX)
#  define LOGFN mexPrintf
#else
#  define LOGFN printf
#endif

/* LOGLEVEL: 0=nothing, 1=INFO, 2=DEBUG */
#if LOGLEVEL>=1
#  define LOGINFO(s)  LOGFN s
#  define LOG(s)      LOGFN s;
#else
#  define LOGINFO(s)
#  define LOG(s)
#endif
#if LOGLEVEL>=2
#  define LOGDEBUG(s) LOGFN s
#else
#  define LOGDEBUG(s)
#endif
                      
#ifndef MAX
#define MAX(x,y) ((x)>(y)?(x):(y))
#endif

#ifndef MIN
#define MIN(x,y) ((x)<(y)?(x):(y))
#endif

/* Mathematical functions */
#define ROUND(x)         ((int)floor((x)+0.5))
#define DEG2RAD(degree)	 ((degree)/180.0 * PI)
#define LOGMINIMUM		 -13
#define LOGEPS			 1E-300
#define LOGDOMAIN(x)     log(x)
#define LOGDOMAINSAFE(x) log(x + LOGEPS)
#define LINDOMAIN(x)	 exp(x)

#define CHECKPOINT(TXT) mexPrintf("checkpoint at %s(%d) " TXT "\n",__FILE__,__LINE__);

#if defined(__LCC__)
#    define LOG2(x)  log2(x)
#else
#    define LOG2(x)  (log(x)/0.69314718055994529)
#endif

/* to avoid compiler warnings about unreferenced parameters */
#ifdef _MSC_VER
#  define UNREFERENCED_PARAMETER(P) (P)
#else
#  define UNREFERENCED_PARAMETER(P) 
#endif

/* DEBUG OUTPUT */
/* #define LOGRAYS */
/* #define LOGRAYS_EXTRA */
/* #define LOGTFS */

/* system-dependent functions */
#if 1
	#ifdef __GNUC__
		#define stricmp     strcasecmp
		#define strnicmp    strncasecmp
	#else
		#define stricmp     _stricmp
		#define strnicmp    _strnicmp
	#endif	
#endif

#endif /* #ifndef _DEFS_H_9834798749126316831231 */
