/*
 *
 * Author: Gerd Ehlers      Created:  11-01-94
 *                          Modified: 02-07-95
 *
 * Module: dcpixseq.cc
 *
 * Purpose:
 * Implementation of the class DcmPixelSequence
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

#include "dcpixseq.h"
#include "dcpxitem.h"
#include "dcitem.h"
#include "dcvr.h"
#include "dcdebug.h"



// ********************************


DcmPixelSequence::DcmPixelSequence( DcmTag &tag )
    : DcmSequenceOfItems( tag )
{
Bdebug((5, "dcpixseq:DcmPixelSequence::DcmPixelSequence(DcmTag&)" ));
debug(( 8, "Object pointer this=0x%p", this ));

    Tag->setVR( EVR_OB );
Edebug(());

}


// ********************************


DcmPixelSequence::DcmPixelSequence( DcmTag &tag,
                                    T_VR_UL len,
                                    iDicomStream *iDStream )
    : DcmSequenceOfItems( tag, len, iDStream )
{
Bdebug((5, "dcpixseq:DcmPixelSequence::DcmPixelSequence(DcmTag&,len=%ld,*iDS)",
           len ));
debug(( 8, "Object pointer this=0x%p", this ));

    Tag->setVR( EVR_OB );
Edebug(());

}


// ********************************


DcmPixelSequence::DcmPixelSequence( const DcmObject &oldObj )
    : DcmSequenceOfItems( InternalUseTag )
{
Bdebug((5, "dcpixseq:DcmPixelSequence::DcmPixelSequence(DcmObject&)" ));
debug(( 8, "Object pointer this=0x%p", this ));

    lastItemComplete = TRUE;
    itemList = new DcmList;
debug(( 5, "ident()=%d", oldObj.ident() ));

    if ( oldObj.ident() == EVR_pixelSQ )
    {
        DcmPixelSequence const *old = (DcmPixelSequence const *)&oldObj;
	*Tag = *old->Tag;
	iDS = old->iDS;
	offsetInFile  = old->offsetInFile;
	valueInMemory = old->valueInMemory;
	valueModified = old->valueModified;
	Length = old->Length;
	Xfer = old->Xfer;
        if ( !old->itemList->empty() )
	{
	    DcmObject *oldDO;
	    DcmObject *newDO;
	    itemList->seek( ELP_first );
            old->itemList->seek( ELP_first );
	    do {
                oldDO = old->itemList->get();
                if ( oldDO->ident() == EVR_pixelItem )
                    newDO = new DcmPixelItem( *oldDO );
		else
		{
                    newDO = new DcmPixelItem( oldDO->getTag() );
                    cerr << "DcmPixelSequence(): Element("
                         << hex << oldDO->getGTag() << "," << oldDO->getETag()
                         << dec << ") found, which was not a PixelItem"
                         << endl;
		}

		itemList->insert( newDO, ELP_next );
            } while ( old->itemList->seek( ELP_next ) );
	}
    }
    else
    {
        cerr << "Warning: DcmPixelSequence: wrong use of Copy-Constructor"
             << endl;
    }
Edebug(());

}


// ********************************


DcmPixelSequence::DcmPixelSequence( const DcmPixelSequence &oldForm )
    : DcmSequenceOfItems( InternalUseTag )
{
Bdebug((5, "dcpixseq:DcmPixelSequence::DcmPixelSequence(DcmPixelSequence&)" ));
debug(( 8, "Object pointer this=0x%p", this ));

    lastItemComplete = TRUE;
    itemList = new DcmList;
debug(( 5, "ident()=%d", oldForm.ident() ));

    if ( oldForm.ident() == EVR_pixelSQ )
    {
        DcmPixelSequence const *old = &oldForm;
	*Tag = *old->Tag;
	iDS = old->iDS;
	offsetInFile  = old->offsetInFile;
	valueInMemory = old->valueInMemory;
	valueModified = old->valueModified;
	Length = old->Length;
	Xfer = old->Xfer;
        if ( !old->itemList->empty() )
	{
	    DcmObject *oldDO;
	    DcmObject *newDO;
	    itemList->seek( ELP_first );
            old->itemList->seek( ELP_first );
	    do {
                oldDO = old->itemList->get();
                if ( oldDO->ident() == EVR_pixelItem )
                    newDO = new DcmPixelItem( *oldDO );
		else
		{
                    newDO = new DcmPixelItem( oldDO->getTag() );
                    cerr << "DcmPixelSequence(): Element("
                         << hex << oldDO->getGTag() << "," << oldDO->getETag()
                         << dec << ") found, which was not a PixelItem"
                         << endl;
		}

		itemList->insert( newDO, ELP_next );
            } while ( old->itemList->seek( ELP_next ) );
	}
    }
    else
    {
        cerr << "Warning: DcmPixelSequence: wrong use of Copy-Constructor"
             << endl;
    }
Edebug(());

}


// ********************************


DcmPixelSequence::~DcmPixelSequence()
{
Bdebug((5, "dcpixseq:DcmPixelSequence::~DcmPixelSequence()" ));
debug(( 8, "Object pointer this=0x%p", this ));
Edebug(());

}


// ********************************


EVR DcmPixelSequence::ident() const
{
    return EVR_pixelSQ;
}


// ********************************


void DcmPixelSequence::print( int level )
{
    Tag->setVR( EVR_OB );
    char *info = new char[200];
    char *title = (char*)NULL;
    if ( Length == UNDEF_LEN )
        title = "\"PixelSequence\"";
    else
        title = "\"PixelSequence with explicit Length\"";

    sprintf( info, "%s #=%ld ", title, card() );
    DcmObject::printInfoLine( level, info );
    delete info;

    if ( !itemList->empty() )
    {
	DcmObject *dO;
	itemList->seek( ELP_first );
	do {
	    dO = itemList->get();
	    dO->print( level + 1 );
	} while ( itemList->seek( ELP_next ) );
    }
    DcmTag delimItemTag( ET_SequenceDelimitationItem );

    if ( Length == UNDEF_LEN )
        DcmObject::printInfoLine( level, delimItemTag,
				  0, "\"SequenceDelimitationItem\"" );
    else
        DcmObject::printInfoLine( level, delimItemTag,
		   0, "\"SequenceDelimitationItem for re-enc.\"" );
}


// ********************************


E_Condition DcmPixelSequence::readSubItem( DcmTag &newTag,             // in
                                           T_VR_UL newLength,          // in
                                           E_TransferSyntax xfer,      // in
                                           E_GrpLenEncoding gltype )   // in
{
Bdebug((4, "dcpixseq:DcmPixelSequence::readSubItem(&newTag,newLength=%d,"
           "xfer=%d,gltype=%d)", newLength, xfer, gltype ));

    E_Condition l_error = EC_Normal;
    DcmItem *subItem = (DcmItem*)NULL;

    switch ( newTag.getVR() )
    {
	case EVR_na :
            if ( newTag.getXTag() == ET_Item )
                subItem = new DcmPixelItem( newTag, newLength, iDS );
            else if ( newTag.getXTag() == ET_SequenceDelimitationItem )
                l_error = EC_SequEnd;
            else if ( newTag.getXTag() == ET_ItemDelimitationItem )
                l_error = EC_InvalidTag;
	    else
                l_error = EC_InvalidTag;
		break;
	default :
            subItem = new DcmPixelItem( newTag, newLength, iDS );
            l_error = EC_CorruptedData;
	    break;
    }

    if ( l_error == EC_Normal && subItem != (DcmItem*)NULL )
    {
	DcmSequenceOfItems::itemList->insert( subItem, ELP_next );
	if ( !iDS->fromFile() ) 		// && !lastItemComplete )
            l_error = subItem->readBlock( xfer, gltype );
	else
            l_error = subItem->read( xfer, gltype );
                                                // hier wird Subitem eingelesen
    }
    else if ( l_error == EC_InvalidTag )  // versuche das Parsing wieder
    {                                     // einzurenken
        for ( T_VR_UL i=0; i<8; i++ )     // Tag and Length sind immer 8 Bytes
            iDS->unget();                 // schreibe Tag wieder in den Stream
        fprintf(stderr,
                "Warning: DcmPixelSequence::readSubItem(): parse error occured:"
                " (0x%4.4hx,0x%4.4hx)", newTag.getGTag(), newTag.getETag() );
debug(( 1, "Warning: DcmPixelSequence::readSubItem(): parse error occured:"
           " (0x%4.4hx,0x%4.4hx)", newTag.getGTag(), newTag.getETag() ));

    }
    else if ( l_error != EC_SequEnd )
    {
        fprintf(stderr, 
                "Error: DcmPixelSequence::readSubItem(): cannot create SubItem"
                " (0x%4.4hx,0x%4.4hx)", newTag.getGTag(), newTag.getETag() );
debug(( 1, "Error: DcmPixelSequence::readSubItem(): cannot create SubItem"
           " (0x%4.4hx,0x%4.4hx)", newTag.getGTag(), newTag.getETag() ));

    }
    Tag->setVR( EVR_OB );

Vdebug((4, l_error!=EC_Normal, "errorFlag(8)=(%d)", l_error ));
Edebug(());

    return l_error;
}


// ********************************

