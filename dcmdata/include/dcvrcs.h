/*
**
** Author: Gerd Ehlers      27.04.94 -- First Creation
**         Andreas Barth    04.12.95 -- new Stream class, unique value field
** Kuratorium OFFIS e.V.
**
** Module: dcvrcs.h
**
** Purpose:
** Interface of class DcmCodeString
**
** Last Update:		$Author: meichel $
** Update Date:		$Date: 1998-11-12 16:47:46 $
** Source File:		$Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/include/Attic/dcvrcs.h,v $
** CVS/RCS Revision:	$Revision: 1.7 $
** Status:		$State: Exp $
**
** CVS/RCS Log at end of file
**
*/

#ifndef DCVRCS_H
#define DCVRCS_H

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include "dctypes.h"
#include "dcbytstr.h"



class DcmCodeString : public DcmByteString 
{
  public:
    DcmCodeString(const DcmTag &tag, const Uint32 len = 0);
    DcmCodeString( const DcmCodeString &newCS );
    virtual ~DcmCodeString();

    DcmCodeString &operator=(const DcmCodeString &obj) { DcmByteString::operator=(obj); return *this; }

    virtual DcmEVR ident(void) const { return EVR_CS; }
    
    virtual E_Condition getOFString(
	OFString & str,
	const unsigned long pos,
	OFBool normalize = OFTrue);

    virtual E_Condition getOFStringArray(
	OFString & str, 
	OFBool normalize = OFTrue);

};


#endif // DCVRCS_H

/*
** CVS/RCS Log:
** $Log: dcvrcs.h,v $
** Revision 1.7  1998-11-12 16:47:46  meichel
** Implemented operator= for all classes derived from DcmObject.
**
** Revision 1.6  1997/09/11 15:13:12  hewett
** Modified getOFString method arguments by removing a default value
** for the pos argument.  By requiring the pos argument to be provided
** ensures that callers realise getOFString only gets one component of
** a multi-valued string.
**
** Revision 1.5  1997/08/29 08:32:40  andreas
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
