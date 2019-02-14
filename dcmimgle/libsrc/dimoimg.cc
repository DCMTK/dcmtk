/*
 *
 *  Copyright (C) 1996-2019 OFFIS e.V.
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
 *  Purpose: DicomMonochromeImage (Source)
 *
 */


#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmdata/dctypes.h"
#include "dcmtk/dcmdata/dcdeftag.h"

#include "dcmtk/dcmimgle/dimoimg.h"
#include "dcmtk/dcmimgle/dimo2img.h"
#include "dcmtk/dcmimgle/dimoipxt.h"
#include "dcmtk/dcmimgle/dimocpt.h"
#include "dcmtk/dcmimgle/dimosct.h"
#include "dcmtk/dcmimgle/dimoflt.h"
#include "dcmtk/dcmimgle/dimorot.h"
#include "dcmtk/dcmimgle/dimoopxt.h"
#include "dcmtk/dcmimgle/digsdfn.h"
#include "dcmtk/dcmimgle/didocu.h"
#include "dcmtk/dcmimgle/diutils.h"
#include "dcmtk/dcmimgle/diregbas.h"

#define INCLUDE_CMATH
#define INCLUDE_CINTTYPES
#include "dcmtk/ofstd/ofstdinc.h"


/*---------------------*
 *  const definitions  *
 *---------------------*/

const unsigned int Default_MinDensity   = 20;
const unsigned int Default_MaxDensity   = 300;
const unsigned int Default_Reflection   = 10;
const unsigned int Default_Illumination = 2000;


/*----------------*
 *  constructors  *
 *----------------*/

/*
 *   standard constructor to create mono-image
 */

DiMonoImage::DiMonoImage(const DiDocument *docu,
                         const EI_Status status)
  : DiImage(docu, status, 1),
    WindowCenter(0),
    WindowWidth(0),
    WindowCount(0),
    VoiLutCount(0),
    ValidWindow(0),
    VoiExplanation(),
    VoiLutFunction(EFV_Default),
    PresLutShape(ESP_Default),
    MinDensity(Default_MinDensity),
    MaxDensity(Default_MaxDensity),
    Reflection(Default_Reflection),
    Illumination(Default_Illumination),
    VoiLutData(NULL),
    PresLutData(NULL),
    InterData(NULL),
    DisplayFunction(NULL),
    OutputData(NULL),
    OverlayData(NULL)
{
    Overlays[0] = NULL;
    Overlays[1] = NULL;
    if ((Document != NULL) && (InputData != NULL) && (ImageStatus == EIS_Normal))
    {
        if (Document->getFlags() & CIF_UsePresentationState)
            PresLutShape = ESP_Identity;
        DiMonoModality *modality = new DiMonoModality(Document, InputData);
        Init(modality);
    }
}


DiMonoImage::DiMonoImage(const DiDocument *docu,
                         const EI_Status status,
                         const double slope,
                         const double intercept)
  : DiImage(docu, status, 1),
    WindowCenter(0),
    WindowWidth(0),
    WindowCount(0),
    VoiLutCount(0),
    ValidWindow(0),
    VoiExplanation(),
    VoiLutFunction(EFV_Default),
    PresLutShape(ESP_Default),
    MinDensity(Default_MinDensity),
    MaxDensity(Default_MaxDensity),
    Reflection(Default_Reflection),
    Illumination(Default_Illumination),
    VoiLutData(NULL),
    PresLutData(NULL),
    InterData(NULL),
    DisplayFunction(NULL),
    OutputData(NULL),
    OverlayData(NULL)
{
    Overlays[0] = NULL;
    Overlays[1] = NULL;
    if ((Document != NULL) && (InputData != NULL) && (ImageStatus == EIS_Normal))
    {
        if (Document->getFlags() & CIF_UsePresentationState)
            PresLutShape = ESP_Identity;
        DiMonoModality *modality = new DiMonoModality(Document, InputData, slope, intercept);
        Init(modality);
    }
}


DiMonoImage::DiMonoImage(const DiDocument *docu,
                         const EI_Status status,
                         const DcmUnsignedShort &data,
                         const DcmUnsignedShort &descriptor,
                         const DcmLongString *explanation)
  : DiImage(docu, status, 1),
    WindowCenter(0),
    WindowWidth(0),
    WindowCount(0),
    VoiLutCount(0),
    ValidWindow(0),
    VoiExplanation(),
    VoiLutFunction(EFV_Default),
    PresLutShape(ESP_Default),
    MinDensity(Default_MinDensity),
    MaxDensity(Default_MaxDensity),
    Reflection(Default_Reflection),
    Illumination(Default_Illumination),
    VoiLutData(NULL),
    PresLutData(NULL),
    InterData(NULL),
    DisplayFunction(NULL),
    OutputData(NULL),
    OverlayData(NULL)
{
    Overlays[0] = NULL;
    Overlays[1] = NULL;
    if ((Document != NULL) && (InputData != NULL) && (ImageStatus == EIS_Normal))
    {
        if (Document->getFlags() & CIF_UsePresentationState)
            PresLutShape = ESP_Identity;
        DiMonoModality *modality = new DiMonoModality(Document, InputData, data, descriptor, explanation);
        Init(modality);
    }
}


/*
 *   kind of copy-constructor, 'dummy' is necessary to differ from the above "standard"-constructor
 */

DiMonoImage::DiMonoImage(const DiDocument *docu,
                         const EI_Status status,
                         const char /*dummy*/)
  : DiImage(docu, status/*, dummy*/),
    WindowCenter(0),
    WindowWidth(0),
    WindowCount(0),
    VoiLutCount(0),
    ValidWindow(0),
    VoiExplanation(),
    VoiLutFunction(EFV_Default),
    PresLutShape(ESP_Default),
    MinDensity(Default_MinDensity),
    MaxDensity(Default_MaxDensity),
    Reflection(Default_Reflection),
    Illumination(Default_Illumination),
    VoiLutData(NULL),
    PresLutData(NULL),
    InterData(NULL),
    DisplayFunction(NULL),
    OutputData(NULL),
    OverlayData(NULL)
{
    Overlays[0] = NULL;
    Overlays[1] = NULL;
}


/*
 *   create image copy of specified frame range
 */

DiMonoImage::DiMonoImage(const DiMonoImage *image,
                         const unsigned long fstart,
                         const unsigned long fcount)
  : DiImage(image, fstart, fcount),
    WindowCenter(image->WindowCenter),
    WindowWidth(image->WindowWidth),
    WindowCount(image->WindowCount),
    VoiLutCount(image->VoiLutCount),
    ValidWindow(image->ValidWindow),
    VoiExplanation(image->VoiExplanation),
    VoiLutFunction(image->VoiLutFunction),
    PresLutShape(image->PresLutShape),
    MinDensity(image->MinDensity),
    MaxDensity(image->MaxDensity),
    Reflection(image->Reflection),
    Illumination(image->Illumination),
    VoiLutData(image->VoiLutData),
    PresLutData(image->PresLutData),
    InterData(NULL),
    DisplayFunction(image->DisplayFunction),
    OutputData(NULL),
    OverlayData(NULL)
{
    Overlays[0] = image->Overlays[0];
    Overlays[1] = image->Overlays[1];
    if (image->InterData != NULL)
    {
        const unsigned long fsize = OFstatic_cast(unsigned long, Columns) * OFstatic_cast(unsigned long, Rows);
        switch (image->InterData->getRepresentation())
        {
            case EPR_Uint8:
                InterData = new DiMonoCopyTemplate<Uint8>(image->InterData, fstart, fcount, fsize);
                break;
            case EPR_Sint8:
                InterData = new DiMonoCopyTemplate<Sint8>(image->InterData, fstart, fcount, fsize);
                break;
            case EPR_Uint16:
                InterData = new DiMonoCopyTemplate<Uint16>(image->InterData, fstart, fcount, fsize);
                break;
            case EPR_Sint16:
                InterData = new DiMonoCopyTemplate<Sint16>(image->InterData, fstart, fcount, fsize);
                break;
            case EPR_Uint32:
                InterData = new DiMonoCopyTemplate<Uint32>(image->InterData, fstart, fcount, fsize);
                break;
            case EPR_Sint32:
                InterData = new DiMonoCopyTemplate<Sint32>(image->InterData, fstart, fcount, fsize);
                break;
        }
    }
    checkInterData();
    for (int i = 0; i < 2; ++i)
    {
        if (Overlays[i] != NULL)
            Overlays[i]->addReference();
    }
    if (VoiLutData != NULL)
        VoiLutData->addReference();
    if (PresLutData != NULL)
        PresLutData->addReference();
}


/*
 *   convert color-image to mono-image with given 'red', 'green' and 'blue' coefficients
 */

DiMonoImage::DiMonoImage(const DiColorImage *image,
                         const double red,
                         const double green,
                         const double blue)
  : DiImage(OFreinterpret_cast(const DiImage *, image)),
    WindowCenter(0),
    WindowWidth(0),
    WindowCount(0),
    VoiLutCount(0),
    ValidWindow(0),
    VoiExplanation(),
    VoiLutFunction(EFV_Default),
    PresLutShape(ESP_Default),
    MinDensity(Default_MinDensity),
    MaxDensity(Default_MaxDensity),
    Reflection(Default_Reflection),
    Illumination(Default_Illumination),
    VoiLutData(NULL),
    PresLutData(NULL),
    InterData(NULL),
    DisplayFunction(NULL),
    OutputData(NULL),
    OverlayData(NULL)
{
    Overlays[0] = NULL;
    Overlays[1] = NULL;
    if ((Document != NULL) && (Document->getFlags() & CIF_UsePresentationState))
        PresLutShape = ESP_Identity;
    if (DiRegisterBase::Pointer != NULL)
        InterData = DiRegisterBase::Pointer->createMonoImageData(image, red, green, blue);
    if ((InterData == NULL) || (InterData->getData() == NULL))
        ImageStatus = EIS_InvalidImage;
}


