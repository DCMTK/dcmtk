/*
 *
 *  Copyright (C) 1997-2010, OFFIS e.V.
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
 *  Purpose: compression routines of the IJG JPEG library configured for 8 bits/sample. 
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:14:22 $
 *  CVS/RCS Revision: $Revision: 1.15 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmjpeg/djeijg8.h"
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
#define IJGE8_BLOCKSIZE 16384

BEGIN_EXTERN_C
#define boolean ijg_boolean
#include "jpeglib8.h"
#include "jerror8.h"
#include "jpegint8.h"
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
struct DJEIJG8ErrorStruct
{
  // the standard IJG error handler object
  struct jpeg_error_mgr pub;

  // our jump buffer
  jmp_buf setjmp_buffer;

  // pointer to this
  DJCompressIJG8Bit *instance;
};

// callback forward declarations
void DJEIJG8ErrorExit(j_common_ptr);
void DJEIJG8EmitMessage(j_common_ptr cinfo, int msg_level);
void DJEIJG8initDestination(j_compress_ptr cinfo);
ijg_boolean DJEIJG8emptyOutputBuffer(j_compress_ptr cinfo);
void DJEIJG8termDestination(j_compress_ptr cinfo);

END_EXTERN_C


// error handler, executes longjmp

void DJEIJG8ErrorExit(j_common_ptr cinfo)
{
  DJEIJG8ErrorStruct *myerr = (DJEIJG8ErrorStruct *)cinfo->err;
  longjmp(myerr->setjmp_buffer, 1);
}

// message handler for warning messages and the like
void DJEIJG8EmitMessage(j_common_ptr cinfo, int msg_level)
{
  DJEIJG8ErrorStruct *myerr = (DJEIJG8ErrorStruct *)cinfo->err;
  myerr->instance->emitMessage(cinfo, msg_level);
}


// callbacks for compress-destination-manager

void DJEIJG8initDestination(j_compress_ptr cinfo)
{
  DJCompressIJG8Bit *encoder = (DJCompressIJG8Bit *)cinfo->client_data;
  encoder->initDestination(cinfo);
}

ijg_boolean DJEIJG8emptyOutputBuffer(j_compress_ptr cinfo)
{
  DJCompressIJG8Bit *encoder = (DJCompressIJG8Bit *)cinfo->client_data;
  return encoder->emptyOutputBuffer(cinfo);
}

void DJEIJG8termDestination(j_compress_ptr cinfo)
{
  DJCompressIJG8Bit *encoder = (DJCompressIJG8Bit *)cinfo->client_data;
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
  if (cinfo->global_state != CSTATE_START) ERREXIT1(cinfo, JERR_BAD_STATE, cinfo->global_state);

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
    cinfo->script_space = (jpeg_scan_info *)
      (*cinfo->mem->alloc_small) ((j_common_ptr) cinfo, 
      JPOOL_PERMANENT, cinfo->script_space_size * sizeof(jpeg_scan_info));
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



DJCompressIJG8Bit::DJCompressIJG8Bit(const DJCodecParameter& cp, EJ_Mode mode, Uint8 theQuality)
: DJEncoder()
, cparam(&cp)
, quality(theQuality)
, psv(1)
, pt(0)
, modeofOperation(mode)
, pixelDataList()
, bytesInLastBlock(0)
{
  assert(mode != EJM_lossless);
}

DJCompressIJG8Bit::DJCompressIJG8Bit(const DJCodecParameter& cp, EJ_Mode mode, int prediction, int ptrans)
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

DJCompressIJG8Bit::~DJCompressIJG8Bit()
{
  cleanup();
}

OFCondition DJCompressIJG8Bit::encode(
    Uint16 /* columns */,
    Uint16 /* rows */,
    EP_Interpretation /* interpr */,
    Uint16 /* samplesPerPixel */,
    Uint16 * /* image_buffer */,
    Uint8 *& /* to */,
    Uint32 & /* length */)
{
  return EC_IllegalCall;
}

OFCondition DJCompressIJG8Bit::encode( 
  Uint16 columns,
  Uint16 rows,
  EP_Interpretation colorSpace,
  Uint16 samplesPerPixel,
  Uint8 * image_buffer,
  Uint8 * & to,
  Uint32 & length)
{

  struct jpeg_compress_struct cinfo;
  struct DJEIJG8ErrorStruct jerr;
  cinfo.err = jpeg_std_error(&jerr.pub);
  jerr.instance = this;
  jerr.pub.error_exit = DJEIJG8ErrorExit;
  jerr.pub.emit_message = DJEIJG8EmitMessage;
  if (setjmp(jerr.setjmp_buffer))
  {
    // the IJG error handler will cause the following code to be executed
    char buffer[JMSG_LENGTH_MAX];    
    (*cinfo.err->format_message)((jpeg_common_struct *)(&cinfo), buffer); /* Create the message */
    jpeg_destroy_compress(&cinfo);
    return makeOFCondition(OFM_dcmjpeg, EJCode_IJG8_Compression, OF_error, buffer);
  }
  jpeg_create_compress(&cinfo);

  // initialize client_data
  cinfo.client_data = (void *)this;

  // Specify destination manager
  jpeg_destination_mgr dest;
  dest.init_destination = DJEIJG8initDestination;
  dest.empty_output_buffer = DJEIJG8emptyOutputBuffer;
  dest.term_destination = DJEIJG8termDestination;
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

  cinfo.optimize_coding = cparam->getOptimizeHuffmanCoding();

  switch (modeofOperation)
  {
    case EJM_baseline:
      jpeg_set_quality(&cinfo, quality, 1);
      break;
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
    row_pointer[0] = & image_buffer[cinfo.next_scanline * row_stride];
    jpeg_write_scanlines(&cinfo, row_pointer, 1);
  }
  jpeg_finish_compress(&cinfo);
  jpeg_destroy_compress(&cinfo);

  length = bytesInLastBlock;
  if (pixelDataList.size() > 1) length += (pixelDataList.size() - 1)*IJGE8_BLOCKSIZE;
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
      memcpy(to+offset, *first, IJGE8_BLOCKSIZE);
      offset += IJGE8_BLOCKSIZE;
    }
    ++first;
  }
  cleanup();
  return EC_Normal;
}

