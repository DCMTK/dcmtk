/*
 *
 *  Copyright (C) 2003-2010, OFFIS e.V.
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
 *  Module:  dcmdata
 *
 *  Author:  Joerg Riesmeier
 *
 *  Purpose: Implementation of DICOMDIR image support (plugin)
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:17:16 $
 *  CVS/RCS Revision: $Revision: 1.4 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#ifndef DDPIIMPL_H
#define DDPIIMPL_H

#include "dcmtk/config/osconfig.h"

#include "dcmtk/dcmdata/dcddirif.h"


/*---------------------*
 *  class declaration  *
 *---------------------*/

/** Implementation of the plugable image support for the DICOMDIR class
 */
class DicomDirImageImplementation
  : public DicomDirImagePlugin
{
  public:

    /** constructor
     */
    DicomDirImageImplementation();

    /** destructor
     */
    virtual ~DicomDirImageImplementation();

    /** scale given pixel data (monochrome only).
     *  The destination pixel data array needs to be allocated by the caller.
     *  @param srcData source pixel data (byte array)
     *  @param srcWidth width of the source pixel data (in pixels)
     *  @param srcHeight height of the source pixel data (in pixels)
     *  @param dstData destination pixel data (resulting byte array, not NULL)
     *  @param dstWidth width of the scaled pixel data (in pixels)
     *  @param dstHeight height of the scaled pixel data (in pixels)
     *  @return OFTrue if successful, OFFalse otherwise
     */
    virtual OFBool scaleData(const Uint8 *srcData,
                             const unsigned int srcWidth,
                             const unsigned int srcHeight,
                             Uint8 *dstData,
                             const unsigned int dstWidth,
                             const unsigned int dstHeight) const;

    /** get scaled pixel data from DICOM image.
     *  The resulting scaled image (pixel array) is always monochrome.
     *  The resulting pixel data array needs to be allocated by the caller.
     *  @param dataset DICOM dataset in which the DICOM image is stored
     *  @param pixel resulting pixel data array (not NULL)
     *  @param count number of pixels allocated for the resulting array
     *  @param frame index of the frame to be scaled (1..n)
     *  @param width width of the scaled image (in pixels)
     *  @param height height of the scaled image (in pixels)
     *  @param decompressAll always decompress complete pixel data if true
     *  @return OFTrue if successful, OFFalse otherwise
     */
    virtual OFBool scaleImage(DcmItem *dataset,
                              Uint8 *pixel,
                              const unsigned long count,
                              const unsigned long frame,
                              const unsigned int width,
                              const unsigned int height,
                              const OFBool decompressAll = OFFalse) const;
};


#endif


/*
 *
 * CVS/RCS Log:
 * $Log: ddpiimpl.h,v $
 * Revision 1.4  2010-10-14 13:17:16  joergr
 * Updated copyright header. Added reference to COPYRIGHT file.
 *
 * Revision 1.3  2009-11-25 13:36:47  joergr
 * Adapted code for new approach to access individual frames of a DICOM image.
 *
 * Revision 1.2  2005-12-08 16:59:09  meichel
 * Changed include path schema for all DCMTK header files
 *
 * Revision 1.1  2003/08/12 13:15:27  joergr
 * Added plugable image support for the new DICOMDIR class.
 *
 *
 */
