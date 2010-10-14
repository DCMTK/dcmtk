/*
 *
 *  Copyright (C) 2001-2010, OFFIS e.V.
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
 *  Author:  Norbert Olges, Marco Eichelberg
 *
 *  Purpose: decompression routines of the IJG JPEG library configured for 8 bits/sample.
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:14:21 $
 *  CVS/RCS Revision: $Revision: 1.21 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmjpeg/djdijg8.h"
#include "dcmtk/dcmjpeg/djcparam.h"
#include "dcmtk/dcmdata/dcerror.h"

#define INCLUDE_CSTDIO
#define INCLUDE_CSETJMP
#define INCLUDE_CSTRING
#include "dcmtk/ofstd/ofstdinc.h"

// These two macros are re-defined in the IJG header files.
// We undefine them here and hope that IJG's configure has
// come to the same conclusion that we have...
#ifdef HAVE_STDLIB_H
#undef HAVE_STDLIB_H
#endif
#ifdef HAVE_STDDEF_H
#undef HAVE_STDDEF_H
#endif

BEGIN_EXTERN_C
#define boolean ijg_boolean
#include "jpeglib8.h"
#include "jerror8.h"
#undef boolean

// disable any preprocessor magic the IJG library might be doing with the "const" keyword
#ifdef const
#undef const
#endif

#ifdef USE_STD_CXX_INCLUDES
// Solaris defines longjmp() in namespace std, other compilers don't...
namespace std { }
using namespace std;
#endif

// private error handler struct
struct DJDIJG8ErrorStruct
{
  // the standard IJG error handler object
  struct jpeg_error_mgr pub;

  // our jump buffer
  jmp_buf setjmp_buffer;

  // pointer to this
  DJDecompressIJG8Bit *instance;
};

// private source manager struct
struct DJDIJG8SourceManagerStruct
{
  // the standard IJG source manager object
  struct jpeg_source_mgr pub;

  // number of bytes to skip at start of buffer
  long skip_bytes;

  // buffer from which reading will continue as soon as the current buffer is empty
  Uint8 *next_buffer;

  // buffer size
  Uint32 next_buffer_size;
};

// callback forward declarations
void DJDIJG8ErrorExit(j_common_ptr);
void DJDIJG8EmitMessage(j_common_ptr cinfo, int msg_level);
void DJDIJG8initSource(j_decompress_ptr);
ijg_boolean DJDIJG8fillInputBuffer(j_decompress_ptr);
void DJDIJG8skipInputData(j_decompress_ptr, long);
void DJDIJG8termSource(j_decompress_ptr);

END_EXTERN_C


// error handler, executes longjmp
void DJDIJG8ErrorExit(j_common_ptr cinfo)
{
  DJDIJG8ErrorStruct *myerr = (DJDIJG8ErrorStruct *)cinfo->err;
  longjmp(myerr->setjmp_buffer, 1);
}

// message handler for warning messages and the like
void DJDIJG8EmitMessage(j_common_ptr cinfo, int msg_level)
{
  DJDIJG8ErrorStruct *myerr = (DJDIJG8ErrorStruct *)cinfo->err;
  myerr->instance->emitMessage(msg_level);
}


// methods for decompress-source-manager

void DJDIJG8initSource(j_decompress_ptr /* cinfo */)
{
}

