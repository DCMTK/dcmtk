/*
 *
 *  Copyright (C) 2001-2022, OFFIS e.V.
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
 *  Purpose: Implements TIFF interface for pluggable image formats
 *
 */


#include "dcmtk/config/osconfig.h"

#ifdef WITH_LIBTIFF

#include "dcmtk/dcmdata/dctypes.h"
#include "dcmtk/dcmimgle/diimage.h"
#include "dcmtk/dcmimage/dipitiff.h"
#include "dcmtk/dcmdata/dcuid.h"      /* for dcmtk version */

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

#if TIFFLIB_VERSION < 20050912
#error TIFF library versions prior to 3.7.4 are not supported by DCMTK on Win32 - critical API change!
#endif

/* Older versions of libtiff expected a Win32 HANDLE when compiled on Windows
 * instead of a file descriptor. The code below was needed to make that work.
 * Libtiff version 3.7.4 and newer are known to use a file descriptor instead,
 * but it is not completely clear at which libtiff release the API change happened.
 *
 * #ifdef __CYGWIN__
 *   stream_fd = OFstatic_cast(int, get_osfhandle(stream_fd));
 * #else
 *   stream_fd =OFstatic_cast(int, _get_osfhandle(stream_fd));
 * #endif
 */

#elif TIFFLIB_VERSION < 20041016
#error TIFF library versions prior to 3.7.0 are not supported by DCMTK - TIFFCleanup is missing!
#endif

    /* create bitmap with 8 bits per sample */
    void *data = OFconst_cast(void *, image->getOutputData(frame, 8 /*bits*/, 0 /*planar*/));
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

        long opt_rowsperstrip = OFstatic_cast(long, rowsPerStrip);
        if (opt_rowsperstrip <= 0) opt_rowsperstrip = 8192 / bytesperrow;
        if (opt_rowsperstrip == 0) opt_rowsperstrip++;

        OFBool OK = OFTrue;
        unsigned char *bytedata = OFstatic_cast(unsigned char *, data);
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
          TIFFSetField(tif, TIFFTAG_IMAGEDESCRIPTION, "Converted DICOM Image");
          TIFFSetField(tif, TIFFTAG_SOFTWARE, "OFFIS DCMTK " OFFIS_DCMTK_VERSION);
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
          /* Clean up internal structures and free memory.
           * However, the file will be closed by the caller, therefore
           * TIFFClose(tif) is not called.
           */
          TIFFCleanup(tif);
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

int dipitiff_cc_dummy_to_keep_linker_from_moaning = 0;

#endif
