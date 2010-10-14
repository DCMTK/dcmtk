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
 *  Purpose: DicomImage (Source)
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:14:18 $
 *  CVS/RCS Revision: $Revision: 1.46 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmdata/dctypes.h"
#include "dcmtk/dcmdata/dcdeftag.h"
#include "dcmtk/dcmdata/dcswap.h"
#include "dcmtk/dcmdata/dcuid.h"

#include "dcmtk/dcmimgle/diimage.h"
#include "dcmtk/dcmimgle/diinpxt.h"
#include "dcmtk/dcmimgle/didocu.h"
#include "dcmtk/dcmimgle/diutils.h"
#include "dcmtk/ofstd/ofstd.h"

#define INCLUDE_CSTRING
#include "dcmtk/ofstd/ofstdinc.h"


/*----------------*
 *  constructors  *
 *----------------*/

DiImage::DiImage(const DiDocument *docu,
                 const EI_Status status,
                 const int spp)
  : ImageStatus(status),
    Document(docu),
    FirstFrame(0),
    NumberOfFrames(0),
    TotalNumberOfFrames(0),
    RepresentativeFrame(0),
    Rows(0),
    Columns(0),
    PixelWidth(1),
    PixelHeight(1),
    BitsAllocated(0),
    BitsStored(0),
    HighBit(0),
    BitsPerSample(0),
    SamplesPerPixel(spp),
    Polarity(EPP_Normal),
    hasSignedRepresentation(0),
    hasPixelSpacing(0),
    hasImagerPixelSpacing(0),
    hasNominalScannedPixelSpacing(0),
    hasPixelAspectRatio(0),
    isOriginal(1),
    InputData(NULL),
    FileCache(),
    CurrentFragment(0)
{
    if ((Document != NULL) && (ImageStatus == EIS_Normal))
    {
        Sint32 sl = 0;
        if (Document->getValue(DCM_NumberOfFrames, sl))
        {
            if (sl < 1)
            {
                DCMIMGLE_WARN("invalid value for 'NumberOfFrames' (" << sl << ") ... assuming 1");
                NumberOfFrames = 1;
            } else
                NumberOfFrames = OFstatic_cast(Uint32, sl);
        } else
            NumberOfFrames = 1;
        Uint16 us = 0;
        if (Document->getValue(DCM_RepresentativeFrameNumber, us))
        {
            if (us <= FirstFrame)
            {
                DCMIMGLE_WARN("invalid value for 'RepresentativeFrameNumber' (" << us << ") ... assuming first frame");
                RepresentativeFrame = FirstFrame;
            }
            else if (us > NumberOfFrames)
            {
                DCMIMGLE_WARN("invalid value for 'RepresentativeFrameNumber' (" << us << ") ... assuming last frame");
                RepresentativeFrame = NumberOfFrames - 1;
            }
            else
                RepresentativeFrame = us - 1;
        }
        FirstFrame = (docu->getFrameStart() < NumberOfFrames) ? docu->getFrameStart() : NumberOfFrames - 1;
        /* store total number of frames in the dataset */
        TotalNumberOfFrames = NumberOfFrames;
        /* restrict to actually processed/loaded number of frames */
        NumberOfFrames -= FirstFrame;
        if ((docu->getFrameCount() > 0) && (NumberOfFrames > docu->getFrameCount()))
            NumberOfFrames = docu->getFrameCount();
        /* start from first processed frame (might still exceed number of loaded frames) */
        RepresentativeFrame -= FirstFrame;
        int ok = (Document->getValue(DCM_Rows, Rows) > 0);
        ok &= (Document->getValue(DCM_Columns, Columns) > 0);
        if (!ok || ((Rows > 0) && (Columns > 0)))
        {
            ok &= (Document->getValue(DCM_BitsAllocated, BitsAllocated) > 0);
            ok &= (Document->getValue(DCM_BitsStored, BitsStored) > 0);
            if (((Document->getValue(DCM_HighBit, HighBit) == 0) || ((HighBit == 0) && (BitsStored > 1))) && ok)
            {
                HighBit = BitsStored - 1;
                DCMIMGLE_WARN("missing value for 'HighBit' ... assuming " << HighBit);
            }
            ok &= (Document->getValue(DCM_PixelRepresentation, us) > 0);
            BitsPerSample = BitsStored;
            hasSignedRepresentation = (us == 1);
            if ((us != 0) && (us != 1))
                DCMIMGLE_WARN("invalid value for 'PixelRepresentation' (" << us << ") ... assuming 'unsigned' (0)");
            if (!(Document->getFlags() & CIF_UsePresentationState))
            {
                hasPixelSpacing = (Document->getValue(DCM_PixelSpacing, PixelHeight, 0) > 0);
                if (hasPixelSpacing)
                {
                    if (Document->getValue(DCM_PixelSpacing, PixelWidth, 1) < 2)
                        DCMIMGLE_WARN("missing second value for 'PixelSpacing' ... assuming 'Width' = " << PixelWidth);
                } else {
                    hasImagerPixelSpacing = (Document->getValue(DCM_ImagerPixelSpacing, PixelHeight, 0) > 0);
                    if (hasImagerPixelSpacing)
                    {
                        if (Document->getValue(DCM_ImagerPixelSpacing, PixelWidth, 1) < 2)
                            DCMIMGLE_WARN("missing second value for 'ImagerPixelSpacing' ... assuming 'Width' = " << PixelWidth);
                    } else {
                        hasNominalScannedPixelSpacing = (Document->getValue(DCM_NominalScannedPixelSpacing, PixelHeight, 0) > 0);
                        if (hasNominalScannedPixelSpacing)
                        {
                            if (Document->getValue(DCM_NominalScannedPixelSpacing, PixelWidth, 1) < 2)
                                DCMIMGLE_WARN("missing second value for 'NominalScannedPixelSpacing' ... assuming 'Width' = " << PixelWidth);
                        } else {
                            Sint32 sl2;
                            hasPixelAspectRatio = (Document->getValue(DCM_PixelAspectRatio, sl2, 0) > 0);
                            if (hasPixelAspectRatio)
                            {
                                PixelHeight = sl2;
                                if (Document->getValue(DCM_PixelAspectRatio, sl2, 1) < 2)
                                    DCMIMGLE_WARN("missing second value for 'PixelAspectRatio' ... assuming 'Width' = " << PixelWidth);
                                else
                                    PixelWidth = sl2;
                            } else {
                                PixelWidth = 1;
                                PixelHeight = 1;
                            }
                        }
                    }
                }
                checkPixelExtension();
            }
            if (ok && (Document->getPixelData() != NULL))
            {
                // convert pixel data (if present)
                convertPixelData();
            } else {
                ImageStatus = EIS_MissingAttribute;
                DCMIMGLE_ERROR("one or more mandatory attributes are missing in image pixel module");
            }
        } else {
            ImageStatus = EIS_InvalidValue;
            DCMIMGLE_ERROR("invalid value for 'Rows' (" << Rows << ") and/or 'Columns' (" << Columns << ")");
        }
    } else {
        ImageStatus = EIS_InvalidDocument;
        DCMIMGLE_ERROR("this DICOM document is invalid");
    }
}