ijg_boolean DJDIJG8fillInputBuffer(j_decompress_ptr cinfo)
{
  DJDIJG8SourceManagerStruct *src = (DJDIJG8SourceManagerStruct *)(cinfo->src);

  // if we already have the next buffer, switch buffers
  if (src->next_buffer)
  {
    src->pub.next_input_byte    = src->next_buffer;
    src->pub.bytes_in_buffer    = (unsigned int) src->next_buffer_size;
    src->next_buffer            = NULL;
    src->next_buffer_size       = 0;

    // The suspension was caused by DJDIJG8skipInputData iff src->skip_bytes > 0.
    // In this case we must skip the remaining number of bytes here.
    if (src->skip_bytes > 0)
    {
      if (src->pub.bytes_in_buffer < (unsigned long) src->skip_bytes)
      {
        src->skip_bytes            -= src->pub.bytes_in_buffer;
        src->pub.next_input_byte   += src->pub.bytes_in_buffer;
        src->pub.bytes_in_buffer    = 0;
        // cause a suspension return
        return FALSE;
      }
      else
      {
        src->pub.bytes_in_buffer   -= (unsigned int) src->skip_bytes;
        src->pub.next_input_byte   += src->skip_bytes;
        src->skip_bytes             = 0;
      }
    }
    return TRUE;
  }

  // otherwise cause a suspension return
  return FALSE;
}

void DJDIJG8skipInputData(
  j_decompress_ptr cinfo,
  long num_bytes)
{
  DJDIJG8SourceManagerStruct *src = (DJDIJG8SourceManagerStruct *)(cinfo->src);

  if (src->pub.bytes_in_buffer < (size_t)num_bytes)
  {
    src->skip_bytes             = num_bytes - src->pub.bytes_in_buffer;
    src->pub.next_input_byte   += src->pub.bytes_in_buffer;
    src->pub.bytes_in_buffer    = 0; // causes a suspension return
  }
  else
  {
    src->pub.bytes_in_buffer   -= (unsigned int) num_bytes;
    src->pub.next_input_byte   += num_bytes;
    src->skip_bytes             = 0;
  }
}

void DJDIJG8termSource(j_decompress_ptr /* cinfo */)
{
}


DJDecompressIJG8Bit::DJDecompressIJG8Bit(const DJCodecParameter& cp, OFBool isYBR)
: DJDecoder()
, cparam(&cp)
, cinfo(NULL)
, suspension(0)
, jsampBuffer(NULL)
, dicomPhotometricInterpretationIsYCbCr(isYBR)
, decompressedColorModel(EPI_Unknown)
{
}

DJDecompressIJG8Bit::~DJDecompressIJG8Bit()
{
  cleanup();
}


OFCondition DJDecompressIJG8Bit::init()
{
  suspension = 0;
  decompressedColorModel = EPI_Unknown;
  cleanup(); // prevent double initialization

  cinfo = new jpeg_decompress_struct();
  if (cinfo)
  {
    volatile DJDIJG8SourceManagerStruct *src = NULL;
    volatile DJDIJG8ErrorStruct *jerr = new DJDIJG8ErrorStruct();
    if (jerr)
    {
      src = new DJDIJG8SourceManagerStruct();
      if (src)
      {
        // Specify the source of the compressed data
        src->pub.init_source       = DJDIJG8initSource;
        src->pub.fill_input_buffer = DJDIJG8fillInputBuffer;
        src->pub.skip_input_data   = DJDIJG8skipInputData;
        src->pub.resync_to_restart = jpeg_resync_to_restart;
        src->pub.term_source       = DJDIJG8termSource;
        src->pub.bytes_in_buffer   = 0;
        src->pub.next_input_byte   = NULL;
        src->skip_bytes             = 0;
        src->next_buffer            = NULL;
        src->next_buffer_size       = 0;
      }
      else
      {
        delete OFconst_cast(DJDIJG8ErrorStruct *, jerr);
        delete cinfo;
        cinfo = NULL;
        return EC_MemoryExhausted;
      }
      cinfo->err = jpeg_std_error(& OFconst_cast(DJDIJG8ErrorStruct *, jerr)->pub);
      jerr->instance = this;
      jerr->pub.error_exit = DJDIJG8ErrorExit;
      jerr->pub.emit_message = DJDIJG8EmitMessage;
      if (setjmp(OFconst_cast(DJDIJG8ErrorStruct *, jerr)->setjmp_buffer))
      {
        // the IJG error handler will cause the following code to be executed
        char buffer[JMSG_LENGTH_MAX];
        (*cinfo->err->format_message)((jpeg_common_struct *)cinfo, buffer); /* Create the message */
        cleanup();
        return makeOFCondition(OFM_dcmjpeg, EJCode_IJG8_Decompression, OF_error, buffer);
      }
    }
    else
    {
      delete cinfo;
      cinfo = NULL;
      return EC_MemoryExhausted;
    }
    jpeg_create_decompress(cinfo);
    cinfo->src = &OFconst_cast(DJDIJG8SourceManagerStruct *, src)->pub;
  } else return EC_MemoryExhausted;

  // everything OK
  return EC_Normal;
}


