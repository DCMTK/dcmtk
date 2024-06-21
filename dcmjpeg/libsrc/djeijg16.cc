/*
 *
 *  Copyright (C) 1997-2024, OFFIS e.V.
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
 *  Author:  Marco Eichelberg, Norbert Olges
 *
 *  Purpose: compression routines of the IJG JPEG library configured for 16 bits/sample.
 *
 */

#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmjpeg/djeijg16.h"
#include "dcmtk/dcmjpeg/djcparam.h"
#include "dcmtk/dcmdata/dcerror.h"
#include "dcmtk/ofstd/ofstdinc.h"
#include "dcmtk/ofstd/ofdiag.h"
#include <csetjmp>

// use 16K blocks for temporary storage of compressed JPEG data
#define IJGE16_BLOCKSIZE 16384

BEGIN_EXTERN_C
#define boolean ijg_boolean
#include "jpeglib16.h"
#include "jerror16.h"
#include "jpegint16.h"
#undef boolean

// disable any preprocessor magic the IJG library might be doing with the "const" keyword
#ifdef const
#undef const
#endif

// Solaris defines longjmp() in namespace std, other compilers don't...
using STD_NAMESPACE longjmp;
using STD_NAMESPACE jmp_buf;

#include DCMTK_DIAGNOSTIC_PUSH
#include DCMTK_DIAGNOSTIC_IGNORE_VISUAL_STUDIO_DECLSPEC_PADDING_WARNING
#include DCMTK_DIAGNOSTIC_IGNORE_VISUAL_STUDIO_DECLSPEC_PADDING_WARNING

// private error handler struct
struct DJEIJG16ErrorStruct
{
  // the standard IJG error handler object
  struct jpeg_error_mgr pub;

  // our jump buffer
  jmp_buf setjmp_buffer;

  // pointer to this
  DJCompressIJG16Bit *instance;
};

#include DCMTK_DIAGNOSTIC_POP

// callback forward declarations
void DJEIJG16ErrorExit(j_common_ptr);
void DJEIJG16EmitMessage(j_common_ptr cinfo, int msg_level);
void DJEIJG16initDestination(j_compress_ptr cinfo);
ijg_boolean DJEIJG16emptyOutputBuffer(j_compress_ptr cinfo);
void DJEIJG16termDestination(j_compress_ptr cinfo);

// helper methods to fix old-style casts warnings
static void OFjpeg_create_compress(j_compress_ptr cinfo)
{
  jpeg_create_compress(cinfo);
}

static void OF_ERREXIT1(j_compress_ptr cinfo, int code, int p1)
{
  ERREXIT1(cinfo, code, p1);
}

END_EXTERN_C


// error handler, executes longjmp

void DJEIJG16ErrorExit(j_common_ptr cinfo)
{
  DJEIJG16ErrorStruct *myerr = OFreinterpret_cast(DJEIJG16ErrorStruct*, cinfo->err);
  longjmp(myerr->setjmp_buffer, 1);
}

// message handler for warning messages and the like
void DJEIJG16EmitMessage(j_common_ptr cinfo, int msg_level)
{
  DJEIJG16ErrorStruct *myerr = OFreinterpret_cast(DJEIJG16ErrorStruct*, cinfo->err);
  myerr->instance->emitMessage(cinfo, msg_level);
}


// callbacks for compress-destination-manager

void DJEIJG16initDestination(j_compress_ptr cinfo)
{
  DJCompressIJG16Bit *encoder = OFreinterpret_cast(DJCompressIJG16Bit*, cinfo->client_data);
  encoder->initDestination(cinfo);
}

ijg_boolean DJEIJG16emptyOutputBuffer(j_compress_ptr cinfo)
{
  DJCompressIJG16Bit *encoder = OFreinterpret_cast(DJCompressIJG16Bit*, cinfo->client_data);
  return encoder->emptyOutputBuffer(cinfo);
}

void DJEIJG16termDestination(j_compress_ptr cinfo)
{
  DJCompressIJG16Bit *encoder = OFreinterpret_cast(DJCompressIJG16Bit*, cinfo->client_data);
  encoder->termDestination(cinfo);
}


// converts dcmtk color space to IJG color space

static J_COLOR_SPACE getJpegColorSpace(EP_Interpretation interpr)
{
  switch (interpr)
  {
    case EPI_Unknown :return JCS_UNKNOWN;
    case EPI_Monochrome1 : return JCS_GRAYSCALE;
    case EPI_Monochrome2 : return JCS_GRAYSCALE;
    case EPI_PaletteColor : return JCS_UNKNOWN;
    case EPI_RGB : return JCS_RGB;
    case EPI_HSV : return JCS_UNKNOWN;
    case EPI_ARGB : return JCS_RGB;
    case EPI_CMYK : return JCS_CMYK;
    case EPI_YBR_Full : return JCS_YCbCr;
    case EPI_YBR_Full_422 : return JCS_YCbCr;
    case EPI_YBR_Partial_422 : return JCS_YCbCr;
    default : return JCS_UNKNOWN;
  }
}

