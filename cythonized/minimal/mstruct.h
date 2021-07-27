/*********************************************************************//**
 * @file mstruct.h
 * @brief Macros for MATLAB-C types, prototypes and conversion routines.
 **********************************************************************/
#include <stdbool.h>


typedef struct {
	int first;
	float second;
} MyStruct;

/***************************
 * Define types            *
 ***************************/
//
//#define STRUCTBEGIN(n)                  typedef struct {
//#define STRUCTEND(n)                    } n;
//
//#define FIELDSTRING(n)                  const char *n;
//#define FIELDDOUBLEARRAY(n,c)           double n[c];
//
///***
// * ACTUAL PARAMETER STRUCTURE
// */
//
//STRUCTBEGIN ( CSensor )
//    FIELDDOUBLEARRAY ( location, 3    )
//    FIELDDOUBLEARRAY ( orientation, 3 )
//    FIELDSTRING      ( description    )
//STRUCTEND ( CSensor )
//
