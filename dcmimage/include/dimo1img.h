/*********************************************************************
** 
**  DicomMonochrome1Image (Header)
**
**  author   : Joerg Riesmeier
**  created  : 16.12.96
**  modified : 13.02.97
**
*********************************************************************/


#ifndef __DIMO1IMG_H
#define __DIMO1IMG_H

#include "osconfig.h"
#include "dctypes.h"

#include "dimoimg.h"


/*---------------------*
 *  class declaration  *
 *---------------------*/

class DiMono1Image : public DiMonoImage
{
 public:
    DiMono1Image(const DiDocument *, const EI_Status);
    virtual ~DiMono1Image();

	void *getOutputData(const unsigned long, const int, const int = 0);

	DiImage *scale(const unsigned long, const unsigned long, const int) const;
	DiImage *clip(const unsigned long, const unsigned long, const unsigned long, const unsigned long) const;

	DiImage *makeMonochrome(const double, const double, const double) const;
	
 protected:
    DiMono1Image(const DiMonoImage *, const Uint16, const Uint16, const int = 0);
    DiMono1Image(const DiMonoImage *, const Uint16, const Uint16, const Uint16, const Uint16);
};


#endif
