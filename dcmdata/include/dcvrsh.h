/*
**
** Author: Gerd Ehlers	    01.05.94 
**
** Kuratorium OFFIS e.V.
**
** Module: dcvrsh.h
**
** Purpose:
** Interface of class DcmShortString
**
**
** Last Update:		$Author: meichel $
** Update Date:		$Date: 1998-11-12 16:47:53 $
** Source File:		$Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/include/Attic/dcvrsh.h,v $
** CVS/RCS Revision:	$Revision: 1.6 $
** Status:		$State: Exp $
**
** CVS/RCS Log at end of file
**
*/

#ifndef DCVRSH_H
#define DCVRSH_H

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include "dctypes.h"
#include "dcchrstr.h"



class DcmShortString : public DcmCharString 
{
  public:
    DcmShortString(const DcmTag &tag, const Uint32 len = 0);
    DcmShortString(const DcmShortString& old);
    virtual ~DcmShortString(void);

    DcmShortString &operator=(const DcmShortString &obj) { DcmCharString::operator=(obj); return *this; }

    virtual DcmEVR ident(void) const { return EVR_SH; }

    virtual E_Condition getOFString(
	OFString & str,
	const unsigned long pos,
	OFBool normalize = OFTrue);

    virtual E_Condition getOFStringArray(
	OFString & str,
	OFBool normalize = OFTrue);

};


#endif // DCVRSH_H

/*
** CVS/RCS Log:
** $Log: dcvrsh.h,v $
** Revision 1.6  1998-11-12 16:47:53  meichel
** Implemented operator= for all classes derived from DcmObject.
**
** Revision 1.5  1997/09/11 15:13:16  hewett
** Modified getOFString method arguments by removing a default value
** for the pos argument.  By requiring the pos argument to be provided
** ensures that callers realise getOFString only gets one component of
** a multi-valued string.
**
** Revision 1.4  1997/08/29 08:32:44  andreas
** - Added methods getOFString and getOFStringArray for all
**   string VRs. These methods are able to normalise the value, i. e.
**   to remove leading and trailing spaces. This will be done only if
**   it is described in the standard that these spaces are not relevant.
**   These methods do not test the strings for conformance, this means
**   especially that they do not delete spaces where they are not allowed!
**   getOFStringArray returns the string with all its parts separated by \
**   and getOFString returns only one value of the string.
**   CAUTION: Currently getString returns a string with trailing
**   spaces removed (if dcmEnableAutomaticInputDataCorrection == OFTrue) and
**   truncates the original string (since it is not copied!). If you rely on this
**   behaviour please change your application now.
**   Future changes will ensure that getString returns the original
**   string from the DICOM object (NULL terminated) inclusive padding.
**   Currently, if you call getOF... before calling getString without
**   normalisation, you can get the original string read from the DICOM object.
**
** Revision 1.3  1996/01/05 13:23:08  andreas
** - changed to support new streaming facilities
** - more cleanups
** - merged read / write methods for block and file transfer
**
**
*/