DiImage::DiImage(const DiDocument *docu,
                 const EI_Status status)
  : ImageStatus(status),
    Document(docu),
    FirstFrame(0),
    NumberOfFrames(0),
    TotalNumberOfFrames(0),
    RepresentativeFrame(0),
    Rows(0),
    Columns(0),
    PixelWidth(1),
    PixelHeight(1),
    BitsAllocated(0),
    BitsStored(0),
    HighBit(0),
    BitsPerSample(0),
    SamplesPerPixel(0),
    Polarity(EPP_Normal),
    hasSignedRepresentation(0),
    hasPixelSpacing(0),
    hasImagerPixelSpacing(0),
    hasNominalScannedPixelSpacing(0),
    hasPixelAspectRatio(0),
    isOriginal(1),
    InputData(NULL),
    FileCache(),
    CurrentFragment(0)
{
}


DiImage::DiImage(const DiImage *image,
                 const unsigned long fstart,
                 const unsigned long fcount)
  : ImageStatus(image->ImageStatus),
    Document(image->Document),
    FirstFrame(image->FirstFrame + fstart),
    NumberOfFrames(fcount),
    TotalNumberOfFrames(image->TotalNumberOfFrames),
    RepresentativeFrame(image->RepresentativeFrame),
    Rows(image->Rows),
    Columns(image->Columns),
    PixelWidth(image->PixelWidth),
    PixelHeight(image->PixelHeight),
    BitsAllocated(image->BitsAllocated),
    BitsStored(image->BitsStored),
    HighBit(image->HighBit),
    BitsPerSample(image->BitsPerSample),
    SamplesPerPixel(image->SamplesPerPixel),
    Polarity(image->Polarity),
    hasSignedRepresentation(image->hasSignedRepresentation),
    hasPixelSpacing(image->hasPixelSpacing),
    hasImagerPixelSpacing(image->hasImagerPixelSpacing),
    hasNominalScannedPixelSpacing(image->hasNominalScannedPixelSpacing),
    hasPixelAspectRatio(image->hasPixelAspectRatio),
    isOriginal(0),
    InputData(NULL),
    FileCache(),
    CurrentFragment(0)
{
}


/* constructor: image scaled */

DiImage::DiImage(const DiImage *image,
                 const Uint16 columns,
                 const Uint16 rows,
                 const int aspect)
  : ImageStatus(image->ImageStatus),
    Document(image->Document),
    FirstFrame(image->FirstFrame),
    NumberOfFrames(image->NumberOfFrames),
    TotalNumberOfFrames(image->TotalNumberOfFrames),
    RepresentativeFrame(image->RepresentativeFrame),
    Rows(rows),
    Columns(columns),
    PixelWidth(1),
    PixelHeight(1),
    BitsAllocated(image->BitsAllocated),
    BitsStored(image->BitsStored),
    HighBit(image->HighBit),
    BitsPerSample(image->BitsPerSample),
    SamplesPerPixel(image->SamplesPerPixel),
    Polarity(image->Polarity),
    hasSignedRepresentation(image->hasSignedRepresentation),
    hasPixelSpacing(0),
    hasImagerPixelSpacing(0),
    hasNominalScannedPixelSpacing(0),
    hasPixelAspectRatio(0),
    isOriginal(0),
    InputData(NULL),
    FileCache(),
    CurrentFragment(0)
{
    /* we do not check for "division by zero", this is already done somewhere else */
    const double xfactor = OFstatic_cast(double, Columns) / OFstatic_cast(double, image->Columns);
    const double yfactor = OFstatic_cast(double, Rows) / OFstatic_cast(double, image->Rows);
    /* re-compute pixel width and height */
    if (image->hasPixelSpacing)
    {
        hasPixelSpacing = image->hasPixelSpacing;
        PixelWidth = image->PixelWidth / xfactor;
        PixelHeight = image->PixelHeight / yfactor;
    }
    else if (image->hasImagerPixelSpacing)
    {
        /* never change ImagerPixelSpacing, use PixelSpacing instead */
        hasImagerPixelSpacing = 0;
        hasPixelSpacing = 1;
        PixelWidth = image->PixelWidth / xfactor;
        PixelHeight = image->PixelHeight / yfactor;
    }
    else if (image->hasNominalScannedPixelSpacing)
    {
        /* never change NominalScannedPixelSpacing, use PixelSpacing instead */
        hasNominalScannedPixelSpacing = 0;
        hasPixelSpacing = 1;
        PixelWidth = image->PixelWidth / xfactor;
        PixelHeight = image->PixelHeight / yfactor;
    }
    else if (image->hasPixelAspectRatio && !aspect /*recognize pixel aspect ratio*/)
    {
        hasPixelAspectRatio = image->hasPixelAspectRatio;
        PixelWidth = image->PixelWidth * xfactor;
        PixelHeight = image->PixelHeight * yfactor;
        /* do not store pixel aspect ratio for square pixels */
        if (PixelWidth == PixelHeight)
            hasPixelAspectRatio = 0;
    }
}