/*
 *   scale 'image' to size given by 'columns' and 'rows', 'interpolate' always if parameter is true
 */

DiMonoImage::DiMonoImage(const DiMonoImage *image,
                         const signed long left_pos,
                         const signed long top_pos,
                         const Uint16 src_cols,
                         const Uint16 src_rows,
                         const Uint16 dest_cols,
                         const Uint16 dest_rows,
                         const int interpolate,
                         const int aspect,
                         const Uint16 pvalue)
  : DiImage(image, dest_cols, dest_rows, aspect),
    WindowCenter(image->WindowCenter),
    WindowWidth(image->WindowWidth),
    WindowCount(image->WindowCount),
    VoiLutCount(image->VoiLutCount),
    ValidWindow(image->ValidWindow),
    VoiExplanation(image->VoiExplanation),
    VoiLutFunction(image->VoiLutFunction),
    PresLutShape(image->PresLutShape),
    MinDensity(image->MinDensity),
    MaxDensity(image->MaxDensity),
    Reflection(image->Reflection),
    Illumination(image->Illumination),
    VoiLutData(image->VoiLutData),
    PresLutData(image->PresLutData),
    InterData(NULL),
    DisplayFunction(image->DisplayFunction),
    OutputData(NULL),
    OverlayData(NULL)
{
    Overlays[0] = NULL;
    Overlays[1] = NULL;
    if (image->InterData != NULL)
    {
        const unsigned int bits = image->InterData->getBits();
        switch (image->InterData->getRepresentation())
        {
            case EPR_Uint8:
                InterData = new DiMonoScaleTemplate<Uint8>(image->InterData, image->Columns, image->Rows,
                    left_pos, top_pos, src_cols, src_rows, dest_cols, dest_rows, NumberOfFrames,
                    bits, interpolate, pvalue);
                break;
            case EPR_Sint8:
                InterData = new DiMonoScaleTemplate<Sint8>(image->InterData, image->Columns, image->Rows,
                    left_pos, top_pos, src_cols, src_rows, dest_cols, dest_rows, NumberOfFrames,
                    bits, interpolate, pvalue);
                break;
            case EPR_Uint16:
                InterData = new DiMonoScaleTemplate<Uint16>(image->InterData, image->Columns, image->Rows,
                    left_pos, top_pos, src_cols, src_rows, dest_cols, dest_rows, NumberOfFrames,
                    bits, interpolate, pvalue);
                break;
            case EPR_Sint16:
                InterData = new DiMonoScaleTemplate<Sint16>(image->InterData, image->Columns, image->Rows,
                    left_pos, top_pos, src_cols, src_rows, dest_cols, dest_rows, NumberOfFrames,
                    bits, interpolate, pvalue);
                break;
            case EPR_Uint32:
                InterData = new DiMonoScaleTemplate<Uint32>(image->InterData, image->Columns, image->Rows,
                    left_pos, top_pos, src_cols, src_rows, dest_cols, dest_rows, NumberOfFrames,
                    bits, interpolate, pvalue);
                break;
            case EPR_Sint32:
                InterData = new DiMonoScaleTemplate<Sint32>(image->InterData, image->Columns, image->Rows,
                    left_pos, top_pos, src_cols, src_rows, dest_cols, dest_rows, NumberOfFrames,
                    bits, interpolate, pvalue);
                break;
        }
    }
    if (checkInterData(0))
    {
        for (int i = 0; i < 2; ++i)
        {
            if ((image->Overlays[i] != NULL) && (image->Overlays[i]->getCount() > 0))
            {
                Overlays[i] = new DiOverlay(image->Overlays[i], left_pos, top_pos,
                    OFstatic_cast(double, dest_cols) / OFstatic_cast(double, src_cols),
                    OFstatic_cast(double, dest_rows) / OFstatic_cast(double, src_rows));
            }
        }
    }
    if (VoiLutData != NULL)
        VoiLutData->addReference();
    if (PresLutData != NULL)
        PresLutData->addReference();
}


/*
 *   flip
 */

DiMonoImage::DiMonoImage(const DiMonoImage *image,
                         const int horz,
                         const int vert)
  : DiImage(image),
    WindowCenter(image->WindowCenter),
    WindowWidth(image->WindowWidth),
    WindowCount(image->WindowCount),
    VoiLutCount(image->VoiLutCount),
    ValidWindow(image->ValidWindow),
    VoiExplanation(image->VoiExplanation),
    VoiLutFunction(image->VoiLutFunction),
    PresLutShape(image->PresLutShape),
    MinDensity(image->MinDensity),
    MaxDensity(image->MaxDensity),
    Reflection(image->Reflection),
    Illumination(image->Illumination),
    VoiLutData(image->VoiLutData),
    PresLutData(image->PresLutData),
    InterData(NULL),
    DisplayFunction(image->DisplayFunction),
    OutputData(NULL),
    OverlayData(NULL)
{
    Overlays[0] = NULL;
    Overlays[1] = NULL;
    if (image->InterData != NULL)
    {
        switch (image->InterData->getRepresentation())
        {
            case EPR_Uint8:
                InterData = new DiMonoFlipTemplate<Uint8>(image->InterData, Columns, Rows, NumberOfFrames, horz, vert);
                break;
            case EPR_Sint8:
                InterData = new DiMonoFlipTemplate<Sint8>(image->InterData, Columns, Rows, NumberOfFrames, horz, vert);
                break;
            case EPR_Uint16:
                InterData = new DiMonoFlipTemplate<Uint16>(image->InterData, Columns, Rows, NumberOfFrames, horz, vert);
                break;
            case EPR_Sint16:
                InterData = new DiMonoFlipTemplate<Sint16>(image->InterData, Columns, Rows, NumberOfFrames, horz, vert);
                break;
            case EPR_Uint32:
                InterData = new DiMonoFlipTemplate<Uint32>(image->InterData, Columns, Rows, NumberOfFrames, horz, vert);
                break;
            case EPR_Sint32:
                InterData = new DiMonoFlipTemplate<Sint32>(image->InterData, Columns, Rows, NumberOfFrames, horz, vert);
                break;
        }
    }
    if (checkInterData(0))
    {
        for (int i = 0; i < 2; ++i)
        {
            if ((image->Overlays[i] != NULL) && (image->Overlays[i]->getCount() > 0))
                Overlays[i] = new DiOverlay(image->Overlays[i], horz, vert, Columns, Rows);
        }
    }
    if (VoiLutData != NULL)
        VoiLutData->addReference();
    if (PresLutData != NULL)
        PresLutData->addReference();
}


/*
 *   rotate
 */

DiMonoImage::DiMonoImage(const DiMonoImage *image,
                         const int degree)
  : DiImage(image, degree),
    WindowCenter(image->WindowCenter),
    WindowWidth(image->WindowWidth),
    WindowCount(image->WindowCount),
    VoiLutCount(image->VoiLutCount),
    ValidWindow(image->ValidWindow),
    VoiExplanation(image->VoiExplanation),
    VoiLutFunction(image->VoiLutFunction),
    PresLutShape(image->PresLutShape),
    MinDensity(image->MinDensity),
    MaxDensity(image->MaxDensity),
    Reflection(image->Reflection),
    Illumination(image->Illumination),
    VoiLutData(image->VoiLutData),
    PresLutData(image->PresLutData),
    InterData(NULL),
    DisplayFunction(image->DisplayFunction),
    OutputData(NULL),
    OverlayData(NULL)
{
    Overlays[0] = NULL;
    Overlays[1] = NULL;
    if (image->InterData != NULL)
    {
        switch (image->InterData->getRepresentation())
        {
            case EPR_Uint8:
                InterData = new DiMonoRotateTemplate<Uint8>(image->InterData, image->Columns, image->Rows, Columns, Rows,
                    NumberOfFrames, degree);
                break;
            case EPR_Sint8:
                InterData = new DiMonoRotateTemplate<Sint8>(image->InterData, image->Columns, image->Rows, Columns, Rows,
                    NumberOfFrames, degree);
                break;
            case EPR_Uint16:
                InterData = new DiMonoRotateTemplate<Uint16>(image->InterData, image->Columns, image->Rows, Columns, Rows,
                    NumberOfFrames, degree);
                break;
            case EPR_Sint16:
                InterData = new DiMonoRotateTemplate<Sint16>(image->InterData, image->Columns, image->Rows, Columns, Rows,
                    NumberOfFrames, degree);
                break;
            case EPR_Uint32:
                InterData = new DiMonoRotateTemplate<Uint32>(image->InterData, image->Columns, image->Rows, Columns, Rows,
                    NumberOfFrames, degree);
                break;
            case EPR_Sint32:
                InterData = new DiMonoRotateTemplate<Sint32>(image->InterData, image->Columns, image->Rows, Columns, Rows,
                    NumberOfFrames, degree);
                break;
        }
    }
    if (checkInterData(0))
    {
        for (int i = 0; i < 2; ++i)
        {
            if ((image->Overlays[i] != NULL) && (image->Overlays[i]->getCount() > 0))
                Overlays[i] = new DiOverlay(image->Overlays[i], degree, Columns, Rows);
        }
    }
    if (VoiLutData != NULL)
        VoiLutData->addReference();
    if (PresLutData != NULL)
        PresLutData->addReference();
}


