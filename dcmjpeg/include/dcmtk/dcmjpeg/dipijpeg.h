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
 *  Module:  dcmjpeg
 *
 *  Author:  Joerg Riesmeier
 *
 *  Purpose: Implements JPEG interface for plugable image formats
 *
 */


#ifndef DIPIJPEG_H
#define DIPIJPEG_H

#include "dcmtk/config/osconfig.h"

#include "dcmtk/dcmimgle/diplugin.h"
#include "dcmtk/dcmjpeg/djutils.h"


/*------------------------*
 *  forward declarations  *
 *------------------------*/

class DiImage;


/*---------------------*
 *  class declaration  *
 *---------------------*/

/** Implementation of a JPEG plugin for the dcmimgle/dcmimage library
 */
class DCMTK_DCMJPEG_EXPORT DiJPEGPlugin
  : public DiPluginFormat
{

  public:

    /** constructor
     */
    DiJPEGPlugin();

    /** destructor
     */
    virtual ~DiJPEGPlugin();

    /** write given image to a file stream (JPEG format)
     *  @param image pointer to DICOM image object to be written
     *  @param stream stream to which the image is written (open in binary mode!)
     *  @param frame index of frame used for output (default: first frame = 0)
     *  @return true if successful, false otherwise
     */
    virtual int write(DiImage *image,
                      FILE *stream,
                      const unsigned long frame = 0) const;

    /** set quality value for JPEG compression
     *  @param quality quality value (0..100, in percent)
     */
    void setQuality(const unsigned int quality);

    /** set (sub) sampling for JPEG compression.
     *  Only used for color images. ESS_444 means no sub-sampling, ESS_422 horizontal
     *  subsampling of chroma components and ESS_422 horizontal and vertical subsampling
     *  of chroma components.
     *  @param sampling sampling (valid values: ESS_444, ESS_422 or ESS_411)
     */
    void setSampling(const E_SubSampling sampling);

    /** callback function used to report IJG warning messages and the like.
     *  Should not be called by user code directly.
     *  @param arg opaque pointer to JPEG compress structure
     */
    void outputMessage(void *arg) const;

    /** get version information of the TIFF library.
     *  Typical output format: "IJG, Version 6b  27-Mar-1998 (modified)"
     *  @return name and version number of the TIFF library
     */
    static OFString getLibraryVersionString();


 private:

    /// quality value (0..100, in percent), default: 75
    unsigned int  Quality;
    /// (sub) sampling: ESS_444, ESS_422 (default), ESS_411
    E_SubSampling Sampling;
};


#endif
