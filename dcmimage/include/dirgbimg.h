/*
 *
 *  Copyright (C) 1996-2000, OFFIS
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
 *  Purpose: DicomRGBImage (Header)
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 2000-03-08 16:21:53 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmimage/include/Attic/dirgbimg.h,v $
 *  CVS/RCS Revision: $Revision: 1.5 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#ifndef __DIRGBIMG_H
#define __DIRGBIMG_H

#include "osconfig.h"

#include "dicoimg.h"


/*---------------------*
 *  class declaration  *
 *---------------------*/

/** Class for RGB images
 */
class DiRGBImage
  : public DiColorImage
{

 public:

    DiRGBImage(const DiDocument *docu,
               const EI_Status status);

    virtual ~DiRGBImage();
};


#endif


/*
 *
 * CVS/RCS Log:
 * $Log: dirgbimg.h,v $
 * Revision 1.5  2000-03-08 16:21:53  meichel
 * Updated copyright header.
 *
 * Revision 1.4  1999/04/28 12:52:02  joergr
 * Corrected some typos, comments and formatting.
 *
 * Revision 1.3  1998/11/27 14:18:19  joergr
 * Added copyright message.
 *
 * Revision 1.2  1998/05/11 14:53:28  joergr
 * Added CVS/RCS header to each file.
 *
 *
 */
