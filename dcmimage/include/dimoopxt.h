/*
**
** Author:  Joerg Riesmeier
** Created: 19.12.96
**
** Module:  dimoopxt.h
**
** Purpose: DicomMonoOutputPixelTemplate (Header)
**
** Last Update:      $Author: joergr $
** Update Date:      $Date: 1998-05-11 14:53:22 $
** Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmimage/include/Attic/dimoopxt.h,v $
** CVS/RCS Revision: $Revision: 1.5 $
** Status:           $State: Exp $
**
** CVS/RCS Log at end of file
**
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

template<class T1, class T2>
class DiMonoOutputPixelTemplate : public DiMonoOutputPixel, public DiPixelRepresentationTemplate<T2>
{
 public:
    DiMonoOutputPixelTemplate(const DiMonoPixel *pixel, const DiOverlay *overlays, const DiLookupTable *table,
        const double center, const double width, const int bits, const Uint32 low, const Uint32 high,
        const Uint16 columns, const Uint16 rows, const unsigned long frame, const unsigned long frames)
      : DiMonoOutputPixel(pixel, frames),
        Data(NULL)
    {
        if ((pixel != NULL) && (getCount() > 0))
        {
            if ((table != NULL) && (table->isValid()))
                voilut((const T1 *)pixel->getData(), frame * getCount(), table, (T2)low, (T2)high);
            else
            {
                if (width == 0)                                                                 // no valid window
                    window((const T1 *)pixel->getData(), frame * getCount(), bits, (T2)low, (T2)high);
                else
                    window((const T1 *)pixel->getData(), frame * getCount(), center, width, (T2)low, (T2)high);
            }
            overlay(overlays, columns, rows, frame);
        }
    }

    virtual ~DiMonoOutputPixelTemplate()
    {
        delete Data;
    }

    inline EP_Representation getRepresentation() const
    {
        return DiPixelRepresentationTemplate<T2>::getRepresentation();
    }

    inline size_t getItemSize() const
    {
        return sizeof(T2);
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
    inline void voilut(const T1 *pixel, const Uint32 start, const DiLookupTable *table, const T2 low, const T2 high)
    {                                                                           // UNTESTED !
        if ((pixel != NULL) && (table != NULL))
        {
            Data = new T2[getCount()];
            if (Data != NULL)
            {
                const T1 min = (T1)table->getFirstEntry();
                const T1 max = (T1)table->getLastEntry();
                const T2 minvalue = (T2)table->getMinValue();
                const T2 maxvalue = (T2)table->getMaxValue();
                const double gradient = ((double)high - (double)low) / ((double)maxvalue - (double)minvalue);
                register const T1 *p = pixel + start;
                register T2 *q = Data;
                register T1 value;
                register unsigned long i;
                for (i = 0; i < getCount(); i++)
                {
                    value = *(p++);
                    if (value <= min)
                        *(q++) = low;
                    else if (value >= max)
                        *(q++) = high;
                    else
                        *(q++) = (T2)(low + ((double)table->getValue(value) - (double)minvalue) * gradient);
                }
            } 
        }
    }

    inline void window(const T1 *pixel, const Uint32 start, const int bits, const T2 low, const T2 high)
    {
        if (pixel != NULL)
        {
            Data = new T2[getCount()];
            if (Data != NULL)
            {
                register const T1 *p = pixel + start;
                register T2 *q = Data;
                const DiPixelRepresentationTemplate<T1> rep;
                const double offset = (rep.isSigned()) ? (double)(maxval(bits - 1, 0)) : 0;
                register unsigned long i;
                register const double gradient = ((double)high - (double)low + 1) / (double)maxval(bits, 0);
                for (i = 0; i < getCount(); i++)
                    *(q++) = (T2)((double)low + (offset + (double)(*(p++))) * gradient);
            }
        }
    }

    inline void window(const T1 *pixel, const Uint32 start, const double center, const double width, const T2 low,
        const T2 high)
    {
        if (pixel != NULL)
        {
            Data = new T2[getCount()];
            if (Data != NULL)
            {
                register const T1 *p = pixel + start;
                register T2 *q = Data;
                register double value;
                register unsigned long i;
                const double left = center - width / 2;
                const double right = center + width / 2;
                const double gradient = ((double)high - (double)low) / width;
                for (i = 0; i < getCount(); i++)
                {
                    value = (double)*(p++);
                    if (value <= left)
                        *(q++) = low;
                    else if (value >= right)
                        *(q++) = high;
                    else
                        *(q++) = (T2)((double)low + (value - left) * gradient);
                } 
            }   
        }
    }

    inline void overlay(const DiOverlay *overlays, const Uint16 columns, const Uint16 rows, const unsigned long frame)
    {
        if ((Data != NULL) && (overlays != NULL))
        {
            const Uint16 left = overlays->getLeft();
            const Uint16 top = overlays->getTop();
            register DiOverlayPlane *plane;
            for (unsigned int i = 0; i < overlays->getCount(); i++)
            {
                plane = overlays->getPlane(i);
                if ((plane != NULL) && plane->isVisible() && plane->reset(frame))
                {
                    register T2 *q;
                    register Uint16 x;
                    register Uint16 y;
                    const Uint16 xmin = (plane->getLeft(left) > 0) ? plane->getLeft(left) : 0;
                    const Uint16 ymin = (plane->getTop(top) > 0) ? plane->getTop(top) : 0;
                    const Uint16 xmax = (plane->getRight(left) < columns) ? plane->getRight(left) : columns;
                    const Uint16 ymax = (plane->getBottom(top) < rows) ? plane->getBottom(top) : rows;
                    const T2 maxvalue = (T2)maxval(bitsof(T2));
                    switch (plane->getMode())
                    {
                        case EMO_Replace:
                        {
                            const T2 fore = (T2)(plane->getForeground() * maxvalue);
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
                            const T2 fore = (T2)(plane->getForeground() * maxvalue);
                            const T2 thresh = (T2)(plane->getThreshold() * maxvalue);
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
                            const T2 thresh = (T2)maxval(bitsof(T2) / 2);
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
                            const int dim = bitsof(T2) / 2;
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
                            cerr << "WARNING: unhandled overlay mode (" << (int)plane->getMode() << ") !" << endl;
                    }
                } 
            }
        }
    }

    T2 *Data;
};


#endif


/*
**
** CVS/RCS Log:
** $Log: dimoopxt.h,v $
** Revision 1.5  1998-05-11 14:53:22  joergr
** Added CVS/RCS header to each file.
**
**
*/
