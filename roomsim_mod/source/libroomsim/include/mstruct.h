/*********************************************************************//**
 * @file mstruct.h
 * @brief Macros for MATLAB-C types, prototypes and conversion routines.
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

/* clear existing definitions */
#undef STRUCTBEGIN
#undef STRUCTEND
#undef FIELDBOOL
#undef FIELDINT
#undef FIELDDOUBLE
#undef FIELDSTRING
#undef FIELDSTRUCT
#undef FIELDINTARRAY
#undef FIELDDOUBLEARRAY
#undef FIELDSTRUCTARRAY
#undef FIELDDYNDOUBLEARRAY
#undef FIELDDYNDOUBLEARRAY2D
#undef FIELDDYNSTRUCTARRAY

/***************************
 * Define types            *
 ***************************/
#if defined(MSTRUCT_DECLARE)

#  ifdef MEX
#    include <matrix.h>
#  else
#	ifndef _STDBOOL
	 typedef int bool;
#    define TRUE  1
#    define FALSE 0
#	else
#	 define TRUE true
#	 define FALSE false
#	endif
#  endif

#define STRUCTBEGIN(n)                  typedef struct {
#define STRUCTEND(n)                    } n;

#define FIELDBOOL(n)                    bool n;
#define FIELDINT(n)                     int n;
#define FIELDDOUBLE(n)                  double n;
#define FIELDSTRING(n)                  const char *n;
#define FIELDSTRUCT(t,n)                t n;

#define FIELDINTARRAY(n,c)              int n[c];
#define FIELDDOUBLEARRAY(n,c)           double n[c];
#define FIELDSTRUCTARRAY(t,n,c)         t n[c];
#define FIELDDYNDOUBLEARRAY(na,nc)      int nc; const double *na;
#define FIELDDYNDOUBLEARRAY2D(na,ncR,ncC) int ncR; int ncC; const double *na;
#define FIELDDYNSTRUCTARRAY(t,na,nc)    int nc; const t *na;


/***************************
 * Define prototypes       *
 ***************************/
#elif defined(MSTRUCT_PROTOTYPE)

#  ifdef MEX

#    include <matrix.h>
#    define STRUCTBEGIN(n) void Load##n(const mxArray *prhs, int index, n *plhs);

#  else

#    include "setup.h"
#    define STRUCTBEGIN(n) void Load##n(CFileSetupItem *pItem, n *p);

#  endif

#define STRUCTEND(n)

#define FIELDBOOL(n)
#define FIELDINT(n) 
#define FIELDDOUBLE(n)
#define FIELDSTRING(n)
#define FIELDSTRUCT(t,n)

#define FIELDINTARRAY(n,c)
#define FIELDDOUBLEARRAY(n,c)
#define FIELDSTRUCTARRAY(t,n,c)
#define FIELDDYNDOUBLEARRAY(na,nc)
#define FIELDDYNDOUBLEARRAY2D(na,ncR,ncC)
#define FIELDDYNSTRUCTARRAY(t,na,nc)


/***************************
 * Define loaders          *
 ***************************/
#elif defined(MSTRUCT_LOAD)

#  ifdef MEX
#    include <mex.h>
#    include <matrix.h>
#    include <stdlib.h>

#    define STRUCTBEGIN(n) void Load##n(const mxArray *prhs, int index, n *plhs) { mxArray *tmp; 
#    define STRUCTEND(n) }

