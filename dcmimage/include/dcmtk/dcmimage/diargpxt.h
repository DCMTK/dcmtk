/*
 *
 *  Copyright (C) 1996-2010, OFFIS e.V.
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
 *  Purpose: DicomARGBPixelTemplate (Header) - UNTESTED !!!
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:16:29 $
 *  CVS/RCS Revision: $Revision: 1.21 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#ifndef DIARGPXT_H
#define DIARGPXT_H

#include "dcmtk/config/osconfig.h"

#include "dcmtk/dcmimage/dicopxt.h"
#include "dcmtk/dcmimgle/diluptab.h"
#include "dcmtk/dcmimgle/diinpx.h"  /* gcc 3.4 needs this */

/*---------------------*
 *  class declaration  *
 *---------------------*/

/** Template class to handle ARGB pixel data
 */
template<class T1, class T2, class T3>
class DiARGBPixelTemplate
  : public DiColorPixelTemplate<T3>
{

 public:

    /** constructor
     *
     ** @param  docu       pointer to DICOM document
     *  @param  pixel      pointer to input pixel representation
     *  @param  palette    pointer to RGB color palette
     *  @param  status     reference to status variable
     *  @param  planeSize  number of pixels in a plane
     *  @param  bits       number of bits per sample
     */
    DiARGBPixelTemplate(const DiDocument *docu,
                        const DiInputPixel *pixel,
                        DiLookupTable *palette[3],
                        EI_Status &status,
                        const unsigned long planeSize,
                        const int bits)
      : DiColorPixelTemplate<T3>(docu, pixel, 4, status)
    {
        if ((pixel != NULL) && (this->Count > 0) && (status == EIS_Normal))
            convert(OFstatic_cast(const T1 *, pixel->getData()) + pixel->getPixelStart(), palette, planeSize, bits);
    }

    /** destructor
     */
    virtual ~DiARGBPixelTemplate()
    {
    }


 private:

    /** convert input pixel data to intermediate representation
     *
     ** @param  pixel      pointer to input pixel data
     *  @param  palette    pointer to RGB color palette
     *  @param  planeSize  number of pixels in a plane
     *  @param  bits       number of bits per sample
     */
    void convert(const T1 *pixel,
                 DiLookupTable *palette[3],
                 const unsigned long planeSize,
                 const int bits)
    {                                             // not very much optimized, but no one really uses ARGB !!
        if (Init(pixel))
        {
            register T2 value;
            const T1 offset = OFstatic_cast(T1, DicomImageClass::maxval(bits - 1));
            // use the number of input pixels derived from the length of the 'PixelData'
            // attribute), but not more than the size of the intermediate buffer
            const unsigned long count = (this->InputCount < this->Count) ? this->InputCount : this->Count;
            if (this->PlanarConfiguration)
            {
/*
                register const T1 *a = pixel;                                   // points to alpha plane
                const T1 *rgb[3];
                rgb[0] = a + this->InputCount;                                        // points to red plane
                rgb[1] = rgb[0] + this->InputCount;                                   // points to green plane
                rgb[2] = rgb[1] + this->InputCount;                                   // points to blue plane
                for (i = 0; i < count; ++i)
                {
                    value = OFstatic_cast(T2, *(a++));                          // get alpha value
                    if (value > 0)
                    {
                        for (int j = 0; j < 3; ++j)                             // set palette color
                        {
                            if (value <= palette[j]->getFirstEntry(value))
                                this->Data[j][i] = OFstatic_cast(T3, palette[j]->getFirstValue());
                            else if (value >= palette[j]->getLastEntry(value))
                                this->Data[j][i] = OFstatic_cast(T3, palette[j]->getLastValue());
                            else
                                this->Data[j][i] = OFstatic_cast(T3, palette[j]->getValue(value));
                            ++rgb[j];                                           // skip RGB values
                        }
                    }
                    else
                    {
                        for (j = 0; j < 3; ++j)                                 // copy RGB values
                            this->Data[j][i] = OFstatic_cast(T3, removeSign(*(rgb[j]++), offset));
                    }
                }
*/
                register unsigned long l;
                register unsigned long i = 0;
                register const T1 *a = pixel;                                   // points to alpha plane
                const T1 *rgb[3];
                rgb[0] = a + planeSize;                                         // points to red plane
                rgb[1] = rgb[0] + planeSize;                                    // points to green plane
                rgb[2] = rgb[1] + planeSize;                                    // points to blue plane
                while (i < count)
                {
                    /* convert a single frame */
                    for (l = planeSize; (l != 0) && (i < count); --l, ++i)
                    {
                        value = OFstatic_cast(T2, *(a++));                          // get alpha value
                        if (value > 0)
                        {
                            for (int j = 0; j < 3; ++j)                             // set palette color
                            {
                                if (value <= palette[j]->getFirstEntry(value))
                                    this->Data[j][i] = OFstatic_cast(T3, palette[j]->getFirstValue());
                                else if (value >= palette[j]->getLastEntry(value))
                                    this->Data[j][i] = OFstatic_cast(T3, palette[j]->getLastValue());
                                else
                                    this->Data[j][i] = OFstatic_cast(T3, palette[j]->getValue(value));
                                ++rgb[j];                                           // skip RGB values
                            }
                        }
                        else
                        {
                            for (int j = 0; j < 3; ++j)                             // copy RGB values
                                this->Data[j][i] = OFstatic_cast(T3, removeSign(*(rgb[j]++), offset));
                        }
                    }
                    /* jump to next frame start (skip 2 planes) */
                    a += 2 * planeSize;
                    for (int j = 0; j < 3; ++j)
                       rgb[j] += 2 * planeSize;
                }
            } else {
                register unsigned long i;
                register const T1 *p = pixel;
                for (i = 0; i < count; ++i)
                {
                    value = OFstatic_cast(T2, *(p++));                          // get alpha value
                    if (value > 0)
                    {
                        for (int j = 0; j < 3; ++j)                             // set palette color
                        {
                            if (value <= palette[j]->getFirstEntry(value))
                                this->Data[j][i] = OFstatic_cast(T3, palette[j]->getFirstValue());
                            else if (value >= palette[j]->getLastEntry(value))
                                this->Data[j][i] = OFstatic_cast(T3, palette[j]->getLastValue());
                            else
                                this->Data[j][i] = OFstatic_cast(T3, palette[j]->getValue(value));
                        }
                        p += 3;                                                 // skip RGB values
                    }
                    else
                    {
                        for (int j = 0; j < 3; ++j)                             // copy RGB values
                            this->Data[j][i] = OFstatic_cast(T3, removeSign(*(p++), offset));
                    }
                }
            }
        }
    }
};


