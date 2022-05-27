/*
 *
 *  Copyright (C) 1996-2022, OFFIS e.V.
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
 *  Module:  dcmimgle
 *
 *  Author:  Joerg Riesmeier
 *
 *  Purpose: DicomImage-Interface (Source)
 *
 */


#include "dcmtk/config/osconfig.h"

#include "dcmtk/dcmdata/dctypes.h"
#include "dcmtk/dcmdata/dcdeftag.h"
#include "dcmtk/dcmdata/dcobject.h"
#include "dcmtk/dcmdata/dcuid.h"
#include "dcmtk/dcmdata/dcdict.h"

#include "dcmtk/dcmimgle/dcmimage.h"
#include "dcmtk/dcmimgle/diovlimg.h"
#include "dcmtk/dcmimgle/dimo1img.h"
#include "dcmtk/dcmimgle/dimo2img.h"
#include "dcmtk/dcmimgle/didocu.h"
#include "dcmtk/dcmimgle/diregbas.h"
#include "dcmtk/dcmimgle/diplugin.h"
#include "dcmtk/dcmdata/dcdicent.h"  /* needed by MSVC5 */

#ifndef FILENAME_MAX
#define FILENAME_MAX 255
#endif


/*------------------*
 *  initialization  *
 *------------------*/

DiRegisterBase *DiRegisterBase::Pointer = NULL;


/*----------------*
 *  constructors  *
 *----------------*/

// --- create 'DicomImage' from 'filename', for valid 'flags' see 'diutils.h'

DicomImage::DicomImage(const char *filename,
                       const unsigned long flags,
                       const unsigned long fstart,
                       const unsigned long fcount)
  : ImageStatus(EIS_Normal),
    PhotometricInterpretation(EPI_Unknown),
    Document(NULL),
    Image(NULL)
{
    if (checkDataDictionary())                  // valid 'dicom.dic' found ?
    {
        Document = new DiDocument(filename, flags | CIF_MayDetachPixelData, fstart, fcount);
        Init();
    }
}


// --- create 'DicomImage' from valid 'DicomObject' with transfer syntax 'xfer'

DicomImage::DicomImage(DcmObject *object,
                       const E_TransferSyntax xfer,
                       const unsigned long flags,
                       const unsigned long fstart,
                       const unsigned long fcount)
  : ImageStatus(EIS_Normal),
    PhotometricInterpretation(EPI_Unknown),
    Document(NULL),
    Image(NULL)
{
    if (checkDataDictionary())                  // valid 'dicom.dic' found ?
    {
        Document = new DiDocument(object, xfer, flags, fstart, fcount);
        Init();
    }
}


// --- create 'DicomImage' from valid 'DicomObject' with given rescale 'slope' and 'intercept'

DicomImage::DicomImage(DcmObject *object,
                       const E_TransferSyntax xfer,
                       const double slope,
                       const double intercept,
                       const unsigned long flags,
                       const unsigned long fstart,
                       const unsigned long fcount)
  : ImageStatus(EIS_Normal),
    PhotometricInterpretation(EPI_Unknown),
    Document(NULL),
    Image(NULL)
{
    if (checkDataDictionary())                  // valid 'dicom.dic' found ?
    {
        Document = new DiDocument(object, xfer, flags, fstart, fcount);
        if ((Document != NULL) && (Document->good()))
        {
            PhotometricInterpretation = EPI_Monochrome2;            // default for presentation states
            Image = new DiMono2Image(Document, ImageStatus, slope, intercept);
        }
    }
}


// --- create 'DicomImage' from valid 'DicomObject' with given modality LUT (specified by 'data' and 'descriptor')

DicomImage::DicomImage(DcmObject *object,
                       E_TransferSyntax xfer,
                       const DcmUnsignedShort &data,
                       const DcmUnsignedShort &descriptor,
                       const DcmLongString *explanation,
                       const unsigned long flags,
                       const unsigned long fstart,
                       const unsigned long fcount)
  : ImageStatus(EIS_Normal),
    PhotometricInterpretation(EPI_Unknown),
    Document(NULL),
    Image(NULL)
{
    if (checkDataDictionary())                  // valid 'dicom.dic' found ?
    {
        Document = new DiDocument(object, xfer, flags, fstart, fcount);
        if ((Document != NULL) && (Document->good()))
        {
            PhotometricInterpretation = EPI_Monochrome2;            // default for presentation states
            Image = new DiMono2Image(Document, ImageStatus, data, descriptor, explanation);
        }
    }
}


