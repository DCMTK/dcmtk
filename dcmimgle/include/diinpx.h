/*
 *
 *  Copyright (C) 1996-99, OFFIS
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
 *  Purpose: DicomInputPixel (Header)
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 1998-11-27 15:06:38 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmimgle/include/Attic/diinpx.h,v $
 *  CVS/RCS Revision: $Revision: 1.1 $
 *  Status:           $State: Exp $
 * 
 *   CVS/RCS Log at end of file
 *
 */


#ifndef __DIINPX_H
#define __DIINPX_H

#include "osconfig.h"
#include "dctypes.h"

#include "diutils.h"


/*---------------------*
 *  class declaration  *
 *---------------------*/

class DiInputPixel 
{
 public:
    DiInputPixel();
    virtual ~DiInputPixel();
    
    virtual int determineMinMax() = 0;
    
    virtual EP_Representation getRepresentation() const = 0;
    virtual void *getData() const = 0;
    virtual double getMinValue() const = 0;
    virtual double getMaxValue() const = 0;
    
    inline unsigned long getCount() const
        { return Count; };
    
 protected:
    unsigned long Count;
};


#endif


/*
**
** CVS/RCS Log:
** $Log: diinpx.h,v $
** Revision 1.1  1998-11-27 15:06:38  joergr
** Added copyright message.
**
** Revision 1.3  1998/05/11 14:53:17  joergr
** Added CVS/RCS header to each file.
**
**
*/
