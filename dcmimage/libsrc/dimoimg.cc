/*********************************************************************
** 
**  DicomMonochromeImage (Source)
**
**  author   : Joerg Riesmeier
**  created  : 12.12.96
**  modified : 25.05.97
**
*********************************************************************/


#include "osconfig.h"
#include "dctypes.h"
#include "dcdeftag.h"

#include "dimoimg.h"
#include "dimo2img.h"
#include "dimoipxt.h"
#include "dicoimg.h"
#include "dicomot.h"
#include "dimosct.h"
#include "dimoclt.h"
#include "dimoopxt.h"
#include "diluptab.h"
#include "didocu.h"
#include "diutils.h"


/*----------------*
 *  constructors  *
 *----------------*/

/*
 *   standard constructor to create mono-image
 */
 
DiMonoImage::DiMonoImage(const DiDocument *docu, const EI_Status status)
  : DiImage(docu, status),
	WindowCenter(0),
	WindowWidth(0),
	WindowCount(0),
	VoiLutCount(0),
	ValidWindow(0),
	Overlays(NULL),
	VoiLutData(NULL),
	InterData(NULL),
	OutputData(NULL)
{
	if ((Document != NULL) && (InputData != NULL) && (Status == EIS_Normal))
	{
		DiMonoModality *modality = new DiMonoModality(Document, InputData);
		if (modality != NULL)
		{
			switch (InputData->getRepresentation())
			{
				case EPR_Uint8:
					switch (modality->getRepresentation())
					{
						case EPR_Uint8:
							InterData = new DiMonoInputPixelTemplate<Uint8, Uint8>(InputData, modality);
							break;
						case EPR_Sint8:
							InterData = new DiMonoInputPixelTemplate<Uint8, Sint8>(InputData, modality);
							break;
						case EPR_Uint16:
							InterData = new DiMonoInputPixelTemplate<Uint8, Uint16>(InputData, modality);
							break;
						case EPR_Sint16:
							InterData = new DiMonoInputPixelTemplate<Uint8, Sint16>(InputData, modality);
							break;
						case EPR_Uint32:
							InterData = new DiMonoInputPixelTemplate<Uint8, Uint32>(InputData, modality);
							break;
						case EPR_Sint32:
							InterData = new DiMonoInputPixelTemplate<Uint8, Sint32>(InputData, modality);
							break;
					}
					break;
				case EPR_Sint8:
					switch (modality->getRepresentation())
					{
						case EPR_Uint8:
							InterData = new DiMonoInputPixelTemplate<Sint8, Uint8>(InputData, modality);
							break;
						case EPR_Sint8:
							InterData = new DiMonoInputPixelTemplate<Sint8, Sint8>(InputData, modality);
							break;
						case EPR_Uint16:
							InterData = new DiMonoInputPixelTemplate<Sint8, Uint16>(InputData, modality);
							break;
						case EPR_Sint16:
							InterData = new DiMonoInputPixelTemplate<Sint8, Sint16>(InputData, modality);
							break;
						case EPR_Uint32:
							InterData = new DiMonoInputPixelTemplate<Sint8, Uint32>(InputData, modality);
							break;
						case EPR_Sint32:
							InterData = new DiMonoInputPixelTemplate<Sint8, Sint32>(InputData, modality);
							break;
					}
					break;
				case EPR_Uint16:
					switch (modality->getRepresentation())
					{
						case EPR_Uint8:
							InterData = new DiMonoInputPixelTemplate<Uint16, Uint8>(InputData, modality);
							break;
						case EPR_Sint8:
							InterData = new DiMonoInputPixelTemplate<Uint16, Sint8>(InputData, modality);
							break;
						case EPR_Uint16:
							InterData = new DiMonoInputPixelTemplate<Uint16, Uint16>(InputData, modality);
							break;
						case EPR_Sint16:
							InterData = new DiMonoInputPixelTemplate<Uint16, Sint16>(InputData, modality);
							break;
						case EPR_Uint32:
							InterData = new DiMonoInputPixelTemplate<Uint16, Uint32>(InputData, modality);
							break;
						case EPR_Sint32:
							InterData = new DiMonoInputPixelTemplate<Uint16, Sint32>(InputData, modality);
							break;
					}
					break;
				case EPR_Sint16:
					switch (modality->getRepresentation())
					{
						case EPR_Uint8:
							InterData = new DiMonoInputPixelTemplate<Sint16, Uint8>(InputData, modality);
							break;
						case EPR_Sint8:
							InterData = new DiMonoInputPixelTemplate<Sint16, Sint8>(InputData, modality);
							break;
						case EPR_Uint16:
							InterData = new DiMonoInputPixelTemplate<Sint16, Uint16>(InputData, modality);
							break;
						case EPR_Sint16:
							InterData = new DiMonoInputPixelTemplate<Sint16, Sint16>(InputData, modality);
							break;
						case EPR_Uint32:
							InterData = new DiMonoInputPixelTemplate<Sint16, Uint32>(InputData, modality);
							break;
						case EPR_Sint32:
							InterData = new DiMonoInputPixelTemplate<Sint16, Sint32>(InputData, modality);
							break;
					}
					break;
				case EPR_Uint32:
					switch (modality->getRepresentation())
					{
						case EPR_Uint8:
							InterData = new DiMonoInputPixelTemplate<Uint32, Uint8>(InputData, modality);
							break;
						case EPR_Sint8:
							InterData = new DiMonoInputPixelTemplate<Uint32, Sint8>(InputData, modality);
							break;
						case EPR_Uint16:
							InterData = new DiMonoInputPixelTemplate<Uint32, Uint16>(InputData, modality);
							break;
						case EPR_Sint16:
							InterData = new DiMonoInputPixelTemplate<Uint32, Sint16>(InputData, modality);
							break;
						case EPR_Uint32:
							InterData = new DiMonoInputPixelTemplate<Uint32, Uint32>(InputData, modality);
							break;
						case EPR_Sint32:
							InterData = new DiMonoInputPixelTemplate<Uint32, Sint32>(InputData, modality);
							break;
					}
					break;
				case EPR_Sint32:
					switch (modality->getRepresentation())
					{
						case EPR_Uint8:
							InterData = new DiMonoInputPixelTemplate<Sint32, Uint8>(InputData, modality);
							break;
						case EPR_Sint8:
							InterData = new DiMonoInputPixelTemplate<Sint32, Sint8>(InputData, modality);
							break;
						case EPR_Uint16:
							InterData = new DiMonoInputPixelTemplate<Sint32, Uint16>(InputData, modality);
							break;
						case EPR_Sint16:
							InterData = new DiMonoInputPixelTemplate<Sint32, Sint16>(InputData, modality);
							break;
						case EPR_Uint32:
							InterData = new DiMonoInputPixelTemplate<Sint32, Uint32>(InputData, modality);
							break;
						case EPR_Sint32:
							InterData = new DiMonoInputPixelTemplate<Sint32, Sint32>(InputData, modality);
							break;
					}
					break;
			}
			deleteInputData();									// no longer needed, save memory
			if (InterData == NULL)
			{
				Status = EIS_MemoryFailure;
				cerr << "ERROR: can't allocate memory for inter-representation !" << endl;
			}
			else if (InterData->getData() == NULL)
				Status = EIS_InvalidImage;
			else
			{
				unsigned long count = (unsigned long)Columns * (unsigned long)Rows * NumberOfFrames;
			 	if ((InterData->getCount() != count) && ((InterData->getCount() >> 1) != ((count + 1) >> 1)))
				{
					cerr << "WARNING: computed (" << count << ") and stored (" << InterData->getCount() << ") ";
					cerr << "pixel count differ !" << endl;
				}
				WindowCount = Document->getVM(DCM_WindowCenter);
				count = Document->getVM(DCM_WindowWidth);
				if (count < WindowCount)						// determine number of voi windows
					WindowCount = count;
				Overlays = new DiOverlay(Document, BitsAllocated);
				showAllOverlays();								// default: show all overlays with stored modes
			}
		}
	}
} 


