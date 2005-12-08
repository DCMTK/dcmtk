/*
 *
 *  Copyright (C) 2001-2005, OFFIS
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
 *  Purpose: Provides abstract interface to plugable image output formats
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 2005-12-08 16:48:05 $
 *  CVS/RCS Revision: $Revision: 1.4 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#ifndef DIPLUGIN_H
#define DIPLUGIN_H

#include "dcmtk/config/osconfig.h"

#define INCLUDE_CSTDIO
#include "dcmtk/ofstd/ofstdinc.h"


/*------------------------*
 *  forward declarations  *
 *------------------------*/

class DiImage;


/*---------------------*
 *  class declaration  *
 *---------------------*/

/** abstract interface to plugable image output formats.
 *  This is an abstract base class used as an interface to support multiple
 *  plugable image output formats for the dcmimle/dcmimage library. An example
 *  implementation can be found in dcmjpeg/libsrc/dipijpeg.cc (JPEG plugin).
 */
class DiPluginFormat
{

  public:

    /** destructor (virtual)
     */
    virtual ~DiPluginFormat() {}

    /** write given image to a file stream (abstract)
     *
     ** @param  image   pointer to DICOM image object to be written
     *  @param  stream  stream to which the image is written (open in binary mode!)
     *  @param  frame   index of frame used for output (default: first frame = 0)
     *
     ** @return true if successful, false otherwise
     */
    virtual int write(DiImage *image,
                      FILE *stream,
                      const unsigned long frame = 0) const = 0;

  protected:

    /** constructor (protected)
     */
    DiPluginFormat() {}
};


#endif


/*
 *
 * CVS/RCS Log:
 * $Log: diplugin.h,v $
 * Revision 1.4  2005-12-08 16:48:05  meichel
 * Changed include path schema for all DCMTK header files
 *
 * Revision 1.3  2003/12/08 19:29:29  joergr
 * Removed leading underscore characters from preprocessor symbols (reserved
 * symbols). Updated copyright header.
 *
 * Revision 1.2  2002/11/27 14:08:07  meichel
 * Adapted module dcmimgle to use of new header file ofstdinc.h
 *
 * Revision 1.1  2001/11/27 18:25:04  joergr
 * Added support for plugable output formats in class DicomImage. First
 * implementation is JPEG.
 *
 *
 */