// --- protected: create 'DicomImage' from source with different image data and photometric interpretation

DicomImage::DicomImage(const DicomImage *dicom,
                       DiImage *image,
                       const EP_Interpretation interpret)
  : ImageStatus(dicom->ImageStatus),
    PhotometricInterpretation(dicom->PhotometricInterpretation),
    Document(dicom->Document),                   // necessary, ever used ??
    Image(image)
{
    if (interpret != EPI_Unknown)
        PhotometricInterpretation = interpret;
    if (Document != NULL)
        Document->addReference();               // 'Document' is only referenced not copied !
}


/*--------------*
 *  destructor  *
 *--------------*/

DicomImage::~DicomImage()
{
    delete Image;
    if (Document != NULL)
        Document->removeReference();            // only delete if object is no longer referenced
}


/*********************************************************************/

// --- initialize 'DicomImage' object (same procedure for every 'real' constructor)

void DicomImage::Init()
{
    if ((Document != NULL) && (Document->good()))
    {
        const char *str;
        if (hasSOPclassUID(UID_RETIRED_StandaloneOverlayStorage))
        {
            PhotometricInterpretation = EPI_Monochrome2;            // standalone overlays are handled like monochrome
            Image = new DiOverlayImage(Document, ImageStatus);      // images without pixel data
        }
        else if (Document->getFlags() & CIF_UsePresentationState)
        {
            PhotometricInterpretation = EPI_Monochrome2;            // default for presentation states
            Image = new DiMono2Image(Document, ImageStatus);
        }
        else if (strlen(str = Document->getPhotometricInterpretation()) > 0)
        {
            const SP_Interpretation *pin = PhotometricInterpretationNames;
            char *cstr = new char[strlen(str) + 1];
            if (cstr != NULL)
            {
                char *q = cstr;
                char c;
                for (const char *p = str; *p != 0; p++)    // remove invalid chars
                {
                    c = *p;
                    if (isalpha(c))
                        *(q++) = OFstatic_cast(char, toupper(c));
                    else if (isdigit(c))
                        *(q++) = c;
                }
                *q = '\0';                                          // end of C string
                while ((pin->Name != NULL) && (strcmp(pin->Name, cstr) != 0))
                    ++pin;
                delete[] cstr;
            } else {
                DCMIMGLE_WARN("can't create filtered version of 'PhotometricInterpretation' (" << str << ")");
                cstr = OFconst_cast(char *, str);                   // just reference the original string
                while ((pin->DefinedTerm != NULL) && (strcmp(pin->DefinedTerm, cstr) != 0))
                    ++pin;
            }
            PhotometricInterpretation = pin->Type;                  // unknown if last entry
            switch (PhotometricInterpretation)
            {
                case EPI_Monochrome1:                               // create 'Image' depending on color model
                    Image = new DiMono1Image(Document, ImageStatus);
                    break;
                case EPI_Monochrome2:
                    Image = new DiMono2Image(Document, ImageStatus);
                    break;
                default:                                            // unknown or unsupported color model
                    if (DiRegisterBase::Pointer != NULL)
                        Image = DiRegisterBase::Pointer->createImage(Document, ImageStatus, PhotometricInterpretation);
                    if (Image == NULL)
                    {
                        if (PhotometricInterpretation == EPI_Unknown)
                        {
                            ImageStatus = EIS_InvalidValue;
                            DCMIMGLE_ERROR("invalid value for 'PhotometricInterpretation' (" << str << ")");
                        } else {
                            ImageStatus = EIS_NotSupportedValue;
                            DCMIMGLE_ERROR("unsupported value for 'PhotometricInterpretation' (" << str << ")");
                        }
                    }
            }
        }
        else if (Document->getFlags() & CIF_AcrNemaCompatibility)   // ACR-NEMA has no 'photometric interpretation'
        {
            PhotometricInterpretation = EPI_Monochrome2;
            Image = new DiMono2Image(Document, ImageStatus);
        } else {
            ImageStatus = EIS_MissingAttribute;
            PhotometricInterpretation = EPI_Missing;
            if (Document->getPixelData() != NULL)
                DCMIMGLE_ERROR("mandatory attribute 'PhotometricInterpretation' is missing or can't be determined");
        }
    }
    else
        ImageStatus = EIS_InvalidDocument;
}


