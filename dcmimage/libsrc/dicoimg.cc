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
 *  Module:  dcmimage
 *
 *  Author:  Joerg Riesmeier
 *
 *  Purpose: DicomColorImage (Source)
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 1998-11-27 14:27:25 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmimage/libsrc/dicoimg.cc,v $
 *  CVS/RCS Revision: $Revision: 1.4 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#include "osconfig.h"
#include "dctypes.h"

#include "dicoimg.h"
#include "dimo2img.h"
#include "dicopxt.h"
#include "dicocpt.h"
#include "dicosct.h"
#include "dicoflt.h"
#include "dicorot.h"
#include "dicoopxt.h"
#include "diutils.h"


/*----------------*
 *  constructors  *
 *----------------*/

DiColorImage::DiColorImage(const DiDocument *docu,
                           const EI_Status status,
                           const int spp)
  : DiImage(docu, status, spp),
    InterData(NULL),
    OutputData(NULL)
{
    detachPixelData();
} 


DiColorImage::DiColorImage(const DiColorImage *image,
                           const unsigned long fstart,
                           const unsigned long fcount)
  : DiImage(image, fstart, fcount),
    InterData(NULL),
    OutputData(NULL)
{
    if (image->InterData != NULL)
    {
        const unsigned long fsize = (unsigned long)Columns * (unsigned long)Rows;
        switch (image->InterData->getRepresentation())
        {
            case EPR_Uint8:
                InterData = new DiColorCopyTemplate<Uint8>(image->InterData, fstart, fcount, fsize);
                break;
            case EPR_Sint8:
                InterData = new DiColorCopyTemplate<Sint8>(image->InterData, fstart, fcount, fsize);
                break;
            case EPR_Uint16:
                InterData = new DiColorCopyTemplate<Uint16>(image->InterData, fstart, fcount, fsize);
                break;
            case EPR_Sint16:
                InterData = new DiColorCopyTemplate<Sint16>(image->InterData, fstart, fcount, fsize);
                break;
            case EPR_Uint32:
                InterData = new DiColorCopyTemplate<Uint32>(image->InterData, fstart, fcount, fsize);
                break;
            case EPR_Sint32:
                InterData = new DiColorCopyTemplate<Sint32>(image->InterData, fstart, fcount, fsize);
                break;
        }
        checkInterData(0);
    }
}


DiColorImage::DiColorImage(const DiColorImage *image,
                           const Uint16 left,
                           const Uint16 top,
                           const Uint16 src_cols,
                           const Uint16 src_rows,                 
                           const Uint16 dest_cols,
                           const Uint16 dest_rows,
                           const int interpolate,
                           const int aspect)
  : DiImage(image, dest_cols, dest_rows, aspect),
    InterData(NULL),
    OutputData(NULL)
{
    if (image->InterData != NULL)
    {
        switch (image->InterData->getRepresentation())
        {
            case EPR_Uint8:
                InterData = new DiColorScaleTemplate<Uint8>(image->InterData, image->Columns, image->Rows, left, top,
                    src_cols, src_rows, dest_cols, dest_rows, NumberOfFrames, image->BitsPerSample, interpolate);
                break;
            case EPR_Uint16:
                InterData = new DiColorScaleTemplate<Uint16>(image->InterData, image->Columns, image->Rows, left, top,
                    src_cols, src_rows, dest_cols, dest_rows, NumberOfFrames, image->BitsPerSample, interpolate);
                break;
            case EPR_Uint32:
                InterData = new DiColorScaleTemplate<Uint32>(image->InterData, image->Columns, image->Rows, left, top,
                    src_cols, src_rows, dest_cols, dest_rows, NumberOfFrames, image->BitsPerSample, interpolate);
                break;
            default:
                if (DicomImageClass::DebugLevel >= DicomImageClass::DL_Warnings)
                    cerr << "WARNING: invalid value for inter-representation !" << endl;
        }
        checkInterData(0);
    }
}


