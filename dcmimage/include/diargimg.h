/*********************************************************************
** 
**  DicomARGBImage (Header)
**
**  author   : Joerg Riesmeier
**  created  : 06.01.97
**  modified : 13.02.97
**
*********************************************************************/


#ifndef __DIARGIMG_H
#define __DIARGIMG_H

#include "osconfig.h"

#include "dicoimg.h"


/*---------------------*
 *  class declaration  *
 *---------------------*/

class DiARGBImage : public DiColorImage {

 public:
    DiARGBImage(const DiDocument *, const EI_Status);
    virtual ~DiARGBImage();
};


#endif