DiImage::DiImage(const DiImage *image,
                 const int degree)
  : ImageStatus(image->ImageStatus),
    Document(image->Document),
    FirstFrame(image->FirstFrame),
    NumberOfFrames(image->NumberOfFrames),
    TotalNumberOfFrames(image->TotalNumberOfFrames),
    RepresentativeFrame(image->RepresentativeFrame),
    Rows(((degree == 90) ||(degree == 270)) ? image->Columns : image->Rows),
    Columns(((degree == 90) ||(degree == 270)) ? image->Rows : image->Columns),
    PixelWidth(((degree == 90) ||(degree == 270)) ? image->PixelHeight : image->PixelWidth),
    PixelHeight(((degree == 90) ||(degree == 270)) ? image-> PixelWidth : image->PixelHeight),
    BitsAllocated(image->BitsAllocated),
    BitsStored(image->BitsStored),
    HighBit(image->HighBit),
    BitsPerSample(image->BitsPerSample),
    SamplesPerPixel(image->SamplesPerPixel),
    Polarity(image->Polarity),
    hasSignedRepresentation(image->hasSignedRepresentation),
    hasPixelSpacing(image->hasPixelSpacing),
    hasImagerPixelSpacing(image->hasImagerPixelSpacing),
    hasNominalScannedPixelSpacing(image->hasNominalScannedPixelSpacing),
    hasPixelAspectRatio(image->hasPixelAspectRatio),
    isOriginal(0),
    InputData(NULL),
    FileCache(),
    CurrentFragment(0)
{
}


DiImage::DiImage(const DiImage *image,
                 const unsigned long frame,
                 const int stored,
                 const int alloc)
  : ImageStatus(image->ImageStatus),
    Document(image->Document),
    FirstFrame(frame),
    NumberOfFrames(1),
    TotalNumberOfFrames(image->TotalNumberOfFrames),
    RepresentativeFrame(0),
    Rows(image->Rows),
    Columns(image->Columns),
    PixelWidth(image->PixelWidth),
    PixelHeight(image->PixelHeight),
    BitsAllocated(alloc),
    BitsStored(stored),
    HighBit(stored - 1),
    BitsPerSample(image->BitsPerSample),
    SamplesPerPixel(image->SamplesPerPixel),
    Polarity(image->Polarity),
    hasSignedRepresentation(0),
    hasPixelSpacing(image->hasPixelSpacing),
    hasImagerPixelSpacing(image->hasImagerPixelSpacing),
    hasNominalScannedPixelSpacing(image->hasNominalScannedPixelSpacing),
    hasPixelAspectRatio(image->hasPixelAspectRatio),
    isOriginal(0),
    InputData(NULL),
    FileCache(),
    CurrentFragment(0)
{
}


/*--------------*
 *  destructor  *
 *--------------*/

DiImage::~DiImage()
{
    delete InputData;
}


/*********************************************************************/


int DiImage::processNextFrames(const unsigned long fcount)
{
    if ((ImageStatus == EIS_Normal) && (Document != NULL) && isOriginal)
    {
        if ((Document->getFlags() & CIF_UsePartialAccessToPixelData) && (Document->getPixelData() != NULL))
        {
            // check whether there are still any frames to be processed
            if (FirstFrame + NumberOfFrames < TotalNumberOfFrames)
            {
                FirstFrame += NumberOfFrames;
                if (fcount > 0)
                    NumberOfFrames = fcount;
                if (FirstFrame + NumberOfFrames > TotalNumberOfFrames)
                    NumberOfFrames = TotalNumberOfFrames - FirstFrame;
                // free memory of previously processed frames
                deleteInputData();
                // create new input data representation
                convertPixelData();
                return (ImageStatus == EIS_Normal);
            }
        }
    }
    return 0;
}


/********************************************************************/


int DiImage::rotate(const int degree)
{
    if ((degree == 90) || (degree == 270))
    {
        Uint16 us = Rows;                   // swap image width and height
        Rows = Columns;
        Columns = us;
        double db = PixelWidth;             // swap pixel width and height
        PixelWidth = PixelHeight;
        PixelHeight = db;
        return 1;
    }
    return 0;
}


/********************************************************************/


void DiImage::deleteInputData()
{
    delete InputData;
    InputData = NULL;
}


