/*
 *
 *  Copyright (C) 1996-2003, OFFIS
 *
 *  This software and supporting documentation were developed by
 *
 *    Kuratorium OFFIS e.V.
 *    Healthcare Information and Communication Systems
 *    Escherweg 2
 *    D-26121 Oldenburg, Germany
 *
 *  THIS SOFTWARE IS MADE AVAILABLE,  AS IS,  AND OFFIS MAKES NO  WARRANTY
 *  REGARDING  THE  SOFTWARE,  ITS  PERFORMANCE,  ITS  MERCHANTABILITY  OR
 *  FITNESS FOR ANY PARTICULAR USE, FREEDOM FROM ANY COMPUTER DISEASES  OR
 *  ITS CONFORMITY TO ANY SPECIFICATION. THE ENTIRE RISK AS TO QUALITY AND
 *  PERFORMANCE OF THE SOFTWARE IS WITH THE USER.
 *
 *  Module:  dcmimgle
 *
 *  Author:  Joerg Riesmeier
 *
 *  Purpose: DicomOverlayData (Source)
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2003-12-08 17:43:04 $
 *  CVS/RCS Revision: $Revision: 1.4 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#include "osconfig.h"
#include "dctypes.h"

#include "diovlay.h"
#include "diovpln.h"

#include "ofbmanip.h"


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
        for (i = 0; i < ArrayEntries; i++)
            delete Planes[i];
    }
    delete[] Planes;
    delete[] DataBuffer;
}


/*
 *
 * CVS/RCS Log:
 * $Log: diovdat.cc,v $
 * Revision 1.4  2003-12-08 17:43:04  joergr
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
