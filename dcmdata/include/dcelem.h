/*
**
** Author: Gerd Ehlers      27.04.94 -- First Creation
**         Andreas Barth    14.12.95 -- Unique design of Value Field
** Kuratorium OFFIS e.V.
**
** Module: dcelem.h
**
** Purpose:
** 	Interface of class DcmElement
**
**
** Last Update:		$Author: andreas $
** Update Date:		$Date: 1996-07-17 12:38:58 $
** Source File:		$Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/include/Attic/dcelem.h,v $
** CVS/RCS Revision:	$Revision: 1.6 $
** Status:		$State: Exp $
**
** CVS/RCS Log at end of file
**
*/


#ifndef DCELEM_H
#define DCELEM_H

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcerror.h"
#include "dctypes.h"
#include "dcobject.h"


class DcmLoadValueType;

class DcmElement : public DcmObject 
{
private:
    DcmLoadValueType * fLoadValue;	// Information to load Value later
    Uint8 * fValue;			// Value of Element

protected:
    E_ByteOrder fByteOrder;

    void * getValue(const E_ByteOrder newByteOrder = gLocalByteOrder);


    E_Condition changeValue(const void * value,		// new Value
			    const Uint32 position,	// position in value array
			    const Uint32 num);	    // number of new value bytes

    E_Condition putValue(const void * value, 	// new value
			 const Uint32 length);	// number of new value bytes

	

    E_Condition loadValue(DcmStream * inStream = NULL);
    virtual void postLoadValue(void);

    virtual Uint8 * newValueField(void);

public:
    DcmElement(const DcmTag & tag, const Uint32 len = 0);
    DcmElement(const DcmElement & elem);
    virtual ~DcmElement();


    virtual Uint32 getLength(const E_TransferSyntax /*xfer*/ 
			     = EXS_LittleEndianImplicit,
			     const E_EncodingType /*enctype*/ 
			     = EET_UndefinedLength)
    {
	return Length;
    }

    virtual BOOL isLeaf(void) const { return TRUE; }
    inline BOOL valueLoaded(void) { return fValue != NULL || Length == 0; }

    virtual void transferInit(void);

    virtual E_Condition read(DcmStream & inStream, 
			     const E_TransferSyntax ixfer,
			     const E_GrpLenEncoding gltype = EGL_withoutGL,
			     const Uint32 maxReadLength = DCM_MaxReadLength);

    virtual E_Condition write(DcmStream & outStream,
			      const E_TransferSyntax oxfer,
			      const E_EncodingType enctype = EET_UndefinedLength,
			      const E_GrpLenEncoding gltype = EGL_withoutGL);

    virtual E_Condition clear(void);

    virtual E_Condition loadAllDataIntoMemory(void);

	// GET-Operations
	
    // One Value an position pos
    virtual E_Condition get(Uint8 & val, const unsigned long pos);
    virtual E_Condition get(Sint16 & val, const unsigned long pos);
    virtual E_Condition get(Uint16 & val, const unsigned long pos);
    virtual E_Condition get(Sint32 & val, const unsigned long pos);
    virtual E_Condition get(Uint32 & val, const unsigned long pos);
    virtual E_Condition get(Float32 & val, const unsigned long pos);
    virtual E_Condition get(Float64 & val, const unsigned long pos);
    virtual E_Condition get(DcmTag & val, const unsigned long pos);

    // Get operations do not copy, they give a reference of the value
	// Values of Length bytes
    virtual E_Condition get(char * & val);	// for strings
    virtual E_Condition get(Uint8 * & val);	// for bytes
    virtual E_Condition get(Sint16 * & val);
    virtual E_Condition get(Uint16 * & val);
    virtual E_Condition get(Sint32 * & val);
    virtual E_Condition get(Uint32 * & val);
    virtual E_Condition get(Float32 * & val);
    virtual E_Condition get(Float64 * & val);



// PUT-Operations
// Put operations copy the value.

    // One Value
    virtual E_Condition put(const char * val);
    virtual E_Condition put(const Sint16 val);
    virtual E_Condition put(const Uint16 val);
    virtual E_Condition put(const Sint32 val);
    virtual E_Condition put(const Uint32 val);
    virtual E_Condition put(const Float32 val);
    virtual E_Condition put(const Float64 val);
    virtual E_Condition put(const DcmTagKey & attrTag);
	

    // One Value at a position pos
    virtual E_Condition put(const Sint16 val, const unsigned long pos);
    virtual E_Condition put(const Uint16 val, const unsigned long pos);
    virtual E_Condition put(const Sint32 val, const unsigned long pos);
    virtual E_Condition put(const Uint32 val, const unsigned long pos);
    virtual E_Condition put(const Float32 val, const unsigned long pos);
    virtual E_Condition put(const Float64 val, const unsigned long pos);
    virtual E_Condition put(const DcmTagKey & attrTag, const unsigned long pos);

    // num Values
    virtual E_Condition put(const Uint8 * vals, const unsigned long num);
    virtual E_Condition put(const Sint16 * vals, const unsigned long num);
    virtual E_Condition put(const Uint16 * vals, const unsigned long num);
    virtual E_Condition put(const Sint32 * vals, const unsigned long num);
    virtual E_Condition put(const Uint32 * vals, const unsigned long num);
    virtual E_Condition put(const Float32 * vals, const unsigned long num);
    virtual E_Condition put(const Float64 * vals, const unsigned long num);

};


#endif // DCELEM_H

/*
** CVS/RCS Log:
** $Log: dcelem.h,v $
** Revision 1.6  1996-07-17 12:38:58  andreas
** new nextObject to iterate a DicomDataset, DicomFileFormat, Item, ...
**
** Revision 1.5  1996/04/16 16:01:36  andreas
** - put methods for AttributeTag with DcmTagKey Parameter
** - better support for NULL values
**
** Revision 1.4  1996/03/12 15:31:56  hewett
** The base virtual get & put functions now support char*.
**
** Revision 1.3  1996/01/05 13:22:55  andreas
** - changed to support new streaming facilities
** - more cleanups
** - merged read / write methods for block and file transfer
**
**
**
*/

