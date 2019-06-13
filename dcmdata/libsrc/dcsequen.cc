/*
 *
 *  Copyright (C) 1994-2019, OFFIS e.V.
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
 *  Purpose: Implementation of class DcmSequenceOfItems
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#define INCLUDE_CSTDLIB
#define INCLUDE_CSTDIO
#include "dcmtk/ofstd/ofstdinc.h"

#include "dcmtk/ofstd/ofstream.h"
#include "dcmtk/ofstd/ofstd.h"
#include "dcmtk/ofstd/ofcast.h"

#include "dcmtk/dcmdata/dcjson.h"
#include "dcmtk/dcmdata/dcsequen.h"
#include "dcmtk/dcmdata/dcitem.h"
#include "dcmtk/dcmdata/dcdirrec.h"
#include "dcmtk/dcmdata/dcvr.h"
#include "dcmtk/dcmdata/dcpxitem.h"
#include "dcmtk/dcmdata/dcswap.h"
#include "dcmtk/dcmdata/dcmetinf.h"
#include "dcmtk/dcmdata/dcdeftag.h"
#include "dcmtk/dcmdata/dcistrma.h"    /* for class DcmInputStream */
#include "dcmtk/dcmdata/dcostrma.h"    /* for class DcmOutputStream */


DcmSequenceOfItems::DcmSequenceOfItems(const DcmTag &tag)
: DcmElement(tag, 0),
  itemList(new DcmList),
  lastItemComplete(OFTrue),
  fStartPosition(0),
  readAsUN_(OFFalse)
{
}


// ********************************


DcmSequenceOfItems::DcmSequenceOfItems(
  const DcmTag &tag,
  const Uint32 len,
  OFBool readAsUN)
: DcmElement(tag, len),
  itemList(new DcmList),
  lastItemComplete(OFTrue),
  fStartPosition(0),
  readAsUN_(readAsUN)
{
}


// ********************************


DcmSequenceOfItems::DcmSequenceOfItems(const DcmSequenceOfItems &old)
  : DcmElement(old),
    itemList(new DcmList),
    lastItemComplete(old.lastItemComplete),
    fStartPosition(old.fStartPosition),
    readAsUN_(old.readAsUN_)
{
    if (!old.itemList->empty())
    {
        itemList->seek(ELP_first);
        old.itemList->seek(ELP_first);
        do
        {
            DcmObject *dO = old.itemList->get()->clone();
            itemList->insert(dO, ELP_next);
            // remember the parent
            dO->setParent(this);
        } while (old.itemList->seek(ELP_next));
    }
}


// ********************************


DcmSequenceOfItems::~DcmSequenceOfItems()
{
    itemList->deleteAllElements();
    delete itemList;
}


// ********************************


DcmSequenceOfItems &DcmSequenceOfItems::operator=(const DcmSequenceOfItems &obj)
{
  if (this != &obj)
  {
    DcmElement::operator=(obj);
    lastItemComplete = obj.lastItemComplete;
    fStartPosition = obj.fStartPosition;
    readAsUN_ = obj.readAsUN_;

    // DcmList has no copy constructor. Need to copy ourselves.
    DcmList *newList = new DcmList;
    if (newList)
    {
        switch (obj.ident())
        {
            case EVR_SQ:
            case EVR_pixelSQ:
            case EVR_fileFormat:
                if (!obj.itemList->empty())
                {
                    DcmObject *oldDO;
                    DcmObject *newDO;
                    newList->seek(ELP_first);
                    obj.itemList->seek(ELP_first);
                    do {
                        oldDO = obj.itemList->get();
                        switch (oldDO->ident())
                        {
                            case EVR_item:
                                newDO = new DcmItem(*OFstatic_cast(DcmItem *, oldDO));
                                break;
                            case EVR_pixelItem:
                                newDO = new DcmPixelItem(*OFstatic_cast(DcmPixelItem *, oldDO));
                                break;
                            case EVR_metainfo:
                                newDO = new DcmMetaInfo(*OFstatic_cast(DcmMetaInfo *, oldDO));
                                break;
                            case EVR_dataset:
                                newDO = new DcmDataset(*OFstatic_cast(DcmDataset *, oldDO));
                                break;
                            default:
                                newDO = new DcmItem(oldDO->getTag());
                                DCMDATA_WARN("DcmSequenceOfItems: Non-item element " << oldDO->getTag() << " found");
                                break;
                        }
                        newList->insert(newDO, ELP_next);
                        // remember the parent
                        newDO->setParent(this);
                    } while (obj.itemList->seek(ELP_next));
                }
                break;
            default:
                // wrong use of assignment operator, should never happen
                break;
        }
    }
    // be sure to clear memory of former elements not in use any more...
    itemList->deleteAllElements();
    // ...and delete the list itself
    delete itemList;
    itemList = newList;
  }
  return *this;
}


// ********************************


OFCondition DcmSequenceOfItems::copyFrom(const DcmObject& rhs)
{
    if (this != &rhs)
    {
        if (rhs.ident() != ident()) return EC_IllegalCall;
        *this = OFstatic_cast(const DcmSequenceOfItems &, rhs);
    }
    return EC_Normal;
}


// ********************************


