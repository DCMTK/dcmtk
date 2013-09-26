/*
 *
 *  Copyright (C) 1994-2013, OFFIS e.V.
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  This software and supporting documentation were developed by
 *
 *    OFFIS e.V.
 *    R&D Division Health
 *    Escherweg 2
 *    D-26121 Oldenburg, Germany
 *
 *
 *  Module:  dcmdata
 *
 *  Author:  Gerd Ehlers, Andreas Barth
 *
 *  Purpose: Implementation of class DcmPixelSequence
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#define INCLUDE_CSTDLIB
#define INCLUDE_CSTDIO
#include "dcmtk/ofstd/ofstdinc.h"

#include "dcmtk/ofstd/ofstream.h"
#include "dcmtk/ofstd/ofuuid.h"

#include "dcmtk/dcmdata/dcpixseq.h"
#include "dcmtk/dcmdata/dcpxitem.h"
#include "dcmtk/dcmdata/dcitem.h"
#include "dcmtk/dcmdata/dcvr.h"

#include "dcmtk/dcmdata/dcdeftag.h"


// ********************************


DcmPixelSequence::DcmPixelSequence(const DcmTag &tag,
                                   const Uint32 len)
  : DcmSequenceOfItems(tag, len),
    Xfer(EXS_Unknown)
{
    setTagVR(EVR_OB);
    setLengthField(DCM_UndefinedLength); // pixel sequences always use undefined length
}


DcmPixelSequence::DcmPixelSequence(const DcmPixelSequence &old)
  : DcmSequenceOfItems(old),
    Xfer(old.Xfer)
{
    /* everything gets handled in DcmSequenceOfItems constructor */
}


DcmPixelSequence::~DcmPixelSequence()
{
}


DcmPixelSequence &DcmPixelSequence::operator=(const DcmPixelSequence &obj)
{
  if (this != &obj)
  {
    DcmSequenceOfItems::operator=(obj);
    Xfer = obj.Xfer;
  }
  return *this;
}


OFCondition DcmPixelSequence::copyFrom(const DcmObject& rhs)
{
  if (this != &rhs)
  {
    if (rhs.ident() != ident()) return EC_IllegalCall;
    *this = OFstatic_cast(const DcmPixelSequence &, rhs);
  }
  return EC_Normal;
}

// ********************************


void DcmPixelSequence::print(STD_NAMESPACE ostream &out,
                             const size_t flags,
                             const int level,
                             const char *pixelFileName,
                             size_t *pixelCounter)
{
    /* print pixel sequence start line */
    if (flags & DCMTypes::PF_showTreeStructure)
    {
        /* empty text */
        printInfoLine(out, flags, level);
        /* print pixel sequence content */
        if (!itemList->empty())
        {
            /* print pixel items */
            DcmObject *dO;
            itemList->seek(ELP_first);
            do {
                dO = itemList->get();
                dO->print(out, flags, level + 1, pixelFileName, pixelCounter);
            } while (itemList->seek(ELP_next));
        }
    } else {
        OFOStringStream oss;
        oss << "(PixelSequence ";
        if (getLengthField() != DCM_UndefinedLength)
            oss << "with explicit length ";
        oss << "#=" << card() << ")" << OFStringStream_ends;
        OFSTRINGSTREAM_GETSTR(oss, tmpString)
        printInfoLine(out, flags, level, tmpString);
        OFSTRINGSTREAM_FREESTR(tmpString)
        /* print pixel sequence content */
        if (!itemList->empty())
        {
            DcmObject *dO;
            itemList->seek(ELP_first);
            do {
                dO = itemList->get();
                dO->print(out, flags, level + 1, pixelFileName, pixelCounter);
            } while (itemList->seek(ELP_next));
        }
        /* print pixel sequence end line */
        DcmTag delimItemTag(DCM_SequenceDelimitationItem);
        if (getLengthField() == DCM_UndefinedLength)
            printInfoLine(out, flags, level, "(SequenceDelimitationItem)", &delimItemTag);
        else
            printInfoLine(out, flags, level, "(SequenceDelimitationItem for re-encod.)", &delimItemTag);
    }
}


