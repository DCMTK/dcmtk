/*
 *
 * Author: Gerd Ehlers      Created:  04-26-94
 *                          Modified: 02-07-95
 *
 * Module: dcitem.cc
 *
 * Purpose:
 * Implementation of the class DcmItem
 *
 *
 * Last Update:   $Author: hewett $
 * Revision:      $Revision: 1.1 $
 * Status:	  $State: Exp $
 *
 */


#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdlib.h>
#include <stdio.h>
#include <iostream.h>

#include "dctk.h"
#include "dcitem.h"
#include "dcobject.h"
#include "dcvr.h"
#include "dcxfer.h"
#include "dcdebug.h"


extern char *VRTypesName[];
extern short DIM_OF_VRTypes;



// ********************************


DcmItem::DcmItem()
    : DcmObject( ItemTag )
{
Bdebug((5, "dcitem:DcmItem::DcmItem()" ));
debug(( 8, "Object pointer this=0x%p", this ));

    elementList = new DcmList;
    lastElementComplete = TRUE;
Edebug(());

}


// ********************************


DcmItem::DcmItem( DcmTag &tag )
    : DcmObject( tag )
{
Bdebug((5, "dcitem:DcmItem::DcmItem(DcmTag&)" ));
debug(( 8, "Object pointer this=0x%p", this ));

    elementList = new DcmList;
    lastElementComplete = TRUE;
Edebug(());

}


// ********************************


DcmItem::DcmItem( DcmTag &tag, T_VR_UL len, iDicomStream *iDStream )
    : DcmObject( tag, len, iDStream )
{
Bdebug((5, "dcitem:DcmItem::DcmItem(DcmTag&,len=%ld,*iDS)", len ));
debug(( 8, "Object pointer this=0x%p", this ));

    elementList = new DcmList;
    lastElementComplete = TRUE;
Edebug(());

}


// ********************************


DcmItem::DcmItem( const DcmObject &oldObj )
    : DcmObject( ItemTag )
{
Bdebug((5, "dcitem:DcmItem::DcmItem(DcmObject&)"));
debug(( 8, "Object pointer this=0x%p", this ));

    lastElementComplete = TRUE;
    elementList = new DcmList;
debug(( 3, "ident()=%d", oldObj.ident() ));

    if (    oldObj.ident() == EVR_item
	 || oldObj.ident() == EVR_dirRecord	  // noetig fuer Konversion
       )
    {
	DcmItem const *old = (DcmItem const *)&oldObj;
	*Tag = *old->Tag;
	iDS = old->iDS;
	offsetInFile  = old->offsetInFile;
	valueInMemory = old->valueInMemory;
	valueModified = old->valueModified;
	Length = old->Length;
	Xfer = old->Xfer;
	if ( !old->elementList->empty() )
	{
	    DcmObject *oldDO;
	    DcmObject *newDO;
	    elementList->seek( ELP_first );
	    old->elementList->seek( ELP_first );
	    do {
		oldDO = old->elementList->get();
		newDO = copyDcmObject( oldDO );

		elementList->insert( newDO, ELP_next );
	    } while ( old->elementList->seek( ELP_next ) );
	}
    }
    else
    {
        cerr << "Warning: DcmItem: wrong use of Copy-Constructor" << endl;
    }
Edebug(());

}


// ********************************


DcmItem::DcmItem( const DcmItem &oldItem )
    : DcmObject( ItemTag )
{
Bdebug((5, "dcitem:DcmItem::DcmItem(DcmItem&)"));
debug(( 8, "Object pointer this=0x%p", this ));

    lastElementComplete = TRUE;
    elementList = new DcmList;
debug(( 5, "ident()=%d", oldItem.ident() ));

    if (    oldItem.ident() == EVR_item
	 || oldItem.ident() == EVR_dirRecord	   // noetig fuer Konversion
       )
    {
	DcmItem const *old = &oldItem;
	*Tag = *old->Tag;
	iDS = old->iDS;
	offsetInFile  = old->offsetInFile;
	valueInMemory = old->valueInMemory;
	valueModified = old->valueModified;
	Length = old->Length;
	Xfer = old->Xfer;
	if ( !old->elementList->empty() )
	{
	    DcmObject *oldDO;
	    DcmObject *newDO;
	    elementList->seek( ELP_first );
	    old->elementList->seek( ELP_first );
	    do {
		oldDO = old->elementList->get();
		newDO = copyDcmObject( oldDO );

		elementList->insert( newDO, ELP_next );
	    } while ( old->elementList->seek( ELP_next ) );
	}
    }
    else
    {
        cerr << "Warning: DcmItem: wrong use of Copy-Constructor" << endl;
    }
Edebug(());

}


// ********************************


DcmItem::~DcmItem()
{
Bdebug((5, "dcitem:DcmItem::~DcmItem()"));
debug(( 8, "Object pointer this=0x%p", this ));

    DcmObject *dO;
    elementList->seek( ELP_first );
    while ( !elementList->empty() )
    {
	dO = elementList->remove();
	if ( dO != (DcmObject*)NULL )
	    delete dO;
    }
    delete elementList;

Edebug(());
}


// ********************************


BOOL DcmItem::foundVR( char* atposition )
{
    int i = 0;
    while ( (i < DIM_OF_VRTypes)
       && (memcmp(VRTypesName[i], atposition, 2) != 0) )
	i++;

    if ( (i < DIM_OF_VRTypes)
       && (memcmp(VRTypesName[i], atposition, 2) == 0) )
	return TRUE;
    else
	return FALSE;
}


// ********************************

/*
   wird in DcmDataset und in DcmMetaInfo benutzt
*/

E_TransferSyntax DcmItem::checkTransferSyntax()
{
Bdebug((3, "dcitem:DcmItem::checkTransferSyntax()" ));

    E_TransferSyntax transferSyntax;
    char tagAndVR[6];
    iDS->read( tagAndVR, 6 );			    // ueberpruefe Tag & VR
    for ( int i=5; i>=0; i--)
	iDS->putback( tagAndVR[i] );

    char c1 = tagAndVR[0];
    char c2 = tagAndVR[1];
    char c3 = tagAndVR[2];
    char c4 = tagAndVR[3];
    T_VR_US t1 = (c1 & 0xff) + ((c2 & 0xff) << 8);  // explizite little endian
    T_VR_US t2 = (c3 & 0xff) + ((c4 & 0xff) << 8);  // Konversion
    DcmTag taglittle( t1, t2 );
    DcmTag tagbig( SwapShort(t1), SwapShort(t2) );

    if ( taglittle.error() && tagbig.error() )	    // kein gueltiges Tag
    {						    // nehme LittleEndian an!
	if ( foundVR( &tagAndVR[4] ) )
	    transferSyntax = EXS_LittleEndianExplicit;
	else
	    transferSyntax = EXS_LittleEndianImplicit;
    }
    else					    // evtl. ist 1.Tag private
    {						    // gueltiges Tag
	if ( foundVR( &tagAndVR[4] ) )
	{					    // explizite VR!
	    if ( tagbig.error() )
		transferSyntax = EXS_LittleEndianExplicit;
	    else
		transferSyntax = EXS_BigEndianExplicit;
	}
	else
	{					    // implizite VR
	    if ( tagbig.error() )
		transferSyntax = EXS_LittleEndianImplicit;
	    else
		transferSyntax = EXS_BigEndianImplicit;
	}
    }						    // gueltige TransferSyntax

debug(( 3, "found TransferSyntax(0,2,3)=(%d)", transferSyntax ));
Edebug(());

    return transferSyntax;
} // DcmItem::checkTransferSyntax


