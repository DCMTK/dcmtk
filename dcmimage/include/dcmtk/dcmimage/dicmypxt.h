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
 *  Purpose: DicomCMYKPixelTemplate (Header)
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:16:29 $
 *  CVS/RCS Revision: $Revision: 1.20 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#ifndef DICMYPXT_H
#define DICMYPXT_H

#include "dcmtk/config/osconfig.h"

#include "dcmtk/dcmimage/dicopxt.h"
#include "dcmtk/dcmimgle/diinpx.h"  /* gcc 3.4 needs this */


/*---------------------*
 *  class declaration  *
 *---------------------*/

/** Template class to handle CMYK pixel data
 */
template<class T1, class T2>
class DiCMYKPixelTemplate
  : public DiColorPixelTemplate<T2>
{

 public:

    /** constructor
     *
     ** @param  docu       pointer to DICOM document
     *  @param  pixel      pointer to input pixel representation
     *  @param  status     reference to status variable
     *  @param  planeSize  number of pixels in a plane
     *  @param  bits       number of bits per sample
     */
    DiCMYKPixelTemplate(const DiDocument *docu,
                        const DiInputPixel *pixel,
                        EI_Status &status,
                        const unsigned long planeSize,
                        const int bits)
      : DiColorPixelTemplate<T2>(docu, pixel, 4, status)
    {
        if ((pixel != NULL) && (this->Count > 0) && (status == EIS_Normal))
            convert(OFstatic_cast(const T1 *, pixel->getData()) + pixel->getPixelStart(), planeSize, bits);
    }

    /** destructor
     */
    virtual ~DiCMYKPixelTemplate()
    {
    }


 private:

    /** convert input pixel data to intermediate representation
     *
     ** @param  pixel      pointer to input pixel data
     *  @param  planeSize  number of pixels in a plane
     *  @param  bits       number of bits per sample
     */
    void convert(const T1 *pixel,
                 const unsigned long planeSize,
                 const int bits)
    {
        if (Init(pixel))
        {
            // use the number of input pixels derived from the length of the 'PixelData'
            // attribute), but not more than the size of the intermediate buffer
            const unsigned long count = (this->InputCount < this->Count) ? this->InputCount : this->Count;
            const T2 maxvalue = OFstatic_cast(T2, DicomImageClass::maxval(bits));
            const T1 offset = OFstatic_cast(T1, DicomImageClass::maxval(bits - 1));
            register const T1 *p = pixel;
            if (this->PlanarConfiguration)
            {
/*
                register const T1 *k;
                register T2 *q;
                for (int j = 0; j < 3; ++j)
                {
                    q = this->Data[j];
                    k = pixel + 3 * this->InputCount;                     // beginning of 'black' plane
                    for (i = count; i != 0; --i)
                        *(q++) = maxvalue - removeSign(*(p++), offset) - removeSign(*(k++), offset);
                }
*/
                register const T1 *k;
                register T2 *q;
                register unsigned long l;
                register unsigned long i = 0;
                while (i < count)
                {
                    /* store current pixel index */
                    const unsigned long iStart = i;
                    /* beginning of 'black' plane */
                    const T1 *kStart = p + 3 * planeSize;
                    /* for all planes ... */
                    for (int j = 0; j < 3; ++j)
                    {
                        q = this->Data[j] + iStart;
                        /* reset to beginning of 'black' plane */
                        k = kStart;
                        /* convert a single plane */
                        for (l = planeSize, i = iStart; (l != 0) && (i < count); --l, ++i)
                            *(q++) = maxvalue - removeSign(*(p++), offset) - removeSign(*(k++), offset);
                    }
                    /* skip black plane */
                    p += planeSize;
                }
            }
            else
            {
                register T1 k;
                register int j;
                register unsigned long i;
                for (i = 0; i < count; ++i)
                {
                    k = *(p + 3);
                    for (j = 0; j < 3; ++j)
                        this->Data[j][i] = maxvalue - removeSign(*(p++), offset) - removeSign(k, offset);
                    ++p;                                            // skip 'black'
                }
            }
        }
    }
};


#endif


/*
 *
 * CVS/RCS Log:
 * $Log: dicmypxt.h,v $
 * Revision 1.20  2010-10-14 13:16:29  joergr
 * Updated copyright header. Added reference to COPYRIGHT file.
 *
 * Revision 1.19  2005/12/08 16:01:27  meichel
 * Changed include path schema for all DCMTK header files
 *
 * Revision 1.18  2004/04/21 10:00:31  meichel
 * Minor modifications for compilation with gcc 3.4.0
 *
 * Revision 1.17  2003/12/23 16:06:21  joergr
 * Replaced additional post-increment/decrement operators by pre-increment/
 * decrement operators.
 *
 * Revision 1.16  2003/12/23 12:38:51  joergr
 * Replaced post-increment/decrement operators by pre-increment/decrement
 * operators where appropriate (e.g. 'i++' by '++i').
 *
 * Revision 1.15  2003/12/23 11:17:23  joergr
 * Adapted type casts to new-style typecast operators defined in ofcast.h.
 * Removed leading underscore characters from preprocessor symbols (reserved
 * symbols). Updated copyright header.
 *
 * Revision 1.14  2002/06/26 16:16:38  joergr
 * Enhanced handling of corrupted pixel data and/or length.
 * Corrected decoding of multi-frame, planar images.
 *
 * Revision 1.13  2001/11/09 16:40:16  joergr
 * Removed 'inline' specifier from certain methods.
 *
 * Revision 1.12  2001/06/01 15:49:27  meichel
 * Updated copyright header
 *
 * Revision 1.11  2000/04/27 13:15:13  joergr
 * Dcmimage library code now consistently uses ofConsole for error output.
 *
 * Revision 1.10  2000/03/08 16:21:49  meichel
 * Updated copyright header.
 *
 * Revision 1.9  1999/09/17 14:03:42  joergr
 * Enhanced efficiency of some "for" loops.
 *
 * Revision 1.8  1999/04/28 12:51:56  joergr
 * Corrected some typos, comments and formatting.
 *
 * Revision 1.7  1999/02/03 16:48:32  joergr
 * Moved global functions maxval() and determineRepresentation() to class
 * DicomImageClass (as static methods).
 *
 * Revision 1.6  1999/01/20 14:37:05  joergr
 * Replaced invocation of getCount() by member variable Count where possible.
 *
 * Revision 1.5  1998/11/27 13:41:04  joergr
 * Added copyright message.
 *
 * Revision 1.4  1998/05/11 14:53:10  joergr
 * Added CVS/RCS header to each file.
 *
 *
 */
