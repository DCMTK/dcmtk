/*
 *
 *  Copyright (C) 1994-2010, OFFIS e.V.
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
 *  Last Update:      $Author: uli $
 *  Update Date:      $Date: 2010-11-01 10:42:44 $
 *  CVS/RCS Revision: $Revision: 1.94 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#define INCLUDE_CSTDLIB
#define INCLUDE_CSTDIO
#include "dcmtk/ofstd/ofstdinc.h"

#include "dcmtk/ofstd/ofstream.h"
#include "dcmtk/ofstd/ofstd.h"
#include "dcmtk/ofstd/ofcast.h"

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
            itemList->insert(old.itemList->get()->clone(), ELP_next);
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


OFCondition DcmSequenceOfItems::checkValue(const OFString &cardinality,
                                           const OFBool /*oldFormat*/)
{
    /* check cardinality (number of items) only */
    return DcmElement::checkVM(this->card(), cardinality);
}


// ********************************


void DcmSequenceOfItems::print(STD_NAMESPACE ostream&out,
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
        DcmTag delimItemTag(DCM_SequenceDelimitationItem);
        if (getLengthField() == DCM_UndefinedLength)
            printInfoLine(out, flags, level, "(SequenceDelimitationItem)", &delimItemTag);
        else
            printInfoLine(out, flags, level, "(SequenceDelimitationItem for re-encod.)", &delimItemTag);
    }
}


// ********************************


OFCondition DcmSequenceOfItems::writeXML(STD_NAMESPACE ostream&out,
                                         const size_t flags)
{
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
    out << " vr=\"" << vr.getVRName() << "\"";
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
            dO->writeXML(out, flags);
        } while (itemList->seek(ELP_next));
    }
    /* XML end tag for "sequence" */
    out << "</sequence>" << OFendl;
    /* always report success */
    return EC_Normal;
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
        // tag has ben read

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
    // For DcmSequenceOfItems, subObject is always inherited from DcmItem
    // For DcmPixelSequence, subObject is always inherited from DcmPixelItem
    DcmObject * subObject = NULL;
    OFCondition l_error = makeSubObject(subObject, newTag, newLength);
    if (l_error.good() && (subObject != NULL))
    {
        // inStream.UnsetPutbackMark(); // not needed anymore with new stream architecture
        itemList->insert(subObject, ELP_next);
        l_error = subObject->read(inStream, xfer, glenc, maxReadLength); // read sub-item
        return l_error; // prevent subObject from getting deleted
    }
    else if (l_error == EC_InvalidTag)  // try to recover parsing
    {
        inStream.putback();
        DCMDATA_ERROR("DcmSequenceOfItems: Parse error in sequence, found " << newTag
            << " instead of item tag");
        DCMDATA_DEBUG("DcmSequenceOfItems::readSubItem(): parse error occurred: " << newTag);
    }
    else if (l_error != EC_SequEnd)
    {
        DCMDATA_ERROR("DcmSequenceOfItems: Parse error in sequence, found " << newTag
            << " instead of a sequence delimiter");
        DCMDATA_DEBUG("DcmSequenceOfItems::readSubItem(): cannot create Sub Item " << newTag);
    } else {
        // inStream.UnsetPutbackMark(); // not needed anymore with new stream architecture
    }

    if (subObject) delete subObject; // only executed if makeSubObject() has returned an error
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
                if ((outStream.avail() >= DCM_TagInfoLength) ||
                    (outStream.avail() >= getTagAndLengthSize(oxfer)))
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
                            DcmTag delim(DCM_SequenceDelimitationItem);
                            errorFlag = writeTag(outStream, delim, oxfer);
                            Uint32 delimLen = 0L;
                            outStream.write(&delimLen, 4); // 4 bytes length
                        } else {
                            // the complete sequence is written but it
                            // is not possible to write the delimination item into the buffer.
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
                if ((outStream.avail() >= DCM_TagInfoLength) ||
                    (outStream.avail() >= getTagAndLengthSize(oxfer)))
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
                        DcmTag delim(DCM_SequenceDelimitationItem);
                        errorFlag = writeTag(outStream, delim, oxfer);
                    } else {
                        // Every subelement of the item was written but it
                        // is not possible to write the delimination item
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


unsigned long DcmSequenceOfItems::card()
{
    return itemList->card();
}


// ********************************


OFCondition DcmSequenceOfItems::prepend(DcmItem *item)
{
    errorFlag = EC_Normal;
    if (item != NULL)
        itemList->prepend(item);
    else
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
    } else
        errorFlag = EC_IllegalCall;
    return errorFlag;
}

