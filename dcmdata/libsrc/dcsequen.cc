/*
 *
 *  Copyright (C) 1994-99, OFFIS
 *
 *  This software and supporting documentation were developed by
 *
 *    Kuratorium OFFIS e.V.
 *    Healthcare Information and Communication Systems
 *    Escherweg 2
 *    D-26121 Oldenburg, Germany
 *
 *  THIS SOFTWARE IS MADE AVAILABLE,  AS IS,  AND OFFIS MAKES NO  WARRANTY
 *  REGARDING  THE  SOFTWARE,  ITS  PERFORMANCE,  ITS  MERCHANTABILITY  OR
 *  FITNESS FOR ANY PARTICULAR USE, FREEDOM FROM ANY COMPUTER DISEASES  OR
 *  ITS CONFORMITY TO ANY SPECIFICATION. THE ENTIRE RISK AS TO QUALITY AND
 *  PERFORMANCE OF THE SOFTWARE IS WITH THE USER.
 *
 *  Module:  dcmdata
 *
 *  Author:  Gerd Ehlers, Andreas Barth
 *
 *  Purpose: class DcmSequenceOfItems
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2000-02-10 10:52:22 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/libsrc/dcsequen.cc,v $
 *  CVS/RCS Revision: $Revision: 1.30 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#ifdef HAVE_STDLIB_H
#ifndef  _BCB_4
/* workaround for bug in Borland C++ Builder 4 */
BEGIN_EXTERN_C
#endif
#include <stdlib.h>
#ifndef  _BCB_4
END_EXTERN_C
#endif
#endif

#include <stdio.h>
#include <iostream.h>

#include "dcsequen.h"
#include "dcitem.h"
#include "dcdirrec.h"
#include "dcvr.h"
#include "dcpxitem.h"
#include "dcswap.h"
#include "dcdebug.h"
#include "dcmetinf.h"

#include "dcdeftag.h"


// ********************************


DcmSequenceOfItems::DcmSequenceOfItems(const DcmTag &tag, const Uint32 len)
  : DcmElement(tag, len),
    itemList(NULL),
    lastItemComplete(OFTrue),
    fStartPosition(0)
{
    itemList = new DcmList;
}


// ********************************


DcmSequenceOfItems::DcmSequenceOfItems(const DcmSequenceOfItems& old)
  : DcmElement(old),
    itemList(NULL),
    lastItemComplete(OFTrue),
    fStartPosition(old.fStartPosition)
{
    itemList = new DcmList;

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
}


// ********************************


DcmSequenceOfItems::~DcmSequenceOfItems()
{
    DcmObject *dO;
    itemList->seek( ELP_first );
    while ( !itemList->empty() )
    {
        dO = itemList->remove();
        if ( dO != (DcmObject*)NULL )
            delete dO;
    }
    delete itemList;
}


// ********************************

DcmSequenceOfItems &DcmSequenceOfItems::operator=(const DcmSequenceOfItems &obj)
{
  DcmElement::operator=(obj);
  lastItemComplete = obj.lastItemComplete;
  fStartPosition = obj.fStartPosition;
  
  DcmList *newList = new DcmList; // DcmList has no copy constructor. Need to copy ourselves.
  if (newList) switch (obj.ident())
  {
    case EVR_SQ:
    case EVR_pixelSQ:
    case EVR_fileFormat:
        if ( !obj.itemList->empty() )
        {
            DcmObject *oldDO;
            DcmObject *newDO;
            newList->seek( ELP_first );
            obj.itemList->seek( ELP_first );
            do {
                oldDO = obj.itemList->get();
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

                newList->insert( newDO, ELP_next );
            } while ( obj.itemList->seek( ELP_next ) );
        }
        break;
    default:
        cerr << "Warning: DcmSequenceOfItems: wrong use of Copy-Constructor"
             << endl;
        break;
    }
    delete itemList;
    itemList = newList;
    
    return *this;
}

// ********************************

