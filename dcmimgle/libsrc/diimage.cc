/*
 *
 *  Copyright (C) 1996-2000, OFFIS
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
 *  Purpose: DicomImage (Source)
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2000-05-25 10:35:02 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmimgle/libsrc/diimage.cc,v $
 *  CVS/RCS Revision: $Revision: 1.12 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#include "osconfig.h"
#include "dctypes.h"
#include "dcdeftag.h"

#include "diimage.h"
#include "diinpxt.h"
#include "didocu.h"
#include "diutils.h"


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
    RepresentativeFrame(0),
    Rows(0),
    Columns(0),
    PixelWidth(1),
    PixelHeight(1),
    BitsAllocated(0),
    BitsStored(0),
    HighBit(0),
    BitsPerSample(0),
    hasSignedRepresentation(0),
    hasPixelSpacing(0),
    hasImagerPixelSpacing(0),
    hasPixelAspectRatio(0),
    isOriginal(1),
    InputData(NULL)
{
    if ((Document != NULL) && (ImageStatus == EIS_Normal))
    {
        Sint32 sl = 0;
        if (Document->getValue(DCM_NumberOfFrames, sl))
        {
            if (sl < 1)
            {
                if (DicomImageClass::checkDebugLevel(DicomImageClass::DL_Warnings))
                {
                    ofConsole.lockCerr() << "WARNING: invalid value for 'NumberOfFrames' (" << sl << ") "
                                         << "... assuming 1 !" << endl;
                    ofConsole.unlockCerr();
                }
                NumberOfFrames = 1;
            }
            else
                NumberOfFrames = (Uint32)sl;
        }
        else
            NumberOfFrames = 1;
        FirstFrame = (docu->getFrameStart() < NumberOfFrames) ? docu->getFrameStart() : NumberOfFrames - 1;
        NumberOfFrames -= FirstFrame;
        if ((docu->getFrameCount() > 0) && (NumberOfFrames > docu->getFrameCount()))
            NumberOfFrames = docu->getFrameCount();
        Uint16 us = 0;
        if (Document->getValue(DCM_RepresentativeFrameNumber, us))
        {
            if (us <= FirstFrame)
            {
                if (DicomImageClass::checkDebugLevel(DicomImageClass::DL_Warnings))
                {
                    ofConsole.lockCerr() << "WARNING: invalid value for 'RepresentativeFrameNumber' (" << sl << ")" << endl
                                         << "         ... assuming first frame !" << endl;
                    ofConsole.unlockCerr();
                }
                RepresentativeFrame = FirstFrame;
            }
            else if (us > NumberOfFrames)
            {
                if (DicomImageClass::checkDebugLevel(DicomImageClass::DL_Warnings))
                {
                    ofConsole.lockCerr() << "WARNING: invalid value for 'RepresentativeFrameNumber' (" << sl << ")" << endl
                                         << "         ... assuming last frame !" << endl;
                    ofConsole.unlockCerr();
                }
                RepresentativeFrame = NumberOfFrames - 1;
            }
            else
                RepresentativeFrame = us - 1;
        }              
        int ok = (Document->getValue(DCM_Rows, Rows) > 0);
        ok &= (Document->getValue(DCM_Columns, Columns) > 0);
        if (!ok || ((Rows > 0) && (Columns > 0)))
        {            
            ok &= (Document->getValue(DCM_BitsAllocated, BitsAllocated) > 0);
            ok &= (Document->getValue(DCM_BitsStored, BitsStored) > 0);
            ok &= (Document->getValue(DCM_HighBit, HighBit) > 0);
            ok &= (Document->getValue(DCM_PixelRepresentation, us) > 0);
            BitsPerSample = BitsStored;
            hasSignedRepresentation = (us == 1);
            if ((us != 0) && (us != 1))
            {
                if (DicomImageClass::checkDebugLevel(DicomImageClass::DL_Warnings))
                {
                    ofConsole.lockCerr() << "WARNING: invalid value for 'PixelRepresentation' (" << us << ") "
                                         << "... assuming 'unsigned' (0) !" << endl;
                    ofConsole.unlockCerr();
                }
            }
            if (!(Document->getFlags() & CIF_UsePresentationState))
            {
                hasPixelSpacing = (Document->getValue(DCM_PixelSpacing, PixelHeight, 0) > 0);
                if (hasPixelSpacing)
                {
                    if (Document->getValue(DCM_PixelSpacing, PixelWidth, 1) < 2)
                    {
                        if (DicomImageClass::checkDebugLevel(DicomImageClass::DL_Warnings))
                        {
                            ofConsole.lockCerr() << "WARNING: missing second value for 'PixelSpacing' ... "
                                                 << "assuming 'Width' = " << PixelWidth << " !" << endl;
                            ofConsole.unlockCerr();
                        }
                    }
                } else {
                    hasImagerPixelSpacing = (Document->getValue(DCM_ImagerPixelSpacing, PixelHeight, 0) > 0);
                    if (hasImagerPixelSpacing)
                    {
                        if (Document->getValue(DCM_ImagerPixelSpacing, PixelWidth, 1) < 2)
                        {
                            if (DicomImageClass::checkDebugLevel(DicomImageClass::DL_Warnings))
                            {
                                ofConsole.lockCerr() << "WARNING: missing second value for 'ImagerPixelSpacing' ... "
                                                     << "assuming 'Width' = " << PixelWidth << " !" << endl;
                                ofConsole.unlockCerr();
                            }
                        }
                    } else {
                        Sint32 sl2;
                        hasPixelAspectRatio = (Document->getValue(DCM_PixelAspectRatio, sl2, 0) > 0);
                        PixelHeight = sl2;
                        if (hasPixelAspectRatio)
                        {
                            if (Document->getValue(DCM_PixelAspectRatio, sl2, 1) < 2)
                            {
                                if (DicomImageClass::checkDebugLevel(DicomImageClass::DL_Warnings))
                                {
                                    ofConsole.lockCerr() << "WARNING: missing second value for 'PixelAspectRatio' ... "
                                                         << "assuming 'Width' = " << PixelWidth << " !" << endl;
                                    ofConsole.unlockCerr();
                                }
                            } else
                                PixelWidth = sl2;
                        } else {
                            PixelWidth = 1;
                            PixelHeight = 1;
                        }
                    }
                }
                checkPixelExtension();
            }
            DcmStack pstack;
            if (ok && Document->search(DCM_PixelData, pstack))
            {
                DcmPixelData *pixel = (DcmPixelData *)pstack.top();
                if ((pixel != NULL) && (pixel->chooseRepresentation(EXS_LittleEndianExplicit, NULL, pstack) == EC_Normal))
                    convertPixelData(pixel, spp);
                else
                {
                    ImageStatus = EIS_NotSupportedValue;
                    if (DicomImageClass::checkDebugLevel(DicomImageClass::DL_Errors))
                    {
                        ofConsole.lockCerr() << "ERROR: cannot change to unencapsulated representation for pixel data !" << endl;
                        ofConsole.lockCerr();
                    }
                }
            }
            else
            {
                ImageStatus = EIS_MissingAttribute;
                if (DicomImageClass::checkDebugLevel(DicomImageClass::DL_Errors))
                {
                    ofConsole.lockCerr() << "ERROR: one or more mandatory attributes are missing in image pixel module !" << endl;
                    ofConsole.unlockCerr();
                }
            }
        }
        else
        {
            ImageStatus = EIS_InvalidValue;
            if (DicomImageClass::checkDebugLevel(DicomImageClass::DL_Errors))
            {
                ofConsole.lockCerr() << "ERROR: invalid value for 'Rows' (" << Rows << ") and/or 'Columns' (" << Columns << ") !" << endl;
                ofConsole.unlockCerr();
            }
        }
    }
    else
    {
        ImageStatus = EIS_InvalidDocument;
        if (DicomImageClass::checkDebugLevel(DicomImageClass::DL_Errors))
        {
            ofConsole.lockCerr() << "ERROR: this DICOM document is invalid !" << endl;
            ofConsole.unlockCerr();
        }
    }
}


DiImage::DiImage(const DiDocument *docu,
                 const EI_Status status)
  : ImageStatus(status),
    Document(docu),
    FirstFrame(0),
    NumberOfFrames(0),
    RepresentativeFrame(0),
    Rows(0),
    Columns(0),
    PixelWidth(1),
    PixelHeight(1),
    BitsAllocated(0),
    BitsStored(0),
    HighBit(0),
    BitsPerSample(0),
    hasSignedRepresentation(0),
    hasPixelSpacing(0),
    hasImagerPixelSpacing(0),
    hasPixelAspectRatio(0),
    isOriginal(1),
    InputData(NULL)
{
}


DiImage::DiImage(const DiImage *image,
                 const unsigned long fstart,
                 const unsigned long fcount)
  : ImageStatus(image->ImageStatus),
    Document(image->Document),
    FirstFrame(image->FirstFrame + fstart),
    NumberOfFrames(fcount),
    RepresentativeFrame(image->RepresentativeFrame),
    Rows(image->Rows), 
    Columns(image->Columns),
    PixelWidth(image->PixelWidth),
    PixelHeight(image->PixelHeight),
    BitsAllocated(image->BitsAllocated),
    BitsStored(image->BitsStored),
    HighBit(image->HighBit),
    BitsPerSample(image->BitsPerSample),
    hasSignedRepresentation(image->hasSignedRepresentation),
    hasPixelSpacing(image->hasPixelSpacing),
    hasImagerPixelSpacing(image->hasImagerPixelSpacing),
    hasPixelAspectRatio(image->hasPixelAspectRatio),
    isOriginal(0),
    InputData(NULL)
{
}


DiImage::DiImage(const DiImage *image,
                 const Uint16 columns,
                 const Uint16 rows,
                 const int aspect)
  : ImageStatus(image->ImageStatus),
    Document(image->Document),
    FirstFrame(image->FirstFrame),
    NumberOfFrames(image->NumberOfFrames),
    RepresentativeFrame(image->RepresentativeFrame),
    Rows(rows), 
    Columns(columns),
    PixelWidth((aspect) ? 1 : image->PixelWidth),
    PixelHeight((aspect) ? 1 : image->PixelHeight),
    BitsAllocated(image->BitsAllocated),
    BitsStored(image->BitsStored),
    HighBit(image->HighBit),
    BitsPerSample(image->BitsPerSample),
    hasSignedRepresentation(image->hasSignedRepresentation),
    hasPixelSpacing(image->hasPixelSpacing),
    hasImagerPixelSpacing(image->hasImagerPixelSpacing),
    hasPixelAspectRatio((aspect) ? 0 : image->hasPixelAspectRatio),
    isOriginal(0),
    InputData(NULL)
{
}


DiImage::DiImage(const DiImage *image,
                 const int degree)
  : ImageStatus(image->ImageStatus),
    Document(image->Document),
    FirstFrame(image->FirstFrame),
    NumberOfFrames(image->NumberOfFrames),
    RepresentativeFrame(image->RepresentativeFrame),
    Rows(((degree == 90) ||(degree == 270)) ? image->Columns : image->Rows),
    Columns(((degree == 90) ||(degree == 270)) ? image->Rows : image->Columns),
    PixelWidth(((degree == 90) ||(degree == 270)) ? image->PixelHeight : image->PixelWidth),
    PixelHeight(((degree == 90) ||(degree == 270)) ? image-> PixelWidth : image->PixelHeight),
    BitsAllocated(image->BitsAllocated),
    BitsStored(image->BitsStored),
    HighBit(image->HighBit),
    BitsPerSample(image->BitsPerSample),
    hasSignedRepresentation(image->hasSignedRepresentation),
    hasPixelSpacing(image->hasPixelSpacing),
    hasImagerPixelSpacing(image->hasImagerPixelSpacing),
    hasPixelAspectRatio(image->hasPixelAspectRatio),
    isOriginal(0),
    InputData(NULL)
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
    RepresentativeFrame(0),
    Rows(image->Rows),
    Columns(image->Columns),
    PixelWidth(image->PixelWidth),
    PixelHeight(image->PixelHeight),
    BitsAllocated(alloc),
    BitsStored(stored),
    HighBit(stored - 1),
    BitsPerSample(image->BitsPerSample),
    hasSignedRepresentation(0),
    hasPixelSpacing(image->hasPixelSpacing),
    hasImagerPixelSpacing(image->hasImagerPixelSpacing),
    hasPixelAspectRatio(image->hasPixelAspectRatio),
    isOriginal(0),
    InputData(NULL)
{
}


/*--------------*
 *  destructor  *
 *--------------*/

