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
 *  Module:  dcmjpeg
 *
 *  Author:  Marco Eichelberg
 *
 *  Purpose: Implements TIFF interface for plugable image formats
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 2001-12-06 10:10:59 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmimage/include/Attic/dipitiff.h,v $
 *  CVS/RCS Revision: $Revision: 1.2 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#ifndef __DIPITIFF_H
#define __DIPITIFF_H

#include "osconfig.h"

#ifdef WITH_LIBTIFF

#include "diplugin.h"

class DiImage;

/** describes the different types of compression supported by
 *  the TIFF plugin.  Enumeration depends on capabilities
 *  of underlying TIFF library (libtiff).
 */
enum DiTIFFCompression
{
  /// PackBits compression (RLE)
  E_tiffPackBitsCompression,
  
  /// LZW compression
  E_tiffLZWCompression,

  /// uncompressed
  E_tiffNoCompression
};

/** describes the optional predictor used with TIFF LZW compression
 */
enum DiTIFFLZWPredictor
{
  /// omit preditor tag (defaults to no prediction)
  E_tiffLZWPredictorDefault,

  /// predictor 1: no prediction scheme
  E_tiffLZWPredictorNoPrediction,

  /// predictor 2: use horizontal differencing
  E_tiffLZWPredictorHDifferencing
};


/** Implementation of a TIFF plugin for the dcmimgle/dcmimage library
 */
class DiTIFFPlugin
  : public DiPluginFormat
{

  public:

    /** constructor
     */
    DiTIFFPlugin();

    /** destructor
     */
    virtual ~DiTIFFPlugin();

    /** write given image to a file stream (TIFF format)
     *  @param image pointer to DICOM image object to be written
     *  @param stream stream to which the image is written (open in binary mode!)
     *  @param frame index of frame used for output (default: first frame = 0)
     *  @return true if successful, false otherwise
     */
    virtual int write(DiImage *image,
                      FILE *stream,
                      const unsigned long frame = 0) const;

    /** set compression type for TIFF creation
     *  @param ctype compression type
     */
    void setCompressionType(DiTIFFCompression ctype);

    /** set predictor type for LZW compression
     *  @param pred predictor type
     */
    void setLZWPredictor(DiTIFFLZWPredictor pred);

    /** set rows per strip for TIFF creation.
     *  @param rows rows per strip. By default (value 0),
     *    rows per strip is calculated automatically such that
     *    each strip contains about 8 kByte of data.
     */
    void setRowsPerStrip(unsigned long rows = 0);

 private:

    /// TIFF compression type
    DiTIFFCompression compressionType;

    /// TIFF predictor type
    DiTIFFLZWPredictor predictor;

    /// TIFF rows per strip
    unsigned long rowsPerStrip;
};

#endif
#endif

/*
 *
 * CVS/RCS Log:
 * $Log: dipitiff.h,v $
 * Revision 1.2  2001-12-06 10:10:59  meichel
 * Removed references to tiffconf.h which does not exist on all installations
 *
 * Revision 1.1  2001/11/30 16:47:56  meichel
 * Added TIFF export option to dcm2pnm and dcmj2pnm
 *
 *
 */
