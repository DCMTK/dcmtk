/*
 *
 *  Copyright (C) 2001-2002, OFFIS
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
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2002-09-19 08:34:07 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmimage/libsrc/dipitiff.cc,v $
 *  CVS/RCS Revision: $Revision: 1.4 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#include "osconfig.h"

#ifdef WITH_LIBTIFF

#include "dctypes.h"
#include "diimage.h"
#include "dipitiff.h"

BEGIN_EXTERN_C
#include <tiffio.h>

#ifdef HAVE_WINDOWS_H
#include <io.h>  /* for _get_osfhandle() */
#endif
END_EXTERN_C


DiTIFFPlugin::DiTIFFPlugin()
: DiPluginFormat()
, compressionType(E_tiffLZWCompression)
, predictor(E_tiffLZWPredictorDefault)
, rowsPerStrip(0)
{
}


DiTIFFPlugin::~DiTIFFPlugin()
{
}


int DiTIFFPlugin::write(
  DiImage *image,
  FILE *stream,
  const unsigned long frame) const
{
  int result = 0;
  if ((image != NULL) && (stream != NULL))
  {
    int stream_fd = fileno(stream);

#ifdef HAVE_WINDOWS_H
    /* The Win32 version of libtiff expects a Win32 HANDLE (casted to int)
     * instead of a file descriptor. Therefore, we use _get_osfhandle()
     * which takes a Unix-style file descriptor and derives the corresponding
     * Win32 API file handle (HANDLE). This function may not be available on all
     * compilers for Win32, sorry.
     */
#ifdef __CYGWIN__
    stream_fd =(int)get_osfhandle(stream_fd);
#else
    stream_fd =(int)_get_osfhandle(stream_fd);
#endif
#endif

    /* create bitmap with 8 bits per sample */
    void *data = image->getOutputData(frame, 8 /*bits*/, 0 /*planar*/);
    if (data != NULL)
    {
      OFBool isMono = (image->getInternalColorModel() == EPI_Monochrome1) || (image->getInternalColorModel() == EPI_Monochrome2);
      Uint16 rows = image->getRows();
      Uint16 cols = image->getColumns();

      short photometric = isMono ? PHOTOMETRIC_MINISBLACK : PHOTOMETRIC_RGB;
      short samplesperpixel = isMono ? 1 : 3;
      unsigned long bytesperrow = cols * samplesperpixel;
      if (bytesperrow > 0)
      {
        short opt_predictor = 0;
        switch (predictor)
        {
          case E_tiffLZWPredictorDefault:
            opt_predictor = 0;
            break;
          case E_tiffLZWPredictorNoPrediction:
            opt_predictor = 1;
            break;
          case E_tiffLZWPredictorHDifferencing:
            opt_predictor = 2;
            break;
        }

        unsigned short opt_compression = COMPRESSION_NONE;
        switch (compressionType)
        {
          case E_tiffLZWCompression:
            opt_compression = COMPRESSION_LZW;
            break;
          case E_tiffPackBitsCompression:
            opt_compression = COMPRESSION_PACKBITS;
            break;
          case E_tiffNoCompression:
            opt_compression = COMPRESSION_NONE;
            break;
        }

        long opt_rowsperstrip = (long) rowsPerStrip;
        if (opt_rowsperstrip <= 0) opt_rowsperstrip = 8192 / bytesperrow;
        if (opt_rowsperstrip == 0) opt_rowsperstrip++;

        OFBool OK = OFTrue;
        unsigned char *bytedata = (unsigned char *)data;
        TIFF *tif = TIFFFdOpen(stream_fd, "TIFF", "w");
        if (tif)
        {
          /* Set TIFF parameters. */
          TIFFSetField(tif, TIFFTAG_IMAGEWIDTH, cols);
          TIFFSetField(tif, TIFFTAG_IMAGELENGTH, rows);
          TIFFSetField(tif, TIFFTAG_BITSPERSAMPLE, 8);
          TIFFSetField(tif, TIFFTAG_ORIENTATION, ORIENTATION_TOPLEFT);
          TIFFSetField(tif, TIFFTAG_COMPRESSION, opt_compression);
          if (opt_compression == COMPRESSION_LZW && opt_predictor != 0)
          TIFFSetField(tif, TIFFTAG_PREDICTOR, opt_predictor);
          TIFFSetField(tif, TIFFTAG_PHOTOMETRIC, photometric);
          TIFFSetField(tif, TIFFTAG_FILLORDER, FILLORDER_MSB2LSB);
          TIFFSetField(tif, TIFFTAG_DOCUMENTNAME, "unnamed");
          TIFFSetField(tif, TIFFTAG_IMAGEDESCRIPTION, "converted DICOM image");
          TIFFSetField(tif, TIFFTAG_SAMPLESPERPIXEL, samplesperpixel);
          TIFFSetField(tif, TIFFTAG_ROWSPERSTRIP, opt_rowsperstrip);
          /* TIFFSetField(tif, TIFFTAG_STRIPBYTECOUNTS, rows / opt_rowsperstrip); */
          TIFFSetField(tif, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);

          /* Now write the TIFF data. */
          unsigned long offset = 0;
          for (Uint16 i=0; (i < rows) && OK; i++)
          {
            if (TIFFWriteScanline(tif, bytedata + offset, i, 0) < 0) OK = OFFalse;
            offset += bytesperrow;
          }
          TIFFFlushData(tif);
          TIFFClose(tif);
        }
        if (OK) result = 1;
      }
    }

    /* delete pixel data */
    image->deleteOutputData();
  }
  return result;
}


void DiTIFFPlugin::setCompressionType(DiTIFFCompression ctype)
{
  compressionType = ctype;
}

void DiTIFFPlugin::setLZWPredictor(DiTIFFLZWPredictor pred)
{
  predictor = pred;
}

void DiTIFFPlugin::setRowsPerStrip(unsigned long rows)
{
  rowsPerStrip = rows;
}

OFString DiTIFFPlugin::getLibraryVersionString()
{
    /* use first line only, omit copyright information */
    OFString versionStr = TIFFGetVersion();
    const size_t pos = versionStr.find('\n');
    if (pos != OFString_npos)
        versionStr.erase(pos);
    return versionStr;
}

#else /* WITH_LIBTIFF */

const int dipitiff_cc_dummy_to_keep_linker_from_moaning = 0;

#endif

/*
 *
 * CVS/RCS Log:
 * $Log: dipitiff.cc,v $
 * Revision 1.4  2002-09-19 08:34:07  joergr
 * Added static method getLibraryVersionString().
 *
 * Revision 1.3  2002/08/29 16:00:56  meichel
 * Fixed DiTIFFPlugin::write(): libtiff's TIFFFdOpen() expects a HANDLE
 *   instead of a file descriptor when compiled on WIN32.
 *
 * Revision 1.2  2001/12/06 10:11:00  meichel
 * Removed references to tiffconf.h which does not exist on all installations
 *
 * Revision 1.1  2001/11/30 16:47:57  meichel
 * Added TIFF export option to dcm2pnm and dcmj2pnm
 *
 *
 */
