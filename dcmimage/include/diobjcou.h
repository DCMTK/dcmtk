/*
**
** Author:  Joerg Riesmeier
** Created: 03.01.97
**
** Module:  diobjcou.h
**
** Purpose: DicomObjectCounter (Header)
**
** Last Update:      $Author: joergr $
** Update Date:      $Date: 1998-05-11 14:53:24 $
** Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmimage/include/Attic/diobjcou.h,v $
** CVS/RCS Revision: $Revision: 1.3 $
** Status:           $State: Exp $
**
** CVS/RCS Log at end of file
**
*/


#ifndef __DIOBJCOU_H
#define __DIOBJCOU_H

#include "osconfig.h"


/*---------------------*
 *  class declaration  *
 *---------------------*/

class DiObjectCounter
{
 public:
    inline void addReference()
        { Counter++; }
    inline void removeReference()
        { if (--Counter == 0) delete this; }

    unsigned long Counter;

 protected:
    DiObjectCounter() : Counter(1)
        { }
    virtual ~DiObjectCounter()
        { }
    
 private:
};


#endif


/*
**
** CVS/RCS Log:
** $Log: diobjcou.h,v $
** Revision 1.3  1998-05-11 14:53:24  joergr
** Added CVS/RCS header to each file.
**
**
*/
