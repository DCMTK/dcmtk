/*
 *
 *  Copyright (C) 1994-2000, OFFIS
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
 *  Author:  Gerd Ehlers
 *
 *  Purpose: class DcmItem
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 2000-03-08 16:26:36 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/libsrc/dcitem.cc,v $
 *  CVS/RCS Revision: $Revision: 1.51 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#ifdef HAVE_STDLIB_H
#ifndef  _BCB4
/* workaround for bug in Borland C++ Builder 4 */
BEGIN_EXTERN_C
#endif
#include <stdlib.h>
#ifndef  _BCB4
END_EXTERN_C
#endif
#endif

#include <stdio.h>
#include <iostream.h>
#include <string.h>
#include <ctype.h>

#include "ofstring.h"
#include "dctk.h"
#include "dcitem.h"
#include "dcobject.h"
#include "dcvr.h"
#include "dcxfer.h"
#include "dcdebug.h"



// ********************************


DcmItem::DcmItem()
  : DcmObject(ItemTag),
    elementList(NULL),
    lastElementComplete(OFTrue),
    fStartPosition(0)
{
    elementList = new DcmList;
}


// ********************************


DcmItem::DcmItem(const DcmTag &tag, const Uint32 len)
  : DcmObject(tag, len),
    elementList(NULL),
    lastElementComplete(OFTrue),
    fStartPosition(0)
{
    elementList = new DcmList;
}


// ********************************


DcmItem::DcmItem( const DcmItem& old )
  : DcmObject(old),
    elementList(NULL),
    lastElementComplete(OFTrue),
    fStartPosition(old.fStartPosition)
{
    elementList = new DcmList;

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
        CERR << "Warning: DcmItem: wrong use of Copy-Constructor" << endl;
        break;
    }
}


// ********************************


DcmItem::~DcmItem()
{
    DcmObject *dO;
    elementList->seek( ELP_first );
    while ( !elementList->empty() ) {
        dO = elementList->remove();
        if ( dO != (DcmObject*)NULL )
            delete dO;
    }
    delete elementList;
}


// ********************************


OFBool DcmItem::foundVR( char* atposition )
{
    char c1 =  atposition[0];
    char c2 = atposition[1];
    OFBool valid = OFFalse;

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
        valid = OFFalse;
    }
    return valid;
}


// ********************************

/*
   wird in DcmDataset und in DcmMetaInfo benutzt
*/

E_TransferSyntax DcmItem::checkTransferSyntax(DcmStream & inStream)
{
    E_TransferSyntax transferSyntax;
    char tagAndVR[6];
    inStream.SetPutbackMark();
    inStream.ReadBytes(tagAndVR, 6);               // check Tag & VR
    inStream.Putback();

    char c1 = tagAndVR[0];
    char c2 = tagAndVR[1];
    char c3 = tagAndVR[2];
    char c4 = tagAndVR[3];
    Uint16 t1 = (c1 & 0xff) + ((c2 & 0xff) << 8);  // explizite little endian
    Uint16 t2 = (c3 & 0xff) + ((c4 & 0xff) << 8);  // Konversion
    DcmTag taglittle(t1, t2);
    DcmTag tagbig(swapShort(t1), swapShort(t2));

    if (taglittle.error() && tagbig.error()) {      // no valid tag
        if (foundVR( &tagAndVR[4])) {               // assume little-endian
            transferSyntax = EXS_LittleEndianExplicit;
        } else {
            transferSyntax = EXS_LittleEndianImplicit;
        }
    } else {
        if ( foundVR( &tagAndVR[4] ) )  {           // explicit VR
            if ( taglittle.error() ) {
                transferSyntax = EXS_BigEndianExplicit;
            } else if ( tagbig.error() ) {
                transferSyntax = EXS_LittleEndianExplicit;
            } else { /* both are error-free */
                /* group 0008 is much more probable than group 0800 for the first tag */
                if ((taglittle.getGTag() > 0xff)&&(tagbig.getGTag() <= 0xff)) transferSyntax = EXS_BigEndianExplicit;
                else transferSyntax = EXS_LittleEndianExplicit;
            }
        } else  {                                   // implicit VR
            if ( taglittle.error() ) {
                transferSyntax = EXS_BigEndianImplicit;
            } else if ( tagbig.error() ) {
                transferSyntax = EXS_LittleEndianImplicit;
            } else { /* both are error-free */
                /* group 0008 is much more probable than group 0800 for the first tag */
                if ((taglittle.getGTag() > 0xff)&&(tagbig.getGTag() <= 0xff)) transferSyntax = EXS_BigEndianImplicit;
                else transferSyntax = EXS_LittleEndianImplicit;
            }
        }
    }                                               // gueltige TransferSyntax

    debug(3, ( "found TransferSyntax=(%s)", DcmXfer(transferSyntax).getXferName()));

    return transferSyntax;
} // DcmItem::checkTransferSyntax


// ********************************


DcmObject* DcmItem::copyDcmObject( DcmObject *oldObj )
{
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
    case EVR_UT:
        newObj = new DcmUnlimitedText( *(DcmUnlimitedText*)oldObj );
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
    case EVR_UN :
    case EVR_UNKNOWN :   // Treat unknown elements as Byte-String
    case EVR_UNKNOWN2B : // Treat unknown elements as Byte-String
        newObj = new DcmOtherByteOtherWord( *(DcmOtherByteOtherWord*)oldObj );
        break;

        // pixel data
    case EVR_PixelData:
        newObj = new DcmPixelData(*(DcmPixelData *)oldObj);
        break;

        // overlay data
    case EVR_OverlayData:
        newObj = new DcmOverlayData(*(DcmOverlayData *)oldObj);
        break;

    case EVR_na :
    default :
        CERR << "Warning: DcmItem::copyDcmObject(): unsupported Element("
             << hex << oldObj->getGTag() << "," << oldObj->getETag()
             << dec << ") with ident()="
             << DcmVR(oldObj->ident()).getVRName() << " found."
             << endl;
        break;
    }
    return newObj;
}


// ********************************


DcmEVR DcmItem::ident() const
{
    return EVR_item;
}


// ********************************