// ********************************


DcmObject* DcmItem::copyDcmObject( DcmObject *oldObj )
{
Bdebug((4, "dcitem:DcmItem::copyDcmObject(DcmObject*)"));

    DcmObject *newObj = (DcmObject*)NULL;
    switch ( oldObj->ident() )
    {
	// Byte-Strings:
	case EVR_AE :
	    newObj = new DcmApplicationEntity( *oldObj );
	    break;
	case EVR_AS :
	    newObj = new DcmAgeString( *oldObj );
	    break;
	case EVR_CS :
	    newObj = new DcmCodeString( *oldObj );
	    break;
	case EVR_DA :
	    newObj = new DcmDate( *oldObj );
	    break;
	case EVR_DS :
	    newObj = new DcmDecimalString( *oldObj );
	    break;
	case EVR_DT :
	    newObj = new DcmDateTime( *oldObj );
	    break;
	case EVR_IS :
	    newObj = new DcmIntegerString( *oldObj );
	    break;
	case EVR_TM :
	    newObj = new DcmTime( *oldObj );
	    break;
	case EVR_UI :
	    newObj = new DcmUniqueIdentifier( *oldObj );
	    break;

	// Charakter-Strings:
	case EVR_LO :
	    newObj = new DcmLongString( *oldObj );
	    break;
	case EVR_LT :
	    newObj = new DcmLongText( *oldObj );
	    break;
	case EVR_PN :
	    newObj = new DcmPersonName( *oldObj );
	    break;
	case EVR_SH :
	    newObj = new DcmShortString( *oldObj );
	    break;
	case EVR_ST :
	    newObj = new DcmShortText( *oldObj );
	    break;

	// abhaengig von ByteOrder:
	case EVR_AT :
	    newObj = new DcmAttributeTag( *oldObj );
	    break;
	case EVR_SS :
	    newObj = new DcmSignedShort( *oldObj );
	    break;
	case EVR_xs : // laut Dicom-Standard V3.0
	case EVR_ux : // laut Dicom-Standard V3.0
	case EVR_US :
	    newObj = new DcmUnsignedShort( *oldObj );
	    break;
	case EVR_SL :
	    newObj = new DcmSignedLong( *oldObj );
	    break;
	case EVR_up : // fuer (0004,eeee) laut Dicom-Standard V3.0
	    newObj = new DcmUnsignedLongOffset( *oldObj );
	    break;
	case EVR_UL :
	    newObj = new DcmUnsignedLong( *oldObj );
	    break;
	case EVR_FL :
	    newObj = new DcmFloatingPointSingle( *oldObj );
	    break;
	case EVR_FD :
	    newObj = new DcmFloatingPointDouble( *oldObj );
	    break;

	// Sequenzen
	case EVR_SQ :
	    newObj = new DcmSequenceOfItems( *oldObj );
	    break;

	// nicht-eindeutig 8- oder 16-Bit:
	case EVR_oa : // laut Dicom-Standard V3.0
	case EVR_oc : // laut Dicom-Standard V3.0
	case EVR_OB :
	case EVR_OW :
	case EVR_op :
	    newObj = new DcmOtherByteOtherWord( *oldObj );
	    break;

	// Gekapselte Pixeldaten als spezielle Sequenz:
	case EVR_pixelSQ :
	    newObj = new DcmPixelSequence( *oldObj );
	    break;

	// Retired Elements und unbekannte Typen als Byte-String behandeln:
	case EVR_RET :
	case EVR_UNKNOWN :
	    newObj = new DcmOtherByteOtherWord( *oldObj );
	    break;

	case EVR_na :
	default :
            cerr << "Warning: DcmItem::copyDcmObject(): unsupported Element("
                 << hex << oldObj->getGTag() << "," << oldObj->getETag()
                 << dec << ") with ident()=" << oldObj->ident() << " found."
                 << endl;
	    break;
    }
Edebug(());

    return newObj;
}


// ********************************


EVR DcmItem::ident() const
{
    return EVR_item;
}


// ********************************


void DcmItem::print( int level )
{
    char *info = new char[200];
    char *title = (char*)NULL;
    if ( Length == UNDEF_LEN )
	title = "\"Item with undefined length\"";
    else
	title = "\"Item with explicit Length\"";

    sprintf( info, "%s offset=$%ld #=%ld ", title, offsetInFile, card() );
    DcmObject::printInfoLine( level, info );
    if ( !elementList->empty() )
    {
	DcmObject *dO;
	elementList->seek( ELP_first );
	do {
	    dO = elementList->get();
	    dO->print( level + 1 );
	} while ( elementList->seek( ELP_next ) );
    }

    DcmTag delimItemTag( ET_ItemDelimitationItem );
    if ( Length == UNDEF_LEN )
	DcmObject::printInfoLine( level, delimItemTag,
				  0, "\"ItemDelimitationItem\"" );
    else
	DcmObject::printInfoLine( level, delimItemTag,
		   0, "\"ItemDelimitationItem for re-encoding\"" );
    delete info;
}


// ********************************


T_VR_UL DcmItem::getVM()
{
    return 1L;
}


// ********************************

/*
   siehe auch DcmDicomDir::lengthUntilSQ()
*/

T_VR_UL DcmItem::calcHeaderLength( EVR vr, E_TransferSyntax xfer )
{
    T_VR_UL templen = 0;
    DcmXfer xferSyn( xfer );
    if ( xferSyn.isExplicitVR() )
    {
#if defined(DCMCODE_OLD_EX_COMPATIBILITY)
	templen = 10;	       // fuer Tag, 32-Bit-Length und VR
#else
	switch ( vr )
	{
	    case EVR_oa :
	    case EVR_oc :
	    case EVR_op :
	    case EVR_OB :
	    case EVR_OW :
	    case EVR_SQ :
		templen = 12;  // fuer Tag, Length, VR und reserved
		break;
	    default:
		templen = 8;   // fuer Tag, Length und VR
		break;
	}
#endif
    }
    else
	templen = 8;	       // fuer Tag und Length
    return templen;
}


// ********************************


T_VR_UL DcmItem::calcElementLength( DcmObject *obj,
                                    E_TransferSyntax xfer,
                                    E_EncodingType enctype )
{
Bdebug((4, "dcitem:DcmItem::calcElementLength(*obj,xfer=%d,enctype=%d)",
	   xfer, enctype ));

    T_VR_UL templen = 0L;
    T_VR_UL sublength = 0L;
    if ( obj != (DcmObject*)NULL )
    {
        sublength = obj->getLength( xfer, enctype );
        templen += sublength;
        templen += DcmItem::calcHeaderLength( obj->getVR(), xfer );

        if (    ( obj->getVR() == EVR_SQ )
             && ( enctype == EET_UndefinedLength )
           )
            templen += 8;           // fuer ItemDelimitationItem
debug(( 4, "ElementLength of (0x%4.4x,0x%4.4x) \"%s\" sublen=%lu len=%lu",
           obj->getGTag(), obj->getETag(),
           DcmVR(obj->getVR()).getVRName(), sublength, templen ));

    }
    else
	templen = 0;
    if ( sublength==UNDEF_LEN )
        templen = UNDEF_LEN;
Edebug(());

    return templen;
}