void DcmSequenceOfItems::print(ostream & out, const OFBool showFullData,
                               const int level, const char *pixelFileName,
                               size_t *pixelCounter)
{
    char info[200]; 
    const char *title = (char*)NULL;
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
            dO->print(out, showFullData, level + 1, pixelFileName, pixelCounter);
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


OFBool DcmSequenceOfItems::canWriteXfer(const E_TransferSyntax newXfer, 
                                      const E_TransferSyntax oldXfer)
{
    OFBool canWrite = OFTrue;

    if (newXfer == EXS_Unknown)
        canWrite = OFFalse;
    else if ( !itemList->empty() )
    {
        DcmObject *dO;
        itemList->seek( ELP_first );
        do 
        {
            dO = itemList->get();
            canWrite = dO -> canWriteXfer(newXfer, oldXfer);
        } while (itemList->seek( ELP_next )  && canWrite);
    }

    return canWrite;
}

// ********************************

Uint32 DcmSequenceOfItems::calcElementLength(const E_TransferSyntax xfer,
                                             const E_EncodingType enctype)
{
    Uint32 seqlen = DcmElement::calcElementLength(xfer, enctype);
    if (Length == DCM_UndefinedLength)
        seqlen += 8;     // for Sequence Delimitation Tag
    return seqlen;
}
    
// ********************************


Uint32 DcmSequenceOfItems::getLength(const E_TransferSyntax xfer,
                                     const E_EncodingType enctype)
{
    Uint32 seqlen = 0;
    if ( !itemList->empty() )
    {
        DcmItem *dI;
        itemList->seek( ELP_first );
        do {
            dI = (DcmItem*)( itemList->get() );
            seqlen += dI->calcElementLength( xfer, enctype );
        } while ( itemList->seek( ELP_next ) );
    }
    return seqlen;
}


// ********************************


E_Condition DcmSequenceOfItems::computeGroupLengthAndPadding
                                   (const E_GrpLenEncoding glenc,
                                    const E_PaddingEncoding padenc,
                                    const E_TransferSyntax xfer,
                                    const E_EncodingType enctype,
                                    const Uint32 padlen,
                                    const Uint32 subPadlen,
                                    Uint32 instanceLength)
{
    E_Condition l_error = EC_Normal;

    if ( !itemList->empty() )
    {
        itemList->seek( ELP_first );
        do {
            DcmItem *dO = (DcmItem*)itemList->get();
            l_error = dO->computeGroupLengthAndPadding
                (glenc, padenc, xfer, enctype, padlen, 
                 subPadlen, instanceLength);
        } while ( itemList->seek( ELP_next ) );
    }
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


E_Condition DcmSequenceOfItems::readTagAndLength(DcmStream & inStream,
                                                 const E_TransferSyntax xfer,
                                                 DcmTag &tag,
                                                 Uint32 & length)
{
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
        swapIfNecessary(gLocalByteOrder, iByteOrder, &groupTag, 2, 2);
        swapIfNecessary(gLocalByteOrder, iByteOrder, &elementTag, 2, 2);
        // Tag ist gelesen

        DcmTag newTag(groupTag, elementTag);

        Uint32 valueLength = 0;
        inStream.ReadBytes(&valueLength, 4);
        swapIfNecessary(gLocalByteOrder, iByteOrder, &valueLength, 4, 4);
        if ((valueLength & 1)&&(valueLength != (Uint32) -1))
            cerr << "Error: Length of sequence with Tag " << Tag << " is odd\n";
        length = valueLength;

        tag = newTag;                  // Rueckgabewert: assignment-operator
    }

    debug(4, ( "errorFlag=(%d) in Sequ.readTag..", l_error ));
    return l_error;
}


// ********************************


E_Condition DcmSequenceOfItems::readSubItem(DcmStream & inStream,
                                            const DcmTag &newTag,
                                            const Uint32 newLength,
                                            const E_TransferSyntax xfer,
                                            const E_GrpLenEncoding glenc,
                                            const Uint32 maxReadLength)
{
    // For DcmSequenceOfItems, subObject is always inherited from DcmItem
    // For DcmPixelSequence, subObject is always inherited from DcmPixelItem
    DcmObject * subObject = NULL;
    E_Condition l_error = this -> makeSubObject(subObject, newTag, newLength);
    if ( l_error == EC_Normal && subObject != NULL )
    {
        inStream.UnsetPutbackMark();
        itemList->insert(subObject, ELP_next);
        // hier wird Subitem eingelesen
        l_error = subObject->read(inStream, xfer, glenc, maxReadLength);
    }
    else if ( l_error == EC_InvalidTag )  // versuche das Parsing wieder
    {                                     // einzurenken
        inStream.Putback();
        cerr << "Warning: DcmSequenceOfItems::readSubItem(): parse error "
            "occured: " << newTag << endl;
        debug(1, ( "Warning: DcmSequenceOfItems::readSubItem(): parse error occured:"
                " (0x%4.4hx,0x%4.4hx)", newTag.getGTag(), newTag.getETag() ));

    }
    else if ( l_error != EC_SequEnd )
    {
        inStream.UnsetPutbackMark();
        cerr << "Error: DcmSequenceOfItems::readSubItem(): cannot create "
             << "SubItem " << newTag << endl;
        debug(1, ( "Error: DcmSequenceOfItems::readSubItem(): cannot create SubItem"
                " (0x%4.4hx,0x%4.4hx)", newTag.getGTag(), newTag.getETag() ));

    }
    else
        inStream.UnsetPutbackMark();

    return l_error;
}


// ********************************


E_Condition DcmSequenceOfItems::read(DcmStream & inStream,
                                     const E_TransferSyntax xfer,
                                     const E_GrpLenEncoding glenc,
                                     const Uint32 maxReadLength)
{
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
                        break;                  // beende while-Schleife
                    else
                        fTransferredBytes += 8;

                    lastItemComplete = OFFalse;
                    errorFlag = readSubItem(inStream, newTag, newValueLength, 
                                            xfer, glenc, maxReadLength);
                    if ( errorFlag == EC_Normal )
                        lastItemComplete = OFTrue;
                }
                else
                {
                    errorFlag = itemList->get()->read(inStream, xfer, glenc,
                                                      maxReadLength);
                    if ( errorFlag == EC_Normal )
                        lastItemComplete = OFTrue;
                }
                fTransferredBytes = inStream.Tell() - fStartPosition;
                                
                if ( errorFlag != EC_Normal )
                    break;                      

            } //while 
            if ((fTransferredBytes < Length || !lastItemComplete) &&
                errorFlag == EC_Normal)
                errorFlag = EC_StreamNotifyClient;
        } // else errorFlag

        if ( errorFlag == EC_SequEnd )
            errorFlag = EC_Normal;
        if ( errorFlag == EC_Normal )
            fTransferState = ERW_ready;      // Sequence ist komplett
    }
    return errorFlag;
}


