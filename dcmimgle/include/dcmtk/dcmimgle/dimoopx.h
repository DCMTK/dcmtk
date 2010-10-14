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
 *  Module:  dcmimgle
 *
 *  Author:  Joerg Riesmeier
 *
 *  Purpose: DicomMonoOutputPixel (Header)
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:16:26 $
 *  CVS/RCS Revision: $Revision: 1.21 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
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
class DiMonoOutputPixel
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


/*
 *
 * CVS/RCS Log:
 * $Log: dimoopx.h,v $
 * Revision 1.21  2010-10-14 13:16:26  joergr
 * Updated copyright header. Added reference to COPYRIGHT file.
 *
 * Revision 1.20  2010-03-01 09:08:47  uli
 * Removed some unnecessary include directives in the headers.
 *
 * Revision 1.19  2006-08-15 16:30:11  meichel
 * Updated the code in module dcmimgle to correctly compile when
 *   all standard C++ classes remain in namespace std.
 *
 * Revision 1.18  2005/12/08 16:47:53  meichel
 * Changed include path schema for all DCMTK header files
 *
 * Revision 1.17  2004/02/06 11:07:50  joergr
 * Distinguish more clearly between const and non-const access to pixel data.
 *
 * Revision 1.16  2004/01/05 14:52:20  joergr
 * Removed acknowledgements with e-mail addresses from CVS log.
 *
 * Revision 1.15  2003/12/08 18:44:05  joergr
 * Removed leading underscore characters from preprocessor symbols (reserved
 * symbols). Updated copyright header.
 *
 * Revision 1.14  2002/11/27 14:08:05  meichel
 * Adapted module dcmimgle to use of new header file ofstdinc.h
 *
 * Revision 1.13  2002/04/16 13:53:11  joergr
 * Added configurable support for C++ ANSI standard includes (e.g. streams).
 *
 * Revision 1.12  2001/06/01 15:49:46  meichel
 * Updated copyright header
 *
 * Revision 1.11  2000/03/08 16:24:19  meichel
 * Updated copyright header.
 *
 * Revision 1.10  2000/02/23 15:12:15  meichel
 * Corrected macro for Borland C++ Builder 4 workaround.
 *
 * Revision 1.9  2000/02/01 10:52:37  meichel
 * Avoiding to include <stdlib.h> as extern "C" on Borland C++ Builder 4,
 *   workaround for bug in compiler header files.
 *
 * Revision 1.8  1999/10/06 13:41:21  joergr
 * Added method to renmoved reference to (internally handles) pixel data.
 *
 * Revision 1.7  1999/09/17 12:26:52  joergr
 * Added/changed/completed DOC++ style comments in the header files.
 *
 * Revision 1.6  1999/07/23 14:05:55  joergr
 * Enhanced handling of corrupted pixel data (wrong length).
 *
 * Revision 1.5  1999/03/24 17:20:13  joergr
 * Added/Modified comments and formatting.
 *
 * Revision 1.4  1999/02/11 16:37:56  joergr
 * Added routine to check whether particular grayscale values are unused in
 * the output data.
 *
 * Revision 1.3  1999/02/03 17:30:30  joergr
 * Added BEGIN_EXTERN_C and END_EXTERN_C to some C includes.
 *
 * Revision 1.2  1999/01/20 15:07:02  joergr
 * Replaced invocation of getCount() by member variable Count where possible.
 *
 * Revision 1.1  1998/11/27 15:25:48  joergr
 * Added copyright message.
 *
 * Revision 1.3  1998/05/11 14:53:22  joergr
 * Added CVS/RCS header to each file.
 *
 *
 */
