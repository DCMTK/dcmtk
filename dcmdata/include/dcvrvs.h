/*
 *
 *  Copyright (C) 1994-99, OFFIS
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
 *  Author:  Andrew Hewett
 *
 *  Purpose: Interface of class DcmVirtualString
 *  Value Representation VS is defined in Correction Proposal 101
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 1999-03-31 09:25:13 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/include/Attic/dcvrvs.h,v $
 *  CVS/RCS Revision: $Revision: 1.3 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
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

    DcmVirtualString &operator=(const DcmVirtualString &obj) { DcmCharString::operator=(obj); return *this; }

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
** Revision 1.3  1999-03-31 09:25:13  meichel
** Updated copyright header in module dcmdata
**
** Revision 1.2  1998/11/12 16:47:59  meichel
** Implemented operator= for all classes derived from DcmObject.
**
** Revision 1.1  1998/01/19 13:19:41  hewett
** Initial version.
**
**
*/