// ********************************

OFCondition DcmSequenceOfItems::insertAtCurrentPos(DcmItem* item,
                                                   OFBool before)
{
    errorFlag = EC_Normal;
    if (item != NULL)
    {
        // insert before or after current position
        E_ListPos whichSide = (before) ? (ELP_prev) : (ELP_next);
        itemList->insert(item, whichSide);
    } else
        errorFlag = EC_IllegalCall;
    return errorFlag;
}


// ********************************


OFCondition DcmSequenceOfItems::append(DcmItem *item)
{
    errorFlag = EC_Normal;
    if (item != NULL)
        itemList->append(item);
    else
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
        itemList->remove();
    else
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


OFCondition DcmSequenceOfItems::getPartialValue(void * /* targetBuffer */,
                                                const Uint32 /* offset */,
                                                Uint32 /* numBytes */,
                                                DcmFileCache * /* cache */,
                                                E_ByteOrder /* byteOrder */)
{
    // cannot use getPartialValue() with class DcmSequenceOfItems or derived classes
    return EC_IllegalCall;
}


/*
** CVS/RCS Log:
** $Log: dcsequen.cc,v $
** Revision 1.94  2010-11-01 10:42:44  uli
** Fixed some compiler warnings reported by gcc with additional flags.
**
** Revision 1.93  2010-10-20 16:44:17  joergr
** Use type cast macros (e.g. OFstatic_cast) where appropriate.
**
** Revision 1.92  2010-10-14 13:14:09  joergr
** Updated copyright header. Added reference to COPYRIGHT file.
**
** Revision 1.91  2010-04-23 14:33:57  joergr
** Added new method to all VR classes which checks whether the stored value
** conforms to the VR definition and to the specified VM.
**
** Revision 1.90  2010-03-25 16:28:40  joergr
** Added check for errors during while loop in computeGroupLengthAndPadding().
**
** Revision 1.89  2010-03-24 11:56:36  onken
** Fixed memory leak in assignment operators of DcmItem and DcmSequenceOfItems.
** Replaced all code occurences of cleaning all elements from internal lists to
** newly introduced function in DcmList.
**
** Revision 1.88  2010-03-03 14:30:28  joergr
** Output return value of readTagAndLength() to the log only in case of error.
** Use return value of getTag() for stream output where possible.
**
** Revision 1.87  2009-12-04 16:54:17  joergr
** Moved some log messages from debug to trace level.
** Sightly modified some log messages.
**
** Revision 1.86  2009-11-13 13:11:21  joergr
** Fixed minor issues in log output.
**
** Revision 1.85  2009-11-04 09:58:10  uli
** Switched to logging mechanism provided by the "new" oflog module
**
** Revision 1.84  2009-08-07 14:35:49  joergr
** Enhanced isEmpty() method by checking whether the data element value consists
** of non-significant characters only.
**
** Revision 1.83  2009-03-25 10:21:22  joergr
** Added new method isEmpty() to DICOM object, item and sequence class.
**
** Revision 1.82  2009-03-05 14:08:05  onken
** Fixed typo.
**
** Revision 1.81  2009-03-05 13:35:07  onken
** Added checks for sequence and item lengths which prevents overflow in length
** field, if total length of contained items (or sequences) exceeds 32-bit
** length field. Also introduced new flag (default: enabled) for writing
** in explicit length mode, which allows for automatically switching encoding
** of only that very sequence/item to undefined length coding (thus permitting
** to actually write the file).
**
** Revision 1.80  2009-02-04 17:57:19  joergr
** Fixes various type mismatches reported by MSVC introduced with OFFile class.
**
** Revision 1.79  2009-02-04 14:04:18  onken
** Removed German comment and removed "magic number".
**
** Revision 1.78  2009-01-06 16:27:03  joergr
** Reworked print() output format for option PF_showTreeStructure.
**
** Revision 1.77  2008-12-12 11:44:41  onken
** Moved path access functions to separate classes
**
** Revision 1.76  2008-12-05 13:51:13  onken
** Introduced new error code number for specific findOrCreatePath() errors.
**
** Revision 1.75  2008-12-05 13:28:01  onken
** Splitted findOrCreatePath() function API for also offering a simple API
** for non-wildcard searches.
**
** Revision 1.74  2008-12-04 16:54:54  onken
** Changed findOrCreatePath() to also support wildcard as item numbers.
**
** Revision 1.73  2008-10-15 12:31:24  onken
** Added findOrCreatePath() functions which allow for finding or creating a
** hierarchy of sequences, items and attributes according to a given "path"
** string.
**
** Revision 1.72  2008-07-17 10:36:38  onken
** *** empty log message ***
**
** Revision 1.71  2008-07-17 10:31:32  onken
** Implemented copyFrom() method for complete DcmObject class hierarchy, which
** permits setting an instance's value from an existing object. Implemented
** assignment operator where necessary.
**
** Revision 1.70  2007-11-29 14:30:21  meichel
** Write methods now handle large raw data elements (such as pixel data)
**   without loading everything into memory. This allows very large images to
**   be sent over a network connection, or to be copied without ever being
**   fully in memory.
**
** Revision 1.69  2007/11/23 15:42:36  meichel
** Copy assignment operators in dcmdata now safe for self assignment
**
** Revision 1.68  2007/06/29 14:17:49  meichel
** Code clean-up: Most member variables in module dcmdata are now private,
**   not protected anymore.
**
** Revision 1.67  2007/02/19 15:04:16  meichel
** Removed searchErrors() methods that are not used anywhere and added
**   error() methods only in the DcmObject subclasses where really used.
**
** Revision 1.66  2006/12/15 14:14:44  joergr
** Added new method that checks whether a DICOM object or element is affected
** by SpecificCharacterSet (0008,0005).
**
** Revision 1.65  2006/12/13 13:59:49  joergr
** Added new optional parameter "checkAllStrings" to method containsExtended
** Characters().
**
** Revision 1.64  2006/08/15 15:49:54  meichel
** Updated all code in module dcmdata to correctly compile when
**   all standard C++ classes remain in namespace std.
**
** Revision 1.63  2006/05/30 15:00:19  joergr
** Added missing method containsExtendedCharacters().
**
** Revision 1.62  2006/05/11 08:51:05  joergr
** Added new option that allows to omit the element name in the XML output.
**
** Revision 1.61  2005/12/08 15:41:36  meichel
** Changed include path schema for all DCMTK header files
**
** Revision 1.60  2005/11/28 15:53:13  meichel
** Renamed macros in dcdebug.h
**
** Revision 1.59  2005/11/15 18:28:04  meichel
** Added new global flag dcmEnableUnknownVRConversion that enables the automatic
**   re-conversion of defined length UN elements read in an explicit VR transfer
**   syntax, if the real VR is defined in the data dictionary. Default is OFFalse,
**   i.e. to retain the previous behavior.
**
** Revision 1.58  2005/11/07 16:59:26  meichel
** Cleaned up some copy constructors in the DcmObject hierarchy.
**
** Revision 1.57  2005/05/10 15:27:18  meichel
** Added support for reading UN elements with undefined length according
**   to CP 246. The global flag dcmEnableCP246Support allows to revert to the
**   prior behaviour in which UN elements with undefined length were parsed
**   like a normal explicit VR SQ element.
**
** Revision 1.56  2004/04/27 09:21:27  wilkens
** Fixed a bug in dcelem.cc which occurs when one is serializing a dataset
** (that contains an attribute whose length value is coded with 2 bytes) into
** a given buffer. Although the number of available bytes in the buffer was
** sufficient, the dataset->write(...) method would always return
** EC_StreamNotifyClient to indicate that there are not sufficient bytes
** available in the buffer. This code modification fixes the problem.
**
** Revision 1.55  2004/01/16 13:49:31  joergr
** Removed acknowledgements with e-mail addresses from CVS log.
**
** Revision 1.54  2003/10/15 16:55:43  meichel
** Updated error messages for parse errors
**
** Revision 1.53  2003/08/08 13:32:18  joergr
** Added new method insertAtCurrentPos() which allows for a much more efficient
** insertion (avoids re-searching for the correct position).
** Adapted type casts to new-style typecast operators defined in ofcast.h.
** Translated remaining German comments.
**
** Revision 1.52  2003/01/06 09:29:48  joergr
** Performed minor text corrections to get a more consistent print() output.
**
** Revision 1.51  2002/12/10 20:03:01  joergr
** Added curly brackets around debug() call to avoid compiler errors with gcc
** 2.9.5 in debug mode.
**
** Revision 1.50  2002/12/06 13:16:59  joergr
** Enhanced "print()" function by re-working the implementation and replacing
** the boolean "showFullData" parameter by a more general integer flag.
** Made source code formatting more consistent with other modules/files.
**
** Revision 1.49  2002/11/27 12:06:51  meichel
** Adapted module dcmdata to use of new header file ofstdinc.h
**
** Revision 1.48  2002/08/27 16:55:56  meichel
** Initial release of new DICOM I/O stream classes that add support for stream
**   compression (deflated little endian explicit VR transfer syntax)
**
** Revision 1.47  2002/07/08 14:44:41  meichel
** Improved dcmdata behaviour when reading odd tag length. Depending on the
**   global boolean flag dcmAcceptOddAttributeLength, the parser now either accepts
**   odd length attributes or implements the old behaviour, i.e. assumes a real
**   length larger by one.
**
** Revision 1.46  2002/04/25 10:26:10  joergr
** Added support for XML output of DICOM objects.
**
** Revision 1.45  2002/04/16 13:43:20  joergr
** Added configurable support for C++ ANSI standard includes (e.g. streams).
**
** Revision 1.44  2002/03/15 13:58:39  meichel
** Fixed incorrect debug message.
**
** Revision 1.43  2001/11/16 15:55:04  meichel
** Adapted digital signature code to final text of supplement 41.
**
** Revision 1.42  2001/09/28 14:21:06  joergr
** Added "#include <iomanip.h>" to keep gcc 3.0 quiet.
**
** Revision 1.41  2001/09/26 15:49:30  meichel
** Modified debug messages, required by OFCondition
**
** Revision 1.40  2001/09/25 17:19:53  meichel
** Adapted dcmdata to class OFCondition
**
** Revision 1.39  2001/06/01 15:49:08  meichel
** Updated copyright header
**
** Revision 1.38  2001/05/10 12:46:28  meichel
** Fixed memory leak that occured when parsing of a sequence failed.
**
** Revision 1.37  2001/05/03 08:15:23  meichel
** Fixed bug in dcmdata sequence handling code that could lead to application
**   failure in rare cases during parsing of a correct DICOM dataset.
**
** Revision 1.36  2000/11/07 16:56:22  meichel
** Initial release of dcmsign module for DICOM Digital Signatures
**
** Revision 1.35  2000/04/14 15:55:07  meichel
** Dcmdata library code now consistently uses ofConsole for error output.
**
** Revision 1.34  2000/03/08 16:26:40  meichel
** Updated copyright header.
**
** Revision 1.33  2000/03/03 15:02:11  joergr
** Corrected bug related to padding of file and item size.
**
** Revision 1.32  2000/03/03 14:05:36  meichel
** Implemented library support for redirecting error messages into memory
**   instead of printing them to stdout/stderr for GUI applications.
**
** Revision 1.31  2000/02/23 15:12:01  meichel
** Corrected macro for Borland C++ Builder 4 workaround.
**
** Revision 1.30  2000/02/10 10:52:22  joergr
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
