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
 *  Update Date:      $Date: 1999-03-24 17:20:10 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmimgle/include/Attic/dimoipxt.h,v $
 *  CVS/RCS Revision: $Revision: 1.8 $
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

/** Template class to convert monochrome pixel data to intermediate representation
 */
template<class T1, class T2, class T3>
class DiMonoInputPixelTemplate
  : public DiMonoPixelTemplate<T3>
{

 public:

    DiMonoInputPixelTemplate(const DiInputPixel *pixel,
                             DiMonoModality *modality)
      : DiMonoPixelTemplate<T3>(pixel, modality)
    {
        if ((pixel != NULL) && (Count > 0))
        {   
            if ((Modality != NULL) && Modality->hasLookupTable() && (bitsof(T1) <= MAX_TABLE_ENTRY_SIZE))
            {
                modlut(pixel);
                determineMinMax((T3)Modality->getMinValue(), (T3)Modality->getMaxValue());
            }
            else if ((Modality != NULL) && Modality->hasRescaling())
            {
                rescale(pixel, Modality->getRescaleSlope(), Modality->getRescaleIntercept());
                determineMinMax((T3)Modality->getMinValue(), (T3)Modality->getMaxValue());
            } else {
                rescale(pixel);                     // copy pixel data
                determineMinMax((T3)pixel->getMinValue(), (T3)pixel->getMaxValue());
            }
        }
    }

    virtual ~DiMonoInputPixelTemplate()
    {
    }


 private:

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

    void modlut(const DiInputPixel *input)
    {
        const T1 *pixel = (const T1 *)input->getData();
        if ((pixel != NULL) && (Modality != NULL))
        {
            const DiLookupTable *mlut = Modality->getTableData();
            if (mlut != NULL)
            {
                Data = new T3[Count];
                if (Data != NULL)
                {
                    if (DicomImageClass::DebugLevel >= DicomImageClass::DL_Informationals)
                        cerr << "INFO: using modality routine 'modlut()'" << endl;
                    register T2 value;
                    const T2 firstentry = mlut->getFirstEntry(value);                     // choose signed/unsigned method
                    const T2 lastentry = mlut->getLastEntry(value);
                    const T3 firstvalue = (T3)mlut->getFirstValue();
                    const T3 lastvalue = (T3)mlut->getLastValue();
                    register T3 *q = Data;
                    register unsigned long i;
                    T3 *lut = NULL;
                    const unsigned long ocnt = (unsigned long)input->getAbsMaxRange();    // number of LUT entries
                    if (initOptimizationLUT(lut, ocnt))
                    {                                                                     // use LUT for optimization
                        const T2 absmin = (T2)input->getAbsMinimum();
                        q = lut;
                        for (i = 0; i < ocnt; i++)                                        // calculating LUT entries
                        {
                            value = (T2)i + absmin;
                            if (value <= firstentry)
                                *(q++) = firstvalue;
                            else if (value >= lastentry)
                                *(q++) = lastvalue;
                            else
                                *(q++) = (T3)mlut->getValue(value);
                        }
                        applyOptimizationLUT(pixel, lut, absmin);
                    }
                    if (lut == NULL)                                                      // use "normal" transformation
                    {
                        register const T1 *p = pixel;
                        for (i = 0; i < Count; i++)
                        {
                            value = (T2)(*(p++));
                            if (value <= firstentry)
                                *(q++) = firstvalue;
                            else if (value >= lastentry)
                                *(q++) = lastvalue;
                            else
                                *(q++) = (T3)mlut->getValue(value);
                        }
                    }
                    delete[] lut;
                }
            } 
        }
    }

    void rescale(const DiInputPixel *input,
                 const double slope = 1.0,
                 const double intercept = 0.0)
    {
        const T1 *pixel = (const T1 *)input->getData();
        if (pixel != NULL)
        {
            Data = new T3[Count];
            if (Data != NULL)
            {
                register T3 *q = Data;
                register unsigned long i;
                if ((slope == 1.0) && (intercept == 0.0))
                {
                    register const T1 *p = pixel;
                    for (i = 0; i < Count; i++)            // copy pixel data: can't use copyMem because T1 isn't always equal to T3
                        *(q++) = (T3)*(p++);
                } else {
                    if (DicomImageClass::DebugLevel >= DicomImageClass::DL_Informationals)
                        cerr << "INFO: using modality routine 'rescale()'" << endl;
                    T3 *lut = NULL;
                    const unsigned long ocnt = (unsigned long)input->getAbsMaxRange();    // number of LUT entries
                    if (initOptimizationLUT(lut, ocnt))
                    {                                                                     // use LUT for optimization
                        const double absmin = input->getAbsMinimum();
                        q = lut;
                        if (slope == 1.0)
                        {
                            for (i = 0; i < ocnt; i++)                                    // calculating LUT entries
                                *(q++) = (T3)((double)i + absmin + intercept);
                        } else {
                            if (intercept == 0.0)
                            {
                                for (i = 0; i < ocnt; i++)
                                    *(q++) = (T3)(((double)i + absmin) * slope);
                            } else {
                                for (i = 0; i < ocnt; i++)
                                    *(q++) = (T3)(((double)i + absmin) * slope + intercept);
                            }
                        }
                        applyOptimizationLUT(pixel, lut, (T2)absmin);
                    }
                    if (lut == NULL)                                                      // use "normal" transformation
                    {
                        register const T1 *p = pixel;
                        if (slope == 1.0)
                        {
                            for (i = 0; i < Count; i++)
                                *(q++) = (T3)((double)*(p++) + intercept);
                        } else {
                            if (intercept == 0.0)
                            {
                                for (i = 0; i < Count; i++)
                                    *(q++) = (T3)((double)*(p++) * slope);
                            } else {
                                for (i = 0; i < Count; i++)
                                    *(q++) = (T3)((double)*(p++) * slope + intercept);
                            }
                        }
                    }
                    delete[] lut;
                }
            }
        }
    }
};


#endif


/*
 *
 * CVS/RCS Log:
 * $Log: dimoipxt.h,v $
 * Revision 1.8  1999-03-24 17:20:10  joergr
 * Added/Modified comments and formatting.
 *
 * Revision 1.7  1999/03/02 12:02:27  joergr
 * Corrected bug: when determining minimum and maximum pixel value (external)
 * modality LUTs were ignored.
 *
 * Revision 1.6  1999/02/11 16:37:10  joergr
 * Removed inline declarations from several methods.
 *
 * Revision 1.5  1999/02/03 17:29:19  joergr
 * Added optimization LUT to transform pixel data.
 *
 * Revision 1.4  1999/01/20 15:06:24  joergr
 * Replaced invocation of getCount() by member variable Count where possible.
 * Added optimization to modality and VOI transformation (using additional
 * LUTs).
 *
 * Revision 1.3  1998/12/22 14:29:39  joergr
 * Replaced method copyMem by for-loop copying each item.
 * Renamed some variables
 *
 * Revision 1.2  1998/12/14 17:21:09  joergr
 * Added support for signed values as second entry in look-up tables
 * (= first value mapped).
 *
 * Revision 1.1  1998/11/27 15:24:08  joergr
 * Added copyright message.
 * Added new cases to optimize rescaling.
 * Added support for new bit manipulation class.
 * Corrected bug in modality LUT transformation method.
 *
 * Revision 1.5  1998/07/01 08:39:23  joergr
 * Minor changes to avoid compiler warnings (gcc 2.8.1 with additional
 * options), e.g. add copy constructors.
 *
 * Revision 1.4  1998/05/11 14:53:21  joergr
 * Added CVS/RCS header to each file.
 *
 *
 */
