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
 *  Module:  dcmimgle
 *
 *  Author:  Joerg Riesmeier
 *
 *  Purpose: DicomOverlayImage (Header)
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 1999-03-24 17:20:20 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmimgle/include/Attic/diovlimg.h,v $
 *  CVS/RCS Revision: $Revision: 1.2 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#ifndef __DIOVLIMG_H
#define __DIOVLIMG_H

#include "osconfig.h"

#include "dimo2img.h"


/*---------------------*
 *  class declaration  *
 *---------------------*/

/** Class for standalone overlay images
 */
class DiOverlayImage
  : public DiMono2Image
{

 public:

    DiOverlayImage(const DiDocument *, const EI_Status);

    virtual ~DiOverlayImage();
};


#endif


/*
 *
 * CVS/RCS Log:
 * $Log: diovlimg.h,v $
 * Revision 1.2  1999-03-24 17:20:20  joergr
 * Added/Modified comments and formatting.
 *
 * Revision 1.1  1998/11/27 15:43:13  joergr
 * Added copyright message.
 *
 * Revision 1.2  1998/05/11 14:53:25  joergr
 * Added CVS/RCS header to each file.
 *
 *
 */
