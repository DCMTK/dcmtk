/*********************************************************************
** 
**  DicomRGBImage (Source)
**
**  author   : Joerg Riesmeier
**  created  : 23.12.96
**  modified : 13.02.97
**
*********************************************************************/


#include "osconfig.h"
#include "dctypes.h"

#include "dirgbimg.h"
#include "dirgbpxt.h"
#include "diinpx.h"


/*----------------*
 *  constructors  *
 *----------------*/

DiRGBImage::DiRGBImage(const DiDocument *docu, const EI_Status status)
  : DiColorImage(docu, status)
{
	if ((Document != NULL) && (InputData != NULL) && (Status == EIS_Normal))
	{
		switch (InputData->getRepresentation())
		{
			case EPR_Uint8:
				InterData = new DiRGBPixelTemplate<Uint8, Uint8>(Document, InputData, Status, BitsPerSample);
				break;
			case EPR_Sint8:
				InterData = new DiRGBPixelTemplate<Sint8, Uint8>(Document, InputData, Status, BitsPerSample);
				break;
			case EPR_Uint16:
				InterData = new DiRGBPixelTemplate<Uint16, Uint16>(Document, InputData, Status, BitsPerSample);
				break;
			case EPR_Sint16:
				InterData = new DiRGBPixelTemplate<Sint16, Uint16>(Document, InputData, Status, BitsPerSample);
				break;
			case EPR_Uint32:
				InterData = new DiRGBPixelTemplate<Uint32, Uint32>(Document, InputData, Status, BitsPerSample);
				break;
			case EPR_Sint32:
				InterData = new DiRGBPixelTemplate<Sint32, Uint32>(Document, InputData, Status, BitsPerSample);
				break;
		}
		deleteInputData();
		checkInterData();
	}
} 


/*--------------*
 *  destructor  *
 *--------------*/

DiRGBImage::~DiRGBImage()
{
}
