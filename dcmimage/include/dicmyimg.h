/*********************************************************************
** 
**  DicomCMYKImage (Header)
**
**  author   : Joerg Riesmeier
**  created  : 05.01.97
**  modified : 13.02.97
**
*********************************************************************/


#ifndef __DICMYIMG_H
#define __DICMYIMG_H

#include "osconfig.h"

#include "dicoimg.h"


/*---------------------*
 *  class declaration  *
 *---------------------*/

class DiCMYKImage : public DiColorImage
{
 public:
    DiCMYKImage(const DiDocument *, const EI_Status);
    virtual ~DiCMYKImage();
};


#endif
