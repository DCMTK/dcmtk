/*********************************************************************
** 
**  DicomHSVImage (Source)
**
**  author   : Joerg Riesmeier
**  created  : 12.01.97
**  modified : 13.02.97
**
*********************************************************************/


#include "osconfig.h"
#include "dctypes.h"

#include "dihsvimg.h"
#include "dihsvpxt.h"
#include "diinpx.h"


/*----------------*
 *  constructors  *
 *----------------*/

DiHSVImage::DiHSVImage(const DiDocument *docu, const EI_Status status)
  : DiColorImage(docu, status)
{
	if ((Document != NULL) && (InputData != NULL) && (Status == EIS_Normal))
	{
		switch (InputData->getRepresentation())
		{
			case EPR_Uint8:
				InterData = new DiHSVPixelTemplate<Uint8, Uint8>(Document, InputData, Status, BitsPerSample);
				break;
			case EPR_Sint8:
				InterData = new DiHSVPixelTemplate<Sint8, Uint8>(Document, InputData, Status, BitsPerSample);
				break;
			case EPR_Uint16:
				InterData = new DiHSVPixelTemplate<Uint16, Uint16>(Document, InputData, Status, BitsPerSample);
				break;
			case EPR_Sint16:
				InterData = new DiHSVPixelTemplate<Sint16, Uint16>(Document, InputData, Status, BitsPerSample);
				break;
			case EPR_Uint32:
				InterData = new DiHSVPixelTemplate<Uint32, Uint32>(Document, InputData, Status, BitsPerSample);
				break;
			case EPR_Sint32:
				InterData = new DiHSVPixelTemplate<Sint32, Uint32>(Document, InputData, Status, BitsPerSample);
				break;
		}
		deleteInputData();
		checkInterData();
	}
} 


/*--------------*
 *  destructor  *
 *--------------*/

DiHSVImage::~DiHSVImage()
{
}
