/*
 *
 *  Copyright (C) 1998-99, OFFIS
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
 *  Purpose: DicomMonochromeCopyTemplate (Header)
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 1998-11-27 15:15:50 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmimgle/include/Attic/dimocpt.h,v $
 *  CVS/RCS Revision: $Revision: 1.1 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#ifndef __DIMOCPT_H
#define __DIMOCPT_H

#include "osconfig.h"
#include "dctypes.h"
#include "ofbmanip.h"

#include "dimopxt.h"


/*---------------------*
 *  class declaration  *
 *---------------------*/

template<class T>
class DiMonoCopyTemplate : public DiMonoPixelTemplate<T>
{
 public:
    DiMonoCopyTemplate(const DiMonoPixel *pixel,
                       const unsigned long fstart,
                       const unsigned long fcount,
                       const unsigned long fsize)
      : DiMonoPixelTemplate<T>(pixel, fcount * fsize)
    {
        if ((pixel != NULL) && (pixel->getCount() > 0))
            copy((const T *)pixel->getData() + fstart * fsize);
    }

    ~DiMonoCopyTemplate()
    {
    }

 private:
    inline void copy(const T *pixel)
    {
        if (pixel != NULL)
        {
            Data = new T[getCount()];
            if (Data != NULL)
                OFBitmanipTemplate<T>::copyMem(pixel, Data, getCount());
        }
    }
};


#endif


/*
**
** CVS/RCS Log:
** $Log: dimocpt.h,v $
** Revision 1.1  1998-11-27 15:15:50  joergr
** Added copyright message.
** Introduced new method and corresponding classes to create a copy of a
** dcmimage object (optional parameters: frame start and count).
**
**
**
*/
