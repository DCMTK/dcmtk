/*
**
** Author: Gerd Ehlers      26.04.94 -- Creation
**         Andreas Barth    30.11.95 -- Support for new stream classes
** Kuratorium OFFIS e.V.
**
** Module: dcsequen.cc
**
** Purpose:
** Implementation of the class DcmSequenceOfItems
**
**
** Last Update:		$Author: andreas $
** Update Date:		$Date: 1996-08-08 10:15:10 $
** Source File:		$Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/libsrc/dcsequen.cc,v $
** CVS/RCS Revision:	$Revision: 1.11 $
** Status:		$State: Exp $
**
** CVS/RCS Log at end of file
**
*/


#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include <stdlib.h>
#include <stdio.h>
#include <iostream.h>

#include "dcsequen.h"
#include "dcitem.h"
#include "dcdirrec.h"
#include "dcvr.h"
#include "dcpxitem.h"
#include "dcdebug.h"

#include "dcdeftag.h"


// ********************************


DcmSequenceOfItems::DcmSequenceOfItems(const DcmTag &tag, const Uint32 len)
    : DcmElement(tag, len)
{
    Bdebug((5,"DcmSequenceOfItems::DcmSequenceOfItems(DcmTag&,len=%ld)",
	    len ));
    debug(( 8, "Object pointer this=0x%p", this ));

    itemList = new DcmList;
    lastItemComplete = TRUE;
    fStartPosition = 0;
    Edebug(());
}


// ********************************


DcmSequenceOfItems::DcmSequenceOfItems(const DcmSequenceOfItems& old)
    : DcmElement(old)
{
    Bdebug((5, "dcsequen:DcmSequenceOfItems::DcmSequenceOfItems(DcmSequenceOfItems&)" ));
    debug(( 8, "Object pointer this=0x%p", this ));

    lastItemComplete = TRUE;
    itemList = new DcmList;
    fStartPosition = old.fStartPosition;
    debug(( 3, "ident()=%d", old.ident() ));

    switch ( old.ident() ) {
    case EVR_SQ:
    case EVR_pixelSQ:
    case EVR_fileFormat:
        if ( !old.itemList->empty() ) {
	    DcmObject *oldDO;
	    DcmObject *newDO;
	    itemList->seek( ELP_first );
            old.itemList->seek( ELP_first );
	    do {
                oldDO = old.itemList->get();
		switch ( oldDO->ident() ) {
		case EVR_item:
		    newDO = new DcmItem( *(DcmItem*)oldDO );
		    break;
		case EVR_pixelItem:
		    newDO = new DcmPixelItem( *(DcmPixelItem*)oldDO );
		    break;
		case EVR_metainfo:
		    newDO = new DcmMetaInfo( *(DcmMetaInfo*)oldDO );
		    break;
		case EVR_dataset:
		    newDO = new DcmDataset( *(DcmDataset*)oldDO );
		    break;
		default:
		    newDO = new DcmItem( oldDO->getTag() );
                    cerr << "Error: DcmSequenceOfItems(): Element("
                         << hex << oldDO->getGTag() << "," << oldDO->getETag()
                         << dec << ") found, which was not an Item" << endl;
		    break;
		}

		itemList->insert( newDO, ELP_next );
            } while ( old.itemList->seek( ELP_next ) );
	}
	break;
    default:
        cerr << "Warning: DcmSequenceOfItems: wrong use of Copy-Constructor"
             << endl;
	break;
    }
    Edebug(());

}


// ********************************


DcmSequenceOfItems::~DcmSequenceOfItems()
{
    Bdebug((5, "dcsequen:DcmSequenceOfItems::~DcmSequenceOfItems()"));
    debug(( 8, "Object pointer this=0x%p", this ));

    DcmObject *dO;
    itemList->seek( ELP_first );
    while ( !itemList->empty() )
    {
	dO = itemList->remove();
	if ( dO != (DcmObject*)NULL )
	    delete dO;
    }
    delete itemList;

    Edebug(());
}


// ********************************


