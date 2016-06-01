/*
 *
 *  Copyright (C) 1996-2016, OFFIS e.V.
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
            Uint32 i = Count;                           // ... but normally not Valid !
            if ((MinValue == lut->getMinValue()) && (MaxValue == lut->getMaxValue()))
            {                                           // additional check for better performance
                const Uint16 *p = Data;
                const Uint16 *q = lut->getData();
                while ((i != 0) && (*(p++) == *(q++)))
                    --i;
            }
            result = (i != 0) ? 3 : 0;                  // check whether data is equal (0)
        }
    }
    return result;
}