// ********************************


OFCondition DcmPixelSequence::writeXML(STD_NAMESPACE ostream &out,
                                       const size_t flags)
{
    OFCondition l_error = EC_Normal;
    if (flags & DCMTypes::XF_useNativeModel)
    {
        /* write XML start tag */
        writeXMLStartTag(out, flags);
        /* for an empty value field, we do not need to do anything */
        if (getLengthField() > 0)
        {
            /* encode binary data as Base64 */
            if (flags & DCMTypes::XF_encodeBase64)
            {
                out << "<InlineBinary>";
                Uint8 *byteValues = OFstatic_cast(Uint8 *, getValue());
                OFStandard::encodeBase64(out, byteValues, OFstatic_cast(size_t, getLengthField()));
                out << "</InlineBinary>" << OFendl;
            } else {
                /* generate a new UID but the binary data is not (yet) written. */
                OFUUID uuid;
                out << "<BulkData uuid=\"";
                uuid.print(out, OFUUID::ER_RepresentationHex);
                out << "\"/>" << OFendl;
            }
        }
        /* write XML end tag */
        writeXMLEndTag(out, flags);
    } else {
        /* the DCMTK-specific XML format requires no special handling */
        l_error = DcmSequenceOfItems::writeXML(out, flags);
    }
    return l_error;
}


// ********************************


Uint32 DcmPixelSequence::calcElementLength(const E_TransferSyntax xfer,
                                           const E_EncodingType enctype)
{
    // add 8 bytes for Sequence Delimitation Tag which always exists for Pixel Sequences
    return DcmElement::calcElementLength(xfer, enctype) + 8;
}


// ********************************


