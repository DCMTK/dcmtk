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
 *  Purpose: DicomMonochromeImage (Source)
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 1999-01-11 09:37:28 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmimgle/libsrc/dimoimg.cc,v $
 *  CVS/RCS Revision: $Revision: 1.8 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#include "osconfig.h"
#include "dctypes.h"
#include "dcdeftag.h"

#include "dimoimg.h"
#include "dimo2img.h"
#include "dimoipxt.h"
#include "dimocpt.h"
#include "dimosct.h"
#include "dimoflt.h"
#include "dimorot.h"
#include "dimoopxt.h"
#include "diluptab.h"
#include "didocu.h"
#include "diutils.h"
#include "diregbas.h"


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
    PresLutShape(ESP_Identity),
    VoiLutData(NULL),
    PresLutData(NULL),
    InterData(NULL),
    OutputData(NULL),
    OverlayData(NULL)
{
    Overlays[0] = NULL;
    Overlays[1] = NULL;
    if ((Document != NULL) && (InputData != NULL) && (ImageStatus == EIS_Normal))
    {
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
    PresLutShape(ESP_Identity),
    VoiLutData(NULL),
    PresLutData(NULL),
    InterData(NULL),
    OutputData(NULL),
    OverlayData(NULL)
{
    Overlays[0] = NULL;
    Overlays[1] = NULL;
    if ((Document != NULL) && (InputData != NULL) && (ImageStatus == EIS_Normal))
    {
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
    PresLutShape(ESP_Identity),
    VoiLutData(NULL),
    PresLutData(NULL),
    InterData(NULL),
    OutputData(NULL),
    OverlayData(NULL)
{
    Overlays[0] = NULL;
    Overlays[1] = NULL;
    if ((Document != NULL) && (InputData != NULL) && (ImageStatus == EIS_Normal))
    {
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
    PresLutShape(ESP_Identity),
    VoiLutData(NULL),
    PresLutData(NULL),
    InterData(NULL),
    OutputData(NULL),
    OverlayData(NULL)
{
    Overlays[0] = NULL;
    Overlays[1] = NULL;
}


/*
 *   
 */

DiMonoImage::DiMonoImage(const DiMonoImage *image,
                         const unsigned long fstart,
                         unsigned long fcount)
  : DiImage(image, fstart, fcount),
    WindowCenter(image->WindowCenter),
    WindowWidth(image->WindowWidth),
    WindowCount(image->WindowCount),
    VoiLutCount(image->VoiLutCount),
    ValidWindow(image->ValidWindow),
    VoiExplanation(image->VoiExplanation),
    PresLutShape(image->PresLutShape),
    VoiLutData(image->VoiLutData),
    PresLutData(image->PresLutData),
    InterData(NULL),
    OutputData(NULL),
    OverlayData(NULL)
{
    Overlays[0] = image->Overlays[0];
    Overlays[1] = image->Overlays[1];
    if (image->InterData != NULL)
    {
        const unsigned long fsize = (unsigned long)Columns * (unsigned long)Rows;
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
    for (int i = 0; i < 2; i++)
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
  : DiImage((DiImage *)image),
    WindowCenter(0),
    WindowWidth(0),
    WindowCount(0),
    VoiLutCount(0),
    ValidWindow(0),
    VoiExplanation(),
    PresLutShape(ESP_Identity),
    VoiLutData(NULL),
    PresLutData(NULL),
    InterData(NULL),
    OutputData(NULL),
    OverlayData(NULL)
{
    Overlays[0] = NULL;
    Overlays[1] = NULL;
    if (DiRegisterBase::Pointer != NULL)
        InterData = DiRegisterBase::Pointer->createMonoImageData(image, red, green, blue);
    if ((InterData == NULL) || (InterData->getData() == NULL))
        ImageStatus = EIS_InvalidImage;
}


/*
 *   scale 'image' to size given by 'columns' and 'rows', 'interpolate' always if parameter is true
 */

DiMonoImage::DiMonoImage(const DiMonoImage *image,
                         const Uint16 left,
                         const Uint16 top,
                         const Uint16 src_cols,
                         const Uint16 src_rows,                 
                         const Uint16 dest_cols,
                         const Uint16 dest_rows,
                         const int interpolate,
                         const int aspect)
  : DiImage(image, dest_cols, dest_rows, aspect),
    WindowCenter(image->WindowCenter),
    WindowWidth(image->WindowWidth),
    WindowCount(image->WindowCount),
    VoiLutCount(image->VoiLutCount),
    ValidWindow(image->ValidWindow),
    VoiExplanation(image->VoiExplanation),
    PresLutShape(image->PresLutShape),
    VoiLutData(image->VoiLutData),
    PresLutData(image->PresLutData),
    InterData(NULL),
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
                InterData = new DiMonoScaleTemplate<Uint8>(image->InterData, image->Columns, image->Rows, left, top,
                    src_cols, src_rows, dest_cols, dest_rows, NumberOfFrames, interpolate);
                break;
            case EPR_Sint8:
                InterData = new DiMonoScaleTemplate<Sint8>(image->InterData, image->Columns, image->Rows, left, top,
                    src_cols, src_rows, dest_cols, dest_rows, NumberOfFrames, interpolate);
                break;
            case EPR_Uint16:
                InterData = new DiMonoScaleTemplate<Uint16>(image->InterData, image->Columns, image->Rows, left, top,
                    src_cols, src_rows, dest_cols, dest_rows, NumberOfFrames, interpolate);
                break;
            case EPR_Sint16:
                InterData = new DiMonoScaleTemplate<Sint16>(image->InterData, image->Columns, image->Rows, left, top,
                    src_cols, src_rows, dest_cols, dest_rows, NumberOfFrames, interpolate);
                break;
            case EPR_Uint32:
                InterData = new DiMonoScaleTemplate<Uint32>(image->InterData, image->Columns, image->Rows, left, top,
                    src_cols, src_rows, dest_cols, dest_rows, NumberOfFrames, interpolate);
                break;
            case EPR_Sint32:
                InterData = new DiMonoScaleTemplate<Sint32>(image->InterData, image->Columns, image->Rows, left, top,
                    src_cols, src_rows, dest_cols, dest_rows, NumberOfFrames, interpolate);
                break;
        }
    }
    if (checkInterData(0))
    {
        for (int i = 0; i < 2; i++)
        {
            if ((image->Overlays[i] != NULL) && (image->Overlays[i]->getCount() > 0))
                Overlays[i] = new DiOverlay(image->Overlays[i], left, top, (double)dest_cols / (double)src_cols, (double)dest_rows / (double)src_rows);
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
    PresLutShape(image->PresLutShape),
    VoiLutData(image->VoiLutData),
    PresLutData(image->PresLutData),
    InterData(NULL),
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
        for (int i = 0; i < 2; i++)
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
    PresLutShape(image->PresLutShape),
    VoiLutData(image->VoiLutData),
    PresLutData(image->PresLutData),
    InterData(NULL),
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
        for (int i = 0; i < 2; i++)
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
    PresLutShape(ESP_Identity),
    VoiLutData(NULL),
    PresLutData(NULL),
    InterData(NULL),
    OutputData(NULL),
    OverlayData(NULL)
{
    if (DicomImageClass::DebugLevel >= DicomImageClass::DL_Errors)
        cerr << "ERROR in DiMonoImage copy-constructor !!!" << endl;
    abort();
}


/*--------------*
 *  destructor  *
 *--------------*/

DiMonoImage::~DiMonoImage()
{
    delete InterData;
    delete OutputData;
    delete OverlayData;
    if (VoiLutData != NULL)
        VoiLutData->removeReference();          // only delete if object is no longer referenced
    if (PresLutData != NULL)
        PresLutData->removeReference();
    for (int i = 0; i < 2; i++)
    {
        if (Overlays[i] != NULL)
            Overlays[i]->removeReference();
    }
}


/*********************************************************************/


void DiMonoImage::Init(DiMonoModality *modality)
{
    if (modality != NULL)
    {
        Overlays[0] = new DiOverlay(Document, BitsAllocated);
        if ((Overlays[0] != NULL) && !(Document->getFlags() & CIF_UsePresentationState))
            Overlays[0]->showAllPlanes();                       // default: show all overlays with stored modes
        if ((Overlays[0] == NULL) || (Overlays[0]->getCount() == 0) || (!Overlays[0]->hasEmbeddedData()))
            detachPixelData();                                  // no longer needed, save memory
        switch (InputData->getRepresentation())
        {
            case EPR_Uint8:
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
                break;
            case EPR_Sint8:
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
                break;
            case EPR_Uint16:
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
                break;
            case EPR_Sint16:
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
                break;
            case EPR_Uint32:
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
                break;
            case EPR_Sint32:
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
                break;
        }
        deleteInputData();                                  // no longer needed, save memory
        if ((modality->hasLookupTable()) && (modality->getTableData() != NULL))
            BitsPerSample = modality->getTableData()->getBits();
        if (checkInterData() && !(Document->getFlags() & CIF_UsePresentationState))
        {
            WindowCount = Document->getVM(DCM_WindowCenter);
            const unsigned long count = Document->getVM(DCM_WindowWidth);
            if (count < WindowCount)                        // determine number of voi windows
                WindowCount = count;
            DcmSequenceOfItems *seq = NULL;
            VoiLutCount = Document->getSequence(DCM_VOILUTSequence, seq);            
        }
    } else
        detachPixelData();
}


int DiMonoImage::checkInterData(const int mode)
{
    if (InterData == NULL)
    {
        if (ImageStatus == EIS_Normal)
        {
            ImageStatus = EIS_MemoryFailure;
            if (DicomImageClass::DebugLevel >= DicomImageClass::DL_Errors)
                cerr << "ERROR: can't allocate memory for inter-representation !" << endl;
        } else
            ImageStatus = EIS_InvalidImage;
    }
    else if (InterData->getData() == NULL)
        ImageStatus = EIS_InvalidImage;
    else if (mode)
    {
        const unsigned long count = (unsigned long)Columns * (unsigned long)Rows * NumberOfFrames;
        if ((InterData->getCount() != count) && ((InterData->getCount() >> 1) != ((count + 1) >> 1)))
        {
            if (DicomImageClass::DebugLevel >= DicomImageClass::DL_Warnings)
            {
                cerr << "WARNING: computed (" << count << ") and stored (" << InterData->getCount() << ") ";
                cerr << "pixel count differ !" << endl;
            }
        }
    }
    return (ImageStatus == EIS_Normal);
}


void DiMonoImage::deleteOutputData()
{
    delete OutputData;
    OutputData = NULL;
}


void *DiMonoImage::getOutputPlane(const int) const
{
    if (OutputData != NULL)
        return (void *)OutputData->getData();               // monochrome images don't have several planes
    return NULL;
}


void DiMonoImage::deleteOverlayData()
{
    delete OverlayData;
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


int DiMonoImage::setNoVoiTransformation()
{
    int old = (VoiLutData != NULL) ? VoiLutData->isValid() : 2;
    delete VoiLutData;
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
            Document->getValue(DCM_WindowCenterWidthExplanation, VoiExplanation, pos);
            return setWindow(center, width);
        }
    }
    return 0;
}


int DiMonoImage::setWindow(const double center,
                           const double width,
                           const char *explanation)
{
    delete VoiLutData;
    VoiLutData = NULL;
    if (explanation != NULL)
        VoiExplanation = explanation;
    if (width <= 0)                                             // according to Cor Loef (author of suppl. 33)
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


int DiMonoImage::setVoiLut(const DcmUnsignedShort &data,
                           const DcmUnsignedShort &descriptor,
                           const DcmLongString *explanation)
{
    delete VoiLutData;
    VoiLutData = new DiLookupTable(data, descriptor, explanation);
    if (VoiLutData != NULL)
    {
        VoiExplanation = VoiLutData->getExplanation();
        return VoiLutData->isValid();
    }
    return 0;
}


int DiMonoImage::setVoiLut(const unsigned long pos)
{
    if (!(Document->getFlags() & CIF_UsePresentationState))
    {
        delete VoiLutData;
        VoiLutData = new DiLookupTable(Document, DCM_VOILUTSequence, DCM_LUTDescriptor, DCM_LUTData,
            DCM_LUTExplanation, pos, &VoiLutCount);
        if (VoiLutData != NULL)
        {
            VoiExplanation = VoiLutData->getExplanation();
            return VoiLutData->isValid();
        }
    }
    return 0;
}


int DiMonoImage::setPresentationLutShape(const ES_PresentationLut shape)
{
    delete PresLutData;
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
                                    const DcmLongString *explanation)
{
    delete PresLutData;
    PresLutData = new DiLookupTable(data, descriptor, explanation, 0);
    if (PresLutData != NULL)
        return PresLutData->isValid();
    return 0;
}


int DiMonoImage::addOverlay(const unsigned int group,
                            const signed int left,
                            const signed int top,
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
        return Overlays[1]->addPlane(group, left, top, columns, rows, data, label, description, mode);
    return 0;
}


int DiMonoImage::removeOverlay(const unsigned int group)
{
    if (Overlays[1] != NULL)
        return Overlays[1]->removePlane(group);
    return 0;
}


int DiMonoImage::flip(const int horz, const int vert)
{
    switch (InterData->getRepresentation())
    {
        case EPR_Uint8:
            DiFlipTemplate<Uint8>(InterData, Columns, Rows, NumberOfFrames, horz, vert);
            break;
        case EPR_Sint8:
            DiFlipTemplate<Sint8>(InterData, Columns, Rows, NumberOfFrames, horz, vert);
            break;
        case EPR_Uint16:
            DiFlipTemplate<Uint16>(InterData, Columns, Rows, NumberOfFrames, horz, vert);
            break;
        case EPR_Sint16:
            DiFlipTemplate<Sint16>(InterData, Columns, Rows, NumberOfFrames, horz, vert);
            break;
        case EPR_Uint32:
            DiFlipTemplate<Uint32>(InterData, Columns, Rows, NumberOfFrames, horz, vert);
            break;
        case EPR_Sint32:
            DiFlipTemplate<Sint32>(InterData, Columns, Rows, NumberOfFrames, horz, vert);
            break;
    }
    for (int i = 0; i < 2; i++)
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
                DiRotateTemplate<Uint8>(InterData, old_cols, old_rows, Columns, Rows, NumberOfFrames, degree);
                break;
            case EPR_Sint8:
                DiRotateTemplate<Sint8>(InterData, old_cols, old_rows, Columns, Rows, NumberOfFrames, degree);
                break;
            case EPR_Uint16:
                DiRotateTemplate<Uint16>(InterData, old_cols, old_rows, Columns, Rows, NumberOfFrames, degree);
                break;
            case EPR_Sint16:
                DiRotateTemplate<Sint16>(InterData, old_cols, old_rows, Columns, Rows, NumberOfFrames, degree);
                break;
            case EPR_Uint32:
                DiRotateTemplate<Uint32>(InterData, old_cols, old_rows, Columns, Rows, NumberOfFrames, degree);
                break;
            case EPR_Sint32:
                DiRotateTemplate<Sint32>(InterData, old_cols, old_rows, Columns, Rows, NumberOfFrames, degree);
                break;
        }
    }
    for (int i = 0; i < 2; i++)
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

void *DiMonoImage::getData(const unsigned long frame,
                           const int bits,
                           const int negative)
{
    if ((InterData != NULL) && (ImageStatus == EIS_Normal) && (frame < NumberOfFrames) && (bits > 0) && (bits <= MAX_BITS))
    {
        deleteOutputData();                             // delete old image data
        if (!ValidWindow)
            WindowWidth = -1;                           // negative width means no window, saves additional parameter ;)
        Uint32 low;
        Uint32 high;
        if ((negative && (PresLutShape == ESP_Identity)) || (!negative && (PresLutShape == ESP_Inverse)))
        {
            low = maxval(bits);                         // inverse/negative: white to black
            high = 0;
        } else {
            low = 0;                                    // normal/positive: black to white
            high = maxval(bits);
        }
        switch (InterData->getRepresentation())
        {
            case EPR_Uint8:
                if (InterData->isPotentiallySigned())
                {
                    if (bits <= 8)
                        OutputData = new DiMonoOutputPixelTemplate<Uint8, Sint32, Uint8>(InterData, Overlays, VoiLutData,
                           PresLutData, WindowCenter, WindowWidth, low, high, Columns, Rows, frame, NumberOfFrames);
                    else if (bits <= 16)
                        OutputData = new DiMonoOutputPixelTemplate<Uint8, Sint32, Uint16>(InterData, Overlays, VoiLutData,
                            PresLutData, WindowCenter, WindowWidth, low, high, Columns, Rows, frame, NumberOfFrames);
                    else
                        OutputData = new DiMonoOutputPixelTemplate<Uint8, Sint32, Uint32>(InterData, Overlays, VoiLutData,
                            PresLutData, WindowCenter, WindowWidth, low, high, Columns, Rows, frame, NumberOfFrames);
                } else {
                    if (bits <= 8)
                        OutputData = new DiMonoOutputPixelTemplate<Uint8, Uint32, Uint8>(InterData, Overlays, VoiLutData,
                           PresLutData, WindowCenter, WindowWidth, low, high, Columns, Rows, frame, NumberOfFrames);
                    else if (bits <= 16)
                        OutputData = new DiMonoOutputPixelTemplate<Uint8, Uint32, Uint16>(InterData, Overlays, VoiLutData,
                            PresLutData, WindowCenter, WindowWidth, low, high, Columns, Rows, frame, NumberOfFrames);
                    else
                        OutputData = new DiMonoOutputPixelTemplate<Uint8, Uint32, Uint32>(InterData, Overlays, VoiLutData,
                            PresLutData, WindowCenter, WindowWidth, low, high, Columns, Rows, frame, NumberOfFrames);
                }
                break;
            case EPR_Sint8:
                if (bits <= 8)
                    OutputData = new DiMonoOutputPixelTemplate<Sint8, Sint32, Uint8>(InterData, Overlays, VoiLutData,
                        PresLutData, WindowCenter, WindowWidth, low, high, Columns, Rows, frame, NumberOfFrames);
                else if (bits <= 16)
                    OutputData = new DiMonoOutputPixelTemplate<Sint8, Sint32, Uint16>(InterData, Overlays, VoiLutData,
                        PresLutData, WindowCenter, WindowWidth, low, high, Columns, Rows, frame, NumberOfFrames);
                else
                    OutputData = new DiMonoOutputPixelTemplate<Sint8, Sint32, Uint32>(InterData, Overlays, VoiLutData,
                        PresLutData, WindowCenter, WindowWidth, low, high, Columns, Rows, frame, NumberOfFrames);
                break;
            case EPR_Uint16:
                if (InterData->isPotentiallySigned())
                {
                    if (bits <= 8)
                        OutputData = new DiMonoOutputPixelTemplate<Uint16, Sint32, Uint8>(InterData, Overlays, VoiLutData,
                            PresLutData, WindowCenter, WindowWidth, low, high, Columns, Rows, frame, NumberOfFrames);
                    else if (bits <= 16)
                        OutputData = new DiMonoOutputPixelTemplate<Uint16, Sint32, Uint16>(InterData, Overlays, VoiLutData,
                            PresLutData, WindowCenter, WindowWidth, low, high, Columns, Rows, frame, NumberOfFrames);
                    else
                        OutputData = new DiMonoOutputPixelTemplate<Uint16, Sint32, Uint32>(InterData, Overlays, VoiLutData,
                            PresLutData, WindowCenter, WindowWidth, low, high, Columns, Rows, frame, NumberOfFrames);
                } else {
                    if (bits <= 8)
                        OutputData = new DiMonoOutputPixelTemplate<Uint16, Uint32, Uint8>(InterData, Overlays, VoiLutData,
                            PresLutData, WindowCenter, WindowWidth, low, high, Columns, Rows, frame, NumberOfFrames);
                    else if (bits <= 16)
                        OutputData = new DiMonoOutputPixelTemplate<Uint16, Uint32, Uint16>(InterData, Overlays, VoiLutData,
                            PresLutData, WindowCenter, WindowWidth, low, high, Columns, Rows, frame, NumberOfFrames);
                    else
                        OutputData = new DiMonoOutputPixelTemplate<Uint16, Uint32, Uint32>(InterData, Overlays, VoiLutData,
                            PresLutData, WindowCenter, WindowWidth, low, high, Columns, Rows, frame, NumberOfFrames);
                }
                break;
            case EPR_Sint16:
                if (bits <= 8)
                    OutputData = new DiMonoOutputPixelTemplate<Sint16, Sint32, Uint8>(InterData, Overlays, VoiLutData,
                        PresLutData, WindowCenter, WindowWidth, low, high, Columns, Rows, frame, NumberOfFrames);
                else if (bits <= 16)
                    OutputData = new DiMonoOutputPixelTemplate<Sint16, Sint32, Uint16>(InterData, Overlays, VoiLutData,
                        PresLutData, WindowCenter, WindowWidth, low, high, Columns, Rows, frame, NumberOfFrames);
                else
                    OutputData = new DiMonoOutputPixelTemplate<Sint16, Sint32, Uint32>(InterData, Overlays, VoiLutData,
                        PresLutData, WindowCenter, WindowWidth, low, high, Columns, Rows, frame, NumberOfFrames);
                break;
            case EPR_Uint32:
                if (InterData->isPotentiallySigned())
                {
                    if (bits <= 8)
                        OutputData = new DiMonoOutputPixelTemplate<Uint32, Sint32, Uint8>(InterData, Overlays, VoiLutData,
                            PresLutData, WindowCenter, WindowWidth, low, high, Columns, Rows, frame, NumberOfFrames);
                    else if (bits <= 16)
                        OutputData = new DiMonoOutputPixelTemplate<Uint32, Sint32, Uint16>(InterData, Overlays, VoiLutData,
                            PresLutData, WindowCenter, WindowWidth, low, high, Columns, Rows, frame, NumberOfFrames);
                    else
                        OutputData = new DiMonoOutputPixelTemplate<Uint32, Sint32, Uint32>(InterData, Overlays, VoiLutData,
                            PresLutData, WindowCenter, WindowWidth, low, high, Columns, Rows, frame, NumberOfFrames);
                } else {
                    if (bits <= 8)
                        OutputData = new DiMonoOutputPixelTemplate<Uint32, Uint32, Uint8>(InterData, Overlays, VoiLutData,
                            PresLutData, WindowCenter, WindowWidth, low, high, Columns, Rows, frame, NumberOfFrames);
                    else if (bits <= 16)
                        OutputData = new DiMonoOutputPixelTemplate<Uint32, Uint32, Uint16>(InterData, Overlays, VoiLutData,
                            PresLutData, WindowCenter, WindowWidth, low, high, Columns, Rows, frame, NumberOfFrames);
                    else
                        OutputData = new DiMonoOutputPixelTemplate<Uint32, Uint32, Uint32>(InterData, Overlays, VoiLutData,
                            PresLutData, WindowCenter, WindowWidth, low, high, Columns, Rows, frame, NumberOfFrames);
                }
                break;
            case EPR_Sint32:
                if (bits <= 8)
                    OutputData = new DiMonoOutputPixelTemplate<Sint32, Sint32, Uint8>(InterData, Overlays, VoiLutData,
                        PresLutData, WindowCenter, WindowWidth, low, high, Columns, Rows, frame, NumberOfFrames);
                else if (bits <= 16)
                    OutputData = new DiMonoOutputPixelTemplate<Sint32, Sint32, Uint16>(InterData, Overlays, VoiLutData,
                        PresLutData, WindowCenter, WindowWidth, low, high, Columns, Rows, frame, NumberOfFrames);
                else
                    OutputData = new DiMonoOutputPixelTemplate<Sint32, Sint32, Uint32>(InterData, Overlays, VoiLutData,
                        PresLutData, WindowCenter, WindowWidth, low, high, Columns, Rows, frame, NumberOfFrames);
                break;
        }
        if (OutputData == NULL)
        {
            ImageStatus = EIS_MemoryFailure;
            if (DicomImageClass::DebugLevel >= DicomImageClass::DL_Errors)
                cerr << "ERROR: can't allocate memory for output-representation !" << endl;
        }
        else
            return OutputData->getData();           // points to beginning of output data
    }
    return NULL;
}


/*
 *   create 8-bit (bi-level) bitmap with overlay 'plane' data
 */

const Uint8 *DiMonoImage::getOverlayData(const unsigned long frame,
                                         const unsigned int plane,
                                         unsigned int &left,
                                         unsigned int &top,
                                         unsigned int &width,
                                         unsigned int &height,
                                         EM_Overlay &mode,
                                         const unsigned int idx,
                                         const Uint8 value)
{
    if (ImageStatus == EIS_Normal)
    {
        int start = 1;                                              // default: additional overlay planes hide dataset planes
        int end = 0;
        if (idx < 2)                                                // specified index of overlay group
            start = end = idx;
        for (int i = start; i >= end; i--)                          // start searching with additional overlay planes
        {
            if ((Overlays[i] != NULL) && (Overlays[i]->hasPlane(plane)))
            {
                deleteOverlayData();
                OverlayData = Overlays[i]->getPlaneData(frame, plane, left, top, width, height, mode, Columns, Rows, value);
                return (const Uint8 *)OverlayData;
            }
        }
    }
    return NULL;
}


/*
 *   create 24-bit true color device independent bitmap (DIB) as needed by MS-Windows
 */

void *DiMonoImage::createDIB(const unsigned long frame)
{
    getOutputData(frame, 8);                        // create output data with 8 bit depth
    Uint8 *data = NULL;
    if ((OutputData != NULL) && (OutputData->getData() != NULL))
    {
        const int gap = Columns & 0x3;              // each line has to start at 32-bit-address!
        data = new Uint8[(unsigned long)(Columns + gap) * (unsigned long)Rows * 3];
        if (data != NULL)
        {
            register const Uint8 *p = (const Uint8 *)(OutputData->getData());
            register Uint8 *q = data;
            register Uint8 value;
            register Uint16 x;
            register Uint16 y;
            register int j;
            for (y = 0; y < Rows; y++)
            {
                for (x = 0; x < Columns; x++)
                {
                    value = *(p++);                 // store gray value
                    for (j = 0; j < 3; j++)
                        *(q++) = value;             // copy to the three RGB-planes
                }
                q += gap;
            }
        }
    }
    deleteOutputData();                             // output data is no longer needed
    return (void *)data;
}


/*
 *   create 8-bit palette/monochrome or 24/32-bit true color bitmap as needed for Java/AWT
 */

void *DiMonoImage::createAWTBitmap(const unsigned long frame, const int bits)
{
    if (bits == 8)                                      // for idx color model (byte)
    {
        getOutputData(frame, 8);                        // create output data with 8 bit depth
        void *data = NULL;
        if ((OutputData != NULL) && (OutputData->getData() != NULL))
        {
            data = OutputData->getData();
            OutputData = NULL;                          // remove reference to internal memory
        }
        return data;
    }
    else if (bits == 32)                                // for direct color model (long int)
    {
        getOutputData(frame, 8);                        // create output data with 8 bit depth
        Uint32 *data = NULL;
        if ((OutputData != NULL) && (OutputData->getData() != NULL))
        {
            data = new Uint32[(unsigned long)Columns * (unsigned long)Rows];
            if (data != NULL)
            {
                register const Uint8 *p = (const Uint8 *)(OutputData->getData());
                register Uint32 *q = data;
                register Uint32 value;
                register Uint16 x;
                register Uint16 y;
                for (y = 0; y < Rows; y++)
                {
                    for (x = 0; x < Columns; x++)
                    {
                        value = *(p++);                 // store gray value
                        *(q++) = (value << 24) | (value << 16) | (value << 8);
                                                        // copy to the three RGB-planes
                    }
                }
            }
        }
        deleteOutputData();                             // output data is no longer needed
        return (void *)data;
    }
    return NULL;
}


/*********************************************************************/


/*
 *   write output data of 'frame' with depth of 'bits' to C++-output 'stream' (format is PGM - portable gray map)
 */

int DiMonoImage::writePPM(ostream &stream, const unsigned long frame, const int bits)
{
    getOutputData(frame, bits);
    if (OutputData != NULL)
    {
        stream << "P2" << endl;
        stream << Columns << " " << Rows << endl;
        stream << maxval(bits) << endl;
        int ok = OutputData->writePPM(stream);
        deleteOutputData();
        return ok;
    }
    return 0;
}


/*
 *   write output data of 'frame' with depth of 'bits' to C-file 'stream' (format is PGM - portable gray map)
 */

int DiMonoImage::writePPM(FILE *stream, const unsigned long frame, const int bits)
{
    if (stream != NULL)
    {
        getOutputData(frame, bits);
        if (OutputData != NULL)
        {
            fprintf(stream, "P2\n%u %u\n%lu\n", Columns, Rows, maxval(bits));
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

int DiMonoImage::writeRawPPM(FILE *stream, const unsigned long frame, const int bits)
{
    if (stream != NULL)
    {
        getOutputData(frame, bits);
        if ((OutputData != NULL) && (OutputData->getData() != NULL))
        {
            fprintf(stream, "P5\n%u %u\n%lu\n", Columns, Rows, maxval(bits));
            fwrite(OutputData->getData(), (size_t)OutputData->getCount(), OutputData->getItemSize(), stream);
            deleteOutputData();
            return 1;
        }
    }
    return 0;
}


/*
 *
 * CVS/RCS Log:
 * $Log: dimoimg.cc,v $
 * Revision 1.8  1999-01-11 09:37:28  joergr
 * Added parameter to method 'getMinMaxValues()' to return absolute minimum
 * and maximum values ('possible') in addition to actually 'used' pixel
 * values.
 *
 * Revision 1.7  1998/12/23 13:22:22  joergr
 * Changed parameter type (long to int) to avoid warning reported by MSVC5.
 *
 * Revision 1.3  1998/12/16 16:15:55  joergr
 * Added explanation string for VOI transformations.
 * Added method to export overlay planes (create 8-bit bitmap).
 * Renamed 'setNoVoiLutTransformation' method ('Voi' instead of 'VOI').
 *
 * Revision 1.2  1998/12/14 17:37:15  joergr
 * Added methods to add and remove additional overlay planes (still untested).
 * Added support for presentation shapes.
 *
 * Revision 1.1  1998/11/27 16:12:48  joergr
 * Added copyright message.
 * Added methods and constructors for flipping and rotating, changed for
 * scaling and clipping.
 * Added method to directly create java AWT bitmaps.
 * Introduced global debug level for dcmimage module to control error output.
 * Renamed variable 'Status' to 'ImageStatus' because of possible conflicts
 * with X windows systems.
 * Added constructors to use external modality transformations.
 * Added methods to support presentation LUTs and shapes.
 * Changed behaviour: now window width of 0 is valid and negative width
 * is invalid.
 *
 * Revision 1.6  1998/05/11 14:52:32  joergr
 * Added CVS/RCS header to each file.
 *
 *
 */
