/*********************************************************************
** 
**  DicomColorPixel (Header)
**
**  author   : Joerg Riesmeier
**  created  : 20.12.96
**  modified : 13.03.98
**
*********************************************************************/


#ifndef __DICOPX_H
#define __DICOPX_H

#include "osconfig.h"

#include "diutils.h"


/*------------------------*
 *  forward declarations  *
 *------------------------*/

class DiDocument;
class DiInputPixel;
class DiMonoPixel;
class DiColorPixel;


/*---------------------*
 *  class declaration  *
 *---------------------*/


class DiColorPixel 
{
 public:
    DiColorPixel(const DiDocument *, const DiInputPixel *, const Uint16, EI_Status &, const Uint16 = 0);
    virtual ~DiColorPixel();
    
    virtual EP_Representation getRepresentation() const = 0;
    virtual void *getData() const = 0;
    virtual void *createDIB(const Uint16, const Uint16, const unsigned long, Sint16) const = 0;
    
    inline unsigned long getCount() const
        { return Count; };

 protected:
    DiColorPixel(const DiMonoPixel *);
    DiColorPixel(const DiColorPixel *, const unsigned long);

    int PlanarConfiguration;

 private:
    unsigned long Count;
};


#endif
