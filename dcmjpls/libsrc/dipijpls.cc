/*
 *
 *  Copyright (C) 2024, OFFIS e.V.
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
 *  Module:  dcmjpls
 *
 *  Author:  Marco Eichelberg
 *
 *  Purpose: JPEG-LS interface for pluggable image formats
 *
 */

#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmjpls/dipijpls.h"
#include "dcmtk/dcmdata/dctypes.h"
#include "dcmtk/dcmimgle/diimage.h" /* for class DiImage */
#include "dcmtk/dcmimgle/dipixel.h" /* for class DiPixel */
#include "dcmtk/dcmjpls/djlsutil.h" /* for DCMJPLS_WARN() */
#include "djerror.h"                /* for class DJLSError */
#include "intrface.h"               /* JPEG-LS library (CharLS) includes */

/*---------------------------------*
 *  constructors and mode setting  *
 *---------------------------------*/

DiJPLSPlugin::DiJPLSPlugin()
: DiPluginFormat()
, trueLosslessMode_(OFFalse)
, force8BitMode_(OFFalse)
{
}

DiJPLSPlugin::~DiJPLSPlugin()
{
}

void DiJPLSPlugin::setTrueLosslessMode(OFBool mode)
{
  trueLosslessMode_ = mode;
}

void DiJPLSPlugin::setForce8BitMode(OFBool mode)
{
  force8BitMode_ = mode;
}

/*-----------------------*
 *  main implementation  *
 *-----------------------*/

static int createAndWriteJPLS(
  int height,
  int width,
  int bitspersample,
  int components,
  size_t data_size,
  const void *data,
  FILE *stream)
{
    // Set up the information structure for CharLS
    JlsParameters jls_params;
    memset((char *) &jls_params, 0, sizeof(jls_params));
    jls_params.height = height;
    jls_params.width = width;
    jls_params.allowedlossyerror = 0;
    jls_params.outputBgr = false;
    jls_params.bitspersample = bitspersample;
    jls_params.colorTransform = 0;
    jls_params.components = components;

    // use default values for custom JPEG-LS parameters
    jls_params.custom.T1 = 0;
    jls_params.custom.T2 = 0;
    jls_params.custom.T3 = 0;
    jls_params.custom.RESET = 0;
    jls_params.custom.MAXVAL = 0;

    // set interleave mode
    if (jls_params.components == 1) jls_params.ilv = ILV_NONE;
        else jls_params.ilv = ILV_LINE;

    size_t compressed_buffer_size = data_size + 1024;
    BYTE *compressed_buffer = new BYTE[compressed_buffer_size];
    size_t bytesWritten = 0;

    JLS_ERROR err = JpegLsEncode(&compressed_buffer, &compressed_buffer_size, &bytesWritten, data, data_size, &jls_params);
    OFCondition cond = DJLSError::convert(err);

    if (cond.good())
    {
        // write file here
        size_t fileWritten = fwrite(compressed_buffer, 1, bytesWritten, stream);
        if (fileWritten != bytesWritten)
        {
            DCMJPLS_WARN("Writing the JPEG-LS file failed, file system full?");
            cond = EC_InvalidStream;
        }
    }
    else
    {
        DCMJPLS_WARN("JPEG-LS encoding failed: " << cond.text());
    }

    delete[] compressed_buffer;
    if (cond.good()) return 1; else return 0;
}


/* This function creates a JPEG-LS file of the rendered DICOM pixel data,
 * with all grayscale transformations, rotation, scaling etc. applied.
 */
static int compressJPEGLSRendered(
  DiImage *dimage,
  FILE *stream,
  const unsigned long frame,
  OFBool force8Bit)
{
    if ((dimage == NULL) || (stream == NULL)) return 0;

    /* determine image parameters */
    int depth = dimage->getBits();
    int out_depth = (depth > 8) ? 16 : 8;
    if (force8Bit) out_depth = 8;
    EP_Interpretation colorModel = dimage->getInternalColorModel();
    int samplesPerPixel = 3;
    if ((colorModel == EPI_Monochrome1) || (colorModel == EPI_Monochrome2)) samplesPerPixel = 1;

    /* create bitmap with 8 or 16 bits per sample */
    size_t data_size = dimage->getOutputDataSize(out_depth);
    const void *data = dimage->getOutputData(frame, out_depth, 0 /*planar*/);
    if (data == NULL)
    {
        DCMJPLS_WARN("cannot access pixel data, JPEG-LS export failed.");
        return 0;
    }

    int result = createAndWriteJPLS(dimage->getRows(), dimage->getColumns(), 
        out_depth, samplesPerPixel, data_size, data, stream);
    dimage->deleteOutputData();
    return result;
}

/* This function creates a JPEG-LS file with the original DICOM pixel data,
 * without applying any grayscale transformation. This is the same algorithm
 * that is also applied by DCMTK's JPEG-LS encoder.
 */
