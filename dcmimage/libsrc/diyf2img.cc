/*********************************************************************
** 
**  DicomYBR422Image (Source)
**
**  author   : Joerg Riesmeier
**  created  : 13.03.98
**  modified : 13.03.98
**
*********************************************************************/


#include "osconfig.h"
#include "dctypes.h"

#include "diyf2img.h"
#include "diyf2pxt.h"
#include "diinpx.h"


/*----------------*
 *  constructors  *
 *----------------*/

DiYBR422Image::DiYBR422Image(const DiDocument *docu, const EI_Status status)
  : DiColorImage(docu, status)
{
    if ((Document != NULL) && (InputData != NULL) && (Status == EIS_Normal))
    {
        switch (InputData->getRepresentation())
        {
            case EPR_Uint8:
                InterData = new DiYBR422PixelTemplate<Uint8, Uint8>(Document, InputData, Status, BitsPerSample);
                break;
            case EPR_Sint8:
                InterData = new DiYBR422PixelTemplate<Sint8, Uint8>(Document, InputData, Status, BitsPerSample);
                break;
            case EPR_Uint16:
                InterData = new DiYBR422PixelTemplate<Uint16, Uint16>(Document, InputData, Status, BitsPerSample);
                break;
            case EPR_Sint16:
                InterData = new DiYBR422PixelTemplate<Sint16, Uint16>(Document, InputData, Status, BitsPerSample);
                break;
            case EPR_Uint32:
                InterData = new DiYBR422PixelTemplate<Uint32, Uint32>(Document, InputData, Status, BitsPerSample);
                break;
            case EPR_Sint32:
                InterData = new DiYBR422PixelTemplate<Sint32, Uint32>(Document, InputData, Status, BitsPerSample);
                break;
        }
        deleteInputData();
        checkInterData();
    }
} 


/*--------------*
 *  destructor  *
 *--------------*/

DiYBR422Image::~DiYBR422Image()
{
}
