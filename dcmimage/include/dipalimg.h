/*********************************************************************
** 
**  DicomPaletteImage (Header)
**
**  author   : Joerg Riesmeier
**  created  : 21.12.96
**  modified : 13.02.97
**
*********************************************************************/


#ifndef __DIPALIMG_H
#define __DIPALIMG_H

#include "osconfig.h"

#include "dicoimg.h"


/*---------------------*
 *  class declaration  *
 *---------------------*/

class DiPaletteImage : public DiColorImage {

 public:
    DiPaletteImage(const DiDocument *, const EI_Status);
    virtual ~DiPaletteImage();
};


#endif