void DiImage::checkPixelExtension()
{
    if (hasPixelSpacing || hasImagerPixelSpacing || hasNominalScannedPixelSpacing || hasPixelAspectRatio)
    {
        if (PixelHeight == 0)
        {
            DCMIMGLE_WARN("invalid value for 'PixelHeight' (" << PixelHeight << ") ... assuming 1");
            /* according to the standard, a value of 0 would be valid in case number of rows is 1 */
            PixelHeight = 1;
        }
        else if (PixelHeight < 0)
        {
            DCMIMGLE_WARN("negative value for 'PixelHeight' (" << PixelHeight << ") ... assuming " << -PixelHeight);
            PixelHeight = -PixelHeight;
        }
        if (PixelWidth == 0)
        {
            DCMIMGLE_WARN("invalid value for 'PixelWidth' (" << PixelWidth << ") ... assuming 1");
            /* according to the standard, a value of 0 would be valid in case number of columns is 1 */
            PixelWidth = 1;
        }
        else if (PixelWidth < 0)
        {
            DCMIMGLE_WARN("negative value for 'PixelWidth' (" << PixelWidth << ") ... assuming " << -PixelWidth);
            PixelHeight = -PixelHeight;
        }
    }
}


void DiImage::convertPixelData()
{
    // pointer is already checked for not being NULL
    DcmPixelData *pixel = Document->getPixelData();
    const DcmEVR evr = pixel->getVR();
    const OFBool compressed = Document->isCompressed();
    const char *xferUID = DcmXfer(Document->getTransferSyntax()).getXferID();
    const char *xferName = dcmFindNameOfUID(xferUID);
    // output some useful information, e.g. for tracing purposes
    DCMIMGLE_TRACE("Pixel Data VR: " << DcmVR(evr).getVRName() << ", " << (compressed ? "Compressed" : "Uncompressed")
        << " (" << ((xferName != NULL) ? xferName : xferUID) << ")");
    DCMIMGLE_TRACE("First frame: " << FirstFrame << ", Number of frames: " << NumberOfFrames
        << ", Total number of frames: " << TotalNumberOfFrames);
    DCMIMGLE_TRACE("Columns: " << Columns << ", Rows: " << Rows << ", Samples per Pixel: " << SamplesPerPixel
        << ", Photometric Interpretation: " << Document->getPhotometricInterpretation());
    DCMIMGLE_TRACE("Bits Allocated: " << BitsAllocated << ", Bits Stored: " << BitsStored << ", High Bit: " << HighBit
        << ", " << (hasSignedRepresentation ? "Signed" : "Unsigned") << " integer");
    /* check for valid/supported pixel data encoding */
    if ((evr == EVR_OW) || ((evr == EVR_OB) && (compressed || (BitsAllocated <= 16))))
    {
        const unsigned long fsize = OFstatic_cast(unsigned long, Rows) * OFstatic_cast(unsigned long, Columns) *
            OFstatic_cast(unsigned long, SamplesPerPixel);
        if ((BitsAllocated < 1) || (BitsStored < 1) || (BitsAllocated < BitsStored) ||
            (BitsStored > OFstatic_cast(Uint16, HighBit + 1)))
        {
            ImageStatus = EIS_InvalidValue;
            DCMIMGLE_ERROR("invalid values for 'BitsAllocated' (" << BitsAllocated << "), "
                << "'BitsStored' (" << BitsStored << ") and/or 'HighBit' (" << HighBit << ")");
            return;
        }
        else if ((evr == EVR_OB) && (BitsStored <= 8))
        {
            // report a warning message on this standard violation
            if (!compressed && (BitsAllocated > 8))
                DCMIMGLE_WARN("invalid value for 'BitsAllocated' (" << BitsAllocated << "), > 8 for OB encoded uncompressed 'PixelData'");
            if (hasSignedRepresentation)
                InputData = new DiInputPixelTemplate<Uint8, Sint8>(Document, BitsAllocated, BitsStored, HighBit, FirstFrame, NumberOfFrames, fsize, &FileCache, CurrentFragment);
            else
                InputData = new DiInputPixelTemplate<Uint8, Uint8>(Document, BitsAllocated, BitsStored, HighBit, FirstFrame, NumberOfFrames, fsize, &FileCache, CurrentFragment);
        }
        else if ((evr == EVR_OB) && (BitsStored <= 16))
        {
            // report a warning message on this standard violation
            if (!compressed && (BitsAllocated > 8))
                DCMIMGLE_WARN("invalid value for 'BitsAllocated' (" << BitsAllocated << "), > 8 for OB encoded uncompressed 'PixelData'");
            if (hasSignedRepresentation)
                InputData = new DiInputPixelTemplate<Uint8, Sint16>(Document, BitsAllocated, BitsStored, HighBit, FirstFrame, NumberOfFrames, fsize, &FileCache, CurrentFragment);
            else
                InputData = new DiInputPixelTemplate<Uint8, Uint16>(Document, BitsAllocated, BitsStored, HighBit, FirstFrame, NumberOfFrames, fsize, &FileCache, CurrentFragment);
        }
        else if ((evr == EVR_OB) && compressed && (BitsStored <= 32))
        {
            if (hasSignedRepresentation)
                InputData = new DiInputPixelTemplate<Uint8, Sint32>(Document, BitsAllocated, BitsStored, HighBit, FirstFrame, NumberOfFrames, fsize, &FileCache, CurrentFragment);
            else
                InputData = new DiInputPixelTemplate<Uint8, Uint32>(Document, BitsAllocated, BitsStored, HighBit, FirstFrame, NumberOfFrames, fsize, &FileCache, CurrentFragment);
        }
        else if (BitsStored <= 8)
        {
            if (hasSignedRepresentation)
                InputData = new DiInputPixelTemplate<Uint16, Sint8>(Document, BitsAllocated, BitsStored, HighBit, FirstFrame, NumberOfFrames, fsize, &FileCache, CurrentFragment);
            else
                InputData = new DiInputPixelTemplate<Uint16, Uint8>(Document, BitsAllocated, BitsStored, HighBit, FirstFrame, NumberOfFrames, fsize, &FileCache, CurrentFragment);
        }
        else if (BitsStored <= 16)
        {
            if (hasSignedRepresentation)
                InputData = new DiInputPixelTemplate<Uint16, Sint16>(Document, BitsAllocated, BitsStored, HighBit, FirstFrame, NumberOfFrames, fsize, &FileCache, CurrentFragment);
            else
                InputData = new DiInputPixelTemplate<Uint16, Uint16>(Document, BitsAllocated, BitsStored, HighBit, FirstFrame, NumberOfFrames, fsize, &FileCache, CurrentFragment);
        }
        else if (BitsStored <= 32)
        {
            if (hasSignedRepresentation)
                InputData = new DiInputPixelTemplate<Uint16, Sint32>(Document, BitsAllocated, BitsStored, HighBit, FirstFrame, NumberOfFrames, fsize, &FileCache, CurrentFragment);
            else
                InputData = new DiInputPixelTemplate<Uint16, Uint32>(Document, BitsAllocated, BitsStored, HighBit, FirstFrame, NumberOfFrames, fsize, &FileCache, CurrentFragment);
        }
        else    /* BitsStored > 32 !! */
        {
            ImageStatus = EIS_NotSupportedValue;
            DCMIMGLE_ERROR("invalid value for 'BitsStored' (" << BitsStored << ") ... exceeds " << MAX_BITS << " bit");
            return;
        }
        if (InputData == NULL)
        {
            ImageStatus = EIS_MemoryFailure;
            DCMIMGLE_ERROR("can't allocate memory for input-representation");
        }
        else if (InputData->getData() == NULL)
        {
            ImageStatus = EIS_InvalidImage;
            DCMIMGLE_ERROR("can't convert input pixel data, probably unsupported compression");
        }
        else if (InputData->getPixelStart() >= InputData->getCount())
        {
            ImageStatus = EIS_InvalidValue;
            DCMIMGLE_ERROR("start offset (" << InputData->getPixelStart() << ") exceeds number of pixels stored ("
                << InputData->getCount() << ")");
        }
    }
    else
    {
        ImageStatus = EIS_NotSupportedValue;
        DCMIMGLE_ERROR("'PixelData' has an other value representation than OB (with 'BitsAllocated' <= 16) or OW");
    }
}