// ********************************


T_VR_UL DcmItem::getLength( E_TransferSyntax xfer, E_EncodingType enctype )
{
Bdebug((4, "dcitem:DcmItem::getLength(xfer=%d,enctype=%d)",
	   xfer, enctype ));

    T_VR_UL templen = 0L;
    T_VR_UL sublength = 0L;
    if ( !elementList->empty() )
    {
	DcmObject *dO;
	elementList->seek( ELP_first );
	do {
	    dO = elementList->get();
            sublength = DcmItem::calcElementLength( dO, xfer, enctype );
            if ( sublength==UNDEF_LEN )
            {
                templen = UNDEF_LEN;
                break;
            }
	    templen += sublength;
	} while ( elementList->seek( ELP_next ) );
    }
    else
	templen = 0;
debug(( 4, "Length of Item=%lu", templen ));
Edebug(());

    return templen;
}


// ********************************


E_Condition DcmItem::addGroupLengthElements( E_TransferSyntax xfer,
                                             E_EncodingType enctype )
{
Bdebug((2, "dcitem:DcmItem::addGroupLengthElements(xfer=%d,enctype=%d)",
	   xfer, enctype ));

    E_Condition l_error = EC_Normal;
    if ( !elementList->empty() )
    {
	DcmObject *dO;
        BOOL beginning = TRUE;
        T_VR_US lastGrp = 0x0000;
        T_VR_US actGrp;
        DcmUnsignedLong *actGLElem = (DcmUnsignedLong*)NULL;
        T_VR_UL grplen = 0L;
        E_Condition err = EC_Normal;
	elementList->seek( ELP_first );
	do {
	    dO = elementList->get();

            // berechne Group Length in untergeordneter Sequenz:
            if ( dO->getVR() == EVR_SQ )
            {
                err = ((DcmSequenceOfItems*)dO)->addGroupLengthElements( xfer,
                                                                         enctype );
                if ( err != EC_Normal )
                    l_error = err;
            }

            // erkenne neue Gruppe und erzeuge Group Length Element:
            actGrp = dO->getGTag();
            if ( actGrp!=lastGrp || beginning )  // neue Gruppe angefangen!
            {
                // Erzeuge Group Length Element, falls nicht schon vorhanden:
                beginning = FALSE;
                if ( dO->getETag() == 0x0000 )   // Group Length Tag (xxxx,0000)
                {
                    if ( dO->ident() != EVR_UL ) // kein Unsigned Long Element
                    {                            // ersetze durch UL
                        delete elementList->remove();
                        DcmTag tagUL( actGrp, 0x0000 );
                        DcmUnsignedLong *dUL = new DcmUnsignedLong( tagUL );
                        elementList->insert( dUL, ELP_prev );
                        dO = dUL;
                        cerr << "Info: DcmItem::addGroupLengthElements()"
                                " Group Length found, which was not from type"
                                " UL - corrected." << endl;
                    }
                }
                else
                {                                // fuege neue Group Length ein
                    DcmTag tagUL( actGrp, 0x0000 );
                    DcmUnsignedLong *dUL = new DcmUnsignedLong( tagUL );
                    elementList->insert( dUL, ELP_prev );
                    dO = dUL;
                }

                // Schreibe berechnete Laenge in das Datenfeld des Group Length
                // Elements der vorherigen Gruppe:
                if ( actGLElem != (DcmUnsignedLong*)NULL )
                {                                // eine Gruppe ist durchlaufen
                    actGLElem->put( grplen );
debug(( 2, "Length of Group 0x%4.4x len=%lu", actGLElem->getGTag(), grplen ));

                    grplen = 0L;
                }
                actGLElem = (DcmUnsignedLong*)dO;
            }
            else                                 // keine Group Length:
                grplen += DcmItem::calcElementLength( dO, xfer, enctype );
            lastGrp = actGrp;
	} while ( elementList->seek( ELP_next ) );

        // die letzte Group Length des Items eintragen
        if ( actGLElem != (DcmUnsignedLong*)NULL )
            actGLElem->put( grplen );
    }
Edebug(());

    return l_error;
}


// ********************************


E_Condition DcmItem::removeGroupLengthElements()
{
Bdebug((2, "dcitem:DcmItem::removeGroupLengthElements()" ));

    E_Condition l_error = EC_Normal;

    if ( !elementList->empty() )
    {
        E_ListPos seekmode = ELP_next;
	elementList->seek( ELP_first );
	do {
            seekmode = ELP_next;
            DcmObject *dO = elementList->get();
            if ( dO->getETag() == 0x0000 )      // Group Length Tag (xxxx,0000)
            {
                delete elementList->remove();
                seekmode = ELP_atpos;           // durch remove um 1 vorwaerts
            }
            else if ( dO->getVR() == EVR_SQ )
            {
                ((DcmSequenceOfItems*)dO)->removeGroupLengthElements();
            }
        } while ( elementList->seek( seekmode ) );
    }

Edebug(());

    return l_error;
}


// ********************************


E_Condition DcmItem::readTagAndLength( E_TransferSyntax newxfer,
                                       DcmTag &tag,
				       T_VR_UL *length,
				       T_VR_UL *item_bytes )
{
Bdebug((4, "dcitem:DcmItem::readTagAndLength(newxfer=%d,&tag,*length,"
           "*item_bytes)", newxfer ));

    E_Condition l_error = EC_Normal;
    E_TransferSyntax xfer = newxfer;
    T_VR_US reserved = 0;
    T_VR_US len_s = 0;
    T_VR_UL len_l = 0;
    EVR nxtobj = EVR_UNKNOWN;
    T_VR_US t1 = UNKNOWN_TAG;
    T_VR_US t2 = UNKNOWN_TAG;

    if ( !iDS->fromFile() && iDS->buffered() < 4 )
    {
Edebug(());
	return EC_InvalidStream;
    }

    iDS->setDataByteOrder( xfer );
    *iDS >> t1; 		    // Transfer Syntax !!
    *iDS >> t2; 		    // Transfer Syntax !!
				    // Tag ist gelesen
    *item_bytes = 4;
    DcmTag newTag( t1, t2 );
    nxtobj = newTag.getVR();	    // VR aus Tag bestimmen

    if (    !iDS->fromFile()
	 && (iDS->buffered()+*item_bytes) < calcHeaderLength( nxtobj, xfer ) )
    {
	for ( unsigned int i=0; i<*item_bytes; i++ )
	    iDS->unget();		     // schreibe Tags zurueck
	*item_bytes = 0;
Edebug(());
	return EC_InvalidStream;
    }

    DcmXfer xferSyn( xfer );
    if (    xferSyn.isImplicitVR()
	 || nxtobj == EVR_na	    // DelimitationItems besitzen keine VR!
       )
    {
	*iDS >> len_l;		    // 4 Byte Laenge; Transfer Syntax !!
	*item_bytes += 4;
debug(( 4, "Implicit xfer=%d", xfer ));

    }
    else if ( xferSyn.isExplicitVR() )
    {
	char vrstr[2];
	iDS->read(vrstr, 2);	    // 2 Byte Laenge:VR als string
	DcmVR vr( vrstr );	    // class DcmVR
	nxtobj = vr.getVR();
	newTag.setVR( nxtobj );     // VR in newTag anpassen
	*item_bytes += 2;

#if defined(DCMCODE_OLD_EX_COMPATIBILITY)
	*iDS >> len_l;
	*item_bytes += 4;
	len_s = reserved = 0; // no Compiler Warning
#else
	if ( nxtobj == EVR_OB || nxtobj == EVR_OW || nxtobj == EVR_SQ )
	{
	    *iDS >> reserved;	    // 2 Byte Laenge; Transfer Syntax !!
	    *iDS >> len_l;	    // 4 Byte Laenge; Transfer Syntax !!
	    *item_bytes += 6;
	}
	else
	{
	    *iDS >> len_s;	    // 2 Byte Laenge; Transfer Syntax !!
	    *item_bytes += 2;
	    len_l = len_s;
	}
#endif

debug(( 4, "Explicit xfer=%d", xfer ));

    } // else if ( xfer = EXS..Explicit ...
    else
    {
	l_error = EC_IllegalCall;
    }
debug(( 3, "TagAndLength read of: (0x%4.4x,0x%4.4x) \"%s\" [0x%8.8x] \"%s\"",
	   newTag.getGTag(), newTag.getETag(),
	   DcmVR(newTag.getVR()).getVRName(), len_l, newTag.getTagName() ));

    *length = len_l;		    // Rueckgabewert
    tag = newTag;                   // Rueckgabewert
Vdebug((4, l_error!=EC_Normal, "errorFlag(4,7)=(%d)", l_error ));
Edebug(());

    return l_error;
}


