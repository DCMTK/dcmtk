/*
 *
 *  Copyright (C) 1996-99, OFFIS
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
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 1999-02-11 16:53:35 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmimgle/libsrc/dimoopx.cc,v $
 *  CVS/RCS Revision: $Revision: 1.3 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#include "osconfig.h"

#include "dimoopx.h"
#include "dimopx.h"


/*----------------*
 *  constructors  *
 *----------------*/

DiMonoOutputPixel::DiMonoOutputPixel(const DiMonoPixel *pixel,
                                     const unsigned long frames,
                                     const unsigned long max)
  : Count(((pixel != NULL) && (frames > 0)) ? pixel->getCount() / frames : 0),
    UsedValues(NULL),
    MaxValue(max)
{
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
            return (int)(UsedValues[value] == 0);
        return 2;                               // out of range
    }
    return 0;
}


/*
 *
 * CVS/RCS Log:
 * $Log: dimoopx.cc,v $
 * Revision 1.3  1999-02-11 16:53:35  joergr
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
