/*
**
** Autor:	Andreas Barth	02.12.95 -- Completely new class design
** Kuratorium OFFIS e.V.
**
** Module: dcchrstr.cc
**
** Purpose:
** Implementation of class DcmCharString
**
** Last Update:		$Author: andreas $
** Update Date:		$Date: 1996-01-05 13:27:33 $
** Source File:		$Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/libsrc/dcchrstr.cc,v $
** CVS/RCS Revision:	$Revision: 1.3 $
** Status:		$State: Exp $
**
** CVS/RCS Log at end of file
**
*/


#include "osconfig.h"    /* make sure OS specific configuration is included first */

//
// This implementation does not support 16 bit character sets. Sinece 8 bit 
// character sets are supported by the class DcmByteString the class 
// DcmCharString is derived from DcmByteString without any extensions.
// No special implementation is necessary.
//
// If the extension for 16 bit character sets will be implemented this class
// must be derived directly from DcmElement. This class is designed to support
// the value representations (LO, LT, PN, SH, ST). They are a problem because 
// there value width (1, 2, .. Bytes) is specified with the element 
// SpecificCharacterSet (0008, 0005) and an implementation must support 
// differnt value width that cannot be derived from the value representation.
// 


#include "dcchrstr.h"

DcmCharString::DcmCharString(const DcmTag & tag, const Uint32 len)
:DcmByteString(tag, len)
{
}

DcmCharString::DcmCharString(const DcmCharString & old, const DcmEVR oldIdent)
:DcmByteString(old, oldIdent)
{
}

DcmCharString::~DcmCharString(void)
{
}