#endif


/*
 *
 * CVS/RCS Log:
 * $Log: diargpxt.h,v $
 * Revision 1.21  2010-10-14 13:16:29  joergr
 * Updated copyright header. Added reference to COPYRIGHT file.
 *
 * Revision 1.20  2005/12/08 16:01:25  meichel
 * Changed include path schema for all DCMTK header files
 *
 * Revision 1.19  2004/04/21 10:00:31  meichel
 * Minor modifications for compilation with gcc 3.4.0
 *
 * Revision 1.18  2003/12/23 16:06:21  joergr
 * Replaced additional post-increment/decrement operators by pre-increment/
 * decrement operators.
 *
 * Revision 1.17  2003/12/23 12:38:51  joergr
 * Replaced post-increment/decrement operators by pre-increment/decrement
 * operators where appropriate (e.g. 'i++' by '++i').
 *
 * Revision 1.16  2003/12/23 11:15:07  joergr
 * Adapted type casts to new-style typecast operators defined in ofcast.h.
 * Removed leading underscore characters from preprocessor symbols (reserved
 * symbols). Updated copyright header.
 *
 * Revision 1.15  2002/06/26 16:16:07  joergr
 * Enhanced handling of corrupted pixel data and/or length.
 * Corrected decoding of multi-frame, planar images.
 *
 * Revision 1.14  2001/11/09 16:39:37  joergr
 * Removed 'inline' specifier from certain methods.
 *
 * Revision 1.13  2001/06/01 15:49:27  meichel
 * Updated copyright header
 *
 * Revision 1.12  2000/04/27 13:15:12  joergr
 * Dcmimage library code now consistently uses ofConsole for error output.
 *
 * Revision 1.11  2000/03/08 16:21:48  meichel
 * Updated copyright header.
 *
 * Revision 1.10  1999/04/28 12:51:54  joergr
 * Corrected some typos, comments and formatting.
 *
 * Revision 1.9  1999/02/03 16:47:54  joergr
 * Moved global functions maxval() and determineRepresentation() to class
 * DicomImageClass (as static methods).
 *
 * Revision 1.8  1999/01/20 14:36:22  joergr
 * Replaced invocation of getCount() by member variable Count where possible.
 *
 * Revision 1.7  1998/12/14 17:08:43  joergr
 * Added support for signed values as second entry in look-up tables
 * (= first value mapped).
 *
 * Revision 1.6  1998/11/27 13:40:11  joergr
 * Added copyright message.
 *
 * Revision 1.5  1998/07/01 08:39:18  joergr
 * Minor changes to avoid compiler warnings (gcc 2.8.1 with additional
 * options), e.g. add copy constructors.
 *
 * Revision 1.4  1998/05/11 14:53:08  joergr
 * Added CVS/RCS header to each file.
 *
 *
 */
