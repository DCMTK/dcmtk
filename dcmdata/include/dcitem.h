/*
**
** Author: Gerd Ehlers	    26.04.94 -- Created
**         Andreas Barth    30.11.95 -- New Stream classes
** Kuratorium OFFIS e.V.
**
** Module: dcitem.h
**
** Purpose:
** Interface of class DcmItem
**
**
** Last Update:		$Author: andreas $
** Update Date:		$Date: 1996-07-17 12:38:58 $
** Source File:		$Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/include/Attic/dcitem.h,v $
** CVS/RCS Revision:	$Revision: 1.9 $
** Status:		$State: Exp $
**
*/


#ifndef DCITEM_H
#define DCITEM_H

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include "dctypes.h"
#include "dcobject.h"
#include "dcvrui.h"
#include "dclist.h"
#include "dcstack.h"



class DcmItem : public DcmObject 
{
protected:
    DcmList * elementList;
    BOOL lastElementComplete;
    Uint32 fStartPosition;

    DcmObject*	     copyDcmObject(DcmObject *oldObj);

    Uint32 calcElementLength(DcmObject *obj,
			     const E_TransferSyntax xfer,
			     const E_EncodingType enctype);

    Uint32 calcHeaderLength(DcmEVR vr, const E_TransferSyntax xfer);

    E_Condition readTagAndLength(DcmStream & inStream,		  // inout
				 const E_TransferSyntax newxfer, // in
				 DcmTag   &tag,                  // out
				 Uint32 & length,         // out
				 Uint32 & bytesRead);     // out

    E_Condition readSubElement(DcmStream & inStream, 		// inout
			       DcmTag &newTag,            	// inout
			       const Uint32 newLength,          // in
			       const E_TransferSyntax xfer,     // in
			       const E_GrpLenEncoding gltype,   // in
			       const Uint32 maxReadLength);	// in

    E_Condition searchSubFromHere(const DcmTag &tag,         // in
				  DcmStack &resultStack,     // inout
				  BOOL searchIntoSub );      // in
    DcmObject * iterObject(const DcmObject * obj,
			   const E_ListPos pos);
    BOOL foundVR(char* atposition );
    E_TransferSyntax checkTransferSyntax(DcmStream & inStream);

public:
    DcmItem(); // create with an item tag
    DcmItem(const DcmTag &tag,
	    const Uint32 len = 0);
    DcmItem( const DcmItem& old );
    virtual ~DcmItem();

    virtual DcmEVR 	ident(void) const;
    virtual BOOL isLeaf(void) const { return FALSE; }
    virtual void	print(const int level = 0);

    virtual unsigned long getVM();
    virtual Uint32 getLength(const E_TransferSyntax xfer 
			     = EXS_LittleEndianImplicit,
			     const E_EncodingType enctype 
			     = EET_UndefinedLength );

    virtual void transferInit();
    virtual void transferEnd();

    virtual E_Condition read(DcmStream & inStream,
			     const E_TransferSyntax ixfer,
			     const E_GrpLenEncoding gltype = EGL_withoutGL,
			     const Uint32 maxReadLength 
			     = DCM_MaxReadLength);

    virtual E_Condition write(DcmStream & outStream,
			      const E_TransferSyntax oxfer,
			      const E_EncodingType enctype 
			      = EET_UndefinedLength,
			      const E_GrpLenEncoding gltype 
			      = EGL_withoutGL);

    virtual unsigned long card();
    virtual E_Condition insert(DcmElement* elem,
			       BOOL replaceOld = FALSE);
    virtual DcmElement* getElement(const unsigned long num);

    // get next Object from position in stack. If stack empty
    // get next Object in this item. if intoSub true, scan
    // complete hierarchy, false scan only elements direct in this
    // item (not deeper). 
    virtual E_Condition nextObject(DcmStack & stack, const BOOL intoSub);
    virtual DcmObject * nextInContainer(const DcmObject * obj);
    virtual DcmElement* remove(const unsigned long num);
    virtual DcmElement* remove(DcmObject* elem);
    virtual DcmElement* remove(const DcmTag& tag);
    virtual E_Condition clear();
    virtual E_Condition verify(const BOOL autocorrect = FALSE );
    virtual E_Condition search(const DcmTag &tag,                 // in
			       DcmStack &resultStack,	       // inout
			       E_SearchMode mode = ESM_fromHere,  // in
			       BOOL searchIntoSub = TRUE );       // in
    virtual E_Condition search(const DcmTagKey& xtag,	       // in
			       DcmStack &resultStack,	       // inout
			       E_SearchMode mode = ESM_fromHere,  // in
			       BOOL searchIntoSub = TRUE );       // in
    virtual E_Condition searchErrors( DcmStack &resultStack );	       // inout
    virtual E_Condition loadAllDataIntoMemory(void);

    virtual E_Condition addGroupLengthElements(const E_TransferSyntax xfer,
					       const E_EncodingType enctype );
    virtual E_Condition removeGroupLengthElements();

    /* simplified search&get functions */
    virtual E_Condition findString(const DcmTagKey& xtag,
				   char* aString, int maxStringLength,
				   BOOL searchIntoSub = FALSE);
    virtual E_Condition findLong(const DcmTagKey& xtag,
				 long& aLong, 
				 BOOL searchIntoSub = FALSE);


};

//
// SUPPORT FUNCTIONS
//



// Function: newDicomElement
// creates a new DicomElement from a Tag.
// 
// Input:
//   tag : Tag of the new element
//   length : length of the element value
//
// Output:
//   newElement: point of a heap allocated new element. If the tag does not
//               describe a dicom element or has ambigious VR (e.g. EVR_ox)
//               a NULL pointer is returned.
//
// Result:
//   EC_Normal:     tag describes an element (possibly with ambiguous VR)
//   EC_InvalidTag: tag describes an item begin or an unknown element
//   EC_SequEnd:    tag describes a sequence delimitation element
//   EC_ItemEnd:    tag describes an item delmitation element
//   other: an error

E_Condition newDicomElement(DcmElement * & newElement,
			    const DcmTag & tag,
			    const Uint32 length = 0);



// Function: newDicomElement
// creates a new DicomElement from a Tag. If differs from the above funtion in 
// not returning a condition. 
 
DcmElement * newDicomElement(const DcmTag & tag,
			     const Uint32 length = 0);

// Function: nextUp
// pop Object from stack and get next Object in top of stack

E_Condition nextUp(DcmStack & stack);


#endif // DCITEM_H

/*
** CVS/RCS Log:
** $Log: dcitem.h,v $
** Revision 1.9  1996-07-17 12:38:58  andreas
** new nextObject to iterate a DicomDataset, DicomFileFormat, Item, ...
**
** Revision 1.8  1996/04/29 15:08:53  hewett
** Replaced DcmItem::findInt(...) with the more general DcmItem::findLong(...).
**
** Revision 1.7  1996/04/16 16:00:05  andreas
** - added const for Tag in newDicomElement
**
** Revision 1.6  1996/03/28 18:52:30  hewett
** Added 2 simple find&get methods (findString & findInt).
**
** Revision 1.5  1996/01/29 13:38:12  andreas
** - new put method for every VR to put value as a string
** - better and unique print methods
**
** Revision 1.4  1996/01/09 11:06:15  andreas
** New Support for Visual C++
** Correct problems with inconsistent const declarations
**
** Revision 1.3  1996/01/05 13:22:56  andreas
** - changed to support new streaming facilities
** - more cleanups
** - merged read / write methods for block and file transfer
**
**
*/
