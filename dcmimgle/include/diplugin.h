/*
 *
 *  Copyright (C) 2001, OFFIS
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
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2001-11-27 18:25:04 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmimgle/include/Attic/diplugin.h,v $
 *  CVS/RCS Revision: $Revision: 1.1 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#ifndef __DIPLUGIN_H
#define __DIPLUGIN_H

#include "osconfig.h"

BEGIN_EXTERN_C
#include <stdio.h>
END_EXTERN_C


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
 * Revision 1.1  2001-11-27 18:25:04  joergr
 * Added support for plugable output formats in class DicomImage. First
 * implementation is JPEG.
 *
 *
 */
