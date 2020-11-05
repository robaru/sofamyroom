/*********************************************************************//**
 * @file msg.h
 * @brief Defines for messaging routines.
 **********************************************************************/
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