void DJDecompressIJG8Bit::cleanup()
{
  if (cinfo)
  {
    jpeg_destroy_decompress(cinfo);
    delete (DJDIJG8ErrorStruct *)(cinfo->err);
    delete (DJDIJG8SourceManagerStruct *)(cinfo->src);
    delete cinfo;
    cinfo = NULL;
  }
}


OFCondition DJDecompressIJG8Bit::decode(
  Uint8 *compressedFrameBuffer,
  Uint32 compressedFrameBufferSize,
  Uint8 *uncompressedFrameBuffer,
  Uint32 uncompressedFrameBufferSize,
  OFBool isSigned)
{

  if (cinfo==NULL || compressedFrameBuffer==NULL || uncompressedFrameBuffer==NULL) return EC_IllegalCall;

  if (setjmp(((DJDIJG8ErrorStruct *)(cinfo->err))->setjmp_buffer))
  {
    // the IJG error handler will cause the following code to be executed
    char buffer[JMSG_LENGTH_MAX];
    (*cinfo->err->format_message)((jpeg_common_struct *)cinfo, buffer); /* Create the message */
    cleanup();
    return makeOFCondition(OFM_dcmjpeg, EJCode_IJG8_Decompression, OF_error, buffer);
  }

  // feed compressed buffer into cinfo structure.
  // The buffer will be activated by the next call to DJDIJG8fillInputBuffer.
  DJDIJG8SourceManagerStruct *src = (DJDIJG8SourceManagerStruct *)(cinfo->src);
  src->next_buffer = compressedFrameBuffer;
  src->next_buffer_size = compressedFrameBufferSize;

  // Obtain image info
  if (suspension < 2)
  {
    if (JPEG_SUSPENDED == jpeg_read_header(cinfo, TRUE))
    {
      suspension = 1;
      return EJ_Suspension;
    }

    // check if color space conversion is enabled
    OFBool colorSpaceConversion = OFFalse;
    // check whether to use the IJG library guess for the JPEG color space
    OFBool colorSpaceGuess = OFFalse;
    switch (cparam->getDecompressionColorSpaceConversion())
    {
      case EDC_photometricInterpretation: // color space conversion if DICOM photometric interpretation is YCbCr
        colorSpaceConversion = dicomPhotometricInterpretationIsYCbCr;
        break;
      case EDC_lossyOnly: // color space conversion if lossy JPEG
        if (cinfo->process != JPROC_LOSSLESS)
          colorSpaceConversion = OFTrue;
        break;
      case EDC_always: // always do color space conversion
        colorSpaceConversion = OFTrue;
        break;
      case EDC_never: // never do color space conversion
        break;
      case EDC_guessLossyOnly: // use color space guess by IJG library if lossy JPEG
        if (cinfo->process != JPROC_LOSSLESS)
        {
          colorSpaceGuess = OFTrue;
          if (cinfo->jpeg_color_space == JCS_YCbCr)
            colorSpaceConversion = OFTrue;
        }
        break;
      case EDC_guess: // always use color space guess by IJG library
        colorSpaceGuess = OFTrue;
        if (cinfo->jpeg_color_space == JCS_YCbCr)
          colorSpaceConversion = OFTrue;
        break;
    }
    // decline color space conversion to RGB for signed pixel data,
    // because IJG can handle only unsigned
    if (colorSpaceConversion && isSigned)
      return EJ_UnsupportedColorConversion;

    // let the IJG library guess the JPEG color space
    if (colorSpaceGuess)
    {
      switch (cinfo->jpeg_color_space)
      {
        case JCS_GRAYSCALE:
          // cinfo->out_color_space = JCS_GRAYSCALE;
          decompressedColorModel = EPI_Monochrome2;
          break;
        case JCS_YCbCr: // enforce conversion YCbCr to RGB
          cinfo->out_color_space = JCS_RGB;
          decompressedColorModel = EPI_RGB;
          break;
        case JCS_RGB:
          // cinfo->out_color_space = JCS_RGB;
          decompressedColorModel = EPI_RGB;
          break;
        default:
          decompressedColorModel = EPI_Unknown;
          break;
      }
    }
    // set color space for decompression
    else if (colorSpaceConversion)
    {
      switch (cinfo->out_color_space)
      {
        case JCS_GRAYSCALE:
          decompressedColorModel = EPI_Monochrome2;
          break;
        case JCS_YCbCr: // enforce conversion YCbCr to RGB
          cinfo->jpeg_color_space = JCS_YCbCr;
          cinfo->out_color_space = JCS_RGB;
          decompressedColorModel = EPI_RGB;
          break;
        case JCS_RGB: // enforce conversion YCbCr to RGB
          cinfo->jpeg_color_space = JCS_YCbCr;
          decompressedColorModel = EPI_RGB;
          break;
        default:
          decompressedColorModel = EPI_Unknown;
          break;
      }
    }
    else
    {
      decompressedColorModel = EPI_Unknown;
      // prevent the library from performing any color space conversion
      cinfo->jpeg_color_space = JCS_UNKNOWN;
      cinfo->out_color_space = JCS_UNKNOWN;
    }
  }

  JSAMPARRAY buffer = NULL;
  int bufsize = 0;
  size_t rowsize = 0;

  if (suspension < 3)
  {
    if (FALSE == jpeg_start_decompress(cinfo))
    {
      suspension = 2;
      return EJ_Suspension;
    }
    bufsize = cinfo->output_width * cinfo->output_components; // number of JSAMPLEs per row
    rowsize = bufsize * sizeof(JSAMPLE); // number of bytes per row
    buffer = (*cinfo->mem->alloc_sarray)((j_common_ptr)cinfo, JPOOL_IMAGE, bufsize, 1);
    if (buffer == NULL) return EC_MemoryExhausted;
    jsampBuffer = buffer;
  }
  else
  {
    bufsize = cinfo->output_width * cinfo->output_components;
    rowsize = bufsize * sizeof(JSAMPLE);
    buffer = (JSAMPARRAY)jsampBuffer;
  }

  if (uncompressedFrameBufferSize < rowsize * cinfo->output_height) return EJ_IJG8_FrameBufferTooSmall;

  while (cinfo->output_scanline < cinfo->output_height)
  {
    if (0 == jpeg_read_scanlines(cinfo, buffer, 1))
    {
      suspension = 3;
      return EJ_Suspension;
    }
    memcpy(uncompressedFrameBuffer + (cinfo->output_scanline-1) * rowsize, *buffer, rowsize);
  }

  if (FALSE == jpeg_finish_decompress(cinfo))
  {
    suspension = 4;
    return EJ_Suspension;
  }

  return EC_Normal;
}

