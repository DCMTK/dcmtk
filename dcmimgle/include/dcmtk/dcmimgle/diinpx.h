/*
 *
 *  Copyright (C) 1996-2011, OFFIS e.V.
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
 *  Module:  dcmimgle
 *
 *  Author:  Joerg Riesmeier
 *
 *  Purpose: DicomInputPixel (Header)
 *
 */


#ifndef DIINPX_H
#define DIINPX_H

#include "dcmtk/config/osconfig.h"

#include "dcmtk/dcmimgle/diutils.h"


/*---------------------*
 *  class declaration  *
 *---------------------*/

/** Abstract base class to convert DICOM pixel stream to intermediate representation
 */
class DCMTK_DCMIMGLE_EXPORT DiInputPixel
{

 public:

    /** constructor
     *
     ** @param  bits    number of bits stored for each pixel (depth)
     *  @param  first   first frame to be processed
     *  @param  number  number of frames to be processed
     *  @param  fsize   number of pixels per frame (frame size)
     */
    DiInputPixel(const unsigned int bits,
                 const unsigned long first,
                 const unsigned long number,
                 const unsigned long fsize);

    /** destructor
     */
    virtual ~DiInputPixel();

    /** determine minimum and maximum pixel value (abstract)
     *
     ** @return status, true if successful, false otherwise
     */
    virtual int determineMinMax() = 0;

    /** get pixel representation (abstract).
     *  Determine which integer type (size and signed/unsigned) is necessary to store
     *  the pixel data.
     *
     ** @return pixel representation
     */
    virtual EP_Representation getRepresentation() const = 0;

    /** get pointer to input pixel data (abstract)
     *
     ** @return pointer to input pixel data
     */
    virtual const void *getData() const = 0;

    /** get pointer to input pixel data (abstract)
     *
     ** @return pointer to input pixel data
     */
    virtual void *getDataPtr() = 0;

    /** remove reference to (internally handled) pixel data (abstract)
     */
    virtual void removeDataReference() = 0;

    /** get minimum pixel value (abstract)
     *
     ** @param  idx  specifies whether to return the global minimum (0) or
     *               the minimum of the selected pixel range (1, see PixelStart/Range)
     *
     ** @return minimum pixel value
     */
    virtual double getMinValue(const int idx) const = 0;

    /** get maximum pixel value (abstract)
     *
     ** @param  idx  specifies whether to return the global maximum (0) or
     *               the maximum of the selected pixel range (1, see PixelStart/Range)
     *
     ** @return maximum pixel value
     */
    virtual double getMaxValue(const int idx) const = 0;

    /** get number of bits per pixel
     *
     ** @return number of bits per pixel
     */
    inline unsigned int getBits() const
    {
        return Bits;
    }

    /** get absolute minimum pixel value
     *
     ** @return absolute minimum pixel value
     */
    inline double getAbsMinimum() const
    {
        return AbsMinimum;
    }

    /** get absolute maximum pixel value
     *
     ** @return absolute maximum pixel value
     */
    inline double getAbsMaximum() const
    {
        return AbsMaximum;
    }

    /** get absolute pixel value range
     *
     ** @return absolute pixel value range
     */
    inline double getAbsMaxRange() const
    {
        return AbsMaximum - AbsMinimum + 1;
    }

    /** get number of pixels stored
     *
     ** @return number of pixels stored
     */
    inline unsigned long getCount() const
    {
        return Count;
    }

    /** get first pixel to be processed
     *
     ** @return first pixel to be processed
     */
    inline unsigned long getPixelStart() const
    {
        return PixelStart;
    }

    /** get number of pixels to be processed
     *
     ** @return number of pixels to be processed
     */
    inline unsigned long getPixelCount() const
    {
        return PixelCount;
    }

    /** get number of pixels computed from the image resolution
     *
     ** @return number of pixels computed
     */
    inline unsigned long getComputedCount() const
    {
        return ComputedCount;
    }


 protected:

    /// number of pixels stored
    unsigned long Count;
    /// bits per pixel/sample
    unsigned int Bits;

    /// first frame to be processed
    unsigned long FirstFrame;
    /// number of frames to be processed
    unsigned long NumberOfFrames;
    /// number of pixels per frame
    unsigned long FrameSize;

    /// first pixel to be processed
    unsigned long PixelStart;
    /// number of pixels to be processed
    unsigned long PixelCount;

    /// number of pixels computed from the image resolution
    unsigned long ComputedCount;

    /// absolute minimum (possible) pixel value
    double AbsMinimum;
    /// absolute maximum (possible) pixel value
    double AbsMaximum;
};


#endif
