/*
**
** Author:  Joerg Riesmeier
** Created: 16.12.96
**
** Module:  dimopx.h
**
** Purpose: DicomMonochromePixel (Header)
**
** Last Update:      $Author: joergr $
** Update Date:      $Date: 1998-05-11 14:53:23 $
** Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmimage/include/Attic/dimopx.h,v $
** CVS/RCS Revision: $Revision: 1.3 $
** Status:           $State: Exp $
**
** CVS/RCS Log at end of file
**
*/


#ifndef __DIMOPX_H
#define __DIMOPX_H

#include "osconfig.h"
#include "dctypes.h"

#include "diutils.h"


/*------------------------*
 *  forward declarations  *
 *------------------------*/

class DiInputPixel;
class DiColorPixel;
class DiMonoModality;


/*---------------------*
 *  class declaration  *
 *---------------------*/

class DiMonoPixel 
{
 public:
    DiMonoPixel(const unsigned long);
    DiMonoPixel(const DiInputPixel *, DiMonoModality *);
    virtual ~DiMonoPixel();
    
    virtual EP_Representation getRepresentation() const = 0;
    virtual void *getData() const = 0;

    virtual int getMinMaxValues(double &, double &) const = 0;

    virtual int getMinMaxWindow(const int, double &, double &) = 0;
    virtual int getHistogramWindow(const double, double &, double &) = 0;
    
    inline unsigned long getCount() const
        { return Count; };

 protected:
    DiMonoPixel(const DiColorPixel *);
    DiMonoPixel(const DiMonoPixel *, const unsigned long);
    
    DiMonoModality *Modality;

 private:
    unsigned long Count;
};


#endif


/*
**
** CVS/RCS Log:
** $Log: dimopx.h,v $
** Revision 1.3  1998-05-11 14:53:23  joergr
** Added CVS/RCS header to each file.
**
**
*/