void DcmSequenceOfItems::print(ostream & out, const BOOL showFullData,
			       const int level)
{
    char info[200]; 
    char *title = (char*)NULL;
    if ( Length == DCM_UndefinedLength )
        title = "Sequence with undefined length";
    else
        title = "Sequence with explicit Length";

    sprintf( info, "(%s #=%ld)", title, (long)card() );
    printInfoLine(out, showFullData, level, info );

    if ( !itemList->empty() )
    {
	DcmObject *dO;
	itemList->seek( ELP_first );
	do {
	    dO = itemList->get();
	    dO->print(out, showFullData, level + 1 );
	} while ( itemList->seek( ELP_next ) );
    }
    DcmTag delimItemTag( DCM_SequenceDelimitationItem );

    if ( Length == DCM_UndefinedLength )
        printInfoLine(out, showFullData, level, delimItemTag,
				  0, "(SequenceDelimitationItem)" );
    else
        printInfoLine(out, showFullData, level, delimItemTag,
				  0, "(SequenceDelimitationItem for re-enc.)" );
}



// ********************************


Uint32 DcmSequenceOfItems::getLength(const E_TransferSyntax xfer,
				     const E_EncodingType enctype )
{
    Bdebug((4, "dcsequen:DcmSequenceOfItems::getLength(xfer=%d,enctype=%d)",
	    xfer, enctype ));

    Uint32 templen = 0;
    if ( !itemList->empty() )
    {
	DcmItem *dI;
	itemList->seek( ELP_first );
	do {
	    dI = (DcmItem*)( itemList->get() );
	    Uint32 sublength = dI->getLength( xfer, enctype );
	    Vdebug((1, sublength==DCM_UndefinedLength, "Warning subitem has undefined Length" ));

	    templen += sublength;
	    templen += 8;		// fuer Tag und Length

	    if ( enctype == EET_UndefinedLength )
                templen += 8;           // fuer ItemDelimitationItem
	} while ( itemList->seek( ELP_next ) );
    }
    else
	templen = 0;
    debug(( 4, "Length of Sequence=%ld", templen ));
    Edebug(());

    return templen;
}


// ********************************


E_Condition DcmSequenceOfItems::addGroupLengthElements( E_TransferSyntax xfer,
                                                        E_EncodingType enctype )
{
    Bdebug((2, "dcitem:DcmSequenceOfItems::addGroupLengthElements(xfer=%d,enctype=%d)",
	    xfer, enctype ));

    E_Condition l_error = EC_Normal;

    if ( !itemList->empty() )
    {
	itemList->seek( ELP_first );
	do {
            DcmItem *dO = (DcmItem*)itemList->get();
            E_Condition err = dO->addGroupLengthElements( xfer, enctype );
            if ( err != EC_Normal )
                l_error = err;
	} while ( itemList->seek( ELP_next ) );
    }
    Edebug(());

    return l_error;
}


// ********************************


E_Condition DcmSequenceOfItems::makeSubObject(DcmObject * & subObject, 
					      const DcmTag & newTag,
					      const Uint32 newLength)
{
    E_Condition l_error = EC_Normal;
    DcmItem *subItem = NULL;

    switch ( newTag.getEVR() )
    {
    case EVR_na:
	if ( newTag.getXTag() == DCM_Item )
	{
	    if ( this->getTag().getXTag() == DCM_DirectoryRecordSequence )
		subItem = new DcmDirectoryRecord(newTag, newLength);
	    else
		subItem = new DcmItem(newTag, newLength);
	}
	else if ( newTag.getXTag() == DCM_SequenceDelimitationItem )
	    l_error = EC_SequEnd;
	else if ( newTag.getXTag() == DCM_ItemDelimitationItem )
	    l_error = EC_ItemEnd;
	else
	    l_error = EC_InvalidTag;
	break;

    default:
	subItem = new DcmItem(newTag, newLength);
	l_error = EC_CorruptedData;
	break;
    }
    subObject = subItem;
    return l_error;
}

// ********************************


E_Condition DcmSequenceOfItems::removeGroupLengthElements()
{
    Bdebug((2, "dcitem:DcmSequenceOfItems::removeGroupLengthElements()" ));

    E_Condition l_error = EC_Normal;

    if ( !itemList->empty() )
    {
        itemList->seek( ELP_first );
	do {
            DcmItem *dO = (DcmItem*)itemList->get();
            E_Condition err = dO->removeGroupLengthElements();
            if ( err != EC_Normal )
                l_error = err;
        } while ( itemList->seek( ELP_next ) );
    }

    Edebug(());

    return l_error;
}