void DcmItem::print(ostream & out, const OFBool showFullData,
                    const int level, const char *pixelFileName,
                    size_t *pixelCounter)
{
    char *info = new char[200];
    const char *title = (char*)NULL;
    if ( Length == DCM_UndefinedLength )
        title = "Item with undefined length";
    else
        title = "Item with explicit Length";

    sprintf( info, "(%s  #=%ld)", title, (long)card() );
    printInfoLine(out, showFullData, level, info );
    if ( !elementList->empty() )
    {
        DcmObject *dO;
        elementList->seek( ELP_first );
        do {
            dO = elementList->get();
            dO->print(out, showFullData, level + 1, pixelFileName, pixelCounter);
        } while ( elementList->seek( ELP_next ) );
    }

    DcmTag delimItemTag( DCM_ItemDelimitationItem );
    if ( Length == DCM_UndefinedLength)
        printInfoLine(out, showFullData, level, delimItemTag,
                      0, "(ItemDelimitationItem)" );
    else
        printInfoLine(out, showFullData, level, delimItemTag,
                      0, "(ItemDelimitationItem for re-encoding)" );
    delete[] info;
}


// ********************************


unsigned long DcmItem::getVM()
{
    return 1L;
}


// ********************************


OFBool DcmItem::canWriteXfer(const E_TransferSyntax newXfer,
                              const E_TransferSyntax oldXfer)
{
    OFBool canWrite = OFTrue;

    if (newXfer == EXS_Unknown)
        canWrite = OFFalse;
    else if ( !elementList->empty() )
    {
        DcmObject *dO;
        elementList->seek( ELP_first );
        do
        {
            dO = elementList->get();
            canWrite = dO -> canWriteXfer(newXfer, oldXfer);
        } while (elementList->seek( ELP_next )  && canWrite);
    }

    return canWrite;
}


// ********************************


Uint32 DcmItem::calcElementLength(const E_TransferSyntax xfer,
                                  const E_EncodingType enctype)
{
    Uint32 itemlen = 0L;
    DcmXfer xferSyn(xfer);
    itemlen = getLength(xfer, enctype) +
        xferSyn.sizeofTagHeader(getVR());
    if (enctype == EET_UndefinedLength)
        itemlen += 8;
    return itemlen;
}


// ********************************


Uint32 DcmItem::getLength(const E_TransferSyntax xfer,
                          const E_EncodingType enctype)
{
    Uint32 itemlen = 0L;
    if ( !elementList->empty() )
    {
        DcmObject *dO;
        elementList->seek( ELP_first );
        do {
            dO = elementList->get();
            itemlen += dO->calcElementLength(xfer, enctype);
        } while ( elementList->seek( ELP_next ) );
    }
    return itemlen;
}


// ********************************


E_Condition DcmItem::computeGroupLengthAndPadding
                            (const E_GrpLenEncoding glenc,
                             const E_PaddingEncoding padenc,
                             const E_TransferSyntax xfer,
                             const E_EncodingType enctype,
                             const Uint32 padlen,
                             const Uint32 subPadlen,
                             Uint32 instanceLength)
{
    if ((padenc == EPD_withPadding && (padlen % 2 || subPadlen % 2)) ||
        ((glenc == EGL_recalcGL || glenc == EGL_withGL ||
          padenc == EPD_withPadding) && xfer == EXS_Unknown))
        return EC_IllegalCall;

    if (glenc == EGL_noChange && padenc == EPD_noChange)
        return EC_Normal;

    E_Condition l_error = EC_Normal;
    if ( !elementList->empty() )
    {
        DcmObject *dO;
        OFBool beginning = OFTrue;
        Uint16 lastGrp = 0x0000;
        Uint16 actGrp;
        DcmUnsignedLong * actGLElem = NULL;
        DcmUnsignedLong * paddingGL = NULL;
        Uint32 grplen = 0L;
        DcmXfer xferSyn(xfer);

        E_ListPos seekmode = ELP_next;
        elementList->seek( ELP_first );
        do
        {
            seekmode = ELP_next;
            dO = elementList->get();

            // compute GroupLength and padding in subSequence
            if ( dO->getVR() == EVR_SQ )
            {
                Uint32 templen = instanceLength +
                    xferSyn.sizeofTagHeader(EVR_SQ);
                l_error =
                    ((DcmSequenceOfItems*)dO)->computeGroupLengthAndPadding
                    (glenc, padenc, xfer, enctype, subPadlen, subPadlen,
                     templen);
            }

            if (l_error == EC_Normal)
            {
                if (((glenc ==  EGL_withGL || glenc == EGL_withoutGL) &&
                     dO->getETag() == 0x0000) ||
                    (padenc != EPD_noChange &&
                     dO->getTag() == DCM_DataSetTrailingPadding))
                    // delete GroupLength or Padding Element
                {
                    delete elementList->remove();
                    seekmode = ELP_atpos;           // remove = 1 forward
                    dO = NULL;
                }
                else  if (glenc == EGL_withGL || glenc == EGL_recalcGL)
                    // recognize new group
                {
                    actGrp = dO->getGTag();
                    if (actGrp!=lastGrp || beginning) // new Group found
                    {
                        beginning = OFFalse;
                        if (dO->getETag() == 0x0000 && // GroupLength (xxxx,0000)
                            dO->ident() != EVR_UL)     // no UL Element
                        { // replace with UL
                            delete elementList->remove();
                            DcmTag tagUL(actGrp, 0x0000, EVR_UL);
                            DcmUnsignedLong *dUL = new DcmUnsignedLong(tagUL);
                            elementList->insert(dUL, ELP_prev);
                            dO = dUL;
                            CERR << "Info: DcmItem::addGroupLengthElements()"
                                " Group Length found, which was not from type"
                                " UL - corrected." << endl;
                        }
                        else if (glenc == EGL_withGL)
                        {
                            // Create GroupLength element
                            DcmTag tagUL(actGrp, 0x0000, EVR_UL);
                            DcmUnsignedLong *dUL = new DcmUnsignedLong( tagUL );
                            // insert new GroupLength element
                            elementList->insert( dUL, ELP_prev );
                            dO = dUL;
                        }
                        // Store GroupLength of group 0xfffc later
                        // access if padding is enabled
                        if (padenc == EPD_withPadding && actGrp == 0xfffc)
                            paddingGL = (DcmUnsignedLong *)dO;

                        // Write computed length of group into GroupLength
                        // Element of previous group
                        if (actGLElem != (DcmUnsignedLong*)NULL )
                        {
                            actGLElem->putUint32(grplen);
                            debug(2, ( "DcmItem::computeGroupLengthAndPadding() Length of Group 0x%4.4x len=%lu", actGLElem->getGTag(), grplen ));
                        }
                        grplen = 0L;
                        if (dO -> getETag() == 0x0000)
                            actGLElem = (DcmUnsignedLong*)dO;
                        else
                            actGLElem = NULL;
                    }
                    else // no GroupLengthElement
                        grplen += dO->calcElementLength(xfer, enctype );
                    lastGrp = actGrp;
                }
            }
        } while (l_error == EC_Normal && elementList->seek(seekmode) );

        // die letzte Group Length des Items eintragen
        if (l_error == EC_Normal &&
            (glenc == EGL_withGL || glenc == EGL_recalcGL) &&
            actGLElem)
            actGLElem->putUint32(grplen);

        if (padenc == EPD_withPadding && padlen)
        {
            Uint32 padding;
            if (ident() == EVR_dataset)
            {
                instanceLength += calcElementLength(xfer, enctype);
                padding = padlen - (instanceLength % padlen);
            } else
                padding = padlen - (getLength(xfer, enctype) % padlen);
            if (padding != padlen)
            {
                // Create new padding
                DcmOtherByteOtherWord * paddingEl =
                    new DcmOtherByteOtherWord(DCM_DataSetTrailingPadding);
                Uint32 tmplen = paddingEl -> calcElementLength(xfer, enctype);

                // padding smaller than header of padding Element
                while (tmplen > padding)
                    padding += padlen;
                padding -= tmplen;
                Uint8 * padBytes = new Uint8[padding];
                memzero(padBytes, size_t(padding));
                paddingEl -> putUint8Array(padBytes, padding);
                delete[] padBytes;
                this -> insert(paddingEl);

                if (paddingGL)
                {   // Update GroupLength for Padding if it exists
                    Uint32 len;
                    paddingGL -> getUint32(len);
                    len += paddingEl->calcElementLength(xfer, enctype);
                    paddingGL -> putUint32(len);
                }
            }
        }
    }
    return l_error;
}


