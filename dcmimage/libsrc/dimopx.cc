/*********************************************************************
** 
**  DicomMonochromePixel (Source)
**
**  author   : Joerg Riesmeier
**  created  : 17.12.96
**  modified : 14.01.97
**
*********************************************************************/


#include "osconfig.h"

#include "dimopx.h"
#include "dicopx.h"
#include "dimomod.h"
#include "diinpx.h"


/*----------------*
 *  constructors  *
 *----------------*/

DiMonoPixel::DiMonoPixel(const unsigned long count)
  : Modality(NULL),
    Count(count)
{
}


DiMonoPixel::DiMonoPixel(const DiInputPixel *pixel, DiMonoModality *modality)
  : Modality(modality),
    Count(0)
{
    if (pixel != NULL)
        Count = pixel->getCount();
}


DiMonoPixel::DiMonoPixel(const DiColorPixel *pixel)
  : Modality(NULL),
    Count(pixel->getCount())
{
}


DiMonoPixel::DiMonoPixel(const DiMonoPixel *pixel, const unsigned long count)
  : Modality(pixel->Modality),
    Count(count)
{
    if (Modality != NULL)
        Modality->addReference();
}


/*--------------*
 *  destructor  *
 *--------------*/

DiMonoPixel::~DiMonoPixel()
{
    if (Modality != NULL)
        Modality->removeReference();
}
