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
 * Revision:      $Revision: 1.2 $
 * Status:	  $State: Exp $
 *
 */


#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include <stdlib.h>
#include <stdio.h>
#include <iostream.h>

#include "dcpixseq.h"
#include "dcpxitem.h"
#include "dcitem.h"
#include "dcvr.h"
#include "dcdebug.h"

#include "dcdeftag.h"


// ********************************


DcmPixelSequence::DcmPixelSequence( const DcmTag &tag,
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


DcmPixelSequence::DcmPixelSequence( const DcmPixelSequence &old )
    : DcmSequenceOfItems( old )
{
Bdebug((5, "dcpixseq:DcmPixelSequence::DcmPixelSequence(DcmPixelSequence&)" ));
debug(( 8, "Object pointer this=0x%p", this ));

debug(( 5, "ident()=%d", old.ident() ));

    /* everything gets handled in DcmSequenceOfItems constructor */
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


DcmEVR DcmPixelSequence::ident() const
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

    sprintf( info, "%s #=%ld ", title, (long)card() );
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
    DcmTag delimItemTag( DCM_SequenceDelimitationItem );

    if ( Length == UNDEF_LEN )
        DcmObject::printInfoLine( level, delimItemTag,
				  0, "\"SequenceDelimitationItem\"" );
    else
        DcmObject::printInfoLine( level, delimItemTag,
		   0, "\"SequenceDelimitationItem for re-enc.\"" );
}


// ********************************


E_Condition DcmPixelSequence::readSubItem( const DcmTag &newTag,       // in
                                           T_VR_UL newLength,          // in
                                           E_TransferSyntax xfer,      // in
                                           E_GrpLenEncoding gltype )   // in
{
Bdebug((4, "dcpixseq:DcmPixelSequence::readSubItem(&newTag,newLength=%d,"
           "xfer=%d,gltype=%d)", newLength, xfer, gltype ));

    E_Condition l_error = EC_Normal;
    DcmItem *subItem = (DcmItem*)NULL;

    switch ( newTag.getEVR() )
    {
	case EVR_na :
            if ( newTag.getXTag() == DCM_Item )
                subItem = new DcmPixelItem( newTag, newLength, iDS );
            else if ( newTag.getXTag() == DCM_SequenceDelimitationItem )
                l_error = EC_SequEnd;
            else if ( newTag.getXTag() == DCM_ItemDelimitationItem )
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

