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
 *  Purpose: class DcmQuantScaleTable
 *
 */


#ifndef DIQTSTAB_H
#define DIQTSTAB_H


#include "dcmtk/config/osconfig.h"
#include "dcmtk/ofstd/ofcast.h"
#include "dcmtk/dcmimage/diqttype.h"  /* for DcmQuantComponent */

#define INCLUDE_CSTDLIB
#define INCLUDE_CASSERT
#include "dcmtk/ofstd/ofstdinc.h"


/** this is a helper class used for temporarily reducing the image bit depth
 *  during calculation of an image histogram.  An object of this class
 *  contains a look-up table that maps image pixel values to scaled-down
 *  image pixel values.
 */
class DCMTK_DCMIMAGE_EXPORT DcmQuantScaleTable
{
public:

  /// constructor
  DcmQuantScaleTable()
  : table(NULL)
  , numEntries(0)
  {
  }

  /// destructor
  ~DcmQuantScaleTable()
  {
    cleanup();
  }

  /** array look-up operation.
   *  @param idx pixel value to look up.  Object must be initialized
   *    (i.e. createTable() must have been called before) and idx must
   *    be less or equal to the parameter oldmaxval passed to createTable().
   *  @return scaled-down value from look-up table.
   */
  inline DcmQuantComponent operator[](unsigned int idx) const
  {
#ifdef DEBUG
    assert(idx < numEntries);
#endif
    return table[idx];
  }

  /** initializes the look-up table.
   *  @param oldmaxval maximum possible pixel value in source image
   *  @param newmaxval new desired maximum value, should be < oldmaxval
   */
  void createTable(
    unsigned long oldmaxval,
    unsigned long newmaxval)
  {
    cleanup();

    table = new DcmQuantComponent[oldmaxval+1];
    if (table)
    {
      numEntries = OFstatic_cast(unsigned int, oldmaxval) + 1;
      for (unsigned int i=0; i < numEntries; i++)
        table[i] = OFstatic_cast(DcmQuantComponent, (OFstatic_cast(unsigned long, i) * newmaxval + oldmaxval/2) / oldmaxval);
    }
  }

private:

  /// resets the object to the default-constructed state
  inline void cleanup()
  {
    delete[] table;
    table = NULL;
    numEntries = 0;
  }

  /// private undefined copy constructor
  DcmQuantScaleTable(const DcmQuantScaleTable& src);

  /// private undefined copy assignment operator
  DcmQuantScaleTable& operator=(const DcmQuantScaleTable& src);

  /// array of pixel values
  DcmQuantComponent *table;

  /// number of entries in array
  unsigned int numEntries;
};


#endif
