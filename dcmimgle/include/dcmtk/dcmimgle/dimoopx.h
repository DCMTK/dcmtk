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
 *  Purpose: DicomMonoOutputPixel (Header)
 *
 */


#ifndef DIMOOPX_H
#define DIMOOPX_H

#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmimgle/diutils.h"

#include "dcmtk/ofstd/ofstream.h"


/*------------------------*
 *  forward declarations  *
 *------------------------*/

class DiMonoPixel;


/*---------------------*
 *  class declaration  *
 *---------------------*/

/** Abstract base class to create monochrome output data
 */
class DCMTK_DCMIMGLE_EXPORT DiMonoOutputPixel
{

 public:

    /** constructor
     *
     ** @param  pixel  pointer to intermediate pixel representation
     *  @param  size   number of pixel per frame
     *  @param  frame  frame to be rendered
     *  @param  max    maximum output value
     */
    DiMonoOutputPixel(const DiMonoPixel *pixel,
                      const unsigned long size,
                      const unsigned long frame,
                      const unsigned long max);

    /** destructor
     */
    virtual ~DiMonoOutputPixel();

    /** get integer representation (abstract)
     *
     ** @return integer representation
     */
    virtual EP_Representation getRepresentation() const = 0;

    /** get pointer to output pixel data (abstract)
     *
     ** @return pointer to pixel data
     */
    virtual const void *getData() const = 0;

    /** get pointer to output pixel data (abstract)
     *
     ** @return pointer to pixel data
     */
    virtual void *getDataPtr() = 0;

    /** remove reference to (internally handled) pixel data (abstract)
     */
    virtual void removeDataReference() = 0;

    /** get size of one pixel / item in the pixel array (abstract)
     *
     ** @return item size
     */
    virtual size_t getItemSize() const = 0;

    /** write pixel data of selected frame to PPM/ASCII file (abstract)
     *
     ** @param  stream  open C++ output stream
     *
     ** @return status, true if successful, false otherwise
     */
    virtual int writePPM(STD_NAMESPACE ostream& stream) const = 0;

    /** write pixel data of selected frame to PPM/ASCII file (abstract)
     *
     ** @param  stream  open C file stream
     *
     ** @return status, true if successful, false otherwise
     */
    virtual int writePPM(FILE *stream) const = 0;

    /** get number of pixel per frame
     *
     ** @return number of pixel per frame
     */
    inline unsigned long getCount() const
    {
        return FrameSize;
    }

    /** check whether specified pixel value is used in the image.
     *  Pixel array is examined when this method is called for the first time
     *  (just-in-time creation / create on demand of the supporting table).
     *
     ** @param  value  pixel value to be checked
     *
     ** @return true if value is used, false otherwise
     */
    int isUnused(const unsigned long value);


 protected:

    /** examine which pixel values are actually used (abstract)
     */
    virtual void determineUsedValues() = 0;


    /// number of pixels per frame (intermediate representation)
    /*const*/ unsigned long Count;
    /// number of pixels per frame (memory buffer size)
    const unsigned long FrameSize;

    /// array of used pixel values
    Uint8 *UsedValues;
    /// maximum output value
    const unsigned long MaxValue;


 private:

 // --- declarations to avoid compiler warnings

    DiMonoOutputPixel(const DiMonoOutputPixel &);
    DiMonoOutputPixel &operator=(const DiMonoOutputPixel &);
};


#endif
