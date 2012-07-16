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
 *  Author:  Marco Eichelberg
 *
 *  Purpose: Implements TIFF interface for plugable image formats
 *
 */


#ifndef DIPITIFF_H
#define DIPITIFF_H

#include "dcmtk/config/osconfig.h"

#ifdef WITH_LIBTIFF

#include "dcmtk/ofstd/ofstring.h"

#include "dcmtk/dcmimgle/diplugin.h"

#include "dcmtk/dcmimage/dicdefin.h"


/*------------------------*
 *  forward declarations  *
 *------------------------*/

class DiImage;


/*--------------------*
 *  type definitions  *
 *--------------------*/

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


/*---------------------*
 *  class declaration  *
 *---------------------*/

/** Implementation of a TIFF plugin for the dcmimgle/dcmimage library
 */
class DCMTK_DCMIMAGE_EXPORT DiTIFFPlugin
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

    /** get version information of the TIFF library.
     *  Typical output format: "LIBTIFF, Version 3.5.7"
     *  @return name and version number of the TIFF library
     */
    static OFString getLibraryVersionString();


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
