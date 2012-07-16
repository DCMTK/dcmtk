/*
 *
 *  Copyright (C) 2002-2010, OFFIS e.V.
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
 *  Module:  dcmimage
 *
 *  Author:  Marco Eichelberg
 *
 *  Purpose: class DcmQuantHistogramItemList
 *
 */

#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmimage/diqthitl.h"


DcmQuantHistogramItemList::DcmQuantHistogramItemList()
: list_()
, first(list_.end())
, last(list_.end())
{
}


DcmQuantHistogramItemList::~DcmQuantHistogramItemList()
{
  first = list_.begin();
  while (first != last)
  {
    delete *first;
    first = list_.erase(first);
  }
}


void DcmQuantHistogramItemList::moveto(DcmQuantHistogramItemPointer *array, unsigned long& counter, unsigned long numcolors)
{
  first = list_.begin();
  while ((first != last) && (counter < numcolors))
  {
    array[counter++] = *first;
    first = list_.erase(first);
  }
}