DiImage::~DiImage()
{
    delete InputData;
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
    if (hasPixelSpacing || hasImagerPixelSpacing || hasPixelAspectRatio)
    {
        if (PixelHeight == 0)
        {
            if (DicomImageClass::checkDebugLevel(DicomImageClass::DL_Warnings))
            {
                ofConsole.lockCerr() << "WARNING: invalid value for 'PixelHeight' (" << PixelHeight << ") "
                                     << "... assuming 1 !" << endl;
                ofConsole.unlockCerr();
            }
            PixelHeight = 1;
        }
        else if (PixelHeight < 0)
        {
            if (DicomImageClass::checkDebugLevel(DicomImageClass::DL_Warnings))
            {
                ofConsole.lockCerr() << "WARNING: negative value for 'PixelHeight' (" << PixelHeight << ") "
                                     << "... assuming " << -PixelHeight << " !" << endl;
                ofConsole.unlockCerr();
            }
            PixelHeight = -PixelHeight;
        }
        if (PixelWidth == 0)
        {
            if (DicomImageClass::checkDebugLevel(DicomImageClass::DL_Warnings))
            {
                ofConsole.lockCerr() << "WARNING: invalid value for 'PixelWidth' (" << PixelWidth << ") "
                                     << "... assuming 1 !" << endl;
                ofConsole.unlockCerr();
            }
            PixelWidth = 1;
        }
        else if (PixelWidth < 0)
        {
            if (DicomImageClass::checkDebugLevel(DicomImageClass::DL_Warnings))
            {
                ofConsole.lockCerr() << "WARNING: negative value for 'PixelWidth' (" << PixelWidth << ") "
                                     << "... assuming " << -PixelWidth << " !" << endl;
                ofConsole.unlockCerr();
            }
            PixelHeight = -PixelHeight;
        }
    }
}


