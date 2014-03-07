/*
 *
 *  Copyright (C) 2002-2014, OFFIS e.V.
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


#ifndef DIQTPBOX_H
#define DIQTPBOX_H


#include "dcmtk/config/osconfig.h"

#define INCLUDE_CSTDLIB
#define INCLUDE_CASSERT
#include "dcmtk/ofstd/ofstdinc.h"

#include "dcmtk/dcmimage/dicdefin.h"

BEGIN_EXTERN_C
#ifdef HAVE_SYS_TYPES_H
/* needed e.g. on Solaris for definition of size_t */
#include <sys/types.h>
#endif
END_EXTERN_C

/** helper structure for class DcmQuantPixelBoxArray.
 *  Each object of this class represents a pixel box used in the
 *  Median Cut algorithm.
 */
struct DCMTK_DCMIMAGE_EXPORT DcmQuantPixelBox
{
  /// index of the color histogram array where the colors represented by this box start
  int ind;

  /// the number of colors represented by this box
  int colors;

  /// the number of pixels represented by this box
  unsigned long sum;
};


/// typedef for a pointer to a DcmQuantPixelBox object
typedef DcmQuantPixelBox *DcmQuantPixelBoxPointer;


/** helper class for the Median Cut algorithm.  An object of
 *  this class maintains an array of pixel boxes which is used
 *  to determine a color look-up table of given size for a
 *  true color image.
 */
class DCMTK_DCMIMAGE_EXPORT DcmQuantPixelBoxArray
{
public:

  /** constructor. Creates an array with the given number of pixel boxes.
   *  @param entries number of entries in array, must be > 0.
   */
  DcmQuantPixelBoxArray(unsigned long entries);

  /// destructor
  ~DcmQuantPixelBoxArray();

  /** looks up an array element
   *  @param idx index, must be smaller than the number of entries
   *    with which this object was initialized
   *  @return non-const reference to DcmQuantPixelBox struct
   */
  inline DcmQuantPixelBox& operator[](size_t idx)
  {
#ifdef DEBUG
    assert(idx < length);
#endif
    return *(array[idx]);
  }

  /** sorts the given number of pixel boxes in the array,
   *  starting with the first array element.
   *  The boxes are sorted in decreasing order of the "sum" member.
   *  @param boxes number of boxes to sort, must be <= entries.
   */
  void sort(unsigned long boxes);

private:

  /// private undefined copy constructor
  DcmQuantPixelBoxArray(const DcmQuantPixelBoxArray& src);

  /// private undefined copy assignment operator
  DcmQuantPixelBoxArray& operator=(const DcmQuantPixelBoxArray& src);

  /// array of pixel boxes
  DcmQuantPixelBoxPointer *array;

  /// number of elements in array
  unsigned long length;
};


#endif
