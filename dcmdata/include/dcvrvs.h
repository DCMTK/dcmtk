/*
**
** Author: Andrew Hewett    18.12.97 -- First Creation
** Kuratorium OFFIS e.V.
**
** Module: dcvrvs.h
**
** Purpose:
** Interface of class DcmVirtualString
** Value Representation VS is defined in Correction Proposal 101
**
** Last Update:		$Author: hewett $
** Update Date:		$Date: 1998-01-19 13:19:41 $
** Source File:		$Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/include/Attic/dcvrvs.h,v $
** CVS/RCS Revision:	$Revision: 1.1 $
** Status:		$State: Exp $
**
** CVS/RCS Log at end of file
**
*/

#ifndef DCVRVS_H
#define DCVRVS_H

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include "dctypes.h"
#include "dcchrstr.h"



class DcmVirtualString : public DcmCharString 
{
  public:
    DcmVirtualString(const DcmTag &tag, const Uint32 len = 0);
    DcmVirtualString( const DcmVirtualString& old );
    virtual ~DcmVirtualString();

    virtual DcmEVR ident() const { return EVR_VS; }

    virtual E_Condition getOFString(
	OFString & str,
	const unsigned long pos,
	OFBool normalize = OFTrue);

    virtual E_Condition getOFStringArray(
	OFString & str, 
	OFBool normalize = OFTrue);
};


#endif // DCVRVS_H

/*
** CVS/RCS Log:
** $Log: dcvrvs.h,v $
** Revision 1.1  1998-01-19 13:19:41  hewett
** Initial version.
**
**
*/