void DiImage::convertPixelData(/*const*/ DcmPixelData *pixel,
                               const int /*spp*/)
{
    if ((pixel->getVR() == EVR_OW) || ((pixel->getVR() == EVR_OB) && (BitsAllocated <= 8)))
    {
        const unsigned long start = FirstFrame * (unsigned long)Rows * (unsigned long)Columns;
        const unsigned long count = NumberOfFrames * (unsigned long)Rows * (unsigned long)Columns;
        if ((BitsAllocated < 1) || (BitsStored < 1) || (BitsAllocated < BitsStored) || 
            (BitsStored > (Uint16)(HighBit + 1)))
        {
            ImageStatus = EIS_InvalidValue;
            if (DicomImageClass::checkDebugLevel(DicomImageClass::DL_Errors))
            {
                ofConsole.lockCerr() << "ERROR: invalid values for 'BitsAllocated' (" << BitsAllocated << "), "
                                     << "'BitsStored' (" << BitsStored << ") and/or 'HighBit' (" << HighBit << ") !" << endl;
                ofConsole.unlockCerr();
            }
            return;
        }
        else if ((pixel->getVR() == EVR_OB) && (BitsAllocated <= 8))
        {
            if (hasSignedRepresentation)
                InputData = new DiInputPixelTemplate<Uint8, Sint8>(pixel, BitsAllocated, BitsStored, HighBit, start, count);
            else
                InputData  = new DiInputPixelTemplate<Uint8, Uint8>(pixel, BitsAllocated, BitsStored, HighBit, start, count);
        }
        else if (BitsStored <= bitsof(Uint8)) 
        {
            if (hasSignedRepresentation)
                InputData = new DiInputPixelTemplate<Uint16, Sint8>(pixel, BitsAllocated, BitsStored, HighBit, start, count);
            else
                InputData  = new DiInputPixelTemplate<Uint16, Uint8>(pixel, BitsAllocated, BitsStored, HighBit, start, count);
        }
        else if (BitsStored <= bitsof(Uint16))
        {
            if (hasSignedRepresentation)
                InputData = new DiInputPixelTemplate<Uint16, Sint16>(pixel, BitsAllocated, BitsStored, HighBit, start, count);
            else
                InputData = new DiInputPixelTemplate<Uint16, Uint16>(pixel, BitsAllocated, BitsStored, HighBit, start, count);
        }
        else if (BitsStored <= bitsof(Uint32))
        {
            if (hasSignedRepresentation)
                InputData = new DiInputPixelTemplate<Uint16, Sint32>(pixel, BitsAllocated, BitsStored, HighBit, start, count);
            else
                InputData = new DiInputPixelTemplate<Uint16, Uint32>(pixel, BitsAllocated, BitsStored, HighBit, start, count);
        }
        else    /* BitsStored > 32 !! */
        {
            ImageStatus = EIS_NotSupportedValue;
            if (DicomImageClass::checkDebugLevel(DicomImageClass::DL_Errors))
            {
                ofConsole.lockCerr() << "ERROR: invalid value for 'BitsStored' (" << BitsStored << ") "
                                     << "... exceeds " << MAX_BITS << " bit !" << endl;
                ofConsole.unlockCerr();
            }
            return;
        }
        if (InputData == NULL)
        {
            ImageStatus = EIS_MemoryFailure;
            if (DicomImageClass::checkDebugLevel(DicomImageClass::DL_Errors))
            {
                ofConsole.lockCerr() << "ERROR: can't allocate memory for input-representation !" << endl;
                ofConsole.unlockCerr();
            }
        }
    }
    else
    {
        ImageStatus = EIS_NotSupportedValue;
        if (DicomImageClass::checkDebugLevel(DicomImageClass::DL_Errors))
        {
            ofConsole.lockCerr() << "ERROR: 'PixelData' has an other value representation than OB "
                                 << "(with 'BitsAllocated' <= 8) or OW !" << endl;
            ofConsole.unlockCerr();
        }
    }
}