// ********************************


E_Condition DcmSequenceOfItems::readTagAndLength(DcmStream & inStream,
						 const E_TransferSyntax xfer,
                                                 DcmTag &tag,
                                                 Uint32 & length)
{
    Bdebug((4, "dcsequen:DcmSequenceOfItems::readTagAndLength(xfer=%d,&tag,"
	    "length)", xfer ));

    Uint16 groupTag = 0xffff;
    Uint16 elementTag = 0xffff;

    E_Condition l_error = inStream.Avail(8);
    if (l_error == EC_Normal)
    {
	DcmXfer iXfer(xfer);
	const E_ByteOrder iByteOrder = iXfer.getByteOrder();
	if (iByteOrder == EBO_unknown)
	    return EC_IllegalCall;
	inStream.SetPutbackMark();
	inStream.ReadBytes(&groupTag, 2);
	inStream.ReadBytes(&elementTag, 2);
	this -> swapIfNecessary(gLocalByteOrder, iByteOrder, &groupTag, 2, 2);
	this -> swapIfNecessary(gLocalByteOrder, iByteOrder, &elementTag, 2, 2);
	// Tag ist gelesen

	DcmTag newTag(groupTag, elementTag);

	Uint32 valueLength = 0;
	inStream.ReadBytes(&valueLength, 4);
	this->swapIfNecessary(gLocalByteOrder, iByteOrder, &valueLength, 4, 4);
	length = valueLength;

#ifndef NO_ANON_CLASS_COMP
	debug((4, "Tag (0x%4.4x,0x%4.4x) \"%s\" [0x%8.8x] \"%s\"",
	       newTag.getGTag(), newTag.getETag(),
	       DcmVR(newTag.getVR()).getVRName(), valueLength, 
	       newTag.getTagName()));
#endif	       

	tag = newTag;                  // Rueckgabewert: assignment-operator
    }

    debug(( 4, "errorFlag=(%d) in Sequ.readTag..", l_error ));
    Edebug(());

    return l_error;
}


// ********************************


E_Condition DcmSequenceOfItems::readSubItem(DcmStream & inStream,
					    const DcmTag &newTag,
					    const Uint32 newLength,
					    const E_TransferSyntax xfer,
					    const E_GrpLenEncoding gltype,
					    const Uint32 maxReadLength)
{
    Bdebug((4, "dcsequen:DcmSequenceOfItems::readSubItem(&newTag,newLength=%d,"
	    "xfer=%d,gltype=%d)", newLength, xfer, gltype ));

    // For DcmSequenceOfItems, subObject is always inherited from DcmItem
    // For DcmPixelSequence, subObject is always inherited from DcmPixelItem
    DcmObject * subObject = NULL;
    E_Condition l_error = this -> makeSubObject(subObject, newTag, newLength);
    if ( l_error == EC_Normal && subObject != NULL )
    {
	inStream.UnsetPutbackMark();
	itemList->insert(subObject, ELP_next);
	// hier wird Subitem eingelesen
        l_error = subObject->read(inStream, xfer, gltype, maxReadLength);
    }
    else if ( l_error == EC_InvalidTag )  // versuche das Parsing wieder
    {                                     // einzurenken
	inStream.Putback();
	cerr << "Warning: DcmSequenceOfItems::readSubItem(): parse error "
	    "occured: " << newTag << endl;
	debug(( 1, "Warning: DcmSequenceOfItems::readSubItem(): parse error occured:"
		" (0x%4.4hx,0x%4.4hx)", newTag.getGTag(), newTag.getETag() ));

    }
    else if ( l_error != EC_SequEnd )
    {
	inStream.UnsetPutbackMark();
        cerr << "Error: DcmSequenceOfItems::readSubItem(): cannot create "
	     << "SubItem " << newTag << endl;
	debug(( 1, "Error: DcmSequenceOfItems::readSubItem(): cannot create SubItem"
		" (0x%4.4hx,0x%4.4hx)", newTag.getGTag(), newTag.getETag() ));

    }
    else
	inStream.UnsetPutbackMark();

    Vdebug((4, l_error!=EC_Normal, "errorFlag(8)=(%d)", l_error ));
    Edebug(());

    return l_error;
}


