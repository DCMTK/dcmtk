/*
**
** Author: Gerd Ehlers	    26.04.94 -- Created
**         Andreas Barth    30.11.95 -- New Stream classes
** Kuratorium OFFIS e.V.
**
** Module: dcsequen.h
**
** Purpose:
** Interface of class DcmSequenceOfItems
**
**
** Last Update:		$Author: andreas $
** Update Date:		$Date: 1996-08-05 08:45:28 $
** Source File:		$Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/include/Attic/dcsequen.h,v $
** CVS/RCS Revision:	$Revision: 1.8 $
** Status:		$State: Exp $
**
** CVS/RCS Log at end of file
**
*/

#ifndef DCSEQUEN_H
#define DCSEQUEN_H

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcerror.h"
#include "dctypes.h"
#include "dcobject.h"
#include "dcitem.h"
#include "dctag.h"
#include "dclist.h"
#include "dcstack.h"


//
// CLASS DcmSequenceOfItems
// A sequence has no explicit value. Therefore, it should be derived from 
// DcmObject. Since a sequence is created in an (pseudo)-item and items collect
// sequences of elements the sequence Tag is derived from element.

class DcmSequenceOfItems : public DcmElement
{
protected:
    DcmList *itemList;
    BOOL lastItemComplete;
    Uint32 fStartPosition;

    virtual E_Condition readTagAndLength(DcmStream & inStream,		   // inout
					 const E_TransferSyntax xfer,  // in
					 DcmTag &tag,                  // out
					 Uint32 & length );	   // out

    virtual E_Condition makeSubObject(DcmObject * & subObject,
				      const DcmTag & mewTag,
				      const Uint32 newLength);

    E_Condition readSubItem(DcmStream & inStream,			// inout
			    const DcmTag &newTag,       	// in
			    const Uint32 newLength, 		// in
			    const E_TransferSyntax xfer,    // in
			    const E_GrpLenEncoding gltype,	// in
			    const Uint32 maxReadLength 		// in
			    = DCM_MaxReadLength);

    virtual E_Condition searchSubFromHere(const DcmTag &tag,          // in
					  DcmStack &resultStack,      // inout
					  const BOOL searchIntoSub ); // in

public:
    DcmSequenceOfItems(const DcmTag &tag, const Uint32 len = 0);
    DcmSequenceOfItems( const DcmSequenceOfItems& oldSeq );
    virtual ~DcmSequenceOfItems();

    virtual DcmEVR ident() const { return EVR_SQ; }
    virtual BOOL isLeaf(void) const { return FALSE; }
    virtual void print(ostream & out = cout, const BOOL showFullData = TRUE,
		       const int level = 0);
    virtual unsigned long getVM() { return 1L; }
    virtual Uint32 getLength(const E_TransferSyntax xfer 
			     = EXS_LittleEndianImplicit,
			     const E_EncodingType enctype 
			     = EET_UndefinedLength );

    virtual void transferInit(void);
    virtual void transferEnd(void);

    virtual E_Condition read(DcmStream & inStream,
			     const E_TransferSyntax xfer,
			     const E_GrpLenEncoding gltype = EGL_withoutGL,
			     const Uint32 maxReadLength = DCM_MaxReadLength);

    virtual E_Condition write(DcmStream & outStream,
			      const E_TransferSyntax oxfer,
			      const E_EncodingType enctype = EET_UndefinedLength,
			      const E_GrpLenEncoding gltype = EGL_withoutGL);

    virtual unsigned long card();
    virtual E_Condition insert(DcmItem* item,
			       unsigned long where = DCM_EndOfListIndex);
    virtual DcmItem* getItem(const unsigned long num);
    virtual E_Condition nextObject(DcmStack & stack, const BOOL intoSub);
    virtual DcmObject * nextInContainer(const DcmObject * obj);
    virtual DcmItem* remove(const unsigned long num);
    virtual DcmItem* remove(DcmItem* item);
    virtual E_Condition clear();
    virtual E_Condition verify(const BOOL autocorrect = FALSE);
    virtual E_Condition search(const DcmTag &tag,                // in
			       DcmStack &resultStack, 	       // inout
			       E_SearchMode mode = ESM_fromHere, // in
			       BOOL searchIntoSub = TRUE );      // in
    virtual E_Condition search(  const DcmTagKey &xtag,	       // in
				 DcmStack &resultStack, 	       // inout
				 E_SearchMode mode = ESM_fromHere,  // in
				 BOOL searchIntoSub = TRUE );	  // in
    virtual E_Condition searchErrors( DcmStack &resultStack );	  // inout
    virtual E_Condition loadAllDataIntoMemory(void);

    virtual E_Condition addGroupLengthElements( E_TransferSyntax xfer,
						E_EncodingType enctype );
    virtual E_Condition removeGroupLengthElements();
};



#endif // DCSEQUEN_H

/*
** CVS/RCS Log:
** $Log: dcsequen.h,v $
** Revision 1.8  1996-08-05 08:45:28  andreas
** new print routine with additional parameters:
**         - print into files
**         - fix output length for elements
** corrected error in search routine with parameter ESM_fromStackTop
**
** Revision 1.7  1996/07/17 12:38:59  andreas
** new nextObject to iterate a DicomDataset, DicomFileFormat, Item, ...
**
** Revision 1.6  1996/01/29 13:38:14  andreas
** - new put method for every VR to put value as a string
** - better and unique print methods
**
** Revision 1.5  1996/01/24 09:34:56  andreas
** Support for 64 bit long
**
** Revision 1.4  1996/01/09 11:06:16  andreas
** New Support for Visual C++
** Correct problems with inconsistent const declarations
**
** Revision 1.3  1996/01/05 13:22:59  andreas
** - changed to support new streaming facilities
** - more cleanups
** - merged read / write methods for block and file transfer
**
**
*/