// ********************************


E_Condition DcmItem::readTagAndLength(DcmStream & inStream,
                                      const E_TransferSyntax xfer,
                                      DcmTag &tag,
                                      Uint32 & length,
                                      Uint32 & bytesRead)
{
    E_Condition l_error = EC_Normal;
    Uint32 valueLength = 0;
    DcmEVR nxtobj = EVR_UNKNOWN;
    Uint16 groupTag = 0xffff;
    Uint16 elementTag = 0xffff;

    DcmXfer xferSyn(xfer);
    debug(4, ("DcmItem::readTagAndLength() read transfer syntax %s", xferSyn.getXferName()));
    if ((l_error = inStream.Avail(xferSyn.isExplicitVR() ? 6:4)) != EC_Normal)
        return l_error;

    const E_ByteOrder byteOrder = xferSyn.getByteOrder();
    if (byteOrder == EBO_unknown)
        return EC_IllegalCall;

    inStream.SetPutbackMark();
    inStream.ReadBytes(&groupTag, 2);
    inStream.ReadBytes(&elementTag, 2);
    swapIfNecessary(gLocalByteOrder, byteOrder, &groupTag, 2, 2);
    swapIfNecessary(gLocalByteOrder, byteOrder, &elementTag, 2, 2);
    // Tag ist gelesen
    bytesRead = 4;
    DcmTag newTag(groupTag, elementTag );

    if (xferSyn.isExplicitVR() &&
        newTag.getEVR() != EVR_na)      // Delimitation Items do not have a VR
    {
        char vrstr[3];
        vrstr[2] = '\0';
        inStream.ReadBytes(vrstr, 2);  // 2 Byte Laenge:VR als string
        DcmVR vr(vrstr);            // class DcmVR
        if (!vr.isStandard())
        {
            /* this VR is unknown (e.g. "??").  print a warning. */
            CERR << "WARNING: parsing attribute: " << newTag.getXTag() << 
                " non-standard VR encountered: '" << vrstr << "', assuming ";
            if (vr.usesExtendedLengthEncoding()) 
              CERR << "4 byte length field" << endl;
              else CERR << "2 byte length field" << endl;
        }
        newTag.setVR(vr);     // VR in newTag anpassen, falls Element
        // nicht fehlerhaft kodiert ist.
        bytesRead += 2;
    }

    nxtobj = newTag.getEVR();       // VR aus Tag bestimmen


    if ((l_error = inStream.Avail(xferSyn.sizeofTagHeader(nxtobj)-bytesRead))
        != EC_Normal)
    {
        inStream.Putback();
        bytesRead = 0;
        return l_error;
    }
    // The UnsetPutbackMark is in readSubElement

    if (xferSyn.isImplicitVR() ||
        nxtobj == EVR_na)           // DelimitationItems besitzen keine VR!
    {
        inStream.ReadBytes(&valueLength, 4);
        swapIfNecessary(gLocalByteOrder, byteOrder, &valueLength, 4, 4);
        bytesRead += 4;
    }
    else if (xferSyn.isExplicitVR())
    {
        DcmVR vr(newTag.getEVR());
        if (vr.usesExtendedLengthEncoding())
        {
            Uint16 reserved;
            inStream.ReadBytes(&reserved, 2);  // 2 Byte Laenge
            inStream.ReadBytes(&valueLength, 4);
            swapIfNecessary(gLocalByteOrder, byteOrder,
                                    &valueLength, 4, 4);
            bytesRead += 6;
        }
        else
        {
            Uint16 tmpValueLength;
            inStream.ReadBytes(&tmpValueLength, 2);
            swapIfNecessary(gLocalByteOrder, byteOrder, &tmpValueLength, 2, 2);
            bytesRead += 2;
            valueLength = tmpValueLength;
        }
    } // else if ( xfer = EXS..Explicit ...
    else
    {
        l_error = EC_IllegalCall;
    }
    debug(3, ( "TagAndLength read of: (0x%4.4x,0x%4.4x) \"%s\" [0x%8.8x] \"%s\"",
            newTag.getGTag(), newTag.getETag(),
            newTag.getVRName(), valueLength, newTag.getTagName() ));

    if ((valueLength & 1)&&(valueLength != (Uint32) -1))
        CERR << "Error Parsing DICOM object: Length of Tag " << newTag << "is odd\n";
    length = valueLength;        // Rueckgabewert
    tag = newTag;                   // Rueckgabewert
    return l_error;
}