/*
 *   this implementation is necessary to avoid linker errors on NeXTSTEP (gcc 2.5.8)
 */

DiMonoImage::DiMonoImage(const DiMonoImage &)
  : DiImage(NULL),
    WindowCenter(0),
    WindowWidth(0),
    WindowCount(0),
    VoiLutCount(0),
    ValidWindow(0),
    VoiExplanation(),
    VoiLutFunction(EFV_Default),
    PresLutShape(ESP_Default),
    MinDensity(Default_MinDensity),
    MaxDensity(Default_MaxDensity),
    Reflection(Default_Reflection),
    Illumination(Default_Illumination),
    VoiLutData(NULL),
    PresLutData(NULL),
    InterData(NULL),
    DisplayFunction(NULL),
    OutputData(NULL),
    OverlayData(NULL)
{
    DCMIMGLE_FATAL("using unimplemented copy constructor in class DiMonoImage ... aborting");
    abort();
}


/*
 *   createMonoOutputImage
 */

DiMonoImage::DiMonoImage(const DiMonoImage *image,
                         DiMonoOutputPixel *pixel,
                         const unsigned long frame,
                         const int stored,
                         const int alloc)
  : DiImage(image, frame, stored, alloc),
    WindowCenter(0),
    WindowWidth(0),
    WindowCount(0),
    VoiLutCount(0),
    ValidWindow(0),
    VoiExplanation(),
    VoiLutFunction(EFV_Default),
    PresLutShape(ESP_Default),
    MinDensity(Default_MinDensity),
    MaxDensity(Default_MaxDensity),
    Reflection(Default_Reflection),
    Illumination(Default_Illumination),
    VoiLutData(NULL),
    PresLutData(NULL),
    InterData(NULL),
    DisplayFunction(NULL),
    OutputData(NULL),
    OverlayData(NULL)
{
    Overlays[0] = NULL;
    Overlays[1] = NULL;
    if ((Document != NULL) && (Document->getFlags() & CIF_UsePresentationState))
        PresLutShape = ESP_Identity;
    if (pixel->getData() != NULL)
    {
        DiMonoModality *modality = new DiMonoModality(stored);
        switch (pixel->getRepresentation())
        {
            case EPR_Uint8:
                InterData = new DiMonoPixelTemplate<Uint8>(pixel, modality);
                break;
            case EPR_Uint16:
                InterData = new DiMonoPixelTemplate<Uint16>(pixel, modality);
                break;
            case EPR_Uint32:
                InterData = new DiMonoPixelTemplate<Uint32>(pixel, modality);
                break;
            default:
                break;
        }
    }
    checkInterData(0);
}


/*--------------*
 *  destructor  *
 *--------------*/

DiMonoImage::~DiMonoImage()
{
    delete InterData;
    delete OutputData;
    delete OFstatic_cast(char *, OverlayData);    // type cast necessary to avoid compiler warnings using gcc 2.95
    if (VoiLutData != NULL)
        VoiLutData->removeReference();            // only delete if object is no longer referenced
    if (PresLutData != NULL)
        PresLutData->removeReference();
    for (int i = 0; i < 2; ++i)
    {
        if (Overlays[i] != NULL)
            Overlays[i]->removeReference();
    }
}


/*********************************************************************/


int DiMonoImage::processNextFrames(const unsigned long fcount)
{
    if (DiImage::processNextFrames(fcount))
    {
        if (InterData != NULL)
        {
            /* do not create a new object but reference the existing one */
            DiMonoModality *modality = InterData->addReferenceToModality();
            delete InterData;
            InterData = NULL;
            Init(modality, OFTrue /* reuse */);
            return (ImageStatus == EIS_Normal);
        }
    }
    return 0;
}

/*********************************************************************/


void DiMonoImage::Init(DiMonoModality *modality,
                       const OFBool reuse)
{
    if (modality != NULL)
    {
        /* only create new overlay object if none exists */
        if (Overlays[0] == NULL)
        {
            Overlays[0] = new DiOverlay(Document, BitsAllocated, BitsStored, HighBit);
            if ((Overlays[0] != NULL) && !(Document->getFlags() & CIF_UsePresentationState))
                Overlays[0]->showAllPlanes();               // default: show all overlays with stored modes
            if ((Overlays[0] == NULL) || (Overlays[0]->getCount() == 0) || (!Overlays[0]->hasEmbeddedData()))
                detachPixelData();                          // no longer needed, save memory
        }
        switch (InputData->getRepresentation())
        {
            case EPR_Uint8:
                InitUint8(modality);
                break;
            case EPR_Sint8:
                InitSint8(modality);
                break;
            case EPR_Uint16:
                InitUint16(modality);
                break;
            case EPR_Sint16:
                InitSint16(modality);
                break;
            case EPR_Uint32:
                InitUint32(modality);
                break;
            case EPR_Sint32:
                InitSint32(modality);
                break;
        }
        deleteInputData();                                  // no longer needed, save memory
        if (modality->getBits() > 0)
            BitsPerSample = modality->getBits();            // get bit depth of internal representation
        if (checkInterData())
        {
            /* get grayscale related attributes (if desired) */
            if (!reuse && !(Document->getFlags() & CIF_UsePresentationState))
            {
                /* VOI windows */
                WindowCount = Document->getVM(DCM_WindowCenter);
                const unsigned long count = Document->getVM(DCM_WindowWidth);
                if (count < WindowCount)                    // determine number of VOI windows
                    WindowCount = count;
                /* VOI LUT */
                DcmSequenceOfItems *seq = NULL;
                VoiLutCount = Document->getSequence(DCM_VOILUTSequence, seq);
                /* VOI LUT Function */
                OFString str;
                if (Document->getValue(DCM_VOILUTFunction, str))
                {
                    if (str == "LINEAR")
                        VoiLutFunction = EFV_Linear;
                    else if (str == "SIGMOID")
                        VoiLutFunction = EFV_Sigmoid;
                    else
                        DCMIMGLE_WARN("unknown value for 'VOILUTFunction' (" << str << ") ... ignoring");
                }
                /* Presentation LUT Shape */
                if (Document->getValue(DCM_PresentationLUTShape, str))
                {
                    if (str == "IDENTITY")
                        PresLutShape = ESP_Identity;
                    else if (str == "INVERSE")
                        PresLutShape = ESP_Inverse;
                    else
                        DCMIMGLE_WARN("unknown value for 'PresentationLUTShape' (" << str << ") ... ignoring");
                }
            }
        }
    } else
        detachPixelData();
}


void DiMonoImage::InitUint8(DiMonoModality *modality)
{
    if (modality != NULL)
    {
        switch (modality->getRepresentation())
        {
            case EPR_Uint8:
                InterData = new DiMonoInputPixelTemplate<Uint8, Uint32, Uint8>(InputData, modality);
                break;
            case EPR_Sint8:
                InterData = new DiMonoInputPixelTemplate<Uint8, Uint32, Sint8>(InputData, modality);
                break;
            case EPR_Uint16:
                InterData = new DiMonoInputPixelTemplate<Uint8, Uint32, Uint16>(InputData, modality);
                break;
            case EPR_Sint16:
                InterData = new DiMonoInputPixelTemplate<Uint8, Uint32, Sint16>(InputData, modality);
                break;
            case EPR_Uint32:
                InterData = new DiMonoInputPixelTemplate<Uint8, Uint32, Uint32>(InputData, modality);
                break;
            case EPR_Sint32:
                InterData = new DiMonoInputPixelTemplate<Uint8, Uint32, Sint32>(InputData, modality);
                break;
        }
    }
}


void DiMonoImage::InitSint8(DiMonoModality *modality)
{
    if (modality != NULL)
    {
        switch (modality->getRepresentation())
        {
            case EPR_Uint8:
                InterData = new DiMonoInputPixelTemplate<Sint8, Sint32, Uint8>(InputData, modality);
                break;
            case EPR_Sint8:
                InterData = new DiMonoInputPixelTemplate<Sint8, Sint32, Sint8>(InputData, modality);
                break;
            case EPR_Uint16:
                InterData = new DiMonoInputPixelTemplate<Sint8, Sint32, Uint16>(InputData, modality);
                break;
            case EPR_Sint16:
                InterData = new DiMonoInputPixelTemplate<Sint8, Sint32, Sint16>(InputData, modality);
                break;
            case EPR_Uint32:
                InterData = new DiMonoInputPixelTemplate<Sint8, Sint32, Uint32>(InputData, modality);
                break;
            case EPR_Sint32:
                InterData = new DiMonoInputPixelTemplate<Sint8, Sint32, Sint32>(InputData, modality);
                break;
        }
    }
}

