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
** Update Date:		$Date: 1997-06-06 09:55:30 $
** Source File:		$Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/libsrc/dcpixseq.cc,v $
** CVS/RCS Revision:	$Revision: 1.9 $
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

    Tag->setVR(EVR_pixelSQ);
    xfer = EXS_Unknown;
Edebug(());

}


// ********************************


DcmPixelSequence::DcmPixelSequence(const DcmPixelSequence &old)
: DcmSequenceOfItems( old )
{
    xfer = old.xfer;

    /* everything gets handled in DcmSequenceOfItems constructor */
}


// ********************************


DcmPixelSequence::~DcmPixelSequence()
{
Bdebug((5, "dcpixseq:DcmPixelSequence::~DcmPixelSequence()" ));
debug(( 8, "Object pointer this=0x%p", this ));
Edebug(());

}


void DcmPixelSequence::print(ostream & out, const BOOL showFullData,
			     const int level )
{
    Tag->setVR( EVR_OB );
    char *info = new char[200];
    char *title = (char*)NULL;
    if ( Length == DCM_UndefinedLength)
        title = "PixelSequence";
    else
        title = "PixelSequence with explicit Length";

    sprintf( info, "(%s #=%ld)", title, (long)card() );
    printInfoLine(out, showFullData, level, info );
    delete info;

    if ( !itemList->empty() )
    {
	DcmObject *dO;
	itemList->seek( ELP_first );
	do {
	    dO = itemList->get();
	    dO->print(out, showFullData, level + 1);
	} while ( itemList->seek( ELP_next ) );
    }
    DcmTag delimItemTag( DCM_SequenceDelimitationItem );

    if ( Length == DCM_UndefinedLength )
        printInfoLine(out, showFullData, level, delimItemTag,
				 0, "(SequenceDelimitationItem)");
    else
        printInfoLine(out, showFullData, level, delimItemTag,
		   0, "(SequenceDelimitationItem for re-enc.)" );
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
	    l_error = EC_ItemEnd;
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


E_Condition DcmPixelSequence::insert(DcmPixelItem* item,
				       unsigned long where)
{
    Bdebug((3, "dcsequen:DcmSequenceOfItems::insert(DcmItem*=%p,where=%ld)",
	    item, where ));

    errorFlag = EC_Normal;
    if ( item != NULL )
    {
	itemList->seek_to( where );
	itemList->insert( item );
	Vdebug((3, where< itemList->card(), "item at position %d inserted", where ));
	Vdebug((3, where>=itemList->card(), "item at last position inserted" ));

    }
    else
	errorFlag = EC_IllegalCall;
    Edebug(());

    return errorFlag;
}


// ********************************


E_Condition DcmPixelSequence::getItem(DcmPixelItem * & item, 
					const unsigned long num)
{
    errorFlag = EC_Normal;
    item = (DcmPixelItem*)( itemList->seek_to(num) );  // liest Item aus Liste
    if ( item == NULL )
	errorFlag = EC_IllegalCall;
    return errorFlag;
}


// ********************************


E_Condition DcmPixelSequence::remove(DcmPixelItem * & item, 
				     const unsigned long num)
{
    Bdebug((3, "dcsequen:DcmSequenceOfItems::remove(num=%ld)", num ));

    errorFlag = EC_Normal;
    item = (DcmPixelItem*)( itemList->seek_to(num) );  // liest Item aus Liste
    if ( item != (DcmPixelItem*)NULL )
    {
	debug(( 3, "item p=%p removed, but not deleted", item ));

	itemList->remove();
    }
    else
	errorFlag = EC_IllegalCall;
    Edebug(());

    return errorFlag;
}


// ********************************


E_Condition DcmPixelSequence::remove(DcmPixelItem* item)
{
    Bdebug((3, "dcsequen:DcmSequenceOfItems::remove(DcmItem*)" ));

    errorFlag = EC_IllegalCall;
    if ( !itemList->empty() && item != NULL )
    {
	DcmObject *dO;
	itemList->seek( ELP_first );
	do {
	    dO = itemList->get();
	    if ( dO == item )
	    {
		debug(( 3, "item p=%p removed, but not deleted", item ));

		itemList->remove();	    // entfernt Element aus Liste,
		// aber loescht es nicht
		errorFlag = EC_Normal;
		break;
	    }
	} while ( itemList->seek( ELP_next ) );
    }

    Edebug(());

    return errorFlag;
}

// ********************************

E_Condition DcmPixelSequence::changeXfer(const E_TransferSyntax newXfer)
{
    if (xfer == EXS_Unknown || canWriteXfer(newXfer, xfer))
    {
	xfer = newXfer;
	return EC_Normal;
    }
    else
	return EC_IllegalCall;
}
	

// ********************************

BOOL DcmPixelSequence::canWriteXfer(const E_TransferSyntax newXfer,
				    const E_TransferSyntax oldXfer)
{
    DcmXfer newXferSyn(newXfer);

    return newXferSyn.isEncapsulated() && 
	newXfer == oldXfer && oldXfer == xfer;
}

// ********************************

E_Condition DcmPixelSequence::read(DcmStream & inStream,
				   const E_TransferSyntax ixfer,
				   const E_GrpLenEncoding glenc,
				   const Uint32 maxReadLength)
{
    E_Condition l_error = changeXfer(ixfer);
    if (l_error == EC_Normal)
	return DcmSequenceOfItems::read(inStream, ixfer, glenc, maxReadLength);
    else
	return l_error;
}

// ********************************

E_Condition DcmPixelSequence::write(DcmStream & outStream,
				      const E_TransferSyntax oxfer,
				      const E_EncodingType /*enctype*/)
{
    E_Condition l_error = changeXfer(oxfer);
    if (l_error == EC_Normal)
	return DcmSequenceOfItems::write(outStream, oxfer, EET_UndefinedLength);
    else
	return l_error;
}


/*
** CVS/RCS Log:
** $Log: dcpixseq.cc,v $
** Revision 1.9  1997-06-06 09:55:30  andreas
** - corrected error: canWriteXfer returns false if the old transfer syntax
**   was unknown, which causes several applications to prohibit the writing
**   of dataset.
**
** Revision 1.8  1997/05/27 13:49:01  andreas
** - Add method canWriteXfer to class DcmObject and all derived classes.
**   This method checks whether it is possible to convert the original
**   transfer syntax to an new transfer syntax. The check is used in the
**   dcmconv utility to prohibit the change of a compressed transfer
**   syntax to a uncompressed.
**
** Revision 1.7  1997/05/26 15:08:15  andreas
** - Corrected typo in write method
**
** Revision 1.6  1997/05/22 16:57:15  andreas
** - Corrected errors for writing of pixel sequences for encapsulated
**   transfer syntaxes.
**
** Revision 1.5  1996/08/05 08:46:15  andreas
** new print routine with additional parameters:
**         - print into files
**         - fix output length for elements
** corrected error in search routine with parameter ESM_fromStackTop
**
** Revision 1.4  1996/01/29 13:38:29  andreas
** - new put method for every VR to put value as a string
** - better and unique print methods
**
** Revision 1.3  1996/01/05 13:27:40  andreas
** - changed to support new streaming facilities
** - unique read/write methods for file and block transfer
** - more cleanups
**
**
*/
