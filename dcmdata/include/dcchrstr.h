/*
**
** Autor:	Andreas Barth	02.12.95 -- Completely new class design
** Kuratorium OFFIS e.V.
**
** Module: dcchrstr.h
**
** Purpose:
** Interface of class DcmCharString
**
** Last Update:		$Author: meichel $
** Update Date:		$Date: 1998-11-12 16:47:37 $
** Source File:		$Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/include/Attic/dcchrstr.h,v $
** CVS/RCS Revision:	$Revision: 1.4 $
** Status:		$State: Exp $
**
** CVS/RCS Log at end of file
**
*/


#ifndef DCCHRSTR_H
#define DCCHRSTR_H

#include "osconfig.h"    /* make sure OS specific configuration is included first */

//
// This implementation does not support 16 bit character sets. Sinece 8 bit 
// character sets are supported by the class DcmByteString the class 
// DcmCharString is derived from DcmByteString without any extensions.
//
// If the extension for 16 bit character sets will be implemented this class
// must be derived directly from DcmElement. This class is designed to support
// the value representations (LO, LT, PN, SH, ST). They are a problem because 
// there value width (1, 2, .. Bytes) is specified with the element 
// SpecificCharacterSet (0008, 0005) and an implementation must support 
// differnt value width that cannot be derived from the value representation.
// 



#include "dcbytstr.h"

class DcmCharString : public DcmByteString
{
	// Dummy class stub

  public:
    DcmCharString(const DcmTag & tag, const Uint32 len);
    DcmCharString(const DcmCharString & old);
    virtual ~DcmCharString(void);

    DcmCharString &operator=(const DcmCharString &obj) { DcmByteString::operator=(obj); return *this; }
};


#endif // DCCHRSTR_H





