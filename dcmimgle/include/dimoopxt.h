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
 *  Update Date:      $Date: 1999-02-28 16:41:01 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmimgle/include/Attic/dimoopxt.h,v $
 *  CVS/RCS Revision: $Revision: 1.10 $
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
#include "didispfn.h"
#include "dibarlut.h"

//BEGIN_EXTERN_C
 #include <math.h>
//END_EXTERN_C


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
                              DiDisplayFunction *disp,
                              const double center,
                              const double width,
                              const Uint32 low,
                              const Uint32 high,
                              const Uint16 columns,
                              const Uint16 rows,
                              const unsigned long frame,
                              const unsigned long frames)
      : DiMonoOutputPixel(pixel, frames, (unsigned long)fabs(high - low)),
        Data(NULL),
        DeleteData(buffer == NULL)
    {
        if ((pixel != NULL) && (Count > 0))
        {
            Data = (T3 *)buffer;
            if ((vlut != NULL) && (vlut->isValid()))            // valid VOI LUT ?
                voilut(pixel, frame * Count, vlut, plut, disp, (T3)low, (T3)high);
            else
            {
                if (width <= 0)                                 // no valid window according to Cor Loef (author of suppl. 33)
                    nowindow(pixel, frame * Count, plut, disp, (T3)low, (T3)high);
                else
                    window(pixel, frame * Count, plut, disp, center, width, (T3)low, (T3)high);
            }
            overlay(overlays, disp, columns, rows, frame);      // add (visible) overlay planes to output bitmap
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


 protected:

    inline void determineUsedValues()
    {
        if ((UsedValues == NULL) && (MaxValue > 0) && (MaxValue < MAX_TABLE_ENTRY_COUNT))
        {
            UsedValues = new Uint8[MaxValue + 1];
            if (UsedValues != NULL)
            {
                OFBitmanipTemplate<Uint8>::zeroMem(UsedValues, MaxValue + 1); // initialize array
                register const T3 *p = Data;
                register Uint8 *q = UsedValues;
                register unsigned long i;
                for (i = 0; i < Count; i++)
                    *(q + *(p++)) = 1;                                        // mark used entries
            }
        }
    }


 private:

    inline void createBartenLUT(const DiBartenLUT *&blut,
                                DiDisplayFunction *disp,
                                const int bits)
    {
        if ((disp != NULL) && (disp->isValid()))
        {                                                                     // create Barten LUT
            blut = disp->getBartenLUT(bits);
            if ((blut != NULL) && (blut->isValid()))                          // LUT is invalid
            {
                if (DicomImageClass::DebugLevel >= DicomImageClass::DL_Informationals)
                    cerr << "INFO: using Barten transformation" << endl;
            } else {
                if (DicomImageClass::DebugLevel >= DicomImageClass::DL_Warnings)
                    cerr << "WARNING: can't create Barten LUT ... ignoring Barten transformation !" << endl;
                blut = NULL;
            }
        }
    }


    inline int initOptimizationLUT(T3 *&lut,
                                   const unsigned long ocnt)
    {
        if ((sizeof(T1) <= 2) && (Count > 3 * ocnt))                          // optimization criteria
        {                                                                     // use LUT for optimization
            lut = new T3[ocnt];
            if (lut != NULL)
            {
                if (DicomImageClass::DebugLevel >= DicomImageClass::DL_Informationals)
                    cerr << "INFO: using optimized routine with additional LUT" << endl;
                return 1;
            }
        }
        return 0;
    }


    inline void applyOptimizationLUT(register const T1 *p,
                                     const T3* lut,
                                     const T2 offset)
    {
        const T3 *lut0 = lut - offset;                                        // points to 'zero' entry
        register T3 *q = Data;
        register unsigned long i;
        for (i = 0; i < Count; i++)                                           // apply LUT
            *(q++) = *(lut0 + (*(p++)));
    }


    void voilut(const DiMonoPixel *inter,                  // apply VOI LUT
                const Uint32 start,
                const DiLookupTable *vlut,
                const DiLookupTable *plut,
                DiDisplayFunction *disp,
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
                if (DicomImageClass::DebugLevel >= DicomImageClass::DL_Informationals)
                    cerr << "INFO: using VOI routine 'voilut()'" << endl;
                const DiBartenLUT *blut = NULL;
                const double minvalue = vlut->getMinValue();
                const double outrange = (double)high - (double)low + 1;
                register unsigned long i;
                if (minvalue == vlut->getMaxValue())                                    // LUT has only one entry or all entries are equal
                {
                    T3 value;
                    if ((plut != NULL) && (plut->isValid()))                            // has presentation LUT
                    {
                        if (DicomImageClass::DebugLevel >= DicomImageClass::DL_Informationals)
                            cerr << "INFO: using presentation LUT transformation" << endl;
                        createBartenLUT(blut, disp, plut->getBits());
                        const Uint32 value2 = (Uint32)((minvalue / (double)vlut->getAbsMaxRange()) * plut->getCount());
                        if (blut != NULL)                                               // perform Barten transformation
                        {
                            /* UNTESTED !! */
                            if (low > high)                                             // invers
                                value = (T3)blut->getValue((Uint16)(plut->getAbsMaxRange() - plut->getValue(value2) - 1));
                            else                                                        // normal
                                value = (T3)blut->getValue((Uint16)plut->getValue(value2));
                        } else {                                                        // don't use Barten: invalid or absent
                            value = (T3)((double)low + (double)plut->getValue(value2) * outrange / (double)plut->getAbsMaxRange());
                        }
                    } else {                                                            // has no presentation LUT
                        createBartenLUT(blut, disp, vlut->getBits());
                        if (blut != NULL)                                               // perform Barten transformation
                        {
                            /* UNTESTED !! */
                            if (low > high)                                             // invers
                                value = (T3)blut->getValue((Uint16)(vlut->getAbsMaxRange() - minvalue - 1));
                            else                                                        // normal
                                value = (T3)blut->getValue((Uint16)minvalue);
                        } else                                                          // don't use Barten: invalid or absent
                            value = (T3)((double)low + (minvalue / (double)vlut->getAbsMaxRange()) * outrange);
                    }
                    OFBitmanipTemplate<T3>::setMem(Data, value, Count);                 // set output pixels to LUT value
                } else {
                    register T2 value;
                    const T2 absmin = (T2)inter->getAbsMinimum();
                    const T2 firstentry = vlut->getFirstEntry(value);                   // choose signed/unsigned method
                    const T2 lastentry = vlut->getLastEntry(value);
                    const unsigned long ocnt = (unsigned long)inter->getAbsMaxRange();  // number of LUT entries
                    register const T1 *p = pixel + start;
                    register T3 *q = Data;
                    T3 *lut = NULL;
                    if ((plut != NULL) && (plut->isValid()))                            // has presentation LUT
                    {
                        if (DicomImageClass::DebugLevel >= DicomImageClass::DL_Informationals)
                            cerr << "INFO: using presentation LUT transformation" << endl;
                        createBartenLUT(blut, disp, plut->getBits());
                        register Uint32 value2;                                         // presentation LUT is always unsigned
                        const Uint32 pcnt = plut->getCount();
                        const double gradient1 = (double)pcnt / (double)vlut->getAbsMaxRange();
                        const Uint32 firstvalue = (Uint32)((double)vlut->getFirstValue() * gradient1);
                        const Uint32 lastvalue = (Uint32)((double)vlut->getLastValue() * gradient1);
                        if (initOptimizationLUT(lut, ocnt))
                        {                                                                 // use LUT for optimization
                            q = lut;
                            if (blut != NULL)                                             // perform Barten transformation
                            {
                                if (low > high)                                           // inverse
                                {
                                    const Uint16 maxvalue = (Uint16)(plut->getAbsMaxRange() - 1);
                                    for (i = 0; i < ocnt; i++)
                                    {
                                        value = (T2)i + absmin;
                                        if (value <= firstentry)
                                            value2 = firstvalue;
                                        else if (value >= lastentry)
                                            value2 = lastvalue;
                                        else
                                            value2 = (Uint32)((double)vlut->getValue(value) * gradient1);
                                        *(q++) = (T3)blut->getValue(maxvalue - plut->getValue(value2));
                                    }
                                } else {                                                  // normal
                                    for (i = 0; i < ocnt; i++)
                                    {
                                        value = (T2)i + absmin;
                                        if (value <= firstentry)
                                            value2 = firstvalue;
                                        else if (value >= lastentry)
                                            value2 = lastvalue;
                                        else
                                            value2 = (Uint32)((double)vlut->getValue(value) * gradient1);
                                        *(q++) = (T3)blut->getValue(plut->getValue(value2));
                                    }
                                }
                            } else {                                                      // don't use Barten: invalid or absent
                                const double gradient2 = outrange / (double)plut->getAbsMaxRange();
                                for (i = 0; i < ocnt; i++)
                                {
                                    value = (T2)i + absmin;
                                    if (value <= firstentry)
                                        value2 = firstvalue;
                                    else if (value >= lastentry)
                                        value2 = lastvalue;
                                    else
                                        value2 = (Uint32)((double)vlut->getValue(value) * gradient1);
                                    *(q++) = (T3)((double)low + (double)plut->getValue(value2) * gradient2);
                                }
                            }
                            applyOptimizationLUT(p, lut, (T2)inter->getAbsMinimum());
                        }
                        if (lut == NULL)                                                  // use "normal" transformation
                        {
                            if (blut != NULL)                                             // perform Barten transformation
                            {
                                if (low > high)                                           // inverse
                                {
                                    const Uint16 maxvalue = (Uint16)(Uint16)(vlut->getAbsMaxRange() - 1);
                                    for (i = 0; i < Count; i++)
                                    {
                                        value = (T2)(*(p++));                             // pixel value                            
                                        if (value <= firstentry)
                                            value2 = firstvalue;
                                        else if (value >= lastentry)
                                            value2 = lastvalue;
                                        else
                                            value2 = (Uint32)((double)vlut->getValue(value) * gradient1);
                                        *(q++) = (T3)blut->getValue(maxvalue - plut->getValue(value2));
                                    }
                                } else {                                                  // normal
                                    for (i = 0; i < Count; i++)
                                    {
                                        value = (T2)(*(p++));                             // pixel value                            
                                        if (value <= firstentry)
                                            value2 = firstvalue;
                                        else if (value >= lastentry)
                                            value2 = lastvalue;
                                        else
                                            value2 = (Uint32)((double)vlut->getValue(value) * gradient1);
                                        *(q++) = (T3)blut->getValue(plut->getValue(value2));
                                    }
                                }
                            } else {                                                      // don't use Barten: invalid or absent
                                const double gradient2 = outrange / (double)plut->getAbsMaxRange();
                                for (i = 0; i < Count; i++)
                                {
                                    value = (T2)(*(p++));                                 // pixel value                            
                                    if (value <= firstentry)
                                        value2 = firstvalue;
                                    else if (value >= lastentry)
                                        value2 = lastvalue;
                                    else
                                        value2 = (Uint32)((double)vlut->getValue(value) * gradient1);
                                    *(q++) = (T3)((double)low + (double)plut->getValue(value2) * gradient2);
                                }
                            }
                        }
                    } else {                                                              // has no presentation LUT
                        createBartenLUT(blut, disp, vlut->getBits());
                        const double gradient = outrange / (double)vlut->getAbsMaxRange();                    
                        const T3 firstvalue = (T3)((double)low + (double)vlut->getFirstValue() * gradient);
                        const T3 lastvalue = (T3)((double)low + (double)vlut->getLastValue() * gradient);
                        if (initOptimizationLUT(lut, ocnt))
                        {                                                                 // use LUT for optimization
                            q = lut;
                            if (blut != NULL)                                             // perform Barten transformation
                            {
                                if (low > high)                                           // inverse
                                {
                                    const Uint16 maxvalue = (Uint16)(Uint16)(vlut->getAbsMaxRange() - 1);
                                    for (i = 0; i < ocnt; i++)
                                    {
                                        value = (T2)i + absmin;
                                        if (value < firstentry)
                                            value = firstentry;
                                        else if (value > lastentry)
                                            value = lastentry;
                                        *(q++) = (T3)blut->getValue(maxvalue - vlut->getValue(value));
                                    }
                                } else {                                                  // normal
                                    for (i = 0; i < ocnt; i++)
                                    {
                                        value = (T2)i + absmin;
                                        if (value < firstentry)
                                            value = firstentry;
                                        else if (value > lastentry)
                                            value = lastentry;
                                        *(q++) = (T3)blut->getValue(vlut->getValue(value));
                                    }
                                }
                            } else {                                                      // don't use Barten: invalid or absent
                                for (i = 0; i < ocnt; i++)                                // calculating LUT entries
                                {
                                    value = (T2)i + absmin;
                                    if (value <= firstentry)
                                        *(q++) = firstvalue;
                                    else if (value >= lastentry)
                                        *(q++) = lastvalue;
                                    else
                                        *(q++) = (T3)((double)low + (double)vlut->getValue(value) * gradient);
                                }
                            }
                            applyOptimizationLUT(p, lut, (T2)inter->getAbsMinimum());
                        }
                        if (lut == NULL)                                                  // use "normal" transformation
                        {
                            if (blut != NULL)                                             // perform Barten transformation
                            {
                                if (low > high)                                           // inverse
                                {
                                    const Uint16 maxvalue = (Uint16)(Uint16)(vlut->getAbsMaxRange() - 1);
                                    for (i = 0; i < Count; i++)
                                    {
                                        value = (T2)(*(p++));
                                        if (value < firstentry)
                                            value = firstentry;
                                        else if (value > lastentry)
                                            value = lastentry;
                                        *(q++) = (T3)blut->getValue(maxvalue - vlut->getValue(value));
                                    }
                                } else {                                                  // normal
                                    for (i = 0; i < Count; i++)
                                    {
                                        value = (T2)(*(p++));
                                        if (value < firstentry)
                                            value = firstentry;
                                        else if (value > lastentry)
                                            value = lastentry;
                                        *(q++) = (T3)blut->getValue(vlut->getValue(value));
                                    }
                                }
                            } else {                                                      // don't use Barten: invalid or absent
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
                    delete[] lut;
                }
            } 
        } else
            Data = NULL;
    }

    void nowindow(const DiMonoPixel *inter,                // perform scaling
                  const Uint32 start,
                  const DiLookupTable *plut,
                  DiDisplayFunction *disp,
                  const T3 low,
                  const T3 high)
    {
        const DiBartenLUT *blut = NULL;
        const T1 *pixel = (const T1 *)inter->getData();
        if (pixel != NULL)
        {
            if (Data == NULL)                                                         // create new output buffer
                Data = new T3[Count];
            if (Data != NULL)
            {
                if (DicomImageClass::DebugLevel >= DicomImageClass::DL_Informationals)
                    cerr << "INFO: using VOI routine 'nowindow()'" << endl;
                const double absmin = inter->getAbsMinimum();
                const double absmax = inter->getAbsMaximum();
                const double outrange = (double)high - (double)low + 1;
                const unsigned long ocnt = (unsigned long)inter->getAbsMaxRange();     // number of LUT entries
                register const T1 *p = pixel + start;
                register T3 *q = Data;
                register unsigned long i;
                T3 *lut = NULL;
                if ((plut != NULL) && (plut->isValid()))                              // has presentation LUT
                {
                    if (DicomImageClass::DebugLevel >= DicomImageClass::DL_Informationals)
                        cerr << "INFO: using presentation LUT transformation" << endl;
                    createBartenLUT(blut, disp, plut->getBits());                    
                    register Uint32 value;                                            // presentation LUT is always unsigned
                    const double gradient1 = (double)plut->getCount() / (inter->getAbsMaxRange());
                    const double gradient2 = outrange / (double)plut->getAbsMaxRange();
                    if (initOptimizationLUT(lut, ocnt))
                    {                                                                 // use LUT for optimization
                        q = lut;
                        if (blut != NULL)                                             // perform Barten transformation
                        {
                            if (low > high)                                           // inverse
                            {
                                const Uint16 maxvalue = (Uint16)(Uint16)(plut->getAbsMaxRange() - 1);
                                for (i = 0; i < Count; i++)
                                {
                                    value = (Uint32)((double)i * gradient1);
                                    *(q++) = (T3)blut->getValue(maxvalue - plut->getValue(value));
                                }
                            } else {                                                  // normal
                                for (i = 0; i < ocnt; i++)
                                {
                                    value = (Uint32)((double)i * gradient1);
                                    *(q++) = (T3)blut->getValue(plut->getValue(value));
                                }
                            }
                        } else {                                                      // don't use Barten: invalid or absent
                            for (i = 0; i < ocnt; i++)
                            {
                                value = (Uint32)((double)i * gradient1);
                                *(q++) = (T3)((double)low + (double)plut->getValue(value) * gradient2);
                            }
                        }
                        applyOptimizationLUT(p, lut, (T2)inter->getAbsMinimum());
                    }
                    if (lut == NULL)                                                  // use "normal" transformation
                    {
                        if (blut != NULL)                                             // perform Barten transformation
                        {
                            if (low > high)                                           // inverse
                            {
                                const Uint16 maxvalue = (Uint16)(Uint16)(plut->getAbsMaxRange() - 1);
                                for (i = 0; i < Count; i++)
                                {
                                    value = (Uint32)(((double)(*(p++)) - absmin) * gradient1);
                                    *(q++) = (T3)blut->getValue(maxvalue - plut->getValue(value));
                                }
                            } else {                                                  // normal
                                for (i = 0; i < Count; i++)
                                {
                                    value = (Uint32)(((double)(*(p++)) - absmin) * gradient1);
                                    *(q++) = (T3)blut->getValue(plut->getValue(value));
                                }
                            }
                        } else {                                                      // don't use Barten: invalid or absent
                            for (i = 0; i < Count; i++)
                            {
                                value = (Uint32)(((double)(*(p++)) - absmin) * gradient1);
                                *(q++) = (T3)((double)low + (double)plut->getValue(value) * gradient2);
                            }
                        }
                    }
                } else {                                                              // has no presentation LUT
                    createBartenLUT(blut, disp, inter->getBits());
                    register const double gradient = outrange / (inter->getAbsMaxRange());
                    if (initOptimizationLUT(lut, ocnt))
                    {                                                                 // use LUT for optimization
                        q = lut;
                        if (blut != NULL)                                             // perform Barten transformation
                        {
                            if (low > high)                                           // inverse
                            {
                                for (i = ocnt; i > 0; i--)                            // calculating LUT entries
                                    *(q++) = (T3)blut->getValue((Uint16)(i - 1));
                            } else{                                                   // normal
                                for (i = 0; i < ocnt; i++)                             // calculating LUT entries
                                    *(q++) = (T3)blut->getValue((Uint16)i);
                            }
                        } else {                                                      // don't use Barten: invalid or absent
                            for (i = 0; i < ocnt; i++)                                // calculating LUT entries
                                *(q++) = (T3)((double)low + (double)i * gradient);
                        }
                        applyOptimizationLUT(p, lut, (T2)inter->getAbsMinimum());
                    }
                    if (lut == NULL)                                                  // use "normal" transformation
                    {
                        if (blut != NULL)                                             // perform Barten transformation
                        {
                            if (low > high)                                           // inverse
                            {
                                for (i = 0; i < Count; i++)
                                    *(q++) = (T3)blut->getValue((Uint16)(absmax - ((double)*(p++) - absmin)));
                            } else {                                                  // normal
                                for (i = 0; i < Count; i++)
                                    *(q++) = (T3)blut->getValue((Uint16)((double)*(p++) - absmin));
                            }
                        } else {                                                      // don't use Barten: invalid or absent
                            for (i = 0; i < Count; i++)
                                *(q++) = (T3)((double)low + ((double)(*(p++)) - absmin) * gradient);
                        }
                    }
                }
                delete[] lut;
            }
        } else
            Data = NULL;
    }


    void window(const DiMonoPixel *inter,                  // apply VOI window
                const Uint32 start,
                const DiLookupTable *plut,
                DiDisplayFunction *disp,
                const double center,
                const double width,
                const T3 low,
                const T3 high)
    {
        const T1 *pixel = (const T1 *)inter->getData();
        if (pixel != NULL)
        {
            if (Data == NULL)
                Data = new T3[Count];                                                 // create new output buffer
            if (Data != NULL)
            {
                if (DicomImageClass::DebugLevel >= DicomImageClass::DL_Informationals)
                    cerr << "INFO: using VOI routine 'window()'" << endl;
                const DiBartenLUT *blut = NULL;
                const double absmin = inter->getAbsMinimum();
                const double left = center - width / 2;                               // window borders
                const double right = center + width / 2;
                const double outrange = (double)high - (double)low + 1;               // output range
                const unsigned long ocnt = (unsigned long)inter->getAbsMaxRange();    // number of LUT entries
                register const T1 *p = pixel + start;
                register T3 *q = Data;
                register unsigned long i;
                register double value;
                T3 *lut = NULL;
                if ((plut != NULL) && (plut->isValid()))                              // has presentation LUT
                {
                    if (DicomImageClass::DebugLevel >= DicomImageClass::DL_Informationals)
                        cerr << "INFO: using presentation LUT transformation" << endl;
                    createBartenLUT(blut, disp, plut->getBits());
                    register Uint32 value2;                                           // presentation LUT is always unsigned
                    const Uint32 pcnt = plut->getCount();
                    const double gradient1 = (double)pcnt / width;
                    if (initOptimizationLUT(lut, ocnt))
                    {                                                                 // use LUT for optimization
                        q = lut;
                        if (blut != NULL)                                             // perform Barten transformation
                        {
                            const double maxvalue = (double)(blut->getCount() - 1);
                            const double lowvalue = (low > high) ? maxvalue : 0;
                            const double gradient2 = (low > high) ? (-maxvalue / (double)plut->getAbsMaxRange()) : 
                                                                    (maxvalue / (double)plut->getAbsMaxRange());
                            for (i = 0; i < ocnt; i++)
                            {
                                value = (double)i + absmin;                           // pixel value
                                if (value <= left)
                                    value2 = 0;                                       // first LUT index
                                else if (value >= right)
                                    value2 = pcnt - 1;                                // last LUT index
                                else
                                    value2 = (Uint32)((value - left) * gradient1);
                                *(q++) = (T3)blut->getValue((Uint16)(lowvalue + (double)plut->getValue(value2) * gradient2));
                            }
                        } else {                                                      // don't use Barten: invalid or absent
                            const double gradient2 = outrange / (double)plut->getAbsMaxRange();
                            for (i = 0; i < ocnt; i++)
                            {
                                value = (double)i + absmin;                           // pixel value
                                if (value <= left)
                                    value2 = 0;                                       // first LUT index
                                else if (value >= right)
                                    value2 = pcnt - 1;                                // last LUT index
                                else
                                    value2 = (Uint32)((value - left) * gradient1);
                                *(q++) = (T3)((double)low + (double)plut->getValue(value2) * gradient2);
                            }
                        }
                        applyOptimizationLUT(p, lut, (T2)absmin);
                    }
                    if (lut == NULL)                                                  // use "normal" transformation
                    {
                        if (blut != NULL)                                             // perform Barten transformation
                        {
                            const double maxvalue = (double)(blut->getCount() - 1);
                            const double lowvalue = (low > high) ? maxvalue : 0;
                            const double gradient2 = (low > high) ? (-maxvalue / (double)plut->getAbsMaxRange()) :
                                                                    (maxvalue / (double)plut->getAbsMaxRange());
                            for (i = 0; i < Count; i++)
                            {
                                value = (double)*(p++);                               // pixel value
                                if (value <= left)
                                    value2 = 0;                                       // first LUT index
                                else if (value >= right)
                                    value2 = pcnt - 1;                                // last LUT index
                                else
                                    value2 = (Uint32)((value - left) * gradient1);
                                *(q++) = (T3)blut->getValue((Uint16)(lowvalue + (double)plut->getValue(value2) * gradient2));
                            }
                        } else {                                                      // don't use Barten: invalid or absent
                            const double gradient2 = outrange / (double)plut->getAbsMaxRange();
                            for (i = 0; i < Count; i++)
                            {
                                value = (double)*(p++);                               // pixel value
                                if (value <= left)
                                    value2 = 0;                                       // first LUT index
                                else if (value >= right)
                                    value2 = pcnt - 1;                                // last LUT index
                                else
                                    value2 = (Uint32)((value - left) * gradient1);
                                *(q++) = (T3)((double)low + (double)plut->getValue(value2) * gradient2);
                            }
                        }
                    }
                } else {                                                              // has no presentation LUT
                    createBartenLUT(blut, disp, bitsof(T1));
                    const double gradient = outrange / width;
                    if (initOptimizationLUT(lut, ocnt))
                    {                                                                 // use LUT for optimization
                        q = lut;
                        if (blut != NULL)                                             // perform Barten transformation
                        {
                            const double maxvalue = (double)(blut->getCount() - 1);
                            const double lowvalue = (low > high) ? maxvalue : 0;
                            const double gradient2 = (low > high) ? (-maxvalue / width) : (maxvalue / width);
                            for (i = 0; i < ocnt; i++)							      // calculating LUT entries
                            {                                        
                                value = (double)i - absmin + left;
                                if (value < 0)                                               // left border
                                    value = 0;
                                else if (value > width)                                      // right border
                                    value = width;
                                *(q++) = (T3)blut->getValue((Uint16)(lowvalue + value * gradient2));   // calculate value
                            }
                        } else {                                                       // don't use Barten: invalid or absent
                            for (i = 0; i < ocnt; i++)                                 // calculating LUT entries
                            {
                                value = (double)i + absmin;
                                if (value <= left)
                                    *(q++) = low;                                            // black/white
                                else if (value >= right)
                                    *(q++) = high;                                           // white/black
                                else
                                    *(q++) = (T3)((double)low + (value - left) * gradient);  // gray value
                            }
                        }
                        applyOptimizationLUT(p, lut, (T2)absmin);
                    }
                    if (lut == NULL)                                                  // use "normal" transformation
                    {
                        if (blut != NULL)                                             // perform Barten transformation
                        {
                            const double maxvalue = (double)(blut->getCount() - 1);
                            const double lowvalue = (low > high) ? maxvalue : 0;
                            const double gradient2 = (low > high) ? (-maxvalue / width) : (maxvalue / width);
                            for (i = 0; i < Count; i++)						    	  // calculating LUT entries
                            {                                        
                                value = (double)*(p++);
                                if (value < 0)                                               // left border
                                    value = 0;
                                else if (value > width)                                      // right border
                                    value = width;
                                *(q++) = (T3)blut->getValue((Uint16)(lowvalue + value * gradient2));   // calculate value
                            }
                        } else {                                                      // don't use Barten: invalid or absent
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
                        }
                    }
                } 
                delete[] lut;
            }   
        } else
            Data = NULL;
    }

    void overlay(DiOverlay *overlays[2],                   // apply overlay planes to output data
                 DiDisplayFunction *disp,
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
                            const T3 maxvalue = (T3)DicomImageClass::maxval(bitsof(T3));
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
                                    const T3 thresh = (T3)DicomImageClass::maxval(bitsof(T3) / 2);
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
                                case EMO_BitmapShutter:
                                {
                                    register T3 fore = (T3)((double)maxvalue * (double)plane->getPValue() / (double)DicomImageClass::maxval(WIDTH_OF_PVALUES));
                                    if ((disp != NULL) && (disp->isValid()))
                                    {
                                        const DiBartenLUT *blut = disp->getBartenLUT(WIDTH_OF_PVALUES);
                                        if ((blut != NULL) && (blut->isValid()))
                                            fore = (T3)blut->getValue(plane->getPValue());
                                    }
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
 * Revision 1.10  1999-02-28 16:41:01  joergr
 * Corrected bug: the output bits for bitmaps shutters were inverted (this was
 * done due to an error in the used test images).
 *
 * Revision 1.9  1999/02/11 16:40:19  joergr
 * Added routine to check whether particular grayscale values are unused in
 * the output data.
 * Removed two small memory leaks reported by dmalloc library.
 *
 * Revision 1.8  1999/02/05 16:44:52  joergr
 * Corrected calculation of DDL value for bitmaps shutters (overlays).
 *
 * Revision 1.7  1999/02/05 15:13:36  joergr
 * Added conversion P-Value to DDL when display function is absent.
 *
 * Revision 1.6  1999/02/03 17:32:43  joergr
 * Added optimization LUT to transform pixel data.
 * Added support for calibration according to Barten transformation (incl.
 * a DISPLAY file describing the monitor characteristic).
 *
 * Revision 1.5  1999/01/20 15:11:05  joergr
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