// ********************************


E_Condition DcmItem::readSubElement(DcmStream & inStream,
                                    DcmTag & newTag,
                                    const Uint32 newLength,
                                    const E_TransferSyntax xfer,
                                    const E_GrpLenEncoding glenc,
                                    const Uint32 maxReadLength)
{
    DcmElement *subElem = NULL;
    E_Condition l_error = newDicomElement(subElem, newTag, newLength);

    if ( l_error == EC_Normal && subElem != (DcmElement*)NULL )
    {
        inStream.UnsetPutbackMark();
        // DcmItem::elementList->insert( subElem, ELP_next );  // bit faster
        insert( subElem );  // but this is better
        subElem->transferInit();
        l_error = subElem->read(inStream, xfer, glenc, maxReadLength);
    }
    else if ( l_error == EC_InvalidTag )  // try error recovery
    {
        // This is the second Putback operation on the putback mark in
        // readTagAndLength but it is impossible that both can be executed
        // without setting the Mark twice.
        inStream.Putback();
        CERR << "Warning: DcmItem::readSubElement(): parse error occurred: "
             <<  newTag << endl;
        debug(1, ( "Warning: DcmItem::readSubElement(): parse error occurred:"
                " (0x%4.4hx,0x%4.4hx)\n", newTag.getGTag(), newTag.getETag() ));

    }
    else if ( l_error != EC_ItemEnd )
    {
        // Very important: Unset the putback mark
        inStream.UnsetPutbackMark();
        CERR << "Error: DcmItem::readSubElement(): cannot create SubElement: "
             <<  newTag << endl;
        debug(1, ( "Error: DcmItem::readSubElement(): cannot create SubElement:"
                " (0x%4.4hx,0x%4.4hx)\n", newTag.getGTag(), newTag.getETag() ));

    }
    else
    {
        inStream.UnsetPutbackMark();
    }

    return l_error;
}


// ********************************


E_Condition DcmItem::read(DcmStream & inStream,
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
                        break;                  // beende while-Schleife

                    lastElementComplete = OFFalse;
                    errorFlag = readSubElement(inStream, newTag,
                                               newValueLength,
                                               xfer, glenc, maxReadLength);

                    if ( errorFlag == EC_Normal )
                        lastElementComplete = OFTrue;
                }
                else
                {
                    errorFlag = elementList->get()->read(inStream, xfer, glenc,
                                                         maxReadLength) ;
                    if ( errorFlag == EC_Normal )
                        lastElementComplete = OFTrue;
                }
                fTransferredBytes = inStream.Tell() - fStartPosition;

                if ( errorFlag != EC_Normal )
                    break;                              // beende while-Schleife

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
            fTransferState = ERW_ready;      // Item ist komplett
    }
    return errorFlag;
} // DcmItem::read()


// ********************************


E_Condition DcmItem::write(DcmStream & outStream,
                           const E_TransferSyntax oxfer,
                           const E_EncodingType enctype)
{
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
                    else
                        Length = DCM_UndefinedLength;

                    errorFlag = this -> writeTag(outStream, Tag, oxfer);
                    Uint32 valueLength = Length;
                    DcmXfer outXfer(oxfer);
                    const E_ByteOrder oByteOrder = outXfer.getByteOrder();
                    if (oByteOrder == EBO_unknown)
                        return EC_IllegalCall;
                    swapIfNecessary(oByteOrder, gLocalByteOrder,
                                    &valueLength, 4, 4);
                    outStream.WriteBytes(&valueLength, 4); // 4 Byte Laenge
                    elementList->seek( ELP_first );
                    fTransferState = ERW_inWork;
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
                            errorFlag = dO->write(outStream, oxfer, enctype);
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
    return errorFlag;
}


// ********************************


void DcmItem::transferInit(void)
{
    DcmObject::transferInit();
    fStartPosition = 0;
    lastElementComplete = OFTrue;
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
                             OFBool replaceOld )
{                                                 // geordnetes Einfuegen
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
                debug(3, ("DcmItem::Insert() element (0x%4.4x,0x%4.4x) / VR=\"%s\" at beginning inserted",
                        elem->getGTag(), elem->getETag(), DcmVR(elem->getVR()).getVRName() ));

                break;
            }
            else if ( elem->getTag() > dE->getTag() )
                // Position gefunden
            {
                elementList->insert( elem, ELP_next );
                debug(3, ( "DcmItem::Insert() element (0x%4.4x,0x%4.4x) / VR=\"%s\" inserted",
                        elem->getGTag(), elem->getETag(),
                        DcmVR(elem->getVR()).getVRName() ));

                break;
            }
            else if ( elem->getTag() == dE->getTag() )
            {                                  // Tag ist schon vorhanden.
                if ( elem != dE )              // Altes und neues Element
                {                              // sind verschieden.
                    if ( replaceOld )          // Altes Elem. wird geloescht.
                    {
                        DcmObject *remObj = elementList->remove();
                        // Es gilt: remObj == dE
                        // Liste zeigt auf naechsten
                        debug(3, ( "DcmItem::insert:element (0x%4.4x,0x%4.4x) VR=\"%s\" p=%p removed",
                                remObj->getGTag(), remObj->getETag(),
                                DcmVR(remObj->getVR()).getVRName(), remObj ));

                        if ( remObj != (DcmObject*)NULL )
                        {
                            delete remObj; // loesche "abgehaengtes" Element
                            debug(3, ( "DcmItem::insert:element p=%p deleted", remObj ));

                        }
                        elementList->insert( elem, ELP_prev );
                        debug(3, ( "DcmItem::insert() element (0x%4.4x,0x%4.4x) VR=\"%s\" p=%p replaced older one",
                                elem->getGTag(), elem->getETag(),
                                DcmVR(elem->getVR()).getVRName(), elem ));

                    }   // if ( replaceOld )
                    else
                    {
                        debug(1, ( "DcmItem::insert() element with (0x%4.4x,0x%4.4x) VR=\"%s\" is already inserted:",
                                elem->getGTag(), elem->getETag(),
                                DcmVR(elem->getVR()).getVRName() ));
                        errorFlag = EC_DoubledTag;
                    }   // if ( !replaceOld )
                }   // if ( elem != dE )
                else         // Versuch, Listen-Element nochmals einzufuegen
                {
                    CERR << "Warning: DcmItem::insert(): element "
                         << elem->getTag() << "VR=\"" 
                         << DcmVR(elem->getVR()).getVRName()
                         << " was already in list: not inserted\n";
                    errorFlag = EC_DoubledTag;
                }
                break;
            }
            // else : nicht noetig!
            seekmode = ELP_prev;
        } while ( dE );
    }
    else
        errorFlag = EC_IllegalCall;
    return errorFlag;
}


