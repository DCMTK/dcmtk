/*
 *
 *  Copyright (C) 1996-2018, OFFIS e.V.
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
 *  Purpose: DicomMonochromeModality (Source)
 *
 */


#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmdata/dcdeftag.h"
#include "dcmtk/dcmdata/dcuid.h"

#include "dcmtk/dcmimgle/dimomod.h"
#include "dcmtk/dcmimgle/didocu.h"
#include "dcmtk/dcmimgle/diinpx.h"


/*----------------*
 *  constructors  *
 *----------------*/

DiMonoModality::DiMonoModality(const DiDocument *docu,
                               DiInputPixel *pixel)
  : Representation(EPR_MaxSigned),
    MinValue(0),
    MaxValue(0),
    Bits(0),
    UsedBits(0),
    AbsMinimum(0),
    AbsMaximum(0),
    RescaleIntercept(0),
    RescaleSlope(0),
    LookupTable(0),
    Rescaling(0),
    TableData(NULL)
{
    if (Init(docu, pixel))
    {
        if (docu->getFlags() & CIF_IgnoreModalityTransformation)        // ignore modality LUT and rescaling
        {
            DCMIMGLE_INFO("configuration flag set ... ignoring possible modality transform");
        }
        else if (!(docu->getFlags() & CIF_UsePresentationState))        // ignore modality LUT and rescaling
        {
            const char *sopClassUID = NULL;                             // check for XA and XRF image (ignore MLUT)
            if (!docu->getValue(DCM_SOPClassUID, sopClassUID) || (sopClassUID == NULL))
                sopClassUID = "";
            if ((strcmp(sopClassUID, UID_XRayAngiographicImageStorage) == 0) ||
                (strcmp(sopClassUID, UID_XRayRadiofluoroscopicImageStorage) == 0) ||
                (strcmp(sopClassUID, UID_RETIRED_XRayAngiographicBiPlaneImageStorage) == 0))
            {
                /* David Clunie in comp.protocols.dicom (2000-12-13):
                    "Modality LUTs in XA and XRF objects are totally screwy and
                     do not follow the normal rules. [...] A Modality LUT may be
                     included with the image to allow it to be scaled back to its
                     proportional value to X-Ray beam intensity. In other words,
                     for the objects that use this module (XA and XRF), the
                     Modality LUT is used BACKWARDS. It is used to convert stored
                     pixels to X-Ray beam intensity space, but it is NOT APPLIED
                     to stored pixels for the purpose of display (or more
                     specifically prior to application of the VOI LUT Module
                     attributes to the stored pixel data)."
                */
                DCMIMGLE_INFO("processing XA or XRF image ... ignoring possible modality transform");
            } else {
                EL_BitsPerTableEntry descMode = ELM_UseValue;          // first search on main level
                if (docu->getFlags() & CIF_IgnoreModalityLutBitDepth)
                    descMode = ELM_IgnoreValue;
                else if (docu->getFlags() & CIF_CheckLutBitDepth)
                    descMode = ELM_CheckValue;
                TableData = new DiLookupTable(docu, DCM_ModalityLUTSequence, DCM_LUTDescriptor, DCM_LUTData,
                    DCM_LUTExplanation, descMode);
                checkTable();
                if (LookupTable)
                    DCMIMGLE_DEBUG("found modality LUT on main dataset level");
                Rescaling = (docu->getValue(DCM_RescaleIntercept, RescaleIntercept) > 0);
                Rescaling &= (docu->getValue(DCM_RescaleSlope, RescaleSlope) > 0);
                if (Rescaling)
                    DCMIMGLE_DEBUG("found 'RescaleSlope/Intercept' on main dataset level");
                else if (!LookupTable)                                 // then check for functional groups sequence
                {
                    DcmSequenceOfItems *seq = NULL;
                    if (docu->getSequence(DCM_SharedFunctionalGroupsSequence, seq))
                    {
                        DcmItem *item = seq->getItem(0);
                        if ((item != NULL) && docu->getSequence(DCM_PixelValueTransformationSequence, seq, item))
                        {
                            item = seq->getItem(0);
                            if (item != NULL)
                            {
                                Rescaling = (docu->getValue(DCM_RescaleIntercept, RescaleIntercept, 0, item) > 0);
                                Rescaling &= (docu->getValue(DCM_RescaleSlope, RescaleSlope, 0, item) > 0);
                                if (Rescaling)
                                    DCMIMGLE_DEBUG("found 'RescaleSlope/Intercept' in 'SharedFunctionalGroupsSequence'");
                            }
                        }
                    }
                }
                checkRescaling(pixel);
                if (Rescaling || LookupTable)                          // check for possibly inappropriate use of MLUT
                {
                    /* David Clunie in comp.protocols.dicom (2012-10-28):
                        "By the way, in general, it can be difficult to decide
                         whether or not to apply the conceptual Modality LUT
                         step before windowing, even if it is specified by
                         Rescale Slope/Intercept values rather than an actual
                         LUT. For example, in MR images to which Philips has
                         added the rescale values, these should not be applied
                         before their window values; likewise in PET images,
                         especially those with GML Units and rescale values to
                         SUV (small decimal numbers), the window values are
                         historically usually in stored pixel values rather than
                         SUVs.
                         Making the correct decision may require comparing the
                         range of possible rescaled output values (across the
                         domain of possible input stored pixel values) with the
                         specific window values, to see if the latter "make
                         sense".
                    */
                    if (strcmp(sopClassUID, UID_MRImageStorage) == 0)
                        DCMIMGLE_WARN("processing MR image ... applying modality transform may create unexpected result");
                    else if (strcmp(sopClassUID, UID_PositronEmissionTomographyImageStorage) == 0)
                        DCMIMGLE_WARN("processing PET image ... applying modality transform may create unexpected result");
                    else if (strcmp(sopClassUID, UID_RTDoseStorage) == 0)
                        DCMIMGLE_WARN("processing RTDOSE object ... applying modality transform may create unexpected result");
                }
            }
        }
        determineRepresentation(docu);
    }
}


