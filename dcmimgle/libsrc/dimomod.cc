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
 *  Purpose: DicomMonochromeModality (Source)
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:14:18 $
 *  CVS/RCS Revision: $Revision: 1.30 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
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
        if (!(docu->getFlags() & CIF_UsePresentationState) &&           // ignore modality LUT and rescaling
            !(docu->getFlags() & CIF_IgnoreModalityTransformation))
        {
            const char *sopClassUID = NULL;                             // check for XA and XRF image (ignore MLUT)
            if ((docu->getValue(DCM_SOPClassUID, sopClassUID) == 0) || (sopClassUID == NULL) ||
               ((strcmp(sopClassUID, UID_XRayAngiographicImageStorage) != 0) &&
                (strcmp(sopClassUID, UID_XRayRadiofluoroscopicImageStorage) != 0) &&
                (strcmp(sopClassUID, UID_RETIRED_XRayAngiographicBiPlaneImageStorage) != 0)))
            {
                EL_BitsPerTableEntry descMode = ELM_UseValue;
                if (docu->getFlags() & CIF_IgnoreModalityLutBitDepth)
                    descMode = ELM_IgnoreValue;
                else if (docu->getFlags() & CIF_CheckLutBitDepth)
                    descMode = ELM_CheckValue;
                TableData = new DiLookupTable(docu, DCM_ModalityLUTSequence, DCM_LUTDescriptor, DCM_LUTData,
                    DCM_LUTExplanation, descMode);
                checkTable();
                Rescaling = (docu->getValue(DCM_RescaleIntercept, RescaleIntercept) > 0);
                Rescaling &= (docu->getValue(DCM_RescaleSlope, RescaleSlope) > 0);
                checkRescaling(pixel);
            } else {
                DCMIMGLE_INFO("processing XA or XRF image ... ignoring possible modality transform");
            }
        } else {
            DCMIMGLE_INFO("configuration flag set ... ignoring possible modality transform");
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
        if (docu->getValue(DCM_SamplesPerPixel, us) && (us != 1))
        {
            DCMIMGLE_WARN("invalid value for 'SamplesPerPixel' (" << us << ") ... assuming 1");
        }
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


/*
 *
 * CVS/RCS Log:
 * $Log: dimomod.cc,v $
 * Revision 1.30  2010-10-14 13:14:18  joergr
 * Updated copyright header. Added reference to COPYRIGHT file.
 *
 * Revision 1.29  2010-09-24 13:25:41  joergr
 * Compared names of SOP Class UIDs with 2009 edition of the DICOM standard. The
 * resulting name changes are mainly caused by the fact that the corresponding
 * SOP Class is now retired.
 *
 * Revision 1.28  2010-02-23 16:52:22  joergr
 * Added trace log message which outputs the internal representation for
 * monochrome images (number of bits and signed/unsigned).
 *
 * Revision 1.27  2009-10-28 14:26:02  joergr
 * Fixed minor issues in log output.
 *
 * Revision 1.26  2009-10-28 09:53:41  uli
 * Switched to logging mechanism provided by the "new" oflog module.
 *
 * Revision 1.25  2009-04-21 08:27:30  joergr
 * Added new compatibility flag CIF_UseAbsolutePixelRange which changes the way
 * the internal representation of monochrome images is determined.
 * Added method getUsedBits() which allows for retrieving the number of bits
 * actually used to store the output data.
 *
 * Revision 1.24  2008-06-20 12:07:26  joergr
 * Added check for retired SOP Class 'X-Ray Angiographic Bi-Plane Image Storage'
 * since the Modality LUT should not be applied to the pixel data in this case.
 *
 * Revision 1.23  2007/03/16 11:51:15  joergr
 * Introduced new flag that allows to select how to handle the BitsPerTableEntry
 * value in the LUT descriptor (use, ignore or check).
 *
 * Revision 1.22  2006/08/15 16:30:11  meichel
 * Updated the code in module dcmimgle to correctly compile when
 *   all standard C++ classes remain in namespace std.
 *
 * Revision 1.21  2005/12/08 15:43:00  meichel
 * Changed include path schema for all DCMTK header files
 *
 * Revision 1.20  2005/03/09 17:37:08  joergr
 * Fixed bug in calculation of bits stored value after modality transformation.
 *
 * Revision 1.19  2003/12/17 16:18:34  joergr
 * Added new compatibility flag that allows to ignore the third value of LUT
 * descriptors and to determine the bits per table entry automatically.
 *
 * Revision 1.18  2003/12/08 17:38:27  joergr
 * Updated CVS header.
 *
 * Revision 1.17  2003/12/08 14:36:35  joergr
 * Adapted type casts to new-style typecast operators defined in ofcast.h.
 *
 * Revision 1.16  2003/05/20 09:25:08  joergr
 * Added new configuration/compatibility flag that allows to ignore the
 * modality transform stored in the dataset.
 *
 * Revision 1.15  2001/09/28 13:17:24  joergr
 * Enhanced algorithm to determine the min and max value.
 *
 * Revision 1.14  2001/06/01 15:49:58  meichel
 * Updated copyright header
 *
 * Revision 1.13  2000/12/14 13:46:45  joergr
 * Ignore modality LUT transform for XA and XRF images (report message on that
 * in verbose mode).
 *
 * Revision 1.12  2000/08/31 15:51:39  joergr
 * Corrected bug: min and max value were reversed for images with negative
 * rescale slope.
 *
 * Revision 1.11  2000/04/28 12:33:46  joergr
 * DebugLevel - global for the module - now derived from OFGlobal (MF-safe).
 *
 * Revision 1.10  2000/04/27 13:10:30  joergr
 * Dcmimgle library code now consistently uses ofConsole for error output.
 *
 * Revision 1.9  2000/03/08 16:24:31  meichel
 * Updated copyright header.
 *
 * Revision 1.8  2000/03/03 14:09:21  meichel
 * Implemented library support for redirecting error messages into memory
 *   instead of printing them to stdout/stderr for GUI applications.
 *
 * Revision 1.7  1999/05/31 12:35:58  joergr
 * Corrected bug concerning the conversion of color images to grayscale.
 *
 * Revision 1.6  1999/04/28 15:04:48  joergr
 * Introduced new scheme for the debug level variable: now each level can be
 * set separately (there is no "include" relationship).
 *
 * Revision 1.5  1999/02/03 17:41:45  joergr
 * Moved global functions maxval() and determineRepresentation() to class
 * DicomImageClass (as static methods).
 * Added member variable and related methods to store number of bits used for
 * pixel data.
 *
 * Revision 1.4  1998/12/22 13:41:04  joergr
 * Changed calculation of AbsMinimum/Maximum.
 * Removed member variable and method for isPotentiallySigned.
 *
 * Revision 1.3  1998/12/16 16:16:50  joergr
 * Added explanation string to LUT class (retrieved from dataset).
 *
 * Revision 1.2  1998/12/14 17:38:18  joergr
 * Added support for correct scaling of input/output values for grayscale
 * transformations.
 *
 * Revision 1.1  1998/11/27 16:14:35  joergr
 * Added copyright message.
 * Introduced global debug level for dcmimage module to control error output.
 * Added constructors to use external modality transformations.
 *
 * Revision 1.4  1998/05/11 14:52:33  joergr
 * Added CVS/RCS header to each file.
 *
 *
 */