void DJDecompressIJG8Bit::emitMessage(int msg_level) const
{
  // This is how we map the message levels:
  // -1 - 0: Warning (could also be errors, but no way to find out)
  //  1    : Debug
  // Everything else: Trace (No point in splitting this further up)
  OFLogger::LogLevel level;

  switch (msg_level)
  {
  case -1:
  case 0:
    level = OFLogger::WARN_LOG_LEVEL;
    break;
  case 1:
    level = OFLogger::DEBUG_LOG_LEVEL;
    break;
  default:
    level = OFLogger::TRACE_LOG_LEVEL;
    break;
  }

  if (cinfo && DCM_dcmjpegGetLogger().isEnabledFor(level))
  {
    char buffer[JMSG_LENGTH_MAX];
    (*cinfo->err->format_message)((jpeg_common_struct *)cinfo, buffer); /* Create the message */
    DCM_dcmjpegGetLogger().forcedLog(level, buffer, __FILE__, __LINE__);
  }
}


/*
 * CVS/RCS Log
 * $Log: djdijg8.cc,v $
 * Revision 1.21  2010-10-14 13:14:21  joergr
 * Updated copyright header. Added reference to COPYRIGHT file.
 *
 * Revision 1.20  2010-06-25 09:15:19  uli
 * Fixed issues with compiling with HAVE_STD_STRING.
 *
 * Revision 1.19  2010-03-24 14:57:40  joergr
 * Added new options for the color space conversion during decompression based
 * on the color model that is "guessed" by the underlying JPEG library (IJG).
 *
 * Revision 1.18  2010-03-01 09:08:48  uli
 * Removed some unnecessary include directives in the headers.
 *
 * Revision 1.17  2010-02-22 11:39:54  uli
 * Remove some unneeded includes.
 *
 * Revision 1.16  2009-11-18 16:17:54  uli
 * Use more than just the INFO log level.
 *
 * Revision 1.15  2009-10-07 12:44:33  uli
 * Switched to logging mechanism provided by the "new" oflog module.
 *
 * Revision 1.14  2009-08-19 12:19:19  meichel
 * Unlike some other compilers, Sun Studio 11 on Solaris
 *   declares longjmp() in namespace std. Added code to handle this case.
 *
 * Revision 1.13  2006-08-16 16:30:21  meichel
 * Updated all code in module dcmjpeg to correctly compile when
 *   all standard C++ classes remain in namespace std.
 *
 * Revision 1.12  2005/12/08 15:43:38  meichel
 * Changed include path schema for all DCMTK header files
 *
 * Revision 1.11  2005/11/30 14:08:50  onken
 * Added check to decline automatic IJG color space conversion of signed pixel
 * data, because IJG lib only handles unsigned input for conversions.
 *
 * Revision 1.10  2005/11/14 17:09:39  meichel
 * Changed some function return types from int to ijg_boolean, to avoid
 *   compilation errors if the ijg_boolean type is ever changed.
 *
 * Revision 1.9  2004/05/07 12:18:45  meichel
 * Added explicit typecast to volatile variables, needed for MSVC
 *
 * Revision 1.8  2004/05/07 10:45:13  meichel
 * Added explicit typecast to volatile variables, needed for MSVC
 *
 * Revision 1.7  2004/05/05 14:10:24  joergr
 * Added explicit typecast to volatile variables to compiler with gcc 3.2.
 *
 * Revision 1.6  2004/05/05 11:50:06  meichel
 * Declared a few local variables as volatile that might otherwise
 *   be clobbered by longjmp.
 *
 * Revision 1.5  2003/10/13 13:25:49  meichel
 * Added workaround for name clash of typedef "boolean" in the IJG header files
 *   and the standard headers for Borland C++.
 *
 * Revision 1.4  2002/11/27 15:40:00  meichel
 * Adapted module dcmjpeg to use of new header file ofstdinc.h
 *
 * Revision 1.3  2001/12/18 09:48:57  meichel
 * Modified configure test for "const" support of the C compiler
 *   in order to avoid a macro recursion error on Sun CC 2.0.1
 *
 * Revision 1.2  2001/11/19 15:13:31  meichel
 * Introduced verbose mode in module dcmjpeg. If enabled, warning
 *   messages from the IJG library are printed on ofConsole, otherwise
 *   the library remains quiet.
 *
 * Revision 1.1  2001/11/13 15:58:29  meichel
 * Initial release of module dcmjpeg
 *
 *
 */