/*
 *   kind of copy-constructor, 'dummy' is necessary to differ from the above standard-constructor
 */

DiMonoImage::DiMonoImage(const DiDocument *docu, const EI_Status status, const int dummy)
  : DiImage(docu, status, dummy),
	WindowCenter(0),
	WindowWidth(0),
	WindowCount(0),
	VoiLutCount(0),
	ValidWindow(0),
	Overlays(NULL),
	VoiLutData(NULL),
	InterData(NULL),
	OutputData(NULL)
{
}


/*
 *   convert color-image to mono-image with given 'red', 'green' and 'blue' coefficients
 */

DiMonoImage::DiMonoImage(const DiColorImage *image, const double red, const double green, const double blue)
  : DiImage(image),
	WindowCenter(0),
	WindowWidth(0),
	WindowCount(0),
	VoiLutCount(0),
	ValidWindow(0),
	Overlays(NULL),
	VoiLutData(NULL),
	InterData(NULL),
	OutputData(NULL)
{
	switch (image->getInterData()->getRepresentation())
	{
		case EPR_Uint8:
			InterData = new DiColorMonoTemplate<Uint8>(image->getInterData(), red, green, blue);
			break;
		case EPR_Sint8:
			InterData = new DiColorMonoTemplate<Sint8>(image->getInterData(), red, green, blue);
			break;
		case EPR_Uint16:
			InterData = new DiColorMonoTemplate<Uint16>(image->getInterData(), red, green, blue);
			break;
		case EPR_Sint16:
			InterData = new DiColorMonoTemplate<Sint16>(image->getInterData(), red, green, blue);
			break;
		case EPR_Uint32:
			InterData = new DiColorMonoTemplate<Uint32>(image->getInterData(), red, green, blue);
			break;
		case EPR_Sint32:
			InterData = new DiColorMonoTemplate<Sint32>(image->getInterData(), red, green, blue);
			break;
	}
	if ((InterData == NULL) || (InterData->getData() == NULL))
		Status = EIS_InvalidImage;
}


