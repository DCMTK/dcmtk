/*********************************************************************
** 
**  DicomMonochromePixel (Header)
**
**  author   : Joerg Riesmeier
**  created  : 16.12.96
**  modified : 07.02.97
**
*********************************************************************/


#ifndef __DIMOPX_H
#define __DIMOPX_H

#include "osconfig.h"
#include "dctypes.h"

#include "diutils.h"


/*------------------------*
 *  forward declarations  *
 *------------------------*/

class DiInputPixel;
class DiColorPixel;
class DiMonoModality;


/*---------------------*
 *  class declaration  *
 *---------------------*/

class DiMonoPixel 
{
 public:
    DiMonoPixel(const unsigned long);
    DiMonoPixel(const DiInputPixel *, DiMonoModality *);
    virtual ~DiMonoPixel();
    
	virtual EP_Representation getRepresentation() const = 0;
    virtual void *getData() const = 0;

	virtual int getMinMaxValues(double &, double &) const = 0;

	virtual int getMinMaxWindow(const int, double &, double &) = 0;
	virtual int getHistogramWindow(const double, double &, double &) = 0;
	
	inline unsigned long getCount() const
		{ return Count; };

 protected:
    DiMonoPixel(const DiColorPixel *);
    DiMonoPixel(const DiMonoPixel *, const unsigned long);
	
	DiMonoModality *Modality;

 private:
 	unsigned long Count;
};


#endif