// --- check whether the loadable 'DataDictionary' is present/loaded

int DicomImage::checkDataDictionary()
{
    if (!dcmDataDict.isDictionaryLoaded())
    {
        ImageStatus = EIS_NoDataDictionary;
        DCMIMGLE_ERROR("can't load data dictionary");
    }
    return ImageStatus == EIS_Normal;
}


/*********************************************************************/


const char *DicomImage::getString(const EI_Status status)
{
    switch (status)
    {
      case EIS_Normal:
        return "Status OK";
      case EIS_NoDataDictionary:
        return "No data dictionary";
      case EIS_InvalidDocument:
        return "Invalid DICOM document";
      case EIS_MissingAttribute:
        return "Missing attribute";
      case EIS_InvalidValue:
        return "Invalid element value";
      case EIS_NotSupportedValue:
        return "Unsupported element value";
      case EIS_MemoryFailure:
        return "Out of memory";
      case EIS_InvalidImage:
        return "Invalid DICOM image";
      case EIS_OtherError:
      default:
        return "Unspecified";
    }
}


const char *DicomImage::getString(const EP_Interpretation interpret)
{
    const SP_Interpretation *pin = PhotometricInterpretationNames;
    while ((pin->DefinedTerm != NULL) && (pin->Type != interpret))
        ++pin;
    return pin->DefinedTerm;
}


// --- return unique 'SOPclassUID' string

const char *DicomImage::getSOPclassUID() const
{
    if (Document != NULL)
    {
        const char *str;
        if (Document->getValue(DCM_SOPClassUID, str))
            return str;
    }
    return NULL;
}


// --- return 'true' (1) if 'Document' has the same 'SOPclassUID' as given in parameter 'uid'

int DicomImage::hasSOPclassUID(const char *uid) const
{
    const char *str = getSOPclassUID();
    return (str != NULL) && (strcmp(str, uid) == 0);
}


// --- create new 'DicomImage' with 'fcount' frames starting with frame 'fstart'

DicomImage *DicomImage::createDicomImage(unsigned long fstart,
                                         unsigned long fcount) const
{
    if ((Image != NULL) && (fstart < getFrameCount()))
    {
        if ((fcount == 0) || (fstart + fcount > getFrameCount()))
            fcount = getFrameCount() - fstart;
        DiImage *image = Image->createImage(fstart, fcount);
        if (image != NULL)
        {
            DicomImage *dicom = new DicomImage(this, image);
            return dicom;
        }
    }
    return NULL;
}


// --- create scaled to given size ('width' and 'height') image, memory isn't handled internally !
// --- if one dimension is '0' the other is automatically adjusted (with respect to pixel aspect ratio)

DicomImage *DicomImage::createScaledImage(const unsigned long width,
                                          const unsigned long height,
                                          const int interpolate,
                                          int aspect) const
{
    return createScaledImage(0, 0, getWidth(), getHeight(), width, height, interpolate, aspect);
}


// --- create scaled with given factors ('xfactor' and 'yfactor') image, memory isn't handled internally !

DicomImage *DicomImage::createScaledImage(const double xfactor,
                                          const double yfactor,
                                          const int interpolate,
                                          const int aspect) const
{
    return createScaledImage(0, 0, getWidth(), getHeight(), OFstatic_cast(unsigned long, xfactor * getWidth()),
        OFstatic_cast(unsigned long, yfactor * getHeight()), interpolate, aspect);
}


// --- clip & scale

