/*
**
** Author:  Joerg Riesmeier
** Created: 10.12.96
**
** Module:  didocu.h
**
** Purpose: DicomDocument (Header)
**
** Last Update:      $Author: joergr $
** Update Date:      $Date: 1998-06-25 08:50:09 $
** Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmimage/include/Attic/didocu.h,v $
** CVS/RCS Revision: $Revision: 1.6 $
** Status:           $State: Exp $
**
** CVS/RCS Log at end of file
**
*/


#ifndef __DIDOCU_H
#define __DIDOCU_H

#include "osconfig.h"
#include "dctypes.h"
#include "dcxfer.h"

#include "diobjcou.h"

#include <stddef.h>


/*------------------------*
 *  forward declarations  *
 *------------------------*/

class DcmStream;
class DcmStack;
class DcmObject;
class DcmTagKey;
class DcmSequenceOfItems;



/*---------------------*
 *  class declaration  *
 *---------------------*/

class DiDocument : public DiObjectCounter
{
public:
    DiDocument(const char *, const unsigned long = 0);
    DiDocument(DcmStream &, const unsigned long = 0);
    DiDocument(DcmObject *, E_TransferSyntax, const unsigned long = 0);
    virtual ~DiDocument();
   
    inline int good() const
        { return Object != NULL; }
    inline unsigned long getFlags() const
        { return Flags; }
    
    DcmObject *search(const DcmTagKey &, DcmObject * = NULL) const;
    int search(const DcmTagKey &, DcmStack &) const;
    
    unsigned long getVM(const DcmTagKey &) const;
    
    unsigned long getValue(const DcmTagKey &, Uint16 &, const unsigned long = 0, DcmObject * = NULL) const;
    unsigned long getValue(const DcmTagKey &, Sint16 &, const unsigned long = 0) const;
    unsigned long getValue(const DcmTagKey &, Uint32 &, const unsigned long = 0) const;
    unsigned long getValue(const DcmTagKey &, Sint32 &, const unsigned long = 0) const;
    unsigned long getValue(const DcmTagKey &, double &, const unsigned long = 0) const;
    
    unsigned long getValue(const DcmTagKey &, const Uint16 *&, DcmObject * = NULL) const;
    unsigned long getValue(const DcmTagKey &, const char *&) const;

    unsigned long getSequence(const DcmTagKey &, DcmSequenceOfItems *&) const;

protected:
    void Init(DcmStream &);

private:
    DcmObject *Object;
    E_TransferSyntax Xfer;

    unsigned long Flags;
    int DeleteObject;
};


#endif


/*
**
** CVS/RCS Log:
** $Log: didocu.h,v $
** Revision 1.6  1998-06-25 08:50:09  joergr
** Added compatibility mode to support ACR-NEMA images and wrong
** palette attribute tags.
**
** Revision 1.5  1998/05/11 14:53:15  joergr
** Added CVS/RCS header to each file.
**
**
*/
