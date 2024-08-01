/*
 *
 *  Copyright (C) 2024, OFFIS e.V.
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
 *  Module:  dcmjpls
 *
 *  Author:  Marco Eichelberg
 *
 *  Purpose: JPEG-LS interface for pluggable image formats
 *
 */

#ifndef DIPIJPLS_H
#define DIPIJPLS_H

#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmimgle/diplugin.h"
#include "dcmtk/dcmjpls/dldefine.h"
#include "dcmtk/ofstd/oftypes.h"

/*------------------------*
 *  forward declarations  *
 *------------------------*/

class DiImage;

/*---------------------*
 *  class declaration  *
 *---------------------*/

/** Implementation of a JPEG-LS plugin for the dcmimgle/dcmimage library
 */
class DCMTK_DCMJPLS_EXPORT DiJPLSPlugin: public DiPluginFormat
{

public:

    /** constructor
     */
    DiJPLSPlugin();

    /** destructor
     */
    virtual ~DiJPLSPlugin();

    /** write given image to a file stream (JPLS format)
     *  @param image pointer to DICOM image object to be written
     *  @param stream stream to which the image is written (open in binary mode!)
     *  @param frame index of frame used for output (default: first frame = 0)
     *  @return true if successful, false otherwise
     */
    virtual int write(DiImage *image,
                      FILE *stream,
                      const unsigned long frame = 0) const;

    /** enable or disable true lossless compression mode
     *  @param mode, OFTrue for true lossless, OFFalse for rendered lossless
     */
    void setTrueLosslessMode(OFBool mode);

    /** enable or disable forcing the output image bit depth to 8 bits in rendered lossless mode
     *  @param mode, OFTrue for force to 8 bits
     */
    void setForce8BitMode(OFBool mode);

private:

   /// flag indicating the active compression mode
   OFBool trueLosslessMode_;

   /// flag indicating the force-to-8-bit mode in rendered lossless mode
   OFBool force8BitMode_;

};


#endif