// ********************************



E_Condition DcmSequenceOfItems::write(DcmStream & outStream,
                                      const E_TransferSyntax oxfer,
                                      const E_EncodingType enctype)
{
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
                                                  enctype);
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
    return errorFlag;
}


// ********************************


void  DcmSequenceOfItems::transferInit(void)
{
    DcmObject::transferInit();
    fStartPosition = 0;
    lastItemComplete = OFTrue;
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


E_Condition DcmSequenceOfItems::prepend(DcmItem* item)
{
    errorFlag = EC_Normal;
    if ( item != (DcmItem*)NULL ) {
        itemList->prepend( item );
    }
    else
        errorFlag = EC_IllegalCall;

    return errorFlag;
}

// ********************************


E_Condition DcmSequenceOfItems::insert(DcmItem* item,
                                       unsigned long where,
                                       OFBool before)
{
    errorFlag = EC_Normal;
    if ( item != (DcmItem*)NULL ) {
        itemList->seek_to( where );
        // insert before or after "where"
        E_ListPos whichSide = (before)?(ELP_prev):(ELP_next);
        itemList->insert( item, whichSide );
        if (before) {
            debug(3, ("DcmSequenceOfItems::insert() item inserted before position %d", where ));
        } else {
            debug(3, ("DcmSequenceOfItems::insert() item inserted after position %d", where ));
        }
    }
    else
        errorFlag = EC_IllegalCall;
    return errorFlag;
}


// ********************************


E_Condition DcmSequenceOfItems::append(DcmItem* item)
{
    errorFlag = EC_Normal;
    if ( item != (DcmItem*)NULL ) {
        itemList->append( item );
    }
    else
        errorFlag = EC_IllegalCall;
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
                )
            { /* do nothing */ }
        }
        return itemList -> seek(ELP_next);
    }
}