DicomImage *DicomImage::createScaledImage(const signed long left_pos,
                                          const signed long top_pos,
                                          unsigned long clip_width,
                                          unsigned long clip_height,
                                          unsigned long scale_width,
                                          unsigned long scale_height,
                                          const int interpolate,
                                          int aspect,
                                          const Uint16 pvalue) const
{
    const unsigned long gw = getWidth();
    const unsigned long gh = getHeight();
    if ((Image != NULL) && (gw > 0) && (gh > 0))
    {
        if ((clip_width == 0) && (left_pos < OFstatic_cast(signed long, gw)))  // set 'width' if parameter is missing
            clip_width = gw - left_pos;
        if ((clip_height == 0) && (top_pos < OFstatic_cast(signed long, gh)))  // same for 'height'
            clip_height = gh - top_pos;
        if ((scale_width == 0) && (scale_height == 0))
        {
            scale_width = clip_width;                                // auto-set width/height
            scale_height = clip_height;
        }
        else if ((clip_width > 0) && (clip_height > 0))
        {
            if (aspect)                                              // maintain pixel aspect ratio
            {
                if (scale_width == 0)
                    scale_width = OFstatic_cast(unsigned long, getWidthHeightRatio() * OFstatic_cast(double, scale_height * clip_width) / clip_height);
                else if (scale_height == 0)
                    scale_height = OFstatic_cast(unsigned long, getHeightWidthRatio() * OFstatic_cast(double, scale_width * clip_height) / clip_width);
                else
                    aspect = 0;                                      // ignore pixel aspect ratio
            }
            else                                                     // ignore pixel aspect ratio
            {
                if (scale_width == 0)
                    scale_width = OFstatic_cast(unsigned long, OFstatic_cast(double, scale_height * clip_width) / clip_height);
                else if (scale_height == 0)
                    scale_height = OFstatic_cast(unsigned long, OFstatic_cast(double, scale_width * clip_height) / clip_width);
            }
        }
        const unsigned long maxvalue = DicomImageClass::maxval(bitsof(Uint16));
        if (scale_width > maxvalue)
            scale_width = maxvalue;                                  // limit 'width' to maximum value (65535)
        if (scale_height > maxvalue)
            scale_height = maxvalue;                                 // same for 'height'

        /* need to limit clipping region ... !? */

        if (((left_pos < 0) || (OFstatic_cast(unsigned long, left_pos + clip_width) > gw) ||
            (top_pos < 0) || (OFstatic_cast(unsigned long, top_pos + clip_height) > gh)) &&
            ((clip_width != scale_width) || (clip_height != scale_height)))
        {
            DCMIMGLE_ERROR("combined clipping & scaling outside image boundaries not yet supported");
        }
        else if ((scale_width > 0) && (scale_height > 0))
        {
            DiImage *image = Image->createScale(left_pos, top_pos, clip_width, clip_height, scale_width, scale_height,
                interpolate, aspect, pvalue);
            if (image != NULL)
            {
                DicomImage *dicom = new DicomImage(this, image);
                return dicom;
            }
        }
    }
    return NULL;
}


// --- clip & scale

DicomImage *DicomImage::createScaledImage(const signed long left_pos,
                                          const signed long top_pos,
                                          unsigned long width,
                                          unsigned long height,
                                          const double xfactor,
                                          const double yfactor,
                                          const int interpolate,
                                          const int aspect,
                                          const Uint16 pvalue) const
{
    if ((xfactor >= 0) && (yfactor >= 0))
    {
        const unsigned long gw = getWidth();
        const unsigned long gh = getHeight();
        if ((width == 0) && (left_pos < OFstatic_cast(signed long, gw)))  // set 'width' if parameter is missing (0)
            width = gw - left_pos;
        if ((height == 0) && (top_pos < OFstatic_cast(signed long, gh)))  // same for 'height'
            height = gh - top_pos;
        return createScaledImage(left_pos, top_pos, width, height, OFstatic_cast(unsigned long, xfactor * width),
            OFstatic_cast(unsigned long, yfactor * height), interpolate, aspect, pvalue);
    }
    return NULL;
}


// --- create clipped to given box ('left_pos', 'top_pos' and 'width', 'height') image,
// ---- memory isn't handled internally! 'width' and 'height' are optional