DiMonoModality::DiMonoModality(const DiDocument *docu,
                               DiInputPixel *pixel,
                               const double slope,
                               const double intercept)
  : Representation(EPR_MaxSigned),
    MinValue(0),
    MaxValue(0),
    Bits(0),
    UsedBits(0),
    AbsMinimum(0),
    AbsMaximum(0),
    RescaleIntercept(intercept),
    RescaleSlope(slope),
    LookupTable(0),
    Rescaling(0),
    TableData(NULL)
{
    if (Init(docu, pixel))
    {
        Rescaling = 1;
        checkRescaling(pixel);
        determineRepresentation(docu);
    }
}


DiMonoModality::DiMonoModality(const DiDocument *docu,
                               DiInputPixel *pixel,
                               const DcmUnsignedShort &data,
                               const DcmUnsignedShort &descriptor,
                               const DcmLongString *explanation)
  : Representation(EPR_MaxSigned),
    MinValue(0),
    MaxValue(0),
    Bits(0),
    UsedBits(0),
    AbsMinimum(0),
    AbsMaximum(0),
    RescaleIntercept(0),
    RescaleSlope(0),
    LookupTable(0),
    Rescaling(0),
    TableData(NULL)
{
    if (Init(docu, pixel))
    {
        EL_BitsPerTableEntry descMode = ELM_UseValue;
        if (docu->getFlags() & CIF_IgnoreModalityLutBitDepth)
            descMode = ELM_IgnoreValue;
        else if (docu->getFlags() & CIF_CheckLutBitDepth)
            descMode = ELM_CheckValue;
        TableData = new DiLookupTable(data, descriptor, explanation, descMode);
        checkTable();
        determineRepresentation(docu);
    }
}


