/*
 *
 *  Copyright (C) 1998-2003, OFFIS
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
 *  Purpose: DicomRegister (Header)
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2003-12-17 18:18:08 $
 *  CVS/RCS Revision: $Revision: 1.8 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#ifndef DIREGIST_H
#define DIREGIST_H

#include "osconfig.h"

#include "diregbas.h"


/*---------------------*
 *  class declaration  *
 *---------------------*/

class DiRegister
  : public DiRegisterBase
{

 public:

    DiRegister();

    virtual ~DiRegister();

    virtual DiImage *createImage(const DiDocument *, const EI_Status, const EP_Interpretation);

    virtual DiMonoPixel *createMonoImageData(const DiColorImage *, const double, const double, const double);
};


/*------------------*
 *  initialization  *
 *------------------*/

static DiRegister Init_dcmimage_Module;        // registration pointer is initialized in class constructor


#endif


/*
 *
 * CVS/RCS Log:
 * $Log: diregist.h,v $
 * Revision 1.8  2003-12-17 18:18:08  joergr
 * Removed leading underscore characters from preprocessor symbols (reserved
 * symbols).
 *
 * Revision 1.7  2001/06/20 15:11:39  joergr
 * Removed old dcmimage license information.
 *
 * Revision 1.6  2001/06/01 15:49:31  meichel
 * Updated copyright header
 *
 * Revision 1.5  2000/03/10 13:15:08  meichel
 * Release 1.1.1 (Release date - 2000-03-10)
 *
 * Revision 1.4  2000/03/08 16:21:53  meichel
 * Updated copyright header.
 *
 * Revision 1.3  1999/05/31 13:01:03  joergr
 * Changed Release version and date.
 *
 * Revision 1.2  1999/05/10 09:32:36  joergr
 * Moved dcm2pnm version definition from module dcmimgle to dcmimage.
 *
 * Revision 1.1  1998/11/27 13:09:54  joergr
 * Added registration class to allow easy combination of both modules.
 * Added copyright message to all source files.
 *
 *
 */

