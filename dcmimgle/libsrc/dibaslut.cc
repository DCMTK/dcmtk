/*
 *
 *  Copyright (C) 1999, OFFIS
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
 *  Purpose: DicomBaseLUT (Source)
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 1999-02-03 17:47:44 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmimgle/libsrc/dibaslut.cc,v $
 *  CVS/RCS Revision: $Revision: 1.1 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#include "osconfig.h"

#include "dibaslut.h"


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


/*--------------*
 *  destructor  *
 *--------------*/

DiBaseLUT::~DiBaseLUT()
{
    delete[] DataBuffer;
}


/********************************************************************/




/*
 *
 * CVS/RCS Log:
 * $Log: dibaslut.cc,v $
 * Revision 1.1  1999-02-03 17:47:44  joergr
 * Added base class for look-up tables (moved main functionality of class
 * DiLookupTable to DiBaseLUT).
 *
 *
 *
 */
