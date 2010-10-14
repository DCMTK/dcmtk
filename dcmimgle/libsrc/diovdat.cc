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
 *  Purpose: DicomOverlayData (Source)
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:14:18 $
 *  CVS/RCS Revision: $Revision: 1.7 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmdata/dctypes.h"

#include "dcmtk/dcmimgle/diovlay.h"
#include "dcmtk/dcmimgle/diovpln.h"

#include "dcmtk/ofstd/ofbmanip.h"


/*---------------*
 *  constructor  *
 *---------------*/

DiOverlayData::DiOverlayData(unsigned int entries,
                             unsigned int count)
  : Count(count),
    ArrayEntries(entries),
    Planes(NULL),
    DataBuffer(NULL)
{
    if ((entries > 0) && (entries <= DiOverlay::MaxOverlayCount))
    {
        Planes = new DiOverlayPlane *[entries];
        if (Planes != NULL)
            OFBitmanipTemplate<DiOverlayPlane *>::zeroMem(Planes, entries);
    }
}


/*---------------*
 *  destructors  *
 *---------------*/

DiOverlayData::~DiOverlayData()
{
    if (Planes != NULL)
    {
        register unsigned int i;
        for (i = 0; i < ArrayEntries; ++i)
            delete Planes[i];
    }
    delete[] Planes;
    delete[] DataBuffer;
}


/*
 *
 * CVS/RCS Log:
 * $Log: diovdat.cc,v $
 * Revision 1.7  2010-10-14 13:14:18  joergr
 * Updated copyright header. Added reference to COPYRIGHT file.
 *
 * Revision 1.6  2005/12/08 15:43:03  meichel
 * Changed include path schema for all DCMTK header files
 *
 * Revision 1.5  2003/12/23 16:03:18  joergr
 * Replaced post-increment/decrement operators by pre-increment/decrement
 * operators where appropriate (e.g. 'i++' by '++i').
 *
 * Revision 1.4  2003/12/08 17:43:04  joergr
 * Updated copyright header.
 *
 * Revision 1.3  2001/06/01 15:49:59  meichel
 * Updated copyright header
 *
 * Revision 1.2  2000/03/08 16:24:32  meichel
 * Updated copyright header.
 *
 * Revision 1.1  1999/09/17 14:21:33  joergr
 * Splitted file diovlay.h into two files (one for each class).
 *
 *
 *
 */
