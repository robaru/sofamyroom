/*********************************************************************//**
 * @file mstruct.h
 * @brief Macros for MATLAB-C types, prototypes and conversion routines.
 **********************************************************************/
#include <stdbool.h>

//typedef struct {
//	int first;
//	float second;
//} CSensor;
//
//typedef struct {
//	int nSources;
//	const CSensor *source;
//} CRoomSetup;
//
//typedef struct {
//	int first;
//	float second;
//} MyStruct;


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

	FIELDSTRING	  ( outputname			)
STRUCTEND ( COptions )

STRUCTBEGIN ( CSensor )
    FIELDDOUBLEARRAY ( location, 3    )
    FIELDDOUBLEARRAY ( orientation, 3 )
    FIELDSTRING      ( description    )
STRUCTEND ( CSensor )

STRUCTBEGIN ( CRoomSetup )
    FIELDSTRUCT         ( CRoom, room                   )
    FIELDSTRUCT         ( COptions, options             )
    FIELDDYNSTRUCTARRAY ( CSensor, source, nSources     )
    FIELDDYNSTRUCTARRAY ( CSensor, receiver, nReceivers )
STRUCTEND ( CRoomSetup )


//typedef struct
//{
//    double  fs;
//    int     nChannels;
//    int     nSamples;
//    double  *sample;
//} BRIR;