int DiImage::detachPixelData()
{
    if ((Document != NULL) && isOriginal)
    {
        if (Document->getFlags() & CIF_MayDetachPixelData)
        {
            /* do not detach if partial access is enabled */
            if (!(Document->getFlags() & CIF_UsePartialAccessToPixelData) || (FirstFrame + NumberOfFrames >= TotalNumberOfFrames))
            {
                DcmPixelData *pixel = Document->getPixelData();
                if (pixel != NULL)
                {
                    DCMIMGLE_DEBUG("detach pixel data");
                    /* clear pixel data value */
                    pixel->putUint16Array(NULL, 0);
                    return 1;
                }
            } else
                DCMIMGLE_DEBUG("do not detach pixel data because of partial access");
        }
    }
    return 0;
}


int DiImage::setColumnRowRatio(const double ratio)
{
    hasPixelAspectRatio = 1;
    hasPixelSpacing = hasImagerPixelSpacing = hasNominalScannedPixelSpacing = 0;
    PixelWidth = ratio;
    PixelHeight = 1;
    checkPixelExtension();
    return 1;
}


int DiImage::setRowColumnRatio(const double ratio)
{
    hasPixelAspectRatio = 1;
    hasPixelSpacing = hasImagerPixelSpacing = hasNominalScannedPixelSpacing = 0;
    PixelWidth = 1;
    PixelHeight = ratio;
    checkPixelExtension();
    return 1;
}


int DiImage::setPolarity(const EP_Polarity polarity)
{
    if (polarity != Polarity)
    {
        Polarity = polarity;
        return 1;
    }
    return 2;
}


void DiImage::updateImagePixelModuleAttributes(DcmItem &dataset)
{
    /* remove outdated attributes from the dataset */
    delete dataset.remove(DCM_SmallestImagePixelValue);
    delete dataset.remove(DCM_LargestImagePixelValue);
/*
    delete dataset.remove(DCM_PixelPaddingValue);
    delete dataset.remove(DCM_PixelPaddingRangeLimit);
    delete dataset.remove(DCM_SmallestPixelValueInSeries);
    delete dataset.remove(DCM_LargestPixelValueInSeries);
*/
    /* update PixelAspectRatio & Co. */
    char buffer[32];
    OFStandard::ftoa(buffer, 15, PixelHeight, OFStandard::ftoa_format_f);
    strcat(buffer, "\\");
    OFStandard::ftoa(strchr(buffer, 0), 15, PixelWidth, OFStandard::ftoa_format_f);

    if (hasPixelSpacing)
        dataset.putAndInsertString(DCM_PixelSpacing, buffer);
    else
        delete dataset.remove(DCM_PixelSpacing);
    /* check for square pixels */
    if (hasPixelAspectRatio && (PixelWidth != PixelHeight))
        dataset.putAndInsertString(DCM_PixelAspectRatio, buffer);
    else
        delete dataset.remove(DCM_PixelAspectRatio);
}


// --- write given frame of the current image to DICOM dataset

