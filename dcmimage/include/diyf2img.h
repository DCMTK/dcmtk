/*********************************************************************
** 
**  DicomYBR422Image (Header)
**
**  author   : Joerg Riesmeier
**  created  : 13.03.98
**  modified : 13.03.98
**
*********************************************************************/


#ifndef __DIYF2IMG_H
#define __DIYF2IMG_H

#include "osconfig.h"

#include "dicoimg.h"


/*---------------------*
 *  class declaration  *
 *---------------------*/

class DiYBR422Image : public DiColorImage
{
 public:
    DiYBR422Image(const DiDocument *, const EI_Status);
    virtual ~DiYBR422Image();
};


#endif