int DcmSequenceOfItems::compare(const DcmElement& rhs) const
{
    int result = DcmElement::compare(rhs);
    if (result != 0)
    {
        return result;
    }

    /* cast away constness (dcmdata is not const correct...) */
    DcmSequenceOfItems* myThis = NULL;
    DcmSequenceOfItems* myRhs = NULL;
    myThis = OFconst_cast(DcmSequenceOfItems*, this);
    myRhs = OFstatic_cast(DcmSequenceOfItems*, OFconst_cast(DcmElement*, &rhs));

    /* check number of items */
    unsigned long rhsNumItems = myRhs->card();
    unsigned long thisNumItems = myThis->card();
    if (thisNumItems < rhsNumItems)
    {
        return -1;
    }
    else if (thisNumItems > rhsNumItems)
    {
        return 1;
    }

    /* iterate over all items and test equality */
    for (unsigned long count = 0; count < thisNumItems; count++)
    {
        DcmItem* val = myThis->getItem(count);
        if (val)
        {
            DcmItem* rhsVal = myRhs->getItem(count);
            if (rhsVal)
            {
                result = val->compare(*rhsVal);
                if (result != 0)
                {
                    return result;
                }
            }
        }
    }

    /* all values as well as VM equal: objects are equal */
    return 0;
}


// ********************************


OFCondition DcmSequenceOfItems::checkValue(const OFString &cardinality,
                                           const OFBool /*oldFormat*/)
{
    /* check cardinality (number of items) only */
    return DcmElement::checkVM(this->card(), cardinality);
}


unsigned long  DcmSequenceOfItems::getVM()
{
    return 1;
}


unsigned long DcmSequenceOfItems::getNumberOfValues()
{
    return itemList->card();
}


unsigned long DcmSequenceOfItems::card() const
{
    return itemList->card();
}


// ********************************


void DcmSequenceOfItems::print(STD_NAMESPACE ostream &out,
                               const size_t flags,
                               const int level,
                               const char *pixelFileName,
                               size_t *pixelCounter)
{
    /* print sequence start line */
    if (flags & DCMTypes::PF_showTreeStructure)
    {
        /* empty text */
        printInfoLine(out, flags, level);
        /* print sequence content */
        if (!itemList->empty())
        {
            /* print all items contained in the sequence */
            DcmObject *dO;
            itemList->seek(ELP_first);
            do {
                dO = itemList->get();
                dO->print(out, flags, level + 1, pixelFileName, pixelCounter);
            } while (itemList->seek(ELP_next));
        }
    } else {
        OFOStringStream oss;
        oss << "(Sequence with ";
        if (getLengthField() == DCM_UndefinedLength)
            oss << "undefined";
        else
            oss << "explicit";
        oss << " length #=" << card() << ")" << OFStringStream_ends;
        OFSTRINGSTREAM_GETSTR(oss, tmpString)
        printInfoLine(out, flags, level, tmpString);
        OFSTRINGSTREAM_FREESTR(tmpString)
        /* print sequence content */
        if (!itemList->empty())
        {
            DcmObject *dO;
            itemList->seek(ELP_first);
            do {
                dO = itemList->get();
                dO->print(out, flags, level + 1, pixelFileName, pixelCounter);
            } while (itemList->seek(ELP_next));
        }
        /* print sequence end line */
        DcmTag delimItemTag(DCM_SequenceDelimitationItemTag);
        if (getLengthField() == DCM_UndefinedLength)
            printInfoLine(out, flags, level, "(SequenceDelimitationItem)", &delimItemTag);
        else
            printInfoLine(out, flags, level, "(SequenceDelimitationItem for re-encod.)", &delimItemTag);
    }
}


// ********************************


OFCondition DcmSequenceOfItems::writeXML(STD_NAMESPACE ostream &out,
                                         const size_t flags)
{
    OFCondition l_error = EC_Normal;
    if (flags & DCMTypes::XF_useNativeModel)
    {
        /* use common method from DcmElement to write start tag */
        DcmElement::writeXMLStartTag(out, flags);
        /* write sequence content */
        if (!itemList->empty())
        {
            unsigned long itemNo = 1;
            /* write content of all children */
            DcmObject *dO;
            itemList->seek(ELP_first);
            do
            {
                out << "<Item number=\"" << (itemNo++) << "\">" << OFendl;
                dO = itemList->get();
                l_error = dO->writeXML(out, flags);
                /* exit loop in case of error */
                if (l_error.bad()) break;
                out << "</Item>" << OFendl;
            } while (itemList->seek(ELP_next));
        }
        if (l_error.good())
        {
            /* use common method from DcmElement to write end tag */
            DcmElement::writeXMLEndTag(out, flags);
        }
    } else {
        OFString xmlString;
        DcmVR vr(getTag().getVR());
        /* XML start tag for "sequence" */
        out << "<sequence";
        /* attribute tag = (gggg,eeee) */
        out << " tag=\"";
        out << STD_NAMESPACE hex << STD_NAMESPACE setfill('0')
            << STD_NAMESPACE setw(4) << getTag().getGTag() << ","
            << STD_NAMESPACE setw(4) << getTag().getETag() << "\""
            << STD_NAMESPACE dec << STD_NAMESPACE setfill(' ');
        /* value representation = VR */
        out << " vr=\"" << vr.getValidVRName() << "\"";
        /* cardinality (number of items) = 1..n */
        out << " card=\"" << card() << "\"";
        /* value length in bytes = 0..max (if not undefined) */
        if (getLengthField() != DCM_UndefinedLength)
            out << " len=\"" << getLengthField() << "\"";
        /* tag name (if known and not suppressed) */
        if (!(flags & DCMTypes::XF_omitDataElementName))
            out << " name=\"" << OFStandard::convertToMarkupString(getTagName(), xmlString) << "\"";
        out << ">" << OFendl;
        /* write sequence content */
        if (!itemList->empty())
        {
            /* write content of all children */
            DcmObject *dO;
            itemList->seek(ELP_first);
            do
            {
                dO = itemList->get();
                l_error = dO->writeXML(out, flags);
            } while (l_error.good() && itemList->seek(ELP_next));
        }
        if (l_error.good())
        {
            /* XML end tag for "sequence" */
            out << "</sequence>" << OFendl;
        }
    }
    return l_error;
}