// ********************************


DcmElement* DcmItem::getElement(const unsigned long num)
{
    errorFlag = EC_Normal;
    DcmElement *elem;
    elem = (DcmElement*)( elementList->seek_to(num) );
    // liest Element aus Liste
    if ( elem == (DcmElement*)NULL )
        errorFlag = EC_IllegalCall;
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
                ) {
                /* do nothing, just keep iterating */
            }
        }
        return elementList -> seek(ELP_next);
    }
}

// ********************************

E_Condition DcmItem::nextObject(DcmStack & stack, const OFBool intoSub)
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


DcmElement* DcmItem::remove(const unsigned long num)
{
    errorFlag = EC_Normal;
    DcmElement *elem;
    elem = (DcmElement*)( elementList->seek_to(num) );
    // liest Element aus Liste
    if ( elem != (DcmElement*)NULL )
    {
        elementList->remove();                  // entfernt Element aus Liste,
    }                                           // aber loescht es nicht
    else
        errorFlag = EC_IllegalCall;
    return elem;
}


// ********************************


DcmElement* DcmItem::remove( DcmObject* elem )
{
    errorFlag = EC_IllegalCall;
    if ( !elementList->empty() && elem != (DcmObject*)NULL )
    {
        DcmObject *dO;
        elementList->seek( ELP_first );
        do {
            dO = elementList->get();
            if ( dO == elem )
            {
                elementList->remove();         // entfernt Element aus Liste,
                // aber loescht es nicht
                errorFlag = EC_Normal;
                break;
            }
        } while ( elementList->seek( ELP_next ) );
    }
    if ( errorFlag == EC_IllegalCall )
        return (DcmElement*)NULL;
    else
        return (DcmElement*)elem;
}


// ********************************


DcmElement* DcmItem::remove(const DcmTagKey & tag)
{
    errorFlag = EC_TagNotFound;
    DcmObject *dO = (DcmObject*)NULL;
    if ( !elementList->empty() )
    {
        elementList->seek( ELP_first );
        do {
            dO = elementList->get();
            if ( dO->getTag() == tag )
            {
                elementList->remove();         // entfernt Element aus Liste,
                // aber loescht es nicht
                errorFlag = EC_Normal;
                break;
            }
        } while ( elementList->seek( ELP_next ) );
    }

    if ( errorFlag == EC_TagNotFound )
        return (DcmElement*)NULL;
    else
        return (DcmElement*)dO;
}


// ********************************


E_Condition DcmItem::clear()
{
    errorFlag = EC_Normal;
    DcmObject *dO;
    elementList->seek( ELP_first );
    while ( !elementList->empty() )
    {
        dO = elementList->remove();
        if ( dO != (DcmObject*)NULL )
            delete dO;                          // loesche auch Sub-Elemente
    }
    Length = 0;

    return errorFlag;
}


// ********************************


E_Condition DcmItem::verify(const OFBool autocorrect )
{
    debug(3, ( "DcmItem::verify() Tag=(0x%4.4x,0x%4.4x) \"%s\" \"%s\"",
            getGTag(), getETag(),
            DcmVR(getVR()).getVRName(), Tag.getTagName() ));

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
    if ( autocorrect == OFTrue )
        Length = this->getLength();
    return errorFlag;
}


// ********************************

    // Vorbedingung: elementList ist nicht leer!
    // Ergebnis:     - return EC_Normal;
    //                 gefundener Element-Zeiger auf resultStack
    //               - return EC_TagNotFound;
    //                 resultStack unveraendert
    // Weitersuchen: schreibe Zeiger des Sub-Elementes auf resultStack und
    //               starte dann Sub-Suche


E_Condition DcmItem::searchSubFromHere( const DcmTagKey &tag,
                                        DcmStack &resultStack,
                                        OFBool searchIntoSub )
{
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
                                          OFTrue );
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
        Cdebug(4, l_error==EC_Normal && dO->getTag()==tag,
               ("DcmItem::searchSubFromHere() Search-Tag=(%4.4x,%4.4x)"
                " found!", tag.getGroup(), tag.getElement()));

    }
    return l_error;
}


// ********************************


