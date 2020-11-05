/*********************************************************************//**
 * @file libroomsim.h
 * @brief Room simulation function prototypes.
 **********************************************************************/

#ifndef _LIBROOMSIM_H_51635172653123019823
#define _LIBROOMSIM_H_51635172653123019823

#include "types.h"
#include "interface.h"

void  ValidateSetup   ( const CRoomSetup *pSetup );
BRIR *Roomsim         ( const CRoomSetup *pSetup );
void  ReleaseBRIR     ( BRIR *brir );
void  ClearAllSensors ( void );

#endif /* #ifndef _LIBROOMSIM_H_51635172653123019823 */