DiColorImage::DiColorImage(const DiColorImage *image,
                           const int horz,
                           const int vert)
  : DiImage(image),
    InterData(NULL),
    OutputData(NULL)
{
    if (image->InterData != NULL)
    {
        switch (image->InterData->getRepresentation())
        {
            case EPR_Uint8:
                InterData = new DiColorFlipTemplate<Uint8>(image->InterData, Columns, Rows, NumberOfFrames, horz, vert);
                break;
            case EPR_Uint16:
                InterData = new DiColorFlipTemplate<Uint16>(image->InterData, Columns, Rows, NumberOfFrames, horz, vert);
                break;
            case EPR_Uint32:
                InterData = new DiColorFlipTemplate<Uint32>(image->InterData, Columns, Rows, NumberOfFrames, horz, vert);
                break;
            default:
                if (DicomImageClass::DebugLevel >= DicomImageClass::DL_Warnings)
                    cerr << "WARNING: invalid value for inter-representation !" << endl;
        }
        checkInterData(0);
    }
}


DiColorImage::DiColorImage(const DiColorImage *image,
                           const int degree)
  : DiImage(image, degree),
    InterData(NULL),
    OutputData(NULL)
{
    if (image->InterData != NULL)
    {
        switch (image->InterData->getRepresentation())
        {
            case EPR_Uint8:
                InterData = new DiColorRotateTemplate<Uint8>(image->InterData, image->Columns, image->Rows, Columns, Rows,
                    NumberOfFrames, degree);
                break;
            case EPR_Uint16:
                InterData = new DiColorRotateTemplate<Uint16>(image->InterData, image->Columns, image->Rows, Columns, Rows,
                    NumberOfFrames, degree);
                break;
            case EPR_Uint32:
                InterData = new DiColorRotateTemplate<Uint32>(image->InterData, image->Columns, image->Rows, Columns, Rows,
                    NumberOfFrames, degree);
                break;
            default:
                if (DicomImageClass::DebugLevel >= DicomImageClass::DL_Warnings)
                    cerr << "WARNING: invalid value for inter-representation !" << endl;
        }
        checkInterData(0);
    }
}


/*--------------*
 *  destructor  *
 *--------------*/

DiColorImage::~DiColorImage()
{
    delete InterData;
    delete OutputData;
}


/*********************************************************************/


int DiColorImage::checkInterData(const int mode)
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


void DiColorImage::deleteOutputData()
{
    delete OutputData;
    OutputData = NULL;
}


void *DiColorImage::getOutputData(const unsigned long frame,
                                  const int bits,
                                  const int planar)
{
    if ((InterData != NULL) && (ImageStatus == EIS_Normal) && (frame < NumberOfFrames) && (bits > 0) && (bits <= MAX_BITS))
    {
        deleteOutputData();                             // delete old image data
        Sint16 shift = getBits() - bits;
        switch (InterData->getRepresentation())
        {
            case EPR_Uint8:
                if (bits <= 8)
                    OutputData = new DiColorOutputPixelTemplate<Uint8, Uint8>(InterData, frame, NumberOfFrames,
                        shift, planar);
                else if (bits <= 16)
                    OutputData = new DiColorOutputPixelTemplate<Uint8, Uint16>(InterData, frame, NumberOfFrames,
                        shift, planar);
                else
                    OutputData = new DiColorOutputPixelTemplate<Uint8, Uint32>(InterData, frame, NumberOfFrames,
                        shift, planar);
                break;
            case EPR_Uint16:
                if (bits <= 8)
                    OutputData = new DiColorOutputPixelTemplate<Uint16, Uint8>(InterData, frame, NumberOfFrames,
                        shift, planar);
                else if (bits <= 16)
                    OutputData = new DiColorOutputPixelTemplate<Uint16, Uint16>(InterData, frame, NumberOfFrames,
                        shift, planar);
                else
                    OutputData = new DiColorOutputPixelTemplate<Uint16, Uint32>(InterData, frame, NumberOfFrames,
                        shift, planar);
                break;
            case EPR_Uint32:
                if (bits <= 8)
                    OutputData = new DiColorOutputPixelTemplate<Uint32, Uint8>(InterData, frame, NumberOfFrames,
                        shift, planar);
                else if (bits <= 16)
                    OutputData = new DiColorOutputPixelTemplate<Uint32, Uint16>(InterData, frame, NumberOfFrames,
                        shift, planar);
                else
                    OutputData = new DiColorOutputPixelTemplate<Uint32, Uint32>(InterData, frame, NumberOfFrames,
                        shift, planar);
                break;
            default:
                if (DicomImageClass::DebugLevel >= DicomImageClass::DL_Warnings)
                    cerr << "WARNING: invalid value for inter-representation !" << endl;
        }
        if (OutputData == NULL)
        {
            ImageStatus = EIS_MemoryFailure;
            if (DicomImageClass::DebugLevel >= DicomImageClass::DL_Errors)
                cerr << "ERROR: can't allocate memory for inter-representation !" << endl;
        }
        else
            return OutputData->getData();
    }
    return NULL;
}


