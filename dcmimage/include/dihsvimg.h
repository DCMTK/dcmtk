/*********************************************************************
** 
**  DicomHSVImage (Header)
**
**  author   : Joerg Riesmeier
**  created  : 12.01.97
**  modified : 13.02.97
**
*********************************************************************/


#ifndef __DIHSVIMG_H
#define __DIHSVIMG_H

#include "osconfig.h"

#include "dicoimg.h"


/*---------------------*
 *  class declaration  *
 *---------------------*/

class DiHSVImage : public DiColorImage
{
 public:
    DiHSVImage(const DiDocument *, const EI_Status);
    virtual ~DiHSVImage();
};


#endif
