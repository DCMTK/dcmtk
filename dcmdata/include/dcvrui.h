/*
**
** Author: Gerd Ehlers      01.05.94 -- First Creation
**         Andreas Barth    07.12.95 -- new Stream class, unique value field
** Kuratorium OFFIS e.V.
**
** Module: dcvrui.h
**
** Purpose:
** Interface of class DcmUniqueIdentifier
**
** Last Update:		$Author: hewett $
** Update Date:		$Date: 1996-05-30 17:19:22 $
** Source File:		$Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/include/Attic/dcvrui.h,v $
** CVS/RCS Revision:	$Revision: 1.5 $
** Status:		$State: Exp $
**
** CVS/RCS Log at end of file
**
*/

#ifndef DCVRUI_H
#define DCVRUI_H

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include "dctypes.h"
#include "dcbytstr.h"



class DcmUniqueIdentifier : public DcmByteString 
{
protected:
    E_Condition makeMachineByteString(void);

public:
    DcmUniqueIdentifier(const DcmTag &tag, const Uint32 len = 0);
    DcmUniqueIdentifier(const DcmUniqueIdentifier &old);
    virtual ~DcmUniqueIdentifier(void);
 
    virtual DcmEVR ident(void) const { return EVR_UI; }
    virtual void print(const int level = 0);

    virtual E_Condition put(const char * value);
};


#endif // DCVRUI_H

/*
** CVS/RCS Log:
** $Log: dcvrui.h,v $
** Revision 1.5  1996-05-30 17:19:22  hewett
** Added a makeMachineByteString() method to strip and trailing whitespace
** from a UID.
**
** Revision 1.4  1996/01/29 13:38:18  andreas
** - new put method for every VR to put value as a string
** - better and unique print methods
**
** Revision 1.3  1996/01/05 13:23:10  andreas
** - changed to support new streaming facilities
** - more cleanups
** - merged read / write methods for block and file transfer
**
**
**
*/

