/*********************************************************************
** 
**  DicomRGBImage (Header)
**
**  author   : Joerg Riesmeier
**  created  : 20.12.96
**  modified : 13.02.97
**
*********************************************************************/


#ifndef __DIRGBIMG_H
#define __DIRGBIMG_H

#include "osconfig.h"

#include "dicoimg.h"


/*---------------------*
 *  class declaration  *
 *---------------------*/

class DiRGBImage : public DiColorImage
{
 public:
    DiRGBImage(const DiDocument *, const EI_Status);
    virtual ~DiRGBImage();
};


#endif