static int compressJPEGLSLossless(
  DiImage *dimage,
  FILE *stream,
  const unsigned long frame)
{
    if ((dimage == NULL) || (stream == NULL)) return 0;

    // access essential image parameters
    int width = dimage->getColumns();
    int height = dimage->getRows();
    int depth = dimage->getBits();
    if ((depth < 1) || (depth > 16)) 
    {
        DCMJPLS_WARN("cannot compress image with " << depth << " bits/sample: JPEG-LS supports max. 16 bits");
        return 0;
    }

    const DiPixel *dinter = dimage->getInterData();
    if (dinter == NULL) return 0;

    int samplesPerPixel = dinter->getPlanes();
    if (samplesPerPixel != 1 && samplesPerPixel != 3) return 0;

    // get pointer to internal raw representation of image data
    const void *draw = dinter->getData();
    if (draw == NULL) return 0;

    OFCondition cond = EC_Normal;
    const void *planes[3] = {NULL, NULL, NULL};
    if (samplesPerPixel == 3)
    {
        // for color images, dinter->getData() returns a pointer to an array
        // of pointers pointing to the real plane data
        const void * const * draw_array = OFstatic_cast(const void * const *,draw);
        planes[0] = draw_array[0];
        planes[1] = draw_array[1];
        planes[2] = draw_array[2];
    }
    else
    {
        // for monochrome images, dinter->getData() directly returns a pointer
        // to the single monochrome plane.
        planes[0] = draw;
    }

    // This is the buffer with the uncompressed pixel data
    Uint8 *buffer;
    size_t buffer_size;
    Uint32 framesize = dimage->getColumns() * dimage->getRows();
    switch(dinter->getRepresentation())
    {
        case EPR_Uint8:
        case EPR_Sint8:
          {
              // image representation is 8 bit signed or unsigned
              if (samplesPerPixel == 1)
              {
                  const Uint8 *yv = OFreinterpret_cast(const Uint8 *, planes[0]) + framesize * frame;
                  buffer_size = framesize;
                  buffer = new Uint8[buffer_size];
                  memcpy(buffer, yv, framesize);
              }
              else
              {
                  const Uint8 *rv = OFreinterpret_cast(const Uint8 *, planes[0]) + framesize * frame;
                  const Uint8 *gv = OFreinterpret_cast(const Uint8 *, planes[1]) + framesize * frame;
                  const Uint8 *bv = OFreinterpret_cast(const Uint8 *, planes[2]) + framesize * frame;
                
                  buffer_size = framesize * 3;
                  buffer = new Uint8[buffer_size];
                
                  size_t i = 0;
                  for (int row=height; row; --row)
                  {
                      for (int col=width; col; --col)
                      {
                          buffer[i++] = *rv;
                          buffer[i++] = *gv;
                          buffer[i++] = *bv;
                        
                          rv++;
                          gv++;
                          bv++;
                      }
                  }
              }
          }
          break;
        case EPR_Uint16:
        case EPR_Sint16:
          {
              // image representation is 16 bit signed or unsigned
              if (samplesPerPixel == 1)
              {
                  const Uint16 *yv = OFreinterpret_cast(const Uint16 *, planes[0]) + framesize * frame;
                  buffer_size = framesize*sizeof(Uint16);
                  buffer = new Uint8[buffer_size];
                  memcpy(buffer, yv, buffer_size);
              }
              else
              {
                  const Uint16 *rv = OFreinterpret_cast(const Uint16 *, planes[0]) + framesize * frame;
                  const Uint16 *gv = OFreinterpret_cast(const Uint16 *, planes[1]) + framesize * frame;
                  const Uint16 *bv = OFreinterpret_cast(const Uint16 *, planes[2]) + framesize * frame;
              
                  buffer_size = framesize * 3;
                  Uint16 *buffer16 = new Uint16[buffer_size];
                  buffer = OFreinterpret_cast(Uint8 *, buffer16);
              
                  // Convert to byte count
                  buffer_size *= 2;
              
                  size_t i = 0;
                  for (int row=height; row; --row)
                  {
                      for (int col=width; col; --col)
                      {
                          buffer16[i++] = *rv;
                          buffer16[i++] = *gv;
                          buffer16[i++] = *bv;
                        
                          rv++;
                          gv++;
                          bv++;
                      }
                  }
              }
          }
          break;
        default:
          // we don't support images with > 16 bits/sample
          DCMJPLS_WARN("cannot compress image with " << depth << " bits/sample with JPEG-LS");
          return 0;
          break;
    }

    int result = createAndWriteJPLS(height, width, depth, samplesPerPixel, buffer_size, buffer, stream);
    delete[] buffer;
    return result;
}


int DiJPLSPlugin::write(
  DiImage *dimage,
  FILE *stream,
  const unsigned long frame) const
{
    if (trueLosslessMode_)
        return compressJPEGLSLossless(dimage, stream, frame);
        else return compressJPEGLSRendered(dimage, stream, frame, force8BitMode_);
}
