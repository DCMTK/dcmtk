/*
**
** Author:  Joerg Riesmeier
** Created: 03.01.97
**
** Module:  dimoipxt.h
**
** Purpose: DicomMonochromeInputPixelTemplate (Header)
**
** Last Update:      $Author: joergr $
** Update Date:      $Date: 1998-07-01 08:39:23 $
** Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmimage/include/Attic/dimoipxt.h,v $
** CVS/RCS Revision: $Revision: 1.5 $
** Status:           $State: Exp $
**
** CVS/RCS Log at end of file
**
*/


#ifndef __DIMOIPXT_H
#define __DIMOIPXT_H

#include "osconfig.h"

#include "dimopxt.h"


/*---------------------*
 *  class declaration  *
 *---------------------*/

template<class T1, class T2>
class DiMonoInputPixelTemplate : public DiMonoPixelTemplate<T2>
{
 public:
    DiMonoInputPixelTemplate(const DiInputPixel *pixel, DiMonoModality *modality)
      : DiMonoPixelTemplate<T2>(pixel, modality)
    {
        if ((pixel != NULL) && (getCount() > 0))
        {   
            if ((Modality != NULL) && Modality->hasLookupTable() && (bitsof(T1) <= MAX_TABLE_ENTRY_SIZE))
            {
                tabular((const T1 *)pixel->getData());
                determineMinMax((T2)Modality->getMinValue(), (T2)Modality->getMaxValue());
            }
            else if ((Modality != NULL) && Modality->hasRescaling() && !((Modality->getRescaleIntercept() == 0) &&
                (Modality->getRescaleSlope() == 1)))
            {
                rescale((const T1 *)pixel->getData());
                determineMinMax((T2)Modality->getMinValue(), (T2)Modality->getMaxValue());
            }
            else
            {
                copy((const T1 *)pixel->getData());
                determineMinMax((T2)pixel->getMinValue(), (T2)pixel->getMaxValue());
            }
        }
    }

    virtual ~DiMonoInputPixelTemplate()
    {
    }

 private:
    inline void tabular(const T1 *pixel)      // UNTESTED !
    {
        if ((pixel != NULL) && (Modality != NULL))
        {
            Data = new T2[getCount()];
            if (Data != NULL)
            {
                const DiLookupTable *table = Modality->getTableData();
                const T1 min = (T1)table->getFirstEntry();
                const T1 max = (T1)table->getLastEntry();
                const T2 minvalue = (T2)table->getFirstValue();
                const T2 maxvalue = (T2)table->getLastValue();
                register const T1 *p = pixel;
                register T2 *q = Data;
                register T1 value;
                register unsigned long i;
                for (i = 0; i < getCount(); i++)
                {
                    value = *(p++);
                    if (value <= min)
                        *(q++) = minvalue;
                    else if (value >= max)
                        *(q++) = maxvalue;
                    else
                        *(q++) = (T2)table->getValue(value);
                }
            } 
        }
    }

    inline void rescale(const T1 *pixel)
    {
        if ((pixel != NULL) && (Modality != NULL))
        {
            Data = new T2[getCount()];
            if (Data != NULL)
            {
                register const T1 *p = pixel;
                register T2 *q = Data;
                register unsigned long i;
                register const double intercept = (double)Modality->getRescaleIntercept();
                register const double slope = (double)Modality->getRescaleSlope();
                for (i = 0; i < getCount(); i++)
                    *(q++) = (T2)((double)*(p++) * slope + intercept);
            }
        }
    }

    inline void copy(const T1 *pixel)
    {
        if (pixel != NULL)
        {
            Data = new T2[getCount()];
            if (Data != NULL)
            {
                register const T1 *p = pixel;
                register T2 *q = Data;
                register unsigned long i;
                for (i = 0; i < getCount(); i++)
                    *(q++) = (T2)*(p++);
            }
        }
    }

};


#endif


/*
**
** CVS/RCS Log:
** $Log: dimoipxt.h,v $
** Revision 1.5  1998-07-01 08:39:23  joergr
** Minor changes to avoid compiler warnings (gcc 2.8.1 with additional
** options), e.g. add copy constructors.
**
** Revision 1.4  1998/05/11 14:53:21  joergr
** Added CVS/RCS header to each file.
**
**
*/
