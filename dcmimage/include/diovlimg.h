/*********************************************************************
** 
**  DicomOverlayImage (Header)
**
**  author   : Joerg Riesmeier
**  created  : 14.01.97
**  modified : 13.02.97
**
*********************************************************************/


#ifndef __DIOVLIMG_H
#define __DIOVLIMG_H

#include "osconfig.h"

#include "dimo2img.h"


/*---------------------*
 *  class declaration  *
 *---------------------*/

class DiOverlayImage : public DiMono2Image
{
 public:
    DiOverlayImage(const DiDocument *, const EI_Status);
    virtual ~DiOverlayImage();
};


#endif
