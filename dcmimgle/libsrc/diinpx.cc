/*
 *
 *  Copyright (C) 1996-2001, OFFIS
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
 *  Purpose: DicomInputPixel (Source)
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 2001-06-01 15:49:56 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmimgle/libsrc/diinpx.cc,v $
 *  CVS/RCS Revision: $Revision: 1.5 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#include "osconfig.h"

#include "diinpx.h"


/*----------------*
 *  constructors  *
 *----------------*/

DiInputPixel::DiInputPixel(const unsigned int bits,
                           const unsigned long start,
                           const unsigned long count)
  : Count(0),
    Bits(bits),
    PixelStart(start),
    PixelCount(count),
    AbsMinimum(0),
    AbsMaximum(0)
{
}


/*--------------*
 *  destructor  *
 *--------------*/

DiInputPixel::~DiInputPixel()
{
}


/*
 *
 * CVS/RCS Log:
 * $Log: diinpx.cc,v $
 * Revision 1.5  2001-06-01 15:49:56  meichel
 * Updated copyright header
 *
 * Revision 1.4  2000/04/27 13:10:28  joergr
 * Dcmimgle library code now consistently uses ofConsole for error output.
 *
 * Revision 1.3  2000/03/08 16:24:29  meichel
 * Updated copyright header.
 *
 * Revision 1.2  1999/02/03 17:39:20  joergr
 * Added member variable and related methods to store number of bits used for
 * pixel data.
 *
 * Revision 1.1  1998/11/27 16:02:13  joergr
 * Added copyright message.
 *
 * Revision 1.2  1998/05/11 14:52:29  joergr
 * Added CVS/RCS header to each file.
 *
 *
 */
