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
 *  Update Date:      $Date: 1999-01-20 15:11:05 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmimgle/include/Attic/dimoopxt.h,v $
 *  CVS/RCS Revision: $Revision: 1.5 $
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

    DiMonoOutputPixelTemplate(void *buffer,
                              const DiMonoPixel *pixel,
                              DiOverlay *overlays[2],
                              const DiLookupTable *vlut,
                              const DiLookupTable *plut,
                              const double center,
                              const double width,
                              const Uint32 low,
                              const Uint32 high,
                              const Uint16 columns,
                              const Uint16 rows,
                              const unsigned long frame,
                              const unsigned long frames)
      : DiMonoOutputPixel(pixel, frames),
        Data(NULL),
        DeleteData(buffer == NULL)
    {
        if ((pixel != NULL) && (Count > 0))
        {
            Data = (T3 *)buffer;
            if ((vlut != NULL) && (vlut->isValid()))       // valid VOI LUT ?
                voilut(pixel, frame * Count, vlut, plut, (T3)low, (T3)high);
            else
            {
                if (width <= 0)                            // no valid window according to Cor Loef (author of suppl. 33)
                    nowindow(pixel, frame * Count, plut, (T3)low, (T3)high);
                else
                    window(pixel, frame * Count, plut, center, width, (T3)low, (T3)high);
            }
            overlay(overlays, columns, rows, frame);       // add (visible) overlay planes to output bitmap
        }
    }

    virtual ~DiMonoOutputPixelTemplate()
    {
        if (DeleteData)
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
            for (i = 0; i < Count; i++)
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
            for (i = 0; i < Count; i++)
                fprintf(stream, "%lu ", (unsigned long)Data[i]);
            return 1;
        }
        return 0;
    }


 private:

    inline void voilut(const DiMonoPixel *inter,
                       const Uint32 start,
                       const DiLookupTable *vlut,
                       const DiLookupTable *plut,
                       const T3 low,
                       const T3 high)
    {
        const T1 *pixel = (const T1 *)inter->getData();
        if ((pixel != NULL) && (vlut != NULL))
        {
            if (Data == NULL)
                Data = new T3[Count];
            if (Data != NULL)
            {
                const double minvalue = vlut->getMinValue();
                const double outrange = (double)high - (double)low + 1;
                register unsigned long i;
                if (minvalue == vlut->getMaxValue())                                  // LUT has only one entry or all entries are equal
                {
                    T3 value;
                    if ((plut != NULL) && (plut->isValid()))                          // has presentation LUT
                    {
                        const Uint32 value2 = (Uint32)((minvalue / (double)vlut->getAbsMaxRange()) * plut->getCount());
                        value = (T3)((double)low + (double)plut->getValue(value2) * outrange / (double)plut->getAbsMaxRange());
                    }
                    else
                        value = (T3)((double)low + (minvalue / (double)vlut->getAbsMaxRange()) * outrange);
                    OFBitmanipTemplate<T3>::setMem(Data, value, Count);          // set output pixels to LUT value
                } else {
                    register T2 value;
                    const T2 firstentry = vlut->getFirstEntry(value);                 // choose signed/unsigned method
                    const T2 lastentry = vlut->getLastEntry(value);
                    register const T1 *p = pixel + start;
                    register T3 *q = Data;
                    if ((plut != NULL) && (plut->isValid()))                          // has presentation LUT
                    {
                        register Uint32 value2;                                       // presentation LUT is always unsigned
                        const Uint32 count = plut->getCount();
                        const double gradient1 = (double)count / (double)vlut->getAbsMaxRange();
                        const double gradient2 = outrange / (double)plut->getAbsMaxRange();
                        const Uint32 firstvalue = (Uint32)((double)vlut->getFirstValue() * gradient1);
                        const Uint32 lastvalue = (Uint32)((double)vlut->getLastValue() * gradient1);
                        for (i = 0; i < Count; i++)
                        {
                            value = (T2)(*(p++));                                     // pixel value                            
                            if (value <= firstentry)
                                value2 = firstvalue;
                            else if (value >= lastentry)
                                value2 = lastvalue;
                            else
                                value2 = (Uint32)((double)vlut->getValue(value) * gradient1);
                            *(q++) = (T3)((double)low + (double)plut->getValue(value2) * gradient2);
                        }
                    } else {                                                          // has no presentation LUT
                        const double gradient = outrange / (double)vlut->getAbsMaxRange();                    
                        const T3 firstvalue = (T3)((double)low + (double)vlut->getFirstValue() * gradient);
                        const T3 lastvalue = (T3)((double)low + (double)vlut->getLastValue() * gradient);
                        T3 *lut = NULL;
                        const unsigned long cnt = (unsigned long)inter->getAbsMaxRange(); // number of LUT entries
                        if ((sizeof(T1) <= 2) && (Count > 3 * cnt))                       // optimization criteria
                        {                                                                 // use LUT for optimization
                            lut = new T3[cnt];
                            if (lut != NULL)
                            {
                                if (DicomImageClass::DebugLevel >= DicomImageClass::DL_Informationals)
                                    cerr << "INFO: using optimized routine for 'voilut()'" << endl;
                                q = lut;
                                for (i = 0; i < cnt; i++)                                 // calculating LUT entries
                                {
                                    value = (T2)i;
                                    if (value <= firstentry)
                                        *(q++) = firstvalue;
                                    else if (value >= lastentry)
                                        *(q++) = lastvalue;
                                    else
                                        *(q++) = (T3)((double)low + (double)vlut->getValue(value) * gradient);
                                }
                                q = Data;
/*
                                const T2 absmin = (T2)inter->getAbsMinimum();
                                if (absmin == 0.0)
                                {                                                         // unsigned inter-representation
                                    for (i = 0; i < Count; i++)
                                        *(q++) = lut[*(p++)];
                                } else {                                                  // signed inter-representation
                                    for (i = 0; i < Count; i++)
                                        *(q++) = lut[*(p++) - absmin];
                                }
*/
                                const T3 *lut0 = lut - (T2)inter->getAbsMinimum();        // points to 'zero' entry
                                for (i = 0; i < Count; i++)
                                    *(q++) = *(lut0 + (*(p++)));
                            }
                        }
                        if (lut == NULL)                                                  // use "normal" transformation
                        {
                            for (i = 0; i < Count; i++)
                            {
                                value = (T2)(*(p++));
                                if (value <= firstentry)
                                    *(q++) = firstvalue;
                                else if (value >= lastentry)
                                    *(q++) = lastvalue;
                                else
                                    *(q++) = (T3)((double)low + (double)vlut->getValue(value) * gradient);
                            }
                        }
                    }
                }
            } 
        } else
            Data = NULL;
    }

    inline void nowindow(const DiMonoPixel *inter,
                         const Uint32 start,
                         const DiLookupTable *plut,
                         const T3 low,
                         const T3 high)
    {
        const T1 *pixel = (const T1 *)inter->getData();
        if (pixel != NULL)
        {
            if (Data == NULL)                                                         // create new output buffer
                Data = new T3[Count];
            if (Data != NULL)
            {
                const double absmin = inter->getAbsMinimum();
                const double outrange = (double)high - (double)low + 1;
                register const T1 *p = pixel + start;
                register T3 *q = Data;
                register unsigned long i;
                if ((plut != NULL) && (plut->isValid()))                              // has presentation LUT
                {
                    register Uint32 value;                                            // presentation LUT is always unsigned
                    const double gradient1 = (double)plut->getCount() / (inter->getAbsMaxRange());
                    const double gradient2 = outrange / (double)plut->getAbsMaxRange();
                    for (i = 0; i < Count; i++)
                    {
                        value = (Uint32)(((double)(*(p++)) - absmin) * gradient1);
                        *(q++) = (T3)((double)low + (double)plut->getValue(value) * gradient2);
                    }
                } else {                                                              // has no presentation LUT
                    register const double gradient = outrange / (inter->getAbsMaxRange());
                    T3 *lut = NULL;
                    const unsigned long cnt = (unsigned long)inter->getAbsMaxRange(); // number of LUT entries
                    if ((sizeof(T1) <= 2) && (Count > 3 * cnt))                       // optimization criteria
                    {                                                                 // use LUT for optimization
                        lut = new T3[cnt];
                        if (lut != NULL)
                        {
                            if (DicomImageClass::DebugLevel >= DicomImageClass::DL_Informationals)
                                cerr << "INFO: using optimized routine for 'nowindow()'" << endl;
                            q = lut;
                            for (i = 0; i < cnt; i++)                                 // calculating LUT entries
                                *(q++) = (T3)((double)low + (double)i * gradient);
                            q = Data;
/*
                            if (absmin == 0.0)
                            {                                                         // unsigned inter-representation
                                for (i = 0; i < Count; i++)
                                    *(q++) = lut[*(p++)];
                            } else {                                                  // signed inter-representation
                                for (i = 0; i < Count; i++)
                                    *(q++) = lut[*(p++) - (T2)absmin];
                            }
*/
                            const T3 *lut0 = lut - (T2)inter->getAbsMinimum();        // points to 'zero' entry
                            for (i = 0; i < Count; i++)
                                *(q++) = *(lut0 + (*(p++)));
                        }
                    }
                    if (lut == NULL)                                                  // use "normal" transformation
                    {
                        for (i = 0; i < Count; i++)
                            *(q++) = (T3)((double)low + ((double)(*(p++)) - absmin) * gradient);
                    } else
                        delete[] lut;
                }
            }
        } else
            Data = NULL;
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
            if (Data == NULL)
                Data = new T3[Count];
            if (Data != NULL)
            {
                const double left = center - width / 2;                                // window borders
                const double right = center + width / 2;
                const double outrange = (double)high - (double)low + 1;
                register const T1 *p = pixel + start;
                register T3 *q = Data;
                register unsigned long i;
                register double value;
                if ((plut != NULL) && (plut->isValid()))                               // has presentation LUT
                {
                    register Uint32 value2;                                            // presentation LUT is always unsigned
                    const Uint32 count = plut->getCount();
                    const double gradient1 = (double)count / width;
                    const double gradient2 = outrange / (double)plut->getAbsMaxRange();
                    for (i = 0; i < Count; i++)
                    {
                        value = (double)*(p++);                                        // pixel value
                        if (value <= left)
                            value2 = 0;                                                // first LUT index
                        else if (value >= right)
                            value2 = count - 1;                                        // last LUT index
                        else
                            value2 = (Uint32)((value - left) * gradient1);
                        *(q++) = (T3)((double)low + (double)plut->getValue(value2) * gradient2);
                    }
                } else {                                                               // has no presentation LUT
                    const double gradient = outrange / width;
                    T3 *lut = NULL;
                    const unsigned long cnt = (unsigned long)inter->getAbsMaxRange();  // number of LUT entries
                    if ((sizeof(T1) <= 2) && (Count > 3 * cnt))                        // optimization criteria
                    {                                                                  // use LUT for optimization
                        lut = new T3[cnt];
                        if (lut != NULL)
                        {
                            if (DicomImageClass::DebugLevel >= DicomImageClass::DL_Informationals)
                                cerr << "INFO: using optimized routine for 'window()'" << endl;

                            q = lut;
                            for (i = 0; i < cnt; i++)                                  // calculating LUT entries
                            {
                                value = (double)i;
                                if (value <= left)
                                    *(q++) = low;                                            // black/white
                                else if (value >= right)
                                    *(q++) = high;                                           // white/black
                                else
                                    *(q++) = (T3)((double)low + (value - left) * gradient);  // gray value
                            }
                            q = Data;
/*
                            const T2 absmin = (T2)inter->getAbsMinimum();
                            if (absmin == 0)
                            {                                                         // unsigned inter-representation
                                for (i = 0; i < Count; i++)
                                    *(q++) = lut[*(p++)];
                            } else {                                                  // signed inter-representation
                                for (i = 0; i < Count; i++)
                                    *(q++) = lut[*(p++) - absmin];
                            }
*/
                            const T3 *lut0 = lut - (T2)inter->getAbsMinimum();        // points to 'zero' entry
                            for (i = 0; i < Count; i++)
                                *(q++) = *(lut0 + (*(p++)));
                        }
                    }
                    if (lut == NULL)                                                  // use "normal" transformation
                    {
                        for (i = 0; i < Count; i++)
                        {
                            value = (double)*(p++);
                            if (value <= left)
                                *(q++) = low;                                            // black/white
                            else if (value >= right)
                                *(q++) = high;                                           // white/black
                            else
                                *(q++) = (T3)((double)low + (value - left) * gradient);  // gray value
                        }
                    } else
                        delete[] lut;
                } 
            }   
        } else
            Data = NULL;
    }

    inline void overlay(DiOverlay *overlays[2],                     // apply overlay planes to output data
                        const Uint16 columns,
                        const Uint16 rows,
                        const unsigned long frame)
    {
        if ((Data != NULL) && (overlays != NULL))
        {
            for (unsigned int j = 0; j < 2; j++)
            {
                if (overlays[j] != NULL)
                {
                    const Uint16 left = overlays[j]->getLeft();
                    const Uint16 top = overlays[j]->getTop();
                    register DiOverlayPlane *plane;
                    for (unsigned int i = 0; i < overlays[j]->getCount(); i++)
                    {
                        plane = overlays[j]->getPlane(i);
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
                                case EMO_BitmapShutter:  /* UNTESTED !! */
                                {
                                    const T3 fore = (T3)(plane->getForeground() * maxvalue);		// ???
                                    for (y = ymin; y < ymax; y++)
                                    {
                                        plane->setStart(left + xmin, top + y);
                                        q = Data + (unsigned long)y * (unsigned long)columns + (unsigned long)xmin;
                                        for (x = xmin; x < xmax; x++, q++)
                                        {
                                            if (!plane->getNextBit())
                                                *q = fore;
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
        }
    }

    T3 *Data;
    int DeleteData;

 // --- declarations to avoid compiler warnings
 
    DiMonoOutputPixelTemplate(const DiMonoOutputPixelTemplate &);
    DiMonoOutputPixelTemplate &operator=(const DiMonoOutputPixelTemplate &);
};


#endif


/*
 *
 * CVS/RCS Log:
 * $Log: dimoopxt.h,v $
 * Revision 1.5  1999-01-20 15:11:05  joergr
 * Replaced invocation of getCount() by member variable Count where possible.
 * Added new output method to fill external memory buffer with rendered pixel
 * data.
 * Added new overlay plane mode for bitmap shutters.
 * Added optimization to modality and VOI transformation (using additional
 * LUTs).
 *
 * Revision 1.4  1998/12/23 12:40:01  joergr
 * Removed unused parameter (BitsPerSample).
 *
 * Revision 1.3  1998/12/22 14:32:49  joergr
 * Improved implementation of presentation LUT application (and other gray
 * scale transformations). Tested with ECR test images from David Clunie.
 *
 * Revision 1.2  1998/12/14 17:25:55  joergr
 * Added support for correct scaling of input/output values for grayscale
 * transformations.
 *
 * Revision 1.1  1998/11/27 15:29:53  joergr
 * Added copyright message.
 * Introduced global debug level for dcmimage module to control error output.
 * Corrected bug in VOI LUT transformation method.
 * Changed behaviour: now window width of 0 is valid and negative width
 * is invalid.
 *
 * Revision 1.6  1998/07/01 08:39:24  joergr
 * Minor changes to avoid compiler warnings (gcc 2.8.1 with additional
 * options), e.g. add copy constructors.
 *
 * Revision 1.5  1998/05/11 14:53:22  joergr
 * Added CVS/RCS header to each file.
 *
 *
 */
