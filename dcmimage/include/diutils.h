/*********************************************************************
** 
**  Utilities (Header)
**
**  author   : Joerg Riesmeier
**  created  : 10.12.96
**  modified : 10.02.97
**
*********************************************************************/


#ifndef __DIUTILS_H
#define __DIUTILS_H

#include "osconfig.h"
#include "dctypes.h"


/*--------------------*
 *  type definitions  *
 *--------------------*/

enum EP_Representation 
{
    EPR_Uint8, EPR_MinUnsigned = EPR_Uint8,
    EPR_Sint8, EPR_MinSigned = EPR_Sint8,
	EPR_Uint16,
	EPR_Sint16,
	EPR_Uint32, EPR_MaxUnsigned = EPR_Uint32,
	EPR_Sint32, EPR_MaxSigned = EPR_Sint32
};


enum EI_Status 
{
    EIS_Normal,
	EIS_NoDataDictionary,
    EIS_InvalidDocument,
    EIS_MissingAttribute,
    EIS_InvalidValue,
    EIS_NotSupportedValue,
	EIS_MemoryFailure,
	EIS_InvalidImage,
	EIS_OtherError
};


enum EM_Overlay
{
    EMO_Replace,
    EMO_ThresholdReplace,
    EMO_Complement,
	EMO_RegionOfInterest
};


/*---------------------*
 *  macro definitions  *
 *---------------------*/

#define MAX_UINT Uint32
#define MAX_SINT Sint32

#define MAX_BITS 32
#define MAX_RAWPPM_BITS	8
#define MAX_INTERPOLATION_BITS 16

#define bitsof(expr) (sizeof(expr) << 3)


/*-------------------------*
 *  function declarations  *
 *-------------------------*/

EP_Representation determineRepresentation(double, double);


/*-------------------*
 *  inline functions *
 *-------------------*/

inline unsigned long maxval(int bits, unsigned long pos = 1)
{
	return (bits < MAX_BITS) ? ((unsigned long)1 << bits) - pos : (Uint32)-1;
}


#endif
