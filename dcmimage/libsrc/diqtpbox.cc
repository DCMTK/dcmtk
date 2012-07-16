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
 *  Purpose: class DcmQuantPixelBoxArray
 *
 */


#include "dcmtk/config/osconfig.h"
#include "dcmtk/ofstd/ofcast.h"
#include "dcmtk/dcmimage/diqtpbox.h"   /* for DcmQuantPixelBoxArray */

#ifdef USE_STD_CXX_INCLUDES
// Solaris defines qsort() in namespace std, other compilers don't...
namespace std { }
using namespace std;
#endif


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