int DiImage::writeFrameToDataset(DcmItem &dataset,
                                 const unsigned long frame,
                                 const int bits,
                                 const int planar)
{
    int result = 0;
    const int bitsStored = getBits(bits);
    /* get output pixel data */
    const void *pixel = getOutputData(frame, bitsStored, planar);
    if (pixel != NULL)
    {
        char buffer[32];
        unsigned long count;
        /* write color model dependent attributes */
        if ((getInternalColorModel() == EPI_Monochrome1) || (getInternalColorModel() == EPI_Monochrome2))
        {
            /* monochrome image */
            count = OFstatic_cast(unsigned long, Columns) * OFstatic_cast(unsigned long, Rows);
            dataset.putAndInsertString(DCM_PhotometricInterpretation, "MONOCHROME2");
            dataset.putAndInsertUint16(DCM_SamplesPerPixel, 1);
        } else {
            /* color image */
            count = OFstatic_cast(unsigned long, Columns) * OFstatic_cast(unsigned long, Rows) * 3 /*samples per pixel*/;
            if (getInternalColorModel() == EPI_YBR_Full)
                dataset.putAndInsertString(DCM_PhotometricInterpretation, "YBR_FULL");
            else
                dataset.putAndInsertString(DCM_PhotometricInterpretation, "RGB");
            dataset.putAndInsertUint16(DCM_PlanarConfiguration, planar != 0);
            dataset.putAndInsertUint16(DCM_SamplesPerPixel, 3);
        }
        /* write remaining attributes */
        dataset.putAndInsertUint16(DCM_Columns, Columns);
        dataset.putAndInsertUint16(DCM_Rows, Rows);
        dataset.putAndInsertString(DCM_NumberOfFrames, "1");
        if (bitsStored <= 8)
            dataset.putAndInsertUint16(DCM_BitsAllocated, 8);
        else if (bitsStored <= 16)
            dataset.putAndInsertUint16(DCM_BitsAllocated, 16);
        else
            dataset.putAndInsertUint16(DCM_BitsAllocated, 32);
        dataset.putAndInsertUint16(DCM_BitsStored, bitsStored);
        dataset.putAndInsertUint16(DCM_HighBit, bitsStored - 1);
        dataset.putAndInsertUint16(DCM_PixelRepresentation, 0);
        /* handle VOI transformations */
        if (dataset.tagExists(DCM_WindowCenter) ||
            dataset.tagExists(DCM_WindowWidth) ||
            dataset.tagExists(DCM_VOILUTSequence))
        {
            delete dataset.remove(DCM_VOILUTSequence);
            sprintf(buffer, "%lu", DicomImageClass::maxval(bitsStored, 0) / 2);
            dataset.putAndInsertString(DCM_WindowCenter, buffer);
            sprintf(buffer, "%lu", DicomImageClass::maxval(bitsStored, 0));
            dataset.putAndInsertString(DCM_WindowWidth, buffer);
        }
        delete dataset.remove(DCM_WindowCenterWidthExplanation);
        /* write pixel data (OB or OW) */
        if (bitsStored <= 8)
            dataset.putAndInsertUint8Array(DCM_PixelData, OFstatic_cast(Uint8 *, OFconst_cast(void *, pixel)), count);
        else if (bitsStored <= 16)
            dataset.putAndInsertUint16Array(DCM_PixelData, OFstatic_cast(Uint16 *, OFconst_cast(void *, pixel)), count);
        else
            dataset.putAndInsertUint16Array(DCM_PixelData, OFstatic_cast(Uint16 *, OFconst_cast(void *, pixel)), count * 2 /*double-words*/);
        /* update other DICOM attributes */
        updateImagePixelModuleAttributes(dataset);
        result = 1;
    }
    return result;
}