// ********************************


OFCondition DcmSequenceOfItems::writeJson(STD_NAMESPACE ostream& out,
                                          DcmJsonFormat &format)
{
    // use common method from DcmElement to write opener
    DcmElement::writeJsonOpener(out, format);
    OFCondition status = EC_Normal;
    // write sequence content
    if (!itemList->empty())
    {
        format.printValuePrefix(out);
        itemList->seek(ELP_first);
        status = itemList->get()->writeJson(out, format);
        while (status.good() && itemList->seek(ELP_next))
        {
            format.printNextArrayElementPrefix(out);
            status = itemList->get()->writeJson(out, format);
        }
        format.printValueSuffix(out);
    }
    DcmElement::writeJsonCloser(out, format);
    return status;
}


// ********************************


OFBool DcmSequenceOfItems::canWriteXfer(const E_TransferSyntax newXfer,
                                        const E_TransferSyntax oldXfer)
{
    OFBool canWrite = OFTrue;

    if (newXfer == EXS_Unknown)
        canWrite = OFFalse;
    else if (!itemList->empty())
    {
        DcmObject *dO;
        itemList->seek(ELP_first);
        do
        {
            dO = itemList->get();
            canWrite = dO -> canWriteXfer(newXfer, oldXfer);
        } while (itemList->seek(ELP_next) && canWrite);
    }

    return canWrite;
}


// ********************************


Uint32 DcmSequenceOfItems::calcElementLength(const E_TransferSyntax xfer,
                                             const E_EncodingType enctype)
{
    /* Get length of sequence header + sequence content (will call DcmSequenceOfItems::getLength()) */
    Uint32 seqlen = DcmElement::calcElementLength(xfer, enctype);
    if (seqlen == DCM_UndefinedLength)
        return DCM_UndefinedLength;
    if (enctype == EET_UndefinedLength)
    {
        if (OFStandard::check32BitAddOverflow(seqlen, 8))
            return DCM_UndefinedLength;
        else
            seqlen += 8;     // for Sequence Delimitation Tag
    }
    return seqlen;
}


// ********************************


Uint32 DcmSequenceOfItems::getLength(const E_TransferSyntax xfer,
                                     const E_EncodingType enctype)
{
    Uint32 seqlen = 0;
    Uint32 sublen = 0;
    if (!itemList->empty())
    {
        DcmItem *dI;
        itemList->seek(ELP_first);
        do {
            dI = OFstatic_cast(DcmItem *, itemList->get());
            sublen = dI->calcElementLength(xfer, enctype);
            /* explicit length: be sure that total size of contained elements fits into sequence's
               32 Bit length field. If not, switch encoding automatically to undefined
               length for this sequence. Nevertheless, any contained items will be
               written with explicit length if possible.
             */
            if ( (enctype == EET_ExplicitLength) && OFStandard::check32BitAddOverflow(seqlen, sublen) )
            {
                if (dcmWriteOversizedSeqsAndItemsUndefined.get())
                {
                    DCMDATA_WARN("DcmSequenceOfItems: Explicit length of sequence " << getTagName() << " " << getTag()
                        << " exceeds 32-bit length field, trying to treat it as undefined length instead");
                }
                else
                {
                    DCMDATA_WARN("DcmSequenceOfItems: Explicit length of sequence " << getTagName() << " " << getTag()
                        << " exceeds 32-bit length field, writing with explicit length will not be possible");
                    errorFlag = EC_SeqOrItemContentOverflow;
                }
                return DCM_UndefinedLength;
            }
            seqlen += sublen;
        } while (itemList->seek(ELP_next));
    }
    return seqlen;
}


// ********************************


OFCondition DcmSequenceOfItems::computeGroupLengthAndPadding(const E_GrpLenEncoding glenc,
                                                             const E_PaddingEncoding padenc,
                                                             const E_TransferSyntax xfer,
                                                             const E_EncodingType enctype,
                                                             const Uint32 padlen,
                                                             const Uint32 subPadlen,
                                                             Uint32 instanceLength)
{
    OFCondition l_error = EC_Normal;

    if (!itemList->empty())
    {
        itemList->seek(ELP_first);
        do {
            DcmItem *dO = OFstatic_cast(DcmItem *, itemList->get());
            l_error = dO->computeGroupLengthAndPadding
                (glenc, padenc, xfer, enctype, padlen, subPadlen, instanceLength);
        } while (l_error.good() && itemList->seek(ELP_next));
    }
    return l_error;
}


// ********************************


