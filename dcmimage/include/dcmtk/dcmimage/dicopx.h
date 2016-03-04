/*
 *
 *  Copyright (C) 1996-2016, OFFIS e.V.
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
 *  Author:  Joerg Riesmeier
 *
 *  Purpose: DicomColorPixel (Header)
 *
 */


#ifndef DICOPX_H
#define DICOPX_H

#include "dcmtk/config/osconfig.h"

#include "dcmtk/dcmimage/dilogger.h"
#include "dcmtk/dcmimgle/dipixel.h"


/*------------------------*
 *  forward declarations  *
 *------------------------*/

class DiDocument;
class DiInputPixel;
class DiMonoPixel;
class DiColorPixel;


/*---------------------*
 *  class declaration  *
 *---------------------*/


/** Abstract base class to handle color pixel data
 */
class DCMTK_DCMIMAGE_EXPORT DiColorPixel
  : public DiPixel
{

 public:

    /** constructor
     *
     ** @param  docu         pointer to the DICOM document
     *  @param  pixel        pointer to input pixel data
     *  @param  samples      number of expected samples per pixel (for checking purposes)
     *  @param  status       status of the image object (reference variable)
     *  @param  sample_rate  dummy parameter (used for derived classes only)
     */
    DiColorPixel(const DiDocument *docu,
                 const DiInputPixel *pixel,
                 const Uint16 samples,
                 EI_Status &status,
                 const Uint16 sample_rate = 0);

    /** destructor
     */
    virtual ~DiColorPixel();

    /** get number of planes
     *
     ** @return number of planes (here 3, color)
     */
    inline int getPlanes() const
    {
        return 3;
    }

    /** get planar configuration of the original pixel data
     *
     ** @return planar configuration (0 = color-by-pixel, 1 = color-by-plane)
     */
    inline int getPlanarConfiguration() const
    {
        return PlanarConfiguration;
    }

    /** fill given memory block with pixel data (all three image planes, RGB)
     *
     ** @param  data    pointer to memory block (array of 8 or 16 bit values, OB/OW)
     *  @param  count   number of T-size entries allocated in the 'data' array
     *  @param  fcount  number of pixels per frame
     *  @param  frames  total number of frames present in intermediate representation
     *  @param  planar  flag indicating whether data shall be stored color-by-pixel or color-by-plane
     *
     ** @return OFTrue if successful, OFFalse otherwise
     */
    virtual OFBool getPixelData(void *data,
                                const unsigned long count,
                                const unsigned long fcount,
                                const unsigned long frames,
                                const int planar) const = 0;

    /** create true color (24/32 bit) bitmap for MS Windows.
     *
     ** @param  data        untyped pointer memory buffer (set to NULL if not allocated externally)
     *  @param  size        size of the memory buffer in bytes (if 0 'data' is set to NULL)
     *  @param  width       number of columns of the image
     *  @param  height      number of rows of the image
     *  @param  frame       index of frame to be converted (starting from 0)
     *  @param  fromBits    number of bits per sample used for internal representation of the image
     *  @param  toBits      number of bits per sample used for the output bitmap (<= 8)
     *  @param  mode        color output mode (24 or 32 bits, see dcmimgle/dcmimage.h for details)
     *  @param  upsideDown  specifies the order of lines in the images (0 = top-down, bottom-up otherwise)
     *  @param  padding     align each line to a 32-bit address if true
     *
     ** @return number of bytes allocated by the bitmap, or 0 if an error occurred
     */
    virtual unsigned long createDIB(void *&data,
                                    const unsigned long size,
                                    const Uint16 width,
                                    const Uint16 height,
                                    const unsigned long frame,
                                    const int fromBits,
                                    const int toBits,
                                    const int mode,
                                    const int upsideDown,
                                    const int padding) const = 0;

    /** create true color (32 bit) bitmap for Java (AWT default format).
     *
     ** @param  data      resulting pointer to bitmap data (set to NULL if an error occurred)
     *  @param  width     number of columns of the image
     *  @param  height    number of rows of the image
     *  @param  frame     index of frame to be converted (starting from 0)
     *  @param  fromBits  number of bits per sample used for internal representation of the image
     *  @param  toBits    number of bits per sample used for the output bitmap (<= 8)
     *
     ** @return number of bytes allocated by the bitmap, or 0 if an error occurred
     */
    virtual unsigned long createAWTBitmap(void *&data,
                                          const Uint16 width,
                                          const Uint16 height,
                                          const unsigned long frame,
                                          const int fromBits,
                                          const int toBits) const = 0;


 protected:

    /** constructor
     *
     ** @param  pixel  pointer to intermediate color pixel data
     *  @param  count  number of pixels
     */
    DiColorPixel(const DiColorPixel *pixel,
                 const unsigned long count);

    /// planar configuration of the original pixel data (0 = color-by-pixel, 1 = color-by-plane)
    int PlanarConfiguration;
};


#endif
