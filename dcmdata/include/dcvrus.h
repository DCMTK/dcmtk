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
 *  Author:  Gerd Ehlers, Andreas Barth
 *
 *  Purpose: Interface of class DcmUnsignedShort
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 2000-03-08 16:26:27 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/include/Attic/dcvrus.h,v $
 *  CVS/RCS Revision: $Revision: 1.12 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#ifndef DCVRUS_H
#define DCVRUS_H

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include "ofconsol.h"
#include "dcerror.h"
#include "dctypes.h"
#include "dcelem.h"



class DcmUnsignedShort : public DcmElement 
{
public:
    DcmUnsignedShort(const DcmTag &tag, const Uint32 len = 0);
    DcmUnsignedShort(const DcmUnsignedShort& old );
    virtual ~DcmUnsignedShort(void);

    DcmUnsignedShort &operator=(const DcmUnsignedShort &obj) { DcmElement::operator=(obj); return *this; }
    virtual DcmEVR ident(void) const { return EVR_US; }
    virtual void print(ostream & out = COUT, const OFBool showFullData = OFTrue,
		       const int level = 0, const char *pixelFileName = NULL,
		       size_t *pixelCounter = NULL);
    virtual unsigned long getVM(void);

    virtual E_Condition putUint16Array(const Uint16 * uintVal,
				       const unsigned long numUints);  

    virtual E_Condition putUint16(const Uint16 uintVal,	 // one Uint16 at any
				  const unsigned long position = 0);  // position

    virtual E_Condition putString(const char * value);  // Uint16 as Strings

    virtual E_Condition getUint16Array(Uint16 * & uintVals);
    virtual E_Condition getUint16(Uint16 & uintVal, const unsigned long pos = 0);

    virtual E_Condition verify(const OFBool autocorrect = OFFalse);
};


#endif // DCVRUS_H

/*
** CVS/RCS Log:
** $Log: dcvrus.h,v $
** Revision 1.12  2000-03-08 16:26:27  meichel
** Updated copyright header.
**
** Revision 1.11  2000/03/03 14:05:29  meichel
** Implemented library support for redirecting error messages into memory
**   instead of printing them to stdout/stderr for GUI applications.
**
** Revision 1.10  2000/02/10 10:50:56  joergr
** Added new feature to dcmdump (enhanced print method of dcmdata): write
** pixel data/item value fields to raw files.
**
** Revision 1.9  1999/03/31 09:25:11  meichel
** Updated copyright header in module dcmdata
**
** Revision 1.8  1998/11/12 16:47:58  meichel
** Implemented operator= for all classes derived from DcmObject.
**
** Revision 1.7  1997/07/21 08:25:18  andreas
** - Replace all boolean types (BOOLEAN, CTNBOOLEAN, DICOM_BOOL, BOOL)
**   with one unique boolean type OFBool.
**
** Revision 1.6  1997/04/18 08:13:34  andreas
** - The put/get-methods for all VRs did not conform to the C++-Standard
**   draft. Some Compilers (e.g. SUN-C++ Compiler, Metroworks
**   CodeWarrier, etc.) create many warnings concerning the hiding of
**   overloaded get methods in all derived classes of DcmElement.
**   So the interface of all value representation classes in the
**   library are changed rapidly, e.g.
**   E_Condition get(Uint16 & value, const unsigned long pos);
**   becomes
**   E_Condition getUint16(Uint16 & value, const unsigned long pos);
**   All (retired) "returntype get(...)" methods are deleted.
**   For more information see dcmdata/include/dcelem.h
**
** Revision 1.5  1996/08/05 08:45:41  andreas
** new print routine with additional parameters:
**         - print into files
**         - fix output length for elements
** corrected error in search routine with parameter ESM_fromStackTop
**
** Revision 1.4  1996/01/29 13:38:19  andreas
** - new put method for every VR to put value as a string
** - better and unique print methods
**
** Revision 1.3  1996/01/05 13:23:12  andreas
** - changed to support new streaming facilities
** - more cleanups
** - merged read / write methods for block and file transfer
**
*/