// ********************************


E_Condition DcmItem::readSubElement( DcmTag &newTag,
				     T_VR_UL newLength,
				     T_VR_UL tagLength,
                                     E_TransferSyntax xfer,
                                     E_GrpLenEncoding gltype )
{
Bdebug((4, "dcitem:DcmItem::readSubElement(&newTag,newLength=%ld,xfer=%d,gltype=%d)",
           newLength, xfer, gltype ));

    E_Condition l_error = EC_Normal;
    DcmElement *subElem = (DcmElement*)NULL;

    switch ( newTag.getVR() )
    {
	// Byte-Strings:
	case EVR_AE :
	    subElem = new DcmApplicationEntity( newTag, newLength, iDS );
	    break;
	case EVR_AS :
	    subElem = new DcmAgeString( newTag, newLength, iDS );
	    break;
	case EVR_CS :
	    subElem = new DcmCodeString( newTag, newLength, iDS );
	    break;
	case EVR_DA :
	    subElem = new DcmDate( newTag, newLength, iDS );
	    break;
	case EVR_DS :
	    subElem = new DcmDecimalString( newTag, newLength, iDS );
	    break;
	case EVR_DT :
	    subElem = new DcmDateTime( newTag, newLength, iDS );
	    break;
	case EVR_IS :
	    subElem = new DcmIntegerString( newTag, newLength, iDS );
	    break;
	case EVR_TM :
	    subElem = new DcmTime( newTag, newLength, iDS );
	    break;
	case EVR_UI :
	    subElem = new DcmUniqueIdentifier( newTag, newLength, iDS );
	    break;

	// Charakter-Strings:
	case EVR_LO :
	    subElem = new DcmLongString( newTag, newLength, iDS );
	    break;
	case EVR_LT :
	    subElem = new DcmLongText( newTag, newLength, iDS );
	    break;
	case EVR_PN :
	    subElem = new DcmPersonName( newTag, newLength, iDS );
	    break;
	case EVR_SH :
	    subElem = new DcmShortString( newTag, newLength, iDS );
	    break;
	case EVR_ST :
	    subElem = new DcmShortText( newTag, newLength, iDS );
	    break;

	// abhaengig von ByteOrder:
	case EVR_AT :
	    subElem = new DcmAttributeTag( newTag, newLength, iDS );
	    break;
	case EVR_SS :
	    subElem = new DcmSignedShort( newTag, newLength, iDS );
	    break;
	case EVR_xs : // laut Dicom-Standard V3.0
	case EVR_ux : // laut Dicom-Standard V3.0
	case EVR_US :
	    subElem = new DcmUnsignedShort( newTag, newLength, iDS );
	    break;
	case EVR_SL :
	    subElem = new DcmSignedLong( newTag, newLength, iDS );
	    break;
	case EVR_up : // fuer (0004,eeee) laut Dicom-Standard V3.0
	case EVR_UL :
	    {
                      // generiere Tag mit VR aus Dictionary!
                DcmTag ulupTag( newTag.getGTag(), newTag.getETag() );
                if ( ulupTag.getVR() == EVR_up )
		    subElem = new DcmUnsignedLongOffset( ulupTag,
							 newLength,
							 iDS );
		else
		    subElem = new DcmUnsignedLong( newTag,
						   newLength,
						   iDS );
	    }
	    break;
	case EVR_FL :
	    subElem = new DcmFloatingPointSingle( newTag, newLength, iDS );
	    break;
	case EVR_FD :
	    subElem = new DcmFloatingPointDouble( newTag, newLength, iDS );
	    break;

	// Sequences and Items
	case EVR_SQ :
	    subElem = new DcmSequenceOfItems( newTag, newLength, iDS );
	    break;
	case EVR_na :
            if ( newTag.getXTag() == ET_Item )
		l_error = EC_InvalidTag;
            else if ( newTag.getXTag() == ET_SequenceDelimitationItem )
		l_error = EC_InvalidTag;
            else if ( newTag.getXTag() == ET_ItemDelimitationItem )
		l_error = EC_SequEnd;
	    else
		l_error = EC_InvalidTag;
		break;

	// nicht-eindeutig 8- oder 16-Bit:
	case EVR_oa : // laut Dicom-Standard V3.0
	case EVR_oc : // laut Dicom-Standard V3.0
	case EVR_OB :
	case EVR_OW :
	case EVR_op :
	    {
                DcmXfer xferSyn( xfer );
                if ( newTag.getXTag() == ET_PixelData )
		{
		    if ( xferSyn.isEncapsulated() )
			subElem = new DcmPixelSequence( newTag, newLength, iDS );
		    else
		    {
			if ( xferSyn.isImplicitVR() )
                            newTag.setVR( EVR_OW );
			subElem = new DcmOtherByteOtherWord( newTag, newLength,
							     iDS );
		    }
		}
		else
		    subElem = new DcmOtherByteOtherWord( newTag, newLength, iDS );
	    }
	    break;

        // Gekapselte Pixeldaten als spezielle Sequenz:
        case EVR_pixelSQ :
	    subElem = new DcmPixelSequence( newTag, newLength, iDS );
	    break;

	// Falls das Parsing nicht so ganz funktionieren sollte:
	case EVR_pixelItem :
	    l_error = EC_InvalidTag;
	    break;

        // Retired Elements als Byte-String lesen:
	case EVR_RET :
	    subElem = new DcmOtherByteOtherWord( newTag, newLength, iDS );
	    break;

        // Unbekannte Typen als Byte-String lesen:
	case EVR_UNKNOWN :
	default :
	    subElem = new DcmOtherByteOtherWord( newTag, newLength, iDS );

        fprintf(stderr,
              "Warning: DcmItem::readSubElement(): unknown Tag detected:"
              " (0x%4.4hx,0x%4.4hx)\n", newTag.getGTag(), newTag.getETag() );
            break;
    }

    if ( l_error == EC_Normal && subElem != (DcmElement*)NULL )
    {
//	  DcmItem::elementList->insert( subElem, ELP_next );  // etwas schneller
	insert( subElem );				      // sicherer
	if ( !iDS->fromFile() && !lastElementComplete )
            l_error = subElem->readBlock( xfer, gltype );
        else                                 // hier wird Subelement eingelesen
            l_error = subElem->read( xfer, gltype );
    }
    else if ( l_error == EC_InvalidTag )  // versuche das Parsing wieder
    {					  // einzurenken
	for ( T_VR_UL i=0; i<tagLength; i++ )
	    iDS->unget();		  // schreibe Tag wieder in den Stream

	fprintf(stderr,
	       "Warning: DcmItem::readSubElement(): parse error occurred:"
                " (0x%4.4hx,0x%4.4hx)\n", newTag.getGTag(), newTag.getETag() );
debug(( 1, "Warning: DcmItem::readSubElement(): parse error occurred:"
           " (0x%4.4hx,0x%4.4hx)\n", newTag.getGTag(), newTag.getETag() ));

    }
    else if ( l_error != EC_SequEnd )
    {

	fprintf(stderr,
	       "Error: DcmItem::readSubElement(): cannot create SubElement:"
                " (0x%4.4hx,0x%4.4hx)\n", newTag.getGTag(), newTag.getETag() );
debug(( 1, "Error: DcmItem::readSubElement(): cannot create SubElement:"
           " (0x%4.4hx,0x%4.4hx)\n", newTag.getGTag(), newTag.getETag() ));

    }
Vdebug((4, l_error!=EC_Normal, "errorFlag(8)=(%d)", l_error ));
Edebug(());

    return l_error;
}


