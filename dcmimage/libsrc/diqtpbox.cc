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
 *  Module:  dcmimage
 *
 *  Author:  Marco Eichelberg
 *
 *  Purpose: class DcmQuantPixelBoxArray
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 2002-01-25 13:32:11 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmimage/libsrc/diqtpbox.cc,v $
 *  CVS/RCS Revision: $Revision: 1.1 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#include "osconfig.h"
#include "diqtpbox.h"   /* for DcmQuantPixelBoxArray */

static int sumcompare(const void *x1, const void *x2) 
{
  return (*(DcmQuantPixelBoxPointer *) x2)->sum 
       - (*(DcmQuantPixelBoxPointer *) x1)->sum;
}

void DcmQuantPixelBoxArray::sort(unsigned long boxes)
{
#ifdef DEBUG
  assert(boxes <= length);
#endif
  qsort((char *)array, boxes,  sizeof(DcmQuantPixelBoxPointer), sumcompare);
}

DcmQuantPixelBoxArray::DcmQuantPixelBoxArray(unsigned long entries)
: array(NULL)
, length(entries)
{
  array = new DcmQuantPixelBoxPointer[entries];
  for (unsigned long i=0; i<entries; i++) array[i] = new DcmQuantPixelBox();
}

DcmQuantPixelBoxArray::~DcmQuantPixelBoxArray()
{
  for (unsigned long i=0; i<length; i++) delete array[i];
  delete[] array;
}

/*
 *
 * CVS/RCS Log:
 * $Log: diqtpbox.cc,v $
 * Revision 1.1  2002-01-25 13:32:11  meichel
 * Initial release of new color quantization classes and
 *   the dcmquant tool in module dcmimage.
 *
 *
 */