OFCondition DcmPixelSequence::makeSubObject(DcmObject *&subObject,
                                            const DcmTag &newTag,
                                            const Uint32 newLength)
{
    OFCondition l_error = EC_Normal;
    DcmObject *newObject = NULL;

    switch (newTag.getEVR())
    {
        case EVR_na:
            if (newTag.getXTag() == DCM_Item)
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


OFCondition DcmPixelSequence::insert(DcmPixelItem *item,
                                     unsigned long where)
{
    errorFlag = EC_Normal;
    if (item != NULL)
    {
        itemList->seek_to(where);
        itemList->insert(item);
        if (where < itemList->card())
            DCMDATA_TRACE("DcmPixelSequence::insert() Item at position " << where << " inserted");
        if (where >= itemList->card())
            DCMDATA_TRACE("DcmPixelSequence::insert() Item at last position inserted");
        // check whether the new item already has a parent
        if (item->getParent() != NULL)
        {
            DCMDATA_DEBUG("DcmPixelSequence::insert() PixelItem already has a parent: "
                << item->getParent()->getTag() << " VR=" << DcmVR(item->getParent()->getVR()).getVRName());
        }
        // remember the parent (i.e. the surrounding sequence)
        item->setParent(this);
    } else
        errorFlag = EC_IllegalCall;
    return errorFlag;
}


// ********************************


OFCondition DcmPixelSequence::getItem(DcmPixelItem *&item,
                                      const unsigned long num)
{
    errorFlag = EC_Normal;
    item = OFstatic_cast(DcmPixelItem*, itemList->seek_to(num));  // read item from list
    if (item == NULL)
        errorFlag = EC_IllegalCall;
    return errorFlag;
}


// ********************************


OFCondition DcmPixelSequence::remove(DcmPixelItem *&item,
                                     const unsigned long num)
{
    errorFlag = EC_Normal;
    item = OFstatic_cast(DcmPixelItem*, itemList->seek_to(num));  // read item from list
    if (item != NULL)
    {
        itemList->remove();
        item->setParent(NULL);          // forget about the parent
    } else
        errorFlag = EC_IllegalCall;
    return errorFlag;
}


// ********************************


OFCondition DcmPixelSequence::remove(DcmPixelItem *item)
{
    errorFlag = EC_IllegalCall;
    if (!itemList->empty() && item != NULL)
    {
        DcmObject *dO;
        itemList->seek(ELP_first);
        do {
            dO = itemList->get();
            if (dO == item)
            {
                itemList->remove();         // remove element from list, but do no delete it
                item->setParent(NULL);      // forget about the parent
                errorFlag = EC_Normal;
                break;
            }
        } while (itemList->seek(ELP_next));
    }
    return errorFlag;
}


// ********************************


OFCondition DcmPixelSequence::changeXfer(const E_TransferSyntax newXfer)
{
    if (Xfer == EXS_Unknown || canWriteXfer(newXfer, Xfer))
    {
        Xfer = newXfer;
        return EC_Normal;
    } else
        return EC_IllegalCall;
}


// ********************************


OFBool DcmPixelSequence::canWriteXfer(const E_TransferSyntax newXfer,
                                      const E_TransferSyntax oldXfer)
{
    DcmXfer newXferSyn(newXfer);

    return newXferSyn.isEncapsulated() && newXfer == oldXfer && oldXfer == Xfer;
}


// ********************************


OFCondition DcmPixelSequence::read(DcmInputStream &inStream,
                                   const E_TransferSyntax ixfer,
                                   const E_GrpLenEncoding glenc,
                                   const Uint32 maxReadLength)
{
    OFCondition l_error = changeXfer(ixfer);
    if (l_error.good())
        return DcmSequenceOfItems::read(inStream, ixfer, glenc, maxReadLength);

    return l_error;
}


// ********************************


OFCondition DcmPixelSequence::write(DcmOutputStream &outStream,
                                    const E_TransferSyntax oxfer,
                                    const E_EncodingType /*enctype*/,
                                    DcmWriteCache *wcache)
{
    OFCondition l_error = changeXfer(oxfer);
    if (l_error.good())
        return DcmSequenceOfItems::write(outStream, oxfer, EET_UndefinedLength, wcache);

    return l_error;
}


// ********************************


OFCondition DcmPixelSequence::writeSignatureFormat(DcmOutputStream &outStream,
                                                   const E_TransferSyntax oxfer,
                                                   const E_EncodingType /*enctype*/,
                                                   DcmWriteCache *wcache)
{
    OFCondition l_error = changeXfer(oxfer);
    if (l_error.good())
        return DcmSequenceOfItems::writeSignatureFormat(outStream, oxfer, EET_UndefinedLength, wcache);

    return l_error;
}


OFCondition DcmPixelSequence::storeCompressedFrame(DcmOffsetList &offsetList,
                                                   Uint8 *compressedData,
                                                   Uint32 compressedLen,
                                                   Uint32 fragmentSize)
{
    if (compressedData == NULL)
        return EC_IllegalCall;

    OFCondition result = EC_Normal;
    if (fragmentSize >= 0x400000)
        fragmentSize = 0;    // prevent overflow
    else
        fragmentSize <<= 10; // unit is kbytes
    if (fragmentSize == 0)
        fragmentSize = compressedLen;

    Uint32 offset = 0;
    Uint32 currentSize = 0;
    Uint32 numFragments = 0;
    DcmPixelItem *fragment = NULL;

    while ((offset < compressedLen) && (result.good()))
    {
        fragment = new DcmPixelItem(DcmTag(DCM_Item, EVR_OB));
        if (fragment == NULL)
            result = EC_MemoryExhausted;
        else
        {
            insert(fragment);
            numFragments++;
            currentSize = fragmentSize;
            if (offset + currentSize > compressedLen)
                currentSize = compressedLen - offset;
            // if currentSize is odd this will be fixed during DcmOtherByteOtherWord::write()
            result = fragment->putUint8Array(compressedData + offset, currentSize);
            if (result.good())
                offset += currentSize;
        }
    }

    currentSize = offset + (numFragments << 3); // 8 bytes extra for each item header
    // odd frame size requires padding, i.e. last fragment uses odd length pixel item
    if (currentSize & 1)
        currentSize++;
    offsetList.push_back(currentSize);
    return result;
}
