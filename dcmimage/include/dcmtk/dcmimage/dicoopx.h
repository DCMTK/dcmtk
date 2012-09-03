/*
 *
 *  Copyright (C) 1996-2012, OFFIS e.V.
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
 *  Purpose: DicomColorOutputPixel (Header)
 *
 */


#ifndef DICOOPX_H
#define DICOOPX_H

#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmimgle/diutils.h"
#include "dcmtk/ofstd/ofstream.h"

#define INCLUDE_CSTDLIB
#define INCLUDE_CSTDIO
#include "dcmtk/ofstd/ofstdinc.h"

#include "dcmtk/dcmimage/dicdefin.h"


/*------------------------*
 *  forward declarations  *
 *------------------------*/

class DiPixel;


/*---------------------*
 *  class declaration  *
 *---------------------*/

/** Abstract base class to create color output data
 */
class DCMTK_DCMIMAGE_EXPORT DiColorOutputPixel
{

 public:

    /** constructor
     *
     ** @param  pixel  pointer to intermediate pixel representation
     *  @param  size   number of pixel per frame
     *  @param  frame  frame to be rendered
     */
    DiColorOutputPixel(const DiPixel *pixel,
                       const unsigned long size,
                       const unsigned long frame);

    /** destructor
     */
    virtual ~DiColorOutputPixel();

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

    /** get pointer to outpout pixel data (abstract)
     *
     ** @return pointer to pixel data
     */
    virtual void *getDataPtr() = 0;

    /** get pointer to given plane of output pixel data (abstract)
     *
     ** @param  plane  number of the plane to be retrieved (0..2)
     *
     ** @return pointer to beginning of plane if successful, NULL otherwise
     */
    virtual const void *getPlane(const int plane) const = 0;

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


 protected:

    /// number of pixels per frame (intermediate representation)
    /*const*/ unsigned long Count;
    /// number of pixels per frame (memory buffer size)
    const unsigned long FrameSize;
};


#endif
