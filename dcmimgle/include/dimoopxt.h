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
 *  Purpose: DicomMonoOutputPixelTemplate (Header)
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 1998-12-14 17:25:55 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmimgle/include/Attic/dimoopxt.h,v $
 *  CVS/RCS Revision: $Revision: 1.2 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#ifndef __DIMOOPXT_H
#define __DIMOOPXT_H

#include "osconfig.h"
#include "dctypes.h"

#include "dimoopx.h"
#include "diluptab.h"
#include "diovlay.h"
#include "dipxrept.h"
#include "diutils.h"


/*---------------------*
 *  class declaration  *
 *---------------------*/

template<class T1, class T2, class T3>
class DiMonoOutputPixelTemplate
  : public DiMonoOutputPixel,
    public DiPixelRepresentationTemplate<T3>
{

 public:

    DiMonoOutputPixelTemplate(const DiMonoPixel *pixel,
                              DiOverlay *overlays[2],
                              const DiLookupTable *vlut,
                              const DiLookupTable *plut,
                              const double center,
                              const double width,
                              const int bits,
                              const Uint32 low,
                              const Uint32 high,
                              const Uint16 columns,
                              const Uint16 rows,
                              const unsigned long frame,
                              const unsigned long frames)
      : DiMonoOutputPixel(pixel, frames),
        Data(NULL)
    {
        if ((pixel != NULL) && (getCount() > 0))
        {
            if ((vlut != NULL) && (vlut->isValid()))
                voilut((const T1 *)pixel->getData(), frame * getCount(), vlut, plut, (T3)low, (T3)high);
            else
            {
                if (width <= 0)                            // no valid window according to Cor Loef (author of suppl. 33)
                    window(pixel, frame * getCount(), plut, bits, (T3)low, (T3)high);
                else
                    window(pixel, frame * getCount(), plut, center, width, (T3)low, (T3)high);
            }
            overlay(overlays, columns, rows, frame);
        }
    }

    virtual ~DiMonoOutputPixelTemplate()
    {
        delete[] Data;
    }

    inline EP_Representation getRepresentation() const
    {
        return DiPixelRepresentationTemplate<T3>::getRepresentation();
    }

    inline size_t getItemSize() const
    {
        return sizeof(T3);
    }

    inline void *getData() const
    {
        return (void *)Data;
    }

    inline int writePPM(ostream &stream) const
    {
        if (Data != NULL)
        {
            register unsigned long i;
            for (i = 0; i < getCount(); i++)
                stream << (unsigned long)Data[i] << " ";        // typecast to resolve problems with 'char'
            return 1;
        }
        return 0;
    }

    inline int writePPM(FILE *stream) const
    {
        if (Data != NULL)
        {
            register unsigned long i;
            for (i = 0; i < getCount(); i++)
                fprintf(stream, "%lu ", (unsigned long)Data[i]);
            return 1;
        }
        return 0;
    }


 private:

    inline void voilut(const T1 *pixel,
                       const Uint32 start,
                       const DiLookupTable *vlut,
                       const DiLookupTable *plut,
                       const T3 low,
                       const T3 high)
    {
        if ((pixel != NULL) && (vlut != NULL))
        {
            Data = new T3[getCount()];
            if (Data != NULL)
            {
                const double minvalue = (double)vlut->getMinValue();
                const double maxvalue = (double)vlut->getMaxValue();
                register unsigned long i;
                if (minvalue == maxvalue)                                             // LUT has only one entry or all entries are equal
                {
                    const T3 value = (T3)((double)low + (minvalue / (double)maxval(vlut->getBits(), 0)) * ((double)high - (double)low + 1));
                    OFBitmanipTemplate<T3>::setMem(Data, value, getCount());          // set output pixels to LUT value
                        
                    /* presentation LUT ? */
                    
                } else {
                    register T2 value;
                    const T2 firstentry = vlut->getFirstEntry(value);                 // choose signed/unsigned method
                    const T2 lastentry = vlut->getLastEntry(value);
                    const double gradient = ((double)high - (double)low + 1) / (maxvalue - minvalue + 1);                    
                    const T3 firstvalue = (T3)((double)low + ((double)vlut->getFirstValue() - minvalue) * gradient);
                    const T3 lastvalue = (T3)((double)low + ((double)vlut->getLastValue() - minvalue) * gradient);
                    register const T1 *p = pixel + start;
                    register T3 *q = Data;
                    if ((plut != NULL) && (plut->isValid()))                          // has presentation LUT
                    {
                        
                        /* merge both tables ? */
                        
                    } else {
                        for (i = 0; i < getCount(); i++)
                        {
                            value = (T2)(*(p++));
                            if (value <= firstentry)
                                *(q++) = firstvalue;
                            else if (value >= lastentry)
                                *(q++) = lastvalue;
                            else
                                *(q++) = (T3)((double)low + ((double)vlut->getValue(value) - minvalue) * gradient);
                        }
                    }
                }
            } 
        }
    }

    inline void window(const DiMonoPixel *inter,
                       const Uint32 start,
                       const DiLookupTable *plut,
                       const int bits,
                       const T3 low,
                       const T3 high)
    {
        const T1 *pixel = (const T1 *)inter->getData();
        if (pixel != NULL)
        {
            Data = new T3[getCount()];
            if (Data != NULL)
            {
                const DiPixelRepresentationTemplate<T1> rep;
                const double offset = (rep.isSigned()) ? maxval(bits - 1, 0) : 0;
                register const T1 *p = pixel + start;
                register T3 *q = Data;
                register unsigned long i;
                if ((plut != NULL) && (plut->isValid()))                              // has presentation LUT
                {
/*
                    const Uint16 lastentry = (Uint16)plut->getLastEntry();
                    const double minentry = (double)plut->getMinValue();
                    const double maxentry = (double)plut->getMaxValue();
                    const double gradient1 = ((double)high - (double)low + 1) / (double)maxval(plut->getBits(), 0);
                    const double gradient2 = ((double)high - (double)low + 1) / (maxentry - minentry);
                    register Uint16 value;
                    for (i = 0; i < getCount(); i++)
                    {
                        value = (Uint16)((offset + (double)(*(p++))) * gradient1);     // scale to LUT input range
                        if (value >= lastentry)
                            *(q++) = high;
                        else
                            *(q++) = (T3)((double)low + ((double)plut->getValue(value) - minentry) * gradient2);
                    }
*/
                } else {            
                    register const double gradient = ((double)high - (double)low + 1) / ((double)maxval(bits) + 1);
                    for (i = 0; i < getCount(); i++)
                        *(q++) = (T3)((double)low + (offset + (double)(*(p++))) * gradient);
                }
            }
        }
    }

    inline void window(const DiMonoPixel *inter,
                       const Uint32 start,
                       const DiLookupTable *plut,
                       const double center,
                       const double width,
                       const T3 low,
                       const T3 high)
    {
        const T1 *pixel = (const T1 *)inter->getData();
        if (pixel != NULL)
        {
            Data = new T3[getCount()];
            if (Data != NULL)
            {
                register const T1 *p = pixel + start;
                register T3 *q = Data;
                register unsigned long i;
                const double left = center - width / 2;
                const double right = center + width / 2;
                if ((plut != NULL) && (plut->isValid()))                               // has presentation LUT
                {
/*
                    const Uint16 lastentry = (Uint16)plut->getLastEntry();
                    const Uint16 min = 0;                                              // first value mapped
                    const Uint16 max = maxval(plut->getBits());                        // last value mapped?
                    const double gradient1 = ((double)max + 1) / width;
                    const double gradient2 = ((double)high - (double)low + 1) / ((double)max + 1);
                    register double value1;
                    register Uint16 value2;
                    for (i = 0; i < getCount(); i++)
                    {
                        value1 = (double)*(p++);                                       // input pixel
                        if (value1 <= left)
                            value2 = min;                                              // minimum window value
                        else if (value1 >= right)
                            value2 = max;                                              // maximum value
                        else
                            value2 = (Uint16)((value1 - left) * gradient1);            // scale to LUT input width
                        if (value2 >= lastentry)
                            *(q++) = high;                                             // maximum output value
                        else
                            *(q++) = (T3)((double)low + (double)plut->getValue(value2) * gradient2);
                    }
*/
                } else {
                    const double gradient = ((double)high - (double)low + 1) / width;
                    register double value;
                    for (i = 0; i < getCount(); i++)
                    {
                        value = (double)*(p++);
                        if (value <= left)
                            *(q++) = low;
                        else if (value >= right)
                            *(q++) = high;
                        else
                            *(q++) = (T3)((double)low + (value - left) * gradient);
                    }
                } 
            }   
        }
    }

    inline void overlay(DiOverlay *overlays[2],
                        const Uint16 columns,
                        const Uint16 rows,
                        const unsigned long frame)
    {
        if ((Data != NULL) && (overlays != NULL) && (overlays[0] != NULL))
        {
            const Uint16 left = overlays[0]->getLeft();
            const Uint16 top = overlays[0]->getTop();
            register DiOverlayPlane *plane;
            for (unsigned int i = 0; i < overlays[0]->getCount(); i++)
            {
                plane = overlays[0]->getPlane(i);
                if ((plane != NULL) && plane->isVisible() && plane->reset(frame))
                {
                    register T3 *q;
                    register Uint16 x;
                    register Uint16 y;
                    const Uint16 xmin = (plane->getLeft(left) > 0) ? plane->getLeft(left) : 0;
                    const Uint16 ymin = (plane->getTop(top) > 0) ? plane->getTop(top) : 0;
                    const Uint16 xmax = (plane->getRight(left) < columns) ? plane->getRight(left) : columns;
                    const Uint16 ymax = (plane->getBottom(top) < rows) ? plane->getBottom(top) : rows;
                    const T3 maxvalue = (T3)maxval(bitsof(T3));
                    switch (plane->getMode())
                    {
                        case EMO_Replace:
                        {
                            const T3 fore = (T3)(plane->getForeground() * maxvalue);
                            for (y = ymin; y < ymax; y++)
                            {
                                plane->setStart(left + xmin, top + y);
                                q = Data + (unsigned long)y * (unsigned long)columns + (unsigned long)xmin;
                                for (x = xmin; x < xmax; x++, q++)
                                {
                                    if (plane->getNextBit())
                                        *q = fore;
                                }
                            }
                            break;
                        }
                        case EMO_ThresholdReplace:
                        {
                            const T3 fore = (T3)(plane->getForeground() * maxvalue);
                            const T3 thresh = (T3)(plane->getThreshold() * maxvalue);
                            for (y = ymin; y < ymax; y++)
                            {
                                plane->setStart(left + xmin, top + y);
                                q = Data + (unsigned long)y * (unsigned long)columns + (unsigned long)xmin;
                                for (x = xmin; x < xmax; x++, q++)
                                {
                                    if (plane->getNextBit())
                                        *q = (*q <= thresh) ? fore : 1;
                                }
                            }
                            break;
                        }
                        case EMO_Complement:
                        {
                            const T3 thresh = (T3)maxval(bitsof(T3) / 2);
                            for (y = ymin; y < ymax; y++)
                            {
                                plane->setStart(left + xmin, top + y);
                                q = Data + (unsigned long)y * (unsigned long)columns + (unsigned long)xmin;
                                for (x = xmin; x < xmax; x++, q++)
                                {
                                    if (plane->getNextBit())
                                        *q = (*q <= thresh) ? maxvalue : 0;
                                }
                            }
                            break;
                        }
                        case EMO_RegionOfInterest:
                        {
                            const int dim = bitsof(T3) / 2;
                            for (y = ymin; y < ymax; y++)
                            {
                                plane->setStart(left + xmin, top + y);
                                q = Data + (unsigned long)y * (unsigned long)columns + (unsigned long)xmin;
                                for (x = xmin; x < xmax; x++, q++)
                                {
                                    if (!plane->getNextBit())
                                        *q = *q >> dim;
                                }
                            }
                            break;
                        }
                        default: /* e.g. EMO_Default */
                            if (DicomImageClass::DebugLevel >= DicomImageClass::DL_Warnings)
                                cerr << "WARNING: unhandled overlay mode (" << (int)plane->getMode() << ") !" << endl;
                    }
                } 
            }
        }
    }

    T3 *Data;

 // --- declarations to avoid compiler warnings
 
    DiMonoOutputPixelTemplate(const DiMonoOutputPixelTemplate &);
    DiMonoOutputPixelTemplate &operator=(const DiMonoOutputPixelTemplate &);
};


#endif


/*
**
** CVS/RCS Log:
** $Log: dimoopxt.h,v $
** Revision 1.2  1998-12-14 17:25:55  joergr
** Added support for correct scaling of input/output values for grayscale
** transformations.
**
** Revision 1.1  1998/11/27 15:29:53  joergr
** Added copyright message.
** Introduced global debug level for dcmimage module to control error output.
** Corrected bug in VOI LUT transformation method.
** Changed behaviour: now window width of 0 is valid and negative width
** is invalid.
**
** Revision 1.6  1998/07/01 08:39:24  joergr
** Minor changes to avoid compiler warnings (gcc 2.8.1 with additional
** options), e.g. add copy constructors.
**
** Revision 1.5  1998/05/11 14:53:22  joergr
** Added CVS/RCS header to each file.
**
**
*/