void DiMonoImage::InitUint16(DiMonoModality *modality)
{
    if (modality != NULL)
    {
        switch (modality->getRepresentation())
        {
            case EPR_Uint8:
                InterData = new DiMonoInputPixelTemplate<Uint16, Uint32, Uint8>(InputData, modality);
                break;
            case EPR_Sint8:
                InterData = new DiMonoInputPixelTemplate<Uint16, Uint32, Sint8>(InputData, modality);
                break;
            case EPR_Uint16:
                InterData = new DiMonoInputPixelTemplate<Uint16, Uint32, Uint16>(InputData, modality);
                break;
            case EPR_Sint16:
                InterData = new DiMonoInputPixelTemplate<Uint16, Uint32, Sint16>(InputData, modality);
                break;
            case EPR_Uint32:
                InterData = new DiMonoInputPixelTemplate<Uint16, Uint32, Uint32>(InputData, modality);
                break;
            case EPR_Sint32:
                InterData = new DiMonoInputPixelTemplate<Uint16, Uint32, Sint32>(InputData, modality);
                break;
        }
    }
}


void DiMonoImage::InitSint16(DiMonoModality *modality)
{
    if (modality != NULL)
    {
        switch (modality->getRepresentation())
        {
            case EPR_Uint8:
                InterData = new DiMonoInputPixelTemplate<Sint16, Sint32, Uint8>(InputData, modality);
                break;
            case EPR_Sint8:
                InterData = new DiMonoInputPixelTemplate<Sint16, Sint32, Sint8>(InputData, modality);
                break;
            case EPR_Uint16:
                InterData = new DiMonoInputPixelTemplate<Sint16, Sint32, Uint16>(InputData, modality);
                break;
            case EPR_Sint16:
                InterData = new DiMonoInputPixelTemplate<Sint16, Sint32, Sint16>(InputData, modality);
                break;
            case EPR_Uint32:
                InterData = new DiMonoInputPixelTemplate<Sint16, Sint32, Uint32>(InputData, modality);
                break;
            case EPR_Sint32:
                InterData = new DiMonoInputPixelTemplate<Sint16, Sint32, Sint32>(InputData, modality);
                break;
        }
    }
}


void DiMonoImage::InitUint32(DiMonoModality *modality)
{
    if (modality != NULL)
    {
        switch (modality->getRepresentation())
        {
            case EPR_Uint8:
                InterData = new DiMonoInputPixelTemplate<Uint32, Uint32, Uint8>(InputData, modality);
                break;
            case EPR_Sint8:
                InterData = new DiMonoInputPixelTemplate<Uint32, Uint32, Sint8>(InputData, modality);
                break;
            case EPR_Uint16:
                InterData = new DiMonoInputPixelTemplate<Uint32, Uint32, Uint16>(InputData, modality);
                break;
            case EPR_Sint16:
                InterData = new DiMonoInputPixelTemplate<Uint32, Uint32, Sint16>(InputData, modality);
                break;
            case EPR_Uint32:
                InterData = new DiMonoInputPixelTemplate<Uint32, Uint32, Uint32>(InputData, modality);
                break;
            case EPR_Sint32:
                InterData = new DiMonoInputPixelTemplate<Uint32, Uint32, Sint32>(InputData, modality);
                break;
        }
    }
}


void DiMonoImage::InitSint32(DiMonoModality *modality)
{
    if (modality != NULL)
    {
        switch (modality->getRepresentation())
        {
            case EPR_Uint8:
                InterData = new DiMonoInputPixelTemplate<Sint32, Sint32, Uint8>(InputData, modality);
                break;
            case EPR_Sint8:
                InterData = new DiMonoInputPixelTemplate<Sint32, Sint32, Sint8>(InputData, modality);
                break;
            case EPR_Uint16:
                InterData = new DiMonoInputPixelTemplate<Sint32, Sint32, Uint16>(InputData, modality);
                break;
            case EPR_Sint16:
                InterData = new DiMonoInputPixelTemplate<Sint32, Sint32, Sint16>(InputData, modality);
                break;
            case EPR_Uint32:
                InterData = new DiMonoInputPixelTemplate<Sint32, Sint32, Uint32>(InputData, modality);
                break;
            case EPR_Sint32:
                InterData = new DiMonoInputPixelTemplate<Sint32, Sint32, Sint32>(InputData, modality);
                break;
        }
    }
}

/*********************************************************************/


int DiMonoImage::checkInterData(const int mode)
{
    if (InterData == NULL)
    {
        if (ImageStatus == EIS_Normal)
        {
            ImageStatus = EIS_MemoryFailure;
            DCMIMGLE_ERROR("can't allocate memory for inter-representation");
        } else
            ImageStatus = EIS_InvalidImage;
    }
    else if (InterData->getData() == NULL)
        ImageStatus = EIS_InvalidImage;
    else if (mode && (ImageStatus == EIS_Normal) && isOriginal /* do not check derived images */)
    {
        const unsigned long count = OFstatic_cast(unsigned long, Columns) * OFstatic_cast(unsigned long, Rows) * NumberOfFrames;
        if ((InterData->getInputCount() != count) && ((InterData->getInputCount() >> 1) != ((count + 1) >> 1)))
        {
            DCMIMGLE_WARN("computed (" << count << ") and stored (" << InterData->getInputCount() << ") pixel count differ");
        }
    }
    return (ImageStatus == EIS_Normal);
}


void DiMonoImage::deleteOutputData()
{
    delete OutputData;
    OutputData = NULL;
}


unsigned long DiMonoImage::getOutputDataSize(const int bits) const
{
    unsigned long result = 0;
    if ((ImageStatus == EIS_Normal) && (((bits > 0) && (bits <= MAX_BITS)) || (bits == MI_PastelColor)))
    {
        int samples = 1;
        int bytesPerPixel = 1;
        if (bits == MI_PastelColor)                         // use true color pastel mode
            samples = 3;
        else if (bits > 16)
            bytesPerPixel = 4;
        else if (bits > 8)
            bytesPerPixel = 2;
        /* compute number of bytes required to store a rendered frame */
        result = OFstatic_cast(unsigned long, Columns) * OFstatic_cast(unsigned long, Rows) * samples * bytesPerPixel;
    }
    return result;
}


const void *DiMonoImage::getOutputPlane(const int) const
{
    if (OutputData != NULL)
        return OutputData->getData();  // monochrome images don't have multiple planes
    return NULL;
}


void DiMonoImage::deleteOverlayData()
{
    delete OFstatic_cast(char *, OverlayData);    // type cast necessary to avoid compiler warnings using gcc 2.95
    OverlayData = NULL;
}


/*********************************************************************/


int DiMonoImage::getMinMaxValues(double &min,
                                 double &max,
                                 const int mode) const
{
    if (InterData != NULL)
    {
        if (mode)
        {
            min = InterData->getAbsMinimum();
            max = InterData->getAbsMaximum();
            return 1;
        }
        return InterData->getMinMaxValues(min, max);
    }
    return 0;
}


int DiMonoImage::setDisplayFunction(DiDisplayFunction *display)
{
    DisplayFunction = display;
    return (DisplayFunction != NULL) && (DisplayFunction->isValid());
}


int DiMonoImage::setNoDisplayFunction()
{
    if (DisplayFunction != NULL)
    {
        DisplayFunction = NULL;
        return 1;
    }
    return 2;
}


int DiMonoImage::convertPValueToDDL(const Uint16 pvalue,
                                    Uint16 &ddl,
                                    const int bits)
{
    const unsigned long maxvalue = DicomImageClass::maxval(bits);
    if ((DisplayFunction != NULL) && (DisplayFunction->isValid()) && (DisplayFunction->getMaxDDLValue() == maxvalue))
    {
        const DiDisplayLUT *dlut = DisplayFunction->getLookupTable(WIDTH_OF_PVALUES);
        if ((dlut != NULL) && (dlut->isValid()))
        {
            ddl = dlut->getValue(pvalue);                               // perform display transformation
            return 1;
        }
    }
    if ((bits >= 1) && (bits <= WIDTH_OF_PVALUES))                      // no display function: perform linear scaling
    {
        ddl = OFstatic_cast(Uint16, OFstatic_cast(double, maxvalue) * OFstatic_cast(double, pvalue) /
            OFstatic_cast(double, DicomImageClass::maxval(WIDTH_OF_PVALUES)));
        return 2;
    }
    return 0;
}


int DiMonoImage::setNoVoiTransformation()
{
    int old = 2;
    if (VoiLutData != NULL)
    {
        if (VoiLutData->isValid())
            old = 1;
        VoiLutData->removeReference();
    }
    VoiLutData = NULL;
    VoiExplanation = "";
    if (ValidWindow)
        old = 1;
    ValidWindow = 0;
    return old;
}


int DiMonoImage::setMinMaxWindow(const int idx)
{
    if (InterData != NULL)
    {
        double center;
        double width;
        if (InterData->getMinMaxWindow(idx != 0, center, width))
            return setWindow(center, width, "Min-Max Window");
    }
    return 0;
}


int DiMonoImage::setRoiWindow(const unsigned long left_pos,
                              const unsigned long top_pos,
                              const unsigned long width,
                              const unsigned long height,
                              const unsigned long frame)
{
    if ((InterData != NULL) && (frame < NumberOfFrames))
    {
        double voiCenter;
        double voiWidth;
        if (InterData->getRoiWindow(left_pos, top_pos, width, height, Columns, Rows, frame, voiCenter, voiWidth))
            return setWindow(voiCenter, voiWidth, "ROI Window");
    }
    return 0;
}


int DiMonoImage::setHistogramWindow(const double thresh)
{
    if (InterData != NULL)
    {
        double center;
        double width;
        if (InterData->getHistogramWindow(thresh, center, width))
            return setWindow(center, width, "Histogram Window");
    }
    return 0;
}


