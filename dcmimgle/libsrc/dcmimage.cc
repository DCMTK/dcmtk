/*
 *
 *  Copyright (C) 1996-99, OFFIS
 *
 *  This software and supporting documentation were developed by
 *
 *    Kuratorium OFFIS e.V.
 *    Healthcare Information and Communication Systems
 *    Escherweg 2
 *    D-26121 Oldenburg, Germany
 *
 *  THIS SOFTWARE IS MADE AVAILABLE,  AS IS,  AND OFFIS MAKES NO  WARRANTY
 *  REGARDING  THE  SOFTWARE,  ITS  PERFORMANCE,  ITS  MERCHANTABILITY  OR
 *  FITNESS FOR ANY PARTICULAR USE, FREEDOM FROM ANY COMPUTER DISEASES  OR
 *  ITS CONFORMITY TO ANY SPECIFICATION. THE ENTIRE RISK AS TO QUALITY AND
 *  PERFORMANCE OF THE SOFTWARE IS WITH THE USER.
 *
 *  Module:  dcmimgle
 *
 *  Author:  Joerg Riesmeier
 *
 *  Purpose: DicomImage-Interface (Source)
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 1999-04-28 14:59:45 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmimgle/libsrc/dcmimage.cc,v $
 *  CVS/RCS Revision: $Revision: 1.8 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */
 

#include "osconfig.h"
#include "dctypes.h"
#include "dcdeftag.h"
#include "dcobject.h"
#include "dcuid.h"
#include "dcdict.h"

#include "dcmimage.h"
#include "diovlimg.h"
#include "dimo1img.h"
#include "dimo2img.h"
#include "didocu.h"
#include "diregbas.h"

BEGIN_EXTERN_C
 #ifdef HAVE_CTYPE_H
  #include <ctype.h>
 #endif
END_EXTERN_C

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


// --- create 'DicomImage' from valid 'DicomFileStream', for valid 'flags' see 'diutils.h'
 
DicomImage::DicomImage(DcmFileStream &stream,
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
        Document = new DiDocument(stream, flags | CIF_MayDetachPixelData, fstart, fcount);
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
        if (hasSOPclassUID(UID_StandaloneOverlayStorage))
        {
            PhotometricInterpretation = EPI_Monochrome2;            // standalone overlays are handled like monochrome
            Image = new DiOverlayImage(Document, ImageStatus);      // images without pixel data
        }
        else if (Document->getFlags() & CIF_UsePresentationState)
        {
            PhotometricInterpretation = EPI_Monochrome2;            // default for presentation states
            Image = new DiMono2Image(Document, ImageStatus);
        }
        else if (Document->getValue(DCM_PhotometricInterpretation, str))
        {
            char *cstr = new char[strlen(str) + 1];
            if (cstr != NULL)
            {
                char *q = cstr;
                for (const char *p = str; *p != 0; p++)             // remove invalid chars
                {
                    if (isalpha(*p))
                        *(q++) = toupper(*p);
                    else if (isdigit(*p))
                        *(q++) = *p;
                }
                *q = '\0';                                          // end of c-string
            }
            else
                cstr = (char *)str;
            const SP_Interpretation *pin = PhotometricInterpretationNames;
            while ((pin->Name != NULL) && (strcmp(pin->Name, cstr) != 0))
                pin++;
            if (cstr != str)
                delete[] cstr;
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
                            if (DicomImageClass::DebugLevel & DicomImageClass::DL_Errors)
                                cerr << "ERROR: invalid value for 'PhotometricInterpretation' (" << str << ") !" << endl;
                        } else {
                            ImageStatus = EIS_NotSupportedValue;
                            if (DicomImageClass::DebugLevel & DicomImageClass::DL_Errors)
                            {
                                cerr << "ERROR: unsupported value for 'PhotometricInterpretation' (" << str << ") !" << endl;
                                cerr << "       library 'dcmimage' required to handle color images !" << endl;
                            }
                        }
                    }
            }
        }
        else if (Document->getFlags() & CIF_AcrNemaCompatibility)    // ACR-NEMA has no 'photometric interpretation'
        {
            PhotometricInterpretation = EPI_Monochrome2;
            Image = new DiMono2Image(Document, ImageStatus);
        }
        else
        {
            ImageStatus = EIS_MissingAttribute;  
            if (DicomImageClass::DebugLevel & DicomImageClass::DL_Errors)
                cerr << "ERROR: mandatory attribute 'PhotometricInterpretation' is missing !" << endl;
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
        if (DicomImageClass::DebugLevel & DicomImageClass::DL_Errors)
            cerr << "ERROR: can't load data dictionary !" << endl;
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
        return "Invalid data value";
      case EIS_NotSupportedValue:
        return "Unsupported data value";
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
    while ((pin->Name != NULL) && (pin->Type != interpret))
        pin++;
    return pin->Name;
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
    return createScaledImage(0, 0, getWidth(), getHeight(), (unsigned long)(xfactor * getWidth()),
        (unsigned long)(yfactor * getHeight()), interpolate, aspect);
}