int DiImage::writeBMP(FILE *stream,
                      const unsigned long frame,
                      const int bits)
{
    int result = 0;
    if ((stream != NULL) && ((bits == 8) || (bits == 24) || (bits == 32)))
    {
        /* create device independent bitmap: palette (8) or truecolor (24/32) */
        void *data = NULL;
        const unsigned long bytes = createDIB(data, 0, frame, bits, 1 /*upsideDown*/);
        if ((data != NULL) && (bytes > 0))
        {
            /* number of bytes */
            SB_BitmapFileHeader fileHeader;
            SB_BitmapInfoHeader infoHeader;
            Uint32 *palette = (bits == 8) ? new Uint32[256] : OFstatic_cast(Uint32 *, NULL);
            /* fill bitmap file header with data */
            fileHeader.bfType[0] = 'B';
            fileHeader.bfType[1] = 'M';
            fileHeader.bfSize = 14 /*sizeof(SB_BitmapFileHeader)*/ + 40 /*sizeof(SB_BitmapInfoHeader)*/ + bytes;
            fileHeader.bfReserved1 = 0;
            fileHeader.bfReserved2 = 0;
            fileHeader.bfOffBits = 14 /*sizeof(SB_BitmapFileHeader)*/ + 40 /*sizeof(SB_BitmapInfoHeader)*/;
            /* fill bitmap info header with data */
            infoHeader.biSize = 40 /*sizeof(SB_BitmapInfoHeader)*/;
            infoHeader.biWidth = Columns;
            infoHeader.biHeight = Rows;
            infoHeader.biPlanes = 1;
            infoHeader.biBitCount = bits;
            infoHeader.biCompression = 0;
            infoHeader.biSizeImage = 0;
            infoHeader.biXPelsPerMeter = 0;
            infoHeader.biYPelsPerMeter = 0;
            infoHeader.biClrUsed = 0;
            infoHeader.biClrImportant = 0;
            /* create and fill color palette */
            if (palette != NULL)
            {
                /* add palette size */
                fileHeader.bfSize += 256 * 4;
                fileHeader.bfOffBits += 256 * 4;
                /* fill palette entries with gray values */
                for (Uint32 i = 0; i < 256; ++i)
                    palette[i] = (i << 16) | (i << 8) | i;
            }
            /* swap bytes if necessary */
            if (gLocalByteOrder != EBO_LittleEndian)
            {
                /* other data elements are always '0' and, therefore, can remain as they are */
                swap4Bytes(OFreinterpret_cast(Uint8 *, &fileHeader.bfSize));
                swap4Bytes(OFreinterpret_cast(Uint8 *, &fileHeader.bfOffBits));
                swap4Bytes(OFreinterpret_cast(Uint8 *, &infoHeader.biSize));
                swap4Bytes(OFreinterpret_cast(Uint8 *, &infoHeader.biWidth));
                swap4Bytes(OFreinterpret_cast(Uint8 *, &infoHeader.biHeight));
                swap2Bytes(OFreinterpret_cast(Uint8 *, &infoHeader.biPlanes));
                swap2Bytes(OFreinterpret_cast(Uint8 *, &infoHeader.biBitCount));
                if (palette != NULL)
                    swapBytes(OFreinterpret_cast(Uint8 *, palette), 256 * 4 /*byteLength*/, 4 /*valWidth*/);
            }
            /* write bitmap file header: do not write the struct because of 32-bit alignment */
            fwrite(&fileHeader.bfType, sizeof(fileHeader.bfType), 1, stream);
            fwrite(&fileHeader.bfSize, sizeof(fileHeader.bfSize), 1, stream);
            fwrite(&fileHeader.bfReserved1, sizeof(fileHeader.bfReserved1), 1, stream);
            fwrite(&fileHeader.bfReserved2, sizeof(fileHeader.bfReserved2), 1, stream);
            fwrite(&fileHeader.bfOffBits, sizeof(fileHeader.bfOffBits), 1, stream);
            /* write bitmap info header: do not write the struct because of 32-bit alignment  */
            fwrite(&infoHeader.biSize, sizeof(infoHeader.biSize), 1, stream);
            fwrite(&infoHeader.biWidth, sizeof(infoHeader.biWidth), 1, stream);
            fwrite(&infoHeader.biHeight, sizeof(infoHeader.biHeight), 1, stream);
            fwrite(&infoHeader.biPlanes, sizeof(infoHeader.biPlanes), 1, stream);
            fwrite(&infoHeader.biBitCount, sizeof(infoHeader.biBitCount), 1, stream);
            fwrite(&infoHeader.biCompression, sizeof(infoHeader.biCompression), 1, stream);
            fwrite(&infoHeader.biSizeImage, sizeof(infoHeader.biSizeImage), 1, stream);
            fwrite(&infoHeader.biXPelsPerMeter, sizeof(infoHeader.biXPelsPerMeter), 1, stream);
            fwrite(&infoHeader.biYPelsPerMeter, sizeof(infoHeader.biYPelsPerMeter), 1, stream);
            fwrite(&infoHeader.biClrUsed, sizeof(infoHeader.biClrUsed), 1, stream);
            fwrite(&infoHeader.biClrImportant, sizeof(infoHeader.biClrImportant), 1, stream);
            /* write color palette (if applicable) */
            if (palette != NULL)
                fwrite(palette, 4, 256, stream);
            /* write pixel data */
            fwrite(data, 1, OFstatic_cast(size_t, bytes), stream);
            /* delete color palette */
            delete[] palette;
            result = 1;
        }
        /* delete pixel data */
        delete OFstatic_cast(char *, data);     // type cast necessary to avoid compiler warnings using gcc >2.95
    }
    return result;
}


