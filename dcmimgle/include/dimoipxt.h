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
 *  Update Date:      $Date: 1999-01-20 15:06:24 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmimgle/include/Attic/dimoipxt.h,v $
 *  CVS/RCS Revision: $Revision: 1.4 $
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
                modlut(pixel);
            else if ((Modality != NULL) && Modality->hasRescaling())
                rescale(pixel, Modality->getRescaleSlope(), Modality->getRescaleIntercept());
            else
                rescale(pixel, 1.0, 0.0);
            determineMinMax((T3)pixel->getMinValue(), (T3)pixel->getMaxValue());
        }
    }

    virtual ~DiMonoInputPixelTemplate()
    {
    }


 private:

    inline void modlut(const DiInputPixel *input)
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
                    register T2 value;
                    const T2 firstentry = mlut->getFirstEntry(value);                     // choose signed/unsigned method
                    const T2 lastentry = mlut->getLastEntry(value);
                    const T3 firstvalue = (T3)mlut->getFirstValue();
                    const T3 lastvalue = (T3)mlut->getLastValue();
                    register const T1 *p = pixel;
                    register T3 *q = Data;
                    register unsigned long i;
                    T3 *lut = NULL;
                    const unsigned long cnt = (unsigned long)input->getAbsMaxRange(); // number of LUT entries
                    if ((sizeof(T1) <= 2) && (Count > 3 * cnt))                       // optimization criteria
                    {                                                                 // use LUT for optimization
                        lut = new T3[cnt];
                        if (lut != NULL)
                        {
                            if (DicomImageClass::DebugLevel >= DicomImageClass::DL_Informationals)
                                cerr << "INFO: using optimized routine for 'modlut()'" << endl;
                            q = lut;
                            for (i = 0; i < cnt; i++)                                 // calculating LUT entries
                            {
                                value = (T2)i;
                                if (value <= firstentry)
                                    *(q++) = firstvalue;
                                else if (value >= lastentry)
                                    *(q++) = lastvalue;
                                else
                                    *(q++) = (T3)mlut->getValue(value);
                            }
                            q = Data;
/*
                            if (absmin == 0)
                            {                                                         // unsigned input-representation
                                for (i = 0; i < Count; i++)
                                    *(q++) = lut[*(p++)];
                            } else {                                                  // signed input-representation
                                for (i = 0; i < Count; i++)
                                    *(q++) = lut[*(p++) - absmin];
                            }
*/
                            const T3 *lut0 = lut - (T2)input->getAbsMinimum();        // points to 'zero' entry
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
                                *(q++) = (T3)mlut->getValue(value);
                        }
                    } else
                        delete[] lut;
                }
            } 
        }
    }

    inline void rescale(const DiInputPixel *input,
                        const double slope,
                        const double intercept)
    {
        const T1 *pixel = (const T1 *)input->getData();
        if (pixel != NULL)
        {
            Data = new T3[Count];
            if (Data != NULL)
            {
                register const T1 *p = pixel;
                register T3 *q = Data;
                register unsigned long i;
                if ((slope == 1.0) && (intercept == 0.0))
                {
                    for (i = 0; i < Count; i++)            // copy pixel data: can't use copyMem because T1 isn't always equal to T3
                        *(q++) = (T3)*(p++);
                } else {
                    T3 *lut = NULL;
                    const unsigned long cnt = (unsigned long)input->getAbsMaxRange(); // number of LUT entries
                    if ((sizeof(T1) <= 2) && (Count > 3 * cnt))                       // optimization criteria
                    {                                                                 // use LUT for optimization
                        lut = new T3[cnt];
                        if (lut != NULL)
                        {
                            if (DicomImageClass::DebugLevel >= DicomImageClass::DL_Informationals)
                                cerr << "INFO: using optimized routine for 'rescale()'" << endl;
                            q = lut;
                            if (slope == 1.0)
                            {
                                for (i = 0; i < cnt; i++)                             // calculating LUT entries
                                    *(q++) = (T3)((double)i + intercept);
                            } else {
                                if (intercept == 0.0)
                                {
                                    for (i = 0; i < cnt; i++)
                                        *(q++) = (T3)((double)i * slope);
                                } else {
                                    for (i = 0; i < cnt; i++)
                                        *(q++) = (T3)((double)i * slope + intercept);
                                }
                            }
                            q = Data;
/*
                            const T2 absmin = (T2)input->getAbsMinimum();
                            if (absmin == 0)
                            {                                                         // unsigned input-representation
                                for (i = 0; i < Count; i++)
                                    *(q++) = lut[*(p++)];
                            } else {                                                  // signed input-representation
                                for (i = 0; i < Count; i++)
                                    *(q++) = lut[*(p++) - absmin];
                            }
*/
                            const T3 *lut0 = lut - (T2)input->getAbsMinimum();        // points to 'zero' entry
                            for (i = 0; i < Count; i++)
                                *(q++) = *(lut0 + (*(p++)));
                        }
                    }
                    if (lut == NULL)                                                  // use "normal" transformation
                    {
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
                    } else
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
 * Revision 1.4  1999-01-20 15:06:24  joergr
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
