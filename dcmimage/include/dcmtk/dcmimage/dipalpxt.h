/*
 *
 *  Copyright (C) 1996-2016, OFFIS e.V.
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  This software and supporting documentation were developed by
 *
 *    OFFIS e.V.
 *    R&D Division Health
 *    Escherweg 2
 *    D-26121 Oldenburg, Germany
 *
 *
 *  Module:  dcmimage
 *
 *  Author:  Joerg Riesmeier
 *
 *  Purpose: DicomPalettePixelTemplate (Header)
 *
 */


#ifndef DIPALPXT_H
#define DIPALPXT_H

#include "dcmtk/config/osconfig.h"

#include "dcmtk/dcmimage/dicopxt.h"
#include "dcmtk/dcmimgle/diluptab.h"
#include "dcmtk/dcmimgle/diinpx.h"  /* gcc 3.4 needs this */


/*---------------------*
 *  class declaration  *
 *---------------------*/

/** Template class to handle Palette color pixel data
 */
template<class T1, class T2, class T3>
class DiPalettePixelTemplate
  : public DiColorPixelTemplate<T3>
{

 public:

    /** constructor
     *
     ** @param  docu     pointer to DICOM document
     *  @param  pixel    pointer to input pixel representation
     *  @param  palette  pointer to RGB color palette
     *  @param  status   reference to status variable
     */
    DiPalettePixelTemplate(const DiDocument *docu,
                           const DiInputPixel *pixel,
                           DiLookupTable *palette[3],
                           EI_Status &status)
      : DiColorPixelTemplate<T3>(docu, pixel, 1, status)
    {
        if ((pixel != NULL) && (this->Count > 0) && (status == EIS_Normal))
        {
            if (this->PlanarConfiguration)
            {
                status = EIS_InvalidValue;
                DCMIMAGE_ERROR("invalid value for 'PlanarConfiguration' (" << this->PlanarConfiguration << ")");
            }
            else
                convert(OFstatic_cast(const T1 *, pixel->getData()) + pixel->getPixelStart(), palette);
        }
    }

    /** destructor
     */
    virtual ~DiPalettePixelTemplate()
    {
    }


 private:

    /** convert input pixel data to intermediate representation
     *
     ** @param  pixel    pointer to input pixel data
     *  @param  palette  pointer to RGB color palette
     */
    void convert(const T1 *pixel,
                 DiLookupTable *palette[3])
    {                                                                // can be optimized if necessary !
        if (this->Init(pixel))
        {
            const T1 *p = pixel;
            T2 value = 0;
            unsigned long i;
            int j;
            // use the number of input pixels derived from the length of the 'PixelData'
            // attribute), but not more than the size of the intermediate buffer
            const unsigned long count = (this->InputCount < this->Count) ? this->InputCount : this->Count;
            for (i = 0; i < count; ++i)
            {
                value = OFstatic_cast(T2, *(p++));
                for (j = 0; j < 3; ++j)
                {
                    if (value <= palette[j]->getFirstEntry(value))
                        this->Data[j][i] = OFstatic_cast(T3, palette[j]->getFirstValue());
                    else if (value >= palette[j]->getLastEntry(value))
                        this->Data[j][i] = OFstatic_cast(T3, palette[j]->getLastValue());
                    else
                        this->Data[j][i] = OFstatic_cast(T3, palette[j]->getValue(value));
                }
            }
        }
    }
};


#endif
