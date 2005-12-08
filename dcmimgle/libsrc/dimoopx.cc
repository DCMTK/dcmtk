/*
 *
 *  Copyright (C) 1996-2005, OFFIS
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
 *  Purpose: DicomMonoOutputPixel (Source)
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 2005-12-08 15:43:01 $
 *  CVS/RCS Revision: $Revision: 1.8 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#include "dcmtk/config/osconfig.h"

#include "dcmtk/dcmimgle/dimoopx.h"
#include "dcmtk/dcmimgle/dimopx.h"


/*----------------*
 *  constructors  *
 *----------------*/

DiMonoOutputPixel::DiMonoOutputPixel(const DiMonoPixel *pixel,
                                     const unsigned long size,
                                     const unsigned long frame,
                                     const unsigned long max)
  : Count(0),
    FrameSize(size),
    UsedValues(NULL),
    MaxValue(max)
{
    if (pixel != NULL)
    {
        if (pixel->getCount() > frame * size)
            Count = pixel->getCount() - frame * size;       // number of pixels remaining for this 'frame'
    }
    if (Count > FrameSize)
        Count = FrameSize;                                  // cut off at frame 'size'
}


/*--------------*
 *  destructor  *
 *--------------*/

DiMonoOutputPixel::~DiMonoOutputPixel()
{
    delete[] UsedValues;
}


/**********************************/


int DiMonoOutputPixel::isUnused(const unsigned long value)
{
    if (UsedValues == NULL)
        determineUsedValues();                  // create on demand
    if (UsedValues != NULL)
    {
        if (value <= MaxValue)
            return OFstatic_cast(int, UsedValues[value] == 0);
        return 2;                               // out of range
    }
    return 0;
}


/*
 *
 * CVS/RCS Log:
 * $Log: dimoopx.cc,v $
 * Revision 1.8  2005-12-08 15:43:01  meichel
 * Changed include path schema for all DCMTK header files
 *
 * Revision 1.7  2003/12/08 14:55:04  joergr
 * Adapted type casts to new-style typecast operators defined in ofcast.h.
 *
 * Revision 1.6  2001/06/01 15:49:58  meichel
 * Updated copyright header
 *
 * Revision 1.5  2000/03/08 16:24:31  meichel
 * Updated copyright header.
 *
 * Revision 1.4  1999/07/23 13:45:39  joergr
 * Enhanced handling of corrupted pixel data (wrong length).
 *
 * Revision 1.3  1999/02/11 16:53:35  joergr
 * Added routine to check whether particular grayscale values are unused in
 * the output data.
 * Removed unused parameter / member variable.
 *
 * Revision 1.2  1999/01/20 14:54:30  joergr
 * Replaced invocation of getCount() by member variable Count where possible.
 *
 * Revision 1.1  1998/11/27 16:15:02  joergr
 * Added copyright message.
 *
 * Revision 1.3  1998/05/11 14:52:33  joergr
 * Added CVS/RCS header to each file.
 *
 *
 */
