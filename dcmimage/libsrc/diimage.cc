/********************************************************************
** 
**  DicomImage (Source)
**
**  author   : Joerg Riesmeier
**  created  : 10.12.96
**  modified : 28.07.97
**
*********************************************************************/


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

DiImage::DiImage(const DiDocument *docu, const EI_Status status)
  : Status(status),
  	Document(docu),
  	NumberOfFrames(0),
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
	hasPixelAspectRatio(0),
	isOriginal(1),
	InputData(NULL)
{
    if ((Document != NULL) && (Status == EIS_Normal))
    {
	Sint32 sl = 0;
        if (Document->getValue(DCM_NumberOfFrames, sl))
	{
	    if (sl < 1)
	    {
		cerr << "WARNING: invalid value for 'NumberOfFrames' (" << sl << ") ";
		cerr << "... assuming " << (NumberOfFrames = 1) << " !" << endl; 
	    }
	    else
		NumberOfFrames = (Uint32)sl;
	}
	else
	    NumberOfFrames = 1;
	int ok = (Document->getValue(DCM_Rows, Rows) > 0);
        ok &= (Document->getValue(DCM_Columns, Columns) > 0);
	if (!ok || ((Rows > 0) && (Columns > 0)))
	{
	    Uint16 us;
	    ok &= (Document->getValue(DCM_BitsAllocated, BitsAllocated) > 0);
	    ok &= (Document->getValue(DCM_BitsStored, BitsStored) > 0);
	    ok &= (Document->getValue(DCM_HighBit, HighBit) > 0);
	    ok &= (Document->getValue(DCM_PixelRepresentation, us) > 0);
	    BitsPerSample = BitsStored;
	    hasSignedRepresentation = (us == 1);
	    if ((us != 0) && (us != 1))
	    {
		cerr << "WARNING: invalid value for 'PixelRepresentation' (" << us << ") ";
		cerr << "... assuming 'unsigned' (0) !" << endl;
	    }
	    hasPixelSpacing = (Document->getValue(DCM_PixelSpacing, PixelHeight, 0) > 0);
	    hasPixelSpacing &= (Document->getValue(DCM_PixelSpacing, PixelWidth, 1) > 0);
	    if (!hasPixelSpacing)
	    {
		Sint32 sl;
		hasPixelAspectRatio = (Document->getValue(DCM_PixelAspectRatio, sl, 0) > 0);
		PixelHeight = sl;
		hasPixelAspectRatio &= (Document->getValue(DCM_PixelAspectRatio, sl, 1) > 0);
		PixelWidth = sl;
		if (!hasPixelAspectRatio)
		{
		    PixelWidth = 1;
		    PixelHeight = 1;
		}
	    }
	    checkPixelExtension();
	    DcmStack pstack;
	    if (ok && Document->search(DCM_PixelData, pstack))
	    {
		DcmPixelData * pixel = (DcmPixelData *)pstack.top();
 		if (pixel->chooseRepresentation(EXS_LittleEndianExplicit, NULL, pstack) == EC_Normal)
		{
		    Status = EIS_Normal;
		    convertPixelData(pixel);
		}
		else
		{
		    Status = EIS_NotSupportedValue;
		    cerr << "ERROR: cannot change to unencapsulated representation for pixel data!" << endl;
		}
	    }
	    else
	    {
		Status = EIS_MissingAttribute;
		cerr << "ERROR: one or more mandatory attributes are missing in image pixel module !" << endl;
	    }
	}
	else
	{
	    Status = EIS_InvalidValue;
	    cerr << "ERROR: invalid value for 'Rows' (" << Rows << ") and/or 'Columns' (" << Columns << ") !" << endl;
	}
    }
    else
    {
        Status = EIS_InvalidDocument;
	cerr << "ERROR: this DICOM document is invalid !" << endl;
    }
}


DiImage::DiImage(const DiDocument *docu, const EI_Status status, const int)
  : Status(status),
  	Document(docu),
  	NumberOfFrames(0),
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
	hasPixelAspectRatio(0),
	isOriginal(1),
	InputData(NULL)
{
}


DiImage::DiImage(const DiImage *image)
  : Status(image->Status),
	Document(image->Document),
	NumberOfFrames(image->NumberOfFrames), 
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
	hasPixelAspectRatio(image->hasPixelAspectRatio),
	isOriginal(0),
	InputData(NULL)
{
}


