/*
 *
 *  Copyright (C) 1994-2000, OFFIS
 *
 *  This software and supporting documentation were developed by
 *
 *    Kuratorium OFFIS e.V.
 *    Healthcare Information and Communication Systems
 *    Escherweg 2
 *    D-26121 Oldenburg, Germany
 *
 *  THIS SOFTWARE IS MADE AVAILABLE,  AS IS,  AND OFFIS MAKES NO  WARRANTY
 *  REGARDING  THE  SOFTWARE,  ITS  PERFORMANCE,  ITS  MERCHANTABILITY  OR
 *  FITNESS FOR ANY PARTICULAR USE, FREEDOM FROM ANY COMPUTER DISEASES  OR
 *  ITS CONFORMITY TO ANY SPECIFICATION. THE ENTIRE RISK AS TO QUALITY AND
 *  PERFORMANCE OF THE SOFTWARE IS WITH THE USER.
 *
 *  Module:  dcmdata
 *
 *  Author:  Gerd Ehlers
 *
 *  Purpose: Interface of class DcmPersonName
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 2000-03-08 16:26:25 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/include/Attic/dcvrpn.h,v $
 *  CVS/RCS Revision: $Revision: 1.8 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#ifndef DCVRPN_H
#define DCVRPN_H

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include "dctypes.h"
#include "dcchrstr.h"



class DcmPersonName : public DcmCharString 
{
  public:
    DcmPersonName(const DcmTag &tag, const Uint32 len = 0);
    DcmPersonName(const DcmPersonName& old);
    virtual ~DcmPersonName(void);

    DcmPersonName &operator=(const DcmPersonName &obj) { DcmCharString::operator=(obj); return *this; }

    virtual DcmEVR ident(void) const { return EVR_PN; }

    virtual E_Condition getOFString(
	OFString & str,
	const unsigned long pos,
	OFBool normalize = OFTrue);

    virtual E_Condition getOFStringArray(
	OFString & str,
	OFBool normalize = OFTrue);

};


#endif // DCVRPN_H

/*
** CVS/RCS Log:
** $Log: dcvrpn.h,v $
** Revision 1.8  2000-03-08 16:26:25  meichel
** Updated copyright header.
**
** Revision 1.7  1999/03/31 09:25:04  meichel
** Updated copyright header in module dcmdata
**
** Revision 1.6  1998/11/12 16:47:52  meichel
** Implemented operator= for all classes derived from DcmObject.
**
** Revision 1.5  1997/09/11 15:13:16  hewett
** Modified getOFString method arguments by removing a default value
** for the pos argument.  By requiring the pos argument to be provided
** ensures that callers realise getOFString only gets one component of
** a multi-valued string.
**
** Revision 1.4  1997/08/29 08:32:43  andreas
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
*/
