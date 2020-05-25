/*********************************************************************//**
 * @file mem.h
 * @brief Defines for memory allocation routines.
 **********************************************************************/

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