/*
 *   scale 'image' to size given by 'columns' and 'rows', 'interpolate' always if parameter is true
 */

DiMonoImage::DiMonoImage(const DiMonoImage *image, const Uint16 columns, const Uint16 rows, const int interpolate)
  : DiImage(image, columns, rows),
	WindowCenter(image->WindowCenter),
	WindowWidth(image->WindowWidth),
	WindowCount(image->WindowCount),
	VoiLutCount(image->VoiLutCount),
	ValidWindow(image->ValidWindow),
	Overlays(NULL),
	VoiLutData(image->VoiLutData),
	InterData(NULL),
	OutputData(NULL)
{
	switch (image->InterData->getRepresentation())
	{
		case EPR_Uint8:
			InterData = new DiMonoScaleTemplate<Uint8>(image->InterData, image->Columns, image->Rows, columns,
				rows, NumberOfFrames, interpolate);
			break;
		case EPR_Sint8:
			InterData = new DiMonoScaleTemplate<Sint8>(image->InterData, image->Columns, image->Rows, columns,
				rows, NumberOfFrames, interpolate);
			break;
		case EPR_Uint16:
			InterData = new DiMonoScaleTemplate<Uint16>(image->InterData, image->Columns, image->Rows, columns,
				rows, NumberOfFrames, interpolate);
			break;
		case EPR_Sint16:
			InterData = new DiMonoScaleTemplate<Sint16>(image->InterData, image->Columns, image->Rows, columns,
				rows, NumberOfFrames, interpolate);
			break;
		case EPR_Uint32:
			InterData = new DiMonoScaleTemplate<Uint32>(image->InterData, image->Columns, image->Rows, columns,
				rows, NumberOfFrames, interpolate);
			break;
		case EPR_Sint32:
			InterData = new DiMonoScaleTemplate<Sint32>(image->InterData, image->Columns, image->Rows, columns,
				rows, NumberOfFrames, interpolate);
			break;
	}
	if ((InterData != NULL) && (InterData->getData() != NULL))
	{
		if ((image->Overlays != NULL) && (image->Overlays->getCount() > 0))
			Overlays = new DiOverlay(image->Overlays, (double)columns / (double)image->Columns, (double)rows /
				(double)image->Rows);
	}
	else
		Status = EIS_InvalidImage;
}


