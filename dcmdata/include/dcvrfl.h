/*
**
** Author: Gerd Ehlers      01.05.94 -- First Creation
**         Andreas Barth    05.12.95 -- new Stream class, unique value field
** Kuratorium OFFIS e.V.
**
** Module: dcvrfl.h
**
** Purpose:
** Interface of class DcmFloatingPointSingle
**
** Last Update:		$Author: meichel $
** Update Date:		$Date: 1998-11-12 16:47:49 $
** Source File:		$Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/include/Attic/dcvrfl.h,v $
** CVS/RCS Revision:	$Revision: 1.8 $
** Status:		$State: Exp $
**
** CVS/RCS Log at end of file
**
*/

#ifndef DCVRFL_H
#define DCVRFL_H

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcerror.h"
#include "dctypes.h"
#include "dcelem.h"



class DcmFloatingPointSingle : public DcmElement 
{
public:
    DcmFloatingPointSingle( const DcmTag &tag, const Uint32 len = 0);
    DcmFloatingPointSingle( const DcmFloatingPointSingle &newFL );
    virtual ~DcmFloatingPointSingle();

    DcmFloatingPointSingle &operator=(const DcmFloatingPointSingle &obj) { DcmElement::operator=(obj); return *this; }

    virtual DcmEVR ident(void) const { return EVR_FL; }
    virtual void print(ostream & out = cout, const OFBool showFullData = OFTrue,
		       const int level = 0);
    virtual unsigned long getVM(void);

    virtual E_Condition putFloat32Array(const Float32 * floatVal,
					const unsigned long numFloats);  

    virtual E_Condition putFloat32(const Float32 floatVal, // one float
				   const unsigned long position = 0);    
                                                           // at any position
 
    virtual E_Condition putString(const char * value);  // float as Strings


    virtual E_Condition getFloat32Array(Float32 * & singleVals);
    virtual E_Condition getFloat32(Float32 & singleVal, 
				   const unsigned long pos = 0);

    virtual E_Condition verify(const OFBool autocorrect = OFFalse);
};


#endif // DCVRFL_H

/*
** CVS/RCS Log:
** $Log: dcvrfl.h,v $
** Revision 1.8  1998-11-12 16:47:49  meichel
** Implemented operator= for all classes derived from DcmObject.
**
** Revision 1.7  1997/07/21 08:25:14  andreas
** - Replace all boolean types (BOOLEAN, CTNBOOLEAN, DICOM_BOOL, BOOL)
**   with one unique boolean type OFBool.
**
** Revision 1.6  1997/04/18 08:13:30  andreas
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
** Revision 1.5  1996/08/05 08:45:32  andreas
** new print routine with additional parameters:
**         - print into files
**         - fix output length for elements
** corrected error in search routine with parameter ESM_fromStackTop
**
** Revision 1.4  1996/01/29 13:38:16  andreas
** - new put method for every VR to put value as a string
** - better and unique print methods
**
** Revision 1.3  1996/01/05 13:23:05  andreas
** - changed to support new streaming facilities
** - more cleanups
** - merged read / write methods for block and file transfer
**
**
**
*/