// ********************************


E_Condition DcmItem::read( E_TransferSyntax xfer,
                           E_GrpLenEncoding gltype )
{
Bdebug((3, "dcitem:DcmItem::read(xfer=%d,gltype=%d)", xfer, gltype ));

    Xfer = xfer;
    E_TransferSyntax newxfer = xfer;
    T_VR_UL bytes_read = 0;

    errorFlag = EC_Normal;
    if ( iDS == (iDicomStream*)NULL )
        errorFlag = EC_InvalidStream;
    else if ( iDS->eof() )
	errorFlag = EC_EndOfFile;
    else if ( !iDS->good() )
	errorFlag = EC_InvalidStream;
    else
    {
	offsetInFile = iDS->tellg() - 8;  // Position von Item-Start
	iDS->setDataByteOrder( newxfer );

        DcmTag newTag( ET_UNKNOWN );
	while ( iDS->good() && bytes_read < Length )
	{
	    T_VR_UL newValueLength = 0;
	    T_VR_UL bytes_tagAndLen = 0;
	    errorFlag = readTagAndLength( newxfer,
					  newTag,
					  &newValueLength,
					  &bytes_tagAndLen );
	    bytes_read += bytes_tagAndLen;
debug(( 3, "Item: Header_Bytes_read           =[0x%8.8x]", bytes_read ));

	    if ( !iDS->good() ) 	  // evtl. Stream zuende
		break;			  // beende while-Schleife

	    errorFlag = readSubElement( newTag,
					newValueLength,
					bytes_tagAndLen,
                                        newxfer,
                                        gltype );
	    bytes_read = (T_VR_UL)(iDS->tellg() - (signed long)offsetInFile - 8);

debug(( 3, "Item: Bytes_read                  =[0x%8.8x]", bytes_read ));

	    if ( errorFlag != EC_Normal )
		break;			  // beende while-Schleife

	} //while ( iDS->good()..
    } // else errorFlag
    valueModified = FALSE;
    if (   errorFlag == EC_SequEnd
	|| errorFlag == EC_InvalidTag
	|| errorFlag == EC_EndOfFile )
	errorFlag = EC_Normal;
Edebug(());

    return errorFlag;
} // DcmItem::read()


// ********************************


E_Condition DcmItem::readBlock( E_TransferSyntax xfer,
                                E_GrpLenEncoding gltype )
{
Bdebug((3, "dcitem:DcmItem::readBlock(xfer=%d,gltype=%d)", xfer, gltype ));

    Xfer = xfer;
    E_TransferSyntax newxfer = xfer;

    errorFlag = EC_Normal;
    if ( iDS == (iDicomStream*)NULL )
        errorFlag = EC_InvalidStream;
    else if ( iDS->eof() || iDS->fromFile() )
	errorFlag = EC_EndOfFile;
    else if ( !iDS->good() )
	errorFlag = EC_InvalidStream;
    else if ( readState() != ERW_ready )
    {
	if ( readState() == ERW_init )
	    offsetInFile = iDS->tellg() - 8;  // Position von Item-Start
	iDS->setDataByteOrder( newxfer );

        DcmTag newTag( ET_UNKNOWN );
	while (    iDS->good()
		&& ( bytesRead < Length || !lastElementComplete ) )
	{
	    T_VR_UL newValueLength = 0;
	    T_VR_UL bytes_tagAndLen = 0;
	    if ( lastElementComplete )
	    {
		errorFlag = readTagAndLength( newxfer,
					      newTag,
					      &newValueLength,
					      &bytes_tagAndLen );
		bytesRead += bytes_tagAndLen;
                if ( errorFlag != EC_Normal )
		    break;			// beende while-Schleife

		lastElementComplete = FALSE;
		errorFlag = readSubElement( newTag,
					    newValueLength,
					    bytes_tagAndLen,
                                            newxfer,
                                            gltype );
		if ( errorFlag == EC_Normal )
		    lastElementComplete = TRUE;
	    }
	    else
	    {
                errorFlag = elementList->get()->readBlock( xfer, gltype ) ;
		if ( errorFlag == EC_Normal )
		    lastElementComplete = TRUE;
	    }
	    bytesRead = (T_VR_UL)(iDS->tellg() - (signed long)offsetInFile - 8);

	    if ( errorFlag != EC_Normal )
		break;				// beende while-Schleife

	} //while ( iDS->good()..
	if (	( bytesRead < Length || !lastElementComplete )
	     && errorFlag != EC_SequEnd )
	    errorFlag = EC_InvalidStream;
	if ( iDS->eof() )
	    errorFlag = EC_EndOfFile;
    } // else errorFlag
    valueModified = FALSE;

    if ( errorFlag == EC_SequEnd || errorFlag == EC_EndOfFile )
	errorFlag = EC_Normal;
    if ( errorFlag == EC_Normal )
	rdStat = ERW_ready;	     // Item ist komplett
debug(( 3, "errorFlag=(%d)", errorFlag ));
Edebug(());

    return errorFlag;
} // DcmItem::read()


// ********************************


