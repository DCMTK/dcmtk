/*
 *
 *  Copyright (C) 2002-2021, OFFIS e.V.
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
 *  Purpose: class DcmQuantPixelBoxArray
 *
 */


#include "dcmtk/config/osconfig.h"
#include "dcmtk/ofstd/ofcast.h"
#include "dcmtk/dcmimage/diqtpbox.h"   /* for DcmQuantPixelBoxArray */
#include "dcmtk/ofstd/ofstdinc.h"

// Solaris defines qsort() in namespace std, other compilers don't...
using STD_NAMESPACE qsort;

BEGIN_EXTERN_C
static int sumcompare(const void *x1, const void *x2)
{
  return (*(DcmQuantPixelBoxPointer *) x2)->sum
       - (*(DcmQuantPixelBoxPointer *) x1)->sum;
}
END_EXTERN_C


void DcmQuantPixelBoxArray::sort(unsigned long boxes)
{
#ifdef DEBUG
  assert(boxes <= length);
#endif
  qsort(OFreinterpret_cast(char *, array), OFstatic_cast(unsigned int, boxes),  sizeof(DcmQuantPixelBoxPointer), sumcompare);
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
