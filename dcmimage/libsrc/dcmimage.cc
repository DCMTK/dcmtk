/********************************************************************
**
**  DicomImage-Interface (Source)
**
**  author   : Joerg Riesmeier
**  created  : 09.12.96
**  modified : 25.04.97
**
*********************************************************************/


#include "osconfig.h"
#include "dctypes.h"
#include "dcdeftag.h"
#include "dcobject.h"
#include "dcuid.h"
#include "dcdict.h"

#include "dcmimage.h"
#include "diovlimg.h"
#include "dimo1img.h"
#include "dimo2img.h"
#include "dipalimg.h"
#include "dirgbimg.h"
#include "dihsvimg.h"
#include "diargimg.h"
#include "dicmyimg.h"
#include "didocu.h"

#include <ctype.h>

#ifndef FILENAME_MAX
 #define FILENAME_MAX 255
#endif


/*----------------------------*
 *  constant initializations  *
 *----------------------------*/

static const SP_Interpretation PhotometricInterpretationNames[] =
{
    {"MONOCHROME1",  EPI_Monochrome1},
    {"MONOCHROME2",  EPI_Monochrome2},
    {"PALETTECOLOR", EPI_PaletteColor},			// space deleted to simplify detection
    {"RGB",			 EPI_RGB},
    {"HSV",			 EPI_HSV},
    {"ARGB",		 EPI_ARGB},
    {"CMYK",		 EPI_CMYK},
	{NULL,			 EPI_Unknown}
};


/*----------------*
 *  constructors  *
 *----------------*/

// --- create 'DicomImage' from 'filename'
 
DicomImage::DicomImage(const char *filename)
  : Status(EIS_Normal),
	PhotometricInterpretation(EPI_Unknown),
	Document(NULL),
	Image(NULL)
{
	if (checkDataDictionary())					// valid 'dicom.dic' found ?
	{
		Document = new DiDocument(filename);
		Init();
	}
}


// --- create DicomImage from valid DicomFileStream
 
DicomImage::DicomImage(DcmFileStream &stream)
  : Status(EIS_Normal),
	PhotometricInterpretation(EPI_Unknown),
	Document(NULL),
	Image(NULL)
{
	if (checkDataDictionary())					// valid 'dicom.dic' found ?
	{
		Document = new DiDocument(stream);
		Init();
	}
}


// --- create 'DicomImage' from valid 'DicomObject'
 
DicomImage::DicomImage(DcmObject *object)
  : Status(EIS_Normal),
	PhotometricInterpretation(EPI_Unknown),
	Document(NULL),
	Image(NULL)
{
	if (checkDataDictionary())					// valid 'dicom.dic' found ?
	{
		Document = new DiDocument(object);
		Init();
	}
}


// --- protected: create 'DicomImage' from source with different image data and photometric interpretation 
 
DicomImage::DicomImage(const DicomImage *dicom, DiImage *image, const EP_Interpretation interp)
  : Status(dicom->Status),
	PhotometricInterpretation(interp),
	Document(dicom->Document),
	Image(image)
{
	if (Document != NULL)
		Document->addReference();				// 'Document' is only referenced not copied !
}


/*--------------* 
 *  destructor  *
 *--------------*/

DicomImage::~DicomImage()
{
	delete Image;
	if (Document != NULL)
		Document->removeReference();			// only delete if object is no longer referenced
}


/*********************************************************************/


// --- initialize 'DicomImage' object (same procedure for every 'real' constructor)