#    define FIELDBOOL(n) \
        if (!(tmp = mxGetField(prhs,index,#n))) mexErrMsgTxt("missing field '" #n "'"); \
        if (!(mxIsLogical(tmp) && (mxGetNumberOfElements(tmp)==1))) mexErrMsgTxt("expected field '" #n "' to be a logical scalar"); \
        (plhs->n) = *((bool *)mxGetData(tmp));

#define FIELDINT(n) \
    if (!(tmp = mxGetField(prhs,index,#n))) mexErrMsgTxt("missing field '" #n "'"); \
    if (!mxIsNumeric(tmp) || mxGetNumberOfElements(tmp)!=1) mexErrMsgTxt("expected field '" #n "' to be a numeric scalar"); \
    (plhs->n) = (int) *((double *)mxGetData(tmp));
    
#define FIELDDOUBLE(n) \
    if (!(tmp = mxGetField(prhs,index,#n))) mexErrMsgTxt("missing field '" #n "'"); \
    if (!mxIsDouble(tmp) || mxGetNumberOfElements(tmp)!=1) mexErrMsgTxt("expected field '" #n "' to be a double scalar"); \
    (plhs->n) = *((double *)mxGetData(tmp));

/* NB: plhs->n should be freed using mxFree upon unloading */
#define FIELDSTRING(n) \
    if (!(tmp = mxGetField(prhs,index,#n))) mexErrMsgTxt("missing field '" #n "'"); \
    if (!mxIsChar(tmp)) mexErrMsgTxt("expected field '" #n "' to be a string"); \
    (plhs->n) = mxArrayToString(tmp); 

#define FIELDSTRUCT(t,n) \
    if (!(tmp = mxGetField(prhs,index,#n))) mexErrMsgTxt("missing field '" #n "'"); \
    if (!mxIsStruct(tmp)) mexErrMsgTxt("expected field '" #n "' to be a structure"); \
    if (mxGetNumberOfElements(tmp)!=1) mexErrMsgTxt("structure array unexpected for field '" #n "'"); \
    Load##t(tmp,0,&(plhs->n));

/* NB: integer valued matlab data of type double expected */
#define FIELDINTARRAY(n,c) \
    if (!(tmp = mxGetField(prhs,index,#n))) mexErrMsgTxt("missing field '" #n "'"); \
    if (!mxIsDouble(tmp) || mxGetNumberOfElements(tmp)>c) mexErrMsgTxt("expected field '" #n "' to be a double vector with at most " #c " elements"); \
    { int i; double *data = mxGetData(tmp); for (i=0; i<c; i++) plhs->n[i] = (int) (data[i]); }

#define FIELDDOUBLEARRAY(n,c) \
    if (!(tmp = mxGetField(prhs,index,#n))) mexErrMsgTxt("missing field '" #n "'"); \
    if (!mxIsDouble(tmp) || mxGetNumberOfElements(tmp)>c) mexErrMsgTxt("expected field '" #n "' to be a double vector with at most " #c " elements"); \
    { int i; double *data = mxGetData(tmp); for (i=0; i<c; i++) (plhs->n[i]) = data[i]; }

#define FIELDSTRUCTARRAY(t,n,c) \
    if (!(tmp = mxGetField(prhs,index,#n))) mexErrMsgTxt("missing field '" #n "'"); \
    if (!mxIsStruct(tmp)) mexErrMsgTxt("expected field '" #n "' to be a structure array"); \
    if (mxGetNumberOfElements(tmp)>c) mexErrMsgTxt("structure array with at most " #c " elements expected for field '" #n "'"); \
    { int i; for (i=0; i<c; i++) Load##t(tmp,i,&(plhs->n[i])); }

#define FIELDDYNDOUBLEARRAY(na,nc) \
    if (!(tmp = mxGetField(prhs,index,#na))) mexErrMsgTxt("missing field '" #na "'");  \
    if (!mxIsDouble(tmp)) mexErrMsgTxt("expected field '" #na "' to be a double vector"); \
    (plhs->nc) = (int) mxGetNumberOfElements(tmp); \
    (plhs->na) = (double *)mxGetData(tmp);

#define FIELDDYNDOUBLEARRAY2D(na,ncR,ncC) \
    if (!(tmp = mxGetField(prhs,index,#na))) mexErrMsgTxt("missing field '" #na "'");  \
    if (mxGetNumberOfDimensions(tmp)!=2) mexErrMsgTxt("expected field '" #na "' to be a two-dimensional array");  \
    (plhs->ncR) = (int) mxGetM(tmp); \
    (plhs->ncC) = (int) mxGetN(tmp); \
	(plhs->na)  = (double *) mxMalloc((plhs->ncR) * (plhs->ncC) * sizeof(double)); \
	{ \
	    int r,c; double *colarr = (double *) mxGetData(tmp); \
		for (r=0; r<(plhs->ncR); r++) { \
			for (c=0; c<(plhs->ncC); c++) { \
			    ((double *)(plhs->na))[r*(plhs->ncC)+c] = colarr[c*(plhs->ncR)+r]; \
			} \
		} \
     }

/*    (plhs->na)  = (double *) mxGetData(tmp); */

/* NB: memory should be freed upon unloading */
#define FIELDDYNSTRUCTARRAY(t,na,nc) \
    if (!(tmp = mxGetField(prhs,index,#na))) mexErrMsgTxt("missing field '" #na "'");  \
    (plhs->nc) = (int) mxGetNumberOfElements(tmp); \
    (plhs->na) = (t *) mxMalloc((plhs->nc) * sizeof(t)); \
    { int i; for (i=0; i<plhs->nc; i++) Load##t(tmp,i,(t *)(&((plhs->na)[i]))); }

#  else /* !MEX */

/* 
#    define STRUCTBEGIN(n)
#    define STRUCTEND(n)

#    define FIELDBOOL(n)
#    define FIELDINT(n) 
#    define FIELDDOUBLE(n)
#    define FIELDSTRING(n)
#    define FIELDSTRUCT(t,n)

#    define FIELDINTARRAY(n,c)
#    define FIELDDOUBLEARRAY(n,c)
#    define FIELDSTRUCTARRAY(t,n,c)
#    define FIELDDYNDOUBLEARRAY(na,nc)
#    define FIELDDYNDOUBLEARRAY2D(na,ncR,ncC)
#    define FIELDDYNSTRUCTARRAY(t,na,nc)
*/

#include <string.h>
#include <stdlib.h>

#include "defs.h"
#include "mem.h"
#include "msg.h"
#include "setup.h"

#define ENTERARRAY(p)	while (*(p)!='[' && *(p)!=0) (p)++; if (*(p)) (p)++
#define SKIPWHITE(p)	while (*(p)<=' ' && *(p)>0) (p)++
#define SKIPCOLSEP(p)	while ((*(p)<=' ' && *(p)>0) || *(p)==',') (p)++
#define SKIPDATA(p)		while (*(p)>' ' && *(p)!=',' && *(p)!=';' && *(p)!=']') (p)++
#define SKIPROWSEP(p)	SKIPWHITE(p); if (*(p)!=';') { /* error */ } (p)++; SKIPWHITE(p)

#define GETFIELD(n) \
	pSubItem = SetupFindField(pItem,#n); \
if (!pSubItem) { MsgPrintf("missing field '"); SetupPrintItemName(pItem); MsgPrintf("." #n "'\n"); return; } 

#define GETSTRUCT(n) \
	pSubItem = SetupFindStruct(pItem,#n); \
if (!pSubItem) { MsgPrintf("missing field '"); SetupPrintItemName(pItem); MsgPrintf("." #n "'\n"); return; } 

#define COUNTFIELDS(count) \
	{ CFileSetupItem *field=pSubItem->data.field; (count)=0; while (field) { field=field->next; (count)++; } } 

int ParseBool(char *str)
{
	if (stricmp(str,"true")==0 || strcmp(str,"1")==0)
		return 1;
	if (stricmp(str,"false")==0 || strcmp(str,"0")==0)
		return 0;
	/* error message */
	return 0;
}

void ParseIntArray(char *str, int *array, int count)
{
	int i;
	ENTERARRAY(str);
	for (i=0; i<count; i++)	array[i] = (int) strtol(str,&str,10);
	while (*str<=' ' && *str>0) str++;
	if (*str!=']') { /* error */ }
}

void ParseDoubleArray(char *str, double *array, int count)
{
	int i;
	ENTERARRAY(str);
	for (i=0; i<count; i++)	array[i] = strtod(str,&str);
	while (*str<=' ' && *str>0) str++;
	if (*str!=']') { /* error */ }
}

void GetArrayDims(char *str, int *nr, int *nc)
{
	char *p;

	*nr = 1; 
	*nc = 0;
	ENTERARRAY(str); 
	SKIPWHITE(str);

	p = str;
	while (*p!=']' && *p!=0)
	{
		if (*p++==';') 
			*nr += 1;
	}

	p = str;
	while (*p!=']' && *p!=';' && *p!=0)
	{
		SKIPDATA(p);
		*nc += 1;
		SKIPCOLSEP(p);
	}
}

void ParseDynDoubleArray(char *str, double **array, int *count)
{
	int  nr, nc, i;

	GetArrayDims(str,&nr,&nc);
	if (nr!=1 || nc==0)
	{
		*array = NULL; 
		*count = 0;
		return;
	}
	*array = MemMalloc(nc*sizeof(double));
	*count = nc;

	ENTERARRAY(str);
	SKIPWHITE(str);
	for (i=0; i<*count; i++)
	{
		(*array)[i] = strtod(str,&str);
		SKIPCOLSEP(str);
	}
}

void ParseDynDoubleArray2D(char *str, double **array, int *nr, int *nc)
{
	int r, c, idx=0;

	GetArrayDims(str,nr,nc);
	if ((*nr) * (*nc) == 0)
	{
		*nr = 0;
		*nc = 0;
		*array = NULL;
		return;
	}

	*array = MemMalloc((*nr) * (*nc) * sizeof(double));

	ENTERARRAY(str);
	SKIPWHITE(str);
	for (r=0; r<(*nr); r++)
	{
		for (c=0; c<(*nc); c++)
		{
			(*array)[idx++] = strtod(str,&str);
			SKIPCOLSEP(str);
		}
		SKIPROWSEP(str);
	}
}

#    define STRUCTBEGIN(n)			void Load##n(CFileSetupItem *pItem, n *p) { CFileSetupItem *pSubItem;
#    define STRUCTEND(n)			}

#    define FIELDBOOL(n)			GETFIELD(n); p->n = ParseBool(pSubItem->data.value);
#    define FIELDINT(n)				GETFIELD(n); p->n = (int) strtol(pSubItem->data.value,NULL,10);
#    define FIELDDOUBLE(n)			GETFIELD(n); p->n = strtod(pSubItem->data.value,NULL);
#    define FIELDSTRING(n)			GETFIELD(n); p->n = pSubItem->data.value;
#    define FIELDSTRUCT(t,n)		GETSTRUCT(n); Load##t(pSubItem, &p->n);
#    define FIELDINTARRAY(n,c)		GETFIELD(n); ParseIntArray(pSubItem->data.value, p->n, c);
#    define FIELDDOUBLEARRAY(n,c)	GETFIELD(n); ParseDoubleArray(pSubItem->data.value, p->n, c);

#    define FIELDSTRUCTARRAY(t,n,c) \
	GETSTRUCT(n); \
	{ int i; pSubItem = pSubItem->data.field; for (i=0; i<p->c; i++) { Load##t(pSubItem->data.field,(t *)&p->n[i]); pSubItem = pSubItem->next; } }

#    define FIELDDYNDOUBLEARRAY(na,nc) \
	GETFIELD(na); ParseDynDoubleArray(pSubItem->data.value, (double **)&p->na, &p->nc);

#    define FIELDDYNDOUBLEARRAY2D(na,ncR,ncC) \
	GETFIELD(na); ParseDynDoubleArray2D(pSubItem->data.value, (double **)&p->na, &p->ncR, &p->ncC);

#    define FIELDDYNSTRUCTARRAY(t,na,nc) \
	GETSTRUCT(na); COUNTFIELDS(p->nc); \
	p->na = MemMalloc(p->nc * sizeof(t)); \
	{ int i; pSubItem=pSubItem->data.field; for (i=0; i<p->nc; i++) { Load##t(pSubItem,(t *)&p->na[i]); pSubItem = pSubItem->next; } }

#  endif /* MEX */

#else

#  error "Specify one of MSTRUCT_DECLARE, MSTRUCT_PROTOTYPE, and MSTRUCT_LOAD"

#endif

/***
 * ACTUAL PARAMETER STRUCTURE 
 */

STRUCTBEGIN ( CSurface )
    FIELDDYNDOUBLEARRAY   ( frequency,  nBands                           )
    FIELDDYNDOUBLEARRAY2D ( absorption, nRowsAbsorption, nColsAbsorption )
    FIELDDYNDOUBLEARRAY2D ( diffusion,  nRowsDiffusion, nColsDiffusion   )
STRUCTEND ( CSurface )

STRUCTBEGIN ( CRoom )
    FIELDDOUBLEARRAY ( dimension, 3      )
    FIELDSTRUCT      ( CSurface, surface )
    FIELDDOUBLE      ( humidity          )
    FIELDDOUBLE      ( temperature       )
STRUCTEND ( CRoom )

STRUCTBEGIN ( COptions )
    FIELDDOUBLE   ( fs                  )
    FIELDDOUBLE   ( responseduration    )
    FIELDDOUBLE   ( bandsperoctave      )
    FIELDDOUBLE   ( referencefrequency  )
    FIELDBOOL     ( airabsorption       )
    FIELDBOOL     ( distanceattenuation )
    FIELDBOOL     ( subsampleaccuracy   )
    FIELDDOUBLE   ( highpasscutoff      )
    FIELDBOOL     ( verbose             )

	FIELDBOOL	  ( simulatespecular    )
    FIELDINTARRAY ( reflectionorder, 3  )

	FIELDBOOL	  ( simulatediffuse     )
	FIELDINT      ( numberofrays        )
	FIELDDOUBLE   ( diffusetimestep     )
	FIELDDOUBLE   ( rayenergyfloordB    )
	FIELDBOOL	  ( uncorrelatednoise   )
STRUCTEND ( COptions )

STRUCTBEGIN ( CSensor )
    FIELDDOUBLEARRAY ( location, 3    )
    FIELDDOUBLEARRAY ( orientation, 3 )
    FIELDSTRING      ( description    )
#	ifdef SOFA
	FIELDBOOL ( interpolate )
#	endif
STRUCTEND ( CSensor )

STRUCTBEGIN ( CRoomSetup )
    FIELDSTRUCT         ( CRoom, room                   )
    FIELDSTRUCT         ( COptions, options             )
    FIELDDYNSTRUCTARRAY ( CSensor, source, nSources     )
    FIELDDYNSTRUCTARRAY ( CSensor, receiver, nReceivers )
STRUCTEND ( CRoomSetup )