DicomImage *DicomImage::createClippedImage(const signed long left_pos,
                                           const signed long top_pos,
                                           unsigned long width,
                                           unsigned long height,
                                           const Uint16 pvalue) const
{
    return createScaledImage(left_pos, top_pos, width, height, OFstatic_cast(unsigned long, 0),
        OFstatic_cast(unsigned long, 0), 0, 0, pvalue);
}


// --- flip image (horizontal: x > 1 and/or vertical y > 1)

int DicomImage::flipImage(int horz,
                          int vert) const
{
    if ((Image != NULL) && (horz || vert))
    {
        if (getWidth() <= 1)
            horz = 0;
        if (getHeight() <= 1)
            vert = 0;
        if (horz || vert)
            return Image->flip(horz, vert);
        else
            return 2;
    }
    return 0;
}


// --- create flipped image (horizontal: x > 1 and/or vertical y > 1), memory isn't handled internally !

DicomImage *DicomImage::createFlippedImage(int horz,
                                           int vert) const
{
    if ((Image != NULL) && (horz || vert))
    {
        if (getWidth() <= 1)                                        // can't flip horizontally
            horz = 0;
        if (getHeight() <= 1)                                       // can't flip vertically
            vert = 0;
        DiImage *image;
        if (horz || vert)                                           // flip at least one axis
            image = Image->createFlip(horz, vert);
        else                                                        // copy image
            image = Image->createImage(0, getFrameCount());
        if (image != NULL)
        {
            DicomImage *dicom = new DicomImage(this, image);
            return dicom;
        }
    }
    return NULL;
}


// -- normalize given 'degree' value to 0, 90, 180, 270

int DicomImage::normalizeDegreeValue(signed int &degree) const
{
    switch (degree)
    {
        case 0:
        case 360:
        case -360:
            degree = 0;
            return 1;
        case 90:
        case -270:
            degree = 90;
            return 1;
        case 180:
        case -180:
            degree = 180;
            return 1;
        case 270:
        case -90:
            degree = 270;
            return 1;
        default:
            return 0;
    }
}


// --- rotate image by given 'degree'

int DicomImage::rotateImage(signed int degree) const
{
    if ((Image != NULL) && normalizeDegreeValue(degree))
    {
        if ((degree == 0) || (getWidth() * getHeight() <= 1))       // nothing to do
            return 2;
        else
            return Image->rotate(OFstatic_cast(int, degree));
    }
    return 0;
}


// --- create by given 'degree' rotated image, memory isn't handled internally !

DicomImage *DicomImage::createRotatedImage(signed int degree) const
{
    if ((Image != NULL) && normalizeDegreeValue(degree))
    {
        DiImage *image = Image->createRotate(OFstatic_cast(int, degree));
        if (image != NULL)
        {
            DicomImage *dicom = new DicomImage(this, image);
            return dicom;
        }
    }
    return NULL;
}


// --- create color-image to mono-image with given 'red', 'green' and 'blue' coefficients converted image, memory ... !

DicomImage *DicomImage::createMonochromeImage(const double red,
                                              const double green,
                                              const double blue) const
{
    if (Image != NULL)
    {
        DiImage *image = Image->createMono(red, green, blue);       // create monochrome image data
        if (image != NULL)
        {
            DicomImage *dicom = new DicomImage(this, image, EPI_Monochrome2);
            return dicom;
        }
    }
    return NULL;
}


// --- create monochrome output image of specified frame (incl. windowing)

DicomImage *DicomImage::createMonoOutputImage(const unsigned long frame,
                                              const int bits)
{
    if ((Image != NULL) && (Image->getMonoImagePtr() != NULL))
    {
        DiImage *image = Image->getMonoImagePtr()->createOutputImage(frame, bits);
        if (image != NULL)
        {
            DicomImage *dicom = new DicomImage(this, image, EPI_Monochrome2);
            return dicom;
        }
    }
    return NULL;
}


/*********************************************************************/


// --- write 'frame' of image data to 'filename' with 'bits' depth