void *DiColorImage::getOutputPlane(const int plane) const
{
    if (OutputData != NULL)
        return OutputData->getPlane(plane);
    return NULL;
}


DiImage *DiColorImage::createImage(const unsigned long fstart,
                                   const unsigned long fcount) const
{
    DiImage *image = new DiColorImage(this, fstart, fcount);
    return image;
}


DiImage *DiColorImage::createScale(const unsigned long left,
                                   const unsigned long top,
                                   const unsigned long src_cols,
                                   const unsigned long src_rows,                 
                                   const unsigned long dest_cols,
                                   const unsigned long dest_rows,
                                   const int interpolate,
                                   const int aspect) const
{
    DiImage *image = new DiColorImage(this, (Uint16)left, (Uint16)top, (Uint16)src_cols, (Uint16)src_rows,
        (Uint16)dest_cols, (Uint16)dest_rows, interpolate, aspect);
    return image;
}


int DiColorImage::flip(const int horz,
                       const int vert)
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
    return 1;
}


DiImage *DiColorImage::createFlip(const int horz,
                                  const int vert) const
{
    DiImage *image = new DiColorImage(this, horz, vert);
    return image;
}


int DiColorImage::rotate(const int degree)
{

    DiImage::rotate(degree);
    return 1;
}


DiImage *DiColorImage::createRotate(const int degree) const
{
    return NULL;
}


DiImage *DiColorImage::createMono(const double red,
                                  const double green,
                                  const double blue) const
{
    DiImage *image = new DiMono2Image(this, red, green, blue);
    return image;
}


void *DiColorImage::createDIB(const unsigned long frame)
{
    if (InterData != NULL)
        return InterData->createDIB(Columns, Rows, frame, (Sint16)getBits() - 8);
    return NULL;
}


void *DiColorImage::createAWTBitmap(const unsigned long frame,
                                    const int bits)
{
    if ((InterData != NULL) && (bits == 32))
        return InterData->createAWTBitmap(Columns, Rows, frame, (Sint16)getBits() - 8);
    return NULL;
}


int DiColorImage::writePPM(ostream &stream,
                           const unsigned long frame,
                           const int bits)
{
    getOutputData(frame, bits);
    if (OutputData != NULL)
    {
        stream << "P3" << endl;
        stream << Columns << " " << Rows << endl;
        stream << maxval(bits) << endl;
        int ok = OutputData->writePPM(stream);
        deleteOutputData();
        return ok;
    }
    return 0;
}


int DiColorImage::writePPM(FILE *stream,
                           const unsigned long frame,
                           const int bits)
{
    if (stream != NULL)
    {
        getOutputData(frame, bits);
        if (OutputData != NULL)
        {
            fprintf(stream, "P3\n%u %u\n%lu\n", Columns, Rows, maxval(bits));
            int ok = OutputData->writePPM(stream);
            deleteOutputData();
            return ok;
        }
    }
    return 0;
}


int DiColorImage::writeRawPPM(FILE *stream,
                              const unsigned long frame,
                              const int bits)
{
    if (stream != NULL)
    {
        getOutputData(frame, bits);
        if ((OutputData != NULL) && (OutputData->getData() != NULL))
        {
            fprintf(stream, "P6\n%u %u\n%lu\n", Columns, Rows, maxval(bits));
            fwrite(OutputData->getData(), (size_t)OutputData->getCount(), OutputData->getItemSize(), stream);
            return 1;
        }
    }
    return 0;
}


/*
**
** CVS/RCS Log:
** $Log: dicoimg.cc,v $
** Revision 1.4  1998-11-27 14:27:25  joergr
** Added copyright message.
** Added method to detach pixel data if it is no longer needed.
**
** Revision 1.3  1998/05/11 14:52:26  joergr
** Added CVS/RCS header to each file.
**
**
*/