// ********************************

E_Condition DcmSequenceOfItems::nextObject(DcmStack & stack, const OFBool intoSub)
{
    E_Condition l_error = EC_Normal;
    DcmObject * container = NULL;
    DcmObject * obj = NULL;
    DcmObject * result = NULL;
    OFBool examSub = intoSub;

    if (stack.empty())
    {
        stack.push(this);
        examSub = OFTrue;
    }

    obj = stack.top();
    if (obj->isLeaf() || !intoSub)
    {
        stack.pop();
        if (stack.card() > 0)
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
    errorFlag = EC_Normal;
    DcmItem *item;
    item = (DcmItem*)( itemList->seek_to(num) );  // liest Item aus Liste
    if ( item != (DcmItem*)NULL )
    {
        itemList->remove();
    }
    else
        errorFlag = EC_IllegalCall;
    return item;
}


// ********************************


DcmItem* DcmSequenceOfItems::remove( DcmItem* item )
{
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
                itemList->remove();         // entfernt Element aus Liste,
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
    return retItem;
}


// ********************************


E_Condition DcmSequenceOfItems::clear()
{
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
    return errorFlag;
}


// ********************************


E_Condition DcmSequenceOfItems::verify(const OFBool autocorrect)
{
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
    if ( autocorrect == OFTrue )
        Length = this->getLength();

    return errorFlag;
}


// ********************************

    // Vorbedingung: itemList ist nicht leer!
    // Ergebnis:     - return EC_Normal;
    //                 gefundener Element-Zeiger auf resultStack
    //               - return EC_TagNotFound;
    //                 resultStack unveraendert
    // Weitersuchen: schreibe Zeiger des Sub-Elementes auf resultStack und
    //               starte dann Sub-Suche


E_Condition DcmSequenceOfItems::searchSubFromHere(const DcmTagKey &tag,
                                                  DcmStack &resultStack,
                                                  const OFBool searchIntoSub)
{
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
                                          OFTrue );
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
    }
    return l_error;
}


// ********************************