int DicomImage::writePPM(const char *filename,
                         const int bits,
                         const unsigned long frame)
{
    if ((filename != NULL) && (Image != NULL))
    {
        char fname[FILENAME_MAX + 1];
        if (sprintf(fname, filename, frame) >= 0)           // replace '%d' etc. with frame number
            filename = fname;
        FILE *stream = fopen(filename, "w");                // open text file for writing
        int ok = writePPM(stream, bits, frame);
        fclose(stream);
        return ok;
    }
    return 0;
}


// --- same for C++ 'ostream'

int DicomImage::writePPM(STD_NAMESPACE ostream& stream,
                         const int bits,
                         const unsigned long frame)
{
    if ((stream.good()) && (Image != NULL))
        return Image->writePPM(stream, frame, Image->getBits(bits));
    return 0;
}


// --- same for C 'FILE'

int DicomImage::writePPM(FILE *stream,
                         const int bits,
                         const unsigned long frame)
{
    if ((stream != NULL) && (Image != NULL))
        return Image->writePPM(stream, frame, Image->getBits(bits));
    return 0;
}


// --- same for RAW PPM (binary form of PPM with a maximum of 8 bits depth)

int DicomImage::writeRawPPM(const char *filename,
                            const int bits,
                            const unsigned long frame)
{
    if ((filename != NULL) && (Image != NULL) && (Image->getBits(bits) <= MAX_RAWPPM_BITS))
    {
        char fname[FILENAME_MAX + 1];
        if (sprintf(fname, filename, frame) >= 0)           // replace '%d' etc. with frame number
            filename = fname;
        FILE *stream = fopen(filename, "wb");               // open binary file for writing
        if (stream != NULL)
        {
            int ok = Image->writeRawPPM(stream, frame, Image->getBits(bits));
            fclose(stream);
            return ok;
        }
    }
    return 0;
}

// --- same for C 'FILE'

int DicomImage::writeRawPPM(FILE *stream,
                            const int bits,
                            const unsigned long frame)
{
    if ((stream != NULL) && (Image != NULL))
        return Image->writeRawPPM(stream, frame, Image->getBits(bits));
    return 0;
}


// --- write 'frame' of image data to 'filename' with 'bits' depth in BMP format

int DicomImage::writeBMP(const char *filename,
                         const int bits,
                         const unsigned long frame)
{
    if ((filename != NULL) && (Image != NULL) &&
        ((bits == 0) || ((bits == 8) && isMonochrome()) || (bits == 24) || (bits == 32)))
    {
        char fname[FILENAME_MAX + 1];
        if (sprintf(fname, filename, frame) >= 0)           // replace '%d' etc. with frame number
            filename = fname;
        FILE *stream = fopen(filename, "wb");               // open binary file for writing
        if (stream != NULL)
        {
            int ok = Image->writeBMP(stream, frame, bits);
            fclose(stream);
            return ok;
        }
    }
    return 0;
}


// --- same for open C 'FILE' in BMP format

int DicomImage::writeBMP(FILE *stream,
                         const int bits,
                         const unsigned long frame)
{
    if ((stream != NULL) && (Image != NULL) &&
        ((bits == 0) || ((bits == 8) && isMonochrome()) || (bits == 24) || (bits == 32)))
    {
        return Image->writeBMP(stream, frame, bits);
    }
    return 0;
}


// --- write 'frame' of image data to 'filename' pluggable image format

int DicomImage::writePluginFormat(const DiPluginFormat *plugin,
                                  const char *filename,
                                  const unsigned long frame)
{
    if ((plugin != NULL) && (filename != NULL) && (Image != NULL))
    {
        char fname[FILENAME_MAX + 1];
        if (sprintf(fname, filename, frame) >= 0)           // replace '%d' etc. with frame number
            filename = fname;
        FILE *stream = fopen(filename, "wb");               // open binary file for writing
        if (stream != NULL)
        {
            int ok = plugin->write(Image, stream, frame);
            fclose(stream);
            return ok;
        }
    }
    return 0;
}


// --- same for open C 'FILE' in pluggable image format

int DicomImage::writePluginFormat(const DiPluginFormat *plugin,
                                  FILE *stream,
                                  const unsigned long frame)
{
    if ((plugin != NULL) && (stream != NULL) && (Image != NULL))
        return plugin->write(Image, stream, frame);
    return 0;
}