/*
 *   clip 'image' to rectangle given by 'left', 'top', 'columns' and 'rows' 
 */

DiMonoImage::DiMonoImage(const DiMonoImage *image, const Uint16 left, const Uint16 top, const Uint16 columns,
	const Uint16 rows)
  : DiImage(image, columns, rows),
	WindowCenter(image->WindowCenter),
	WindowWidth(image->WindowWidth),
	WindowCount(image->WindowCount),
	VoiLutCount(image->VoiLutCount),
	ValidWindow(image->ValidWindow),
	Overlays(NULL),
	VoiLutData(image->VoiLutData),
	InterData(NULL),
	OutputData(NULL)
{
	switch (image->InterData->getRepresentation())
	{
		case EPR_Uint8:
			InterData = new DiMonoClipTemplate<Uint8>(image->InterData, image->Columns, image->Rows, left, top,
				columns, rows, NumberOfFrames);
			break;
		case EPR_Sint8:
			InterData = new DiMonoClipTemplate<Sint8>(image->InterData, image->Columns, image->Rows, left, top,
				columns, rows, NumberOfFrames);
			break;
		case EPR_Uint16:
			InterData = new DiMonoClipTemplate<Uint16>(image->InterData, image->Columns, image->Rows, left, top,
				columns, rows, NumberOfFrames);
			break;
		case EPR_Sint16:
			InterData = new DiMonoClipTemplate<Sint16>(image->InterData, image->Columns, image->Rows, left, top,
				columns, rows, NumberOfFrames);
			break;
		case EPR_Uint32:
			InterData = new DiMonoClipTemplate<Uint32>(image->InterData, image->Columns, image->Rows, left, top,
				columns, rows, NumberOfFrames);
			break;
		case EPR_Sint32:
			InterData = new DiMonoClipTemplate<Sint32>(image->InterData, image->Columns, image->Rows, left, top,
				columns, rows, NumberOfFrames);
			break;
	}
	if ((InterData != NULL) && (InterData->getData() != NULL))
	{
		if ((image->Overlays != NULL) && (image->Overlays->getCount() > 0))
			Overlays = new DiOverlay(image->Overlays, left, top);
	}
	else
		Status = EIS_InvalidImage;
}


/*--------------*
 *  destructor  *
 *--------------*/

DiMonoImage::~DiMonoImage()
{
	delete InterData;
	delete OutputData;
	delete VoiLutData;
	delete Overlays;
}


/*********************************************************************/


void DiMonoImage::deleteOutputData()
{
	delete OutputData;
	OutputData = NULL;
}


void *DiMonoImage::getOutputPlane(const int) const
{
	if (OutputData != NULL)
		return (void *)OutputData->getData();				// monochrome images don't have several planes
	return NULL;
}


/*********************************************************************/


int DiMonoImage::setMinMaxWindow(const int index)
{
	if (InterData != NULL)
	{
		double center;
		double width;
		if (InterData->getMinMaxWindow(index != 0, center, width))
			return setWindow(center, width);
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
			return setWindow(center, width);
	}
	return 0;
}


