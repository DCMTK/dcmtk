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
 *  Author:  Joerg Riesmeier
 *
 *  Purpose: Implements JPEG interface for plugable image formats
 *
 */


#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmdata/dctypes.h"
#include "dcmtk/dcmimgle/diimage.h"
#include "dcmtk/dcmjpeg/dipijpeg.h"

#define INCLUDE_CSETJMP
#define INCLUDE_CSTDIO
#include "dcmtk/ofstd/ofstdinc.h"

BEGIN_EXTERN_C
#define boolean ijg_boolean
#include "jpeglib8.h"
#include "jerror8.h"
#include "jpegint8.h"
#include "jversion8.h"
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
struct DIEIJG8ErrorStruct
{
    // the standard IJG error handler object
    struct jpeg_error_mgr pub;
    // our jump buffer
    jmp_buf setjmp_buffer;
    // pointer to this
    const DiJPEGPlugin *instance;
};

// callback forward declarations
void DIEIJG8ErrorExit(j_common_ptr);
void DIEIJG8OutputMessage(j_common_ptr cinfo);

// helper method to fix old-style casts warnings
static void OFjpeg_create_compress(j_compress_ptr cinfo)
{
    jpeg_create_compress(cinfo);
}

END_EXTERN_C


/*-------------*
 *  callbacks  *
 *-------------*/

// error handler, executes longjmp
void DIEIJG8ErrorExit(j_common_ptr cinfo)
{
  DIEIJG8ErrorStruct *myerr = OFreinterpret_cast(DIEIJG8ErrorStruct*, cinfo->err);
  longjmp(myerr->setjmp_buffer, 1);
}

// message handler for warning messages and the like
void DIEIJG8OutputMessage(j_common_ptr cinfo)
{
  DIEIJG8ErrorStruct *myerr = OFreinterpret_cast(DIEIJG8ErrorStruct*, cinfo->err);
  myerr->instance->outputMessage(cinfo);
}


/*----------------*
 *  constructors  *
 *----------------*/

DiJPEGPlugin::DiJPEGPlugin()
  : DiPluginFormat(),
    Quality(75),
    Sampling(ESS_444)
{
}


DiJPEGPlugin::~DiJPEGPlugin()
{
}


/*------------------*
 *  implementation  *
 *------------------*/

void DiJPEGPlugin::setQuality(const unsigned int quality)
{
    /* valid range: 0..100 (percent) */
    if (Quality <= 100)
        Quality = quality;
}


void DiJPEGPlugin::setSampling(const E_SubSampling sampling)
{
    Sampling = sampling;
}


void DiJPEGPlugin::outputMessage(void *arg) const
{
    jpeg_common_struct *cinfo = OFreinterpret_cast(jpeg_common_struct*, arg);
    if (cinfo && DCM_dcmjpegLogger.isEnabledFor(OFLogger::WARN_LOG_LEVEL))
    {
        char buffer[JMSG_LENGTH_MAX];
        (*cinfo->err->format_message)(cinfo, buffer); /* Create the message */
        DCMJPEG_WARN(buffer);
    }
}


int DiJPEGPlugin::write(DiImage *image,
                        FILE *stream,
                        const unsigned long frame) const
{
    int result = 0;
    if ((image != NULL) && (stream != NULL))
    {
        /* create bitmap with 8 bits per sample */
        const void *data = image->getOutputData(frame, 8 /*bits*/, 0 /*planar*/);
        if (data != NULL)
        {
            const OFBool isMono = (image->getInternalColorModel() == EPI_Monochrome1) ||
                                  (image->getInternalColorModel() == EPI_Monochrome2);

            /* code derived from "cjpeg.c" (IJG) and "djeijg8.cc" (DCMJPEG) */
            struct jpeg_compress_struct cinfo;
            OFjpeg_create_compress(&cinfo);
            /* Initialize JPEG parameters. */
            cinfo.image_width = image->getColumns();
            cinfo.image_height = image->getRows();
            cinfo.input_components = (isMono) ? 1 : 3;
            cinfo.in_color_space = (isMono) ? JCS_GRAYSCALE : ((image->getInternalColorModel() == EPI_YBR_Full) ? JCS_YCbCr : JCS_RGB);

            /* Set up the error handling. This has to be after all uses of isMono */
            struct DIEIJG8ErrorStruct jerr;
            /* Initialize the JPEG compression object with default error handling. */
            cinfo.err = jpeg_std_error(&jerr.pub);
            /* overwrite with specific error handling */
            jerr.instance = this;
            jerr.pub.error_exit = DIEIJG8ErrorExit;
            jerr.pub.output_message = DIEIJG8OutputMessage;
            if (setjmp(jerr.setjmp_buffer))
            {
                // the IJG error handler will cause the following code to be executed
                char buffer[JMSG_LENGTH_MAX];
                /* Create the message */
                (*cinfo.err->format_message)(OFreinterpret_cast(jpeg_common_struct*, &cinfo), buffer);
                /* Release memory */
                jpeg_destroy_compress(&cinfo);
                image->deleteOutputData();
                /* return error code */
                return 0;
            }
            jpeg_set_defaults(&cinfo);
            cinfo.optimize_coding = TRUE;
            /* Set quantization tables for selected quality. */
            jpeg_set_quality(&cinfo, Quality, TRUE /*force_baseline*/);
            /* Specify data destination for compression */
            jpeg_stdio_dest(&cinfo, stream);
            /* initialize sampling factors */
            if (cinfo.jpeg_color_space == JCS_YCbCr)
            {
                switch(Sampling)
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
            } else {
                // JPEG color space is not YCbCr, disable subsampling.
                cinfo.comp_info[0].h_samp_factor = 1;
                cinfo.comp_info[0].v_samp_factor = 1;
            }
            // all other components are set to 1x1
            for (int sfi = 1; sfi < MAX_COMPONENTS; sfi++)
            {
                cinfo.comp_info[sfi].h_samp_factor = 1;
                cinfo.comp_info[sfi].v_samp_factor = 1;
            }
            /* Start compressor */
            jpeg_start_compress(&cinfo, TRUE);
            /* Process data */
            JSAMPROW row_pointer[1];
            Uint8 *image_buffer = OFreinterpret_cast(Uint8*, OFconst_cast(void*, data));
            const size_t row_stride = cinfo.image_width * cinfo.input_components;
            while (cinfo.next_scanline < cinfo.image_height)
            {
                row_pointer[0] = &image_buffer[cinfo.next_scanline * row_stride];
                (void)jpeg_write_scanlines(&cinfo, row_pointer, 1);
            }
            /* Finish compression and release memory */
            jpeg_finish_compress(&cinfo);
            jpeg_destroy_compress(&cinfo);
            /* All done. */
            result = 1;
        }
        /* delete pixel data */
        image->deleteOutputData();
    }
    return result;
}


OFString DiJPEGPlugin::getLibraryVersionString()
{
    /* create version information */
    return "IJG, Version " JVERSION " (modified)";
}
