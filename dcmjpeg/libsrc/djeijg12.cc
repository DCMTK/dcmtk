/*
 *
 *  Copyright (C) 1997-2014, OFFIS e.V.
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
 *  Purpose: compression routines of the IJG JPEG library configured for 12 bits/sample. 
 *
 */

#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmjpeg/djeijg12.h"
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

// use 16K blocks for temporary storage of compressed JPEG data
#define IJGE12_BLOCKSIZE 16384

BEGIN_EXTERN_C
#define boolean ijg_boolean
#include "jpeglib12.h"
#include "jerror12.h"
#include "jpegint12.h"
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
struct DJEIJG12ErrorStruct
{
  // the standard IJG error handler object
  struct jpeg_error_mgr pub;

  // our jump buffer
  jmp_buf setjmp_buffer;

  // pointer to this
  DJCompressIJG12Bit *instance;
};

// callback forward declarations
void DJEIJG12ErrorExit(j_common_ptr);
void DJEIJG12EmitMessage(j_common_ptr cinfo, int msg_level);
void DJEIJG12initDestination(j_compress_ptr cinfo);
ijg_boolean DJEIJG12emptyOutputBuffer(j_compress_ptr cinfo);
void DJEIJG12termDestination(j_compress_ptr cinfo);

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

void DJEIJG12ErrorExit(j_common_ptr cinfo)
{
  DJEIJG12ErrorStruct *myerr = OFreinterpret_cast(DJEIJG12ErrorStruct*, cinfo->err);
  longjmp(myerr->setjmp_buffer, 1);
}

// message handler for warning messages and the like
void DJEIJG12EmitMessage(j_common_ptr cinfo, int msg_level)
{
  DJEIJG12ErrorStruct *myerr = OFreinterpret_cast(DJEIJG12ErrorStruct*, cinfo->err);
  myerr->instance->emitMessage(cinfo, msg_level);
}

// callbacks for compress-destination-manager

void DJEIJG12initDestination(j_compress_ptr cinfo)
{
  DJCompressIJG12Bit *encoder = OFreinterpret_cast(DJCompressIJG12Bit*, cinfo->client_data);
  encoder->initDestination(cinfo);
}

ijg_boolean DJEIJG12emptyOutputBuffer(j_compress_ptr cinfo)
{
  DJCompressIJG12Bit *encoder = OFreinterpret_cast(DJCompressIJG12Bit*, cinfo->client_data);
  return encoder->emptyOutputBuffer(cinfo);
}

void DJEIJG12termDestination(j_compress_ptr cinfo)
{
  DJCompressIJG12Bit *encoder = OFreinterpret_cast(DJCompressIJG12Bit*, cinfo->client_data);
  encoder->termDestination(cinfo);
}

/*
 * jpeg_simple_spectral_selection() creates a scan script
 * for progressive JPEG with spectral selection only,
 * similar to jpeg_simple_progression() for full progression.
 * The scan sequence for YCbCr is as proposed in the IJG documentation.
 * The scan sequence for all other color models is somewhat arbitrary.
 */
