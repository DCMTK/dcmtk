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
** Update Date:		$Date: 1996-01-05 13:22:56 $
** Source File:		$Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/include/Attic/dcitem.h,v $
** CVS/RCS Revision:	$Revision: 1.3 $
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

    E_Condition      searchSubFromHere(const DcmTag &tag,         // in
				       DcmStack &resultStack,	  // inout
				       BOOL searchIntoSub );      // in
    BOOL	     foundVR(char* atposition );
    E_TransferSyntax checkTransferSyntax(DcmStream & inStream);

public:
    DcmItem(); // create with an item tag
    DcmItem(const DcmTag &tag,
	    const Uint32 len = 0);
    DcmItem( const DcmItem& old );
    virtual ~DcmItem();

    virtual DcmEVR 	ident() const;
    virtual void	print(int level = 0);

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
    virtual DcmElement* getElement(unsigned long num);
    virtual DcmElement* remove(unsigned long num);
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
};


#endif // DCITEM_H

/*
** CVS/RCS Log:
** $Log: dcitem.h,v $
** Revision 1.3  1996-01-05 13:22:56  andreas
** - changed to support new streaming facilities
** - more cleanups
** - merged read / write methods for block and file transfer
**
**
*/