void DicomImage::Init()
{
	if ((Document != NULL) && (Document->good()))
	{
		const char *str;
		if (hasSOPclassUID(UID_StandaloneOverlayStorage))
		{
			PhotometricInterpretation = EPI_Monochrome2;			// standalone overlays are handled like monochrome
			Image = new DiOverlayImage(Document, Status);			// images without pixel data
		}
		else if (Document->getValue(DCM_PhotometricInterpretation, str))
		{
			char *cstr = new char[strlen(str) + 1];
			if (cstr != NULL)
			{
				char *q = cstr;
				for (const char *p = str; *p != 0; p++)				// remove invalid chars
				{
					if (isalpha(*p))
						*(q++) = toupper(*p);
					else if (isdigit(*p))
						*(q++) = *p;
				}
				*q = '\0';											// end of c-string
			}
			else
				cstr = (char *)str;
			const SP_Interpretation *pin = PhotometricInterpretationNames;
			while ((pin->Name != NULL) && (strcmp(pin->Name, cstr) != 0))
				pin++;
			if (cstr != str)
				delete cstr;
			PhotometricInterpretation = pin->Type;					// unknown if last entry
			switch (PhotometricInterpretation)
			{
				case EPI_Monochrome1:
					Image = new DiMono1Image(Document, Status);		// create 'Image' depending on color model
					break;
				case EPI_Monochrome2:
					Image = new DiMono2Image(Document, Status);
					break;
				case EPI_PaletteColor:
					Image = new DiPaletteImage(Document, Status);
					break;
				case EPI_RGB:
					Image = new DiRGBImage(Document, Status);
					break;
				case EPI_HSV:
					Image = new DiHSVImage(Document, Status);
					break;
				case EPI_ARGB:
					Image = new DiARGBImage(Document, Status);
					break;
				case EPI_CMYK:
					Image = new DiCMYKImage(Document, Status);
					break;
				default:											// unknown or unsupported color model
					Status = EIS_InvalidValue;	
					cerr << "ERROR: invalid value for 'PhotometricInterpretation' (" << str << ") ! " << endl;
			}
		}
		else
		{
			Status = EIS_MissingAttribute;	
			cerr << "ERROR: mandatory attribute 'PhotometricInterpretation' is missing ! " << endl;
		}
	}
	else
        Status = EIS_InvalidDocument;
}


// --- check whether the loadable 'DataDictionary' is present/loaded

int DicomImage::checkDataDictionary()
{
	if (dcmDataDict.numberOfEntries() == 0)
	{
		Status = EIS_NoDataDictionary;
		cerr << "ERROR: can't load data dictionary !" << endl;
	}
	return Status == EIS_Normal;
}


/*********************************************************************/


// --- return unique 'SOPclassUID' string

const char *DicomImage::getSOPclassUID() const
{
	if (Document != NULL)
	{
		const char *str;
		if (Document->getValue(DCM_SOPClassUID, str))
			return str;
	}
	return NULL;
}


// --- return 'true' (1) if 'Document' has the same 'SOPclassUID' as given in parameter 'uid'
		
int DicomImage::hasSOPclassUID(const char *uid) const
{
	const char *str = getSOPclassUID();
	return (str != NULL) && (strcmp(str, uid) == 0);
}


// --- create scaled to given size ('width' and 'height') image, memory isn't handled internally !
// --- if one dimension ist '0' the other is automatically adjusted (with respect to pixel aspect ratio)

DicomImage *DicomImage::createScaledImage(unsigned long width, unsigned long height, const int interpolate,
	const int aspect) const
{
	if ((Image != NULL) && (getWidth() > 0) && (getHeight() > 0)) 
	{
		if (aspect)												// maintain pixel aspect ratio
		{
			if (width == 0)
				width = (unsigned long)(getWidthHeightRatio() * (double)(height * getWidth()) / getHeight());
			else if (height == 0)
				height = (unsigned long)(getHeightWidthRatio() * (double)(width * getHeight()) / getWidth());
		}
		else													// ignore pixel aspect ratio
		{
			if (width == 0)
				width = (unsigned long)((double)(height * getWidth()) / getHeight());
			else if (height == 0)
				height = (unsigned long)((double)(width * getHeight()) / getWidth());
		}
		const unsigned long maxvalue = maxval(bitsof(Uint16));
		if (width > maxvalue)
			width = maxvalue;									// limit 'width' to maximum value (65535)
		if (height > maxvalue)
			height = maxvalue;									// same for 'height'
		if ((width > 0) && (height > 0))
		{
			DiImage *image = Image->scale(width, height, interpolate);
			if (image != NULL)
			{
				DicomImage *dicom = new DicomImage(this, image, PhotometricInterpretation);
				return dicom;
			}
		}
	}
	return NULL;
}