int DiMonoImage::setWindow(const unsigned long pos)
{
    if (!(Document->getFlags() & CIF_UsePresentationState))
    {
        double center;
        double width;
        WindowCount = Document->getValue(DCM_WindowCenter, center, pos);
        unsigned long count = Document->getValue(DCM_WindowWidth, width, pos);
        if (count < WindowCount)
            WindowCount = count;
        if (pos < WindowCount)
        {
            /* save return value to be used later (setWindow clears the explanation string!) */
            const int result = setWindow(center, width);
            /* get the stored explanation string */
            Document->getValue(DCM_WindowCenterWidthExplanation, VoiExplanation, pos);
            return result;
        }
    }
    return 0;
}


int DiMonoImage::setWindow(const double center,
                           const double width,
                           const char *explanation)
{
    if (VoiLutData != NULL)
        VoiLutData->removeReference();
    VoiLutData = NULL;
    if (explanation != NULL)
        VoiExplanation = explanation;
    else
        VoiExplanation = "";
    if (width < 1)                                              // not valid, according to supplement 33
        return ValidWindow = 0;
    else if (!ValidWindow || (center != WindowCenter) || (width != WindowWidth))
    {
        WindowCenter = center;
        WindowWidth = width;
        return ValidWindow = 1;
    }
    return 2;                                                   // window is unchanged (and valid)
}


int DiMonoImage::getWindow(double &center, double &width)
{
    if (ValidWindow)
    {
        center = WindowCenter;
        width = WindowWidth;
        return 1;
    }
    return 0;
}


EF_VoiLutFunction DiMonoImage::getVoiLutFunction() const
{
    return VoiLutFunction;
}


int DiMonoImage::setVoiLutFunction(const EF_VoiLutFunction function)
{
    if (function != VoiLutFunction)
    {
        VoiLutFunction = function;
        return 1;
    }
    return 2;
}


int DiMonoImage::setVoiLut(const DcmUnsignedShort &data,
                           const DcmUnsignedShort &descriptor,
                           const DcmLongString *explanation,
                           const EL_BitsPerTableEntry descripMode)
{
    if (VoiLutData != NULL)
        VoiLutData->removeReference();
    VoiLutData = new DiLookupTable(data, descriptor, explanation, descripMode);
    if (VoiLutData != NULL)
    {
        VoiExplanation = OFSTRING_GUARD(VoiLutData->getExplanation());
        return VoiLutData->isValid();
    }
    VoiExplanation = "";
    return 0;
}


int DiMonoImage::setVoiLut(const unsigned long pos,
                           const EL_BitsPerTableEntry descripMode)
{
    if (!(Document->getFlags() & CIF_UsePresentationState))
    {
        if (VoiLutData != NULL)
            VoiLutData->removeReference();
        VoiLutData = new DiLookupTable(Document, DCM_VOILUTSequence, DCM_LUTDescriptor, DCM_LUTData,
            DCM_LUTExplanation, descripMode, pos, &VoiLutCount);
        if (VoiLutData != NULL)
        {
            VoiExplanation = OFSTRING_GUARD(VoiLutData->getExplanation());
            return VoiLutData->isValid();
        }
        VoiExplanation = "";
    }
    return 0;
}


const char *DiMonoImage::getVoiWindowExplanation(const unsigned long pos,
                                                 OFString &explanation) const
{
    const char *result = NULL;
    /* get the stored explanation string */
    if (Document->getValue(DCM_WindowCenterWidthExplanation, explanation, pos) > 0)
        result = explanation.c_str();
    return result;
}


const char *DiMonoImage::getVoiLutExplanation(const unsigned long pos,
                                              OFString &explanation) const
{
    const char *result = NULL;
    /* get the given sequence item ... */
    DcmSequenceOfItems *seq = NULL;
    if ((pos < Document->getSequence(DCM_VOILUTSequence, seq)) && (seq != NULL))
    {
        /* ... and then the stored explanation string */
        if (Document->getValue(DCM_LUTExplanation, explanation, 0 /*vm pos*/, seq->getItem(pos)) > 0)
            result = explanation.c_str();
    }
    return result;
}


int DiMonoImage::setHardcopyParameters(const unsigned int min,
                                       const unsigned int max,
                                       const unsigned int reflect,
                                       const unsigned int illumin)
{
    int result = 0;
    if (min < max)
    {
        result = 2;
        if (min != MinDensity)
        {
            MinDensity = min;
            result = 1;
        }
        if (max != MaxDensity)
        {
            MaxDensity = max;
            result = 1;
        }
        if (reflect != Reflection)
        {
            Reflection = reflect;
            result = 1;
        }
        if (illumin != Illumination)
        {
            Illumination = illumin;
            result = 1;
        }
        if ((result == 1) && (PresLutShape == ESP_LinOD) && (PresLutData != NULL))
        {
            PresLutData->removeReference();       // look-up table no longer valid
            PresLutData = NULL;
        }
    }
    return result;
}


ES_PresentationLut DiMonoImage::getPresentationLutShape() const
{
    return (PresLutData != NULL) ? ESP_Default : PresLutShape;
}


int DiMonoImage::setPresentationLutShape(const ES_PresentationLut shape)
{
    if (PresLutData != NULL)
        PresLutData->removeReference();
    PresLutData = NULL;
    if (shape != PresLutShape)
    {
        PresLutShape = shape;
        return 1;
    }
    return 2;
}


int DiMonoImage::setPresentationLut(const DcmUnsignedShort &data,
                                    const DcmUnsignedShort &descriptor,
                                    const DcmLongString *explanation,
                                    const EL_BitsPerTableEntry descripMode)
{
    if (PresLutData != NULL)
        PresLutData->removeReference();
    PresLutData = new DiLookupTable(data, descriptor, explanation, descripMode, 0);
    if (PresLutData != NULL)
    {
        PresLutShape = ESP_Default;
        return PresLutData->isValid();
    }
    return 0;
}


int DiMonoImage::setInversePresentationLut(const DcmUnsignedShort &data,
                                           const DcmUnsignedShort &descriptor,
                                           const EL_BitsPerTableEntry descripMode)
{
    int status = 0;
    if (PresLutData != NULL)
        PresLutData->removeReference();
    PresLutData = NULL;
    DiLookupTable *lut = new DiLookupTable(data, descriptor, NULL, descripMode, 0);
    if ((lut != NULL) && (lut->isValid()))
    {
        PresLutData = lut->createInverseLUT();
        if (PresLutData != NULL)
            status = PresLutData->isValid();
    }
    delete lut;
    return status;
}


int DiMonoImage::addOverlay(const unsigned int group,
                            const signed int left_pos,
                            const signed int top_pos,
                            const unsigned int columns,
                            const unsigned int rows,
                            const DcmOverlayData &data,
                            const DcmLongString &label,
                            const DcmLongString &description,
                            const EM_Overlay mode)
{
    if (Overlays[1] == NULL)
        Overlays[1] = new DiOverlay();
    if (Overlays[1] != NULL)
        return Overlays[1]->addPlane(group, left_pos, top_pos, columns, rows, data, label, description, mode);
    return 0;
}


int DiMonoImage::removeAllOverlays()
{
    if (Overlays[1] != NULL)
    {
        delete Overlays[1];
        Overlays[1] = NULL;
        return 1;
    }
    return 2;
}


int DiMonoImage::flip(const int horz,
                      const int vert)
{
    switch (InterData->getRepresentation())
    {
        case EPR_Uint8:
            {
                DiFlipTemplate<Uint8> dummy(InterData, Columns, Rows, NumberOfFrames, horz, vert);
            }
            break;
        case EPR_Sint8:
            {
                DiFlipTemplate<Sint8> dummy(InterData, Columns, Rows, NumberOfFrames, horz, vert);
            }
            break;
        case EPR_Uint16:
            {
                DiFlipTemplate<Uint16> dummy(InterData, Columns, Rows, NumberOfFrames, horz, vert);
            }
            break;
        case EPR_Sint16:
            {
                DiFlipTemplate<Sint16> dummy(InterData, Columns, Rows, NumberOfFrames, horz, vert);
            }
            break;
        case EPR_Uint32:
            {
                DiFlipTemplate<Uint32> dummy(InterData, Columns, Rows, NumberOfFrames, horz, vert);
            }
            break;
        case EPR_Sint32:
            {
                DiFlipTemplate<Sint32> dummy(InterData, Columns, Rows, NumberOfFrames, horz, vert);
            }
            break;
    }
    for (int i = 0; i < 2; ++i)
    {
        if ((Overlays[i] != NULL) && (Overlays[i]->getCount() > 0))
        {
            DiOverlay *old = Overlays[i];
            Overlays[i] = new DiOverlay(old, horz, vert, Columns, Rows);
            old->removeReference();
        }
    }
    return 1;
}