DJCompressIJG16Bit::DJCompressIJG16Bit(const DJCodecParameter& cp, EJ_Mode mode, int prediction, int ptrans)
: DJEncoder()
, cparam(&cp)
, psv(prediction)
, pt(ptrans)
, modeofOperation(mode)
, pixelDataList()
, bytesInLastBlock(0)
{
  assert(mode == EJM_lossless);
}

DJCompressIJG16Bit::~DJCompressIJG16Bit()
{
  cleanup();
}

OFCondition DJCompressIJG16Bit::encode(
    Uint16 /* columns */,
    Uint16 /* rows */,
    EP_Interpretation /* interpr */,
    Uint16 /* samplesPerPixel */,
    Uint8 * /* image_buffer */,
    Uint8 *& /* to */,
    Uint32 & /* length */)
{
  return EC_IllegalCall;
}

#include DCMTK_DIAGNOSTIC_PUSH
#include DCMTK_DIAGNOSTIC_IGNORE_VISUAL_STUDIO_OBJECT_DESTRUCTION_WARNING

OFCondition DJCompressIJG16Bit::encode(
  Uint16 columns,
  Uint16 rows,
  EP_Interpretation colorSpace,
  Uint16 samplesPerPixel,
  Uint16 * image_buffer,
  Uint8 * & to,
  Uint32 & length)
{

  struct dcmtk_jpeg16_compress_struct cinfo;
  struct DJEIJG16ErrorStruct jerr;
  cinfo.err = jpeg_std_error(&jerr.pub);
  jerr.instance = this;
  jerr.pub.error_exit = DJEIJG16ErrorExit;
  jerr.pub.emit_message = DJEIJG16EmitMessage;
  if (setjmp(jerr.setjmp_buffer))
  {
    // the IJG error handler will cause the following code to be executed
    char buffer[JMSG_LENGTH_MAX];
    (*cinfo.err->format_message)(OFreinterpret_cast(jpeg_common_struct*, &cinfo), buffer); /* Create the message */
    jpeg_destroy_compress(&cinfo);
    return makeOFCondition(OFM_dcmjpeg, EJCode_IJG16_Compression, OF_error, buffer);
  }
  OFjpeg_create_compress(&cinfo);

  // initialize client_data
  cinfo.client_data = this;

  // Specify destination manager
  jpeg_destination_mgr dest;
  dest.init_destination = DJEIJG16initDestination;
  dest.empty_output_buffer = DJEIJG16emptyOutputBuffer;
  dest.term_destination = DJEIJG16termDestination;
  cinfo.dest = &dest;

  cinfo.image_width = columns;
  cinfo.image_height = rows;
  cinfo.input_components = samplesPerPixel;
  cinfo.in_color_space = getJpegColorSpace(colorSpace);

  jpeg_set_defaults(&cinfo);

  if (cparam->getCompressionColorSpaceConversion() != ECC_lossyYCbCr)
  {
    // prevent IJG library from doing any color space conversion
    jpeg_set_colorspace (&cinfo, cinfo.in_color_space);
  }

  cinfo.optimize_coding = OFTrue; // must always be true for 16 bit compression

  switch (modeofOperation)
  {
    case EJM_lossless:
     // always disables any kind of color space conversion
     jpeg_simple_lossless(&cinfo,psv,pt);
     break;
    default:
     return makeOFCondition(OFM_dcmjpeg, EJCode_IJG16_Compression, OF_error, "JPEG with 16 bits/sample only allowed with lossless compression");
     /* break; */
  }

  cinfo.smoothing_factor = cparam->getSmoothingFactor();

  // initialize sampling factors
  if ((cinfo.jpeg_color_space == JCS_YCbCr) && (modeofOperation != EJM_lossless))
  {
    switch(cparam->getSampleFactors())
    {
      case ESS_444: /* 4:4:4 sampling (no subsampling) */
        cinfo.comp_info[0].h_samp_factor = 1;
        cinfo.comp_info[0].v_samp_factor = 1;
        break;
      case ESS_422: /* 4:2:2 sampling (horizontal subsampling of chroma components) */
        cinfo.comp_info[0].h_samp_factor = 2;
        cinfo.comp_info[0].v_samp_factor = 1;
        break;
      case ESS_411: /* 4:1:1 sampling (horizontal and vertical subsampling of chroma components) */
        cinfo.comp_info[0].h_samp_factor = 2;
        cinfo.comp_info[0].v_samp_factor = 2;
        break;
    }
  }
  else
  {
    // JPEG color space is not YCbCr, or we are using lossless compression.
    // Disable subsampling.
    cinfo.comp_info[0].h_samp_factor = 1;
    cinfo.comp_info[0].v_samp_factor = 1;
  }

  // all other components are set to 1x1
  for (int sfi=1; sfi< MAX_COMPONENTS; sfi++)
  {
    cinfo.comp_info[sfi].h_samp_factor = 1;
    cinfo.comp_info[sfi].v_samp_factor = 1;
  }

  JSAMPROW row_pointer[1];
  jpeg_start_compress(&cinfo,TRUE);
  int row_stride = columns * samplesPerPixel;
  while (cinfo.next_scanline < cinfo.image_height)
  {
    // JSAMPLE might be signed, typecast to avoid a warning
    row_pointer[0] = OFreinterpret_cast(JSAMPLE*, image_buffer + (cinfo.next_scanline * row_stride));
    jpeg_write_scanlines(&cinfo, row_pointer, 1);
  }
  jpeg_finish_compress(&cinfo);
  jpeg_destroy_compress(&cinfo);

  length = OFstatic_cast(Uint32, bytesInLastBlock);
  if (pixelDataList.size() > 1) length += OFstatic_cast(Uint32, (pixelDataList.size() - 1)*IJGE16_BLOCKSIZE);
  OFBool length_is_odd = (length % 2) > 0;
  if (length_is_odd) length++; // ensure even length

  to = new Uint8[length];
  if (to == NULL) return EC_MemoryExhausted;

  size_t offset=0;
  OFListIterator(unsigned char *) first = pixelDataList.begin();
  OFListIterator(unsigned char *) last = pixelDataList.end();
  OFListIterator(unsigned char *) shortBlock = last;
  --shortBlock;
  while (first != last)
  {
    if (first == shortBlock)
    {
      memcpy(to+offset, *first, bytesInLastBlock);
      offset += bytesInLastBlock;
    }
    else
    {
      memcpy(to+offset, *first, IJGE16_BLOCKSIZE);
      offset += IJGE16_BLOCKSIZE;
    }
    ++first;
  }
  if (length_is_odd) DcmJpegHelper::fixPadding(to, length);
  cleanup();

  return EC_Normal;
}