void DJCompressIJG8Bit::initDestination(jpeg_compress_struct *cinfo)
{
  cleanup(); // erase old list of compressed blocks, if any

  unsigned char *newBlock = new unsigned char[IJGE8_BLOCKSIZE];
  if (newBlock)
  {
    pixelDataList.push_back(newBlock);
    cinfo->dest->next_output_byte = newBlock;
    cinfo->dest->free_in_buffer = IJGE8_BLOCKSIZE;    
  }
  else
  {
    cinfo->dest->next_output_byte = NULL;
    cinfo->dest->free_in_buffer = 0;    
  }
}

int DJCompressIJG8Bit::emptyOutputBuffer(jpeg_compress_struct *cinfo)
{
  bytesInLastBlock = 0;
  unsigned char *newBlock = new unsigned char[IJGE8_BLOCKSIZE];
  if (newBlock)
  {
    pixelDataList.push_back(newBlock);
    cinfo->dest->next_output_byte = newBlock;
    cinfo->dest->free_in_buffer = IJGE8_BLOCKSIZE;    
  }
  else
  {
    // should never happen
    cinfo->dest->next_output_byte = NULL;
    cinfo->dest->free_in_buffer = 0;    
    ERREXIT1(cinfo, JERR_OUT_OF_MEMORY, 0xFF);
  }
  return TRUE;
}


void DJCompressIJG8Bit::termDestination(jpeg_compress_struct *cinfo)
{
  bytesInLastBlock = IJGE8_BLOCKSIZE - cinfo->dest->free_in_buffer;
}

void DJCompressIJG8Bit::cleanup()
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

void DJCompressIJG8Bit::emitMessage(void *arg, int msg_level) const
{
  jpeg_common_struct *cinfo = (jpeg_common_struct *)arg;

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
    (*cinfo->err->format_message)(cinfo, buffer); /* Create the message */
    DCM_dcmjpegGetLogger().forcedLog(level, buffer, __FILE__, __LINE__);
  }
}


/*
 * CVS/RCS Log
 * $Log: djeijg8.cc,v $
 * Revision 1.15  2010-10-14 13:14:22  joergr
 * Updated copyright header. Added reference to COPYRIGHT file.
 *
 * Revision 1.14  2010-06-25 09:15:19  uli
 * Fixed issues with compiling with HAVE_STD_STRING.
 *
 * Revision 1.13  2010-03-01 09:08:48  uli
 * Removed some unnecessary include directives in the headers.
 *
 * Revision 1.12  2010-02-22 11:39:54  uli
 * Remove some unneeded includes.
 *
 * Revision 1.11  2009-11-18 16:17:54  uli
 * Use more than just the INFO log level.
 *
 * Revision 1.10  2009-10-07 12:44:34  uli
 * Switched to logging mechanism provided by the "new" oflog module.
 *
 * Revision 1.9  2009-08-19 12:19:20  meichel
 * Unlike some other compilers, Sun Studio 11 on Solaris
 *   declares longjmp() in namespace std. Added code to handle this case.
 *
 * Revision 1.8  2006-08-16 16:30:21  meichel
 * Updated all code in module dcmjpeg to correctly compile when
 *   all standard C++ classes remain in namespace std.
 *
 * Revision 1.7  2005/12/08 15:43:41  meichel
 * Changed include path schema for all DCMTK header files
 *
 * Revision 1.6  2005/11/14 17:09:39  meichel
 * Changed some function return types from int to ijg_boolean, to avoid
 *   compilation errors if the ijg_boolean type is ever changed.
 *
 * Revision 1.5  2003/10/13 13:25:49  meichel
 * Added workaround for name clash of typedef "boolean" in the IJG header files
 *   and the standard headers for Borland C++.
 *
 * Revision 1.4  2002/11/27 15:40:01  meichel
 * Adapted module dcmjpeg to use of new header file ofstdinc.h
 *
 * Revision 1.3  2001/12/18 09:48:59  meichel
 * Modified configure test for "const" support of the C compiler
 *   in order to avoid a macro recursion error on Sun CC 2.0.1
 *
 * Revision 1.2  2001/11/19 15:13:32  meichel
 * Introduced verbose mode in module dcmjpeg. If enabled, warning
 *   messages from the IJG library are printed on ofConsole, otherwise
 *   the library remains quiet.
 *
 * Revision 1.1  2001/11/13 15:58:30  meichel
 * Initial release of module dcmjpeg
 *
 *
 */
