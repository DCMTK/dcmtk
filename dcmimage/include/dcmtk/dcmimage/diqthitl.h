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
 *  Purpose: class DcmQuantHistogramItemList
 *
 */


#ifndef DIQTHITL_H
#define DIQTHITL_H


#include "dcmtk/config/osconfig.h"
#include "dcmtk/ofstd/oflist.h"       /* for OFList */
#include "dcmtk/dcmimage/diqthitm.h"  /* for DcmQuantHistogramItem */


/** this is a helper class used by class DcmQuantColorHashTable.
 *  It maintains a list of DcmQuantHistogramItem objects.
 */
class DCMTK_DCMIMAGE_EXPORT DcmQuantHistogramItemList
{
public:

  /// constructor
  DcmQuantHistogramItemList();

  /// destructor. Destroys all objects pointed to by list.
  ~DcmQuantHistogramItemList();

  /** this method moves the contents of this list into the given array.
   *  The list becomes empty if the array is large enough to contain all list members.
   *  @param array array of pointers to DcmQuantHistogramItem
   *  @param counter When called, contains the index of the array element
   *    into which the first member of the list will be moved.  Must be < numcolors.
   *    Upon return, contains the array index of the last element moved + 1.
   *  @param numcolors number of elements in array
   */
  void moveto(DcmQuantHistogramItemPointer *array, unsigned long& counter, unsigned long numcolors);

  /** searches the list for an entry that equals the given pixel value.
   *  If found, the integer value assigned to that pixel is returned, otherwise returns -1.
   *  @param colorP pixel to lookup in list
   *  @return integer value for given color if found, -1 otherwise.
   */
  inline int lookup(const DcmQuantPixel& colorP)
  {
    first = list_.begin();
    while (first != last)
    {
      if ((*first)->equals(colorP)) return (*first)->getValue();
      ++first;
    }
    return -1;
  }

  /** adds the given pixel to the list.  If the pixel is already
   *  contained in the list, it's integer value (counter) is increased
   *  and 0 is returned.  Otherwise, a new entry with a counter of 1
   *  is created and 1 is returned.
   *  @param colorP pixel to add to the list
   *  @return 0 if pixel was already in list, 1 otherwise.
   */
  inline unsigned long add(const DcmQuantPixel& colorP)
  {
    first = list_.begin();
    while (first != last)
    {
      if ((*first)->equals(colorP))
      {
        (*first)->incValue();
        return 0;
      }
      ++first;
    }

    // not found in list, create new entry
    list_.push_front(new DcmQuantHistogramItem(colorP, 1));
    return 1;
  }

  /** inserts a new DcmQuantHistogramItem at the beginning of the list.
   *  @param colorP pixel value assigned to the new object in the list
   *  @param value integer value assigned to the new object in the list
   */
  inline void push_front(const DcmQuantPixel& colorP, int value)
  {
    list_.push_front(new DcmQuantHistogramItem(colorP, value));
  }

  /// returns current number of objects in the list
  inline size_t size() const
  {
    return list_.size();
  }

private:

  /// list of (pointers to) DcmQuantHistogramItem objects
  OFList<DcmQuantHistogramItem *> list_;

  /// temporary iterator used in various methods; declared here for efficiency reasons only.
  OFListIterator(DcmQuantHistogramItem *) first;

  /// constant iterator which always contains list_.end(); declared here for efficiency reasons only.
  OFListIterator(DcmQuantHistogramItem *) last;

};


/// typedef for a pointer to a DcmQuantHistogramItemList object
typedef DcmQuantHistogramItemList *DcmQuantHistogramItemListPointer;


#endif