/*
 *
 * CVS/RCS Log:
 * $Log: diimage.cc,v $
 * Revision 1.46  2010-10-14 13:14:18  joergr
 * Updated copyright header. Added reference to COPYRIGHT file.
 *
 * Revision 1.45  2010-10-05 14:56:19  joergr
 * Also remove PixelPaddingRangeLimit element from the dataset (if required).
 *
 * Revision 1.44  2010-03-05 13:33:08  joergr
 * Fixed issue with processNextFrames() when called with non-default parameter.
 *
 * Revision 1.43  2009-11-25 16:39:17  joergr
 * Adapted code for new approach to access individual frames of a DICOM image.
 *
 * Revision 1.42  2009-10-28 14:26:02  joergr
 * Fixed minor issues in log output.
 *
 * Revision 1.41  2009-10-28 09:53:40  uli
 * Switched to logging mechanism provided by the "new" oflog module.
 *
 * Revision 1.40  2009-04-03 11:46:22  joergr
 * Do not write PixelAspectRatio for square pixels (width == height).
 *
 * Revision 1.39  2009-04-03 10:29:08  joergr
 * Avoid setting the value of PixelSpacing and PixelAspectRatio twice.
 *
 * Revision 1.38  2009-02-12 12:05:28  joergr
 * Never update value of ImagerPixelSpacing when image is scaled, use
 * PixelSpacing instead.
 * Added support for NominalScannedPixelSpacing in order to determine the pixel
 * aspect ratio (used for the new SC image IODs).
 *
 * Revision 1.37  2007-10-23 16:53:04  joergr
 * Fixed bug in writeFrameToDataset() for images with BitsAllocated = 32.
 *
 * Revision 1.36  2006/08/15 16:30:11  meichel
 * Updated the code in module dcmimgle to correctly compile when
 *   all standard C++ classes remain in namespace std.
 *
 * Revision 1.35  2006/07/27 14:02:40  joergr
 * Fixed typo.
 *
 * Revision 1.34  2006/07/19 08:31:52  joergr
 * Fixed wrong warning when HightBit = 0 and BitsStored = 1.
 *
 * Revision 1.33  2006/07/10 10:54:26  joergr
 * Added support for 32-bit BMP images.
 *
 * Revision 1.32  2005/12/08 15:42:51  meichel
 * Changed include path schema for all DCMTK header files
 *
 * Revision 1.31  2005/12/01 09:52:07  joergr
 * Added heuristics for images where the value of HighBit is 0.
 *
 * Revision 1.30  2005/03/09 17:41:16  joergr
 * Added heuristics for images where the attribute HighBit is missing.
 * Fixed possibly uninitialized variable when determining the pixel height.
 *
 * Revision 1.29  2004/09/22 11:35:01  joergr
 * Introduced new member variable "TotalNumberOfFrames".
 *
 * Revision 1.28  2004/03/16 08:18:54  joergr
 * Added support for non-standard encoding of pixel data (OB with BitsAllocated
 * > 8 and <= 16).
 *
 * Revision 1.27  2004/02/06 11:09:21  joergr
 * Fixed inconsistency in re-calculation of PixelSpacing and ImagerPixelSpacing
 * when scaling images.
 *
 * Revision 1.26  2003/12/23 16:03:18  joergr
 * Replaced post-increment/decrement operators by pre-increment/decrement
 * operators where appropriate (e.g. 'i++' by '++i').
 *
 * Revision 1.25  2003/12/08 17:17:04  joergr
 * Adapted type casts to new-style typecast operators defined in ofcast.h.
 *
 * Revision 1.24  2002/12/13 09:28:22  joergr
 * Added explicit type cast to pointer initialization to avoid warning reported
 * by gcc 2.7.2.1.
 *
 * Revision 1.23  2002/12/04 10:41:23  meichel
 * Changed toolkit to use OFStandard::ftoa instead of sprintf for all
 *   double to string conversions that are supposed to be locale independent
 *
 * Revision 1.22  2002/11/26 14:48:12  joergr
 * Added Smallest/LargestImagePixelValue to the list of attributes to be
 * removed from a newly created dataset.
 *
 * Revision 1.21  2002/08/02 15:04:53  joergr
 * Enhanced writeFrameToDataset() routine (remove out-data DICOM attributes
 * from the dataset).
 * Re-compute Imager/Pixel Spacing and Pixel Aspect Ratio for scaled images.
 *
 * Revision 1.20  2002/06/26 16:11:12  joergr
 * Added support for polarity flag to color images.
 * Added new method to write a selected frame to a DICOM dataset (incl. required
 * attributes from the "Image Pixel Module").
 *
 * Revision 1.19  2001/11/29 16:59:53  joergr
 * Fixed bug in dcmimgle that caused incorrect decoding of some JPEG compressed
 * images (certain DICOM attributes, e.g. photometric interpretation, might
 * change during decompression process).
 *
 * Revision 1.18  2001/11/27 18:21:38  joergr
 * Added support for plugable output formats in class DicomImage. First
 * implementation is JPEG.
 *
 * Revision 1.17  2001/11/19 12:57:17  joergr
 * Adapted code to support new dcmjpeg module and JPEG compressed images.
 *
 * Revision 1.16  2001/11/13 18:01:41  joergr
 * Added type cast to delete a void pointer to keep gcc 2.95 compiler quiet.
 *
 * Revision 1.15  2001/11/09 16:29:04  joergr
 * Added support for Windows BMP file format.
 *
 * Revision 1.14  2001/09/28 13:14:22  joergr
 * Corrected wrong warning message regarding the optional RepresentativeFrame
 * Number.
 *
 * Revision 1.13  2001/06/01 15:49:55  meichel
 * Updated copyright header
 *
 * Revision 1.12  2000/05/25 10:35:02  joergr
 * Removed ununsed variable from parameter list (avoid compiler warnings).
 *
 * Revision 1.11  2000/05/03 09:47:23  joergr
 * Removed most informational and some warning messages from release built
 * (#ifndef DEBUG).
 *
 * Revision 1.10  2000/04/28 12:33:44  joergr
 * DebugLevel - global for the module - now derived from OFGlobal (MT-safe).
 *
 * Revision 1.9  2000/04/27 13:10:27  joergr
 * Dcmimgle library code now consistently uses ofConsole for error output.
 *
 * Revision 1.8  2000/03/08 16:24:28  meichel
 * Updated copyright header.
 *
 * Revision 1.7  2000/03/03 14:09:18  meichel
 * Implemented library support for redirecting error messages into memory
 *   instead of printing them to stdout/stderr for GUI applications.
 *
 * Revision 1.6  1999/10/06 13:45:55  joergr
 * Corrected creation of PrintBitmap pixel data: VOI windows should be applied
 * before clipping to avoid that the region outside the image (border) is also
 * windowed (this requires a new method in dcmimgle to create a DicomImage
 * with the grayscale transformations already applied).
 *
 * Revision 1.5  1999/09/17 13:15:20  joergr
 * Corrected typos and formatting.
 *
 * Revision 1.4  1999/07/23 14:21:31  joergr
 * Corrected bug in method 'detachPixelData' (data has never really been
 * removed from memory).
 *
 * Revision 1.2  1999/04/28 15:01:44  joergr
 * Introduced new scheme for the debug level variable: now each level can be
 * set separately (there is no "include" relationship).
 *
 * Revision 1.1  1998/11/27 16:01:43  joergr
 * Added copyright message.
 * Added methods and constructors for flipping and rotating, changed for
 * scaling and clipping.
 * Added method to directly create java AWT bitmaps.
 * Introduced global debug level for dcmimage module to control error output.
 * Renamed variable 'Status' to 'ImageStatus' because of possible conflicts
 * with X windows systems.
 * Added method to detach pixel data if it is no longer needed.
 *
 * Revision 1.7  1998/06/25 08:51:41  joergr
 * Removed some wrong newline characters.
 *
 * Revision 1.6  1998/05/11 14:52:29  joergr
 * Added CVS/RCS header to each file.
 *
 *
 */
