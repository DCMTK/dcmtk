/*********************************************************************
** 
**  DicomMonochrome2Image (Header)
**
**  author   : Joerg Riesmeier
**  created  : 16.12.96
**  modified : 13.02.97
**
*********************************************************************/


#ifndef __DIMO2IMG_H
#define __DIMO2IMG_H

#include "osconfig.h"
#include "dctypes.h"

#include "dimoimg.h"


/*---------------------*
 *  class declaration  *
 *---------------------*/

class DiMono2Image : public DiMonoImage
{
 public:
    DiMono2Image(const DiDocument *, const EI_Status);
    DiMono2Image(const DiColorImage *, const double, const double, const double);
    virtual ~DiMono2Image();

	void *getOutputData(const unsigned long, const int, const int = 0);

	DiImage *scale(const unsigned long, const unsigned long, const int) const;
	DiImage *clip(const unsigned long, const unsigned long, const unsigned long, const unsigned long) const;
	
	DiImage *makeMonochrome(const double, const double, const double) const;

 protected:
    DiMono2Image(const DiDocument *, const EI_Status, const int);
    DiMono2Image(const DiMonoImage *, const Uint16, const Uint16, const int = 0);
    DiMono2Image(const DiMonoImage *, const Uint16, const Uint16, const Uint16, const Uint16);
};


#endif