E_Condition DcmItem::write( oDicomStream &oDS,
			    E_TransferSyntax oxfer,
                            E_EncodingType enctype,
                            E_GrpLenEncoding gltype )
{
Bdebug((3, "dcitem:DcmItem::write(&oDS,oxfer=%d,enctype=%d,gltype=%d)",
           oxfer, enctype, gltype ));

    errorFlag = EC_Normal;
    if ( !oDS.good() )
	errorFlag = EC_InvalidStream;
    else
    {
	if ( enctype == EET_ExplicitLength )
	    Length = this->getLength( oxfer, enctype );
				  // getLength kann u.U. auch UNDEF_LEN liefern
	else
	    Length = UNDEF_LEN;

        oDS.setDataByteOrder( oxfer );
        oDS << Tag->getGTag();         // 2 Byte Laenge; Transfer Syntax !!
        oDS << Tag->getETag();         // 2 Byte Laenge; Transfer Syntax !!
        oDS << Length;                 // 4 Byte Laenge; Transfer Syntax !!

	if ( !elementList->empty() )
	{
	    DcmObject *dO;
	    elementList->seek( ELP_first );
	    do {
		dO = elementList->get();
                errorFlag = dO->write( oDS, oxfer, enctype, gltype );
	    } while ( elementList->seek( ELP_next ) && errorFlag == EC_Normal );
	}

	if ( Length == UNDEF_LEN )	// schreibe ItemDelimitationItem
	{
            DcmTag delim( ET_ItemDelimitationItem );
            oDS << delim.getGTag();   // 2 Byte Laenge; Transfer Syntax !!
            oDS << delim.getETag();   // 2 Byte Laenge; Transfer Syntax !!
	    T_VR_UL delimLen = 0L;
            oDS << delimLen;           // 4 Byte Laenge; Transfer Syntax !!
	}

	valueModified = FALSE;
    }
Edebug(());

    return errorFlag;
}


// ********************************


E_Condition DcmItem::writeBlock( oDicomStream &oDS,
				 E_TransferSyntax oxfer,
                                 E_EncodingType enctype,
                                 E_GrpLenEncoding gltype )
{
Bdebug((3, "dcitem:DcmItem::writeBlock(&oDS,oxfer=%d,enctype=%d,gltype=%d)",
           oxfer, enctype, gltype ));

    errorFlag = EC_Normal;
    if ( !oDS.good() || oDS.intoFile() )
	errorFlag = EC_InvalidStream;
    else
    {
	if ( writeState() != ERW_ready )
	{
	    if ( writeState() == ERW_init )
	    {
                if ( oDS.avail() >= 8 )
		{
		    if ( enctype == EET_ExplicitLength )
			Length = this->getLength( oxfer, enctype );
				  // getLength kann u.U. auch UNDEF_LEN liefern
		    else
			Length = UNDEF_LEN;

                    oDS.setDataByteOrder( oxfer );
                    oDS << Tag->getGTag(); // 2 Byte Laenge; Transfer Syntax !!
                    oDS << Tag->getETag(); // 2 Byte Laenge; Transfer Syntax !!
                    oDS << Length;         // 4 Byte Laenge; Transfer Syntax !!
		    wrStat = ERW_inWork;
		    elementList->seek( ELP_first );
		}
		else
		    errorFlag = EC_InvalidStream;
	    }
	    if ( !elementList->empty() && writeState() != ERW_init )
	    {
		DcmObject *dO;
		do {
		    dO = elementList->get();
                    errorFlag = dO->writeBlock( oDS, oxfer, enctype, gltype );
		} while (    errorFlag == EC_Normal
			  && elementList->seek( ELP_next ) );
	    }

	    if ( errorFlag == EC_Normal && writeState() != ERW_init )
	    {
                if ( Length == UNDEF_LEN && oDS.avail() >= 8 )
					     // schreibe ItemDelimitationItem
		{
                    DcmTag delim( ET_ItemDelimitationItem );
                    oDS << delim.getGTag(); // 2 Byte Laenge; Transfer Syntax !
                    oDS << delim.getETag(); // 2 Byte Laenge; Transfer Syntax !
		    T_VR_UL delimLen = 0L;
                    oDS << delimLen;         // 4 Byte Laenge; Transfer Syntax !
		}
		wrStat = ERW_ready;
	    }
	    valueModified = FALSE;
	}
    }
Edebug(());

    return errorFlag;
}


// ********************************


E_Condition DcmItem::readBlockInit()
{
    rdStat = ERW_init;
    lastElementComplete = TRUE;
    if ( !elementList->empty() )
    {
	elementList->seek( ELP_first );
	do {
	    elementList->get()->readBlockInit();

	} while ( elementList->seek( ELP_next ) );
    }
    return EC_Normal;
}


// ********************************


E_Condition DcmItem::writeBlockInit()
{
    wrStat = ERW_init;
    if ( !elementList->empty() )
    {
	elementList->seek( ELP_first );
	do {
	    elementList->get()->readBlockInit();

	} while ( elementList->seek( ELP_next ) );
    }
    return EC_Normal;
}


// ********************************


T_VR_UL DcmItem::card()
{
    return elementList->card();
}


// ********************************


E_Condition DcmItem::insert( DcmElement* elem,
			     BOOL replaceOld )
{						  // geordnetes Einfuegen

Bdebug((3, "dcitem:DcmItem::insert(DcmElement*=%p,BOOL replaceOld=%d)",
           elem, replaceOld ));

    errorFlag = EC_Normal;
    if ( elem != (DcmElement*)NULL )
    {
	DcmElement *dE;
	E_ListPos seekmode = ELP_last;
	do {
	    dE = (DcmElement*)(elementList->seek( seekmode ));
	    if ( dE == (DcmElement*)NULL )     // elementList ist leer
					       // oder elem hat kleinstes Tag
	    {
		elementList->insert( elem, ELP_first );
		valueModified = TRUE;
debug(( 3, "element (0x%4.4x,0x%4.4x) / VR=\"%s\" at beginning inserted",
	   elem->getGTag(), elem->getETag(), DcmVR(elem->getVR()).getVRName() ));

		break;
	    }
            else if ( elem->getTag() > dE->getTag() )
					       // Position gefunden
	    {
		elementList->insert( elem, ELP_next );
		valueModified = TRUE;
debug(( 3, "element (0x%4.4x,0x%4.4x) / VR=\"%s\" inserted",
	   elem->getGTag(), elem->getETag(),
	   DcmVR(elem->getVR()).getVRName() ));

		break;
	    }
            else if ( elem->getTag() == dE->getTag() )
	    {				       // Tag ist schon vorhanden.
		if ( elem != dE )	       // Altes und neues Element
		{			       // sind verschieden.
		    if ( replaceOld )	       // Altes Elem. wird geloescht.
		    {
			DcmObject *remObj = elementList->remove();
					       // Es gilt: remObj == dE
					       // Liste zeigt auf naechsten
debug(( 3, "DcmItem::insert:element (0x%4.4x,0x%4.4x) VR=\"%s\" p=%p removed",
	   remObj->getGTag(), remObj->getETag(),
	   DcmVR(remObj->getVR()).getVRName(), remObj ));

			    if ( remObj != (DcmObject*)NULL )
			    {
				delete remObj; // loesche "abgehaengtes" Element
debug(( 3, "DcmItem::insert:element p=%p deleted", remObj ));

			    }
			    elementList->insert( elem, ELP_prev );
			    valueModified = TRUE;
debug(( 3, "element (0x%4.4x,0x%4.4x) VR=\"%s\" p=%p replaced older one",
	   elem->getGTag(), elem->getETag(),
	   DcmVR(elem->getVR()).getVRName(), elem ));

		    }	// if ( replaceOld )
		    else
		    {
debug(( 1, "element with (0x%4.4x,0x%4.4x) VR=\"%s\" is already inserted:",
	   elem->getGTag(), elem->getETag(),
	   DcmVR(elem->getVR()).getVRName() ));

		    }	// if ( !replaceOld )
		}   // if ( elem != dE )
		else	     // Versuch, Listen-Element nochmals einzufuegen
		{
		    fprintf(stderr,
			  "Warning: DcmItem::insert(): element (0x%4.4x,"
			  "0x%4.4x) VR=\"%s\" p=%p was already in list:"
			  " not inserted\n",
			  elem->getGTag(), elem->getETag(),
			  DcmVR(elem->getVR()).getVRName(), elem );
		}
		errorFlag = EC_DoubledTag;
		break;
	    }
	    // else : nicht noetig!
	    seekmode = ELP_prev;
	} while ( dE );
    }
    else
	errorFlag = EC_IllegalCall;
Edebug(());

    return errorFlag;
}


