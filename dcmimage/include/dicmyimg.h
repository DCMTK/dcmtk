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
 *  Purpose: DicomCMYKImage (Header) 
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 1998-11-27 13:40:40 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmimage/include/Attic/dicmyimg.h,v $
 *  CVS/RCS Revision: $Revision: 1.3 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#ifndef __DICMYIMG_H
#define __DICMYIMG_H

#include "osconfig.h"

#include "dicoimg.h"


/*---------------------*
 *  class declaration  *
 *---------------------*/

/** Class for CMYK images
 */
class DiCMYKImage : public DiColorImage
{
 public:
    DiCMYKImage(const DiDocument *, const EI_Status);
    virtual ~DiCMYKImage();
};


#endif
                        

/*
**
** CVS/RCS Log:
** $Log: dicmyimg.h,v $
** Revision 1.3  1998-11-27 13:40:40  joergr
** Added copyright message.
**
** Revision 1.2  1998/05/11 14:53:09  joergr
** Added CVS/RCS header to each file.
**
**
*/
