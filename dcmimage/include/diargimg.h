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
 *  Purpose: DiARGBImage (Header)
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 1999-04-28 12:51:54 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmimage/include/Attic/diargimg.h,v $
 *  CVS/RCS Revision: $Revision: 1.4 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */
                        

#ifndef __DIARGIMG_H
#define __DIARGIMG_H

#include "osconfig.h"

#include "dicoimg.h"


/*---------------------*
 *  class declaration  *
 *---------------------*/

/** Class for ARGB images
 */
class DiARGBImage
  : public DiColorImage
{

 public:

    DiARGBImage(const DiDocument *docu,
                const EI_Status status);

    virtual ~DiARGBImage();
};


#endif


/*
 *
 * CVS/RCS Log:
 * $Log: diargimg.h,v $
 * Revision 1.4  1999-04-28 12:51:54  joergr
 * Corrected some typos, comments and formatting.
 *
 * Revision 1.3  1998/11/27 13:39:30  joergr
 * Added copyright message.
 *
 * Revision 1.2  1998/05/11 14:53:07  joergr
 * Added CVS/RCS header to each file.
 * 
 *
 */
