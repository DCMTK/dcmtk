/*********************************************************************
** 
**  DicomInputPixel (Header)
**
**  author   : Joerg Riesmeier
**  created  : 10.12.96
**  modified : 13.01.97
**
*********************************************************************/


#ifndef __DIINPX_H
#define __DIINPX_H

#include "osconfig.h"
#include "dctypes.h"

#include "diutils.h"


/*---------------------*
 *  class declaration  *
 *---------------------*/

class DiInputPixel 
{
 public:
    DiInputPixel();
    virtual ~DiInputPixel();
    
    virtual int determineMinMax() = 0;
    
    virtual EP_Representation getRepresentation() const = 0;
    virtual void *getData() const = 0;
    virtual double getMinValue() const = 0;
    virtual double getMaxValue() const = 0;
    
    inline unsigned long getCount() const
        { return Count; };
    
 protected:
    unsigned long Count;
};


#endif