int DiMonoImage::rotate(const int degree)
{
    const Uint16 old_cols = Columns;                // save old values
    const Uint16 old_rows = Rows;
    DiImage::rotate(degree);                        // swap width and height if necessary
    if ((Columns > 1) && (Rows > 1))                // re-interpret pixel data for cols = 1 or rows = 1
    {
        switch (InterData->getRepresentation())
        {
            case EPR_Uint8:
                {
                    DiRotateTemplate<Uint8> dummy(InterData, old_cols, old_rows, Columns, Rows, NumberOfFrames, degree);
                }
                break;
            case EPR_Sint8:
                {
                    DiRotateTemplate<Sint8> dummy(InterData, old_cols, old_rows, Columns, Rows, NumberOfFrames, degree);
                }
                break;
            case EPR_Uint16:
                {
                    DiRotateTemplate<Uint16> dummy(InterData, old_cols, old_rows, Columns, Rows, NumberOfFrames, degree);
                }
                break;
            case EPR_Sint16:
                {
                    DiRotateTemplate<Sint16> dummy(InterData, old_cols, old_rows, Columns, Rows, NumberOfFrames, degree);
                }
                break;
            case EPR_Uint32:
                {
                    DiRotateTemplate<Uint32> dummy(InterData, old_cols, old_rows, Columns, Rows, NumberOfFrames, degree);
                }
                break;
            case EPR_Sint32:
                {
                    DiRotateTemplate<Sint32> dummy(InterData, old_cols, old_rows, Columns, Rows, NumberOfFrames, degree);
                }
                break;
        }
    }
    for (int i = 0; i < 2; ++i)
    {
        if ((Overlays[i] != NULL) && (Overlays[i]->getCount() > 0))
        {
            DiOverlay *old = Overlays[i];
            Overlays[i] = new DiOverlay(old, degree, Columns, Rows);
            old->removeReference();
        }
    }
    return 1;
}


/*********************************************************************/


/*
 *   create output data of 'frame' with depth of 'bits' and min/max values depending on 'negative' (support mono1/2)
 */

const void *DiMonoImage::getData(void *buffer,
                                 const unsigned long size,
                                 const unsigned long frame,
                                 int bits,
                                 const int /*planar*/,            /* not yet supported, needed for pastel color images !! */
                                 const int negative)
{
    if ((InterData != NULL) && (ImageStatus == EIS_Normal) && (frame < NumberOfFrames) &&
        (((bits > 0) && (bits <= MAX_BITS)) || (bits == MI_PastelColor)))
    {
        if ((buffer == NULL) || (size >= getOutputDataSize(bits)))
        {
            deleteOutputData();                             // delete old image data
            if (!ValidWindow)
                WindowWidth = -1;                           // negative width means no window, saves additional parameter ;)
            Uint32 low;
            Uint32 high;
            if ((PresLutData == NULL) &&
               ((PresLutShape == ESP_Inverse) || (negative && (PresLutShape == ESP_Default))))
            {
                low = DicomImageClass::maxval(bits);        // inverse/negative: white to black
                high = 0;
            } else {
                low = 0;                                    // normal/positive: black to white
                high = DicomImageClass::maxval(bits);
            }
            if ((PresLutData == NULL) && (PresLutShape == ESP_LinOD))
            {
                if (!createLinODPresentationLut(4096, 16))  // create presentation LUT converting linOD data (on demand)
                {
                    DCMIMGLE_WARN("could not create presentation LUT for LinOD conversion ... ignoring presentation LUT shape LinOD");
                }
            }
            if (Polarity == EPP_Reverse)                    // swap high and low value
            {
                Uint32 temp = low;
                low = high;
                high = temp;
            }
            DiDisplayFunction *disp = DisplayFunction;
            if ((disp != NULL) && (disp->isValid()) && (disp->getMaxDDLValue() != DicomImageClass::maxval(bits)))
            {
                DCMIMGLE_WARN("selected display function doesn't fit to requested output depth (" << bits
                    << ") ... ignoring display transformation");
                disp = NULL;
            }
            const int samples = (bits == MI_PastelColor) ? 3 : 1;
            switch (InterData->getRepresentation())
            {
                case EPR_Uint8:
                    getDataUint8(buffer, disp, samples, frame, bits, low, high);
                    break;
                case EPR_Sint8:
                    getDataSint8(buffer, disp, samples, frame, bits, low, high);
                    break;
                case EPR_Uint16:
                    getDataUint16(buffer, disp, samples, frame, bits, low, high);
                    break;
                case EPR_Sint16:
                    getDataSint16(buffer, disp, samples, frame, bits, low, high);
                    break;
                case EPR_Uint32:
                    getDataUint32(buffer, disp, samples, frame, bits, low, high);
                    break;
                case EPR_Sint32:
                    getDataSint32(buffer, disp, samples, frame, bits, low, high);
                    break;
            }
            if (OutputData == NULL)
            {
                ImageStatus = EIS_MemoryFailure;
                DCMIMGLE_ERROR("can't allocate memory for output-representation");
            }
            else
                return OutputData->getData();           // points to beginning of output data
        } else {
            DCMIMGLE_ERROR("given output buffer is too small (only " << size << " bytes)");
        }
    }
    return NULL;
}


/*
 *   create 1/8/16-bit (bi-level) bitmap with overlay 'plane' data
 */

const void *DiMonoImage::getOverlayData(const unsigned long frame,
                                        const unsigned int plane,
                                        unsigned int &left_pos,
                                        unsigned int &top_pos,
                                        unsigned int &width,
                                        unsigned int &height,
                                        EM_Overlay &mode,
                                        const unsigned int idx,
                                        const int bits,
                                        const Uint16 fore,
                                        const Uint16 back)
{
    if ((ImageStatus == EIS_Normal) && (bits > 0) && (bits <= 16) && (fore != back))
    {
        int start = 1;                                              // default: additional overlay planes hide dataset planes
        int end = 0;
        if (idx < 2)                                                // specified index of overlay group
            start = end = idx;
        for (int i = start; i >= end; --i)                          // start searching with additional overlay planes
        {
            if ((Overlays[i] != NULL) && (Overlays[i]->hasPlane(plane)))
            {
                deleteOverlayData();
                OverlayData = Overlays[i]->getPlaneData(frame, plane, left_pos, top_pos, width, height,
                    mode, Columns, Rows, bits, fore, back);
                return OFstatic_cast(const void *, OverlayData);
            }
        }
    }
    return NULL;
}


/*
 *   create 1/8/16-bit (bi-level) bitmap with overlay 'plane' data
 */

const void *DiMonoImage::getFullOverlayData(const unsigned long frame,
                                            const unsigned int plane,
                                            unsigned int &width,
                                            unsigned int &height,
                                            const unsigned int idx,
                                            const int bits,
                                            const Uint16 fore,
                                            const Uint16 back)
{
    if ((ImageStatus == EIS_Normal) && (bits > 0) && (bits <= 16) && (fore != back))
    {
        if ((idx < 2) && (Overlays[idx] != NULL) && (Overlays[idx]->hasPlane(plane)))
        {
            deleteOverlayData();
            OverlayData = Overlays[idx]->getFullPlaneData(frame, plane, width, height, bits, fore, back);
            return OFstatic_cast(const void *, OverlayData);
        }
    }
    return NULL;
}


/*
 *   create 1-bit (bi-level) bitmap with overlay 'plane' data
 *   as required for the (6xxx,3000) OverlayData format
 */

unsigned long DiMonoImage::create6xxx3000OverlayData(Uint8 *&buffer,
                                                     const unsigned int plane,
                                                     unsigned int &width,
                                                     unsigned int &height,
                                                     unsigned long &frames,
                                                     const unsigned int idx)
{
    if (ImageStatus == EIS_Normal)
    {
        if ((idx < 2) && (Overlays[idx] != NULL) && (Overlays[idx]->hasPlane(plane)))
            return Overlays[idx]->create6xxx3000PlaneData(buffer, plane, width, height, frames);
    }
    return 0;
}


/*
 *   create 8-bit palette/monochrome or 24/32-bit true color device independent bitmap (DIB) as needed by MS-Windows
 */

