/*********************************************************************
** 
**  DicomYBRImage (Header)
**
**  author   : Joerg Riesmeier
**  created  : 12.03.98
**  modified : 12.03.98
**
*********************************************************************/


#ifndef __DIYBRIMG_H
#define __DIYBRIMG_H

#include "osconfig.h"

#include "dicoimg.h"


/*---------------------*
 *  class declaration  *
 *---------------------*/

class DiYBRImage : public DiColorImage
{
 public:
    DiYBRImage(const DiDocument *, const EI_Status);
    virtual ~DiYBRImage();
};


#endif