static void jpeg_simple_spectral_selection(j_compress_ptr cinfo)
{
  int ncomps = cinfo->num_components;
  jpeg_scan_info *scanptr = NULL;
  int nscans = 0;

  /* Safety check to ensure start_compress not called yet. */
  if (cinfo->global_state != CSTATE_START)
  {
    OF_ERREXIT1(cinfo, JERR_BAD_STATE, cinfo->global_state);
  }

  if (ncomps == 3 && cinfo->jpeg_color_space == JCS_YCbCr) nscans = 7;
  else nscans = 1 + 2 * ncomps;	/* 1 DC scan; 2 AC scans per component */

  /* Allocate space for script.
   * We need to put it in the permanent pool in case the application performs
   * multiple compressions without changing the settings.  To avoid a memory
   * leak if jpeg_simple_spectral_selection is called repeatedly for the same JPEG
   * object, we try to re-use previously allocated space, and we allocate
   * enough space to handle YCbCr even if initially asked for grayscale.
   */
  if (cinfo->script_space == NULL || cinfo->script_space_size < nscans)
  {
    cinfo->script_space_size = nscans > 7 ? nscans : 7;
    cinfo->script_space = OFreinterpret_cast(jpeg_scan_info*,
      (*cinfo->mem->alloc_small) (OFreinterpret_cast(j_common_ptr, cinfo),
      JPOOL_PERMANENT, cinfo->script_space_size * sizeof(jpeg_scan_info)));
  }
  scanptr = cinfo->script_space;
  cinfo->scan_info = scanptr;
  cinfo->num_scans = nscans;

  if (ncomps == 3 && cinfo->jpeg_color_space == JCS_YCbCr)
  {
    /* Custom script for YCbCr color images. */

    // Interleaved DC scan for Y,Cb,Cr:
    scanptr[0].component_index[0] = 0;
    scanptr[0].component_index[1] = 1;
    scanptr[0].component_index[2] = 2;
    scanptr[0].comps_in_scan = 3;
    scanptr[0].Ss = 0;
    scanptr[0].Se = 0;
    scanptr[0].Ah = 0;
    scanptr[0].Al = 0;
    
    // AC scans
    // First two Y AC coefficients
    scanptr[1].component_index[0] = 0;
    scanptr[1].comps_in_scan = 1;
    scanptr[1].Ss = 1;
    scanptr[1].Se = 2;
    scanptr[1].Ah = 0;
    scanptr[1].Al = 0;
    
    // Three more
    scanptr[2].component_index[0] = 0;
    scanptr[2].comps_in_scan = 1;
    scanptr[2].Ss = 3;
    scanptr[2].Se = 5;
    scanptr[2].Ah = 0;
    scanptr[2].Al = 0;
    
    // All AC coefficients for Cb
    scanptr[3].component_index[0] = 1;
    scanptr[3].comps_in_scan = 1;
    scanptr[3].Ss = 1;
    scanptr[3].Se = 63;
    scanptr[3].Ah = 0;
    scanptr[3].Al = 0;
    
    // All AC coefficients for Cr
    scanptr[4].component_index[0] = 2;
    scanptr[4].comps_in_scan = 1;
    scanptr[4].Ss = 1;
    scanptr[4].Se = 63;
    scanptr[4].Ah = 0;
    scanptr[4].Al = 0;
    
    // More Y coefficients
    scanptr[5].component_index[0] = 0;
    scanptr[5].comps_in_scan = 1;
    scanptr[5].Ss = 6;
    scanptr[5].Se = 9;
    scanptr[5].Ah = 0;
    scanptr[5].Al = 0;
    
    // Remaining Y coefficients
    scanptr[6].component_index[0] = 0;
    scanptr[6].comps_in_scan = 1;
    scanptr[6].Ss = 10;
    scanptr[6].Se = 63;
    scanptr[6].Ah = 0;
    scanptr[6].Al = 0;
  }
  else
  {
    /* All-purpose script for other color spaces. */
    int j=0;
    
    // Interleaved DC scan for all components
    for (j=0; j<ncomps; j++) scanptr[0].component_index[j] = j;
    scanptr[0].comps_in_scan = ncomps;
    scanptr[0].Ss = 0;
    scanptr[0].Se = 0;
    scanptr[0].Ah = 0;
    scanptr[0].Al = 0;

    // first AC scan for each component
    for (j=0; j<ncomps; j++) 
    {
      scanptr[j+1].component_index[0] = j;
      scanptr[j+1].comps_in_scan = 1;
      scanptr[j+1].Ss = 1;
      scanptr[j+1].Se = 5;
      scanptr[j+1].Ah = 0;
      scanptr[j+1].Al = 0;
    }

    // second AC scan for each component
    for (j=0; j<ncomps; j++) 
    {
      scanptr[j+ncomps+1].component_index[0] = j;
      scanptr[j+ncomps+1].comps_in_scan = 1;
      scanptr[j+ncomps+1].Ss = 6;
      scanptr[j+ncomps+1].Se = 63;
      scanptr[j+ncomps+1].Ah = 0;
      scanptr[j+ncomps+1].Al = 0;
    }
  }
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



DJCompressIJG12Bit::DJCompressIJG12Bit(const DJCodecParameter& cp, EJ_Mode mode, Uint8 theQuality)
: DJEncoder()
, cparam(&cp)
, quality(theQuality)
, psv(1)
, pt(0)
, modeofOperation(mode)
, pixelDataList()
, bytesInLastBlock(0)
{
  assert((mode != EJM_lossless) && (mode != EJM_baseline));
}

DJCompressIJG12Bit::DJCompressIJG12Bit(const DJCodecParameter& cp, EJ_Mode mode, int prediction, int ptrans)
: DJEncoder()
, cparam(&cp)
, quality(90)
, psv(prediction)
, pt(ptrans)
, modeofOperation(mode)
, pixelDataList()
, bytesInLastBlock(0)
{
  assert(mode == EJM_lossless);
}

DJCompressIJG12Bit::~DJCompressIJG12Bit()
{
  cleanup();
}

OFCondition DJCompressIJG12Bit::encode(
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

OFCondition DJCompressIJG12Bit::encode( 
  Uint16 columns,
  Uint16 rows,
  EP_Interpretation colorSpace,
  Uint16 samplesPerPixel,
  Uint16 * image_buffer,
  Uint8 * & to,
  Uint32 & length)
{

  struct jpeg_compress_struct cinfo;
  struct DJEIJG12ErrorStruct jerr;
  cinfo.err = jpeg_std_error(&jerr.pub);
  jerr.instance = this;
  jerr.pub.error_exit = DJEIJG12ErrorExit;
  jerr.pub.emit_message = DJEIJG12EmitMessage;
  if (setjmp(jerr.setjmp_buffer))
  {
    // the IJG error handler will cause the following code to be executed
    char buffer[JMSG_LENGTH_MAX];    
    (*cinfo.err->format_message)(OFreinterpret_cast(jpeg_common_struct*, &cinfo), buffer); /* Create the message */
    jpeg_destroy_compress(&cinfo);
    return makeOFCondition(OFM_dcmjpeg, EJCode_IJG12_Compression, OF_error, buffer);
  }
  OFjpeg_create_compress(&cinfo);

  // initialize client_data
  cinfo.client_data = this;

  // Specify destination manager
  jpeg_destination_mgr dest;
  dest.init_destination = DJEIJG12initDestination;
  dest.empty_output_buffer = DJEIJG12emptyOutputBuffer;
  dest.term_destination = DJEIJG12termDestination;
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

  cinfo.optimize_coding = OFTrue; // must always be true for 12 bit compression

  switch (modeofOperation)
  {
    case EJM_baseline: // baseline only supports 8 bits/sample. Assume sequential.
    case EJM_sequential:
      jpeg_set_quality(&cinfo, quality, 0);
      break;
    case EJM_spectralSelection:
      jpeg_set_quality(&cinfo, quality, 0);
      jpeg_simple_spectral_selection(&cinfo);
      break;
    case EJM_progressive:
      jpeg_set_quality(&cinfo, quality, 0);
      jpeg_simple_progression(&cinfo);
      break;
    case EJM_lossless:
     // always disables any kind of color space conversion
     jpeg_simple_lossless(&cinfo,psv,pt);
     break;
  }
  
  cinfo.smoothing_factor = cparam->getSmoothingFactor();

  // initialize sampling factors
  if (cinfo.jpeg_color_space == JCS_YCbCr)
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
    // JPEG color space is not YCbCr, disable subsampling.
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
    // JSAMPLE is signed, typecast to avoid a warning
    row_pointer[0] = OFreinterpret_cast(JSAMPLE*, image_buffer + (cinfo.next_scanline * row_stride));
    jpeg_write_scanlines(&cinfo, row_pointer, 1);
  }
  jpeg_finish_compress(&cinfo);
  jpeg_destroy_compress(&cinfo);

  length = OFstatic_cast(Uint32, bytesInLastBlock);
  if (pixelDataList.size() > 1) length += OFstatic_cast(Uint32, (pixelDataList.size() - 1)*IJGE12_BLOCKSIZE);
  if (length % 2) length++; // ensure even length    

  to = new Uint8[length];
  if (to == NULL) return EC_MemoryExhausted;
  if (length > 0) to[length-1] = 0;    

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
      memcpy(to+offset, *first, IJGE12_BLOCKSIZE);
      offset += IJGE12_BLOCKSIZE;
    }
    ++first;
  }
  cleanup();

  return EC_Normal;
}