// --- create scaled with given factors ('xfactor' and 'yfactor') image, memory isn't handled internally !

DicomImage *DicomImage::createScaledImage(const double xfactor, const double yfactor, const int interpolate,
	const int aspect) const
{
	return createScaledImage((unsigned long)(xfactor * getWidth()), (unsigned long)(yfactor * getHeight()),
		interpolate, aspect);
}


// --- create clipped to given box ('left', 'top' and 'width', 'height') image, memory isn't handled internally !
// --- 'width' and 'height' are optional

DicomImage *DicomImage::createClippedImage(const unsigned long left, const unsigned long top,
	unsigned long width, unsigned long height) const
{
	if ((Image != NULL) && (left < getWidth()) && (top < getHeight()))
	{
		if ((width == 0) || (left + width > getWidth()))		// set 'width' if missing or exceeding image's width
			width = getWidth() - left;
		if ((height == 0) || (top + height > getHeight()))		// same for 'height'
			height = getHeight() - top;
		DiImage *image = Image->clip(left, top, width, height);
		if (image != NULL)
		{
			DicomImage *dicom = new DicomImage(this, image, PhotometricInterpretation);
			return dicom;
		}
	}
	return NULL;
}


// --- create color-image to mono-image with given 'red', 'green' and 'blue' coefficients converted image, memory ... !

DicomImage *DicomImage::createMonochromeImage(const double red, const double green, const double blue) const
{
	if (Image != NULL)
	{
		DiImage *image = Image->makeMonochrome(red, green, blue);		// create monochrome image data
		if (image != NULL)
		{
			DicomImage *dicom = new DicomImage(this, image, EPI_Monochrome2);
			return dicom;
		}
	}
	return NULL;
}


/*********************************************************************/


// --- write 'frame' of image data to 'filename' with 'bits' depth

int DicomImage::writePPM(const char *filename, const int bits, const unsigned long frame)
{
	if ((filename != NULL) && (Image != NULL))
	{
		char fname[FILENAME_MAX + 1];
		if (sprintf(fname, filename, frame) >= 0)			// replace '%d' etc. with frame number
			filename = fname;
		FILE *stream = fopen(filename, "w");				// open text file for writing
		int ok = writePPM(stream, bits, frame);
		fclose(stream);
		return ok;
	}
	return 0;
}


// --- same for C++ 'ostream'

int DicomImage::writePPM(ostream &stream, const int bits, const unsigned long frame)
{
	if ((stream.good()) && (Image != NULL))
		return Image->writePPM(stream, frame, Image->getBits(bits));
	return 0;
}


// --- same for C 'FILE'

int DicomImage::writePPM(FILE *stream, const int bits, const unsigned long frame)
{
	if ((stream != NULL) && (Image != NULL))
		return Image->writePPM(stream, frame, Image->getBits(bits));
	return 0;
}


// --- same for RAW PPM (binary form of PPM with a maximum of 8 bits depth)

int DicomImage::writeRawPPM(const char *filename, const int bits, const unsigned long frame)
{
	if ((filename != NULL) && (Image != NULL) && (Image->getBits(bits) <= MAX_RAWPPM_BITS))
	{
		char fname[FILENAME_MAX + 1];
		if (sprintf(fname, filename, frame) >= 0)			// replace '%d' etc. with frame number
			filename = fname;
		FILE *stream = fopen(filename, "wb");				// open binary file for writing
		if (stream != NULL)
		{
			int ok = Image->writeRawPPM(stream, frame, Image->getBits(bits));
			fclose(stream);
			return ok;
		}
	}
	return 0;
}
