/*
**
** Author: Andrew Hewett    18.12.97 -- First Creation
** Kuratorium OFFIS e.V.
**
** Module: dcvrut.h
**
** Purpose:
** Interface of class DcmUnlimitedText
** Value Representation UT is defined in Correction Proposal 101
**
** Last Update:		$Author: hewett $
** Update Date:		$Date: 1998-01-19 13:19:40 $
** Source File:		$Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/include/Attic/dcvrut.h,v $
** CVS/RCS Revision:	$Revision: 1.1 $
** Status:		$State: Exp $
**
** CVS/RCS Log at end of file
**
*/

#ifndef DCVRUT_H
#define DCVRUT_H

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include "dctypes.h"
#include "dcchrstr.h"



class DcmUnlimitedText : public DcmCharString 
{
  public:
    DcmUnlimitedText(const DcmTag &tag, const Uint32 len = 0);
    DcmUnlimitedText( const DcmUnlimitedText& old );
    virtual ~DcmUnlimitedText();

    virtual DcmEVR ident() const { return EVR_UT; }

    virtual E_Condition getOFString(
	OFString & str,
	const unsigned long pos,
	OFBool normalize = OFTrue);

    virtual E_Condition getOFStringArray(
	OFString & str, 
	OFBool normalize = OFTrue);
};


#endif // DCVRUT_H

/*
** CVS/RCS Log:
** $Log: dcvrut.h,v $
** Revision 1.1  1998-01-19 13:19:40  hewett
** Initial version.
**
**
*/
