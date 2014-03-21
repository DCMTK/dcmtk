/*
 *
 *  Copyright (C) 2001-2014, OFFIS e.V.
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
 *  Purpose: decompression routines of the IJG JPEG library configured for 12 bits/sample.
 *
 */

#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmjpeg/djdijg12.h"
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
#include "jpeglib12.h"
#include "jerror12.h"
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
struct DJDIJG12ErrorStruct
{
  // the standard IJG error handler object
  struct jpeg_error_mgr pub;

  // our jump buffer
  jmp_buf setjmp_buffer;

  // pointer to this
  DJDecompressIJG12Bit *instance;
};

// private source manager struct
struct DJDIJG12SourceManagerStruct
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
void DJDIJG12ErrorExit(j_common_ptr);
void DJDIJG12EmitMessage(j_common_ptr cinfo, int msg_level);
void DJDIJG12initSource(j_decompress_ptr);
ijg_boolean DJDIJG12fillInputBuffer(j_decompress_ptr);
void DJDIJG12skipInputData(j_decompress_ptr, long);
void DJDIJG12termSource(j_decompress_ptr);

// helper method to fix old-style casts warnings
static void OFjpeg_create_decompress(j_decompress_ptr cinfo)
{
  jpeg_create_decompress(cinfo);
}

END_EXTERN_C


// error handler, executes longjmp
void DJDIJG12ErrorExit(j_common_ptr cinfo)
{
  DJDIJG12ErrorStruct *myerr = OFreinterpret_cast(DJDIJG12ErrorStruct*, cinfo->err);
  longjmp(myerr->setjmp_buffer, 1);
}

// message handler for warning messages and the like
void DJDIJG12EmitMessage(j_common_ptr cinfo, int msg_level)
{
  DJDIJG12ErrorStruct *myerr = OFreinterpret_cast(DJDIJG12ErrorStruct*, cinfo->err);
  myerr->instance->emitMessage(msg_level);
}


// methods for decompress-source-manager

void DJDIJG12initSource(j_decompress_ptr /* cinfo */)
{
}

ijg_boolean DJDIJG12fillInputBuffer(j_decompress_ptr cinfo)
{
  DJDIJG12SourceManagerStruct *src = OFreinterpret_cast(DJDIJG12SourceManagerStruct*, cinfo->src);

  // if we already have the next buffer, switch buffers
  if (src->next_buffer)
  {
    src->pub.next_input_byte    = src->next_buffer;
    src->pub.bytes_in_buffer    = OFstatic_cast(unsigned int, src->next_buffer_size);
    src->next_buffer            = NULL;
    src->next_buffer_size       = 0;

    // The suspension was caused by DJDIJG12skipInputData iff src->skip_bytes > 0.
    // In this case we must skip the remaining number of bytes here.
    if (src->skip_bytes > 0)
    {
      if (src->pub.bytes_in_buffer < OFstatic_cast(unsigned long, src->skip_bytes))
      {
        src->skip_bytes            -= OFstatic_cast(Uint32, src->pub.bytes_in_buffer);
        src->pub.next_input_byte   += src->pub.bytes_in_buffer;
        src->pub.bytes_in_buffer    = 0;
        // cause a suspension return
        return FALSE;
      }
      else
      {
        src->pub.bytes_in_buffer   -= OFstatic_cast(unsigned int, src->skip_bytes);
        src->pub.next_input_byte   += src->skip_bytes;
        src->skip_bytes             = 0;
      }
    }
    return TRUE;
  }

  // otherwise cause a suspension return
  return FALSE;
}

void DJDIJG12skipInputData(
  j_decompress_ptr cinfo,
  long num_bytes)
{
  DJDIJG12SourceManagerStruct *src = OFreinterpret_cast(DJDIJG12SourceManagerStruct*, cinfo->src);

  if (src->pub.bytes_in_buffer < OFstatic_cast(size_t, num_bytes))
  {
    src->skip_bytes             = num_bytes - OFstatic_cast(Uint32, src->pub.bytes_in_buffer);
    src->pub.next_input_byte   += src->pub.bytes_in_buffer;
    src->pub.bytes_in_buffer    = 0; // causes a suspension return
  }
  else
  {
    src->pub.bytes_in_buffer   -= OFstatic_cast(unsigned int, num_bytes);
    src->pub.next_input_byte   += num_bytes;
    src->skip_bytes             = 0;
  }
}

void DJDIJG12termSource(j_decompress_ptr /* cinfo */)
{
}


DJDecompressIJG12Bit::DJDecompressIJG12Bit(const DJCodecParameter& cp, OFBool isYBR)
: DJDecoder()
, cparam(&cp)
, cinfo(NULL)
, suspension(0)
, jsampBuffer(NULL)
, dicomPhotometricInterpretationIsYCbCr(isYBR)
, decompressedColorModel(EPI_Unknown)
{
}

DJDecompressIJG12Bit::~DJDecompressIJG12Bit()
{
  cleanup();
}


