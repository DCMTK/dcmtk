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
 *  Module:  dcmimage
 *
 *  Author:  Joerg Riesmeier
 *
 *  Purpose: DicomColorMonochromeTemplate (Header)
 *
 *  Last Update:         $Author: joergr $
 *  Update Date:         $Date: 1999-09-17 14:03:42 $
 *  Source File:         $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmimage/include/Attic/dicomot.h,v $
 *  CVS/RCS Revision:    $Revision: 1.8 $
 *  Status:              $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#ifndef __DICOMOT_H
#define __DICOMOT_H

#include "osconfig.h"

#include "dimopxt.h"
#include "dicopx.h"


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

    DiColorMonoTemplate(const DiColorPixel *pixel,
                        DiMonoModality *modality,
                        const double red,
                        const double green,
                        const double blue)
      : DiMonoPixelTemplate<T>(pixel, modality)
    {
        if ((pixel != NULL) && (pixel->getCount() > 0))
        {
            convert((const T **)pixel->getData(), red, green, blue);
            determineMinMax();
        }
    }
    
    virtual ~DiColorMonoTemplate()
    {
    }


 private:

    inline void convert(const T *pixel[3],
                        const double red,
                        const double green,
                        const double blue)
    {
        if (pixel != NULL)
        {
            Data = new T[Count];
            if (Data != NULL)
            {
                register const T *r = pixel[0];
                register const T *g = pixel[1];
                register const T *b = pixel[2];
                register T *q = Data;
                register unsigned long i;
                for (i = Count; i != 0; i--)
                    *(q++) = (T)((double)*(r++) * red + (double)*(g++) * green + (double)*(b++) * blue);
            }
        }
    }
};


#endif


/*
 *
 * CVS/RCS Log:
 * $Log: dicomot.h,v $
 * Revision 1.8  1999-09-17 14:03:42  joergr
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
