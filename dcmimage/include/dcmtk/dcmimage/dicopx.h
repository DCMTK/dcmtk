/*
 *
 *  Copyright (C) 1996-2010, OFFIS e.V.
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
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:16:29 $
 *  CVS/RCS Revision: $Revision: 1.20 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
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
class DiColorPixel
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
     ** @return number of bytes allocated by the bitmap, or 0 if an error occured
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
     ** @return number of bytes allocated by the bitmap, or 0 if an error occured
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


/*
 *
 * CVS/RCS Log:
 * $Log: dicopx.h,v $
 * Revision 1.20  2010-10-14 13:16:29  joergr
 * Updated copyright header. Added reference to COPYRIGHT file.
 *
 * Revision 1.19  2010-03-01 09:08:46  uli
 * Removed some unnecessary include directives in the headers.
 *
 * Revision 1.18  2009-11-25 14:36:20  joergr
 * Added new source and header file for the module logger.
 *
 * Revision 1.17  2008-05-13 10:03:34  joergr
 * Fixed issue with multi-frame color images: writeImageToDataset() used wrong
 * format for color-by-plane output.
 *
 * Revision 1.16  2005/12/08 16:01:34  meichel
 * Changed include path schema for all DCMTK header files
 *
 * Revision 1.15  2004/10/19 12:57:47  joergr
 * Enhanced API documentation.
 *
 * Revision 1.14  2003/12/17 18:18:08  joergr
 * Removed leading underscore characters from preprocessor symbols (reserved
 * symbols).
 *
 * Revision 1.13  2002/09/12 14:10:37  joergr
 * Replaced "createPixelData" by "getPixelData" which uses a new dcmdata
 * routine and is therefore more efficient.
 *
 * Revision 1.12  2002/08/29 12:57:49  joergr
 * Added method that creates pixel data in DICOM format.
 *
 * Revision 1.11  2002/01/29 17:07:08  joergr
 * Added optional flag to the "Windows DIB" methods allowing to switch off the
 * scanline padding.
 *
 * Revision 1.10  2001/11/09 16:44:01  joergr
 * Enhanced and renamed createTrueColorDIB() method.
 * Updated/Enhanced comments.
 *
 * Revision 1.9  2001/06/01 15:49:29  meichel
 * Updated copyright header
 *
 * Revision 1.8  2000/03/08 16:21:51  meichel
 * Updated copyright header.
 *
 * Revision 1.7  1999/04/29 09:31:13  joergr
 * Moved color related image files back to non-public part.
 *
 * Revision 1.1  1999/04/28 14:57:32  joergr
 * Moved files from dcmimage module to dcmimgle to support new pastel color
 * output format.
 *
 * Revision 1.5  1999/01/20 14:44:04  joergr
 * Corrected some typos and formatting.
 *
 * Revision 1.4  1998/11/27 13:47:54  joergr
 * Added copyright message. Added method to directly create java AWT bitmaps.
 *
 * Revision 1.3  1998/05/11 14:53:13  joergr
 * Added CVS/RCS header to each file.
 *
 *
 */
