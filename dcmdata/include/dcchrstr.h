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
 *  Author:  Andreas Barth
 *
 *  Purpose: Interface of class DcmCharString
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 2000-03-08 16:26:11 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/include/Attic/dcchrstr.h,v $
 *  CVS/RCS Revision: $Revision: 1.6 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
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

/*
 * CVS/RCS Log:
 * $Log: dcchrstr.h,v $
 * Revision 1.6  2000-03-08 16:26:11  meichel
 * Updated copyright header.
 *
 * Revision 1.5  1999/03/31 09:24:30  meichel
 * Updated copyright header in module dcmdata
 *
 *
 */
