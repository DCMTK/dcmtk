/*********************************************************************
** 
**  DicomColorOutputPixel (Source)
**
**  author   : Joerg Riesmeier
**  created  : 20.12.96
**  modified : 22.01.97
**
*********************************************************************/


#include "osconfig.h"

#include "dicoopx.h"
#include "dicopx.h"


/*----------------*
 *  constructors  *
 *----------------*/

DiColorOutputPixel::DiColorOutputPixel(const DiColorPixel *pixel, const unsigned long frames)
  : Count(0)
{
    if (pixel != NULL)
        Count = pixel->getCount() / frames;
}


/*--------------*
 *  destructor  *
 *--------------*/

DiColorOutputPixel::~DiColorOutputPixel()
{
}