// ********************************


DcmElement* DcmItem::getElement( T_VR_UL num )
{
Bdebug((5, "dcitem:DcmItem::getElement(num=%d)", num ));

    errorFlag = EC_Normal;
    DcmElement *elem;
    elem = (DcmElement*)( elementList->seek_to(num) );
						// liest Element aus Liste
    if ( elem == (DcmElement*)NULL )
	errorFlag = EC_IllegalCall;
Edebug(());

    return elem;
}


// ********************************


DcmElement* DcmItem::remove( T_VR_UL num )
{
Bdebug((3, "dcitem:DcmItem::remove(num=%d)", num ));

    errorFlag = EC_Normal;
    DcmElement *elem;
    elem = (DcmElement*)( elementList->seek_to(num) );
						// liest Element aus Liste
    if ( elem != (DcmElement*)NULL )
    {
debug(( 3, "element p=%p removed, but not deleted", elem ));

	elementList->remove();			// entfernt Element aus Liste,
    }						// aber loescht es nicht
    else
	errorFlag = EC_IllegalCall;
Edebug(());

    return elem;
}


// ********************************


DcmElement* DcmItem::remove( DcmObject* elem )
{
Bdebug((3, "dcitem:DcmItem::remove(DcmObject*=%p)", elem ));

    errorFlag = EC_IllegalCall;
    if ( !elementList->empty() && elem != (DcmObject*)NULL )
    {
	DcmObject *dO;
	elementList->seek( ELP_first );
	do {
	    dO = elementList->get();
	    if ( dO == elem )
	    {
debug(( 3, "element p=%p removed, but not deleted", elem ));

		elementList->remove();	       // entfernt Element aus Liste,
					       // aber loescht es nicht
		errorFlag = EC_Normal;
		break;
	    }
	} while ( elementList->seek( ELP_next ) );
    }
Edebug(());

    if ( errorFlag == EC_IllegalCall )
	return (DcmElement*)NULL;
    else
	return (DcmElement*)elem;
}


// ********************************


DcmElement* DcmItem::remove( DcmTag& tag )
{
Bdebug((3, "dcitem:DcmItem::remove(tag=(%4.4x,%4.4x))",
           tag.getGTag(), tag.getETag() ));

    errorFlag = EC_TagNotFound;
    DcmObject *dO = (DcmObject*)NULL;
    if ( !elementList->empty() )
    {
	elementList->seek( ELP_first );
	do {
	    dO = elementList->get();
            if ( tag == dO->getTag() )
	    {
debug(( 3, "element p=%p removed, but not deleted", dO ));

		elementList->remove();	       // entfernt Element aus Liste,
					       // aber loescht es nicht
		errorFlag = EC_Normal;
		break;
	    }
	} while ( elementList->seek( ELP_next ) );
    }
Edebug(());

    if ( errorFlag == EC_TagNotFound )
	return (DcmElement*)NULL;
    else
        return (DcmElement*)dO;
}


// ********************************


E_Condition DcmItem::clear()
{
Bdebug((2, "dcitem:DcmItem::clear()"));

    errorFlag = EC_Normal;
    DcmObject *dO;
    elementList->seek( ELP_first );
    while ( !elementList->empty() )
    {
	dO = elementList->remove();
	if ( dO != (DcmObject*)NULL )
	    delete dO;				// loesche auch Sub-Elemente
    }
    Length = 0;
Edebug(());

    return errorFlag;
}


// ********************************


E_Condition DcmItem::verify( BOOL autocorrect )
{
Bdebug((3, "dcitem:DcmItem::verify(autocorrect=%d)", autocorrect ));
debug(( 3, "Tag=(0x%4.4x,0x%4.4x) \"%s\" \"%s\"",
	   getGTag(), getETag(),
	   DcmVR(getVR()).getVRName(), Tag->getTagName() ));

    errorFlag = EC_Normal;
    if ( !elementList->empty() )
    {
	DcmObject *dO;
	elementList->seek( ELP_first );
	do {
	    dO = elementList->get();
	    if ( dO->verify( autocorrect ) != EC_Normal )
		errorFlag = EC_CorruptedData;
	} while ( elementList->seek( ELP_next ) );
    }
    if ( autocorrect == TRUE )
	Length = this->getLength();
Edebug(());

    return errorFlag;
}


// ********************************

    // Vorbedingung: elementList ist nicht leer!
    // Ergebnis:     - return EC_Normal;
    //		       gefundener Element-Zeiger auf resultStack
    //		     - return EC_TagNotFound;
    //		       resultStack unveraendert
    // Weitersuchen: schreibe Zeiger des Sub-Elementes auf resultStack und
    //		     starte dann Sub-Suche


E_Condition DcmItem::searchSubFromHere( DcmTag &tag,
					DcmStack &resultStack,
					BOOL searchIntoSub )
{
Bdebug((5, "dcitem:DcmItem::searchSubFromHere(tag=(%4.4x,%4.4x),Stack&(%ld),"
           "sub=%d)", tag.getGTag(), tag.getETag(), resultStack.card(),
           searchIntoSub ));

    DcmObject *dO;
    E_Condition l_error = EC_TagNotFound;
    if ( !elementList->empty() )
    {
        elementList->seek( ELP_first );
        do {
            dO = elementList->get();
            if ( searchIntoSub )
            {
                resultStack.push( dO );
                if ( tag == dO->getTag() )
                    l_error = EC_Normal;
                else
                    l_error = dO->search( tag,
                                          resultStack,
                                          ESM_fromStackTop,
                                          TRUE );
                if ( l_error != EC_Normal )
                    resultStack.pop();
            }
            else
            {
                if ( tag == dO->getTag() )
                {
                    resultStack.push( dO );
                    l_error = EC_Normal;
                }
            }
        } while ( l_error != EC_Normal && elementList->seek( ELP_next ) );
Vdebug((4, l_error==EC_Normal && tag==dO->getTag(), "Search-Tag=(%4.4x,%4.4x)"
           " \"%s\" found!", tag.getGTag(), tag.getETag(),
           DcmVR(tag.getVR()).getVRName() ));

    }
Edebug(());

    return l_error;
}


