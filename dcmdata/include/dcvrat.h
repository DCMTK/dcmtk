/*
**
** Author: Gerd Ehlers      27.04.94 -- First Creation
**         Andreas Barth    04.12.95 -- new Stream class, unique value field
** Kuratorium OFFIS e.V.
**
** Module: dcvrat.h
**
** Purpose:
** Interface of class DcmAttributeTag
**
** Last Update:		$Author: andreas $
** Update Date:		$Date: 1996-04-16 16:01:37 $
** Source File:		$Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/include/Attic/dcvrat.h,v $
** CVS/RCS Revision:	$Revision: 1.6 $
** Status:		$State: Exp $
**
** CVS/RCS Log at end of file
**
*/

#ifndef DCVRAT_H
#define DCVRAT_H

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include "dctypes.h"
#include "dcelem.h"



class DcmAttributeTag : public DcmElement 
{
public:
    DcmAttributeTag(const DcmTag &tag, const Uint32 len = 0);
    DcmAttributeTag( const DcmAttributeTag &newAT );
    virtual ~DcmAttributeTag();

    virtual DcmEVR ident() const { return EVR_AT; }
    virtual void print(const int level = 0);
    virtual unsigned long getVM();

    virtual E_Condition put(const Uint16 * attrValue,	  // Tags
                            const unsigned long tagNum);  // number of tags

    virtual E_Condition put(const DcmTagKey & attrTag);

    virtual E_Condition put(const DcmTagKey & attrTag, 	    // new Tag
			    const unsigned long position);  // pos. in Tag array

    virtual E_Condition put(const char * val);

    virtual E_Condition get(DcmTag & attrTag, const unsigned long pos = 0);
    virtual E_Condition get(Uint16 * & attributeTags);
    Uint16 * get(void);
    DcmTag get(const unsigned long num);

    virtual E_Condition verify(const BOOL autocorrect = FALSE);
};


#endif // DCVRAT_H

/*
** CVS/RCS Log:
** $Log: dcvrat.h,v $
** Revision 1.6  1996-04-16 16:01:37  andreas
** - put methods for AttributeTag with DcmTagKey Parameter
** - better support for NULL values
**
** Revision 1.5  1996/01/29 13:38:15  andreas
** - new put method for every VR to put value as a string
** - better and unique print methods
**
** Revision 1.4  1996/01/09 11:06:17  andreas
** New Support for Visual C++
** Correct problems with inconsistent const declarations
**
** Revision 1.3  1996/01/05 13:23:03  andreas
** - changed to support new streaming facilities
** - more cleanups
** - merged read / write methods for block and file transfer
**
**
**
*/
