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
 *  Purpose: DicomRegisterBase (Header)
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


#ifndef __DIREGBAS_H
#define __DIREGBAS_H

#include "osconfig.h"

#include "diutils.h"


/*------------------------*
 *  forward declarations  *
 *------------------------*/

class DiImage;
class DiColorImage;
class DiMonoPixel;
class DiDocument;
class DiRegisterBase;


/*---------------------*
 *  class declaration  *
 *---------------------*/

class DiRegisterBase
{
public:
    DiRegisterBase() { }
    virtual ~DiRegisterBase() { }
    
    virtual DiImage *createImage(const DiDocument *, const EI_Status, const EP_Interpretation) = 0;
    virtual DiMonoPixel *createMonoImageData(const DiColorImage *, const double, const double, const double) = 0;

    static DiRegisterBase *Pointer;       // global pointer to registered 'dcmimext' library
};


#endif


/*
**
** CVS/RCS Log:
** $Log: diregbas.h,v $
** Revision 1.1  1998-11-27 13:08:07  joergr
** Added registration class to allow easy combination of both modules.
** Added copyright message to all source files.
**
**
**
*/

