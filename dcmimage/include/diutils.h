/*
**
** Author:  Joerg Riesmeier
** Created: 10.12.96
**
** Module:  diutils.h
**
** Purpose: Utilities (Header)
**
** Last Update:      $Author: joergr $
** Update Date:      $Date: 1998-06-25 08:50:10 $
** Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmimage/include/Attic/diutils.h,v $
** CVS/RCS Revision: $Revision: 1.5 $
** Status:           $State: Exp $
**
** CVS/RCS Log at end of file
**
*/


#ifndef __DIUTILS_H
#define __DIUTILS_H

#include "osconfig.h"
#include "dctypes.h"


/*---------------------*
 *  const definitions  *
 *---------------------*/

const unsigned long CIF_AcrNemaCompatibility      = 0x0000001;
const unsigned long CIF_WrongPaletteAttributeTags = 0x0000002;


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
    EMO_Default,
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
#define MAX_RAWPPM_BITS 8
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


/*
**
** CVS/RCS Log:
** $Log: diutils.h,v $
** Revision 1.5  1998-06-25 08:50:10  joergr
** Added compatibility mode to support ACR-NEMA images and wrong
** palette attribute tags.
**
** Revision 1.4  1998/05/11 14:53:30  joergr
** Added CVS/RCS header to each file.
**
**
*/