// ********************************


E_Condition DcmSequenceOfItems::read(DcmStream & inStream,
				     const E_TransferSyntax xfer,
                                     const E_GrpLenEncoding gltype,
				     const Uint32 maxReadLength)
{
    Bdebug((3, "DcmSequenceOfItems::read(xfer=%d,gltype=%d)",
	    xfer, gltype ));

    if (fTransferState == ERW_notInitialized)
	errorFlag = EC_IllegalCall;
    else
    {
	errorFlag = inStream.GetError();
	if (errorFlag == EC_Normal && inStream.EndOfStream())
	    errorFlag = EC_EndOfStream;
	else if (errorFlag == EC_Normal && fTransferState != ERW_ready)
	{
	    if (fTransferState == ERW_init)
	    {
		fStartPosition = inStream.Tell();   // Position Sequence-Value
		fTransferState = ERW_inWork;
	    }

	    itemList->seek( ELP_last ); // Die Daten werden ans Ende gehaengt
	    while (!inStream.Fail() && 
		   (fTransferredBytes < Length || !lastItemComplete ))
	    {
		DcmTag newTag;
		Uint32 newValueLength = 0;

		if (lastItemComplete)
		{
		    errorFlag = this -> readTagAndLength(inStream, xfer, 
							 newTag, newValueLength );

		    if ( errorFlag != EC_Normal )
			break;			// beende while-Schleife
		    else
			fTransferredBytes += 8;

		    lastItemComplete = FALSE;
		    errorFlag = readSubItem(inStream, newTag, newValueLength, 
					    xfer, gltype, maxReadLength);
		    if ( errorFlag == EC_Normal )
			lastItemComplete = TRUE;
		}
		else
		{
		    errorFlag = itemList->get()->read(inStream, xfer, 
						      gltype, maxReadLength);
		    if ( errorFlag == EC_Normal )
			lastItemComplete = TRUE;
		}
		fTransferredBytes = inStream.Tell() - fStartPosition;
				
		if ( errorFlag != EC_Normal )
		    break;				// beende while-Schleife

	    } //while 
	    if ((fTransferredBytes < Length || !lastItemComplete) &&
		errorFlag == EC_Normal)
		errorFlag = EC_StreamNotifyClient;
	} // else errorFlag

	if ( errorFlag == EC_SequEnd )
	    errorFlag = EC_Normal;
	if ( errorFlag == EC_Normal )
	    fTransferState = ERW_ready;	     // Sequence ist komplett
    }
    debug(( 3, "errorFlag=(%d)", errorFlag ));
    Edebug(());
	
    return errorFlag;
}


// ********************************



