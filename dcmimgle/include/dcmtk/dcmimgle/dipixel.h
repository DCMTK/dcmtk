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
 *  Purpose: DicomPixel (Header)
 *
 */


#ifndef DIPIXEL_H
#define DIPIXEL_H

#include "dcmtk/config/osconfig.h"

#include "dcmtk/dcmimgle/diutils.h"


/*---------------------*
 *  class declaration  *
 *---------------------*/

/** Abstract base class to handle pixel data
 */
class DCMTK_DCMIMGLE_EXPORT DiPixel
{

 public:

    /** constructor
     *
     ** @param  count       number of pixels stored in the buffer
     *  @param  inputCount  number of pixels read from 'PixelData' attribute
     */
    DiPixel(const unsigned long count,
            const unsigned long inputCount = 0)
      : Count(count),
        InputCount(inputCount)
    {
    }

    /** destructor
     */
    virtual ~DiPixel()
    {
    }

    /** get integer representation (abstract)
     *
     ** @return integer representation of the internally stored pixel data
     */
    virtual EP_Representation getRepresentation() const = 0;

    /** get number of planes (abstract).
     *
     ** @return number of planes (e.g. 1 for monochrome and 3 for color images)
     */
    virtual int getPlanes() const = 0;

    /** get pointer to pixel data (abstract).
     *  NB: See implemented method in derived class for details.
     *
     ** @return pointer to pixel data
     */
    virtual const void *getData() const = 0;

    /** get pointer to pixel data (abstract).
     *  NB: See implemented method in derived class for details.
     *
     ** @return pointer to pixel data
     */
    virtual void *getDataPtr() = 0;

    /** get pointer to array of pixel data (abstract).
     *  The number of planes in the returned array can be determined using 'getPlanes()'.
     *
     ** @return pointer to array of pixel data
     */
    virtual void *getDataArrayPtr() = 0;

    /** get number of pixels
     *
     ** @return number of pixels
     */
    inline unsigned long getCount() const
    {
        return Count;
    }

    /** get number of pixels stored in the 'PixelData' element
     *
     ** @return number pixels read from the input buffer
     */
    inline unsigned long getInputCount() const
    {
        return InputCount;
    }


 protected:

    /// number of pixels
    /*const*/ unsigned long Count;

    /// number of pixels in the input buffer
    unsigned long InputCount;
};


#endif
