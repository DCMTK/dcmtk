/*
**
** Author:  Joerg Riesmeier
** Created: 10.12.96
**
** Module:  diinpx.h
**
** Purpose: DicomInputPixel (Header)
**
** Last Update:      $Author: joergr $
** Update Date:      $Date: 1998-05-11 14:53:17 $
** Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmimage/include/Attic/diinpx.h,v $
** CVS/RCS Revision: $Revision: 1.3 $
** Status:           $State: Exp $
**
** CVS/RCS Log at end of file
**
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
** Revision 1.3  1998-05-11 14:53:17  joergr
** Added CVS/RCS header to each file.
**
**
*/