// ********************************


E_Condition DcmItem::search( DcmTag &tag,
			     DcmStack &resultStack,
			     E_SearchMode mode,
			     BOOL searchIntoSub )
{
Bdebug((5, "dcitem:DcmItem::search(tag=(%4.4x,%4.4x),Stack&(%ld),mode=%d,"
           "sub=%d)", tag.getGTag(), tag.getETag(), resultStack.card(),
           mode, searchIntoSub ));
debug(( 5, "local Info: Tag=(%4.4x,%4.4x) \"%s\" p=%p",
           getGTag(), getETag(), DcmVR(getVR()).getVRName(), this ));

    DcmObject *dO = (DcmObject*)NULL;
    E_Condition l_error = EC_TagNotFound;
    if ( mode == ESM_afterStackTop && resultStack.top() == this )
    {
        l_error = searchSubFromHere( tag, resultStack, searchIntoSub );
debug(( 5, "mode=ESM_afterStackTop && resultStack.top()=this" ));

    }
    else if ( !elementList->empty() )
    {
        if ( mode == ESM_fromHere || resultStack.empty() )
	{
debug(( 5, "mode=ESM_fromHere || resultStack.empty()" ));

	    resultStack.clear();
	    l_error = searchSubFromHere( tag, resultStack, searchIntoSub );
	}
	else if ( mode == ESM_fromStackTop )
	{
debug(( 5, "mode=ESM_fromStackTop" ));

	    dO = resultStack.top();
	    if ( dO == this )
		l_error = searchSubFromHere( tag, resultStack, searchIntoSub );
	    else
	    {	// gehe direkt zu Sub-Baum und suche dort weiter
		l_error = dO->search( tag, resultStack, mode, searchIntoSub );
                if ( l_error != EC_Normal ) // raeumt nur die oberste Stackebene
                    resultStack.pop();      // ab; der Rest ist unveraendert
	    }
	}
	else if ( mode == ESM_afterStackTop && searchIntoSub )
	{
debug(( 5, "mode=ESM_afterStackTop && searchIntoSub" ));

	    // resultStack enthaelt Zielinformationen:
	    // - stelle Zustand der letzen Suche in den einzelnen Suchroutinen
	    //	 wieder her
	    // - finde Position von dO in Baum-Struktur
	    //	 1. suche eigenen Stack-Eintrag
	    //	    - bei Fehlschlag Suche beenden
	    //	 2. nehme naechsthoeheren Eintrag dnO
	    //	 3. stelle eigene Liste auf Position von dnO
	    //	 4. starte Suche ab dnO

	    T_VR_UL i = resultStack.card();
debug(( 5, "resultStack: card()=%d", i ));

	    while ( i > 0 && (dO = resultStack.elem(i-1)) != this )
	    {
debug(( 5, "--dO=elem(%d)=%p  this=%p", i-1, dO, this ));

		i--;
	    }
	    if ( dO != this && resultStack.card() > 0 )
	    {			       // oberste Ebene steht nie in resultStack
		i = resultStack.card()+1;// zeige jetzt auf hoechste Ebene+1
                dO = this;               // Treffer der hoechsten Ebene!
	    }
	    if ( dO == this )
	    {
debug(( 5, "--dO=elem(%d)=%p==this=%p", i-1, dO, this ));
debug(( 5, "currently at resultStack position %d, dO=%p", i-1, dO ));

                if ( i == 1 )                 // habe resultStack.top() gefunden
                    l_error = EC_TagNotFound; // markiere als kein Treffer, s.o.
                else                          //   siehe oben
		{
		    E_SearchMode submode = mode;
		    BOOL searchNode = TRUE;
		    DcmObject *dnO;
		    dnO = resultStack.elem( i-2 ); // Knoten der naechsten Ebene
debug(( 5, "elementList: dnO=%p", dnO ));

		    elementList->seek( ELP_first );
		    do {
			dO = elementList->get();
			searchNode = searchNode ? ( dO != dnO ) : FALSE;
Vdebug((5, searchNode,  "--searching Node dnO=%p, found: dO=%p", dnO, dO ));
Vdebug((5, !searchNode && submode==ESM_afterStackTop,
                        "--searching Node dnO=%p found!", dO ));
Vdebug((5, !searchNode && submode!=ESM_afterStackTop,
                        "--next Node dO=%p for beeing tested", dO ));

			if ( !searchNode )
			{				// suche jetzt weiter
			    if ( submode == ESM_fromStackTop )
				resultStack.push( dO ); // Stack aktualisieren
                            if (    submode == ESM_fromStackTop
                                 && tag == dO->getTag()
                               )
                                l_error = EC_Normal;
                            else
                                l_error = dO->search( tag,
                                                      resultStack,
                                                      submode,
                                                      TRUE );
			    if ( l_error != EC_Normal )
                                    resultStack.pop();
			    else
				break;
			    submode = ESM_fromStackTop; // ab hier normale Suche
			}
		    } while ( elementList->seek( ELP_next ) );
		}
	    }
	    else
		l_error = EC_IllegalCall;
	} // ( mode == ESM_afterStackTop
	else
	    l_error = EC_IllegalCall;
    }
Edebug(());

    return l_error;
}


// ********************************


E_Condition DcmItem::search( ETag etag,
			     DcmStack &resultStack,
			     E_SearchMode mode,
			     BOOL searchIntoSub )
{
Bdebug((5, "dcitem:DcmItem::search(etag=%d,Stack&,mode=%d,sub=%d)",
	   etag, mode, searchIntoSub ));

    DcmTag tag( etag );
    E_Condition l_error = search( tag, resultStack, mode, searchIntoSub );
Edebug(());

    return l_error;
}


// ********************************


E_Condition DcmItem::searchErrors( DcmStack &resultStack )
{
Bdebug((5, "dcitem:DcmItem::searchErrors(Stack&)" ));

    E_Condition l_error = errorFlag;
    DcmObject *dO = (DcmObject*)NULL;
    if ( errorFlag != EC_Normal )
	resultStack.push( this );
    if ( !elementList->empty() )
    {
	elementList->seek( ELP_first );
	do {
	    E_Condition err;
	    dO = elementList->get();
	    if ( (err = dO->searchErrors( resultStack )) != EC_Normal )
		l_error = err;
	} while ( elementList->seek( ELP_next ) );
    }
Edebug(());

    return l_error;
}


// ********************************


E_Condition DcmItem::loadAllDataIntoMemory()
{
Bdebug((3, "dcitem:DcmItem::loadAllDataIntoMemory()"));

    E_Condition l_error = EC_Normal;
    if ( !elementList->empty() )
    {
	elementList->seek( ELP_first );
	do {
	    E_Condition err;
            DcmObject *dO = elementList->get();
            if ( (err = dO->loadAllDataIntoMemory()) != EC_Normal )
		l_error = err;
	} while ( elementList->seek( ELP_next ) );
    }
Edebug(());

    return l_error;
}


// ********************************