void DJCompressIJG12Bit::initDestination(jpeg_compress_struct *cinfo)
{
  cleanup(); // erase old list of compressed blocks, if any

  unsigned char *newBlock = new unsigned char[IJGE12_BLOCKSIZE];
  if (newBlock)
  {
    pixelDataList.push_back(newBlock);
    cinfo->dest->next_output_byte = newBlock;
    cinfo->dest->free_in_buffer = IJGE12_BLOCKSIZE;    
  }
  else
  {
    cinfo->dest->next_output_byte = NULL;
    cinfo->dest->free_in_buffer = 0;    
  }
}

int DJCompressIJG12Bit::emptyOutputBuffer(jpeg_compress_struct *cinfo)
{
  bytesInLastBlock = 0;
  unsigned char *newBlock = new unsigned char[IJGE12_BLOCKSIZE];
  if (newBlock)
  {
    pixelDataList.push_back(newBlock);
    cinfo->dest->next_output_byte = newBlock;
    cinfo->dest->free_in_buffer = IJGE12_BLOCKSIZE;    
  }
  else
  {
    cinfo->dest->next_output_byte = NULL;
    cinfo->dest->free_in_buffer = 0;
    OF_ERREXIT1(cinfo, JERR_OUT_OF_MEMORY, 0xFF);
  }
  return TRUE;
}


void DJCompressIJG12Bit::termDestination(jpeg_compress_struct *cinfo)
{
  bytesInLastBlock = IJGE12_BLOCKSIZE - cinfo->dest->free_in_buffer;
}

void DJCompressIJG12Bit::cleanup()
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

void DJCompressIJG12Bit::emitMessage(void *arg, int msg_level) const
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
