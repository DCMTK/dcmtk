/*
 *
 * Author: Gerd Ehlers	    Created:  04-26-94
 *                          Modified: 02-07-95
 *
 * Module: dcitem.h
 *
 * Purpose:
 * Interface of class DcmItem
 *
 *
 * Last Update:   $Author: hewett $
 * Revision:      $Revision: 1.2 $
 * Status:	  $State: Exp $
 *
 */

#ifndef DCITEM_H
#define DCITEM_H

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include "dctypes.h"
#include "dcobject.h"
#include "dcvrui.h"
#include "dclist.h"
#include "dcstack.h"



class DcmItem : public DcmObject {
protected:
    DcmList *elementList;
    BOOL    lastElementComplete;

    DcmObject*	     copyDcmObject(	DcmObject *oldObj );
    T_VR_UL          calcElementLength( DcmObject *obj,
                                        E_TransferSyntax xfer,
                                        E_EncodingType enctype );
    T_VR_UL	     calcHeaderLength(	DcmEVR vr,
					E_TransferSyntax xfer );
    E_Condition      readTagAndLength(	E_TransferSyntax newxfer,      // in
                                        DcmTag   &tag,                 // out
                                        T_VR_UL  *length,              // out
                                        T_VR_UL  *item_bytes );        // out
    E_Condition      readSubElement(    DcmTag   &newTag,              // in
                                        T_VR_UL  newLength,            // in
                                        T_VR_UL  tagLength,            // in
                                        E_TransferSyntax xfer,         // in
                                        E_GrpLenEncoding gltype );     // in
    E_Condition      searchSubFromHere( const DcmTag   &tag,           // in
					DcmStack &resultStack,	       // inout
                                        BOOL     searchIntoSub );      // in
    BOOL	     foundVR(		char* atposition );
    E_TransferSyntax checkTransferSyntax();

public:
    DcmItem(); // create with an item tag
    DcmItem( const DcmTag &tag,
	     T_VR_UL len = 0,
	     iDicomStream *iDStream = NULL);
    DcmItem( const DcmItem& old );
    virtual ~DcmItem();

    virtual DcmEVR 	ident() const;
    virtual void	print(int level = 0);

    virtual E_Condition readBlockInit();
    virtual E_Condition writeBlockInit();

    virtual T_VR_UL     getVM();
    virtual T_VR_UL	getLength(  E_TransferSyntax xfer = EXS_LittleEndianImplicit,
				    E_EncodingType enctype = EET_UndefinedLength );
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
    virtual E_Condition insert(     DcmElement* elem,
				    BOOL replaceOld = FALSE );
    virtual DcmElement* getElement( T_VR_UL num );
    virtual DcmElement* remove(     T_VR_UL num );
    virtual DcmElement* remove(     DcmObject* elem );
    virtual DcmElement* remove(     const DcmTag& tag );
    virtual E_Condition clear();
    virtual E_Condition verify(     BOOL autocorrect = FALSE );
    virtual E_Condition search(     const DcmTag &tag,                 // in
				    DcmStack &resultStack,	       // inout
				    E_SearchMode mode = ESM_fromHere,  // in
				    BOOL searchIntoSub = TRUE );       // in
    virtual E_Condition search(     const DcmTagKey& xtag,	       // in
				    DcmStack &resultStack,	       // inout
				    E_SearchMode mode = ESM_fromHere,  // in
				    BOOL searchIntoSub = TRUE );       // in
    virtual E_Condition searchErrors( DcmStack &resultStack );	       // inout
    virtual E_Condition loadAllDataIntoMemory();

    virtual E_Condition addGroupLengthElements( E_TransferSyntax xfer,
                                                E_EncodingType enctype );
    virtual E_Condition removeGroupLengthElements();
};


#endif // DCITEM_H

