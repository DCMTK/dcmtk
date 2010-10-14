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
 *  Purpose: DicomOverlayImage (Header)
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:16:26 $
 *  CVS/RCS Revision: $Revision: 1.8 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#ifndef DIOVLIMG_H
#define DIOVLIMG_H

#include "dcmtk/config/osconfig.h"

#include "dcmtk/dcmimgle/dimo2img.h"


/*---------------------*
 *  class declaration  *
 *---------------------*/

/** Class for standalone overlay images
 */
class DiOverlayImage
  : public DiMono2Image
{

 public:

    /** constructor
     *
     ** @param  docu    pointer to dataset (encapsulated)
     *  @param  status  current image status
     */
    DiOverlayImage(const DiDocument *docu,
                   const EI_Status status);

    /** destructor
     */
    virtual ~DiOverlayImage();
};


#endif


/*
 *
 * CVS/RCS Log:
 * $Log: diovlimg.h,v $
 * Revision 1.8  2010-10-14 13:16:26  joergr
 * Updated copyright header. Added reference to COPYRIGHT file.
 *
 * Revision 1.7  2005-12-08 16:48:02  meichel
 * Changed include path schema for all DCMTK header files
 *
 * Revision 1.6  2003/12/09 10:06:26  joergr
 * Removed leading underscore characters from preprocessor symbols (reserved
 * symbols). Updated copyright header.
 *
 * Revision 1.5  2001/06/01 15:49:49  meichel
 * Updated copyright header
 *
 * Revision 1.4  2000/03/08 16:24:22  meichel
 * Updated copyright header.
 *
 * Revision 1.3  1999/09/17 12:46:58  joergr
 * Added/changed/completed DOC++ style comments in the header files.
 *
 * Revision 1.2  1999/03/24 17:20:20  joergr
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
