/*
**
** Author: Gerd Ehlers	    01.11.94 -- Created
**         Andreas Barth    04.12.95 -- New Stream classes
** Kuratorium OFFIS e.V.
**
** Module: dcpixseq.cc
**
** Purpose:
** Implementation of class DcmPixelSequence
**
**
** Last Update:		$Author: andreas $
** Update Date:		$Date: 1996-01-05 13:27:40 $
** Source File:		$Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/libsrc/dcpixseq.cc,v $
** CVS/RCS Revision:	$Revision: 1.3 $
** Status:		$State: Exp $
**
** CVS/RCS Log at end of file
**
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


DcmPixelSequence::DcmPixelSequence(const DcmTag &tag,
								   const Uint32 len)
: DcmSequenceOfItems(tag, len)
{
Bdebug((5, "dcpixseq:DcmPixelSequence::DcmPixelSequence(DcmTag&,len=%ld)",
           len ));
debug(( 8, "Object pointer this=0x%p", this ));

    Tag->setVR( EVR_OB );
Edebug(());

}


// ********************************


DcmPixelSequence::DcmPixelSequence(const DcmPixelSequence &old)
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


void DcmPixelSequence::print( int level )
{
    Tag->setVR( EVR_OB );
    char *info = new char[200];
    char *title = (char*)NULL;
    if ( Length == DCM_UndefinedLength)
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

    if ( Length == DCM_UndefinedLength )
        DcmObject::printInfoLine( level, delimItemTag,
				  0, "\"SequenceDelimitationItem\"" );
    else
        DcmObject::printInfoLine( level, delimItemTag,
		   0, "\"SequenceDelimitationItem for re-enc.\"" );
}


// ********************************


E_Condition DcmPixelSequence::makeSubObject(DcmObject * & subObject,
										  const DcmTag & newTag,
										  const Uint32 newLength)
{
    E_Condition l_error = EC_Normal;
    DcmObject * newObject = NULL;

    switch ( newTag.getEVR() )
    {
	  case EVR_na:
		if ( newTag.getXTag() == DCM_Item )
			newObject = new DcmPixelItem(newTag, newLength);
		else if (newTag.getXTag() == DCM_SequenceDelimitationItem)
			l_error = EC_SequEnd;
		else if (newTag.getXTag() == DCM_ItemDelimitationItem)
			l_error = EC_InvalidTag;
	    else
			l_error = EC_InvalidTag;
		break;

	  default:
		newObject = new DcmPixelItem(newTag, newLength);
		l_error = EC_CorruptedData;
	    break;
    }

	subObject = newObject;
	return l_error;
}

// ********************************

/*
** CVS/RCS Log:
** $Log: dcpixseq.cc,v $
** Revision 1.3  1996-01-05 13:27:40  andreas
** - changed to support new streaming facilities
** - unique read/write methods for file and block transfer
** - more cleanups
**
**
*/
