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
 *  Module:  dcmimage
 *
 *  Author:  Joerg Riesmeier
 *
 *  Purpose: DicomRegister (Header)
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date $
 *  Source File:      $Source $
 *  CVS/RCS Revision: $Revision $
 *  Status:           $State $
 *
 *  CVS/RCS Log at end of file
 *
 */


#ifndef __DIREGIST_H
#define __DIREGIST_H

#include "osconfig.h"

#define OFFIS_DCMIMAGE_VERSION     "1.1.0"
#define OFFIS_DCMIMAGE_RELEASEDATE "1999/05/31"

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
 * Revision 1.3  1999-05-31 13:01:03  joergr
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

