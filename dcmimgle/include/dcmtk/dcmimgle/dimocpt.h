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
 *  Module:  dcmimgle
 *
 *  Author:  Joerg Riesmeier
 *
 *  Purpose: DicomMonochromeCopyTemplate (Header)
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:16:26 $
 *  CVS/RCS Revision: $Revision: 1.15 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#ifndef DIMOCPT_H
#define DIMOCPT_H

#include "dcmtk/config/osconfig.h"
#include "dcmtk/ofstd/ofbmanip.h"
#include "dcmtk/ofstd/ofcast.h"

#include "dcmtk/dcmimgle/dimopxt.h"


/*---------------------*
 *  class declaration  *
 *---------------------*/

/** Template class to copy monochrome pixel data
 */
template<class T>
class DiMonoCopyTemplate
  : public DiMonoPixelTemplate<T>
{

 public:

    /** constructor
     *
     ** @param  pixel   pointer to monochrome intermediate representation of pixel data
     *  @param  fstart  first frame to be copied
     *  @param  fcount  number of frames to be copied
     *  @param  fsize   size of one frame (in bytes)
     */
    DiMonoCopyTemplate(const DiMonoPixel *pixel,
                       const unsigned long fstart,
                       const unsigned long fcount,
                       const unsigned long fsize)
      : DiMonoPixelTemplate<T>(pixel, fcount * fsize)
    {
        if ((pixel != NULL) && (pixel->getCount() > 0))
        {
            if ((pixel->getCount() > fstart * fsize) && (pixel->getCount() >= (fstart + fcount) * fsize))
                copy(OFstatic_cast(const T *, pixel->getData()) + fstart * fsize);
            /* need to determine the global min/max value */
            this->determineMinMax();
        }
    }

    /** destructor
     */
    ~DiMonoCopyTemplate()
    {
    }


 private:

    /** copy specified amount of pixel data
     *
     ** @param  pixel  array of pixel data to be copied
     */
    inline void copy(const T *pixel)
    {
        if (pixel != NULL)
        {
            this->Data = new T[this->getCount()];
            if (this->Data != NULL)
                OFBitmanipTemplate<T>::copyMem(pixel, this->Data, this->getCount());
        }
    }
};


#endif


/*
 *
 * CVS/RCS Log:
 * $Log: dimocpt.h,v $
 * Revision 1.15  2010-10-14 13:16:26  joergr
 * Updated copyright header. Added reference to COPYRIGHT file.
 *
 * Revision 1.14  2010-03-01 09:08:47  uli
 * Removed some unnecessary include directives in the headers.
 *
 * Revision 1.13  2007-12-10 16:11:53  joergr
 * Fixed compilation problem with gcc 4.1.2 on Linux x86_64.
 *
 * Revision 1.12  2007/02/08 17:10:27  joergr
 * Need to determine global min/max value after copying pixel data.
 *
 * Revision 1.11  2005/12/08 16:47:48  meichel
 * Changed include path schema for all DCMTK header files
 *
 * Revision 1.10  2004/04/21 10:00:36  meichel
 * Minor modifications for compilation with gcc 3.4.0
 *
 * Revision 1.9  2003/12/08 18:26:50  joergr
 * Adapted type casts to new-style typecast operators defined in ofcast.h.
 * Removed leading underscore characters from preprocessor symbols (reserved
 * symbols). Updated copyright header.
 *
 * Revision 1.8  2001/06/01 15:49:44  meichel
 * Updated copyright header
 *
 * Revision 1.7  2000/09/12 10:04:44  joergr
 * Corrected bug: wrong parameter for attribute search routine led to crashes
 * when multiple pixel data attributes were contained in the dataset (e.g.
 * IconImageSequence). Added new checking routines to avoid crashes when
 * processing corrupted image data.
 *
 * Revision 1.6  2000/03/08 16:24:18  meichel
 * Updated copyright header.
 *
 * Revision 1.5  1999/09/17 12:23:56  joergr
 * Added/changed/completed DOC++ style comments in the header files.
 *
 * Revision 1.4  1999/04/28 14:49:19  joergr
 * Changed comments/formatting.
 *
 * Revision 1.2  1999/02/11 16:01:56  joergr
 * Corrected some typos and formatting.
 *
 * Revision 1.1  1998/11/27 15:15:50  joergr
 * Added copyright message.
 * Introduced new method and corresponding classes to create a copy of a
 * dcmimage object (optional parameters: frame start and count).
 *
 *
 */
