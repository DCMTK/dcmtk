/*********************************************************************
** 
**  DicomMonoOutputPixel (Source)
**
**  author   : Joerg Riesmeier
**  created  : 19.12.96
**  modified : 13.01.97
**
*********************************************************************/


#include "osconfig.h"

#include "dimoopx.h"
#include "dimopx.h"


/*----------------*
 *  constructors  *
 *----------------*/

DiMonoOutputPixel::DiMonoOutputPixel(const DiMonoPixel *pixel, const unsigned long frames)
  : Count(0)
{
    if (pixel != NULL)
        Count = pixel->getCount() / frames;
}


/*--------------*
 *  destructor  *
 *--------------*/

DiMonoOutputPixel::~DiMonoOutputPixel()
{
}