unsigned long DiMonoImage::createDIB(void *&data,
                                     const unsigned long size,
                                     const unsigned long frame,
                                     const int bits,
                                     const int upsideDown,
                                     const int padding)
{
    unsigned long bytes = 0;
    if (size == 0)
        data = NULL;
    if ((bits == 8) || (bits == 24) || (bits == 32))
    {
        getOutputData(frame, 8);                            // create output data with 8 bit depth
        if ((OutputData != NULL) && (OutputData->getData() != NULL))
        {
            const signed long nextRow = (upsideDown) ? -2 * OFstatic_cast(signed long, Columns) : 0;
            const Uint8 *p = OFstatic_cast(const Uint8 *, OutputData->getData()) + ((upsideDown) ?
                OFstatic_cast(unsigned long, Rows - 1) * OFstatic_cast(unsigned long, Columns) : 0);
            if (bits == 8)                                  // -- for idx color model (byte)
            {
                // each line has to start at 32-bit-address, if 'padding' is true
                const int gap = (padding) ? (4 - (Columns & 0x3)) & 0x3 : 0;
                const unsigned long count = OFstatic_cast(unsigned long, Columns + gap) * OFstatic_cast(unsigned long, Rows);
                if ((gap > 0) || (nextRow != 0) || (data != NULL))
                {
                    if ((data == NULL) || (size >= count))
                    {
                        if (data == NULL)
                            data = new Uint8[count];            // allocated memory buffer
                        if (data != NULL)
                        {
                            Uint8 *q = OFstatic_cast(Uint8 *, data);
                            Uint16 x;
                            Uint16 y;
                            for (y = Rows; y != 0; --y)
                            {
                                for (x = Columns; x != 0; --x)
                                    *(q++) = *(p++);            // store gray value
                                p += nextRow;                   // jump (backwards) to next row
                                q += gap;                       // skip gap at the end of each line (32-bit boundary)
                            }
                            bytes = count;
                        }
                    }
                } else {                                    // data already aligned and correctly oriented
                    data = OutputData->getDataPtr();
                    OutputData->removeDataReference();      // remove reference to internal memory
                    bytes = count;
                }
            }
            else if (bits == 24)                            // -- for direct color model (24 bits/pixel)
            {
                const unsigned long col3 = OFstatic_cast(unsigned long, Columns) * 3;
                // each line has to start at 32-bit-address, if 'padding' is true
                const int gap = (padding) ? OFstatic_cast(int, (4 - (col3 & 0x3)) & 0x3) : 0;
                const unsigned long count = (col3 + gap) * OFstatic_cast(unsigned long, Rows);
                if ((data == NULL) || (size >= count))
                {
                    if (data == NULL)
                        data = new Uint8[count];               // allocated memory buffer
                    if (data != NULL)
                    {
                        Uint8 *q = OFstatic_cast(Uint8 *, data);
                        Uint8 value;
                        Uint16 x;
                        Uint16 y;
                        int j;
                        for (y = Rows; y != 0; --y)
                        {
                            for (x = Columns; x != 0; --x)
                            {
                                value = *(p++);                 // store gray value
                                for (j = 3; j != 0; --j)
                                    *(q++) = value;             // copy to the three RGB-planes
                            }
                            p += nextRow;                       // jump (backwards) to next row
                            q += gap;                           // skip gap at the end of each line (32-bit boundary)
                        }
                        bytes = count;
                    }
                }
            }
            else if (bits == 32)                            // -- for direct color model (32 bits/pixel)
            {
                const unsigned long count = OFstatic_cast(unsigned long, Columns) * OFstatic_cast(unsigned long, Rows);
                if ((data == NULL) || (size >= count * 4))
                {
                    if (data == NULL)
                        data = new Uint32[count];               // allocated memory buffer
                    if (data != NULL)
                    {
                        Uint32 *q = OFstatic_cast(Uint32 *, data);
                        Uint32 value;
                        Uint16 x;
                        Uint16 y;
                        for (y = Rows; y != 0; --y)
                        {
                            for (x = Columns; x != 0; --x)
                            {
                                value = *(p++);                 // store gray value
                                *(q++) = (value << 16) |
                                         (value << 8) |
                                         value;                 // copy to the three RGB-planes
                            }
                            p += nextRow;                       // jump (backwards) to next row
                        }
                        bytes = count * 4;
                    }
                }
            }
        }
        deleteOutputData();                                 // output data is no longer needed
    }
    return bytes;
}


/*
 *   create 8-bit palette/monochrome or 32-bit true color bitmap as needed for Java/AWT
 */

unsigned long DiMonoImage::createAWTBitmap(void *&data,
                                           const unsigned long frame,
                                           const int bits)
{
    data = NULL;
    unsigned long bytes = 0;
    if (bits == 8)                                      // for idx color model (byte)
    {
        getOutputData(frame, 8);                        // create output data with 8 bit depth
        if ((OutputData != NULL) && (OutputData->getData() != NULL))
        {
            bytes = OFstatic_cast(unsigned long, Columns) * OFstatic_cast(unsigned long, Rows);
            data = OutputData->getDataPtr();
            OutputData = NULL;                          // remove reference to internal memory
        }
    }
    else if (bits == 32)                                // for direct color model (long int)
    {
        getOutputData(frame, 8);                        // create output data with 8 bit depth
        if ((OutputData != NULL) && (OutputData->getData() != NULL))
        {
            const unsigned long count = OFstatic_cast(unsigned long, Columns) * OFstatic_cast(unsigned long, Rows);
            data = new Uint32[count];
            if (data != NULL)
            {
                const Uint8 *p = OFstatic_cast(const Uint8 *, OutputData->getData());
                Uint32 *q = OFstatic_cast(Uint32 *, data);
                Uint32 value;
                unsigned long i;
                for (i = count; i != 0; --i)
                {
                    value = *(p++);                     // store gray value
                    *(q++) = (value << 24) |
                             (value << 16) |
                             (value << 8);              // copy to the three RGB-planes
                }
                bytes = count * 4;
            }
        }
        deleteOutputData();                             // output data is no longer needed
    }
    return bytes;
}


/*
 *   create packed bitmap (e.g. 12 bit for DICOM printers), currently restricted to 12/16 bit
 */

void *DiMonoImage::createPackedBitmap(const void *buffer,
                                      const unsigned long size,
                                      const unsigned long count,
                                      const int alloc,              // number of bits allocated in buffer
                                      const int stored)             // number of bits stored in buffer
{
    if ((buffer != NULL) && (size > 0) && (alloc > 0) && (stored > 0) && (stored < alloc))
    {
        if ((alloc == 16) && (stored == 12))
        {
            /* need to split 'size' in order to avoid integer overflow for large pixel data */
            const unsigned long size_1 = size / alloc;
            const unsigned long size_2 = size % alloc;
//          # old code: if ((size * 8 + alloc - 1) / alloc == count)
            if (8 * size_1 + (8 * size_2 + alloc - 1) / alloc == count)
            {
                Uint16 *data = NULL;
                data = new Uint16[((count + 1) * stored - 1) / 16];     // create new memory buffer
                if (data != NULL)
                {
                    const Uint16 *p = OFstatic_cast(const Uint16 *, buffer);
                    Uint16 *q = data;
                    unsigned long i;
                    Uint16 value1;
                    Uint16 value2;
                    for (i = 0; i < count - 3; i += 4)                  // make 3 items out of 4
                    {
                        value1 = *(p++);
                        value2 = *(p++);
                        *(q++) = OFstatic_cast(Uint16, (value1 & 0x0fff) | (value2 << 12));
                        value1 = *(p++);
                        *(q++) = OFstatic_cast(Uint16, ((value2 >> 4) & 0x00ff) | (value1 << 8));
                        value2 = *(p++);
                        *(q++) = OFstatic_cast(Uint16, ((value1 >> 8) & 0x000f) | (value2 << 4));
                    }
                    switch (count - i)                                  // add remaining pixels
                    {
                        case 1:                                         // add 1 pixel
                            *(q++) = OFstatic_cast(Uint16, *(p++) & 0x0fff);
                            break;
                        case 2:                                         // add 2 pixels
                            value1 = *(p++);
                            value2 = *(p++);
                            *(q++) = OFstatic_cast(Uint16, (value1 & 0x0fff) | (value2 << 12));
                            *(q++) = OFstatic_cast(Uint16, (value2 >> 4) & 0x00ff);
                            break;
                        case 3:                                         // add 3 pixels
                            value1 = *(p++);
                            value2 = *(p++);
                            *(q++) = OFstatic_cast(Uint16, (value1 & 0x0fff) | (value2 << 12));
                            value1 = *(p++);
                            *(q++) = OFstatic_cast(Uint16, ((value2 >> 4) & 0x00ff) | (value1 << 8));
                            *(q++) = OFstatic_cast(Uint16, (value1 >> 8) & 0x000f);
                            break;
                        default:                                        // add no pixel
                            ;
                    }
                    return OFstatic_cast(void *, data);
                }
            }
        }
    }
    return NULL;
}


DiImage *DiMonoImage::createOutputImage(const unsigned long frame,
                                        const int bits)
{
    getOutputData(frame, bits);
    if ((OutputData != NULL) && (OutputData->getData() != NULL))
    {

        DiImage *image = new DiMono2Image(this, OutputData, frame, bits, OFstatic_cast(int, OutputData->getItemSize() * 8));
        if (image != NULL)
            OutputData->removeDataReference();              // output data is now handled by new mono image
        return image;
    }
    return NULL;
}


int DiMonoImage::createLinODPresentationLut(const unsigned long count, const int bits)
{
    if ((PresLutData == NULL) && (MinDensity < MaxDensity) &&
        (count > 1) && (count <= MAX_TABLE_ENTRY_COUNT) &&
        (bits > 0) && (bits <= MAX_TABLE_ENTRY_SIZE))
    {
        Uint16 *data = new Uint16[count];
        if (data != NULL)
        {
            const double l0 = OFstatic_cast(double, Illumination);
            const double la = OFstatic_cast(double, Reflection);
            const double dmin = OFstatic_cast(double, MinDensity) / 100;
            const double dmax = OFstatic_cast(double, MaxDensity) / 100;
            const double lmin = la + l0 * pow(OFstatic_cast(double, 10), -dmax);
            const double lmax = la + l0 * pow(OFstatic_cast(double, 10), -dmin);
            const double jmin = DiGSDFunction::getJNDIndex(lmin);
            const double jmax = DiGSDFunction::getJNDIndex(lmax);
            const double factor = OFstatic_cast(double, DicomImageClass::maxval(bits)) / (jmax - jmin);
            const double density = (dmax - dmin) / OFstatic_cast(double, count - 1);
            Uint16 *p = data;
            for (unsigned long i = 0; i < count; ++i)
            {
                *(p++) = OFstatic_cast(Uint16, (DiGSDFunction::getJNDIndex(la + l0 *
                    pow(OFstatic_cast(double, 10), -(dmin + OFstatic_cast(double, i) * density))) - jmin) * factor);
            }
            PresLutData = new DiLookupTable(data, count, bits);
            return (PresLutData != NULL) && (PresLutData->isValid());
        }
    }
    return 0;
}


/*********************************************************************/

