/*********************************************************************
** 
**  DicomCMYKImage (Source)
**
**  author   : Joerg Riesmeier
**  created  : 05.01.97
**  modified : 13.02.97
**
*********************************************************************/


#include "osconfig.h"
#include "dctypes.h"

#include "dicmyimg.h"
#include "dicmypxt.h"
#include "diinpx.h"


/*----------------*
 *  constructors  *
 *----------------*/

DiCMYKImage::DiCMYKImage(const DiDocument *docu, const EI_Status status)
  : DiColorImage(docu, status)
{
	if ((Document != NULL) && (InputData != NULL) && (Status == EIS_Normal))
	{
		switch (InputData->getRepresentation())
		{
			case EPR_Uint8:
				InterData = new DiCMYKPixelTemplate<Uint8, Uint8>(Document, InputData, Status, BitsPerSample);
				break;
			case EPR_Sint8:
				InterData = new DiCMYKPixelTemplate<Sint8, Uint8>(Document, InputData, Status, BitsPerSample);
				break;
			case EPR_Uint16:
				InterData = new DiCMYKPixelTemplate<Uint16, Uint16>(Document, InputData, Status, BitsPerSample);
				break;
			case EPR_Sint16:
				InterData = new DiCMYKPixelTemplate<Sint16, Uint16>(Document, InputData, Status, BitsPerSample);
				break;
			case EPR_Uint32:
				InterData = new DiCMYKPixelTemplate<Uint32, Uint32>(Document, InputData, Status, BitsPerSample);
				break;
			case EPR_Sint32:
				InterData = new DiCMYKPixelTemplate<Sint32, Uint32>(Document, InputData, Status, BitsPerSample);
				break;
		}
		deleteInputData();
		checkInterData();
	}
} 


/*--------------*
 *  destructor  *
 *--------------*/

DiCMYKImage::~DiCMYKImage()
{
}