E_Condition DcmItem::search(const DcmTagKey &tag,
                            DcmStack &resultStack,
                            E_SearchMode mode,
                            OFBool searchIntoSub)
{
    DcmObject *dO = (DcmObject*)NULL;
    E_Condition l_error = EC_TagNotFound;
    if ( mode == ESM_afterStackTop && resultStack.top() == this )
    {
        l_error = searchSubFromHere(tag, resultStack, searchIntoSub);
        debug(5, ( "DcmItem::search() mode=ESM_afterStackTop && resultStack.top()=this" ));

    }
    else if ( !elementList->empty() )
    {
        if ( mode == ESM_fromHere || resultStack.empty() )
        {
            debug(5, ( "DcmItem::search() mode=ESM_fromHere || resultStack.empty()" ));
            resultStack.clear();
            l_error = searchSubFromHere( tag, resultStack, searchIntoSub );
        }
        else if ( mode == ESM_fromStackTop )
        {
            debug(5, ( "DcmItem::search() mode=ESM_fromStackTop" ));
            dO = resultStack.top();
            if ( dO == this )
                l_error = searchSubFromHere( tag, resultStack, searchIntoSub );
            else
            {   // gehe direkt zu Sub-Baum und suche dort weiter
                l_error = dO->search( tag, resultStack, mode, searchIntoSub );
// The next two lines destroy the stack -> so delete them
//                if ( l_error != EC_Normal ) // raeumt nur die oberste Stackebene
//                    resultStack.pop();      // ab; der Rest ist unveraendert
            }
        }
        else if ( mode == ESM_afterStackTop && searchIntoSub )
        {
            debug(5, ( "DcmItem::search() mode=ESM_afterStackTop && searchIntoSub" ));
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
            debug(5, ( "DcmItem::search() resultStack: card()=%d", i ));

            while ( i > 0 && (dO = resultStack.elem(i-1)) != this )
            {
                debug(5, ( "DcmItem::search() --dO=elem(%d)=%p  this=%p", i-1, dO, this ));

                i--;
            }
            if ( dO != this && resultStack.card() > 0 )
            {                          // oberste Ebene steht nie in resultStack
                i = resultStack.card()+1;// zeige jetzt auf hoechste Ebene+1
                dO = this;               // Treffer der hoechsten Ebene!
            }
            if ( dO == this )
            {
                debug(5, ( "DcmItem::search() --dO=elem(%d)=%p==this=%p", i-1, dO, this ));
                debug(5, ( "currently at resultStack position %d, dO=%p", i-1, dO ));

                if ( i == 1 )                 // habe resultStack.top() gefunden
                    l_error = EC_TagNotFound; // markiere als kein Treffer, s.o.
                else                          //   siehe oben
                {
                    E_SearchMode submode = mode;
                    OFBool searchNode = OFTrue;
                    DcmObject *dnO;
                    dnO = resultStack.elem( i-2 ); // Knoten der naechsten Ebene
                    debug(5, ( "DcmItem::search() elementList: dnO=%p", dnO ));

                    elementList->seek( ELP_first );
                    do {
                        dO = elementList->get();
                        searchNode = searchNode ? ( dO != dnO ) : OFFalse;
                        Cdebug(5, searchNode,  ("DcmItem::search() --searching Node dnO=%p, found: dO=%p", dnO, dO ));
                        Cdebug(5, !searchNode && submode==ESM_afterStackTop,
                                ("DcmItem::search() --searching Node dnO=%p found!", dO ));
                        Cdebug(5, !searchNode && submode!=ESM_afterStackTop,
                                ("DcmItem::search() --next Node dO=%p for beeing tested", dO ));

                        if ( !searchNode )
                        {                               // suche jetzt weiter
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
                                                      OFTrue );
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
    return l_error;
}


// ********************************


E_Condition DcmItem::searchErrors( DcmStack &resultStack )
{
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
    return l_error;
}


// ********************************


E_Condition DcmItem::loadAllDataIntoMemory(void)
{
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
    case EVR_UT:
        newElement = new DcmUnlimitedText( tag, length);
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
        if (tag == DCM_PixelData)
            newElement = new DcmPixelData(tag, length);
        else if (((tag.getGTag() & 0xffe1) == 0x6000)&&(tag.getETag() == 0x3000)) // DCM_OverlayData
            newElement = new DcmOverlayData(tag, length);
        else
            /* we don't know this element's real transfer syntax, so we just
             * use the defaults of class DcmOtherByteOtherWord and let the
             * application handle it.
             */
            newElement = new DcmOtherByteOtherWord(tag, length);
        break;

    case EVR_OB :
    case EVR_OW :
        if (tag == DCM_PixelData)
            newElement = new DcmPixelData(tag, length);
        else if (((tag.getGTag() & 0xffe1) == 0x6000)&&(tag.getETag() == 0x3000)) // DCM_OverlayData
            newElement = new DcmOverlayData(tag, length);
        else
            if (length == DCM_UndefinedLength) {
                // The attribute is OB or OW but is encoded with undefined
                // length.  Assume it is really a sequence so that we can
                // catch the sequence delimitation item.
                newElement = new DcmSequenceOfItems(tag, length);
            } else {
                newElement = new DcmOtherByteOtherWord(tag, length);
            }
        break;

        // Unbekannte Typen als Byte-String lesen:
    case EVR_UNKNOWN :
    case EVR_UNKNOWN2B :
    case EVR_UN :
    default :
        if (length == DCM_UndefinedLength) {
            // The attribute is unknown but is encoded with undefined
            // length.  Assume it is really a sequence so that we can
            // catch the sequence delimitation item.
            newElement = new DcmSequenceOfItems(tag, length);
        } else {
            newElement = new DcmOtherByteOtherWord(tag, length);
        }
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
** Simple tests for existance
*/
OFBool DcmItem::tagExists(const DcmTagKey& key, OFBool searchIntoSub)
{
    DcmStack stack;

    E_Condition ec = search(key, stack, ESM_fromHere, searchIntoSub);
    return (ec == EC_Normal);
}

OFBool DcmItem::tagExistsWithValue(const DcmTagKey& key, OFBool searchIntoSub)
{
    DcmElement *elem = NULL;
    Uint32 len = 0;
    DcmStack stack;

    E_Condition ec = search(key, stack, ESM_fromHere, searchIntoSub);
    elem = (DcmElement*) stack.top();
    if (ec == EC_Normal && elem != NULL) {
        len = elem->getLength();
    }

    return (ec == EC_Normal) && (len > 0);
}

/*
** simplified search&get functions
*/


#if 0
/* New find String Version can be used if every code is checked for
   normalization!
*/
E_Condition
DcmItem::findString(
    const DcmTagKey& xtag,
    char* aString, size_t maxStringLength,
    OFBool normalize, OFBool searchIntoSub)
{
    OFString str;
    E_Condition l_error = findOFStringArray(xtag, str, normalize, searchIntoSub);
    strncpy(aString, str.c_str(), maxStringLength);
    return l_error;

}
#endif

E_Condition
DcmItem::findString(const DcmTagKey& xtag,
                    char* aString, size_t maxStringLength,
                    OFBool searchIntoSub)
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
            ec = elem->getString(s);
            if (ec == EC_Normal) {
                strncpy(aString, s, maxStringLength);
            }
        }
    }

    return ec;
}


E_Condition
DcmItem::findOFStringArray(
    const DcmTagKey& xtag,
    OFString & aString,
    OFBool normalize, OFBool searchIntoSub)
{
    DcmElement *elem;
    DcmStack stack;
    E_Condition ec = EC_Normal;

    aString = "";
    ec = search(xtag, stack, ESM_fromHere, searchIntoSub);
    elem = (DcmElement*) stack.top();
    if (ec == EC_Normal && elem != NULL) {
        if (elem->getLength() != 0)
            ec = elem->getOFStringArray(aString, normalize);
    }

    return ec;
}

E_Condition
DcmItem::findOFString(
    const DcmTagKey& xtag,
    OFString & aString, const unsigned long which,
    OFBool normalize, OFBool searchIntoSub)
{
    DcmElement *elem;
    DcmStack stack;
    E_Condition ec = EC_Normal;

    aString = "";
    ec = search(xtag, stack, ESM_fromHere, searchIntoSub);
    elem = (DcmElement*) stack.top();
    if (ec == EC_Normal && elem != NULL) {
        if (elem->getLength() != 0) {
            ec = elem->getOFString(aString, which, normalize);
        }
    }

    return ec;
}

E_Condition
DcmItem::findIntegerNumber(
    const DcmTagKey& xtag,
    long& aLong, const unsigned long which,
    OFBool searchIntoSub)
{
    DcmElement *elem;
    DcmStack stack;
    E_Condition ec = EC_Normal;

    ec = search(xtag, stack, ESM_fromHere, searchIntoSub);
    elem = (DcmElement*) stack.top();
    if (ec == EC_Normal && elem != NULL) {
        switch (elem->ident()) {
        case EVR_UL:
        case EVR_up:
            Uint32 ul;
            ec = elem->getUint32(ul, which);
            aLong = ul;
            break;
        case EVR_SL:
            Sint32 sl;
            ec = elem->getSint32(sl, which);
            aLong = sl;
            break;
        case EVR_US:
        case EVR_xs:
            Uint16 us;
            ec = elem->getUint16(us, which);
            aLong = us;
            break;
        case EVR_SS:
            Sint16 ss;
            ec = elem->getSint16(ss, which);
            aLong = ss;
            break;
        case EVR_OB:
            // DcmOtherByteOtherWord does not implement getUint8
            Uint8* obPtr;
            ec = elem->getUint8Array(obPtr);
            if (obPtr && (ec==EC_Normal) && (which<elem->getLength())) {
                aLong = obPtr[which];
            } else {
                ec = EC_IllegalCall;
            }
            break;
        case EVR_OW:
        case EVR_ox:
            // DcmOtherByteOtherWord does not implement getUint16
            Uint16* owPtr;
            ec = elem->getUint16Array(owPtr);
            if (owPtr && (ec==EC_Normal) && (which<(elem->getLength()/2))) {
                aLong = owPtr[which];
            } else {
                ec = EC_IllegalCall;
            }
            break;
        default:
            ec = EC_IllegalCall;
            break;
        }
    }

    return ec;
}

E_Condition
DcmItem::findRealNumber(
    const DcmTagKey& xtag,
    double& aDouble, const unsigned long which,
    OFBool searchIntoSub)
{
    DcmElement *elem;
    DcmStack stack;
    E_Condition ec = EC_Normal;

    ec = search(xtag, stack, ESM_fromHere, searchIntoSub);
    elem = (DcmElement*) stack.top();
    if (ec == EC_Normal && elem != NULL) {
        switch (elem->ident()) {
        case EVR_FL:
            Float32 fl;
            ec = elem->getFloat32(fl, which);
            aDouble = fl;
            break;
        case EVR_FD:
            Float64 fd;
            ec = elem->getFloat64(fd, which);
            aDouble = fd;
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
** Revision 1.51  2000-03-08 16:26:36  meichel
** Updated copyright header.
**
** Revision 1.50  2000/03/03 15:02:09  joergr
** Corrected bug related to padding of file and item size.
**
** Revision 1.49  2000/03/03 14:05:34  meichel
** Implemented library support for redirecting error messages into memory
**   instead of printing them to stdout/stderr for GUI applications.
**
** Revision 1.48  2000/02/29 11:49:29  meichel
** Removed support for VS value representation. This was proposed in CP 101
**   but never became part of the standard.
**
** Revision 1.47  2000/02/23 15:11:55  meichel
** Corrected macro for Borland C++ Builder 4 workaround.
**
** Revision 1.46  2000/02/10 10:52:19  joergr
** Added new feature to dcmdump (enhanced print method of dcmdata): write
** pixel data/item value fields to raw files.
**
** Revision 1.45  2000/02/02 14:32:52  joergr
** Replaced 'delete' statements by 'delete[]' for objects created with 'new[]'.
**
** Revision 1.44  2000/02/01 10:12:07  meichel
** Avoiding to include <stdlib.h> as extern "C" on Borland C++ Builder 4,
**   workaround for bug in compiler header files.
**
** Revision 1.43  1999/03/31 09:25:30  meichel
** Updated copyright header in module dcmdata
**
** Revision 1.42  1999/03/22 15:55:52  meichel
** New handling of unknown (unsupported) VRs encountered when reading explicit
**   VR data. If the VR string consists of uppercase letters, we assume a
**   "future DICOM VR" and decode it expecting an extended length field
**   (4 bytes). Otherwise, we assume an illegal VR string created by some old
**   equipment (i.e.) "??" and decode without extended length (2 bytes).
**
** Revision 1.41  1998/07/15 15:51:59  joergr
** Removed several compiler warnings reported by gcc 2.8.1 with
** additional options, e.g. missing copy constructors and assignment
** operators, initialization of member variables in the body of a
** constructor instead of the member initialization list, hiding of
** methods by use of identical names, uninitialized member variables,
** missing const declaration of char pointers. Replaced tabs by spaces.
**
** Revision 1.40  1998/01/14 15:23:42  hewett
** Added support for the VRs UT (Unlimited Text) and VS (Virtual String).
**
** Revision 1.39  1998/01/14 09:13:53  meichel
** Corrected bug: Overlay Data elements in the groups
**   6002-601f were handled by DcmOtherByteOtherWord
**   instead of the "polymorphous" DcmOverlayData class.
**
** Revision 1.38  1998/01/14 08:42:32  meichel
** Improved algorithm for auto-detection of transfer syntax
**   used when opening a DICOM file without metaheader.
**   Big endian datasets are now detected much more reliably.
**
** Revision 1.37  1997/11/07 08:52:18  meichel
** Corrected bug in the dcmdata read routines which caused incorrect reading
**   of datasets containing attributes with value representation "ox" (= OB or OW)
**   in the dicom dictionary other than PixelData and OverlayData.
**   (thanks to Gilles Mevel <Gilles.Mevel@etiam.com> for the bug report and
**   sample data set).
**
** Revision 1.36  1997/09/22 14:50:53  hewett
** - Added 2 simple methods to test for the existance of an attribute
**   to DcmItem class (tagExists and tagExistsWithValue).  This code
**   was part of dcmgpdir.cc but is more generally useful.
** - Added 2 methods to find an attribute and retrieve numeric values
**   to DcmItem class (findIntegerNumber and findRealNumber).  The old
**   method findLong is now marked as obsolete and reimplemented using
**   findIntegerNumber.
**
** Revision 1.35  1997/09/12 13:44:53  meichel
** The algorithm introduced on 97.08.28 to detect incorrect odd-length
**   value fields falsely reported undefined length sequences and items
**   to be wrong. Corrected.
**
** Revision 1.34  1997/08/29 08:31:33  andreas
** - Added methods getOFString and getOFStringArray for all
**   string VRs. These methods are able to normalise the value, i. e.
**   to remove leading and trailing spaces. This will be done only if
**   it is described in the standard that these spaces are not relevant.
**   These methods do not test the strings for conformance, this means
**   especially that they do not delete spaces where they are not allowed!
**   getOFStringArray returns the string with all its parts separated by \
**   and getOFString returns only one value of the string.
**   CAUTION: Currently getString returns a string with trailing
**   spaces removed (if dcmEnableAutomaticInputDataCorrection == OFTrue) and
**   truncates the original string (since it is not copied!). If you rely on this
**   behaviour please change your application now.
**   Future changes will ensure that getString returns the original
**   string from the DICOM object (NULL terminated) inclusive padding.
**   Currently, if you call getOF... before calling getString without
**   normalisation, you can get the original string read from the DICOM object.
**
** Revision 1.33  1997/08/29 07:52:40  andreas
** - New error messages if length of an element is odd. Previously, no
**   error was reported. But the length is corrected by the method
**   newValueFiel and. so it was impossible for a checking utility to find
**   such an error in DICOM objects.
**
** Revision 1.32  1997/07/24 13:10:52  andreas
** - Removed Warnings from SUN CC 2.0.1
**
** Revision 1.31  1997/07/21 08:11:42  andreas
** - Support for CP 14. PixelData and OverlayData can have VR OW or OB
**   (depending on the transfer syntax). New internal value
**   representation (only for ident()) for OverlayData.
** - New environment for encapsulated pixel representations. DcmPixelData
**   can contain different representations and uses codecs to convert
**   between them. Codecs are derived from the DcmCodec class. New error
**   codes are introduced for handling of representations. New internal
**   value representation (only for ident()) for PixelData
** - Replace all boolean types (BOOLEAN, CTNBOOLEAN, DICOM_BOOL, BOOL)
**   with one unique boolean type OFBool.
**
** Revision 1.30  1997/07/07 07:43:59  andreas
** - Changed type for Tag attribute in DcmObject from prointer to value
** - Changed parameter type DcmTag & to DcmTagKey & in all search functions
**   in DcmItem, DcmSequenceOfItems, DcmDirectoryRecord and DcmObject
** - Enhanced (faster) byte swapping routine. swapIfNecessary moved from
**   a method in DcmObject to a general function.
**
** Revision 1.29  1997/07/03 15:09:59  andreas
** - removed debugging functions Bdebug() and Edebug() since
**   they write a static array and are not very useful at all.
**   Cdebug and Vdebug are merged since they have the same semantics.
**   The debugging functions in dcmdata changed their interfaces
**   (see dcmdata/include/dcdebug.h)
**
** Revision 1.28  1997/06/06 09:55:29  andreas
** - corrected error: canWriteXfer returns false if the old transfer syntax
**   was unknown, which causes several applications to prohibit the writing
**   of dataset.
**
** Revision 1.27  1997/05/27 13:49:00  andreas
** - Add method canWriteXfer to class DcmObject and all derived classes.
**   This method checks whether it is possible to convert the original
**   transfer syntax to an new transfer syntax. The check is used in the
**   dcmconv utility to prohibit the change of a compressed transfer
**   syntax to a uncompressed.
**
** Revision 1.26  1997/05/16 08:13:49  andreas
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
** - Deleted obsolete method DcmItem::calcHeaderLength because the
**   samce functionality is performed by DcmXfer::sizeofTagHeader
**
** Revision 1.25  1997/05/07 12:27:27  andreas
** Corrected error reading ItemDelimitationItem using explicit transfer syntaxes
**
** Revision 1.24  1997/04/30 16:32:50  andreas
** - Corrected Bug for reading of encapsulated pixel sequences
**
** Revision 1.23  1997/04/24 12:12:18  hewett
** Fixed DICOMDIR generation bug affecting the use of Unknown VR
** attributes (the file offsets were not being computed correctly).
**
** Revision 1.22  1997/04/18 08:10:49  andreas
** - Corrected debugging code
** - The put/get-methods for all VRs did not conform to the C++-Standard
**   draft. Some Compilers (e.g. SUN-C++ Compiler, Metroworks
**   CodeWarrier, etc.) create many warnings concerning the hiding of
**   overloaded get methods in all derived classes of DcmElement.
**   So the interface of all value representation classes in the
**   library are changed rapidly, e.g.
**   E_Condition get(Uint16 & value, const unsigned long pos);
**   becomes
**   E_Condition getUint16(Uint16 & value, const unsigned long pos);
**   All (retired) "returntype get(...)" methods are deleted.
**   For more information see dcmdata/include/dcelem.h
**
** Revision 1.21  1997/03/27 15:52:50  hewett
** Extended preliminary support for Unknown VR (UN) described in
** Supplement 14.  Attributes with undefined length should now
** be handled as a sequence.
**
** Revision 1.20  1997/03/26 17:15:57  hewett
** Added very preliminary support for Unknown VR (UN) described in
** Supplement 14.  WARNING: handling of unknown attributes with undefined
** length is not yet supported.
**
** Revision 1.19  1996/09/24 15:54:14  hewett
** Corrected erroneous setting of an error flag when inserting an
** attribute into an Item (via Item::insert(...)) and the attribute
** was already present.  Now the error flag is only set if replaceOld
** is OFFalse and an attribute already exists.
**
** Revision 1.18  1996/09/13 12:04:12  hewett
** Corrected missing () in function call (stack.card()) used in nextObject(...)
**
** Revision 1.17  1996/08/08 10:15:09  andreas
** Some more testing in nextObject
**
** Revision 1.16  1996/08/08 10:06:23  andreas
** Correct error for intoSub=OFFalse
**
** Revision 1.15  1996/08/05 08:46:12  andreas
** new print routine with additional parameters:
**         - print into files
**         - fix output length for elements
** corrected error in search routine with parameter ESM_fromStackTop
**
** Revision 1.14  1996/07/31 13:14:30  andreas
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
*/
