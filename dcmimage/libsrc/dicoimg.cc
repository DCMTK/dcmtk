/*
**
** Author:  Joerg Riesmeier
** Created: 20.12.96
**
** Module:  dicoimg.cc
**
** Purpose: DicomColorImage (Source)
**
** Last Update:      $Author: joergr $
** Update Date:      $Date: 1998-05-11 14:52:26 $
** Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmimage/libsrc/dicoimg.cc,v $
** CVS/RCS Revision: $Revision: 1.3 $
** Status:           $State: Exp $
**
** CVS/RCS Log at end of file
**
*/


#include "osconfig.h"
#include "dctypes.h"

#include "dicoimg.h"
#include "dimo2img.h"
#include "dicopxt.h"
#include "dicosct.h"
#include "dicoclt.h"
#include "dicoopxt.h"
#include "diutils.h"


/*----------------*
 *  constructors  *
 *----------------*/

DiColorImage::DiColorImage(const DiDocument *docu, const EI_Status status)
  : DiImage(docu, status),
    InterData(NULL),
    OutputData(NULL)
{
} 


DiColorImage::DiColorImage(const DiColorImage *image, const Uint16 columns, const Uint16 rows, const int interpolate)
  : DiImage(image, columns, rows),
    InterData(NULL),
    OutputData(NULL)
{
    if (image->InterData != NULL)
    {
        switch (image->InterData->getRepresentation())
        {
            case EPR_Uint8:
                InterData = new DiColorScaleTemplate<Uint8>(image->InterData, image->Columns, image->Rows, 
                    columns, rows, NumberOfFrames, image->BitsPerSample, interpolate);
                break;
            case EPR_Uint16:
                InterData = new DiColorScaleTemplate<Uint16>(image->InterData, image->Columns, image->Rows,
                    columns, rows, NumberOfFrames, image->BitsPerSample, interpolate);
                break;
            case EPR_Uint32:
                InterData = new DiColorScaleTemplate<Uint32>(image->InterData, image->Columns, image->Rows,
                    columns, rows, NumberOfFrames, image->BitsPerSample, interpolate);
                break;
            default:
                cerr << "WARNING: invalid value for inter-representation !" << endl;
        }
        checkInterData();
    }
}


DiColorImage::DiColorImage(const DiColorImage *image, const Uint16 left, const Uint16 top,
    const Uint16 columns, const Uint16 rows)
  : DiImage(image, columns, rows),
    InterData(NULL),
    OutputData(NULL)
{
    if (image->InterData != NULL)
    {
        switch (image->InterData->getRepresentation())
        {
            case EPR_Uint8:
                InterData = new DiColorClipTemplate<Uint8>(image->InterData, image->Columns, image->Rows,
                    left, top, columns, rows, NumberOfFrames);
                break;
            case EPR_Uint16:
                InterData = new DiColorClipTemplate<Uint16>(image->InterData, image->Columns, image->Rows,
                    left, top, columns, rows, NumberOfFrames);
                break;
            case EPR_Uint32:
                InterData = new DiColorClipTemplate<Uint32>(image->InterData, image->Columns, image->Rows,
                    left, top, columns, rows, NumberOfFrames);
                break;
            default:
                cerr << "WARNING: invalid value for inter-representation !" << endl;
        }
        checkInterData();
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


void DiColorImage::checkInterData()
{
    if (InterData == NULL)
    {
        Status = EIS_MemoryFailure;
        cerr << "ERROR: can't allocate memory for inter-representation !" << endl;
    }
    else if (InterData->getData() == NULL)
        Status = EIS_InvalidImage;
    else
    {
        const unsigned long count = (unsigned long)Columns * (unsigned long)Rows * NumberOfFrames;
        if ((InterData->getCount() != count) && ((InterData->getCount() >> 1) != ((count + 1) >> 1)))
        {
            cerr << "WARNING: computed (" << count << ") and stored (" << InterData->getCount() << ") ";
            cerr << "pixel count differ !" << endl;
        }
    }
}


void DiColorImage::deleteOutputData()
{
    delete OutputData;
    OutputData = NULL;
}


void *DiColorImage::getOutputData(const unsigned long frame, const int bits, const int planar)
{
    if ((InterData != NULL) && (Status == EIS_Normal) && (frame < NumberOfFrames) && (bits > 0) && (bits <= MAX_BITS))
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
                cerr << "WARNING: invalid value for inter-representation !" << endl;
        }
        if (OutputData == NULL)
        {
            Status = EIS_MemoryFailure;
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


DiImage *DiColorImage::scale(const unsigned long width, const unsigned long height, const int interpolate) const
{
    DiImage *image = new DiColorImage(this, (Uint16)width, (Uint16)height, interpolate);
    return image;
}


DiImage *DiColorImage::clip(const unsigned long left, const unsigned long top, const unsigned long width,
    const unsigned long height) const
{
    DiImage *image = new DiColorImage(this, (Uint16)left, (Uint16)top, (Uint16)width, (Uint16)height);
    return image;
}


DiImage *DiColorImage::makeMonochrome(const double red, const double green, const double blue) const
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


int DiColorImage::writePPM(ostream &stream, const unsigned long frame, const int bits)
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


int DiColorImage::writePPM(FILE *stream, const unsigned long frame, const int bits)
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


int DiColorImage::writeRawPPM(FILE *stream, const unsigned long frame, const int bits)
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
** Revision 1.3  1998-05-11 14:52:26  joergr
** Added CVS/RCS header to each file.
**
**
*/
