/*********************************************************************//**
 * @file interface.h
 * @brief MATLAB-C interface data types and function prototypes.
 **********************************************************************/

#ifndef _INTERFACE_H_94718466154761251723
#define _INTERFACE_H_94718466154761251723

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
