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
** Last Update:		$Author: andreas $
** Update Date:		$Date: 1996-01-29 13:38:16 $
** Source File:		$Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/include/Attic/dcvrfl.h,v $
** CVS/RCS Revision:	$Revision: 1.4 $
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

    virtual DcmEVR ident(void) const { return EVR_FL; }
    virtual void print(const int level = 0);
    virtual unsigned long getVM(void);

    virtual E_Condition put(const Float32 * floatVal,
                            const unsigned long numFloats);  // number of floats

    virtual E_Condition put(const Float32 floatVal);   // for one float only

    virtual E_Condition put(const Float32 floatVal,           // one float
                            const unsigned long position);    // at any position
 
    virtual E_Condition put(const char * value);  // float as Strings


    virtual	E_Condition get(Float32 * & singleVals);
    virtual E_Condition get(Float32 & singleVal, const unsigned long pos = 0);

    Float32 * get(void);
    Float32 get(const unsigned long position);

    virtual E_Condition verify(const BOOL autocorrect = FALSE);
};


#endif // DCVRFL_H

/*
** CVS/RCS Log:
** $Log: dcvrfl.h,v $
** Revision 1.4  1996-01-29 13:38:16  andreas
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
