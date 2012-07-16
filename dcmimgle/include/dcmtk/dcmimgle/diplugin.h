/*
 *
 *  Copyright (C) 2001-2011, OFFIS e.V.
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
 *  Purpose: Provides abstract interface to plugable image output formats
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
class DCMTK_DCMIMGLE_EXPORT DiPluginFormat
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