E_Condition DcmSequenceOfItems::write(DcmStream & outStream,
				      const E_TransferSyntax oxfer,
				      const E_EncodingType enctype,
				      const E_GrpLenEncoding gltype)
{
    Bdebug((3, "DcmSequenceOfItems::writeBlock(oxfer=%d,enctype=%d)",
	    oxfer, enctype ));

    if (fTransferState == ERW_notInitialized)
	errorFlag = EC_IllegalCall;
    else
    {
	errorFlag = outStream.GetError();
	if(errorFlag == EC_Normal && fTransferState != ERW_ready)
	{
	    if (fTransferState == ERW_init )
	    {
		if (outStream.Avail() >= DCM_TagInfoLength)  
		    // n.B.: Header ist u. U. kleiner
		{
		    if ( enctype == EET_ExplicitLength )
			// getLength kann u.U. auch DCM_UndefinedLength liefern
			Length = this->getLength(oxfer, enctype);
		    else
			Length = DCM_UndefinedLength;

		    Uint32 written_bytes = 0;
		    errorFlag = this -> writeTagAndLength(outStream,
							  oxfer,
							  written_bytes);
		    if ( errorFlag == EC_Normal )
		    {
			fTransferState = ERW_inWork;
			itemList->seek(ELP_first);
		    }
		}
		else
		    errorFlag = EC_StreamNotifyClient;
	    }
			
	    if (fTransferState == ERW_inWork)
	    {
		if (!itemList->empty())
		{
		    DcmObject *dO;
		    do 
		    {
			dO = itemList->get();
			if (dO->transferState() != ERW_ready)
			    errorFlag = dO->write(outStream, oxfer, 
						  enctype, gltype);
		    } while (errorFlag == EC_Normal && itemList->seek(ELP_next));
		}


		if (errorFlag == EC_Normal)
		{
		    fTransferState = ERW_ready;
		    if (Length == DCM_UndefinedLength && outStream.Avail() >= 8 )
			// schreibe SquenceDelimitationItem
		    {
			DcmTag delim( DCM_SequenceDelimitationItem );
			errorFlag = this -> writeTag(outStream, delim, oxfer);
			Uint32 delimLen = 0L;
			outStream.WriteBytes(&delimLen, 4); // 4 Byte Laenge
		    }
		    else if (outStream.Avail() < 8)
		    {
			// Every subelement of the item was written but it
			// is not possible to write the delimination item 
			// into the buffer. 
			fTransferState = ERW_inWork;
			errorFlag = EC_StreamNotifyClient;
		    }
		}
	    }
	}
    }
    Edebug(());

    return errorFlag;
}


// ********************************


void  DcmSequenceOfItems::transferInit(void)
{
    DcmObject::transferInit();
    fStartPosition = 0;
    lastItemComplete = TRUE;
    if (!itemList->empty() )
    {
	itemList->seek( ELP_first );
	do 
	{
	    itemList->get()->transferInit();

	} while ( itemList->seek( ELP_next ) );
    }
}


// ********************************


void  DcmSequenceOfItems::transferEnd(void)
{
    DcmObject::transferEnd();
    if (!itemList->empty() )
    {
	itemList->seek( ELP_first );
	do 
	{
	    itemList->get()->transferEnd();

	} while ( itemList->seek( ELP_next ) );
    }
}


// ********************************


unsigned long DcmSequenceOfItems::card()
{
    return itemList->card();
}


// ********************************


