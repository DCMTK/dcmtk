/*
 *
 *  Copyright (C) 1997-99, OFFIS
 *
 *  This software and supporting documentation were developed by
 *
 *    Kuratorium OFFIS e.V.
 *    Healthcare Information and Communication Systems
 *    Escherweg 2
 *    D-26121 Oldenburg, Germany
 *
 *  THIS SOFTWARE IS MADE AVAILABLE,  AS IS,  AND OFFIS MAKES NO  WARRANTY
 *  REGARDING  THE  SOFTWARE,  ITS  PERFORMANCE,  ITS  MERCHANTABILITY  OR
 *  FITNESS FOR ANY PARTICULAR USE, FREEDOM FROM ANY COMPUTER DISEASES  OR
 *  ITS CONFORMITY TO ANY SPECIFICATION. THE ENTIRE RISK AS TO QUALITY AND
 *  PERFORMANCE OF THE SOFTWARE IS WITH THE USER.
 *
 *  Module:  dcmimgle
 *
 *  Author:  Joerg Riesmeier
 *
 *  Purpose: DicomMonochromeInputPixelTemplate (Header)
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 1998-11-27 15:24:08 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmimgle/include/Attic/dimoipxt.h,v $
 *  CVS/RCS Revision: $Revision: 1.1 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#ifndef __DIMOIPXT_H
#define __DIMOIPXT_H

#include "osconfig.h"
#include "ofbmanip.h"

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
                tabular((const T1 *)pixel->getData());
            else if ((Modality != NULL) && Modality->hasRescaling())
                rescale((const T1 *)pixel->getData(), Modality->getRescaleSlope(), Modality->getRescaleIntercept());
            else
                rescale((const T1 *)pixel->getData(), 1.0, 0.0);
            determineMinMax((T2)pixel->getMinValue(), (T2)pixel->getMaxValue());
        }
    }

    virtual ~DiMonoInputPixelTemplate()
    {
    }

 private:
    inline void tabular(const T1 *pixel)
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

    inline void rescale(const T1 *pixel, const double slope, const double intercept)
    {
        if (pixel != NULL)
        {
            Data = new T2[getCount()];
            if (Data != NULL)
            {
                register const T1 *p = pixel;
                register T2 *q = Data;
                register unsigned long i;
                if (slope == 1.0) {
                    if (intercept == 0.0) {
                        OFBitmanipTemplate<T2>::copyMem((const T2 *)pixel, Data, getCount());
/*                        
                        for (i = 0; i < getCount(); i++)            // copy pixel data
                            *(q++) = (T2)*(p++);
*/                            
                    } else {
                        for (i = 0; i < getCount(); i++)
                            *(q++) = (T2)((double)*(p++) + intercept);
                    }
                } else {
                    if (intercept == 0.0) {
                        for (i = 0; i < getCount(); i++)
                            *(q++) = (T2)((double)*(p++) * slope);
                    } else {
                        for (i = 0; i < getCount(); i++)
                            *(q++) = (T2)((double)*(p++) * slope + intercept);
                    }
                }
            }
        }
    }

};


#endif


/*
**
** CVS/RCS Log:
** $Log: dimoipxt.h,v $
** Revision 1.1  1998-11-27 15:24:08  joergr
** Added copyright message.
** Added new cases to optimize rescaling.
** Added support for new bit manipulation class.
** Corrected bug in modality LUT transformation method.
**
** Revision 1.5  1998/07/01 08:39:23  joergr
** Minor changes to avoid compiler warnings (gcc 2.8.1 with additional
** options), e.g. add copy constructors.
**
** Revision 1.4  1998/05/11 14:53:21  joergr
** Added CVS/RCS header to each file.
**
**
*/