int DiMonoImage::setWindow(const unsigned long pos)
{
	double center;
	double width;
	WindowCount = Document->getValue(DCM_WindowCenter, center, pos);
	unsigned long count = Document->getValue(DCM_WindowWidth, width, pos);
	if (count < WindowCount)
		WindowCount = count;
	if (pos < WindowCount)
		return setWindow(center, width);
	return 0;
}


int DiMonoImage::setWindow(const double center, const double width)
{
	delete VoiLutData;
	if (!ValidWindow || (center != WindowCenter) || (width != WindowWidth))
	{
		WindowCenter = center;
		WindowWidth = width;
		return ValidWindow = 1;
	}
	return 2;													// window is unchanged
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


int DiMonoImage::setVoiLut(const unsigned long pos)					// UNTESTED !!
{
	delete VoiLutData;
	VoiLutData = new DiLookupTable(Document, DCM_VOILUTSequence, DCM_LUTDescriptor, DCM_LUTData, pos, &VoiLutCount);
	if (VoiLutData != NULL)
		return VoiLutData->isValid();
	return 0;
}


/*********************************************************************/


/*
 *   create output data of 'frame' with depth of 'bits' and min/max values of 'low' and 'high' (support mono1/2)
 */

void *DiMonoImage::getOutputData(const unsigned long frame, const int bits, const Uint32 low, const Uint32 high)
{
	if ((InterData != NULL) && (Status == EIS_Normal) && (frame < NumberOfFrames) && (bits > 0) && (bits <= MAX_BITS))
	{
		deleteOutputData();								// delete old image data
		if (!ValidWindow)
			WindowWidth = 0;							// width of zero means no window, saves additional parameter
		switch (InterData->getRepresentation())
		{
			case EPR_Uint8:
				if (bits <= 8)
					OutputData = new DiMonoOutputPixelTemplate<Uint8, Uint8>(InterData, Overlays, VoiLutData,
						WindowCenter, WindowWidth, BitsPerSample, low, high, Columns, Rows, frame, NumberOfFrames);
				else if (bits <= 16)
					OutputData = new DiMonoOutputPixelTemplate<Uint8, Uint16>(InterData, Overlays, VoiLutData,
						WindowCenter, WindowWidth, BitsPerSample, low, high, Columns, Rows, frame, NumberOfFrames);
				else
					OutputData = new DiMonoOutputPixelTemplate<Uint8, Uint32>(InterData, Overlays, VoiLutData,
						WindowCenter, WindowWidth, BitsPerSample, low, high, Columns, Rows, frame, NumberOfFrames);
				break;
			case EPR_Sint8:
				if (bits <= 8)
					OutputData = new DiMonoOutputPixelTemplate<Sint8, Uint8>(InterData, Overlays, VoiLutData,
						WindowCenter, WindowWidth, BitsPerSample, low, high, Columns, Rows, frame, NumberOfFrames);
				else if (bits <= 16)
					OutputData = new DiMonoOutputPixelTemplate<Sint8, Uint16>(InterData, Overlays, VoiLutData,
						WindowCenter, WindowWidth, BitsPerSample, low, high, Columns, Rows, frame, NumberOfFrames);
				else
					OutputData = new DiMonoOutputPixelTemplate<Sint8, Uint32>(InterData, Overlays, VoiLutData,
						WindowCenter, WindowWidth, BitsPerSample, low, high, Columns, Rows, frame, NumberOfFrames);
				break;
			case EPR_Uint16:
				if (bits <= 8)
					OutputData = new DiMonoOutputPixelTemplate<Uint16, Uint8>(InterData, Overlays, VoiLutData,
						WindowCenter, WindowWidth, BitsPerSample, low, high, Columns, Rows, frame, NumberOfFrames);
				else if (bits <= 16)
					OutputData = new DiMonoOutputPixelTemplate<Uint16, Uint16>(InterData, Overlays, VoiLutData,
						WindowCenter, WindowWidth, BitsPerSample, low, high, Columns, Rows, frame, NumberOfFrames);
				else
					OutputData = new DiMonoOutputPixelTemplate<Uint16, Uint32>(InterData, Overlays, VoiLutData,
						WindowCenter, WindowWidth, BitsPerSample, low, high, Columns, Rows, frame, NumberOfFrames);
				break;
			case EPR_Sint16:
				if (bits <= 8)
					OutputData = new DiMonoOutputPixelTemplate<Sint16, Uint8>(InterData, Overlays, VoiLutData,
						WindowCenter, WindowWidth, BitsPerSample, low, high, Columns, Rows, frame, NumberOfFrames);
				else if (bits <= 16)
					OutputData = new DiMonoOutputPixelTemplate<Sint16, Uint16>(InterData, Overlays, VoiLutData,
						WindowCenter, WindowWidth, BitsPerSample, low, high, Columns, Rows, frame, NumberOfFrames);
				else
					OutputData = new DiMonoOutputPixelTemplate<Sint16, Uint32>(InterData, Overlays, VoiLutData,
						WindowCenter, WindowWidth, BitsPerSample, low, high, Columns, Rows, frame, NumberOfFrames);
				break;
			case EPR_Uint32:
				if (bits <= 8)
					OutputData = new DiMonoOutputPixelTemplate<Uint32, Uint8>(InterData, Overlays, VoiLutData,
						WindowCenter, WindowWidth, BitsPerSample, low, high, Columns, Rows, frame, NumberOfFrames);
				else if (bits <= 16)
					OutputData = new DiMonoOutputPixelTemplate<Uint32, Uint16>(InterData, Overlays, VoiLutData,
						WindowCenter, WindowWidth, BitsPerSample, low, high, Columns, Rows, frame, NumberOfFrames);
				else
					OutputData = new DiMonoOutputPixelTemplate<Uint32, Uint32>(InterData, Overlays, VoiLutData,
						WindowCenter, WindowWidth, BitsPerSample, low, high, Columns, Rows, frame, NumberOfFrames);
				break;
			case EPR_Sint32:
				if (bits <= 8)
					OutputData = new DiMonoOutputPixelTemplate<Sint32, Uint8>(InterData, Overlays, VoiLutData,
						WindowCenter, WindowWidth, BitsPerSample, low, high, Columns, Rows, frame, NumberOfFrames);
				else if (bits <= 16)
					OutputData = new DiMonoOutputPixelTemplate<Sint32, Uint16>(InterData, Overlays, VoiLutData,
						WindowCenter, WindowWidth, BitsPerSample, low, high, Columns, Rows, frame, NumberOfFrames);
				else
					OutputData = new DiMonoOutputPixelTemplate<Sint32, Uint32>(InterData, Overlays, VoiLutData,
						WindowCenter, WindowWidth, BitsPerSample, low, high, Columns, Rows, frame, NumberOfFrames);
				break;
		}
		if (OutputData == NULL)
		{
			Status = EIS_MemoryFailure;
			cerr << "ERROR: can't allocate memory for output-representation !" << endl;
		}
		else
			return OutputData->getData();			// points to beginning of output data
	}
	return NULL;
}


/*
 *   create 24-bit true color device independent bitmap (DIB) as needed by MS-Windows
 */

void *DiMonoImage::createDIB(const unsigned long frame)
{
	getOutputData(frame, 8);						// create output data with 8 bit depth
	Uint8 *data = NULL;
	if ((OutputData != NULL) && (OutputData->getData() != NULL))
	{
		const int gap = Columns & 0x3;				// each line has to start at 32-bit-address!
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
					value = *(p++);					// store gray value
					for (j = 0; j < 3; j++)
						*(q++) = value;				// copy to the three RGB-planes
				}
				q += gap;
			}
		}
	}
	deleteOutputData();
							// output data is no longer needed
	return (void *)data;
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