OFCondition DJDecompressIJG12Bit::init()
{
  suspension = 0;
  decompressedColorModel = EPI_Unknown;
  cleanup(); // prevent double initialization

  cinfo = new jpeg_decompress_struct();
  if (cinfo)
  {
    volatile DJDIJG12SourceManagerStruct *src = NULL;
    volatile DJDIJG12ErrorStruct *jerr = new DJDIJG12ErrorStruct();
    if (jerr)
    {
      src = new DJDIJG12SourceManagerStruct();
      if (src)
      {
        // Specify the source of the compressed data
        src->pub.init_source       = DJDIJG12initSource;
        src->pub.fill_input_buffer = DJDIJG12fillInputBuffer;
        src->pub.skip_input_data   = DJDIJG12skipInputData;
        src->pub.resync_to_restart = jpeg_resync_to_restart;
        src->pub.term_source       = DJDIJG12termSource;
        src->pub.bytes_in_buffer   = 0;
        src->pub.next_input_byte   = NULL;
        src->skip_bytes             = 0;
        src->next_buffer            = NULL;
        src->next_buffer_size       = 0;
      }
      else
      {
        delete OFconst_cast(DJDIJG12ErrorStruct *, jerr);
        delete cinfo;
        cinfo = NULL;
        return EC_MemoryExhausted;
      }
      cinfo->err = jpeg_std_error(& OFconst_cast(DJDIJG12ErrorStruct *, jerr)->pub);
      jerr->instance = this;
      jerr->pub.error_exit = DJDIJG12ErrorExit;
      jerr->pub.emit_message = DJDIJG12EmitMessage;
      if (setjmp(OFconst_cast(DJDIJG12ErrorStruct *, jerr)->setjmp_buffer))
      {
        // the IJG error handler will cause the following code to be executed
        char buffer[JMSG_LENGTH_MAX];
        (*cinfo->err->format_message)(OFreinterpret_cast(jpeg_common_struct*, cinfo), buffer); /* Create the message */
        cleanup();
        delete OFconst_cast(DJDIJG12SourceManagerStruct *, src);
        return makeOFCondition(OFM_dcmjpeg, EJCode_IJG12_Decompression, OF_error, buffer);
      }
    }
    else
    {
      delete cinfo;
      cinfo = NULL;
      return EC_MemoryExhausted;
    }
    OFjpeg_create_decompress(cinfo);
    cinfo->src = &OFconst_cast(DJDIJG12SourceManagerStruct *, src)->pub;
  } else return EC_MemoryExhausted;

  // everything OK
  return EC_Normal;
}


void DJDecompressIJG12Bit::cleanup()
{
  if (cinfo)
  {
    jpeg_destroy_decompress(cinfo);
    delete OFreinterpret_cast(DJDIJG12ErrorStruct*, cinfo->err);
    delete OFreinterpret_cast(DJDIJG12SourceManagerStruct*, cinfo->src);
    delete cinfo;
    cinfo = NULL;
  }
}


OFCondition DJDecompressIJG12Bit::decode(
  Uint8 *compressedFrameBuffer,
  Uint32 compressedFrameBufferSize,
  Uint8 *uncompressedFrameBuffer,
  Uint32 uncompressedFrameBufferSize,
  OFBool isSigned)
{

  if (cinfo==NULL || compressedFrameBuffer==NULL || uncompressedFrameBuffer==NULL) return EC_IllegalCall;

  if (setjmp(OFreinterpret_cast(DJDIJG12ErrorStruct*, cinfo->err)->setjmp_buffer))
  {
    // the IJG error handler will cause the following code to be executed
    char buffer[JMSG_LENGTH_MAX];
    (*cinfo->err->format_message)(OFreinterpret_cast(jpeg_common_struct*, cinfo), buffer); /* Create the message */
    cleanup();
    return makeOFCondition(OFM_dcmjpeg, EJCode_IJG12_Decompression, OF_error, buffer);
  }

  // feed compressed buffer into cinfo structure.
  // The buffer will be activated by the next call to DJDIJG12fillInputBuffer.
  DJDIJG12SourceManagerStruct *src = OFreinterpret_cast(DJDIJG12SourceManagerStruct*, cinfo->src);
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
    buffer = (*cinfo->mem->alloc_sarray)(OFreinterpret_cast(j_common_ptr, cinfo), JPOOL_IMAGE, bufsize, 1);
    if (buffer == NULL) return EC_MemoryExhausted;
    jsampBuffer = buffer;
  }
  else
  {
    bufsize = cinfo->output_width * cinfo->output_components;
    rowsize = bufsize * sizeof(JSAMPLE);
    buffer = OFreinterpret_cast(JSAMPARRAY, jsampBuffer);
  }

  if (uncompressedFrameBufferSize < rowsize * cinfo->output_height) return EJ_IJG12_FrameBufferTooSmall;

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

void DJDecompressIJG12Bit::emitMessage(int msg_level) const
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

  if (cinfo && DCM_dcmjpegLogger.isEnabledFor(level))
  {
    char buffer[JMSG_LENGTH_MAX];
    (*cinfo->err->format_message)(OFreinterpret_cast(jpeg_common_struct*, cinfo), buffer); /* Create the message */
    DCM_dcmjpegLogger.forcedLog(level, buffer, __FILE__, __LINE__);
  }
}