// --- clip & scale

DicomImage *DicomImage::createScaledImage(const unsigned long left,
                                          const unsigned long top,
                                          unsigned long clip_width,
                                          unsigned long clip_height,
                                          unsigned long scale_width,
                                          unsigned long scale_height,
                                          const int interpolate,
                                          int aspect) const
{
    const unsigned long gw = getWidth();
    const unsigned long gh = getHeight();
    if ((Image != NULL) && (gw > 0) && (gh > 0) && (left < gw) && (top < gh))
    {
        if ((clip_width == 0) || (left + clip_width > gw))           // set 'width' if missing or exceeding image's width
            clip_width = gw - left;
        if ((clip_height == 0) || (top + clip_height > gh))          // same for 'height'
            clip_height = gh - top;
        if ((scale_width == 0) && (scale_height == 0))
        {
            scale_width = clip_width;                                // auto-set width/height
            scale_height = clip_height;
        }
        if (aspect)                                                  // maintain pixel aspect ratio
        {
            if (scale_width == 0)
                scale_width = (unsigned long)(getWidthHeightRatio() * (double)(scale_height * gw) / gh);
            else if (scale_height == 0)
                scale_height = (unsigned long)(getHeightWidthRatio() * (double)(scale_width * gh) / gw);
            else
                aspect = 0;                                           // ignore pixel aspect ratio
        }
        else                                                          // ignore pixel aspect ratio
        {
            if (scale_width == 0)
                scale_width = (unsigned long)((double)(scale_height * gw) / gh);
            else if (scale_height == 0)
                scale_height = (unsigned long)((double)(scale_width * gh) / gw);
        }
        const unsigned long maxvalue = DicomImageClass::maxval(bitsof(Uint16));
        if (scale_width > maxvalue)
            scale_width = maxvalue;                                   // limit 'width' to maximum value (65535)
        if (scale_height > maxvalue)
            scale_height = maxvalue;                                  // same for 'height'
        if ((scale_width > 0) && (scale_height > 0))
        {
            DiImage *image = Image->createScale(left, top, clip_width, clip_height, scale_width, scale_height, interpolate, aspect);
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

DicomImage *DicomImage::createScaledImage(const unsigned long left,
                                          const unsigned long top,
                                          unsigned long width,
                                          unsigned long height,
                                          const double xfactor,
                                          const double yfactor,
                                          const int interpolate,
                                          const int aspect) const
{
    if ((xfactor >= 0) && (yfactor >= 0))
    {
        const unsigned long gw = getWidth();
        const unsigned long gh = getHeight();
        if ((width == 0) || (left + width > gw))           // set 'width' if missing or exceeding image's width
            width = gw - left;
        if ((height == 0) || (top + height > gh))          // same for 'height'
            height = gh - top;
        return createScaledImage(left, top, width, height, (unsigned long)(xfactor * width), (unsigned long)(yfactor * height),
            interpolate, aspect);
    }
    return NULL;
}


// --- create clipped to given box ('left', 'top' and 'width', 'height') image, memory isn't handled internally !
// --- 'width' and 'height' are optional

DicomImage *DicomImage::createClippedImage(const unsigned long left,
                                           const unsigned long top,
                                           unsigned long width,
                                           unsigned long height) const
{
    
    return createScaledImage(left, top, width, height);
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
            return Image->rotate((int)degree);
    }
    return 0;
}


// --- create by given 'degree' rotated image, memory isn't handled internally !

DicomImage *DicomImage::createRotatedImage(signed int degree) const
{
    if ((Image != NULL) && normalizeDegreeValue(degree))
    {
        DiImage *image = Image->createRotate((int)degree);
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

int DicomImage::writePPM(ostream &stream,
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


/*
 *
 * CVS/RCS Log:
 * $Log: dcmimage.cc,v $
 * Revision 1.8  1999-04-28 14:59:45  joergr
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
