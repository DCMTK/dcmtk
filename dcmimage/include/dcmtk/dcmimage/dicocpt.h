/*
 *
 *  Copyright (C) 1998-2010, OFFIS e.V.
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
 *  Purpose: DicomColorCopyTemplate (Header)
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:16:29 $
 *  CVS/RCS Revision: $Revision: 1.12 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#ifndef DICOCPT_H
#define DICOCPT_H

#include "dcmtk/config/osconfig.h"
#include "dcmtk/ofstd/ofbmanip.h"

#include "dcmtk/dcmimage/dicopxt.h"


/*---------------------*
 *  class declaration  *
 *---------------------*/

/** Template class to copy color pixel data
 */
template<class T>
class DiColorCopyTemplate
  : public DiColorPixelTemplate<T>
{

 public:

    /** constructor
     *
     ** @param  pixel   pointer to color intermediate representation of pixel data
     *  @param  fstart  first frame to be copied
     *  @param  fcount  number of frames to be copied
     *  @param  fsize   size of one frame (in bytes)
     */
    DiColorCopyTemplate(const DiColorPixel *pixel,
                        const unsigned long fstart,
                        const unsigned long fcount,
                        const unsigned long fsize)
      : DiColorPixelTemplate<T>(pixel, fcount * fsize)
    {
        if ((pixel != NULL) && (pixel->getCount() > 0))
        {
            if ((pixel->getCount() > fstart * fsize) && (pixel->getCount() >= (fstart + fcount) * fsize))
                copy(OFstatic_cast(const T **, OFconst_cast(void *, pixel->getData())), fstart * fsize);
        }
    }

    /** destructor
     */
    ~DiColorCopyTemplate()
    {
    }


 private:

    /** copy specified amount of pixel data
     *
     ** @param  pixel   array of pixel data (3 color components) to be copied
     *  @param  offset  offset to the first pixel to be copied
     */
    inline void copy(const T *pixel[3],
                     const unsigned long offset)
    {
        if (Init(pixel))
        {
            for (int j = 0; j < 3; j++)
                OFBitmanipTemplate<T>::copyMem(pixel[j] + offset, this->Data[j], this->getCount());
        }
    }
};


#endif


/*
 *
 * CVS/RCS Log:
 * $Log: dicocpt.h,v $
 * Revision 1.12  2010-10-14 13:16:29  joergr
 * Updated copyright header. Added reference to COPYRIGHT file.
 *
 * Revision 1.11  2005/12/08 16:01:28  meichel
 * Changed include path schema for all DCMTK header files
 *
 * Revision 1.10  2004/04/21 10:00:31  meichel
 * Minor modifications for compilation with gcc 3.4.0
 *
 * Revision 1.9  2004/02/06 11:18:18  joergr
 * Distinguish more clearly between const and non-const access to pixel data.
 *
 * Revision 1.8  2003/12/23 11:18:26  joergr
 * Adapted type casts to new-style typecast operators defined in ofcast.h.
 * Removed leading underscore characters from preprocessor symbols (reserved
 * symbols). Updated copyright header.
 *
 * Revision 1.7  2001/11/09 16:40:45  joergr
 * Updated/Enhanced comments.
 *
 * Revision 1.6  2001/06/01 15:49:27  meichel
 * Updated copyright header
 *
 * Revision 1.5  2000/12/08 14:06:01  joergr
 * Added new checking routines to avoid crashes when processing corrupted image
 * data.
 *
 * Revision 1.4  2000/03/08 16:21:49  meichel
 * Updated copyright header.
 *
 * Revision 1.3  1999/04/28 12:51:56  joergr
 * Corrected some typos, comments and formatting.
 *
 * Revision 1.2  1999/01/20 14:38:30  joergr
 * Corrected some typos and formatting.
 *
 * Revision 1.1  1998/11/27 15:16:38  joergr
 * Added copyright message.
 * Introduced new method and corresponding classes to create a copy of a
 * dcmimage object (optional parameters: frame start and count).
 *
 *
 */
