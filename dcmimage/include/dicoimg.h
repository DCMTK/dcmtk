/*********************************************************************
** 
**  DicomColorImage (Header)
**
**  author   : Joerg Riesmeier
**  created  : 20.12.96
**  modified : 13.02.97
**
*********************************************************************/


#ifndef __DICOIMG_H
#define __DICOIMG_H

#include "osconfig.h"
#include "dctypes.h"

#include "diimage.h"
#include "diutils.h"


/*------------------------*
 *  forward declarations  *
 *------------------------*/

class DiMonoImage;
class DiColorPixel;
class DiColorOutputPixel;


/*---------------------*
 *  class declaration  *
 *---------------------*/

class DiColorImage : public DiImage {

 public:
    DiColorImage(const DiDocument *, const EI_Status);
    DiColorImage(const DiMonoImage *);
    virtual ~DiColorImage();

	void *getOutputData(const unsigned long, const int, const int = 0);
	void *getOutputPlane(const int) const;
	void deleteOutputData();
	
	DiImage *scale(const unsigned long, const unsigned long, const int) const;
	DiImage *clip(const unsigned long, const unsigned long, const unsigned long, const unsigned long) const;

	DiImage *makeMonochrome(const double, const double, const double) const;
	const DiColorPixel *getInterData() const
		{ return InterData; }

	void *createDIB(const unsigned long);

	int writePPM(ostream &, const unsigned long, const int); 
	int writePPM(FILE *, const unsigned long, const int); 
	int writeRawPPM(FILE *, const unsigned long, const int); 

 protected:
    DiColorImage(const DiColorImage *, const Uint16, const Uint16, const int = 0);
    DiColorImage(const DiColorImage *, const Uint16, const Uint16, const Uint16, const Uint16);

	void checkInterData();

 	DiColorPixel *InterData;

 private:
	DiColorOutputPixel *OutputData;
 };


#endif
