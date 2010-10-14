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
 *  Purpose: DicomColorMonochromeTemplate (Header)
 *
 *  Last Update:         $Author: joergr $
 *  Update Date:         $Date: 2010-10-14 13:16:29 $
 *  CVS/RCS Revision:    $Revision: 1.16 $
 *  Status:              $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#ifndef DICOMOT_H
#define DICOMOT_H

#include "dcmtk/config/osconfig.h"

#include "dcmtk/dcmimgle/dimopxt.h"
#include "dcmtk/dcmimage/dicopx.h"


/*---------------------*
 *  class declaration  *
 *---------------------*/

/** Template class to convert color image to monochrome images.
 *  (on pixel data level)
 */
template<class T>
class DiColorMonoTemplate
  : public DiMonoPixelTemplate<T>
{

 public:

    /** constructor
     *
     ** @param  pixel     intermediate representation of color pixel data
     *  @param  modality  pointer to object managing modality transform
     *  @param  red       coefficient of red pixel component
     *  @param  green     coefficient of green pixel component
     *  @param  blue      coefficient of blue pixel component
     */
    DiColorMonoTemplate(const DiColorPixel *pixel,
                        DiMonoModality *modality,
                        const double red,
                        const double green,
                        const double blue)
      : DiMonoPixelTemplate<T>(pixel, modality)
    {
        if ((pixel != NULL) && (pixel->getCount() > 0))
        {
            convert(OFstatic_cast(const T **, OFconst_cast(void *, pixel->getData())), red, green, blue);
            this->determineMinMax();
        }
    }

    /** destructor
     */
    virtual ~DiColorMonoTemplate()
    {
    }


 private:

    /** convert color pixel data to monochrome format
     *
     ** @param  pixel  intermediate representation of color pixel data
     *  @param  red    coefficient of red pixel component
     *  @param  green  coefficient of green pixel component
     *  @param  blue   coefficient of blue pixel component
     */
    void convert(const T *pixel[3],
                 const double red,
                 const double green,
                 const double blue)
    {
        if (pixel != NULL)
        {
            this->Data = new T[this->Count];
            if (this->Data != NULL)
            {
                register const T *r = pixel[0];
                register const T *g = pixel[1];
                register const T *b = pixel[2];
                register T *q = this->Data;
                register unsigned long i;
                for (i = this->Count; i != 0; i--)
                {
                    *(q++) = OFstatic_cast(T, OFstatic_cast(double, *(r++)) * red +
                                              OFstatic_cast(double, *(g++)) * green +
                                              OFstatic_cast(double, *(b++)) * blue);
                }
            }
        }
    }
};


#endif


/*
 *
 * CVS/RCS Log:
 * $Log: dicomot.h,v $
 * Revision 1.16  2010-10-14 13:16:29  joergr
 * Updated copyright header. Added reference to COPYRIGHT file.
 *
 * Revision 1.15  2005/12/08 16:01:31  meichel
 * Changed include path schema for all DCMTK header files
 *
 * Revision 1.14  2004/04/21 10:00:31  meichel
 * Minor modifications for compilation with gcc 3.4.0
 *
 * Revision 1.13  2004/02/06 11:18:18  joergr
 * Distinguish more clearly between const and non-const access to pixel data.
 *
 * Revision 1.12  2003/12/23 11:21:12  joergr
 * Adapted type casts to new-style typecast operators defined in ofcast.h.
 * Removed leading underscore characters from preprocessor symbols (reserved
 * symbols). Added missing API documentation. Updated copyright header.
 *
 * Revision 1.11  2001/11/09 16:41:34  joergr
 * Removed 'inline' specifier from certain methods.
 *
 * Revision 1.10  2001/06/01 15:49:28  meichel
 * Updated copyright header
 *
 * Revision 1.9  2000/03/08 16:21:50  meichel
 * Updated copyright header.
 *
 * Revision 1.8  1999/09/17 14:03:42  joergr
 * Enhanced efficiency of some "for" loops.
 *
 * Revision 1.7  1999/05/31 13:01:13  joergr
 * Corrected bug concerning the conversion of color images to grayscale.
 *
 * Revision 1.6  1999/04/28 12:51:58  joergr
 * Corrected some typos, comments and formatting.
 *
 * Revision 1.5  1999/01/20 14:40:41  joergr
 * Replaced invocation of getCount() by member variable Count where possible.
 *
 * Revision 1.4  1998/11/27 13:43:54  joergr
 * Added copyright message.
 *
 * Revision 1.3  1998/05/11 14:53:11  joergr
 * Added CVS/RCS header to each file.
 *
 *
 */
