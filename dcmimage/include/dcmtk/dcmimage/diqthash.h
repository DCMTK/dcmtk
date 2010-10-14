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
 *  Purpose: class DcmQuantColorHashTable
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:16:29 $
 *  CVS/RCS Revision: $Revision: 1.4 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#ifndef DIQTHASH_H
#define DIQTHASH_H


#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmimage/diqtpix.h"   /* for DcmQuantPixel */
#include "dcmtk/dcmimage/diqthitl.h"  /* for DcmQuantHistogramItemList */


class DicomImage;


/** this class implements a hash table for colors.  
 *  Each entry of the hash table consists of an RGB 
 *  color (DcmQuantPixel object) and an integer value (e. g. counter).
 *  This class is used during the quantization of a color image.
 */ 
class DcmQuantColorHashTable
{
public: 
  /// constructor
  DcmQuantColorHashTable(); 

  /// destructor
  ~DcmQuantColorHashTable(); 

  /** adds a new color to the hash table.  The color must not yet
   *  be present (the caller is responsible for checking this).
   *  @param colorP color to be added to table
   *  @param value integer value (counter) associated to color
   */
  inline void add(const DcmQuantPixel& colorP, int value)
  {
    table[colorP.hash()]->push_front(colorP, value);
  }

  /** looks up the given color in the hash table.
   *  If found, the value associated to the color is returned, -1 otherwise.
   *  @param colorP color to look up in hash table
   *  @return value associated to given color if found, -1 otherwise.
   */
  inline int lookup(const DcmQuantPixel& colorP) const
  {
    return table[colorP.hash()]->lookup(colorP);  
  }

  /** adds all pixels of all frames of the given image (which must be a 
   *  color image) to the hash table.  The counter (integer value associated 
   *  to each color) counts the occurence of the color in the image.
   *  If more than maxcolors colors are found, the function returns zero.
   *  @param image image in which colors are to be counted
   *  @param newmaxval maximum pixel value to which the contents of the 
   *    image are scaled down (see documentation of class DcmQuantScaleTable)
   *    before counting colors.  This is used by the caller to reduce the number 
   *    of colors in the image if necessary.
   *  @param maxcolors maximum number of colors allowed.  If more colors are found,
   *    the method immediately returns with a return value of zero.
   *  @return number of colors found, 0 if too many colors.
   */
  unsigned long addToHashTable(
    DicomImage& image, 
    unsigned long newmaxval,
    unsigned long maxcolors);

  /** counts the number of entries in the hash table
   *  @return number of entries in hash table
   */
  unsigned long countEntries() const;

  /** converts the contents of this hash table into a histogram array.
   *  After a successful call to addToHashTable(), this object contains
   *  an image histogram stored in a hash table.  This method creates
   *  a new array of DcmQuantHistogramItem pointers on the heap
   *  and moves the contents of the hash table into this array.
   *  @param array the hash table array is returned in this parameter
   *  @return number of elements in array
   */
  unsigned long createHistogram(DcmQuantHistogramItemPointer *& array);

private:

  /// private undefined copy constructor
  DcmQuantColorHashTable(const DcmQuantColorHashTable& src);

  /// private undefined copy assignment operator
  DcmQuantColorHashTable& operator=(const DcmQuantColorHashTable& src);

  /// hash array of lists for color/value pairs
  DcmQuantHistogramItemListPointer *table;
};


#endif


/*
 * CVS/RCS Log:
 * $Log: diqthash.h,v $
 * Revision 1.4  2010-10-14 13:16:29  joergr
 * Updated copyright header. Added reference to COPYRIGHT file.
 *
 * Revision 1.3  2005/12/08 16:01:47  meichel
 * Changed include path schema for all DCMTK header files
 *
 * Revision 1.2  2003/12/23 12:17:26  joergr
 * Updated copyright header.
 *
 * Revision 1.1  2002/01/25 13:32:05  meichel
 * Initial release of new color quantization classes and
 *   the dcmquant tool in module dcmimage.
 *
 *
 */