void DiMonoImage::updateImagePixelModuleAttributes(DcmItem &dataset)
{
    DiImage::updateImagePixelModuleAttributes(dataset);
    /* replace any modality transformation in the dataset */
    if (dataset.tagExists(DCM_RescaleIntercept) ||
        dataset.tagExists(DCM_RescaleSlope) ||
        dataset.tagExists(DCM_ModalityLUTSequence))
    {
        dataset.putAndInsertString(DCM_RescaleIntercept, "0");
        dataset.putAndInsertString(DCM_RescaleSlope, "1");
        delete dataset.remove(DCM_ModalityLUTSequence);
    }
    /* remove embedded overlay planes */
    for (Uint16 grp = 0x6000; grp < 0x601f; grp += 2)
    {
        if (!dataset.tagExists(DcmTagKey(grp, DCM_OverlayData.getElement())))
        {
            delete dataset.remove(DcmTagKey(grp, 0x0000)); // group length
            delete dataset.remove(DcmTagKey(grp, DCM_OverlayRows.getElement()));
            delete dataset.remove(DcmTagKey(grp, DCM_OverlayColumns.getElement()));
            delete dataset.remove(DcmTagKey(grp, DCM_RETIRED_OverlayPlanes.getElement()));
            delete dataset.remove(DcmTagKey(grp, DCM_NumberOfFramesInOverlay.getElement()));
            delete dataset.remove(DcmTagKey(grp, DCM_OverlayDescription.getElement()));
            delete dataset.remove(DcmTagKey(grp, DCM_OverlayType.getElement()));
            delete dataset.remove(DcmTagKey(grp, DCM_OverlaySubtype.getElement()));
            delete dataset.remove(DcmTagKey(grp, DCM_OverlayOrigin.getElement()));
            delete dataset.remove(DcmTagKey(grp, DCM_ImageFrameOrigin.getElement()));
            delete dataset.remove(DcmTagKey(grp, DCM_RETIRED_OverlayPlaneOrigin.getElement()));
            delete dataset.remove(DcmTagKey(grp, DCM_OverlayBitsAllocated.getElement()));
            delete dataset.remove(DcmTagKey(grp, DCM_OverlayBitPosition.getElement()));
            delete dataset.remove(DcmTagKey(grp, DCM_OverlayLabel.getElement()));
        }
    }
}


// --- write current image to DICOM dataset

int DiMonoImage::writeImageToDataset(DcmItem &dataset,
                                     const int mode,
                                     const int /*planar*/)
{
    int result = 0;
    if (InterData != NULL)
    {
        const void *pixel = InterData->getData();
        const unsigned long count = InterData->getCount();
        const EP_Representation repres = InterData->getRepresentation();
        if ((BitsPerSample > 0) && (pixel != NULL) && (count > 0))
        {
            char numBuf[20];
            unsigned int bits = BitsPerSample;
            /* only 'used' pixel data */
            if (!mode)
            {
                double minValue, maxValue;
                InterData->getMinMaxValues(minValue, maxValue);
                if (minValue < maxValue)
                    bits = DicomImageClass::rangeToBits(minValue, maxValue);
                else
                    bits = 1;
            }
            /* check determined bits stored value */
            const unsigned int repBits = DicomImageClass::getRepresentationBits(repres);
            if (bits > repBits)
                bits = repBits;
            /* set color model */
            if (getInternalColorModel() == EPI_Monochrome1)
                dataset.putAndInsertString(DCM_PhotometricInterpretation, "MONOCHROME1");
            else
                dataset.putAndInsertString(DCM_PhotometricInterpretation, "MONOCHROME2");
            /* set image resolution */
            dataset.putAndInsertUint16(DCM_Columns, Columns);
            dataset.putAndInsertUint16(DCM_Rows, Rows);
#ifdef PRIu32
            sprintf(numBuf, "%" PRIu32, NumberOfFrames);
#elif SIZEOF_LONG == 8
            sprintf(numBuf, "%u", NumberOfFrames);
#else
            sprintf(numBuf, "%lu", NumberOfFrames);
#endif
            dataset.putAndInsertString(DCM_NumberOfFrames, numBuf);
            dataset.putAndInsertUint16(DCM_SamplesPerPixel, 1);
            /* set pixel encoding and data */
            switch (repres)
            {
                case EPR_Uint8:
                    dataset.putAndInsertUint16(DCM_BitsAllocated, 8);
                    dataset.putAndInsertUint16(DCM_PixelRepresentation, 0);
                    dataset.putAndInsertUint8Array(DCM_PixelData, OFstatic_cast(const Uint8 *, pixel), count);
                    break;
                case EPR_Sint8:
                    dataset.putAndInsertUint16(DCM_BitsAllocated, 8);
                    dataset.putAndInsertUint16(DCM_PixelRepresentation, 1);
                    dataset.putAndInsertUint8Array(DCM_PixelData, OFstatic_cast(const Uint8 *, pixel), count);
                    break;
                case EPR_Uint16:
                    dataset.putAndInsertUint16(DCM_BitsAllocated, 16);
                    dataset.putAndInsertUint16(DCM_PixelRepresentation, 0);
                    dataset.putAndInsertUint16Array(DCM_PixelData, OFstatic_cast(const Uint16 *, pixel), count);
                    break;
                case EPR_Sint16:
                    dataset.putAndInsertUint16(DCM_BitsAllocated, 16);
                    dataset.putAndInsertUint16(DCM_PixelRepresentation, 1);
                    dataset.putAndInsertUint16Array(DCM_PixelData, OFstatic_cast(const Uint16 *, pixel), count);
                    break;
                case EPR_Uint32:
                    dataset.putAndInsertUint16(DCM_BitsAllocated, 32);
                    dataset.putAndInsertUint16(DCM_PixelRepresentation, 0);
                    dataset.putAndInsertUint16Array(DCM_PixelData, OFstatic_cast(const Uint16 *, pixel), count * 2 /*double-words*/);
                    break;
                case EPR_Sint32:
                    dataset.putAndInsertUint16(DCM_BitsAllocated, 32);
                    dataset.putAndInsertUint16(DCM_PixelRepresentation, 1);
                    dataset.putAndInsertUint16Array(DCM_PixelData, OFstatic_cast(const Uint16 *, pixel), count * 2 /*double-words*/);
                    break;
            }
            dataset.putAndInsertUint16(DCM_BitsStored, bits);
            dataset.putAndInsertUint16(DCM_HighBit, bits - 1);
            /* update other DICOM attributes */
            updateImagePixelModuleAttributes(dataset);
            result = 1;
        }
    }
    return result;
}


/*
 *   write output data of 'frame' with depth of 'bits' to C++-output 'stream' (format is PGM - portable gray map)
 */

int DiMonoImage::writePPM(STD_NAMESPACE ostream& stream,
                          const unsigned long frame,
                          const int bits)
{
    getOutputData(frame, bits);
    if (OutputData != NULL)
    {
        if (bits == MI_PastelColor)
        {
            stream << "P3" << OFendl;
            stream << Columns << " " << Rows << OFendl;
            stream << "255" << OFendl;
        } else {
            stream << "P2" << OFendl;
            stream << Columns << " " << Rows << OFendl;
            stream << DicomImageClass::maxval(bits) << OFendl;
        }
        int ok = OutputData->writePPM(stream);
        deleteOutputData();
        return ok;
    }
    return 0;
}


/*
 *   write output data of 'frame' with depth of 'bits' to C-file 'stream' (format is PGM - portable gray map)
 */

int DiMonoImage::writePPM(FILE *stream,
                          const unsigned long frame,
                          const int bits)
{
    if (stream != NULL)
    {
        getOutputData(frame, bits);
        if (OutputData != NULL)
        {
            if (bits == MI_PastelColor)
                fprintf(stream, "P3\n%u %u\n255\n", Columns, Rows);
            else
                fprintf(stream, "P2\n%u %u\n%lu\n", Columns, Rows, DicomImageClass::maxval(bits));
            int ok = OutputData->writePPM(stream);
            deleteOutputData();
            return ok;
        }
    }
    return 0;
}


/*
 *   write output data of 'frame' with depth of 'bits' (max. 8) to C-file 'stream' (format is RAW-PGM - binary PGM)
 */

int DiMonoImage::writeRawPPM(FILE *stream,
                             const unsigned long frame,
                             const int bits)
{
    if ((stream != NULL) && (bits <= MAX_RAWPPM_BITS))
    {
        getOutputData(frame, bits);
        if ((OutputData != NULL) && (OutputData->getData() != NULL))
        {
            if (bits == MI_PastelColor)
                fprintf(stream, "P6\n%u %u\n255\n", Columns, Rows);
            else
                fprintf(stream, "P5\n%u %u\n%lu\n", Columns, Rows, DicomImageClass::maxval(bits));
            const size_t count = OFstatic_cast(size_t, OutputData->getCount());
            int ok = (fwrite(OutputData->getData(), OutputData->getItemSize(), count, stream) == count) ? 1 : 0;
            deleteOutputData();
            return ok;
        }
    }
    return 0;
}


/*
 *   write output data of 'frame' with depth of 'bits' (8, 24 or 32) to C-file 'stream' (format is BMP)
 */

int DiMonoImage::writeBMP(FILE *stream,
                          const unsigned long frame,
                          const int bits)
{
    if ((bits == 0) || (bits == 8) || (bits == 24) || (bits == 32))
        return DiImage::writeBMP(stream, frame, (bits == 0) ? 8 : bits);
    return 0;
}
