/*********************************************************************
** 
**  DicomYBRPart422Image (Header)
**
**  author   : Joerg Riesmeier
**  created  : 13.03.98
**  modified : 13.03.98
**
*********************************************************************/


#ifndef __DIYP2IMG_H
#define __DIYP2IMG_H

#include "osconfig.h"

#include "dicoimg.h"


/*---------------------*
 *  class declaration  *
 *---------------------*/

class DiYBRPart422Image : public DiColorImage
{
 public:
    DiYBRPart422Image(const DiDocument *, const EI_Status);
    virtual ~DiYBRPart422Image();
};


#endif
