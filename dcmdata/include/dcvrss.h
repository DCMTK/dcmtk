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
 *  Author:  Gerd Ehlers, Andreas Barth
 *
 *  Purpose: Interface of class DcmSignedShort
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2000-02-10 10:50:55 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/include/Attic/dcvrss.h,v $
 *  CVS/RCS Revision: $Revision: 1.10 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#ifndef DCVRSS_H
#define DCVRSS_H

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcerror.h"
#include "dctypes.h"
#include "dcelem.h"



class DcmSignedShort : public DcmElement 
{
public:
    DcmSignedShort(const DcmTag &tag, const Uint32 len = 0);
    DcmSignedShort(const DcmSignedShort& old );
    virtual ~DcmSignedShort(void);

    DcmSignedShort &operator=(const DcmSignedShort &obj) { DcmElement::operator=(obj); return *this; }

    virtual DcmEVR ident(void) const { return EVR_SS; }
    virtual void print(ostream & out = cout, const OFBool showFullData = OFTrue,
		       const int level = 0, const char *pixelFileName = NULL,
		       size_t *pixelCounter = NULL);
    virtual unsigned long getVM(void);

    virtual E_Condition putSint16Array(const Sint16 * sintVal,
				       const unsigned long numSints); 

    virtual E_Condition putSint16(const Sint16 sintVal,	 // one Sint16
				  const unsigned long numSint = 0);     
                                                         // at any position

    virtual E_Condition putString(const char * value);  // Sint16 as Strings

    virtual E_Condition getSint16(Sint16 & sintVal, 
				  const unsigned long pos = 0);
    virtual E_Condition getSint16Array(Sint16 * & sintVals);

    virtual E_Condition verify(const OFBool autocorrect = OFFalse);
};


#endif // DCVRSS_H

/*
** CVS/RCS Log:
** $Log: dcvrss.h,v $
** Revision 1.10  2000-02-10 10:50:55  joergr
** Added new feature to dcmdump (enhanced print method of dcmdata): write
** pixel data/item value fields to raw files.
**
** Revision 1.9  1999/03/31 09:25:07  meichel
** Updated copyright header in module dcmdata
**
** Revision 1.8  1998/11/12 16:47:55  meichel
** Implemented operator= for all classes derived from DcmObject.
**
** Revision 1.7  1997/07/21 08:25:16  andreas
** - Replace all boolean types (BOOLEAN, CTNBOOLEAN, DICOM_BOOL, BOOL)
**   with one unique boolean type OFBool.
**
** Revision 1.6  1997/04/18 08:13:32  andreas
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
** Revision 1.5  1996/08/05 08:45:36  andreas
** new print routine with additional parameters:
**         - print into files
**         - fix output length for elements
** corrected error in search routine with parameter ESM_fromStackTop
**
** Revision 1.4  1996/01/29 13:38:18  andreas
** - new put method for every VR to put value as a string
** - better and unique print methods
**
** Revision 1.3  1996/01/05 13:23:09  andreas
** - changed to support new streaming facilities
** - more cleanups
** - merged read / write methods for block and file transfer
**
*/

