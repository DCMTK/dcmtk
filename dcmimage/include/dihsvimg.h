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
 *  Purpose: DicomHSVImage (Header)
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 2000-03-08 16:21:52 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmimage/include/Attic/dihsvimg.h,v $
 *  CVS/RCS Revision: $Revision: 1.5 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#ifndef __DIHSVIMG_H
#define __DIHSVIMG_H

#include "osconfig.h"

#include "dicoimg.h"


/*---------------------*
 *  class declaration  *
 *---------------------*/

/** Class for HSV images
 */
class DiHSVImage
  : public DiColorImage
{

 public:

    DiHSVImage(const DiDocument *docu,
               const EI_Status status);

    virtual ~DiHSVImage();
};


#endif


/*
 *
 * CVS/RCS Log:
 * $Log: dihsvimg.h,v $
 * Revision 1.5  2000-03-08 16:21:52  meichel
 * Updated copyright header.
 *
 * Revision 1.4  1999/04/28 12:52:00  joergr
 * Corrected some typos, comments and formatting.
 *
 * Revision 1.3  1998/11/27 13:51:30  joergr
 * Added copyright message.
 *
 * Revision 1.2  1998/05/11 14:53:15  joergr
 * Added CVS/RCS header to each file.
 * 
 *
 */
