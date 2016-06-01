/*
 *
 *  Copyright (C) 2002-2016, OFFIS e.V.
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
 *  Purpose: class DcmQuantColorTable
 *
 */


#ifndef DIQTCTAB_H
#define DIQTCTAB_H

#include "dcmtk/config/osconfig.h"
#include "dcmtk/ofstd/oftypes.h"      /* for OFBool */
#include "dcmtk/ofstd/ofcond.h"       /* for OFCondition */
#include "dcmtk/dcmimage/diqtpix.h"   /* for DcmQuantPixel */
#include "dcmtk/dcmimage/diqthash.h"  /* for DcmQuantHistogramItem */
#include "dcmtk/ofstd/ofstring.h"     /* for class OFString */


class DicomImage;
class DcmItem;


/** this class implements a color table that can either be
 *  a look-up table or an image color histogram.
 */
class DCMTK_DCMIMAGE_EXPORT DcmQuantColorTable
{
public:

  /// constructor
  DcmQuantColorTable();

  /// destructor
  ~DcmQuantColorTable();

  /// resets the object to default-constructed state
  void clear();

  /** returns the number of colors in the color table
   *  @return number of colors in color table
   */
  inline unsigned long getColors() const
  {
    return numColors;
  }

  /** creates a description string suitable for use as
   *  Derivation Description.
   *  @param str description string returned in this parameter
   */
  void setDescriptionString(OFString& str) const;

  /** creates a color table containing a histogram of the given
   *  image.  Pixel sample values are downsampled if necessary
   *  to make sure the histogram fits into the given size limit.
   *  @param image color image for which a histogram is computed
   *  @param maxcolors maximum number of colors allowed in histogram.
   *    If necessary, pixel sample values are down-sampled to enforce
   *    this maximum.
   *  @return EC_Normal if successful, an error code otherwise.
   */
  OFCondition computeHistogram(DicomImage& image, unsigned long maxcolors);

  /** after a call to computeHistogram(), this method
   *  returns the maximum pixel value to which all color samples
   *  were down-sampled during computation of the histogram.
   */
  inline unsigned long getMaxVal() const
  {
    return maxval;
  }

  /** returns the color at index idx.
   *  @param idx index, must be < getColors()
   *  @return const reference to color
   */
  inline const DcmQuantPixel& getPixel(unsigned long idx) const
  {
#ifdef DEBUG
    assert(array && idx < numColors);
#endif
    return *(array[idx]);
  }

  /** returns the red color component at index idx
   *  @param idx index, must be < getColors()
   *  @return red color component
   */
  inline DcmQuantComponent getRed(unsigned long idx) const
  {
#ifdef DEBUG
    assert(array && idx < numColors);
#endif
    return array[idx]->getRed();
  }

  /** returns the green color component at index idx
   *  @param idx index, must be < getColors()
   *  @return green color component
   */
  inline DcmQuantComponent getGreen(unsigned long idx) const
  {
#ifdef DEBUG
    assert(array && idx < numColors);
#endif
    return array[idx]->getGreen();
  }

  /** returns the blue color component at index idx
   *  @param idx index, must be < getColors()
   *  @return blue color component
   */
  inline DcmQuantComponent getBlue(unsigned long idx) const
  {
#ifdef DEBUG
    assert(array && idx < numColors);
#endif
    return array[idx]->getBlue();
  }

  /** computes a color LUT for the given image histogram.
   *  This median-cut colormap generator is based
   *  on Paul Heckbert's paper "Color Image Quantization for Frame Buffer
   *  Display", SIGGRAPH '82 Proceedings, page 297.
   *  @param histogram image color histogram
   *  @param sum number of pixels in image (colums * rows * frames)
   *  @param theMaxval maximum value to which pixels were
   *    downsampled for histogram computation
   *  @param numberOfColors desired number of colors in color LUT
   *  @param largeType algorithm used for determining the largest dimension
   *    in the Median Cut algorithm
   *  @param repType algorithm for choosing a representative color for each
   *  box in the Median Cut algorithm
   *  @return EC_Normal if successful, an error code otherwise.
   */
  OFCondition medianCut(
    DcmQuantColorTable& histogram,
    unsigned long sum,
    unsigned long theMaxval,
    unsigned long numberOfColors,
    DcmLargestDimensionType largeType,
    DcmRepresentativeColorType repType);

  /** determines for a given color the closest match in the color LUT.
   *  @param px color to look up in LUT
   *  @return index of closest match in LUT, -1 if look-up table empty
   */
  inline int computeIndex(const DcmQuantPixel& px) const
  {
    int result = -1;
    int r2, g2, b2;
    long newdist;
    int r1 = OFstatic_cast(int, px.getRed());
    int g1 = OFstatic_cast(int, px.getGreen());
    int b1 = OFstatic_cast(int, px.getBlue());
    long dist = 2000000000;
    for (unsigned long i = 0; i < numColors; ++i)
    {
        r2 = r1 - OFstatic_cast(int, array[i]->getRed());
        g2 = g1 - OFstatic_cast(int, array[i]->getGreen());
        b2 = b1 - OFstatic_cast(int, array[i]->getBlue());
        newdist = r2*r2 + g2*g2 + b2*b2;
        if (newdist < dist)
        {
            result = OFstatic_cast(int, i);
            dist = newdist;
            if (dist < array[i]->getValue()) i=numColors; // break out of for loop
        }
    }
    return result;
  }

  /** writes the current color table into a DICOM object, encoded as
   *  Red/Green/Blue Palette Color Lookup Table and Data.
   *  @param target DICOM dataset to write to
   *  @param writeAsOW if true, LUT data is encoded as OW, otherwise LUT data
   *    is encoded as US.  A LUT with 64k entries is always encoded as OW since
   *    a US data element with 64k entries cannot be written in explicit VR.
   *  @param write16BitEntries if true, LUT data is encoded with 16 bits per entry
   *  @return EC_Normal if successful, an error code otherwise.
   */
  OFCondition write(
    DcmItem& target,
    OFBool writeAsOW,
    OFBool write16BitEntries);


private:

  /** after a call to medianCut(), this method computes for each entry in
   *  the color map the minimum of the euclidean distances to any other
   *  of the entries.  Any color which has an euclidean distance of less
   *  than half of this distance is necessarily mapped to this entry.
   *  This data is used by computeIndex()
   */
  void computeClusters();

  /// private undefined copy constructor
  DcmQuantColorTable(const DcmQuantColorTable& src);

  /// private undefined copy assignment operator
  DcmQuantColorTable& operator=(const DcmQuantColorTable& src);

  /// color table data
  DcmQuantHistogramItemPointer *array;

  /// number of entries in color table
  unsigned long numColors;

  /** maximum pixel value to which all color samples
   *  were down-sampled during computation of the histogram.
   */
  unsigned long maxval;

};

#endif