DiImage::DiImage(const DiImage *image, const Uint16 columns, const Uint16 rows)
  : Status(image->Status),
	Document(image->Document),
	NumberOfFrames(image->NumberOfFrames), 
	Rows(rows), 
	Columns(columns),
	PixelWidth(image->PixelWidth),
	PixelHeight(image->PixelHeight),
	BitsAllocated(image->BitsAllocated),
	BitsStored(image->BitsStored),
	HighBit(image->HighBit),
	BitsPerSample(image->BitsPerSample),
	hasSignedRepresentation(image->hasSignedRepresentation),
	hasPixelSpacing(image->hasPixelSpacing),
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


void DiImage::deleteInputData()
{
	delete InputData;
	InputData = NULL;
}


void DiImage::checkPixelExtension()
{
	if (hasPixelSpacing || hasPixelAspectRatio)
	{
		if (PixelHeight == 0)
		{
			cerr << "WARNING: invalid value for 'PixelHeight' (" << PixelHeight << ") ";
			cerr << "... assuming " << (PixelHeight = 1) << " !" << endl;
		}
		else if (PixelHeight < 0)
		{
			cerr << "WARNING: negative value for 'PixelHeight' (" << PixelHeight << ") ";
			cerr << "... assuming " << (PixelHeight = -PixelHeight) << " !" << endl;
		}
		if (PixelWidth == 0)
		{
			cerr << "WARNING: invalid value for 'PixelWidth' (" << PixelWidth << ") ";
			cerr << "... assuming " << (PixelWidth = 1) << " !" << endl;
		}
		else if (PixelWidth < 0)
		{
			cerr << "WARNING: negative value for 'PixelWidth' (" << PixelWidth << ") ";
			cerr << "... assuming " << (PixelWidth = -PixelWidth) << " !" << endl;
		}
	}
}


void DiImage::convertPixelData(/*const*/ DcmPixelData *pixel)
{
    if ((pixel->getVR() == EVR_OW) || ((pixel->getVR() == EVR_OB) && (BitsAllocated <= 8)))
    {
	if ((BitsAllocated < 1) || (BitsStored < 1) || (BitsAllocated < BitsStored) || 
	    (BitsStored > (Uint16)(HighBit + 1)))
	{
	    Status = EIS_InvalidValue;
	    cerr << "ERROR: invalid values for 'BitsAllocated' (" << BitsAllocated << "), ";
	    cerr << "'BitsStored' (" << BitsStored << ") and/or 'HighBit' (" << HighBit << ") !" << endl;
	    return;
	}
	else if ((pixel->getVR() == EVR_OB) && (BitsAllocated <= 8))
	{
	    if (hasSignedRepresentation)
		InputData = new DiInputPixelTemplate<Uint8, Sint8>(pixel, BitsAllocated, BitsStored, HighBit);
	    else
		InputData  = new DiInputPixelTemplate<Uint8, Uint8>(pixel, BitsAllocated, BitsStored, HighBit);
	}
	else if (BitsStored <= bitsof(Uint8)) 
	{
	    if (hasSignedRepresentation)
		InputData = new DiInputPixelTemplate<Uint16, Sint8>(pixel, BitsAllocated, BitsStored, HighBit);
	    else
		InputData  = new DiInputPixelTemplate<Uint16, Uint8>(pixel, BitsAllocated, BitsStored, HighBit);
	}
	else if (BitsStored <= bitsof(Uint16))
	{
	    if (hasSignedRepresentation)
		InputData = new DiInputPixelTemplate<Uint16, Sint16>(pixel, BitsAllocated, BitsStored, HighBit);
	    else
		InputData = new DiInputPixelTemplate<Uint16, Uint16>(pixel, BitsAllocated, BitsStored, HighBit);
	}
	else if (BitsStored <= bitsof(Uint32))
	{
	    if (hasSignedRepresentation)
		InputData = new DiInputPixelTemplate<Uint16, Sint32>(pixel, BitsAllocated, BitsStored, HighBit);
	    else
		InputData = new DiInputPixelTemplate<Uint16, Uint32>(pixel, BitsAllocated, BitsStored, HighBit);
	}
	else	/* BitsStored > 32 !! */
	{
	    Status = EIS_NotSupportedValue;
	    cerr << "ERROR: invalid value for 'BitsStored' (" << BitsStored << ") ";
	    cerr << "... exceeds " << MAX_BITS << " bit !" << endl;
	    return;
    	}
	if (InputData == NULL)
	{
	    Status = EIS_MemoryFailure;
	    cerr << "ERROR: can't allocate memory for input-representation !" << endl;
	}
    }
    else
    {
	Status = EIS_NotSupportedValue;
	cerr << "ERROR: 'PixelData' has an other value representation than OB (with 'BitsAllocated' <= 8) ";
	cerr << "or OW !" << endl;
    }
}
