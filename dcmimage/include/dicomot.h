/*
**
** Author:  Joerg Riesmeier
** Created: 05.01.97
**
** Module:  dicomot.h
**
** Purpose: DicomColorMonochromeTemplate (Header)
**
** Last Update:         $Author: joergr $
** Update Date:         $Date: 1998-05-11 14:53:11 $
** Source File:         $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmimage/include/Attic/dicomot.h,v $
** CVS/RCS Revision:    $Revision: 1.3 $
** Status:              $State: Exp $
**
** CVS/RCS Log at end of file
**
*/


#ifndef __DICOMOT_H
#define __DICOMOT_H

#include "osconfig.h"

#include "dimopxt.h"
#include "dicopx.h"


/*---------------------*
 *  class declaration  *
 *---------------------*/

template<class T>
class DiColorMonoTemplate : public DiMonoPixelTemplate<T>
{
 public:
    DiColorMonoTemplate(const DiColorPixel *pixel, const double red, const double green, const double blue)
      : DiMonoPixelTemplate<T>(pixel)
    {
        if ((pixel != NULL) && (pixel->getCount() > 0))
        {
            convert((const T **)pixel->getData(), red, green, blue);
            determineMinMax();
        }
    }
    
    virtual ~DiColorMonoTemplate()
    {
    }

 private:
    inline void convert(const T *pixel[3], const double red, const double green, const double blue)
    {
        if (pixel != NULL)
        {
            Data = new T[getCount()];
            if (Data != NULL)
            {
                register const T *r = pixel[0];
                register const T *g = pixel[1];
                register const T *b = pixel[2];
                register T *q = Data;
                register unsigned long i;
                for (i = 0; i < getCount(); i++)
                    *(q++) = (T)((double)*(r++) * red + (double)*(g++) * green + (double)*(b++) * blue);
            }
        }
    }

};


#endif


/*
**
** CVS/RCS Log:
** $Log: dicomot.h,v $
** Revision 1.3  1998-05-11 14:53:11  joergr
** Added CVS/RCS header to each file.
**
**
*/
