/*
 *
 *  Copyright (C) 1996-99, OFFIS
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
 *  Purpose: DicomMonochromePixelTemplate (Header)
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 1998-11-27 15:36:43 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmimgle/include/Attic/dimopxt.h,v $
 *  CVS/RCS Revision: $Revision: 1.1 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#ifndef __DIMOPXT_H
#define __DIMOPXT_H

#include "osconfig.h"
#include "dctypes.h"
#include "dcdefine.h"
#include "ofbmanip.h"

#include "dimopx.h"
#include "dipxrept.h"
#include "dimomod.h"
#include "diinpx.h"


/*---------------------*
 *  class declaration  *
 *---------------------*/

template<class T>
class DiMonoPixelTemplate : public DiMonoPixel, public DiPixelRepresentationTemplate<T>
{
 public:
    inline DiMonoPixelTemplate(const unsigned long count)
      : DiMonoPixel(count),
        Data(NULL)
    {
        MinValue[0] = 0;
        MinValue[1] = 0;
        MaxValue[0] = 0;
        MaxValue[1] = 0;
        Data = new T[getCount()];
        if (Data != NULL)
            OFBitmanipTemplate<T>::zeroMem(Data, getCount());
    }

    DiMonoPixelTemplate(const DiInputPixel *pixel, DiMonoModality *modality)
      : DiMonoPixel(pixel, modality),
        Data(NULL)
    {
        MinValue[0] = 0;
        MinValue[1] = 0;
        MaxValue[0] = 0;
        MaxValue[1] = 0;
    }

    virtual ~DiMonoPixelTemplate()
    {
        delete[] Data;
    }

    inline EP_Representation getRepresentation() const
    {
        return DiPixelRepresentationTemplate<T>::getRepresentation();
    }

    inline void *getData() const
    {
        return (void *)Data;
    }

    inline void *getDataPtr()
    {
        return (void *)(&Data);
    }
    
    inline int getMinMaxValues(double &min, double &max) const
    { 
        min = MinValue[0];
        max = MaxValue[0];
        return 1; 
    }

    inline int getMinMaxWindow(const int idx, double &center, double &width)
    {
        int result = 0; 
        if ((idx >= 0) && (idx <= 1))
        { 
            center = ((double)MinValue[idx] + (double)MaxValue[idx]) / 2;       // type cast to avoid overflows !
            width = (double)MaxValue[idx] - (double)MinValue[idx];
            result = (width > 0);
        }
        return result;
    }

    int getHistogramWindow(const double thresh, double &center, double &width)
    {
        if ((Data != NULL) && (MinValue[0] < MaxValue[0]))
        {
            const Uint32 count = (Uint32)(MaxValue[0] - MinValue[0] + 1);
            Uint32 *quant = new Uint32[count];
            if (quant != NULL)
            {
                register unsigned long i;
                OFBitmanipTemplate<Uint32>::zeroMem(quant, count);                  // initialize array
                for (i = 0; i < getCount(); i++)
                {
                    if ((Data[i] >= MinValue[0]) && (Data[i] <= MaxValue[0]))       // only for stability !
                        quant[(Uint32)(Data[i] - MinValue[0])]++;                   // count values
                    else if (DicomImageClass::DebugLevel >= DicomImageClass::DL_Warnings)
                    {
                        cerr << "WARNING: invalid value (" << Data[i] << ") in ";
                        cerr << "int DiMonoPixelTemplate<T>::getHistogramWindow() ! " << endl;
                    }
                }
                const Uint32 threshvalue = (Uint32)(thresh * (double)getCount());
                register Uint32 t = 0;
                i = 0;
                while ((i < count) && (t < threshvalue))
                    t += quant[i++];
                const T minvalue = (i < count) ? (T)(MinValue[0] + i) : 0;
                t = 0;
                i = count;
                while ((i > 0) && (t < threshvalue))
                    t += quant[--i];
                const T maxvalue = (i > 0) ? (T)(MinValue[0] + i) : 0;
                delete[] quant;
                if (minvalue < maxvalue)
                {
                    center = ((double)minvalue + (double)maxvalue) / 2;
                    width = (double)maxvalue - (double)minvalue;
                    return (width > 0);
                }
            }
        }
        return 0;
    }

 protected:
    DiMonoPixelTemplate(const DiPixel *pixel)
      : DiMonoPixel(pixel),
        Data(NULL)
    {
        MinValue[0] = 0;
        MinValue[1] = 0;
        MaxValue[0] = 0;
        MaxValue[1] = 0;
    }

    DiMonoPixelTemplate(const DiMonoPixel *pixel, const unsigned long count)
      : DiMonoPixel(pixel, count),
        Data(NULL)
    {
        MinValue[0] = 0;
        MinValue[1] = 0;
        MaxValue[0] = 0;
        MaxValue[1] = 0;
    }

    void determineMinMax(T minvalue = 0, T maxvalue = 0)
    {
        if (Data != NULL)
        {
            if ((minvalue == 0) && (maxvalue == 0))
            {
                register T *p = Data;
                register T value = *p;
                register unsigned long i;
                minvalue = value;
                maxvalue = value;
                for (i = 1; i < getCount(); i++)
                {
                    value = *(++p);
                    if (value < minvalue)
                        minvalue = value;
                    if (value > maxvalue)
                        maxvalue = value;
                }
            }
            MinValue[0] = minvalue;                         // global minimum
            MaxValue[0] = maxvalue;                         // global maximum
            register T *p = Data;
            register T value;
            register int firstmin = 1;
            register int firstmax = 1;
            register unsigned long i;
            for (i = 0; i < getCount(); i++)
            {
                value = *(p++);
                if ((value > minvalue) && ((value < MinValue[1]) || firstmin))
                {
                    MinValue[1] = value;
                    firstmin = 0;
                }
                if ((value < maxvalue) && ((value > MaxValue[1]) || firstmax))
                {
                    MaxValue[1] = value;
                    firstmax = 0;
                }
            }
        }
    }

    T *Data;
    
 private:
    T MinValue[2];
    T MaxValue[2];

 // --- declarations to avoid compiler warnings
 
    DiMonoPixelTemplate(const DiMonoPixelTemplate &);
    DiMonoPixelTemplate &operator=(const DiMonoPixelTemplate &);
};


#endif




/*
**
** CVS/RCS Log:
** $Log: dimopxt.h,v $
** Revision 1.1  1998-11-27 15:36:43  joergr
** Added copyright message.
** Replaced delete by delete[] for array types.
** Added method to give direct (non-const) access to internal data buffer.
** Added support for new bit manipulation class.
**
** Revision 1.7  1998/07/01 08:39:25  joergr
** Minor changes to avoid compiler warnings (gcc 2.8.1 with additional
** options), e.g. add copy constructors.
**
** Revision 1.6  1998/05/11 14:53:23  joergr
** Added CVS/RCS header to each file.
**
**
*/
