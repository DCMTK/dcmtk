/*
 *
 *  Copyright (C) 1996-2002, OFFIS
 *
 *  This software and supporting documentation were developed by
 *
 *    Kuratorium OFFIS e.V.
 *    Healthcare Information and Communication Systems
 *    Escherweg 2
 *    D-26121 Oldenburg, Germany
 *
 *  THIS SOFTWARE IS MADE AVAILABLE,  AS IS,  AND OFFIS MAKES NO  WARRANTY
 *  REGARDING  THE  SOFTWARE,  ITS  PERFORMANCE,  ITS  MERCHANTABILITY  OR
 *  FITNESS FOR ANY PARTICULAR USE, FREEDOM FROM ANY COMPUTER DISEASES  OR
 *  ITS CONFORMITY TO ANY SPECIFICATION. THE ENTIRE RISK AS TO QUALITY AND
 *  PERFORMANCE OF THE SOFTWARE IS WITH THE USER.
 *
 *  Module:  dcmimgle
 *
 *  Author:  Joerg Riesmeier
 *
 *  Purpose: DicomMonoOutputPixel (Header)
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 2002-11-27 14:08:05 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmimgle/include/Attic/dimoopx.h,v $
 *  CVS/RCS Revision: $Revision: 1.14 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#ifndef __DIMOOPX_H
#define __DIMOOPX_H

#include "osconfig.h"
#include "diutils.h"

#define INCLUDE_CSTDLIB
#define INCLUDE_CSTDIO
#include "ofstdinc.h"

#include "ofstream.h"

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
     ** @return pointer to pixel data if sucessful, NULL otherwise
     */
    virtual void *getData() const = 0;

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
    virtual int writePPM(ostream &stream) const = 0;

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
 * Revision 1.14  2002-11-27 14:08:05  meichel
 * Adapted module dcmimgle to use of new header file ofstdinc.h
 *
 * Revision 1.13  2002/04/16 13:53:11  joergr
 * Added configurable support for C++ ANSI standard includes (e.g. streams).
 * Thanks to Andreas Barth <Andreas.Barth@bruker-biospin.de> for his
 * contribution.
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