E_Condition DcmSequenceOfItems::search( const DcmTagKey &tag,
                                        DcmStack &resultStack,
                                        E_SearchMode mode,
                                        OFBool searchIntoSub )
{
    DcmObject *dO = (DcmObject*)NULL;
    E_Condition l_error = EC_TagNotFound;
    if ( mode == ESM_afterStackTop && resultStack.top() == this )
    {
        l_error = searchSubFromHere( tag, resultStack, searchIntoSub );
        debug(5, ( "DcmSequenceOfItems::search() mode=ESM_afterStackTop && resultStack.top()=this" ));

    }
    else if ( !itemList->empty() )
    {
        if ( mode == ESM_fromHere || resultStack.empty() )
        {
            debug(5, ( "DcmSequenceOfItems::search() mode=ESM_fromHere || resultStack.empty()" ));

            resultStack.clear();
            l_error = searchSubFromHere( tag, resultStack, searchIntoSub );
        }
        else if ( mode == ESM_fromStackTop )
        {
            debug(5, ( "DcmSequenceOfItems::search() mode=ESM_fromStackTop" ));

            dO = resultStack.top();
            if ( dO == this )
                l_error = searchSubFromHere( tag, resultStack, searchIntoSub );
            else
            {   // gehe direkt zu Sub-Baum und suche dort weiter
                l_error = dO->search( tag, resultStack, mode, searchIntoSub );
// The next two lines destroy the stack, delete them
//                if ( l_error != EC_Normal ) // raeumt nur die oberste Stackebene
//                    resultStack.pop();      // ab; der Rest ist unveraendert
            }
        }
        else if ( mode == ESM_afterStackTop && searchIntoSub )
        {
            debug(5, ( "DcmSequenceOfItems::search() mode=ESM_afterStackTop && searchIntoSub" ));

            // resultStack enthaelt Zielinformationen:
            // - stelle Zustand der letzen Suche in den einzelnen Suchroutinen
            //   wieder her
            // - finde Position von dO in Baum-Struktur
            //   1. suche eigenen Stack-Eintrag
            //      - bei Fehlschlag Suche beenden
            //   2. nehme naechsthoeheren Eintrag dnO
            //   3. stelle eigene Liste auf Position von dnO
            //   4. starte Suche ab dnO

            unsigned long i = resultStack.card();
            debug(5, ( "DcmSequenceOfItems::search() resultStack: card()=%d", i ));

            while ( i > 0 && (dO = resultStack.elem(i-1)) != this )
            {
                debug(5, ( "DcmSequenceOfItems::search() --dO=elem(%d)=%p  this=%p", i-1, dO, this ));
                i--;
            }
            if ( dO != this && resultStack.card() > 0 )
            {                          // oberste Ebene steht nie in resultStack
                i = resultStack.card()+1;// zeige jetzt auf hoechste Ebene+1
                dO = this;               // Treffer der hoechsten Ebene!
            }
            if ( dO == this )
            {
                debug(5, ( "DcmSequenceOfItems::search() --dO=elem(%d)=%p==this=%p", i-1, dO, this ));
                debug(5, ( "currently at resultStack position %d, dO=%p", i-1, dO ));

                if ( i == 1 )                 // habe resultStack.top() gefunden
                    l_error = EC_TagNotFound; // markiere als kein Treffer, s.o.
                else
                {
                    E_SearchMode submode = mode;
                    OFBool searchNode = OFTrue;
                    DcmObject *dnO;
                    dnO = resultStack.elem( i-2 ); // Knoten der naechsten Ebene
                    debug(5, ( "DcmSequenceOfItems::search() itemList: dnO=%p", dnO ));

                    itemList->seek( ELP_first );
                    do {
                        dO = itemList->get();
                        searchNode = searchNode ? ( dO != dnO ) : OFFalse;
                        Cdebug(5, searchNode,  ("DcmSequenceOfItems::search() --searching Node dnO=%p, found: dO=%p", dnO, dO ));
                        Cdebug(5, !searchNode && submode==ESM_afterStackTop,
                                ("DcmSequenceOfItems::search() --searching Node dnO=%p found!", dO ));
                        Cdebug(5, !searchNode && submode!=ESM_afterStackTop,
                                ("DcmSequenceOfItems::search() --next Node dO=%p for beeing tested", dO ));

                        if ( !searchNode )
                        {                               // suche jetzt weiter
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
                                                      OFTrue );
                            if ( l_error != EC_Normal )
                                resultStack.pop();
                            else
                                break;
                            submode = ESM_fromStackTop; // ab hier normale Suche
                        }
                    } while ( itemList->seek( ELP_next ) );
                }
            }
            else                              // wird vermutlich nie erreicht
                l_error = EC_IllegalCall;
        } // ( mode == ESM_afterStackTop
        else
            l_error = EC_IllegalCall;
    }
    return l_error;
}


// ********************************


E_Condition DcmSequenceOfItems::searchErrors( DcmStack &resultStack )
{
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
    return l_error;
}


// ********************************


E_Condition DcmSequenceOfItems::loadAllDataIntoMemory()
{
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
    return l_error;
}


// ********************************



