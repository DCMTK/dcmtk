/*
**
** Author: Gerd Ehlers      01.05.94 -- First Creation
**         Andreas Barth    07.12.95 -- new Stream class, unique value field
** Kuratorium OFFIS e.V.
**
** Module: dcvrui.h
**
** Purpose:
** Interface of class DcmUniqueIdentifier
**
** Last Update:		$Author: andreas $
** Update Date:		$Date: 1997-07-21 08:25:16 $
** Source File:		$Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/include/Attic/dcvrui.h,v $
** CVS/RCS Revision:	$Revision: 1.8 $
** Status:		$State: Exp $
**
** CVS/RCS Log at end of file
**
*/

#ifndef DCVRUI_H
#define DCVRUI_H

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include "dctypes.h"
#include "dcbytstr.h"



class DcmUniqueIdentifier : public DcmByteString 
{
protected:
    E_Condition makeMachineByteString(void);

public:
    DcmUniqueIdentifier(const DcmTag &tag, const Uint32 len = 0);
    DcmUniqueIdentifier(const DcmUniqueIdentifier &old);
    virtual ~DcmUniqueIdentifier(void);
 
    virtual DcmEVR ident(void) const { return EVR_UI; }
    virtual void print(ostream & out = cout, const OFBool showFullData = OFTrue,
		       const int level = 0);

    virtual E_Condition putString(const char * value);
};


#endif // DCVRUI_H

/*
** CVS/RCS Log:
** $Log: dcvrui.h,v $
** Revision 1.8  1997-07-21 08:25:16  andreas
** - Replace all boolean types (BOOLEAN, CTNBOOLEAN, DICOM_BOOL, BOOL)
**   with one unique boolean type OFBool.
**
** Revision 1.7  1997/04/18 08:13:33  andreas
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
** Revision 1.6  1996/08/05 08:45:38  andreas
** new print routine with additional parameters:
**         - print into files
**         - fix output length for elements
** corrected error in search routine with parameter ESM_fromStackTop
**
** Revision 1.5  1996/05/30 17:19:22  hewett
** Added a makeMachineByteString() method to strip and trailing whitespace
** from a UID.
**
** Revision 1.4  1996/01/29 13:38:18  andreas
** - new put method for every VR to put value as a string
** - better and unique print methods
**
** Revision 1.3  1996/01/05 13:23:10  andreas
** - changed to support new streaming facilities
** - more cleanups
** - merged read / write methods for block and file transfer
**
**
**
*/

