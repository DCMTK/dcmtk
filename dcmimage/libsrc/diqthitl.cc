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
 *  Purpose: class DcmQuantHistogramItemList
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 2002-01-25 13:32:11 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmimage/libsrc/diqthitl.cc,v $
 *  CVS/RCS Revision: $Revision: 1.1 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "osconfig.h"
#include "diqthitl.h"

DcmQuantHistogramItemList::DcmQuantHistogramItemList()
: list()
, first(list.end())
, last(list.end())
{
}

DcmQuantHistogramItemList::~DcmQuantHistogramItemList()
{
  first = list.begin();
  while (first != last)
  {
    delete *first;
    first = list.erase(first);
  }
}

void DcmQuantHistogramItemList::moveto(DcmQuantHistogramItemPointer *array, unsigned long& counter, unsigned long numcolors)
{
  first = list.begin();
  while ((first != last) && (counter < numcolors))
  {
    array[counter++] = *first;
    first = list.erase(first);
  }
}


/*
 *
 * CVS/RCS Log:
 * $Log: diqthitl.cc,v $
 * Revision 1.1  2002-01-25 13:32:11  meichel
 * Initial release of new color quantization classes and
 *   the dcmquant tool in module dcmimage.
 *
 *
 */