/*
** CVS/RCS Log:
** $Log: dcsequen.cc,v $
** Revision 1.30  2000-02-10 10:52:22  joergr
** Added new feature to dcmdump (enhanced print method of dcmdata): write
** pixel data/item value fields to raw files.
**
** Revision 1.29  2000/02/03 16:29:40  joergr
** Corrected bug that caused wrong calculation of group length for sequence
** of items (e.g. encapsulated pixel data).
**
** Revision 1.28  2000/02/01 10:12:10  meichel
** Avoiding to include <stdlib.h> as extern "C" on Borland C++ Builder 4,
**   workaround for bug in compiler header files.
**
** Revision 1.27  1999/03/31 09:25:38  meichel
** Updated copyright header in module dcmdata
**
** Revision 1.26  1998/11/12 16:48:20  meichel
** Implemented operator= for all classes derived from DcmObject.
**
** Revision 1.25  1998/07/15 15:52:06  joergr
** Removed several compiler warnings reported by gcc 2.8.1 with
** additional options, e.g. missing copy constructors and assignment
** operators, initialization of member variables in the body of a
** constructor instead of the member initialization list, hiding of
** methods by use of identical names, uninitialized member variables,
** missing const declaration of char pointers. Replaced tabs by spaces.
**
** Revision 1.24  1998/01/27 10:49:26  meichel
** Minor bug corrections (string too short, incorrect return value).
**   Thanks to Andreas Barth <anba@bruker.de> for the report.
**
** Revision 1.23  1997/09/12 13:44:54  meichel
** The algorithm introduced on 97.08.28 to detect incorrect odd-length
**   value fields falsely reported undefined length sequences and items
**   to be wrong. Corrected.
**
** Revision 1.22  1997/08/29 07:53:24  andreas
** - New error messages if length of an element is odd. Previously, no
**   error was reported. But the length is corrected by the method
**   newValueField and so it was impossible for a checking utility to find
**   such an error in DICOM objects.
**
** Revision 1.21  1997/07/21 08:25:29  andreas
** - Replace all boolean types (BOOLEAN, CTNBOOLEAN, DICOM_BOOL, BOOL)
**   with one unique boolean type OFBool.
**
** Revision 1.20  1997/07/07 07:46:20  andreas
** - Changed parameter type DcmTag & to DcmTagKey & in all search functions
**   in DcmItem, DcmSequenceOfItems, DcmDirectoryRecord and DcmObject
** - Enhanced (faster) byte swapping routine. swapIfNecessary moved from
**   a method in DcmObject to a general function.
**
** Revision 1.19  1997/07/03 15:10:04  andreas
** - removed debugging functions Bdebug() and Edebug() since
**   they write a static array and are not very useful at all.
**   Cdebug and Vdebug are merged since they have the same semantics.
**   The debugging functions in dcmdata changed their interfaces
**   (see dcmdata/include/dcdebug.h)
**
** Revision 1.18  1997/06/06 09:55:31  andreas
** - corrected error: canWriteXfer returns false if the old transfer syntax
**   was unknown, which causes several applications to prohibit the writing
**   of dataset.
**
** Revision 1.17  1997/05/30 06:45:45  andreas
** - fixed problem of inconsistent interfaces and implementation that the
**   syntax check of GNU C++ does not find.
**
** Revision 1.16  1997/05/27 13:49:02  andreas
** - Add method canWriteXfer to class DcmObject and all derived classes.
**   This method checks whether it is possible to convert the original
**   transfer syntax to an new transfer syntax. The check is used in the
**   dcmconv utility to prohibit the change of a compressed transfer
**   syntax to a uncompressed.
**
** Revision 1.15  1997/05/16 08:23:55  andreas
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
** Revision 1.14  1997/04/24 12:11:49  hewett
** Fixed DICOMDIR generation bug affecting ordering of
** patient/study/series/image records (item insertion into a sequence
** did produce the expected ordering).
**
** Revision 1.13  1997/04/18 08:08:54  andreas
** - Corrected debugging code
**
** Revision 1.12  1996/09/13 12:04:13  hewett
** Corrected missing () in function call (stack.card()) used in nextObject(...)
**
** Revision 1.11  1996/08/08 10:15:10  andreas
** Some more testing in nextObject
**
** Revision 1.10  1996/08/08 10:06:24  andreas
** Correct error for intoSub=OFFalse
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
*/

