/*
 *
 *  Copyright (C) 1996-2010, OFFIS e.V.
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  This software and supporting documentation were developed by
 *
 *    OFFIS e.V.
 *    R&D Division Health
 *    Escherweg 2
 *    D-26121 Oldenburg, Germany
 *
 *
 *  Module:  dcmimgle
 *
 *  Author:  Joerg Riesmeier
 *
 *  Purpose: DicomBaseLUT (Source)
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:14:17 $
 *  CVS/RCS Revision: $Revision: 1.13 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#include "dcmtk/config/osconfig.h"

#include "dcmtk/dcmimgle/dibaslut.h"


/*----------------*
 *  constructors  *
 *----------------*/

DiBaseLUT::DiBaseLUT(const Uint32 count,
                     const Uint16 bits)
  : Count(count),
    FirstEntry(0),
    Bits(bits),
    MinValue(0),
    MaxValue(0),
    Valid(0),
    Explanation(),
    Data(NULL),
    DataBuffer(NULL)
{
}


DiBaseLUT::DiBaseLUT(Uint16 *buffer,
                     const Uint32 count,
                     const Uint16 bits)
  : Count(count),
    FirstEntry(0),
    Bits(bits),
    MinValue(0),
    MaxValue(0),
    Valid(0),
    Explanation(),
    Data(buffer),
    DataBuffer(buffer)
{
}


/*--------------*
 *  destructor  *
 *--------------*/

DiBaseLUT::~DiBaseLUT()
{
    delete[] DataBuffer;
}


/********************************************************************/


OFBool DiBaseLUT::operator==(const DiBaseLUT &lut)
{
    return (compare(&lut) == 0);
}


int DiBaseLUT::compare(const DiBaseLUT *lut)
{
    int result = 1;                                     // invalid LUT (1)
    if (Valid && (lut != NULL) && lut->isValid())
    {
        result = 2;                                     // descriptor differs (2)
        if ((Count == lut->getCount()) && (FirstEntry == lut->getFirstEntry()) && (Bits == lut->getBits()))
        {                                               // special case: if Count == 0 LUT data is equal
            register Uint32 i = Count;                  // ... but normally not Valid !
            if ((MinValue == lut->getMinValue()) && (MaxValue == lut->getMaxValue()))
            {                                           // additional check for better performance
                register const Uint16 *p = Data;
                register const Uint16 *q = lut->getData();
                while ((i != 0) && (*(p++) == *(q++)))
                    --i;
            }
            result = (i != 0) ? 3 : 0;                  // check whether data is equal (0)
        }
    }
    return result;
}


/*
 *
 * CVS/RCS Log:
 * $Log: dibaslut.cc,v $
 * Revision 1.13  2010-10-14 13:14:17  joergr
 * Updated copyright header. Added reference to COPYRIGHT file.
 *
 * Revision 1.12  2005/12/08 15:42:43  meichel
 * Changed include path schema for all DCMTK header files
 *
 * Revision 1.11  2003/12/23 16:03:18  joergr
 * Replaced post-increment/decrement operators by pre-increment/decrement
 * operators where appropriate (e.g. 'i++' by '++i').
 *
 * Revision 1.10  2003/12/08 17:43:04  joergr
 * Updated copyright header.
 *
 * Revision 1.9  2001/06/01 15:49:53  meichel
 * Updated copyright header
 *
 * Revision 1.8  2000/03/08 16:24:26  meichel
 * Updated copyright header.
 *
 * Revision 1.7  1999/10/20 10:36:36  joergr
 * Enhanced method invertTable to distinguish between copy of LUT data and
 * original (referenced) LUT data.
 *
 * Revision 1.6  1999/09/30 11:37:54  joergr
 * Added methods to compare two lookup tables.
 *
 * Revision 1.5  1999/09/17 13:13:27  joergr
 * Enhanced efficiency of some "for" loops.
 *
 * Revision 1.4  1999/09/08 15:20:31  joergr
 * Completed implementation of setting inverse presentation LUT as needed
 * e.g. for DICOM print (invert 8->12 bits PLUT).
 *
 * Revision 1.3  1999/03/03 17:56:12  joergr
 * Added type cast to avoid compiler warning on MSVC5.
 *
 * Revision 1.2  1999/03/03 12:05:43  joergr
 * Added method to invert lookup table data (used for presentation state LUTs).
 *
 * Revision 1.1  1999/02/03 17:47:44  joergr
 * Added base class for look-up tables (moved main functionality of class
 * DiLookupTable to DiBaseLUT).
 *
 *
 */