DiMonoModality::DiMonoModality(const int bits)
  : Representation(EPR_MaxSigned),
    MinValue(0),
    MaxValue(0),
    Bits(bits),
    UsedBits(0),
    AbsMinimum(0),
    AbsMaximum(DicomImageClass::maxval(bits)),
    RescaleIntercept(0),
    RescaleSlope(0),
    LookupTable(0),
    Rescaling(0),
    TableData(NULL)
{
}


/*--------------*
 *  destructor  *
 *--------------*/

DiMonoModality::~DiMonoModality()
{
    delete TableData;
}


/*********************************************************************/


int DiMonoModality::Init(const DiDocument *docu,
                         DiInputPixel *pixel)
{
    if ((docu != NULL) && (pixel != NULL))
    {
        pixel->determineMinMax();
        MinValue = pixel->getMinValue(1 /* selected range of pixels only */);
        MaxValue = pixel->getMaxValue(1 /* selected range of pixels only */);
        Bits = pixel->getBits();
        AbsMinimum = pixel->getAbsMinimum();
        AbsMaximum = pixel->getAbsMaximum();
        Uint16 us;
        if (docu->getValue(DCM_SamplesPerPixel, us) > 0)
        {
            if (us != 1)
                DCMIMGLE_WARN("invalid value for 'SamplesPerPixel' (" << us << ") ... assuming 1");
        } else
            DCMIMGLE_WARN("missing value for 'SamplesPerPixel' ... assuming 1");
        return 1;
    }
    return 0;
}


void DiMonoModality::checkTable()
{
    if (TableData != NULL)
    {
        LookupTable = TableData->isValid();
        if (LookupTable)
        {
            MinValue = TableData->getMinValue();
            MaxValue = TableData->getMaxValue();
            Bits = TableData->getBits();
            AbsMinimum = 0;
            AbsMaximum = DicomImageClass::maxval(Bits);
        }
    }
}


void DiMonoModality::checkRescaling(const DiInputPixel *pixel)
{
    if (Rescaling)
    {
        if (LookupTable)
        {
            DCMIMGLE_WARN("redundant values for 'RescaleSlope/Intercept' ... using modality LUT transformation");
            Rescaling = 0;
        } else {
            if (RescaleSlope == 0)
            {
                DCMIMGLE_WARN("invalid value for 'RescaleSlope' (" << RescaleSlope << ") ... ignoring modality transformation");
                Rescaling = 0;
            } else {
                if (RescaleSlope < 0)                                       // negative slope value
                {
                    const double temp = MinValue;
                    MinValue = MaxValue * RescaleSlope + RescaleIntercept;
                    MaxValue = temp * RescaleSlope + RescaleIntercept;
                    AbsMinimum = pixel->getAbsMaximum() * RescaleSlope + RescaleIntercept;
                    AbsMaximum = pixel->getAbsMinimum() * RescaleSlope + RescaleIntercept;
                } else {                                                    // positive slope value
                    MinValue = MinValue * RescaleSlope + RescaleIntercept;
                    MaxValue = MaxValue * RescaleSlope + RescaleIntercept;
                    AbsMinimum = pixel->getAbsMinimum() * RescaleSlope + RescaleIntercept;
                    AbsMaximum = pixel->getAbsMaximum() * RescaleSlope + RescaleIntercept;
                }
                Bits = DicomImageClass::rangeToBits(AbsMinimum, AbsMaximum);
            }
        }
    }
}


void DiMonoModality::determineRepresentation(const DiDocument *docu)
{
    UsedBits = DicomImageClass::rangeToBits(MinValue, MaxValue);
    if ((docu != NULL) && (docu->getFlags() & CIF_UseAbsolutePixelRange))
        Representation = DicomImageClass::determineRepresentation(AbsMinimum, AbsMaximum);
    else
        Representation = DicomImageClass::determineRepresentation(MinValue, MaxValue);
    DCMIMGLE_TRACE("internal representation for monochrome images: "
        << DicomImageClass::getRepresentationBits(Representation) << " bits ("
        << (DicomImageClass::isRepresentationSigned(Representation) ? "signed" : "unsigned") << ")");
}
