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
** Update Date:		$Date: 1997-05-16 08:23:48 $
** Source File:		$Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/include/Attic/dcsequen.h,v $
** CVS/RCS Revision:	$Revision: 1.10 $
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

    virtual E_Condition readTagAndLength(DcmStream & inStream,	 // inout
					 const E_TransferSyntax xfer,  // in
					 DcmTag &tag,                  // out
					 Uint32 & length );	   // out

    virtual E_Condition makeSubObject(DcmObject * & subObject,
				      const DcmTag & mewTag,
				      const Uint32 newLength);

    E_Condition readSubItem(DcmStream & inStream,		// inout
			    const DcmTag &newTag,       	// in
			    const Uint32 newLength, 		// in
			    const E_TransferSyntax xfer,        // in
			    const E_GrpLenEncoding glenc,       // in
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

   virtual E_Condition computeGroupLengthAndPadding
                            (const E_GrpLenEncoding glenc,
			     const E_PaddingEncoding padenc = EPD_noChange,
			     const E_TransferSyntax xfer = EXS_Unknown,
			     const E_EncodingType enctype = EET_ExplicitLength,
			     const Uint32 padlen = 0,
			     const Uint32 subPadlen = 0,
			     Uint32 instanceLength = 0);

    virtual Uint32 calcElementLength(const E_TransferSyntax xfer,
				     const E_EncodingType enctype);

    virtual Uint32 getLength(const E_TransferSyntax xfer 
			     = EXS_LittleEndianImplicit,
			     const E_EncodingType enctype 
			     = EET_UndefinedLength );

    virtual void transferInit(void);
    virtual void transferEnd(void);

    virtual E_Condition read(DcmStream & inStream,
			     const E_TransferSyntax xfer,
			     const E_GrpLenEncoding glenc = EGL_noChange,
			     const Uint32 maxReadLength = DCM_MaxReadLength);

    virtual E_Condition write(DcmStream & outStream,
			      const E_TransferSyntax oxfer,
			      const E_EncodingType enctype = EET_UndefinedLength);


    virtual unsigned long card();

    virtual E_Condition prepend(DcmItem* item);
    virtual E_Condition insert(DcmItem* item,
			       unsigned long where = DCM_EndOfListIndex,
			       BOOL before = FALSE );
    virtual E_Condition append(DcmItem* item);
			       
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

};



#endif // DCSEQUEN_H

/*
** CVS/RCS Log:
** $Log: dcsequen.h,v $
** Revision 1.10  1997-05-16 08:23:48  andreas
** - Revised handling of GroupLength elements and support of
**   DataSetTrailingPadding elements. The enumeratio E_GrpLenEncoding
**   got additional enumeration values (for a description see dctypes.h).
**   addGroupLength and removeGroupLength methods are replaced by
**   computeGroupLengthAndPadding. To support Padding, the parameters of
**   element and sequence write functions changed.
** - Added a new method calcElementLength to calculate the length of an
**   element, item or sequence. For elements it returns the length of
**   tag, length field, vr field, and value length, for item and
**   sequences it returns the length of the whole item. sequence including
**   the Delimitation tag (if appropriate).  It can never return
**   UndefinedLength.
**
** Revision 1.9  1997/04/24 12:09:02  hewett
** Fixed DICOMDIR generation bug affecting ordering of
** patient/study/series/image records (item insertion into a sequence
** did produce the expected ordering).
**
** Revision 1.8  1996/08/05 08:45:28  andreas
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
