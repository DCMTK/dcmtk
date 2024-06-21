/*
 *
 *  Copyright (C) 2003-2024, OFFIS e.V.
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
 *  Module:  dcmimage
 *
 *  Author:  Alexander Haderer
 *
 *  Purpose: Implements PNG interface for pluggable image formats
 *
 */


#include "dcmtk/config/osconfig.h"

#ifdef WITH_LIBPNG

#include "dcmtk/dcmimage/dipipng.h"
#include "dcmtk/dcmdata/dctypes.h"
#include "dcmtk/dcmimgle/diimage.h"
#include "dcmtk/dcmdata/dcuid.h"      /* for dcmtk version */
#include "dcmtk/ofstd/ofdiag.h"
#include "dcmtk/ofstd/ofstd.h"

BEGIN_EXTERN_C
#ifdef HAVE_LIBPNG_PNG_H
#include <libpng/png.h>
#else
#include <png.h>
#endif
END_EXTERN_C


DiPNGPlugin::DiPNGPlugin()
: DiPluginFormat()
, interlaceType(E_pngInterlaceAdam7)
, metainfoType(E_pngFileMetainfo)
, bitsPerSample(8)
{
}


DiPNGPlugin::~DiPNGPlugin()
{
}

#include DCMTK_DIAGNOSTIC_PUSH
#include DCMTK_DIAGNOSTIC_IGNORE_VISUAL_STUDIO_OBJECT_DESTRUCTION_WARNING

int DiPNGPlugin::write(
  DiImage *image,
  FILE *stream,
  const unsigned long frame) const
{
  volatile int result = 0;  // gcc -W requires volatile here because of longjmp
  if ((image != NULL) && (stream != NULL))
  {
    /* create bitmap with 8 or 16 bits per sample */
    const int bit_depth = bitsPerSample;
    const void *data = image->getOutputData(frame, bit_depth /*bits*/, 0 /*planar*/);
    if (data != NULL)
    {
      png_struct *png_ptr = NULL;
      png_info *info_ptr = NULL;
      png_byte *pix_ptr = NULL;

      png_byte ** volatile row_ptr = NULL;
      volatile png_textp  text_ptr = NULL;
      png_time ptime;

      const int width  = image->getColumns();
      const int height = image->getRows();
      int color_type;
      int bpp;            // bytesperpixel

      int row;

      // create png write struct
      png_ptr = png_create_write_struct( PNG_LIBPNG_VER_STRING, NULL, NULL, NULL );
      if( png_ptr == NULL ) {
        return 0;
      }

      // create png info struct
      info_ptr = png_create_info_struct( png_ptr );
      if( info_ptr == NULL ) {
        png_destroy_write_struct( &png_ptr, NULL );
        return 0;
      }

      // setjmp stuff for png lib
      if( setjmp(png_jmpbuf(png_ptr) ) ) {
        png_destroy_write_struct( &png_ptr, NULL );
        if( row_ptr )  delete[] row_ptr;
        if( text_ptr ) delete[] text_ptr;
        return 0;
      }

      if( (image->getInternalColorModel() == EPI_Monochrome1) ||
          (image->getInternalColorModel() == EPI_Monochrome2) )
      {
        color_type = PNG_COLOR_TYPE_GRAY;
        bpp = bit_depth / 8;
      } else {
        color_type = PNG_COLOR_TYPE_RGB;
        bpp = 3 * bit_depth / 8;
      }

      int opt_interlace = 0;
      switch (interlaceType) {
        case E_pngInterlaceAdam7:
          opt_interlace = PNG_INTERLACE_ADAM7;
          break;
        case E_pngInterlaceNone:
          opt_interlace = PNG_INTERLACE_NONE;
          break;
      }

      // init png io structure
      png_init_io( png_ptr, stream );

      // set write mode
      png_set_IHDR( png_ptr, info_ptr, width, height, bit_depth, color_type,
                    opt_interlace, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

      // set text & time
      if( metainfoType == E_pngFileMetainfo ) {
        text_ptr = new png_text[3];
        if( text_ptr == NULL ) {
          png_destroy_write_struct( &png_ptr, NULL );
          return result;
        }
        text_ptr[0].key         = OFconst_cast(char *, "Title");
        text_ptr[0].text        = OFconst_cast(char *, "Converted DICOM Image");
        text_ptr[0].compression = PNG_TEXT_COMPRESSION_NONE;
        text_ptr[1].key         = OFconst_cast(char *, "Software");
        text_ptr[1].text        = OFconst_cast(char *, "OFFIS DCMTK");
        text_ptr[1].compression = PNG_TEXT_COMPRESSION_NONE;
        text_ptr[2].key         = OFconst_cast(char *, "Version");
        text_ptr[2].text        = OFconst_cast(char *, OFFIS_DCMTK_VERSION);
        text_ptr[2].compression = PNG_TEXT_COMPRESSION_NONE;
#ifdef PNG_iTXt_SUPPORTED
        text_ptr[0].lang = NULL;
        text_ptr[1].lang = NULL;
        text_ptr[2].lang = NULL;
#endif
        png_set_text( png_ptr, info_ptr, text_ptr, 3 );

        png_convert_from_time_t( &ptime, time(NULL) );
        png_set_tIME( png_ptr, info_ptr, &ptime );
      }

      // write header
      png_write_info( png_ptr, info_ptr );
      row_ptr = new png_bytep[height];
      if( row_ptr == NULL ) {
        png_destroy_write_struct( &png_ptr, NULL );
        if( text_ptr ) delete[] text_ptr;
        return result;
      }
      for( row=0, pix_ptr=OFstatic_cast(png_byte*, OFconst_cast(void*, data));
        row<height;
        row++, pix_ptr+=width*bpp )
      {
        row_ptr[row] = pix_ptr;
      }

      // swap bytes (if needed)
      if ( (bit_depth == 16) && (gLocalByteOrder != EBO_BigEndian) )
        png_set_swap( png_ptr );

      // write image
      png_write_image( png_ptr, row_ptr );

      // write additional chunks
      png_write_end( png_ptr, info_ptr );

      // finish
      png_destroy_write_struct( &png_ptr, &info_ptr );
      delete[] row_ptr;
      if( text_ptr ) delete[] text_ptr;
      result = 1;
    }
  }

  return result;
}

#include DCMTK_DIAGNOSTIC_POP


void DiPNGPlugin::setInterlaceType(DiPNGInterlace itype)
{
  interlaceType = itype;
}


void DiPNGPlugin::setMetainfoType(DiPNGMetainfo minfo)
{
  metainfoType = minfo;
}


void DiPNGPlugin::setBitsPerSample(const int bpp)
{
  if( (bpp == 8) || (bpp == 16) )
    bitsPerSample = bpp;
}


OFString DiPNGPlugin::getLibraryVersionString()
{
  OFString versionStr = "LIBPNG, Version ";
  char cver[10];
  png_uint_32 ver = png_access_version_number();
  if( ver < 999999 ) {
    OFStandard::snprintf(cver, sizeof(cver), "%li.%li.%li",
                   OFstatic_cast(long int, (ver/10000)%100),
                   OFstatic_cast(long int, (ver/100)%100),
                   OFstatic_cast(long int, ver%100) );
  }else{
    OFStandard::snprintf(cver, sizeof(cver), "unknown" );
  }
  versionStr.append( cver );
  return versionStr;
}

#else /* WITH_LIBPNG */

int dipipng_cc_dummy_to_keep_linker_from_moaning = 0;

#endif
