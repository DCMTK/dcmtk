/*
 *
 *  Copyright (C) 2001-2024, OFFIS e.V.
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

#if TIFFLIB_VERSION < 20050912
#error TIFF library versions prior to 3.7.4 are not supported by DCMTK
#endif

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


#ifdef _WIN32

    /* On Windows, TIFFFdOpen() expects a Windows HANDLE (which is a pointer
     * type) instead of a file descriptor, but passes the file descriptor as an
     * int.  Despite HANDLE being a 64-bit type on Win64 and int being 32-bit,
     * this is apparently safe, because Win64 guarantees to only use 32-bit
     * handles, for interoperability reasons, as documented here:
     *
     *   https://docs.microsoft.com/en-us/windows/win32/winprog64/interprocess-communication
     *
     * Therefore, we use _get_osfhandle() to access the HANDLE underlying the
     * file descriptor.
     */

#ifdef __CYGWIN__
    stream_fd = OFstatic_cast(int, get_osfhandle(stream_fd));
#else
    stream_fd = OFstatic_cast(int, _get_osfhandle(stream_fd));
#endif

#endif /* _WIN32 */

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
           * TIFFClose() is not called.
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
