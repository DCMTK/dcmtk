/*
 *
 *  Copyright (C) 1996-2001, OFFIS
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
 *  Purpose: DicomColorCopyTemplate (Header)
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 2001-06-01 15:49:27 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmimage/include/Attic/dicocpt.h,v $
 *  CVS/RCS Revision: $Revision: 1.6 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#ifndef __DICOCPT_H
#define __DICOCPT_H

#include "osconfig.h"
#include "ofbmanip.h"

#include "dicopxt.h"


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
 
    DiColorCopyTemplate(const DiColorPixel *pixel,
                        const unsigned long fstart,
                        const unsigned long fcount,
                        const unsigned long fsize)
      : DiColorPixelTemplate<T>(pixel, fcount * fsize)
    {
        if ((pixel != NULL) && (pixel->getCount() > 0))
        {
            if ((pixel->getCount() > fstart * fsize) && (pixel->getCount() >= (fstart + fcount) * fsize))
                copy((const T **)pixel->getData(), fstart * fsize);
        }
    }

    ~DiColorCopyTemplate()
    {
    }

 
 private:
    
    inline void copy(const T *pixel[3],
                     const unsigned long offset)
    {
        if (Init(pixel))
        {
            for (int j = 0; j < 3; j++)
                OFBitmanipTemplate<T>::copyMem(pixel[j] + offset, Data[j], getCount());
        }
    }
};


#endif


/*
 *
 * CVS/RCS Log:
 * $Log: dicocpt.h,v $
 * Revision 1.6  2001-06-01 15:49:27  meichel
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