E_Condition DcmSequenceOfItems::insert(DcmItem* item,
				       unsigned long where)
{
    Bdebug((3, "dcsequen:DcmSequenceOfItems::insert(DcmItem*=%p,where=%ld)",
	    item, where ));

    errorFlag = EC_Normal;
    if ( item != (DcmItem*)NULL )
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


DcmItem* DcmSequenceOfItems::getItem(const unsigned long num)
{
    errorFlag = EC_Normal;
    DcmItem *item;
    item = (DcmItem*)( itemList->seek_to(num) );  // liest Item aus Liste
    if ( item == (DcmItem*)NULL )
	errorFlag = EC_IllegalCall;
    return item;
}


// ********************************

DcmObject * DcmSequenceOfItems::nextInContainer(const DcmObject * obj)
{
    if (!obj)
	return itemList -> get(ELP_first);
    else 
    {
	if (itemList -> get() != obj)
	{
	    for(DcmObject * search_obj = itemList -> seek(ELP_first);
		search_obj && search_obj != obj;
		search_obj = itemList -> seek(ELP_next)
		);
	}
	return itemList -> seek(ELP_next);
    }
}

// ********************************

E_Condition DcmSequenceOfItems::nextObject(DcmStack & stack, const BOOL intoSub)
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
    if (obj->isLeaf() || !intoSub)
    {
	stack.pop();
	if (stack.card > 0)
	{
	    container = stack.top();
	    result = container -> nextInContainer(obj);
	}
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


DcmItem* DcmSequenceOfItems::remove(const unsigned long num)
{
    Bdebug((3, "dcsequen:DcmSequenceOfItems::remove(num=%ld)", num ));

    errorFlag = EC_Normal;
    DcmItem *item;
    item = (DcmItem*)( itemList->seek_to(num) );  // liest Item aus Liste
    if ( item != (DcmItem*)NULL )
    {
	debug(( 3, "item p=%p removed, but not deleted", item ));

	itemList->remove();
    }
    else
	errorFlag = EC_IllegalCall;
    Edebug(());

    return item;
}


// ********************************


DcmItem* DcmSequenceOfItems::remove( DcmItem* item )
{
    Bdebug((3, "dcsequen:DcmSequenceOfItems::remove(DcmItem*)" ));

    DcmItem *retItem = (DcmItem*)NULL;
    errorFlag = EC_IllegalCall;
    if ( !itemList->empty() && item != (DcmItem*)NULL )
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
    if ( errorFlag == EC_IllegalCall )
        retItem = (DcmItem*)NULL;
    else
        retItem = item;
    Edebug(());

    return item;
}


// ********************************


E_Condition DcmSequenceOfItems::clear()
{
    Bdebug((2, "dcsequen:DcmSequenceOfItems::clear()"));

    errorFlag = EC_Normal;
    DcmObject *dO;
    itemList->seek( ELP_first );
    while ( !itemList->empty() )
    {
	dO = itemList->remove();
	if ( dO != (DcmObject*)NULL )
        {
	    delete dO;
            dO = (DcmObject*)NULL;
        }
    }
    Length = 0;
    Edebug(());

    return errorFlag;
}


// ********************************


E_Condition DcmSequenceOfItems::verify(const BOOL autocorrect)
{
    Bdebug((3, "dcsequen:DcmSequenceOfItems::verify(autocorrect=%d)",
	    autocorrect ));
    debug(( 3, "Tag=(0x%4.4x,0x%4.4x) \"%s\" \"%s\"",
	    getGTag(), getETag(),
	    DcmVR(getVR()).getVRName(), Tag->getTagName() ));

    errorFlag = EC_Normal;
    if ( !itemList->empty() )
    {
	DcmObject *dO;
	itemList->seek( ELP_first );
	do {
	    dO = itemList->get();
	    if ( dO->verify( autocorrect ) != EC_Normal )
		errorFlag = EC_CorruptedData;
	} while ( itemList->seek( ELP_next ) );
    }
    if ( autocorrect == TRUE )
	Length = this->getLength();
    Edebug(());

    return errorFlag;
}


// ********************************

    // Vorbedingung: itemList ist nicht leer!
    // Ergebnis:     - return EC_Normal;
    //		       gefundener Element-Zeiger auf resultStack
    //		     - return EC_TagNotFound;
    //		       resultStack unveraendert
    // Weitersuchen: schreibe Zeiger des Sub-Elementes auf resultStack und
    //		     starte dann Sub-Suche


E_Condition DcmSequenceOfItems::searchSubFromHere(const DcmTag &tag,
						   DcmStack &resultStack,
						   const BOOL searchIntoSub)
{
    Bdebug((5, "dcsequen:DcmSequenceOfItems::searchSubFromHere(tag=(%4.4x,%4.4x),"
	    "Stack&(%ld),sub=%d)", tag.getGTag(), tag.getETag(),
	    resultStack.card(), searchIntoSub ));

    DcmObject *dO;
    E_Condition l_error = EC_TagNotFound;
    if ( !itemList->empty() )
    {
        itemList->seek( ELP_first );
        do {
            dO = itemList->get();
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
        } while ( l_error != EC_Normal && itemList->seek( ELP_next ) );
#ifndef NO_ANON_CLASS_COMP
	Vdebug((4, l_error==EC_Normal && tag==dO->getTag(), "Search-Tag=(%4.4x,%4.4x)"
		" \"%s\" found!", tag.getGTag(), tag.getETag(),
		DcmVR(tag.getVR()).getVRName() )); 
#endif

    }
    Edebug(());

    return l_error;
}


// ********************************


E_Condition DcmSequenceOfItems::search( const DcmTag &tag,
					DcmStack &resultStack,
					E_SearchMode mode,
					BOOL searchIntoSub )
{
    Bdebug((5, "dcsequen:DcmSequenceOfItems::search(tag=(%4.4x,%4.4x),Stack&(%ld),"
	    "mode=%d,sub=%d)", tag.getGTag(), tag.getETag(), resultStack.card(),
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
    else if ( !itemList->empty() )
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
// The next two lines destroy the stack, delete them
//                if ( l_error != EC_Normal ) // raeumt nur die oberste Stackebene
//                    resultStack.pop();      // ab; der Rest ist unveraendert
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
		else
		{
		    E_SearchMode submode = mode;
		    BOOL searchNode = TRUE;
		    DcmObject *dnO;
		    dnO = resultStack.elem( i-2 ); // Knoten der naechsten Ebene
		    debug(( 5, "itemList: dnO=%p", dnO ));

		    itemList->seek( ELP_first );
		    do {
			dO = itemList->get();
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
		    } while ( itemList->seek( ELP_next ) );
		}
	    }
	    else			      // wird vermutlich nie erreicht
                l_error = EC_IllegalCall;
	} // ( mode == ESM_afterStackTop
	else
            l_error = EC_IllegalCall;
    }
    Edebug(());

    return l_error;
}


// ********************************


E_Condition DcmSequenceOfItems::search( const DcmTagKey &xtag,
					DcmStack &resultStack,
					E_SearchMode mode,
					BOOL searchIntoSub )
{
    Bdebug((5, "dcsequen:DcmSequenceOfItems::search(xtag=(%x,%x),Stack&,mode=%d,sub=%d)",
	    xtag.getGroup(), xtag.getElement(), mode, searchIntoSub ));

    DcmTag tag( xtag );
    E_Condition l_error = search( tag, resultStack, mode, searchIntoSub );
    Edebug(());

    return l_error;
}


// ********************************


E_Condition DcmSequenceOfItems::searchErrors( DcmStack &resultStack )
{
    Bdebug((5, "dcsequen:DcmSequenceOfItems::searchErrors(Stack&)" ));

    E_Condition l_error = errorFlag;
    DcmObject *dO = (DcmObject*)NULL;
    if ( errorFlag != EC_Normal )
	resultStack.push( this );
    if ( !itemList->empty() )
    {
	itemList->seek( ELP_first );
	do {
	    E_Condition err;
	    dO = itemList->get();
	    if ( (err = dO->searchErrors( resultStack )) != EC_Normal )
                l_error = err;
	} while ( itemList->seek( ELP_next ) );
    }
    Edebug(());

    return l_error;
}


// ********************************


E_Condition DcmSequenceOfItems::loadAllDataIntoMemory()
{
    Bdebug((3, "dcsequen:DcmSequenceOfItems::loadAllDataIntoMemory()"));

    E_Condition l_error = EC_Normal;
    if ( !itemList->empty() )
    {
        itemList->seek( ELP_first );
	do 
	{
	    E_Condition err;
            DcmObject *dO = itemList->get();
            if ( (err = dO->loadAllDataIntoMemory()) != EC_Normal )
		l_error = err;
        } while ( itemList->seek( ELP_next ) );
    }
    Edebug(());

    return l_error;
}


// ********************************



/*
** CVS/RCS Log:
** $Log: dcsequen.cc,v $
** Revision 1.11  1996-08-08 10:15:10  andreas
** Some more testing in nextObject
**
** Revision 1.10  1996/08/08 10:06:24  andreas
** Correct error for intoSub=FALSE
**
** Revision 1.9  1996/08/05 08:46:17  andreas
** new print routine with additional parameters:
**         - print into files
**         - fix output length for elements
** corrected error in search routine with parameter ESM_fromStackTop
**
** Revision 1.8  1996/07/31 13:14:32  andreas
** - Minor corrections: error code for swapping to or from byteorder unknown
**                      correct read of dataset in fileformat
**
** Revision 1.7  1996/07/17 12:39:40  andreas
** new nextObject for DcmDataSet, DcmFileFormat, DcmItem, ...
**
** Revision 1.6  1996/06/19 13:54:10  andreas
** - correct error when reading big sequences with little buffers from net
**
** Revision 1.5  1996/01/29 13:38:29  andreas
** - new put method for every VR to put value as a string
** - better and unique print methods
**
** Revision 1.4  1996/01/09 11:06:48  andreas
** New Support for Visual C++
** Correct problems with inconsistent const declarations
** Correct error in reading Item Delimitation Elements
**
** Revision 1.3  1996/01/05 13:27:41  andreas
** - changed to support new streaming facilities
** - unique read/write methods for file and block transfer
** - more cleanups
**
**
*/

