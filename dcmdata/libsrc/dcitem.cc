/*
**
** Author: Gerd Ehlers      26.04.94 -- Created
**         Andreas Barth    30.11.95 -- New Stream classes and value here
** Kuratorium OFFIS e.V.
**
** Module: dcitem.cc
**
** Purpose:
** Implementation of the class DcmItem
**
**
** Last Update:		$Author: andreas $
** Update Date:		$Date: 1996-07-31 13:14:30 $
** Source File:		$Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/libsrc/dcitem.cc,v $
** CVS/RCS Revision:	$Revision: 1.14 $
** Status:		$State: Exp $
**
** CVS/RCS Log at end of file
**
*/


#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include <stdlib.h>
#include <stdio.h>
#include <iostream.h>
#include <string.h>
#include <ctype.h>

#include "dctk.h"
#include "dcitem.h"
#include "dcobject.h"
#include "dcvr.h"
#include "dcxfer.h"
#include "dcdebug.h"



// ********************************


DcmItem::DcmItem()
    : DcmObject( ItemTag )
{
    Bdebug((5, "dcitem:DcmItem::DcmItem()" ));
    debug(( 8, "Object pointer this=0x%p", this ));

    elementList = new DcmList;
    lastElementComplete = TRUE;
    fStartPosition = 0;
    Edebug(());

}


// ********************************


DcmItem::DcmItem(const DcmTag &tag, const Uint32 len)
    : DcmObject(tag, len)
{
    Bdebug((5, "dcitem:DcmItem::DcmItem(DcmTag&,len=%ld)", len ));
    debug(( 8, "Object pointer this=0x%p", this ));

    elementList = new DcmList;
    lastElementComplete = TRUE;
    fStartPosition = 0;
    Edebug(());

}


// ********************************