#include DCMTK_DIAGNOSTIC_POP

void DJCompressIJG16Bit::initDestination(dcmtk_jpeg16_compress_struct *cinfo)
{
  cleanup(); // erase old list of compressed blocks, if any

  unsigned char *newBlock = new unsigned char[IJGE16_BLOCKSIZE];
  if (newBlock)
  {
    pixelDataList.push_back(newBlock);
    cinfo->dest->next_output_byte = newBlock;
    cinfo->dest->free_in_buffer = IJGE16_BLOCKSIZE;
  }
  else
  {
    cinfo->dest->next_output_byte = NULL;
    cinfo->dest->free_in_buffer = 0;
  }
}

int DJCompressIJG16Bit::emptyOutputBuffer(dcmtk_jpeg16_compress_struct *cinfo)
{
  bytesInLastBlock = 0;
  unsigned char *newBlock = new unsigned char[IJGE16_BLOCKSIZE];
  if (newBlock)
  {
    pixelDataList.push_back(newBlock);
    cinfo->dest->next_output_byte = newBlock;
    cinfo->dest->free_in_buffer = IJGE16_BLOCKSIZE;
  }
  else
  {
    cinfo->dest->next_output_byte = NULL;
    cinfo->dest->free_in_buffer = 0;
    OF_ERREXIT1(cinfo, JERR_OUT_OF_MEMORY, 0xFF);
  }
  return TRUE;
}


void DJCompressIJG16Bit::termDestination(dcmtk_jpeg16_compress_struct *cinfo)
{
  bytesInLastBlock = IJGE16_BLOCKSIZE - cinfo->dest->free_in_buffer;
}

void DJCompressIJG16Bit::cleanup()
{
  OFListIterator(unsigned char *) first = pixelDataList.begin();
  OFListIterator(unsigned char *) last = pixelDataList.end();
  while (first != last)
  {
    delete[] *first;
    first = pixelDataList.erase(first);
  }
  bytesInLastBlock = 0;
}

void DJCompressIJG16Bit::emitMessage(void *arg, int msg_level) const
{
  jpeg_common_struct *cinfo = OFreinterpret_cast(jpeg_common_struct*, arg);

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
    (*cinfo->err->format_message)(cinfo, buffer); /* Create the message */
    DCM_dcmjpegLogger.forcedLog(level, buffer, __FILE__, __LINE__);
  }
}
