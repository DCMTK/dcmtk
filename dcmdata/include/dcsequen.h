/*
 *
 * Author: Gerd Ehlers	    Created:  04-26-94
 *                          Modified: 02-07-95
 *
 * Module: dcsequen.h
 *
 * Purpose:
 * Interface of class DcmSequenceOfItems
 *
 *
 * Last Update:   $Author: hewett $
 * Revision:      $Revision: 1.2 $
 * Status:	  $State: Exp $
 *
 */

#ifndef DCSEQUEN_H
#define DCSEQUEN_H

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include "dctypes.h"
#include "dcelem.h"
#include "dcitem.h"
#include "dctag.h"
#include "dclist.h"
#include "dcstack.h"


class DcmSequenceOfItems : public DcmElement {
protected:
    DcmList *itemList;
    BOOL    lastItemComplete;

    virtual E_Condition readTagAndLength(  E_TransferSyntax xfer,      // in
                                           DcmTag &tag,                // out
					   T_VR_UL *length );	       // out
    virtual E_Condition readSubItem(       const DcmTag &newTag,       // in
                                           T_VR_UL newLength,          // in
                                           E_TransferSyntax xfer,      // in
                                           E_GrpLenEncoding gltype );  // in
    virtual E_Condition searchSubFromHere( const DcmTag &tag,          // in
					   DcmStack &resultStack,      // inout
					   BOOL searchIntoSub );       // in

public:
    DcmSequenceOfItems( const DcmTag &tag,
			T_VR_UL len = 0,
			iDicomStream *iDStream = NULL );
    DcmSequenceOfItems( const DcmSequenceOfItems& oldSeq );
    virtual ~DcmSequenceOfItems();

    virtual DcmEVR 	ident() const;
    virtual void        print(   int level = 0 );
    virtual T_VR_UL	getVM();
    virtual T_VR_UL	getLength(  E_TransferSyntax xfer = EXS_LittleEndianImplicit,
				    E_EncodingType enctype = EET_UndefinedLength );

    virtual E_Condition readBlockInit();
    virtual E_Condition writeBlockInit();
    virtual E_Condition read(       E_TransferSyntax xfer,
                                    E_GrpLenEncoding gltype = EGL_withoutGL );
    virtual E_Condition write(      oDicomStream &oDS,
				    E_TransferSyntax oxfer,
                                    E_EncodingType enctype = EET_UndefinedLength,
                                    E_GrpLenEncoding gltype = EGL_withoutGL );
    virtual E_Condition readBlock(  E_TransferSyntax xfer,
                                    E_GrpLenEncoding gltype = EGL_withoutGL );
    virtual E_Condition writeBlock( oDicomStream &oDS,
				    E_TransferSyntax oxfer,
                                    E_EncodingType enctype = EET_UndefinedLength,
                                    E_GrpLenEncoding gltype = EGL_withoutGL );
    virtual T_VR_UL	card();
    virtual E_Condition insert(  DcmItem* item,
				 T_VR_UL where = UNDEF_LEN );
    virtual DcmItem*	getItem( T_VR_UL num );
    virtual DcmItem*	remove(  T_VR_UL num );
    virtual DcmItem*    remove(  DcmItem* item );
    virtual E_Condition clear();
    virtual E_Condition verify(  BOOL autocorrect = FALSE );
    virtual E_Condition search(  const DcmTag &tag,                    // in
				 DcmStack &resultStack, 	       // inout
				 E_SearchMode mode = ESM_fromHere,     // in
				 BOOL searchIntoSub = TRUE );	       // in
    virtual E_Condition search(  const DcmTagKey &xtag,		       // in
				 DcmStack &resultStack, 	       // inout
				 E_SearchMode mode = ESM_fromHere,     // in
				 BOOL searchIntoSub = TRUE );	       // in
    virtual E_Condition searchErrors( DcmStack &resultStack );	       // inout
    virtual E_Condition loadAllDataIntoMemory();

    virtual E_Condition addGroupLengthElements( E_TransferSyntax xfer,
                                                E_EncodingType enctype );
    virtual E_Condition removeGroupLengthElements();
};



#endif // DCSEQUEN_H