DcmItem::DcmItem( const DcmItem& old )
    : DcmObject( old )
{
    Bdebug((5, "dcitem:DcmItem::DcmItem(DcmItem&)"));
    debug(( 8, "Object pointer this=0x%p", this ));

    lastElementComplete = TRUE;
    fStartPosition = old.fStartPosition;
    elementList = new DcmList;
    debug(( 5, "ident()=%d", old.ident() ));

    switch ( old.ident() ) {
    case EVR_item:
    case EVR_dirRecord:
    case EVR_dataset:
    case EVR_metainfo:
	if ( !old.elementList->empty() ) {
	    DcmObject *oldDO;
	    DcmObject *newDO;
	    elementList->seek( ELP_first );
	    old.elementList->seek( ELP_first );
	    do {
		oldDO = old.elementList->get();
		newDO = copyDcmObject( oldDO );

		elementList->insert( newDO, ELP_next );
	    } while ( old.elementList->seek( ELP_next ) );
	}
	break;
    default:
        cerr << "Warning: DcmItem: wrong use of Copy-Constructor" << endl;
	break;
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
    while ( !elementList->empty() ) {
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
    char c1 =  atposition[0];
    char c2 = atposition[1];
    BOOL valid = FALSE;
        
    if (isalpha(c1) && isalpha(c2)) {
        char vrName[3];
        vrName[0] = c1;
        vrName[1] = c2;
        vrName[2] = '\0';
    
    	/* is this VR name a standard VR descriptor */
    	DcmVR vr(vrName);
    	valid = vr.isStandard();
    } else {
        /* cannot be a valid VR name since non-characters */
        valid = FALSE;
    }
    return valid;    
}


// ********************************

/*
   wird in DcmDataset und in DcmMetaInfo benutzt
*/

E_TransferSyntax DcmItem::checkTransferSyntax(DcmStream & inStream)
{
    Bdebug((3, "dcitem:DcmItem::checkTransferSyntax()" ));

    E_TransferSyntax transferSyntax;
    char tagAndVR[6];
    inStream.SetPutbackMark();
    inStream.ReadBytes(tagAndVR, 6);		   // check Tag & VR
    inStream.Putback();

    char c1 = tagAndVR[0];
    char c2 = tagAndVR[1];
    char c3 = tagAndVR[2];
    char c4 = tagAndVR[3];
    Uint16 t1 = (c1 & 0xff) + ((c2 & 0xff) << 8);  // explizite little endian
    Uint16 t2 = (c3 & 0xff) + ((c4 & 0xff) << 8);  // Konversion
    DcmTag taglittle(t1, t2);
    DcmTag tagbig(swapShort(t1), swapShort(t2));

    if (taglittle.error() && tagbig.error()) {	    // no valid tag
	if (foundVR( &tagAndVR[4])) {		    // assume little-endian
	    transferSyntax = EXS_LittleEndianExplicit;
	} else {
	    transferSyntax = EXS_LittleEndianImplicit;
	}
    } else {
	if ( foundVR( &tagAndVR[4] ) )	{	    // explicit VR
	    if ( taglittle.error() ) {
		transferSyntax = EXS_BigEndianExplicit;
	    } else if ( tagbig.error() ) {
		transferSyntax = EXS_LittleEndianExplicit;
	    } else { /* if both are error-free then assume little-endian */
		transferSyntax = EXS_LittleEndianExplicit;
	    }
	} else	{				    // implicit VR
	    if ( taglittle.error() ) {
		transferSyntax = EXS_BigEndianImplicit;
	    } else if ( tagbig.error() ) {
		transferSyntax = EXS_LittleEndianImplicit;
	    } else { /* if both are error-free then assume little-endian */
		transferSyntax = EXS_LittleEndianImplicit;
	    }
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
	newObj = new DcmApplicationEntity( *(DcmApplicationEntity*)(oldObj) );
	break;
    case EVR_AS :
	newObj = new DcmAgeString( *(DcmAgeString*)oldObj );
	break;
    case EVR_CS :
	newObj = new DcmCodeString( *(DcmCodeString*)oldObj );
	break;
    case EVR_DA :
	newObj = new DcmDate( *(DcmDate*)oldObj );
	break;
    case EVR_DS :
	newObj = new DcmDecimalString( *(DcmDecimalString*)oldObj );
	break;
    case EVR_DT :
	newObj = new DcmDateTime( *(DcmDateTime*)oldObj );
	break;
    case EVR_IS :
	newObj = new DcmIntegerString( *(DcmIntegerString*)oldObj );
	break;
    case EVR_TM :
	newObj = new DcmTime( *(DcmTime*)oldObj );
	break;
    case EVR_UI :
	newObj = new DcmUniqueIdentifier( *(DcmUniqueIdentifier*)oldObj );
	break;

	// Charakter-Strings:
    case EVR_LO :
	newObj = new DcmLongString( *(DcmLongString*)oldObj );
	break;
    case EVR_LT :
	newObj = new DcmLongText( *(DcmLongText*)oldObj );
	break;
    case EVR_PN :
	newObj = new DcmPersonName( *(DcmPersonName*)oldObj );
	break;
    case EVR_SH :
	newObj = new DcmShortString( *(DcmShortString*)oldObj );
	break;
    case EVR_ST :
	newObj = new DcmShortText( *(DcmShortText*)oldObj );
	break;

	// abhaengig von ByteOrder:
    case EVR_AT :
	newObj = new DcmAttributeTag( *(DcmAttributeTag*)oldObj );
	break;
    case EVR_SS :
	newObj = new DcmSignedShort( *(DcmSignedShort*)oldObj );
	break;
    case EVR_xs : // laut Dicom-Standard V3.0
    case EVR_US :
	newObj = new DcmUnsignedShort( *(DcmUnsignedShort*)oldObj );
	break;
    case EVR_SL :
	newObj = new DcmSignedLong( *(DcmSignedLong*)oldObj );
	break;
    case EVR_up : // fuer (0004,eeee) laut Dicom-Standard V3.0
	newObj = new DcmUnsignedLongOffset( *(DcmUnsignedLongOffset*)oldObj );
	break;
    case EVR_UL :
	newObj = new DcmUnsignedLong( *(DcmUnsignedLong*)oldObj );
	break;
    case EVR_FL :
	newObj = new DcmFloatingPointSingle( *(DcmFloatingPointSingle*)oldObj );
	break;
    case EVR_FD :
	newObj = new DcmFloatingPointDouble( *(DcmFloatingPointDouble*)oldObj );
	break;

	// Sequenzen
    case EVR_SQ :
	newObj = new DcmSequenceOfItems( *(DcmSequenceOfItems*)oldObj );
	break;

	// nicht-eindeutig 8- oder 16-Bit:
    case EVR_OB :
    case EVR_OW :
    case EVR_ox :
	newObj = new DcmOtherByteOtherWord( *(DcmOtherByteOtherWord*)oldObj );
	break;


	// Gekapselte Pixeldaten als spezielle Sequenz:
    case EVR_pixelSQ :
	newObj = new DcmPixelSequence( *(DcmPixelSequence*)oldObj );
	break;

	// Treat unknown elements as Byte-String:
    case EVR_UNKNOWN :
	newObj = new DcmOtherByteOtherWord( *(DcmOtherByteOtherWord*)oldObj );
	break;

    case EVR_na :
    default :
	cerr << "Warning: DcmItem::copyDcmObject(): unsupported Element("
	     << hex << oldObj->getGTag() << "," << oldObj->getETag()
	     << dec << ") with ident()=" 
	     << DcmVR(oldObj->ident()).getVRName() << " found."
	     << endl;
	break;
    }
    Edebug(());

    return newObj;
}


// ********************************


DcmEVR DcmItem::ident() const
{
    return EVR_item;
}


// ********************************


void DcmItem::print(const int level)
{
    char *info = new char[200];
    char *title = (char*)NULL;
    if ( Length == DCM_UndefinedLength )
	title = "Item with undefined length";
    else
	title = "Item with explicit Length";

    sprintf( info, "(%s  #=%ld)", title, (long)card() );
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

    DcmTag delimItemTag( DCM_ItemDelimitationItem );
    if ( Length == DCM_UndefinedLength)
	DcmObject::printInfoLine( level, delimItemTag,
				  0, "(ItemDelimitationItem)" );
    else
	DcmObject::printInfoLine( level, delimItemTag,
				  0, "(ItemDelimitationItem for re-encoding)" );
    delete info;
}


// ********************************


unsigned long DcmItem::getVM()
{
    return 1L;
}


// ********************************

/*
   siehe auch DcmDicomDir::lengthUntilSQ()
*/

Uint32 DcmItem::calcHeaderLength(DcmEVR vr, const E_TransferSyntax xfer)
{
    Uint32 templen = 0;
    DcmXfer xferSyn(xfer);
    if (xferSyn.isExplicitVR())
    {
	switch ( vr )
	{
	case EVR_ox :
	case EVR_OB :
	case EVR_OW :
	case EVR_SQ :
	    templen = 12;  // fuer Tag, Length, VR und reserved
	    break;
	default:
	    templen = 8;   // fuer Tag, Length und VR
	    break;
	}
    }
    else
	templen = 8;	       // fuer Tag und Length
    return templen;
}


// ********************************


Uint32 DcmItem::calcElementLength( DcmObject *obj,
				   const E_TransferSyntax xfer,
				   const E_EncodingType enctype )
{
    Bdebug((4, "dcitem:DcmItem::calcElementLength(*obj,xfer=%d,enctype=%d)",
	    xfer, enctype ));

    Uint32 templen = 0L;
    Uint32 sublength = 0L;
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
    if ( sublength == DCM_UndefinedLength)
        templen = DCM_UndefinedLength;
    Edebug(());

    return templen;
}


// ********************************


Uint32 DcmItem::getLength(const E_TransferSyntax xfer, 
			  const E_EncodingType enctype)
{
    Bdebug((4, "dcitem:DcmItem::getLength(xfer=%d,enctype=%d)",
	    xfer, enctype ));

    Uint32 templen = 0L;
    Uint32 sublength = 0L;
    if ( !elementList->empty() )
    {
	DcmObject *dO;
	elementList->seek( ELP_first );
	do {
	    dO = elementList->get();
            sublength = DcmItem::calcElementLength( dO, xfer, enctype );
            if ( sublength==DCM_UndefinedLength )
            {
                templen = DCM_UndefinedLength;
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


E_Condition DcmItem::addGroupLengthElements(const E_TransferSyntax xfer,
                                            const E_EncodingType enctype )
{
    Bdebug((2, "dcitem:DcmItem::addGroupLengthElements(xfer=%d,enctype=%d)",
	    xfer, enctype ));

    E_Condition l_error = EC_Normal;
    if ( !elementList->empty() )
    {
	DcmObject *dO;
        BOOL beginning = TRUE;
        Uint16 lastGrp = 0x0000;
        Uint16 actGrp;
        DcmUnsignedLong *actGLElem = (DcmUnsignedLong*)NULL;
        Uint32 grplen = 0L;
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
                        DcmTag tagUL( actGrp, 0x0000, EVR_UL );
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
                    DcmTag tagUL( actGrp, 0x0000, EVR_UL );
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


E_Condition DcmItem::readTagAndLength(DcmStream & inStream,
				      const E_TransferSyntax xfer,
                                      DcmTag &tag,
				      Uint32 & length,
				      Uint32 & bytesRead)
{
    Bdebug((4, "dcitem:DcmItem::readTagAndLength(newxfer=%d,&tag,*length,"
	    "*item_bytes)", xfer ));

    E_Condition l_error = EC_Normal;
    Uint32 valueLength = 0;
    DcmEVR nxtobj = EVR_UNKNOWN;
    Uint16 groupTag = 0xffff;
    Uint16 elementTag = 0xffff;

    DcmXfer xferSyn(xfer);
    if ((l_error = inStream.Avail(xferSyn.isExplicitVR() ? 6:4)) != EC_Normal)
    {
	Edebug(());
	return l_error;
    }

    const E_ByteOrder byteOrder = xferSyn.getByteOrder();
    if (byteOrder == EBO_unknown)
	return EC_IllegalCall;

    inStream.SetPutbackMark();
    inStream.ReadBytes(&groupTag, 2);
    inStream.ReadBytes(&elementTag, 2);
    this -> swapIfNecessary(gLocalByteOrder, byteOrder, &groupTag, 2, 2);
    this -> swapIfNecessary(gLocalByteOrder, byteOrder, &elementTag, 2, 2);
    // Tag ist gelesen
    bytesRead = 4;
    DcmTag newTag(groupTag, elementTag );
    nxtobj = newTag.getEVR();	    // VR aus Tag bestimmen

    if (xferSyn.isExplicitVR() && 
	nxtobj != EVR_na) 	// Delimitation Items do not have a VR
    {
	char vrstr[3];
	vrstr[2] = '\0';
	inStream.ReadBytes(vrstr, 2);  // 2 Byte Laenge:VR als string
	DcmVR vr(vrstr);	    // class DcmVR
	nxtobj = vr.getEVR();
	newTag.setVR(nxtobj);     // VR in newTag anpassen, falls Element
	// nicht fehlerhaft kodiert ist.
	bytesRead += 2;
    }

    if ((l_error = inStream.Avail(calcHeaderLength(nxtobj, xfer)-bytesRead))
	!= EC_Normal)
    {
	inStream.Putback();
	bytesRead = 0;
	Edebug(());
	return l_error;
    }
    // The UnsetPutbackMark is in readSubElement
	
    if (xferSyn.isImplicitVR() ||
	nxtobj == EVR_na)	    // DelimitationItems besitzen keine VR!
    {
	inStream.ReadBytes(&valueLength, 4);
	this->swapIfNecessary(gLocalByteOrder, byteOrder, &valueLength, 4, 4);
	bytesRead += 4;
	debug(( 4, "Implicit xfer=%d", xfer ));

    }
    else if (xferSyn.isExplicitVR())
    {
	if (nxtobj == EVR_OB || nxtobj == EVR_OW || nxtobj == EVR_SQ)
	{
	    Uint16 reserved;
	    inStream.ReadBytes(&reserved, 2);  // 2 Byte Laenge
	    inStream.ReadBytes(&valueLength, 4);
	    this -> swapIfNecessary(gLocalByteOrder, byteOrder, 
				    &valueLength, 4, 4);
	    bytesRead += 6;
	}
	else
	{
	    Uint16 tmpValueLength;
	    inStream.ReadBytes(&tmpValueLength, 2);
	    this -> swapIfNecessary(gLocalByteOrder, byteOrder,  
				    &tmpValueLength, 2, 2);
	    bytesRead += 2;
	    valueLength = tmpValueLength;
	}

	debug(( 4, "Explicit xfer=%d", xfer ));

    } // else if ( xfer = EXS..Explicit ...
    else
    {
	l_error = EC_IllegalCall;
    }
    debug(( 3, "TagAndLength read of: (0x%4.4x,0x%4.4x) \"%s\" [0x%8.8x] \"%s\"",
	    newTag.getGTag(), newTag.getETag(),
	    newTag.getVRName(), valueLength, newTag.getTagName() ));

    length = valueLength;	 // Rueckgabewert
    tag = newTag;                   // Rueckgabewert
    Vdebug((4, l_error!=EC_Normal, "errorFlag(4,7)=(%d)", l_error ));
    Edebug(());

    return l_error;
}


// ********************************


E_Condition DcmItem::readSubElement(DcmStream & inStream,
				    DcmTag & newTag,
				    const Uint32 newLength,
				    const E_TransferSyntax xfer,
				    const E_GrpLenEncoding gltype,
				    const Uint32 maxReadLength)
{
    Bdebug((4, "dcitem:DcmItem::readSubElement(&newTag,newLength=%ld,xfer=%d,gltype=%d)",
	    newLength, xfer, gltype ));

    DcmElement *subElem = NULL;
    E_Condition l_error = newDicomElement(subElem, newTag, newLength);


    if (l_error == EC_Normal && subElem == NULL && 
	newTag.getXTag() == DCM_PixelData) 
    {
	DcmXfer xferSyn(xfer);
	if (xferSyn.isEncapsulated())
	    subElem = new DcmPixelSequence(newTag, newLength);
	else
	{
	    if ( xferSyn.isImplicitVR() )
		newTag.setVR(EVR_OW);
	    subElem = new DcmOtherByteOtherWord(newTag, newLength);
	}
    }

    if ( l_error == EC_Normal && subElem != (DcmElement*)NULL )
    {
	inStream.UnsetPutbackMark();
	// DcmItem::elementList->insert( subElem, ELP_next );  // etwas schneller
	insert( subElem );				      				   // sicherer
	subElem->transferInit();
	l_error = subElem->read(inStream, xfer, gltype, maxReadLength);
    }
    else if ( l_error == EC_InvalidTag )  	// versuche das Parsing wieder
    {					  					// einzurenken
	// This is the second Putback operation on the putback mark in 
	// readTagAndLength but it is impossible that both can be executed 
	// without setting the Mark twice.
	inStream.Putback();
	cerr << "Warning: DcmItem::readSubElement(): parse error occurred: "
             <<  newTag << endl;
	debug(( 1, "Warning: DcmItem::readSubElement(): parse error occurred:"
		" (0x%4.4hx,0x%4.4hx)\n", newTag.getGTag(), newTag.getETag() ));

    }
    else if ( l_error != EC_ItemEnd )
    {
	// Very important: Unset the putback mark
	inStream.UnsetPutbackMark();
	cerr << "Error: DcmItem::readSubElement(): cannot create SubElement: "
             <<  newTag << endl;
	debug(( 1, "Error: DcmItem::readSubElement(): cannot create SubElement:"
		" (0x%4.4hx,0x%4.4hx)\n", newTag.getGTag(), newTag.getETag() ));

    }
    else
    {
	Vdebug((4, l_error!=EC_Normal, "errorFlag(8)=(%d)", l_error ));
	inStream.UnsetPutbackMark();
    }
    Edebug(());

    return l_error;
}


// ********************************


E_Condition DcmItem::read(DcmStream & inStream,
			  const E_TransferSyntax xfer,
                          const E_GrpLenEncoding gltype,
			  const Uint32 maxReadLength)
{
    Bdebug((3, "DcmItem::read(xfer=%d,gltype=%d)", xfer, gltype ));

    if (fTransferState == ERW_notInitialized)
	errorFlag = EC_IllegalCall;
    else
    {
	errorFlag = inStream.GetError();
	if (errorFlag == EC_Normal && inStream.EndOfStream())
	    errorFlag = EC_EndOfStream;
	else if (errorFlag == EC_Normal && fTransferState != ERW_ready)
	{
	    if (fTransferState == ERW_init )
	    {
		fStartPosition = inStream.Tell();  // Position von Item-Start
		fTransferState = ERW_inWork;
	    }

	    DcmTag newTag;
	    while (inStream.GetError() == EC_Normal &&
		   (fTransferredBytes < Length || !lastElementComplete))
	    {
		Uint32 newValueLength = 0;
		Uint32 bytes_tagAndLen = 0;
		if (lastElementComplete)
		{
		    errorFlag = readTagAndLength(inStream, xfer, newTag,
						 newValueLength, 
						 bytes_tagAndLen );
		    fTransferredBytes += bytes_tagAndLen;
		    if ( errorFlag != EC_Normal )
			break;			// beende while-Schleife

		    lastElementComplete = FALSE;
		    errorFlag = readSubElement(inStream, newTag, newValueLength,
					       xfer, gltype, maxReadLength);

		    if ( errorFlag == EC_Normal )
			lastElementComplete = TRUE;
		}
		else
		{
		    errorFlag = elementList->get()->read(inStream, xfer, 
							 gltype, maxReadLength) ;
		    if ( errorFlag == EC_Normal )
			lastElementComplete = TRUE;
		}
		fTransferredBytes = inStream.Tell() - fStartPosition;

		if ( errorFlag != EC_Normal )
		    break;				// beende while-Schleife

	    } //while 
	    if ((fTransferredBytes < Length || !lastElementComplete) &&
		errorFlag == EC_Normal)
		errorFlag = EC_StreamNotifyClient;
	    if (errorFlag == EC_Normal && inStream.EndOfStream())
		errorFlag = EC_EndOfStream;
	} // else errorFlag
		
	if (errorFlag == EC_ItemEnd || errorFlag == EC_EndOfStream)
	    errorFlag = EC_Normal;
	if ( errorFlag == EC_Normal )
	    fTransferState = ERW_ready;	     // Item ist komplett
    }
    debug(( 3, "errorFlag=(%d)", errorFlag ));
    Edebug(());

    return errorFlag;
} // DcmItem::read()


// ********************************


E_Condition DcmItem::write(DcmStream & outStream,
			   const E_TransferSyntax oxfer,
			   const E_EncodingType enctype,
			   const E_GrpLenEncoding gltype )
{
    Bdebug((3, "DcmItem::writeBlock(&outStream,oxfer=%d,enctype=%d,gltype=%d)",
	    oxfer, enctype, gltype ));

    if (fTransferState == ERW_notInitialized)
	errorFlag = EC_IllegalCall;
    else
    {
	errorFlag = outStream.GetError();
	if (errorFlag == EC_Normal && fTransferState != ERW_ready)
	{
	    if (fTransferState == ERW_init)
	    {
		if ((errorFlag = outStream.Avail(8)) == EC_Normal)
		{
		    if (enctype == EET_ExplicitLength)
			Length = this->getLength(oxfer, enctype);
		    // getLength kann u.U. auch DCM_UndefinedLength liefern
		    else
			Length = DCM_UndefinedLength;

		    errorFlag = this -> writeTag(outStream, *Tag, oxfer);
		    Uint32 valueLength = Length;
		    DcmXfer outXfer(oxfer);
		    const E_ByteOrder oByteOrder = outXfer.getByteOrder();
		    if (oByteOrder == EBO_unknown)
			return EC_IllegalCall;
		    this -> swapIfNecessary(oByteOrder, 
					    gLocalByteOrder, 
					    &valueLength, 4, 4);
		    outStream.WriteBytes(&valueLength, 4); // 4 Byte Laenge
		    fTransferState = ERW_inWork;
		    elementList->seek( ELP_first );
		}
	    }

	    if (fTransferState == ERW_inWork)
	    {
		if (!elementList->empty())
		{
		    DcmObject *dO = NULL;
		    do 
		    {
			dO = elementList->get();
			if (dO->transferState() != ERW_ready) 
			    errorFlag = dO->write(outStream, oxfer, 
						  enctype, gltype);
		    } while (errorFlag == EC_Normal && 
			     elementList->seek(ELP_next));
		}

		if (errorFlag == EC_Normal)
		{
		    fTransferState = ERW_ready;
		    if (Length == DCM_UndefinedLength && 
			(errorFlag = outStream.Avail(8)) == EC_Normal)
			// schreibe ItemDelimitationItem
		    {
			DcmTag delim( DCM_ItemDelimitationItem );
			errorFlag = this -> writeTag(outStream, delim, oxfer);
			Uint32 delimLen = 0L;
			outStream.WriteBytes(&delimLen, 4); // 4 Byte Laenge
		    }
		    else if (errorFlag != EC_Normal)
		    {
			// Every subelement of the item is written but it
			// is not possible to write the delimination item into 
			// the buffer. 
			fTransferState = ERW_inWork;
		    }
		}
	    }
	}
    }
    Edebug(());

    return errorFlag;
}


// ********************************


void DcmItem::transferInit(void)
{
    DcmObject::transferInit();
    fStartPosition = 0;
    lastElementComplete = TRUE;
    if ( !elementList->empty() )
    {
	elementList->seek( ELP_first );
	do 
	{
	    elementList->get()->transferInit();

	} while ( elementList->seek( ELP_next ) );
    }
}


// ********************************


void DcmItem::transferEnd(void)
{
    DcmObject::transferEnd();
    if ( !elementList->empty() )
    {
	elementList->seek( ELP_first );
	do 
	{
	    elementList->get()->transferEnd();

	} while ( elementList->seek( ELP_next ) );
    }
}


// ********************************


unsigned long DcmItem::card()
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
		debug(( 3, "element (0x%4.4x,0x%4.4x) / VR=\"%s\" at beginning inserted",
			elem->getGTag(), elem->getETag(), DcmVR(elem->getVR()).getVRName() ));

		break;
	    }
            else if ( elem->getTag() > dE->getTag() )
		// Position gefunden
	    {
		elementList->insert( elem, ELP_next );
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
		    cerr << "Warning: DcmItem::insert(): element "
			 << elem->getTag() << "VR=\"" 
			 << DcmVR(elem->getVR()).getVRName()
			 << " was already in list: not inserted\n";
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


DcmElement* DcmItem::getElement(const unsigned long num)
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

DcmObject * DcmItem::nextInContainer(const DcmObject * obj)
{
    if (!obj)
	return elementList -> get(ELP_first);
    else
    {
	if (elementList -> get() != obj)
	{
	    for(DcmObject * search_obj = elementList -> seek(ELP_first);
		search_obj && search_obj != obj;
		search_obj = elementList -> seek(ELP_next)
		);
	}
	return elementList -> seek(ELP_next);
    }
}

// ********************************

E_Condition DcmItem::nextObject(DcmStack & stack, const BOOL intoSub)
{
    E_Condition l_error = EC_Normal;
    DcmObject * container = NULL;
    DcmObject * obj = NULL;
    DcmObject * result = NULL;
    BOOL examSub = intoSub;

    if (stack.empty())
    {
	stack.push(this);
	examSub = TRUE;
    }

    obj = stack.top();
    if (obj->isLeaf())
    {
	stack.pop();
	container = stack.top();
	result = container -> nextInContainer(obj);
    }
    else if (examSub) 
	result = obj -> nextInContainer(NULL);

    if (result)
	stack.push(result);
    else if (intoSub)
	l_error = nextUp(stack);
    else
	l_error = EC_SequEnd;

    return l_error;
}
    

// ********************************


DcmElement* DcmItem::remove(const unsigned long num)
{
    Bdebug((3, "dcitem:DcmItem::remove(num=%ld)", num ));

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


DcmElement* DcmItem::remove( const DcmTag& tag )
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
            if ( dO->getTag() == tag )
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


E_Condition DcmItem::verify(const BOOL autocorrect )
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


E_Condition DcmItem::searchSubFromHere( const DcmTag &tag,
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
                if ( dO->getTag() == tag )
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
                if ( dO->getTag() == tag )
                {
                    resultStack.push( dO );
                    l_error = EC_Normal;
                }
            }
        } while ( l_error != EC_Normal && elementList->seek( ELP_next ) );
	Vdebug((4, l_error==EC_Normal && dO->getTag()==tag, "Search-Tag=(%4.4x,%4.4x)"
		" \"%s\" found!", tag.getGTag(), tag.getETag(),
		tag.getVR().getVRName() ));

    }
    Edebug(());

    return l_error;
}


// ********************************


E_Condition DcmItem::search( const DcmTag &tag,
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

	    unsigned long i = resultStack.card();
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
				    && dO->getTag() == tag
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


E_Condition DcmItem::search( const DcmTagKey &xtag,
			     DcmStack &resultStack,
			     E_SearchMode mode,
			     BOOL searchIntoSub )
{
    Bdebug((5, "dcitem:DcmItem::search(xtag=(%x,%x),Stack&,mode=%d,sub=%d)",
	    xtag.getGroup(), xtag.getElement(), mode, searchIntoSub ));

    DcmTag tag( xtag );
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


E_Condition DcmItem::loadAllDataIntoMemory(void)
{
    Bdebug((3, "dcitem:DcmItem::loadAllDataIntoMemory()"));

    E_Condition l_error = EC_Normal;
    if (!elementList -> empty())
    {
	elementList -> seek(ELP_first);
	do
	{
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

//
// Support functions

DcmElement * newDicomElement(const DcmTag & tag,
			     const Uint32 length)
{
    DcmElement * newElement = NULL;
    newDicomElement(newElement, tag, length);
    return newElement;
}

// ********************************

E_Condition newDicomElement(DcmElement * & newElement,
			    const DcmTag & tag,
			    const Uint32 length)
{
    E_Condition l_error = EC_Normal;
    newElement = NULL;

    switch (tag.getEVR())
    {
	// Byte-Strings:
    case EVR_AE :
	newElement = new DcmApplicationEntity( tag, length);
	break;
    case EVR_AS :
	newElement = new DcmAgeString( tag, length);
	break;
    case EVR_CS :
	newElement = new DcmCodeString( tag, length);
	break;
    case EVR_DA :
	newElement = new DcmDate( tag, length);
	break;
    case EVR_DS :
	newElement = new DcmDecimalString( tag, length);
	break;
    case EVR_DT :
	newElement = new DcmDateTime( tag, length);
	break;
    case EVR_IS :
	newElement = new DcmIntegerString( tag, length);
	break;
    case EVR_TM :
	newElement = new DcmTime( tag, length);
	break;
    case EVR_UI :
	newElement = new DcmUniqueIdentifier( tag, length);
	break;

	// Charakter-Strings:
    case EVR_LO :
	newElement = new DcmLongString( tag, length);
	break;
    case EVR_LT :
	newElement = new DcmLongText( tag, length);
	break;
    case EVR_PN :
	newElement = new DcmPersonName( tag, length);
	break;
    case EVR_SH :
	newElement = new DcmShortString( tag, length);
	break;
    case EVR_ST :
	newElement = new DcmShortText( tag, length);
	break;

	// abhaengig von ByteOrder:
    case EVR_AT :
	newElement = new DcmAttributeTag( tag, length);
	break;
    case EVR_SS :
	newElement = new DcmSignedShort( tag, length);
	break;
    case EVR_xs : // laut Dicom-Standard V3.0
    case EVR_US :
	newElement = new DcmUnsignedShort( tag, length);
	break;
    case EVR_SL :
	newElement = new DcmSignedLong( tag, length);
	break;
    case EVR_up : // fuer (0004,eeee) laut Dicom-Standard V3.0
    case EVR_UL :
    {
	// generiere Tag mit VR aus Dictionary!
	DcmTag ulupTag(tag.getXTag());
	if ( ulupTag.getEVR() == EVR_up )
	    newElement = new DcmUnsignedLongOffset(ulupTag, length);
	else
	    newElement = new DcmUnsignedLong( tag, length);
    }
    break;
    case EVR_FL:
	newElement = new DcmFloatingPointSingle( tag, length);
	break;
    case EVR_FD :
	newElement = new DcmFloatingPointDouble( tag, length);
	break;

	// Sequences and Items
    case EVR_SQ :
	newElement = new DcmSequenceOfItems( tag, length);
	break;
    case EVR_na :
	if ( tag.getXTag() == DCM_Item )
	    l_error = EC_InvalidTag;
	else if ( tag.getXTag() == DCM_SequenceDelimitationItem )
	    l_error = EC_SequEnd;
	else if ( tag.getXTag() == DCM_ItemDelimitationItem )
	    l_error = EC_ItemEnd;
	else
	    l_error = EC_InvalidTag;
	break;

	// nicht-eindeutig 8- oder 16-Bit:

    case EVR_ox :
	// We cannot handle EVR_ox here because we do not know the
	// transfer syntax
	break;

    case EVR_OB :
    case EVR_OW :
	newElement = new DcmOtherByteOtherWord(tag, length);
	break;

    // Gekapselte Pixeldaten als spezielle Sequenz:
    case EVR_pixelSQ :
	newElement = new DcmPixelSequence( tag, length);
	break;

	// Falls das Parsing nicht so ganz funktionieren sollte:
    case EVR_pixelItem :
	l_error = EC_InvalidTag;
	break;

        // Unbekannte Typen als Byte-String lesen:
    case EVR_UNKNOWN :
    default :
	newElement = new DcmOtherByteOtherWord(tag, length);
	debug((1, 
	       "Warning: newDicomElement(): unknown Tag detected: (%04x,%04x)",
	       tag.getGTag(), tag.getETag()));
	break;
    }

    return l_error;
}


E_Condition nextUp(DcmStack & stack)
{
    DcmObject * oldContainer = stack.pop();
    if (oldContainer -> isLeaf())
	return EC_IllegalCall;
    else if (!stack.empty())
    {
	DcmObject * container = stack.top();
	DcmObject * result = container -> nextInContainer(oldContainer);
	if (result)
	{
	    stack.push(result);
	    return EC_Normal;
	}
	else
	    return nextUp(stack);
    }
    return EC_TagNotFound;
}
    

/* 
** simplified search&get functions 
*/

E_Condition 
DcmItem::findString(const DcmTagKey& xtag,
		    char* aString, int maxStringLength,
		    BOOL searchIntoSub)
{
    DcmElement *elem;
    DcmStack stack;
    E_Condition ec = EC_Normal;
    char* s;
    
    aString[0] = '\0';
    ec = search(xtag, stack, ESM_fromHere, searchIntoSub);
    elem = (DcmElement*) stack.top();
    if (ec == EC_Normal && elem != NULL) {
	if (elem->getLength() != 0) {
            ec = elem->get(s);
	    if (ec == EC_Normal) {
		strncpy(aString, s, maxStringLength);
	    }
        }
    }

    return ec;
}

E_Condition 
DcmItem::findLong(const DcmTagKey& xtag,
		  long& aLong, 
		  BOOL searchIntoSub)
{
    DcmElement *elem;
    DcmStack stack;
    E_Condition ec = EC_Normal;

    ec = search(xtag, stack, ESM_fromHere, searchIntoSub);
    elem = (DcmElement*) stack.top();
    if (ec == EC_Normal && elem != NULL) {
	switch (elem->ident()) {
	case EVR_UL:
	    Uint32 ul;
	    ec = elem->get(ul, 0);
	    aLong = ul;
	    break;
	case EVR_SL:
	    Sint32 sl;
	    ec = elem->get(sl, 0);
	    aLong = sl;
	    break;
	case EVR_US:
	    Uint16 us;
	    ec = elem->get(us, 0);
	    aLong = us;
	    break;
	case EVR_SS:
	    Sint16 ss;
	    ec = elem->get(ss, 0);
	    aLong = ss;
	    break;
	default:
	    ec = EC_IllegalCall;
	    break;
	}
    }

    return ec;
}


// ********************************

/*
** CVS/RCS Log:
** $Log: dcitem.cc,v $
** Revision 1.14  1996-07-31 13:14:30  andreas
** - Minor corrections: error code for swapping to or from byteorder unknown
**                      correct read of dataset in fileformat
**
** Revision 1.13  1996/07/17 12:39:38  andreas
** new nextObject for DcmDataSet, DcmFileFormat, DcmItem, ...
**
** Revision 1.12  1996/04/29 15:08:14  hewett
** Replaced DcmItem::findInt(...) with the more general DcmItem::findLong(...).
**
** Revision 1.11  1996/04/27 14:04:55  hewett
** Eliminated compiler warnings when compiling without -DDEBUG.  Very
** minor corrections, mostly unused parameters and uninitialized variables.
**
** Revision 1.10  1996/04/16 16:04:54  andreas
** - const tag Parameter in newDicomElement
**
** Revision 1.9  1996/03/28 18:52:39  hewett
** Added 2 simple find&get methods (findString & findInt).
**
** Revision 1.8  1996/03/12 15:23:27  hewett
** When generating group length tags, the VR of a tag is now explicity
** set to be EVR_UL.  Group length tags not in the dictionary (e.g. for
** private groups) were getting coded incorrectly.
**
** Revision 1.7  1996/03/11 14:16:00  hewett
** Corrected error whereby explicit encoding was being recognised as implicit.
**
** Revision 1.6  1996/03/11 13:03:51  hewett
** Rearranged logic of DcmItem::checkTransferSyntax to make little-endian
** the default if both big and little endian are possible.
**
** Revision 1.5  1996/01/29 13:38:27  andreas
** - new put method for every VR to put value as a string
** - better and unique print methods
**
** Revision 1.4  1996/01/09 11:06:46  andreas
** New Support for Visual C++
** Correct problems with inconsistent const declarations
** Correct error in reading Item Delimitation Elements
**
** Revision 1.3  1996/01/05 13:27:37  andreas
** - changed to support new streaming facilities
** - unique read/write methods for file and block transfer
** - more cleanups
**
**
*/