OFCondition DcmSequenceOfItems::makeSubObject(DcmObject *&subObject,
                                              const DcmTag &newTag,
                                              const Uint32 newLength)
{
    OFCondition l_error = EC_Normal;
    DcmItem *subItem = NULL;

    switch (newTag.getEVR())
    {
        case EVR_na:
            if (newTag.getXTag() == DCM_Item)
            {
                if (getTag().getXTag() == DCM_DirectoryRecordSequence)
                    subItem = new DcmDirectoryRecord(newTag, newLength);
                else
                    subItem = new DcmItem(newTag, newLength);
            }
            else if (newTag.getXTag() == DCM_SequenceDelimitationItem)
                l_error = EC_SequEnd;
            else if (newTag.getXTag() == DCM_ItemDelimitationItem)
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


OFCondition DcmSequenceOfItems::readTagAndLength(DcmInputStream &inStream,
                                                 const E_TransferSyntax xfer,
                                                 DcmTag &tag,
                                                 Uint32 &length)
{
    Uint16 groupTag = 0xffff;
    Uint16 elementTag = 0xffff;

    OFCondition l_error = EC_Normal;
    if (inStream.avail() < 8)
        l_error = EC_StreamNotifyClient;

    if (l_error.good())
    {
        DcmXfer iXfer(xfer);
        const E_ByteOrder iByteOrder = iXfer.getByteOrder();
        if (iByteOrder == EBO_unknown)
            return EC_IllegalCall;
        inStream.mark();
        inStream.read(&groupTag, 2);
        inStream.read(&elementTag, 2);
        swapIfNecessary(gLocalByteOrder, iByteOrder, &groupTag, 2, 2);
        swapIfNecessary(gLocalByteOrder, iByteOrder, &elementTag, 2, 2);
        // tag has been read

        DcmTag newTag(groupTag, elementTag);

        Uint32 valueLength = 0;
        inStream.read(&valueLength, 4);
        swapIfNecessary(gLocalByteOrder, iByteOrder, &valueLength, 4, 4);
        if ((valueLength & 1) && (valueLength != DCM_UndefinedLength))
        {
            DCMDATA_WARN("DcmSequenceOfItems: Length of item in sequence "
                << getTagName() << " " << getTag() << " is odd");
        }
        length = valueLength;
        tag = newTag; // return value: assignment-operator
    }

    if (l_error.bad())
        DCMDATA_TRACE("DcmSequenceOfItems::readTagAndLength() returns error = " << l_error.text());
    return l_error;
}


// ********************************


OFCondition DcmSequenceOfItems::readSubItem(DcmInputStream &inStream,
                                            const DcmTag &newTag,
                                            const Uint32 newLength,
                                            const E_TransferSyntax xfer,
                                            const E_GrpLenEncoding glenc,
                                            const Uint32 maxReadLength)
{
    // For DcmSequenceOfItems, subObject is always inherited from DcmItem.
    // For DcmPixelSequence, subObject is always inherited from DcmPixelItem.
    DcmObject *subObject = NULL;
    OFCondition l_error = makeSubObject(subObject, newTag, newLength);
    if (l_error.good() && (subObject != NULL))
    {
        // inStream.UnsetPutbackMark(); // not needed anymore with new stream architecture
        itemList->insert(subObject, ELP_next);
        // dump some information if required
        DCMDATA_TRACE("DcmSequenceOfItems::readSubItem() Sub Item " << newTag << " inserted");
        // remember the parent (i.e. the surrounding sequence)
        subObject->setParent(this);
        // read sub-item
        l_error = subObject->read(inStream, xfer, glenc, maxReadLength);
        // prevent subObject from getting deleted
        return l_error;
    }
    else if (l_error == EC_InvalidTag)  // try to recover parsing
    {
        inStream.putback();
        // dump some information if required
        DCMDATA_WARN("DcmSequenceOfItems: Parse error in sequence " << getTag() << ", found "
            << newTag << " instead of item tag " << DCM_Item);
    }
    else if (l_error != EC_SequEnd)
    {
        // dump some information if required
        if (dcmIgnoreParsingErrors.get() || (dcmReplaceWrongDelimitationItem.get() && (l_error == EC_ItemEnd)))
        {
            DCMDATA_WARN("DcmSequenceOfItems: Parse error in sequence " << getTag() << ", found "
                << newTag << " instead of sequence delimiter " << DCM_SequenceDelimitationItem);
        } else {
            DCMDATA_ERROR("DcmSequenceOfItems: Parse error in sequence "  << getTag() << ", found "
                << newTag << " instead of sequence delimiter " << DCM_SequenceDelimitationItem);
        }
        // some systems use the wrong delimitation item at the end of a sequence
        if (dcmReplaceWrongDelimitationItem.get() && (l_error == EC_ItemEnd))
        {
            DCMDATA_DEBUG("DcmSequenceOfItems::readSubItem() replacing wrong item delimiter "
                << DCM_ItemDelimitationItem << " by sequence delimiter "
                << DCM_SequenceDelimitationItem << " because it is expected here");
            l_error = EC_SequEnd;
        } else {
            DCMDATA_DEBUG("DcmSequenceOfItems::readSubItem() cannot create Sub Item " << newTag);
            // treat this incorrect encoding as an error
            if (!dcmIgnoreParsingErrors.get())
                l_error = EC_SequDelimitationItemMissing;
        }
    } else {
        // inStream.UnsetPutbackMark(); // not needed anymore with new stream architecture
    }

    if (subObject)
        delete subObject; // only executed if makeSubObject() has returned an error
    // dump some information if required
    DCMDATA_TRACE("DcmSequenceOfItems::readSubItem() returns error = " << l_error.text());
    return l_error;
}


// ********************************


OFCondition DcmSequenceOfItems::read(DcmInputStream &inStream,
                                     const E_TransferSyntax xfer,
                                     const E_GrpLenEncoding glenc,
                                     const Uint32 maxReadLength)
{
    if (getTransferState() == ERW_notInitialized)
        errorFlag = EC_IllegalCall;
    else
    {
        errorFlag = inStream.status();

        if (errorFlag.good() && inStream.eos())
            errorFlag = EC_EndOfStream;
        else if (errorFlag.good() && (getTransferState() != ERW_ready))
        {
            if (getTransferState() == ERW_init)
            {
                fStartPosition = inStream.tell();   // Position Sequence-Value
                setTransferState(ERW_inWork);
            }

            E_TransferSyntax readxfer = readAsUN_ ? EXS_LittleEndianImplicit : xfer;

            itemList->seek(ELP_last); // append data at end
            while (inStream.good() && ((getTransferredBytes() < getLengthField()) || !lastItemComplete))
            {
                DcmTag newTag;
                Uint32 newValueLength = 0;

                if (lastItemComplete)
                {
                    if (inStream.eos())
                    {
                        DCMDATA_WARN("DcmSequenceOfItems: Reached end of stream before the end of sequence "
                                << getTagName() << " " << getTag());
                        if (dcmIgnoreParsingErrors.get())
                        {
                            /* "Invent" a SequenceDelimitationItem.
                             * This will be turned into EC_Normal below. */
                            errorFlag = EC_SequEnd;
                        }
                        else
                            errorFlag = EC_SequDelimitationItemMissing;
                        break;
                    }

                    errorFlag = readTagAndLength(inStream, readxfer, newTag, newValueLength);

                    if (errorFlag.bad())
                        break;                  // finish while loop
                    else
                        incTransferredBytes(8);

                    lastItemComplete = OFFalse;
                    errorFlag = readSubItem(inStream, newTag, newValueLength, readxfer, glenc, maxReadLength);
                    if (errorFlag.good())
                        lastItemComplete = OFTrue;
                }
                else
                {
                    errorFlag = itemList->get()->read(inStream, readxfer, glenc, maxReadLength);
                    if (errorFlag.good())
                        lastItemComplete = OFTrue;
                }
                setTransferredBytes(OFstatic_cast(Uint32, inStream.tell() - fStartPosition));

                if (errorFlag.bad())
                    break;

            } //while
            if (((getTransferredBytes() < getLengthField()) || !lastItemComplete) && errorFlag.good())
                errorFlag = EC_StreamNotifyClient;
        } // else errorFlag

        if (errorFlag == EC_SequEnd)
            errorFlag = EC_Normal;
        if (errorFlag.good())
            setTransferState(ERW_ready);      // sequence is complete
    }
    // dump information if required
    DCMDATA_TRACE("DcmSequenceOfItems::read() returns error = " << errorFlag.text());
    return errorFlag;
}


// ********************************

OFCondition DcmSequenceOfItems::write(DcmOutputStream &outStream,
                                      const E_TransferSyntax oxfer,
                                      const E_EncodingType enctype,
                                      DcmWriteCache *wcache)
{
  if (getTransferState() == ERW_notInitialized)
        errorFlag = EC_IllegalCall;
    else
    {
        errorFlag = outStream.status();
        if (errorFlag.good() && (getTransferState() != ERW_ready))
        {
            if (getTransferState() == ERW_init)
            {
                /* first compare with DCM_TagInfoLength (12). If there is not enough space
                 * in the buffer, check if the buffer is still sufficient for the requirements
                 * of this element, which may need only 8 instead of 12 bytes.
                 */
                if ((outStream.avail() >= OFstatic_cast(offile_off_t, DCM_TagInfoLength)) ||
                    (outStream.avail() >= OFstatic_cast(offile_off_t, getTagAndLengthSize(oxfer))))
                {
                    if (enctype == EET_ExplicitLength)
                        setLengthField(getLength(oxfer, enctype));
                    else
                        setLengthField(DCM_UndefinedLength);
                    if (errorFlag == EC_SeqOrItemContentOverflow)
                      return EC_SeqOrItemContentOverflow;
                    Uint32 written_bytes = 0;
                    errorFlag = writeTagAndLength(outStream, oxfer, written_bytes);
                    if (errorFlag.good())
                    {
                        setTransferState(ERW_inWork);
                        itemList->seek(ELP_first);
                    }
                } else
                    errorFlag = EC_StreamNotifyClient;
            }
            if (getTransferState() == ERW_inWork)
            {
                // itemList->get() can be NULL if buffer was full after
                // writing the last item but before writing the sequence delimitation.
                if (!itemList->empty() && (itemList->get() != NULL))
                {
                    DcmObject *dO;
                    do
                    {
                      dO = itemList->get();
                      if (dO->transferState() != ERW_ready)
                          errorFlag = dO->write(outStream, oxfer, enctype, wcache);
                    } while (errorFlag.good() && itemList->seek(ELP_next));
                }
                if (errorFlag.good())
                {
                    setTransferState(ERW_ready);
                    if (getLengthField() == DCM_UndefinedLength)
                    {
                        if (outStream.avail() >= 8)
                        {
                            // write sequence delimitation item
                            const DcmTag delim(DCM_SequenceDelimitationItemTag);
                            errorFlag = writeTag(outStream, delim, oxfer);
                            Uint32 delimLen = 0L;
                            outStream.write(&delimLen, 4); // 4 bytes length
                        } else {
                            // the complete sequence is written but it
                            // is not possible to write the delimitation item into the buffer.
                            errorFlag = EC_StreamNotifyClient;
                            setTransferState(ERW_inWork);
                        }
                    }
                }
            }
        }
    }
    return errorFlag;
}

// ********************************


OFCondition DcmSequenceOfItems::writeTagAndVR(DcmOutputStream &outStream,
                                              const DcmTag &tag,
                                              DcmEVR vr,
                                              const E_TransferSyntax oxfer)
{
    OFCondition l_error = outStream.status();
    if (l_error.good())
    {
        /* write the tag information (a total of 4 bytes, group number and element */
        /* number) to the stream. Mind the transfer syntax's byte ordering. */
        l_error = writeTag(outStream, tag, oxfer);
        /* create an object which represents the transfer syntax */
        DcmXfer oxferSyn(oxfer);
        /* if the transfer syntax is one with explicit value representation */
        /* this value's data type also has to be written to the stream. Do so */
        /* and also write the length information to the stream. */
        if (oxferSyn.isExplicitVR())
        {
            /* Create an object that represents this object's data type */
            DcmVR myvr(vr);
            /* get name of data type */
            const char *vrname = myvr.getValidVRName();
            /* write data type name to the stream (a total of 2 bytes) */
            outStream.write(vrname, 2);
            /* create another data type object on the basis of the above created object */
            DcmVR outvr(myvr.getValidEVR());
            /* in case we are dealing with a transfer syntax with explicit VR (see if above) */
            /* and the actual VR uses extended length encoding (see DICOM standard (year 2000) */
            /* part 5, section 7.1.2) (or the corresponding section in a later version of the */
            /* standard) we have to add 2 reserved bytes (set to a value of 00H) to the data */
            /* type field and the actual length field is 4 bytes wide. Write the corresponding */
            /* information to the stream. */
            if (outvr.usesExtendedLengthEncoding())
            {
              Uint16 reserved = 0;
              outStream.write(&reserved, 2);  // write 2 reserved bytes to stream
            }
        }
    }
    /* return result */
    return l_error;
}


OFCondition DcmSequenceOfItems::writeSignatureFormat(DcmOutputStream &outStream,
                                                     const E_TransferSyntax oxfer,
                                                     const E_EncodingType enctype,
                                                     DcmWriteCache *wcache)
{
    if (getTransferState() == ERW_notInitialized)
        errorFlag = EC_IllegalCall;
    else
    {
        errorFlag = outStream.status();
        if (errorFlag.good() && (getTransferState() != ERW_ready))
        {
            if (getTransferState() == ERW_init)
            {
                /* first compare with DCM_TagInfoLength (12). If there is not enough space
                 * in the buffer, check if the buffer is still sufficient for the requirements
                 * of this element, which may need only 8 instead of 12 bytes.
                 */
                if ((outStream.avail() >= OFstatic_cast(offile_off_t, DCM_TagInfoLength)) ||
                    (outStream.avail() >= OFstatic_cast(offile_off_t, getTagAndLengthSize(oxfer))))
                {
                    if (enctype == EET_ExplicitLength)
                        setLengthField(getLength(oxfer, enctype));
                    else
                        setLengthField(DCM_UndefinedLength);
                    errorFlag = writeTagAndVR(outStream, getTag(), getVR(), oxfer);
                    /* we don't write the sequence length */
                    if (errorFlag.good())
                    {
                        setTransferState(ERW_inWork);
                        itemList->seek(ELP_first);
                    }
                } else
                    errorFlag = EC_StreamNotifyClient;
            }
            if (getTransferState() == ERW_inWork)
            {
                // itemList->get() can be NULL if buffer was full after
                // writing the last item but before writing the sequence delimitation.
                if (!itemList->empty() && (itemList->get() != NULL))
                {
                    DcmObject *dO;
                    do {
                        dO = itemList->get();
                        if (dO->transferState() != ERW_ready)
                            errorFlag = dO->writeSignatureFormat(outStream, oxfer, enctype, wcache);
                    } while (errorFlag.good() && itemList->seek(ELP_next));
                }
                if (errorFlag.good())
                {
                    setTransferState(ERW_ready);
                    /* we always write a sequence delimitation item tag, but no length */
                    if (outStream.avail() >= 4)
                    {
                        // write sequence delimitation item
                        const DcmTag delim(DCM_SequenceDelimitationItemTag);
                        errorFlag = writeTag(outStream, delim, oxfer);
                    } else {
                        // Every subelement of the item was written but it
                        // is not possible to write the delimitation item
                        // into the buffer.
                        setTransferState(ERW_inWork);
                        errorFlag = EC_StreamNotifyClient;
                    }
                }
            }
        }
    }
    return errorFlag;
}


// ********************************


void DcmSequenceOfItems::transferInit()
{
    DcmObject::transferInit();
    fStartPosition = 0;
    lastItemComplete = OFTrue;
    if (!itemList->empty())
    {
        itemList->seek(ELP_first);
        do {
            itemList->get()->transferInit();
        } while (itemList->seek(ELP_next));
    }
}


// ********************************


void DcmSequenceOfItems::transferEnd()
{
    DcmObject::transferEnd();
    if (!itemList->empty())
    {
        itemList->seek(ELP_first);
        do {
            itemList->get()->transferEnd();
        } while (itemList->seek(ELP_next));
    }
}


// ********************************


OFCondition DcmSequenceOfItems::prepend(DcmItem *item)
{
    errorFlag = EC_Normal;
    if (item != NULL)
    {
        itemList->prepend(item);
        // remember the parent (i.e. the surrounding sequence)
        item->setParent(this);
    } else
        errorFlag = EC_IllegalCall;

    return errorFlag;
}


// ********************************


OFCondition DcmSequenceOfItems::insert(DcmItem *item,
                                       unsigned long where,
                                       OFBool before)
{
    errorFlag = EC_Normal;
    if (item != NULL)
    {
        itemList->seek_to(where);
        // insert before or after "where"
        E_ListPos whichSide = (before) ? (ELP_prev) : (ELP_next);
        itemList->insert(item, whichSide);
        if (where == DCM_EndOfListIndex)
        {
            DCMDATA_TRACE("DcmSequenceOfItems::insert() Item inserted "
                << (before ? "before" : "after") << " last position");
        } else {
            DCMDATA_TRACE("DcmSequenceOfItems::insert() Item inserted "
                << (before ? "before" : "after") << " position " << where);
        }
        // check whether the new item already has a parent
        if (item->getParent() != NULL)
        {
            DCMDATA_DEBUG("DcmSequenceOfItems::insert() Item already has a parent: "
                << item->getParent()->getTag() << " VR=" << DcmVR(item->getParent()->getVR()).getVRName());
        }
        // remember the parent (i.e. the surrounding sequence)
        item->setParent(this);
    } else
        errorFlag = EC_IllegalCall;
    return errorFlag;
}


// ********************************


OFCondition DcmSequenceOfItems::insertAtCurrentPos(DcmItem *item,
                                                   OFBool before)
{
    errorFlag = EC_Normal;
    if (item != NULL)
    {
        // insert before or after current position
        E_ListPos whichSide = (before) ? (ELP_prev) : (ELP_next);
        itemList->insert(item, whichSide);
        // check whether the new item already has a parent
        if (item->getParent() != NULL)
        {
            DCMDATA_DEBUG("DcmSequenceOfItems::insertAtCurrentPos() Item already has a parent: "
                << item->getParent()->getTag() << " VR=" << DcmVR(item->getParent()->getVR()).getVRName());
        }
        // remember the parent (i.e. the surrounding sequence)
        item->setParent(this);
    } else
        errorFlag = EC_IllegalCall;
    return errorFlag;
}


// ********************************


OFCondition DcmSequenceOfItems::append(DcmItem *item)
{
    errorFlag = EC_Normal;
    if (item != NULL)
    {
        itemList->append(item);
        // check whether the new item already has a parent
        if (item->getParent() != NULL)
        {
            DCMDATA_DEBUG("DcmSequenceOfItems::append() Item already has a parent: "
                << item->getParent()->getTag() << " VR=" << DcmVR(item->getParent()->getVR()).getVRName());
        }
        // remember the parent (i.e. the surrounding sequence)
        item->setParent(this);
    } else
        errorFlag = EC_IllegalCall;
    return errorFlag;
}


// ********************************


DcmItem* DcmSequenceOfItems::getItem(const unsigned long num)
{
    errorFlag = EC_Normal;
    DcmItem *item;
    item = OFstatic_cast(DcmItem *, itemList->seek_to(num));  // read item from list
    if (item == NULL)
        errorFlag = EC_IllegalCall;
    return item;
}


// ********************************


DcmObject *DcmSequenceOfItems::nextInContainer(const DcmObject *obj)
{
    if (!obj)
        return itemList->get(ELP_first);
    else
    {
        if (itemList->get() != obj)
        {
            for (DcmObject *search_obj = itemList -> seek(ELP_first);
                search_obj && (search_obj != obj);
                search_obj = itemList -> seek(ELP_next)
               )
            { /* do nothing */ }
        }
        return itemList -> seek(ELP_next);
    }
}


// ********************************


OFCondition DcmSequenceOfItems::nextObject(DcmStack &stack,
                                           const OFBool intoSub)
{
    OFCondition l_error = EC_Normal;
    DcmObject *container = NULL;
    DcmObject *obj = NULL;
    DcmObject *result = NULL;
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
    } else if (examSub)
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


DcmItem *DcmSequenceOfItems::remove(const unsigned long num)
{
    errorFlag = EC_Normal;
    DcmItem *item;
    item = OFstatic_cast(DcmItem *, itemList->seek_to(num));  // read item from list
    if (item != NULL)
    {
        itemList->remove();
        item->setParent(NULL);              // forget about the parent
    } else
        errorFlag = EC_IllegalCall;
    return item;
}


// ********************************


DcmItem *DcmSequenceOfItems::remove(DcmItem *item)
{
    DcmItem *retItem = NULL;
    errorFlag = EC_IllegalCall;
    if (!itemList->empty() && (item != NULL))
    {
        DcmObject *dO;
        itemList->seek(ELP_first);
        do {
            dO = itemList->get();
            if (dO == item)
            {
                itemList->remove();         // removes element from list but does not delete it
                item->setParent(NULL);      // forget about the parent
                errorFlag = EC_Normal;
                break;
            }
        } while (itemList->seek(ELP_next));
    }
    if (errorFlag == EC_IllegalCall)
        retItem = NULL;
    else
        retItem = item;
    return retItem;
}


// ********************************


OFCondition DcmSequenceOfItems::clear()
{
    errorFlag = EC_Normal;
    // remove all items from sequence and delete them from memory
    itemList->deleteAllElements();
    setLengthField(0);
    return errorFlag;
}


OFBool DcmSequenceOfItems::isEmpty(const OFBool /*normalize*/)
{
    return itemList->empty();
}


// ********************************


OFCondition DcmSequenceOfItems::verify(const OFBool autocorrect)
{
    errorFlag = EC_Normal;
    if (!itemList->empty())
    {
        DcmObject *dO;
        itemList->seek(ELP_first);
        do {
            dO = itemList->get();
            if (dO->verify(autocorrect).bad())
                errorFlag = EC_CorruptedData;
        } while (itemList->seek(ELP_next));
    }
    if (autocorrect == OFTrue)
        setLengthField(getLength());

    return errorFlag;
}


/*
 * precondition: itemList not empty.
 * result:
 *  - element pointer on resultStack if return value is EC_Normal
 *  - unmodified resultStack if return value is EC_TagNotFound
 * continue search: push pointer to sub-element onto resultStack and start sub-search
 */

OFCondition DcmSequenceOfItems::searchSubFromHere(const DcmTagKey &tag,
                                                  DcmStack &resultStack,
                                                  const OFBool searchIntoSub)
{
    DcmObject *dO;
    OFCondition l_error = EC_TagNotFound;
    if (!itemList->empty())
    {
        itemList->seek(ELP_first);
        do {
            dO = itemList->get();
            if (searchIntoSub)
            {
                resultStack.push(dO);
                if (tag == dO->getTag())
                    l_error = EC_Normal;
                else
                    l_error = dO->search(tag, resultStack, ESM_fromStackTop, OFTrue);
                if (l_error.bad())
                    resultStack.pop();
            } else {
                if (tag == dO->getTag())
                {
                    resultStack.push(dO);
                    l_error = EC_Normal;
                }
            }
        } while (l_error.bad() && itemList->seek(ELP_next));
    }
    return l_error;
}


// ********************************


OFCondition DcmSequenceOfItems::search(const DcmTagKey &tag,
                                       DcmStack &resultStack,
                                       E_SearchMode mode,
                                       OFBool searchIntoSub)
{
    DcmObject *dO = NULL;
    OFCondition l_error = EC_TagNotFound;
    if ((mode == ESM_afterStackTop) && (resultStack.top() == this))
    {
        l_error = searchSubFromHere(tag, resultStack, searchIntoSub);
    }
    else if (!itemList->empty())
    {
        if ((mode == ESM_fromHere) || resultStack.empty())
        {
            resultStack.clear();
            l_error = searchSubFromHere(tag, resultStack, searchIntoSub);
        }
        else if (mode == ESM_fromStackTop)
        {
            dO = resultStack.top();
            if (dO == this)
                l_error = searchSubFromHere(tag, resultStack, searchIntoSub);
            else
            {   // continue directly in sub-tree
                l_error = dO->search(tag, resultStack, mode, searchIntoSub);
// The next two lines destroy the stack, delete them
//                if (l_error.bad())
//                    resultStack.pop();
            }
        }
        else if ((mode == ESM_afterStackTop) && searchIntoSub)
        {
            // resultStack contains additional information:
            // - restore state of the last search in each search routine
            // - find position of dO in tree structure
            //   1. search for own stack entry
            //      - in case of failure terminate search
            //   2. take next upper entry dnO
            //   3. set own list to position of dnO
            //   4. restart search from dnO

            unsigned long i = resultStack.card();
            while ((i > 0) && ((dO = resultStack.elem(i-1)) != this))
            {
                i--;
            }
            if ((dO != this) && (resultStack.card() > 0))
            {                                 // highest level is never in resultStack
                i = resultStack.card()+1;     // now points to highest level +1
                dO = this;                    // match in highest level
            }
            if (dO == this)
            {
                if (i == 1)                   // resultStack.top() found
                    l_error = EC_TagNotFound; // don't mark as match, see above
                else
                {
                    E_SearchMode submode = mode;
                    OFBool searchNode = OFTrue;
                    DcmObject *dnO;
                    dnO = resultStack.elem(i-2); // node of next level
                    itemList->seek(ELP_first);
                    do {
                        dO = itemList->get();
                        searchNode = searchNode ? (dO != dnO) : OFFalse;
                        if (!searchNode)
                        {                               // continue search
                            if (submode == ESM_fromStackTop)
                                resultStack.push(dO);   // update stack
                            if ((submode == ESM_fromStackTop) && (tag == dO->getTag()))
                                l_error = EC_Normal;
                            else
                                l_error = dO->search(tag, resultStack, submode, OFTrue);
                            if (l_error.bad())
                                resultStack.pop();
                            else
                                break;
                            submode = ESM_fromStackTop; // normal search from here
                        }
                    } while (itemList->seek(ELP_next));
                }
            } else                              // probably never reached
                l_error = EC_IllegalCall;
        } // (mode == ESM_afterStackTop
        else
            l_error = EC_IllegalCall;
    }
    return l_error;
}


// ********************************


OFCondition DcmSequenceOfItems::loadAllDataIntoMemory()
{
    OFCondition l_error = EC_Normal;
    if (!itemList->empty())
    {
        itemList->seek(ELP_first);
        do {
            OFCondition err = EC_Normal;
            DcmObject *dO = itemList->get();
            if ((err = dO->loadAllDataIntoMemory()).bad())
                l_error = err;
        } while (itemList->seek(ELP_next));
    }
    return l_error;
}


// ********************************


OFBool DcmSequenceOfItems::isSignable() const
{
    // a sequence is signable if the tag and VR doesn't prevent signing
    // and if none of the items contains a UN element
    return DcmElement::isSignable() && !containsUnknownVR();
}


OFBool DcmSequenceOfItems::containsUnknownVR() const
{
    if (!itemList->empty())
    {
        itemList->seek(ELP_first);
        do {
            if (itemList->get()->containsUnknownVR())
                return OFTrue;
        } while (itemList->seek(ELP_next));
    }
    return OFFalse;
}


OFBool DcmSequenceOfItems::containsExtendedCharacters(const OFBool checkAllStrings)
{
    if (!itemList->empty())
    {
        itemList->seek(ELP_first);
        do {
            if (itemList->get()->containsExtendedCharacters(checkAllStrings))
                return OFTrue;
        } while (itemList->seek(ELP_next));
    }
    return OFFalse;
}


OFBool DcmSequenceOfItems::isAffectedBySpecificCharacterSet() const
{
    if (!itemList->empty())
    {
        itemList->seek(ELP_first);
        do {
            if (itemList->get()->isAffectedBySpecificCharacterSet())
                return OFTrue;
        } while (itemList->seek(ELP_next));
    }
    return OFFalse;
}


OFCondition DcmSequenceOfItems::convertCharacterSet(DcmSpecificCharacterSet &converter)
{
    OFCondition status = EC_Normal;
    if (!itemList->empty())
    {
        // iterate over all items in this sequence and convert the string elements
        itemList->seek(ELP_first);
        do {
            status = itemList->get()->convertCharacterSet(converter);
        } while (status.good() && itemList->seek(ELP_next));
    }
    return status;
}


OFCondition DcmSequenceOfItems::getPartialValue(void * /* targetBuffer */,
                                                const Uint32 /* offset */,
                                                Uint32 /* numBytes */,
                                                DcmFileCache * /* cache */,
                                                E_ByteOrder /* byteOrder */)
{
    // cannot use getPartialValue() with class DcmSequenceOfItems or derived classes
    return EC_IllegalCall;
}
