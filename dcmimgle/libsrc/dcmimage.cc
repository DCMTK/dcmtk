/*
 *
 *  Copyright (C) 1996-2010, OFFIS e.V.
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
 *  Last Update:      $Author: uli $
 *  Update Date:      $Date: 2010-11-01 08:55:56 $
 *  CVS/RCS Revision: $Revision: 1.37 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
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

#define INCLUDE_CCTYPE
#include "dcmtk/ofstd/ofstdinc.h"

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
                unsigned char c;
                for (const char *p = str; *p != 0; p++)    // remove invalid chars
                {
                    c = OFstatic_cast(unsigned char, *p);
                    if (isalpha(c))
                        *(q++) = toupper(c);
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
// --- if one dimension ist '0' the other is automatically adjusted (with respect to pixel aspect ratio)

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


// --- write 'frame' of image data to 'filename' plugable image format

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


// --- same for open C 'FILE' in plugable image format

int DicomImage::writePluginFormat(const DiPluginFormat *plugin,
                                  FILE *stream,
                                  const unsigned long frame)
{
    if ((plugin != NULL) && (stream != NULL) && (Image != NULL))
        return plugin->write(Image, stream, frame);
    return 0;
}


/*
 *
 * CVS/RCS Log:
 * $Log: dcmimage.cc,v $
 * Revision 1.37  2010-11-01 08:55:56  uli
 * Moved variable declarations in front of their loop.
 *
 * Revision 1.36  2010-10-20 07:41:35  uli
 * Made sure isalpha() & friends are only called with valid arguments.
 *
 * Revision 1.35  2010-10-14 13:14:17  joergr
 * Updated copyright header. Added reference to COPYRIGHT file.
 *
 * Revision 1.34  2010-09-24 13:25:41  joergr
 * Compared names of SOP Class UIDs with 2009 edition of the DICOM standard. The
 * resulting name changes are mainly caused by the fact that the corresponding
 * SOP Class is now retired.
 *
 * Revision 1.33  2009-11-25 16:33:23  joergr
 * Adapted code for new approach to access individual frames of a DICOM image.
 * The getString() method now returns the Defined Term of the attribute
 * PhotometricInterpretation.
 *
 * Revision 1.32  2009-10-28 14:26:01  joergr
 * Fixed minor issues in log output.
 *
 * Revision 1.31  2009-10-28 09:53:40  uli
 * Switched to logging mechanism provided by the "new" oflog module.
 *
 * Revision 1.30  2008-05-20 15:27:51  joergr
 * Added more checks on parameters for combined scaling and clipping.
 *
 * Revision 1.29  2008-05-20 10:01:33  joergr
 * Fixed issue with wrong image aspect ratio in combined scaling/clipping mode.
 *
 * Revision 1.28  2006/08/15 16:30:11  meichel
 * Updated the code in module dcmimgle to correctly compile when
 *   all standard C++ classes remain in namespace std.
 *
 * Revision 1.27  2006/07/10 10:54:26  joergr
 * Added support for 32-bit BMP images.
 *
 * Revision 1.26  2005/12/08 15:42:42  meichel
 * Changed include path schema for all DCMTK header files
 *
 * Revision 1.25  2003/12/23 16:03:18  joergr
 * Replaced post-increment/decrement operators by pre-increment/decrement
 * operators where appropriate (e.g. 'i++' by '++i').
 *
 * Revision 1.24  2003/12/08 15:06:37  joergr
 * Adapted type casts to new-style typecast operators defined in ofcast.h.
 *
 * Revision 1.23  2003/06/04 10:20:33  meichel
 * Fixed incorrect include
 *
 * Revision 1.22  2003/06/03 09:30:41  meichel
 * Added include for dcdicent.h, needed by MSVC5 with STL
 *
 * Revision 1.21  2002/12/09 13:34:50  joergr
 * Renamed parameter/local variable to avoid name clashes with global
 * declaration left and/or right (used for as iostream manipulators).
 *
 * Revision 1.20  2002/11/27 14:08:10  meichel
 * Adapted module dcmimgle to use of new header file ofstdinc.h
 *
 * Revision 1.19  2002/08/21 09:51:47  meichel
 * Removed DicomImage and DiDocument constructors that take a DcmStream
 *   parameter
 *
 * Revision 1.18  2001/11/27 18:21:37  joergr
 * Added support for plugable output formats in class DicomImage. First
 * implementation is JPEG.
 *
 * Revision 1.17  2001/11/09 16:28:48  joergr
 * Added support for Windows BMP file format.
 *
 * Revision 1.16  2001/06/01 15:49:52  meichel
 * Updated copyright header
 *
 * Revision 1.15  2000/04/28 12:33:40  joergr
 * DebugLevel - global for the module - now derived from OFGlobal (MF-safe).
 *
 * Revision 1.14  2000/04/27 13:10:24  joergr
 * Dcmimgle library code now consistently uses ofConsole for error output.
 *
 * Revision 1.13  2000/03/08 16:24:25  meichel
 * Updated copyright header.
 *
 * Revision 1.12  2000/03/03 14:09:16  meichel
 * Implemented library support for redirecting error messages into memory
 *   instead of printing them to stdout/stderr for GUI applications.
 *
 * Revision 1.11  2000/02/02 11:04:50  joergr
 * Removed space characters before preprocessor directives.
 *
 * Revision 1.10  1999/10/06 13:45:54  joergr
 * Corrected creation of PrintBitmap pixel data: VOI windows should be applied
 * before clipping to avoid that the region outside the image (border) is also
 * windowed (this requires a new method in dcmimgle to create a DicomImage
 * with the grayscale transformations already applied).
 *
 * Revision 1.9  1999/08/25 16:43:06  joergr
 * Added new feature: Allow clipping region to be outside the image
 * (overlapping).
 *
 * Revision 1.8  1999/04/28 14:59:45  joergr
 * Introduced new scheme for the debug level variable: now each level can be
 * set separately (there is no "include" relationship).
 *
 * Revision 1.7  1999/03/24 17:22:03  joergr
 * Removed bug when scaling and clipping images where the clipping area exceeds
 * the original images.
 *
 * Revision 1.6  1999/02/03 17:37:55  joergr
 * Added BEGIN_EXTERN_C and END_EXTERN_C to some C includes.
 *
 * Revision 1.5  1999/01/20 14:52:03  joergr
 * Changed default value for compatibility flag.
 *
 * Revision 1.4  1998/12/22 13:28:09  joergr
 * Corrected some typos.
 *
 * Revision 1.3  1998/12/16 16:08:36  joergr
 * Implemented combined clipping and scaling for pixel replication and
 * suppression.
 *
 * Revision 1.2  1998/12/14 17:32:59  joergr
 * Added first implementation of method to export overlay plane bitmaps.
 *
 * Revision 1.1  1998/11/27 15:58:13  joergr
 * Added copyright message.
 * Added methods and constructors for flipping and rotating, changed for
 * scaling and clipping.
 * Added method to directly create java AWT bitmaps.
 * Added constructors to use external modality transformations.
 * Added methods to support presentation LUTs and shapes.
 * Added methods to convert module defined enum types to strings.
 * Introduced global debug level for dcmimage module to control error output.
 * Added support of frame start and count for future use (will be explained
 * later if it is fully implemented).
 * Introduced new method and corresponding classes to create a copy of a
 * dcmimage object (optional parameters: frame start and count).
 * Combined clipping and scaling methods.
 *
 * Revision 1.10  1998/07/01 08:39:33  joergr
 * Minor changes to avoid compiler warnings (gcc 2.8.1 with additional
 * options), e.g. add copy constructors.
 *
 * Revision 1.9  1998/06/25 08:52:05  joergr
 * Added compatibility mode to support ACR-NEMA images and wrong
 * palette attribute tags.
 *
 * Revision 1.8  1998/05/11 14:52:24  joergr
 * Added CVS/RCS header to each file.
 *
 *
 */
