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
 *  Purpose: DicomColorOutputPixel (Header)
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:16:29 $
 *  CVS/RCS Revision: $Revision: 1.21 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
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


/*------------------------*
 *  forward declarations  *
 *------------------------*/

class DiPixel;


/*---------------------*
 *  class declaration  *
 *---------------------*/

/** Abstract base class to create color output data
 */
class DiColorOutputPixel
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
     ** @return pointer to beginning of plane if sucessful, NULL otherwise
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


/*
 *
 * CVS/RCS Log:
 * $Log: dicoopx.h,v $
 * Revision 1.21  2010-10-14 13:16:29  joergr
 * Updated copyright header. Added reference to COPYRIGHT file.
 *
 * Revision 1.20  2006/08/15 16:35:01  meichel
 * Updated the code in module dcmimage to correctly compile when
 *   all standard C++ classes remain in namespace std.
 *
 * Revision 1.19  2005/12/08 16:01:32  meichel
 * Changed include path schema for all DCMTK header files
 *
 * Revision 1.18  2004/02/06 11:18:18  joergr
 * Distinguish more clearly between const and non-const access to pixel data.
 *
 * Revision 1.17  2003/12/23 11:23:30  joergr
 * Added missing API documentation.
 *
 * Revision 1.16  2003/12/17 18:13:41  joergr
 * Removed leading underscore characters from preprocessor symbols (reserved
 * symbols).
 *
 * Revision 1.15  2002/11/27 14:16:56  meichel
 * Adapted module dcmimage to use of new header file ofstdinc.h
 *
 * Revision 1.14  2002/04/16 13:54:42  joergr
 * Added configurable support for C++ ANSI standard includes (e.g. streams).
 *
 * Revision 1.13  2001/06/01 15:49:28  meichel
 * Updated copyright header
 *
 * Revision 1.12  2000/03/08 16:21:50  meichel
 * Updated copyright header.
 *
 * Revision 1.11  2000/02/23 15:09:39  meichel
 * Corrected macro for Borland C++ Builder 4 workaround.
 *
 * Revision 1.10  2000/02/01 11:02:19  meichel
 * Avoiding to include <stdlib.h> as extern "C" on Borland C++ Builder 4,
 *   workaround for bug in compiler header files.
 *
 * Revision 1.9  1999/07/23 13:20:44  joergr
 * Enhanced handling of corrupted pixel data (wrong length).
 *
 * Revision 1.8  1999/04/29 09:31:12  joergr
 * Moved color related image files back to non-public part.
 *
 * Revision 1.1  1999/04/28 14:57:31  joergr
 * Moved files from dcmimage module to dcmimgle to support new pastel color
 * output format.
 *
 * Revision 1.6  1999/02/03 16:53:42  joergr
 * Added BEGIN_EXTERN_C and END_EXTERN_C to some C includes.
 *
 * Revision 1.5  1999/01/20 14:42:06  joergr
 * Replaced invocation of getCount() by member variable Count where possible.
 *
 * Revision 1.4  1998/11/27 13:44:29  joergr
 * Added copyright message.
 *
 * Revision 1.3  1998/05/11 14:53:12  joergr
 * Added CVS/RCS header to each file.
 *
 *
 */
