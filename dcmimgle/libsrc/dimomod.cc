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
 *  Purpose: DicomMonochromeModality (Source)
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 1998-12-14 17:38:18 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmimgle/libsrc/dimomod.cc,v $
 *  CVS/RCS Revision: $Revision: 1.2 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#include "osconfig.h"
#include "dcdeftag.h"

#include "dimomod.h"
#include "didocu.h"
#include "diinpx.h"


/*----------------*
 *  constructors  *
 *----------------*/

DiMonoModality::DiMonoModality(const DiDocument *docu,
                               DiInputPixel *pixel)
  : Representation(EPR_MaxSigned),
    MinValue(0),
    MaxValue(0),
    RescaleIntercept(0),
    RescaleSlope(0),
    LookupTable(0),
    Rescaling(0),
    PotentialSignedRange(0),
    TableData(NULL)
{
    if (Init(docu, pixel))
    {
        if (!(docu->getFlags() & CIF_UsePresentationState))             // ignore modality LUT and rescaling
        {
            TableData = new DiLookupTable(docu, DCM_ModalityLUTSequence, DCM_LUTDescriptor, DCM_LUTData);
            checkTable();
            Rescaling = (docu->getValue(DCM_RescaleIntercept, RescaleIntercept) > 0);
            Rescaling &= (docu->getValue(DCM_RescaleSlope, RescaleSlope) > 0);
            checkRescaling(pixel);
        }
        Representation = determineRepresentation(MinValue, MaxValue);
    }
}


DiMonoModality::DiMonoModality(const DiDocument *docu,
                               DiInputPixel *pixel,
                               const double slope,
                               const double intercept)
  : Representation(EPR_MaxSigned),
    MinValue(0),
    MaxValue(0),
    RescaleIntercept(intercept),
    RescaleSlope(slope),
    LookupTable(0),
    Rescaling(0),
    PotentialSignedRange(0),
    TableData(NULL)
{
    if (Init(docu, pixel))
    {
        Rescaling = 1;
        checkRescaling(pixel);
        Representation = determineRepresentation(MinValue, MaxValue);
    }
}


DiMonoModality::DiMonoModality(const DiDocument *docu,
                               DiInputPixel *pixel,
                               const DcmUnsignedShort &data,
                               const DcmUnsignedShort &descriptor)
  : Representation(EPR_MaxSigned),
    MinValue(0),
    MaxValue(0),
    RescaleIntercept(0),
    RescaleSlope(0),
    LookupTable(0),
    Rescaling(0),
    PotentialSignedRange(0),
    TableData(NULL)
{
    if (Init(docu, pixel))
    {
        TableData = new DiLookupTable(data, descriptor);
        checkTable();
        Representation = determineRepresentation(MinValue, MaxValue);
    }
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
        MinValue = pixel->getMinValue(); 
        MaxValue = pixel->getMaxValue();
        Uint16 us;
        if (docu->getValue(DCM_SamplesPerPixel, us) && (us != 1))
        {
            if (DicomImageClass::DebugLevel >= DicomImageClass::DL_Warnings)
                cerr << "WARNING: invalid value for 'SamplesPerPixel' (" << us << ") ... assuming 1 !" << endl;
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
        }
    }
}


void DiMonoModality::checkRescaling(const DiInputPixel *pixel)
{
    if (Rescaling)
    {
        if (LookupTable) {
            if (DicomImageClass::DebugLevel >= DicomImageClass::DL_Warnings)
            {
                cerr << "WARNING: redundant values for 'RescaleSlope/Intercept'";
                cerr << " ... using modality LUT transformation !" << endl;
            }
            Rescaling = 0;
        } else {
            if (RescaleSlope == 0)
            {
                if (DicomImageClass::DebugLevel >= DicomImageClass::DL_Warnings)
                {
                    cerr << "WARNING: invalid value for 'RescaleSlope' (" << RescaleSlope;
                    cerr << ") ... ignoring modality transformation !" << endl;
                }
                Rescaling = 0;
            }
            else
            {
                if (RescaleSlope < 0) {                                     // negative slope value
                    MinValue = MaxValue * RescaleSlope + RescaleIntercept;
                    MaxValue = MinValue * RescaleSlope + RescaleIntercept;
                } else {                                                    // positive slope value
                    MinValue = MinValue * RescaleSlope + RescaleIntercept;
                    MaxValue = MaxValue * RescaleSlope + RescaleIntercept;
                }
            }
/*
            if ((pixel->getAbsMinimum() * RescaleSlope + RescaleIntercept < 0) || (pixel->getAbsMaximum() * RescaleSlope + RescaleIntercept < 0))
                PotentialSignedRange = 1;
*/
        }
    }
}

    
/*
**
** CVS/RCS Log:
** $Log: dimomod.cc,v $
** Revision 1.2  1998-12-14 17:38:18  joergr
** Added support for correct scaling of input/output values for grayscale
** transformations.
**
** Revision 1.1  1998/11/27 16:14:35  joergr
** Added copyright message.
** Introduced global debug level for dcmimage module to control error output.
** Added constructors to use external modality transformations.
**
** Revision 1.4  1998/05/11 14:52:33  joergr
** Added CVS/RCS header to each file.
**
**
*/