int DiImage::detachPixelData()
{
    DcmStack pstack;    
    if ((Document != NULL) && (Document->getFlags() & CIF_MayDetachPixelData) && Document->search(DCM_PixelData, pstack))
    {
        DcmPixelData *pixel = (DcmPixelData *)pstack.top();
        if (pixel != NULL)
        {
#ifdef DEBUG
            if (DicomImageClass::checkDebugLevel(DicomImageClass::DL_Informationals))
            {
                ofConsole.lockCerr() << "INFO: detach pixel data" << endl;
                ofConsole.unlockCerr();
            }
#endif
            pixel->putUint16Array(NULL, 0);                       // remove pixel data from memory/dataset
            return 1;
        }
    }
    return 0;        
}


int DiImage::setColumnRowRatio(const double ratio)
{
    hasPixelAspectRatio = 1;
    PixelWidth = ratio;
    PixelHeight = 1;
    checkPixelExtension();
    return 1;
}


int DiImage::setRowColumnRatio(const double ratio)
{
    hasPixelAspectRatio = 1;
    PixelWidth = 1;
    PixelHeight = ratio;
    checkPixelExtension();
    return 1;
}


/*
 *
 * CVS/RCS Log:
 * $Log: diimage.cc,v $
 * Revision 1.12  2000-05-25 10:35:02  joergr
 * Removed ununsed variable from parameter list (avoid compiler warnings).
 *
 * Revision 1.11  2000/05/03 09:47:23  joergr
 * Removed most informational and some warning messages from release built
 * (#ifndef DEBUG).
 *
 * Revision 1.10  2000/04/28 12:33:44  joergr
 * DebugLevel - global for the module - now derived from OFGlobal (MF-safe).
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
