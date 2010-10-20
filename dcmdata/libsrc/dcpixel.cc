/*
 *
 *  Copyright (C) 1997-2010, OFFIS e.V.
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
 *  Author:  Andreas Barth
 *
 *  Purpose: class DcmPixelData
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-20 16:44:16 $
 *  CVS/RCS Revision: $Revision: 1.51 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/dcmdata/dcpixel.h"
#include "dcmtk/dcmdata/dccodec.h"
#include "dcmtk/dcmdata/dcpixseq.h"
#include "dcmtk/dcmdata/dcdeftag.h"
#include "dcmtk/dcmdata/dcitem.h"

//
// class DcmRepresentationEntry
//

DcmRepresentationEntry::DcmRepresentationEntry(
    const E_TransferSyntax rt,
    const DcmRepresentationParameter *rp,
    DcmPixelSequence * ps)
  : repType(rt),
    repParam(NULL),
    pixSeq(ps)
{
    if (rp)
        repParam = rp->clone();
}

DcmRepresentationEntry::DcmRepresentationEntry(
    const DcmRepresentationEntry & oldEntry)
  : repType(oldEntry.repType),
    repParam(NULL),
    pixSeq(NULL)
{
    if (oldEntry.repParam)
        repParam = oldEntry.repParam->clone();
    pixSeq = new DcmPixelSequence(*(oldEntry.pixSeq));
}

DcmRepresentationEntry::~DcmRepresentationEntry()
{
    delete repParam;
    delete pixSeq;
}

OFBool
DcmRepresentationEntry::operator==(const DcmRepresentationEntry & x) const
{
    return (repType == x.repType) &&
        ((x.repParam == NULL && repParam == NULL) ||
         ((x.repParam != NULL) && (repParam != NULL) && (*(x.repParam) == *repParam)));
}

//
// class DcmPixelData
//

// Constructors / Deconstructors

DcmPixelData::DcmPixelData(
    const DcmTag & tag,
    const Uint32 len)
  : DcmPolymorphOBOW(tag, len),
    repList(),
    repListEnd(),
    original(),
    current(),
    existUnencapsulated(OFFalse),
    alwaysUnencapsulated(OFFalse),
    unencapsulatedVR(EVR_UNKNOWN),
    pixelSeqForWrite(NULL)
{
    repListEnd = repList.end();
    current = original = repListEnd;
    if (getTag().getEVR() == EVR_ox) setTagVR(EVR_OW);
    unencapsulatedVR = getTag().getEVR();
    recalcVR();
}

DcmPixelData::DcmPixelData(
    const DcmPixelData & oldPixelData)
  : DcmPolymorphOBOW(oldPixelData),
    repList(),
    repListEnd(),
    original(),
    current(),
    existUnencapsulated(oldPixelData.existUnencapsulated),
    alwaysUnencapsulated(oldPixelData.alwaysUnencapsulated),
    unencapsulatedVR(oldPixelData.unencapsulatedVR),
    pixelSeqForWrite(NULL)
{
    repListEnd = repList.end();
    original = repListEnd;
    current = original;
    recalcVR();
    DcmRepresentationListConstIterator oldEnd(oldPixelData.repList.end());
    for (DcmRepresentationListConstIterator it(oldPixelData.repList.begin());
         it != oldEnd;
         ++it)
    {
        DcmRepresentationEntry * repEnt = new DcmRepresentationEntry(**it);
        repList.push_back(repEnt);
        if (it == oldPixelData.original)
            original = --repList.end();

        if (it == oldPixelData.current)
        {
            current = --repList.end();
            recalcVR();
        }
    }
}


DcmPixelData::~DcmPixelData()
{
    for (DcmRepresentationListIterator it(repList.begin());
         it != repListEnd;
         ++it)
    {
        delete *it;
        *it = NULL;
    }
}

DcmPixelData &DcmPixelData::operator=(const DcmPixelData &obj)
{
  if (this != &obj)
  {
    DcmPolymorphOBOW::operator=(obj);
    existUnencapsulated = obj.existUnencapsulated;
    alwaysUnencapsulated = obj.alwaysUnencapsulated;
    unencapsulatedVR = obj.unencapsulatedVR;
    pixelSeqForWrite = NULL;
    repList.clear();
    repListEnd = repList.end();
    original = repListEnd;
    current = original;
    recalcVR();
    DcmRepresentationListConstIterator oldEnd(obj.repList.end());
    DcmRepresentationListConstIterator it(obj.repList.begin());
    while (it != oldEnd)
    {
        DcmRepresentationEntry *repEnt = new DcmRepresentationEntry(**it);
        repList.push_back(repEnt);
        if (it == obj.original) original = --repList.end();
        if (it == current)
        {
            current = --repList.end();
            recalcVR();
        }
        ++it;
    }

  }
  return *this;
}


OFCondition DcmPixelData::copyFrom(const DcmObject& rhs)
{
  if (this != &rhs)
  {
    if (rhs.ident() != ident()) return EC_IllegalCall;
    *this = OFstatic_cast(const DcmPixelData &, rhs);
  }
  return EC_Normal;
}


// methods in alphabetical order

Uint32
DcmPixelData::calcElementLength(
    const E_TransferSyntax xfer,
    const E_EncodingType enctype)
{
    DcmXfer xferSyn(xfer);
    errorFlag = EC_Normal;
    Uint32 elementLength = 0;

    if (xferSyn.isEncapsulated() && (! alwaysUnencapsulated))
    {
        DcmRepresentationListIterator found;
        errorFlag =
            findConformingEncapsulatedRepresentation(xfer, NULL, found);
        if (errorFlag == EC_Normal)
            elementLength = (*found)->pixSeq->calcElementLength(xfer, enctype);
    }
    else if (existUnencapsulated)
        elementLength =  DcmPolymorphOBOW::calcElementLength(xfer, enctype);
    else
        errorFlag = EC_RepresentationNotFound;

    return elementLength;
}


OFBool
DcmPixelData::canChooseRepresentation(
    const E_TransferSyntax repType,
    const DcmRepresentationParameter * repParam)
{
    OFBool result = OFFalse;
    DcmXfer toType(repType);

    const DcmRepresentationEntry findEntry(repType, repParam, NULL);
    DcmRepresentationListIterator resultIt(repListEnd);
    if ((!toType.isEncapsulated() && existUnencapsulated) ||
        (toType.isEncapsulated() && alwaysUnencapsulated && existUnencapsulated) ||
        (toType.isEncapsulated() && findRepresentationEntry(findEntry, resultIt) == EC_Normal))
    {
        // representation found
        result = OFTrue;
    }
    else
    {
        // representation not found, check if we have a codec that can create the
        // desired representation.
        if (original == repListEnd)
        {
          result = DcmCodecList::canChangeCoding(EXS_LittleEndianExplicit, toType.getXfer());
        }
        else if (toType.isEncapsulated())
        {
          result = DcmCodecList::canChangeCoding(EXS_LittleEndianExplicit, toType.getXfer());

          if (!result)
          {
            // direct transcoding is not possible. Check if we can decode and then encode.
            result = canChooseRepresentation(EXS_LittleEndianExplicit, NULL);
            if (result) result = DcmCodecList::canChangeCoding(EXS_LittleEndianExplicit, toType.getXfer());
          }
        }
        else
        {
          result = DcmCodecList::canChangeCoding((*original)->repType, EXS_LittleEndianExplicit);
        }
    }
    return result;
}

OFBool
DcmPixelData::canWriteXfer(
    const E_TransferSyntax newXfer,
    const E_TransferSyntax /*oldXfer*/)
{
    DcmXfer newXferSyn(newXfer);
    DcmRepresentationListIterator found;
    OFBool result = existUnencapsulated && (!newXferSyn.isEncapsulated() || alwaysUnencapsulated);

    if (!result && newXferSyn.isEncapsulated())
        result = (findConformingEncapsulatedRepresentation(newXferSyn, NULL, found) == EC_Normal);
    return result;
}

OFCondition
DcmPixelData::chooseRepresentation(
    const E_TransferSyntax repType,
    const DcmRepresentationParameter * repParam,
    DcmStack & pixelStack)
{
    OFCondition l_error = EC_CannotChangeRepresentation;
    DcmXfer toType(repType);

    const DcmRepresentationEntry findEntry(repType, repParam, NULL);
    DcmRepresentationListIterator result(repListEnd);
    if ((!toType.isEncapsulated() && existUnencapsulated) ||
        (toType.isEncapsulated() && existUnencapsulated && alwaysUnencapsulated) ||
        (toType.isEncapsulated() && findRepresentationEntry(findEntry, result) == EC_Normal))
    {
        // representation found
        current = result;
        recalcVR();
        l_error = EC_Normal;
    }
    else
    {
        if (original == repListEnd)
            l_error = encode(EXS_LittleEndianExplicit, NULL, NULL,
                             toType, repParam, pixelStack);
        else if (toType.isEncapsulated())
            l_error = encode((*original)->repType, (*original)->repParam,
                             (*original)->pixSeq, toType, repParam, pixelStack);
        else
            l_error = decode((*original)->repType, (*original)->repParam,
                             (*original)->pixSeq, pixelStack);
    }
    return l_error;
}


void
DcmPixelData::clearRepresentationList(
    DcmRepresentationListIterator leaveInList)
{
    /* define iterators to go through all representations in the list */
    DcmRepresentationListIterator it(repList.begin());
    DcmRepresentationListIterator del;

    /* as long as we have not encountered the end of the */
    /* representation list, go through all representations */
    while (it != repListEnd)
    {
        /* if this representation shall not be left in the list */
        if (it != leaveInList)
        {
            /* delete representation and move it to the next representation */
            delete *it;
            del = it++;
            repList.erase(del);
        }
        /* else leave this representation in the list and just go to the next */
        else
            ++it;
    }
}

OFCondition
DcmPixelData::decode(
    const DcmXfer & fromType,
    const DcmRepresentationParameter * fromParam,
    DcmPixelSequence * fromPixSeq,
    DcmStack & pixelStack)
{
    if (existUnencapsulated) return EC_Normal;
    OFCondition l_error = DcmCodecList::decode(fromType, fromParam, fromPixSeq, *this, pixelStack);
    if (l_error.good())
    {
        existUnencapsulated = OFTrue;
        current = repListEnd;
        setVR(EVR_OW);
        recalcVR();
    }
    else
    {
        DcmPolymorphOBOW::putUint16Array(NULL,0);
        existUnencapsulated = OFFalse;
    }
    return l_error;
}



OFCondition
DcmPixelData::encode(
    const DcmXfer & fromType,
    const DcmRepresentationParameter * fromParam,
    DcmPixelSequence * fromPixSeq,
    const DcmXfer & toType,
    const DcmRepresentationParameter *toParam,
    DcmStack & pixelStack)
{
    OFCondition l_error = EC_CannotChangeRepresentation;
    if (toType.isEncapsulated())
    {
       DcmPixelSequence * toPixSeq = NULL;
       if (fromType.isEncapsulated())
       {
         l_error = DcmCodecList::encode(fromType.getXfer(), fromParam, fromPixSeq,
                   toType.getXfer(), toParam, toPixSeq, pixelStack);
       }
       else
       {
         Uint16 * pixelData;
         l_error = DcmPolymorphOBOW::getUint16Array(pixelData);
         Uint32 length = DcmPolymorphOBOW::getLength();
         if (l_error == EC_Normal)
         {
           l_error = DcmCodecList::encode(fromType.getXfer(), pixelData, length,
                     toType.getXfer(), toParam, toPixSeq, pixelStack);
         }
       }

       if (l_error.good())
       {
           current = insertRepresentationEntry(
             new DcmRepresentationEntry(toType.getXfer(), toParam, toPixSeq));
           recalcVR();
       } else delete toPixSeq;

       // if it was possible to convert one encapsulated syntax into
       // another directly try it using decoding and encoding!
       if (l_error.bad() && fromType.isEncapsulated())
       {
           l_error = decode(fromType, fromParam, fromPixSeq, pixelStack);
           if (l_error.good()) l_error = encode(EXS_LittleEndianExplicit, NULL, NULL, toType, toParam, pixelStack);
       }
    }
    return l_error;
}

OFCondition
DcmPixelData::findRepresentationEntry(
    const DcmRepresentationEntry & findEntry,
    DcmRepresentationListIterator & result)
{
    result = repList.begin();
    while(result != repListEnd &&
          (*result)->repType < findEntry.repType)
        ++result;

    DcmRepresentationListIterator it(result);

    while(it != repListEnd && **it != findEntry)
        ++it;
    if (it == repListEnd || **it != findEntry)
        return EC_RepresentationNotFound;
    else
    {
        result = it;
        return EC_Normal;
    }
}


OFCondition
DcmPixelData::findConformingEncapsulatedRepresentation(
    const DcmXfer & repTypeSyn,
    const DcmRepresentationParameter * repParam,
    DcmRepresentationListIterator & result)
{
    E_TransferSyntax repType = repTypeSyn.getXfer();
    result = repListEnd;
    OFCondition l_error = EC_RepresentationNotFound;
    // we are looking for an encapsulated representation
    // of this pixel data element which meets both
    // the transfer syntax and (if given) the representation
    // parameter (i.e. quality factor for lossy JPEG).
    if (repTypeSyn.isEncapsulated())
    {
        // first we check the current (active) representation if any.
        if ((current != repListEnd) && ((*current)->repType == repType) &&
            ((repParam==NULL) || (((*current)->repParam != NULL)&&(*(*current)->repParam == *repParam))))
        {
            result = current;
            l_error = EC_Normal;
        }
        else
        {
            // now we check all representations
            DcmRepresentationListIterator it(repList.begin());
            OFBool found = OFFalse;
            while (!found && (it != repListEnd))
            {
              if ((*it)->repType == repType)
              {
                if ((repParam == NULL) || (((*it)->repParam != NULL)&&(*(*it)->repParam == *repParam)))
                {
                  // repParam is NULL or matches the one we are comparing with
                  found = OFTrue;
                  result = it;
                  l_error = EC_Normal;
                } else ++it;
              } else ++it;
            }
        }
    }
    return l_error;
}


OFCondition
DcmPixelData::getEncapsulatedRepresentation(
    const E_TransferSyntax repType,
    const DcmRepresentationParameter * repParam,
    DcmPixelSequence * & pixSeq)
{
    DcmRepresentationListIterator found;
    DcmRepresentationEntry findEntry(repType, repParam, NULL);
    if (findRepresentationEntry(findEntry, found) == EC_Normal)
    {
        pixSeq = (*found)->pixSeq;
        return EC_Normal;
    }

    return EC_RepresentationNotFound;
}


OFBool
DcmPixelData::hasRepresentation(
    const E_TransferSyntax repType,
    const DcmRepresentationParameter * repParam)
{
    DcmXfer repTypeSyn(repType);
    DcmRepresentationListIterator found;
    if (!repTypeSyn.isEncapsulated() && existUnencapsulated)
        return OFTrue;
    else if (repTypeSyn.isEncapsulated())
        return findConformingEncapsulatedRepresentation(repTypeSyn, repParam, found).good();
    return OFFalse;
}


Uint32
DcmPixelData::getLength(const E_TransferSyntax xfer,
                        const E_EncodingType enctype)
{
    DcmXfer xferSyn(xfer);
    errorFlag = EC_Normal;
    Uint32 valueLength = 0;

    if (xferSyn.isEncapsulated() && !alwaysUnencapsulated)
    {
        DcmRepresentationListIterator foundEntry;
        errorFlag = findConformingEncapsulatedRepresentation(
            xferSyn, NULL, foundEntry);
        if (errorFlag == EC_Normal)
            valueLength = (*foundEntry)->pixSeq->getLength(xfer, enctype);
    }
    else if (existUnencapsulated)
        valueLength =  DcmPolymorphOBOW::getLength(xfer, enctype);
    else
        errorFlag = EC_RepresentationNotFound;

    return valueLength;
}


void
DcmPixelData::getCurrentRepresentationKey(
    E_TransferSyntax & repType,
    const DcmRepresentationParameter * & repParam)
{
    if (current != repListEnd)
    {
        repType = (*current)->repType;
        repParam = (*current)->repParam;
    }
    else
    {
        repType = EXS_LittleEndianExplicit;
        repParam = NULL;
    }
}

void
DcmPixelData::getOriginalRepresentationKey(
    E_TransferSyntax & repType,
    const DcmRepresentationParameter * & repParam)
{
    if (original != repListEnd)
    {
        repType = (*original)->repType;
        repParam = (*original)->repParam;
    }
    else
    {
        repType = EXS_LittleEndianExplicit;
        repParam = NULL;
    }
}

DcmRepresentationListIterator
DcmPixelData::insertRepresentationEntry(
    DcmRepresentationEntry * repEntry)
{
    DcmRepresentationListIterator insertedEntry;
    DcmRepresentationListIterator result;
    if (findRepresentationEntry(*repEntry, result).good())
    {
        // this type of representation entry was already present in the list
        if (repEntry != *result)
        {
          insertedEntry = repList.insert(result, repEntry);

          // delete old entry from representation list
          delete *result;
          repList.erase(result);
        }
    }
    else
        insertedEntry = repList.insert(result,repEntry);
    return insertedEntry;
}

void
DcmPixelData::print(
    STD_NAMESPACE ostream&out,
    const size_t flags,
    const int level,
    const char *pixelFileName,
    size_t *pixelCounter)
{
    if (current == repListEnd)
        printPixel(out, flags, level, pixelFileName, pixelCounter);
    else
        (*current)->pixSeq->print(out, flags, level, pixelFileName, pixelCounter);
}

OFCondition
DcmPixelData::putUint8Array(
    const Uint8 * byteValue,
    const unsigned long length)
{
    // clear RepresentationList
    clearRepresentationList(repListEnd);
    OFCondition l_error = DcmPolymorphOBOW::putUint8Array(byteValue, length);
    original = current = repListEnd;
    recalcVR();
    existUnencapsulated = OFTrue;
    return l_error;
}

OFCondition
DcmPixelData::putUint16Array(
    const Uint16 * wordValue,
    const unsigned long length)
{
    // clear RepresentationList
    clearRepresentationList(repListEnd);
    OFCondition l_error = DcmPolymorphOBOW::putUint16Array(wordValue, length);
    original = current = repListEnd;
    recalcVR();
    existUnencapsulated = OFTrue;
    return l_error;
}

OFCondition
DcmPixelData::createUint8Array(
    const Uint32 numBytes,
    Uint8 * & bytes)
{
    OFCondition l_error = DcmPolymorphOBOW::createUint8Array(numBytes, bytes);
    existUnencapsulated = OFTrue;
    return l_error;
}

OFCondition
DcmPixelData::createUint16Array(
    const Uint32 numWords,
    Uint16 * & words)
{
    OFCondition l_error = DcmPolymorphOBOW::createUint16Array(numWords, words);
    existUnencapsulated = OFTrue;
    return l_error;
}

OFCondition
DcmPixelData::createValueFromTempFile(
    DcmInputStreamFactory *factory,
    const Uint32 length,
    const E_ByteOrder byteOrder)
{
    OFCondition l_error = DcmPolymorphOBOW::createValueFromTempFile(factory, length, byteOrder);
    existUnencapsulated = OFTrue;
    return l_error;
}

void
DcmPixelData::putOriginalRepresentation(
    const E_TransferSyntax repType,
    const DcmRepresentationParameter * repParam,
    DcmPixelSequence * pixSeq)
{
    // delete RepresentationList
    clearRepresentationList(repListEnd);
    // delete unencapsulated representation
    DcmPolymorphOBOW::putUint16Array(NULL,0);
    existUnencapsulated = OFFalse;
    // insert new Representation
    current = original = insertRepresentationEntry(
        new DcmRepresentationEntry(repType, repParam, pixSeq));
    recalcVR();
}

OFCondition
DcmPixelData::read(
    DcmInputStream & inStream,
    const E_TransferSyntax ixfer,
    const E_GrpLenEncoding glenc,
    const Uint32 maxReadLength)
{
    /* if this element's transfer state shows ERW_notInitialized, this is an illegal call */
    if (getTransferState() == ERW_notInitialized)
        errorFlag = EC_IllegalCall;
    else
    {
        /* if this is not an illegal call, go ahead */

        /* if the transfer state is ERW_init, we need to prepare the reading of the pixel */
        /* data from the stream: remove all representations from the representation list. */
        if (getTransferState() == ERW_init)
            clearRepresentationList(repListEnd);

        /* create a DcmXfer object based on the transfer syntax which was passed */
        DcmXfer ixferSyn(ixfer);

        /* determine if the pixel data is captured in native (uncompressed) or encapsulated
         * (compressed) format.  We only derive this information from the length field
         * which is set to undefined length for encapsulated data because even in
         * compressed transfer syntaxes the Icon Image Sequence may contain an
         * uncompressed image.
         */
        if (getLengthField() == DCM_UndefinedLength)
        {
            /* the pixel data is captured in encapsulated (compressed) format */

            /* if the transfer state is ERW_init, we need to prepare */
            /* the reading of the pixel data from the stream. */
            if (getTransferState() == ERW_init)
            {
                current = insertRepresentationEntry(
                    new DcmRepresentationEntry(
                        ixfer, NULL, new DcmPixelSequence(getTag(), getLengthField())));
                recalcVR();
                original = current;
                existUnencapsulated = OFFalse;
                setTransferState(ERW_inWork);

                if (! ixferSyn.isEncapsulated())
                {
                  /* Special case: we have encountered a compressed image
                   * although we're decoding an uncompressed transfer syntax.
                   * This could be a compressed image stored without meta-header.
                   * For now, we just accept the data element; however, any attempt
                   * to write the dataset will fail because no suitable decoder
                   * is known.
                   */
                }
            }

            /* conduct the reading process */
            errorFlag =
                (*current)->pixSeq->read(inStream, ixfer, glenc, maxReadLength);

            /* if the errorFlag equals EC_Normal, all pixel data has been */
            /* read; hence, the transfer state has to be set to ERW_ready */
            if (errorFlag == EC_Normal)
                setTransferState(ERW_ready);
        }
        else
        {
            /* the pixel data is captured in native (uncompressed) format */

            /* if the transfer state is ERW_init, we need to prepare */
            /* the reading of the pixel data from the stream. */
            if (getTransferState() == ERW_init)
            {
                current = original = repListEnd;
                unencapsulatedVR = getTag().getEVR();
                recalcVR();
                existUnencapsulated = OFTrue;

                if (ixferSyn.isEncapsulated())
                {
                  /* Special case: we have encountered an uncompressed image
                   * although we're decoding an encapsulated transfer syntax.
                   * This is probably an icon image.
                   */
                  alwaysUnencapsulated = OFTrue;
                }
            }

            /* conduct the reading process */
            errorFlag =
                DcmPolymorphOBOW::read(inStream, ixfer, glenc, maxReadLength);
        }
    }

    /* return result value */
    return errorFlag;
}


void
DcmPixelData::removeAllButCurrentRepresentations()
{
    clearRepresentationList(current);
    if (current != repListEnd && existUnencapsulated)
    {
        DcmPolymorphOBOW::putUint16Array(NULL,0);
        existUnencapsulated = OFFalse;
    }
    original = current;
}


void
DcmPixelData::removeAllButOriginalRepresentations()
{
    clearRepresentationList(original);
    if (original != repListEnd && existUnencapsulated)
    {
        DcmPolymorphOBOW::putUint16Array(NULL,0);
        existUnencapsulated = OFFalse;
    }
    current = original;
    recalcVR();
}

OFCondition
DcmPixelData::removeOriginalRepresentation(
    const E_TransferSyntax repType,
    const DcmRepresentationParameter * repParam)
{
    OFCondition l_error = EC_Normal;
    DcmXfer repTypeSyn(repType);

    if (!repTypeSyn.isEncapsulated())
    {
        if (original != repListEnd)
        {
            if (current == original)
            {
                current = repListEnd;
                recalcVR();
            }
            repList.erase(original);
            original = repListEnd;
        }
        else
            l_error = EC_IllegalCall;
    }
    else
    {
        DcmRepresentationListIterator result;
        DcmRepresentationEntry findEntry(repType, repParam, NULL);
        if (findRepresentationEntry(findEntry, result) == EC_Normal)
        {
            if (result != original)
            {
                if (current == original)
                {
                    current = result;
                    recalcVR();
                }
                if (original == repListEnd)
                {
                    DcmPolymorphOBOW::putUint16Array(NULL, 0);
                    existUnencapsulated = OFFalse;
                }
                else
                    repList.erase(original);
                original = result;
            }
            else
                l_error = EC_IllegalCall;
        }
        else
            l_error = EC_RepresentationNotFound;
    }
    return l_error;
}

OFCondition
DcmPixelData::removeRepresentation(
    const E_TransferSyntax repType,
    const DcmRepresentationParameter * repParam)
{
    OFCondition l_error = EC_Normal;
    DcmXfer repTypeSyn(repType);

    if (!repTypeSyn.isEncapsulated())
    {
        if (original != repListEnd && existUnencapsulated)
        {
            DcmPolymorphOBOW::putUint16Array(NULL, 0);
            existUnencapsulated = OFFalse;
        }
        else
            l_error = EC_CannotChangeRepresentation;
    }
    else
    {
        DcmRepresentationListIterator result;
        DcmRepresentationEntry findEntry(repType, repParam, NULL);
        if (findRepresentationEntry(findEntry, result) == EC_Normal)
        {
            if (original != result)
                repList.erase(result);
            else l_error = EC_CannotChangeRepresentation;
        }
        else
            l_error = EC_RepresentationNotFound;
    }
    return l_error;
}


OFCondition
DcmPixelData::setCurrentRepresentationParameter(
    const DcmRepresentationParameter * repParam)
{
    if (current != repListEnd)
    {
        if (repParam == NULL)
            (*current)->repParam = NULL;
        else
            (*current)->repParam = repParam->clone();
        return EC_Normal;
    }
    return EC_RepresentationNotFound;
}

OFCondition
DcmPixelData::setVR(DcmEVR vr)
{
    unencapsulatedVR = vr;
    return DcmPolymorphOBOW::setVR(vr);
}

void
DcmPixelData::transferEnd()
{
    DcmPolymorphOBOW::transferEnd();
    for (DcmRepresentationListIterator it(repList.begin());
         it != repListEnd;
         ++it)
        (*it)->pixSeq->transferEnd();
}

void
DcmPixelData::transferInit()
{
    DcmPolymorphOBOW::transferInit();
    for (DcmRepresentationListIterator it(repList.begin());
         it != repListEnd;
         ++it)
        (*it)->pixSeq->transferInit();
}

OFCondition DcmPixelData::write(
    DcmOutputStream &outStream,
    const E_TransferSyntax oxfer,
    const E_EncodingType enctype,
    DcmWriteCache *wcache)
{
  errorFlag = EC_Normal;
  if (getTransferState() == ERW_notInitialized) errorFlag = EC_IllegalCall;
  else
  {
    DcmXfer xferSyn(oxfer);
    if (xferSyn.isEncapsulated() && (! alwaysUnencapsulated))
    {
      if (getTransferState() == ERW_init)
      {
        DcmRepresentationListIterator found;
        errorFlag = findConformingEncapsulatedRepresentation(xferSyn, NULL, found);
        if (errorFlag == EC_Normal)
        {
          current = found;
          recalcVR();
          pixelSeqForWrite = (*found)->pixSeq;
          setTransferState(ERW_inWork);
        }
      }
      if (errorFlag == EC_Normal && pixelSeqForWrite) errorFlag = pixelSeqForWrite->write(outStream, oxfer, enctype, wcache);
      if (errorFlag == EC_Normal) setTransferState(ERW_ready);
    }
    else if (existUnencapsulated)
    {
      current = repListEnd;
      recalcVR();
      errorFlag = DcmPolymorphOBOW::write(outStream, oxfer, enctype, wcache);
    }
    else if (getValue() == NULL)
    {
      errorFlag = DcmPolymorphOBOW::write(outStream, oxfer, enctype, wcache);
    } else errorFlag = EC_RepresentationNotFound;
  }
  return errorFlag;
}

OFCondition DcmPixelData::writeXML(
    STD_NAMESPACE ostream&out,
    const size_t flags)
{
    if (current == repListEnd)
    {
        errorFlag = DcmPolymorphOBOW::writeXML(out, flags);
    } else {
        /* pixel sequence (encapsulated data) */
        errorFlag = (*current)->pixSeq->writeXML(out, flags);
    }
    return errorFlag;
}

OFCondition DcmPixelData::writeSignatureFormat(
    DcmOutputStream &outStream,
    const E_TransferSyntax oxfer,
    const E_EncodingType enctype,
    DcmWriteCache *wcache)
{
  errorFlag = EC_Normal;
  if (getTransferState() == ERW_notInitialized) errorFlag = EC_IllegalCall;
  else if (getTag().isSignable())
  {
    DcmXfer xferSyn(oxfer);
    if (xferSyn.isEncapsulated() && (! alwaysUnencapsulated))
    {
      if (getTransferState() == ERW_init)
      {
        DcmRepresentationListIterator found;
        errorFlag = findConformingEncapsulatedRepresentation(xferSyn, NULL, found);
        if (errorFlag == EC_Normal)
        {
          current = found;
          recalcVR();
          pixelSeqForWrite = (*found)->pixSeq;
          setTransferState(ERW_inWork);
        }
      }
      if (errorFlag == EC_Normal && pixelSeqForWrite) errorFlag = pixelSeqForWrite->writeSignatureFormat(outStream, oxfer, enctype, wcache);
      if (errorFlag == EC_Normal) setTransferState(ERW_ready);
    }
    else if (existUnencapsulated)
    {
      current = repListEnd;
      recalcVR();
      errorFlag = DcmPolymorphOBOW::writeSignatureFormat(outStream, oxfer, enctype, wcache);
    }
    else if (getValue() == NULL)
    {
      errorFlag = DcmPolymorphOBOW::writeSignatureFormat(outStream, oxfer, enctype, wcache);
    } else errorFlag = EC_RepresentationNotFound;
  } else errorFlag = EC_Normal;
  return errorFlag;
}

OFCondition DcmPixelData::loadAllDataIntoMemory(void)
{
    if (current == repListEnd)
        return DcmElement::loadAllDataIntoMemory();
    else
        return (*current)->pixSeq->loadAllDataIntoMemory();
}

void DcmPixelData::setNonEncapsulationFlag(OFBool flag)
{
    alwaysUnencapsulated = flag;
}

OFCondition DcmPixelData::getUncompressedFrame(
    DcmItem *dataset,
    Uint32 frameNo,
    Uint32& startFragment,
    void *buffer,
    Uint32 bufSize,
    OFString& decompressedColorModel,
    DcmFileCache *cache)
{
    if ((dataset == NULL) || (buffer == NULL)) return EC_IllegalCall;

    Sint32 numberOfFrames = 1;
    dataset->findAndGetSint32(DCM_NumberOfFrames, numberOfFrames); // don't fail if absent
    if (numberOfFrames < 1) numberOfFrames = 1;

    Uint32 frameSize;
    OFCondition result = getUncompressedFrameSize(dataset, frameSize);
    if (result.bad()) return result;

    // determine the minimum buffer size, which may be frame size plus one pad byte if frame size is odd.
    // We need this extra byte, because the image might be in a different
    // endianness than our host cpu. In this case the decoder will swap
    // the data to the host byte order which could overflow the buffer.
    Uint32 minBufSize = frameSize;
    if (minBufSize & 1) ++minBufSize;

    if (bufSize < minBufSize) return EC_IllegalCall;

    // check frame number
    if (frameNo >= OFstatic_cast(Uint32, numberOfFrames)) return EC_IllegalCall;

    if (existUnencapsulated)
    {
      // we already have an uncompressed version of the pixel data
      // either in memory or in file. We can directly access this using
      // DcmElement::getPartialValue.
      result = getPartialValue(buffer, frameNo * frameSize, frameSize, cache);
      if (result.good()) result = dataset->findAndGetOFString(DCM_PhotometricInterpretation, decompressedColorModel);
    }
    else
    {
      // we only have a compressed version of the pixel data.
      // Identify a codec for decompressing the frame.
      result = DcmCodecList::decodeFrame(
        (*original)->repType, (*original)->repParam, (*original)->pixSeq,
        dataset, frameNo, startFragment, buffer, bufSize, decompressedColorModel);
    }
    return result;
}


OFCondition DcmPixelData::getDecompressedColorModel(
    DcmItem *dataset,
    OFString &decompressedColorModel)
{
    OFCondition result = EC_IllegalCall;
    if (dataset != NULL)
    {
      if (existUnencapsulated)
      {
        // we already have an uncompressed version of the pixel data either in memory or in file,
        // so just retrieve the color model from the given dataset
        result = dataset->findAndGetOFString(DCM_PhotometricInterpretation, decompressedColorModel);
      } else {
        // we only have a compressed version of the pixel data.
        // Identify a codec for determining the color model.
        result = DcmCodecList::determineDecompressedColorModel(
          (*original)->repType, (*original)->repParam, (*original)->pixSeq,
          dataset, decompressedColorModel);
      }
    }
    return result;
}


/*
** CVS/RCS Log:
** $Log: dcpixel.cc,v $
** Revision 1.51  2010-10-20 16:44:16  joergr
** Use type cast macros (e.g. OFstatic_cast) where appropriate.
**
** Revision 1.50  2010-10-14 13:14:08  joergr
** Updated copyright header. Added reference to COPYRIGHT file.
**
** Revision 1.49  2010-07-02 12:34:42  uli
** Added comment explaining why an even buffer size is required.
**
** Revision 1.48  2010-03-01 09:08:45  uli
** Removed some unnecessary include directives in the headers.
**
** Revision 1.47  2009-11-17 16:41:26  joergr
** Added new method that allows for determining the color model of the
** decompressed image.
**
** Revision 1.46  2009-05-11 16:10:14  meichel
** DcmPixelData::getUncompressedFrame() now returns color model also
**   for uncompressed images.
**
** Revision 1.45  2009-05-11 16:06:51  meichel
** DcmPixelData::getUncompressedFrame() now works with uncompressed multi-frame
**   images with odd frame size.
**
** Revision 1.44  2009-01-30 13:28:14  joergr
** Fixed bug in hasRepresentation() which returned the wrong status in case of
** compressed pixel data.
**
** Revision 1.43  2008-11-03 14:29:45  joergr
** Added method createValueFromTempFile() - overrides method in DcmElement.
**
** Revision 1.42  2008-07-17 10:31:31  onken
** Implemented copyFrom() method for complete DcmObject class hierarchy, which
** permits setting an instance's value from an existing object. Implemented
** assignment operator where necessary.
**
** Revision 1.41  2008-05-29 10:46:16  meichel
** Implemented new method DcmPixelData::getUncompressedFrame
**   that permits frame-wise access to compressed and uncompressed
**   objects without ever loading the complete object into main memory.
**   For this new method to work with compressed images, all classes derived from
**   DcmCodec need to implement a new method decodeFrame(). For now, only
**   dummy implementations returning an error code have been defined.
**
** Revision 1.40  2007/11/29 14:30:21  meichel
** Write methods now handle large raw data elements (such as pixel data)
**   without loading everything into memory. This allows very large images to
**   be sent over a network connection, or to be copied without ever being
**   fully in memory.
**
** Revision 1.39  2007/06/29 14:17:49  meichel
** Code clean-up: Most member variables in module dcmdata are now private,
**   not protected anymore.
**
** Revision 1.38  2006/08/15 15:49:54  meichel
** Updated all code in module dcmdata to correctly compile when
**   all standard C++ classes remain in namespace std.
**
** Revision 1.37  2005/12/08 15:41:25  meichel
** Changed include path schema for all DCMTK header files
**
** Revision 1.36  2005/05/26 09:06:55  meichel
** Renamed isIconImage flag to alwaysUnencapsulated to clarify meaning.
**   Added public method DcmPixelData::setNonEncapsulationFlag() that allows
**   DcmCodec instances to enable the flag. Improved documentation.
**
** Revision 1.35  2004/07/01 12:28:12  meichel
** Fixed copy constructor for class DcmPixelData which did not work correctly
**   under certain circumstances due to an uninitialized attribute.
**
** Revision 1.34  2004/04/07 13:56:08  meichel
** Compressed image datasets containing uncompressed icon images
**   are now correctly handled by the parser.
**
** Revision 1.33  2004/01/16 13:49:53  joergr
** Removed acknowledgements with e-mail addresses from CVS log.
**
** Revision 1.32  2003/06/12 18:22:23  joergr
** Modified code to use const_iterators where appropriate (required for STL).
**
** Revision 1.31  2003/06/02 16:55:41  meichel
** Cleaned up implementation of DcmRepresentationEntry, added doc++ comments
**
** Revision 1.30  2003/04/03 14:30:37  joergr
** Fixed bug introduced in the context of the new createUint8/16Array() methods.
**
** Revision 1.29  2003/04/01 12:35:17  joergr
** Added implementation of createUint8/16Array() methods to DcmPixelData.
** Required to work properly with chooseRepresentation() for pixel compression.
**
** Revision 1.28  2003/03/21 13:08:04  meichel
** Minor code purifications for warnings reported by MSVC in Level 4
**
** Revision 1.27  2002/12/09 09:30:54  wilkens
** Modified/Added doc++ documentation.
**
** Revision 1.26  2002/12/06 13:16:57  joergr
** Enhanced "print()" function by re-working the implementation and replacing
** the boolean "showFullData" parameter by a more general integer flag.
** Made source code formatting more consistent with other modules/files.
**
** Revision 1.25  2002/09/17 13:22:45  meichel
** Fixed two bugs in class DcmPixelData.
**
** Revision 1.24  2002/09/10 15:24:04  meichel
** Fixed two issues in parser. Dcmdata will now correctly parse compressed
**   data sets containing uncompressed pixel data (e.g. icon images) and
**   uncompressed data sets containing compressed pixel data (e.g. compressed
**   file but meta-header missing). Note that write-back of such datasets will
**   fail unless appropriate compression codecs are registered.
**
** Revision 1.23  2002/08/27 16:55:54  meichel
** Initial release of new DICOM I/O stream classes that add support for stream
**   compression (deflated little endian explicit VR transfer syntax)
**
** Revision 1.22  2002/07/10 11:49:30  meichel
** Fixed memory leak that occured when compression of an image failed in
**   a compression codec.
**
** Revision 1.21  2002/04/25 10:19:53  joergr
** Added support for XML output of DICOM objects.
**
** Revision 1.20  2001/11/08 16:19:42  meichel
** Changed interface for codec registration. Now everything is thread-safe
**   and multiple codecs can be registered for a single transfer syntax (e.g.
**   one encoder and one decoder).
**
** Revision 1.19  2001/11/01 14:55:42  wilkens
** Added lots of comments.
**
** Revision 1.18  2001/09/25 17:18:36  meichel
** Updated abstract class DcmRepresentationParameter for use with dcmjpeg
**
** Revision 1.17  2001/06/01 15:49:07  meichel
** Updated copyright header
**
** Revision 1.16  2001/05/25 09:53:54  meichel
** Modified DcmCodec::decode() interface, required for future dcmjpeg module.
**
** Revision 1.15  2000/11/07 16:56:21  meichel
** Initial release of dcmsign module for DICOM Digital Signatures
**
** Revision 1.14  2000/09/27 08:19:58  meichel
** Minor changes in DcmCodec interface, required for future dcmjpeg module.
**
** Revision 1.13  2000/04/14 16:09:16  meichel
** Made function DcmCodec and related functions thread safe.
**   registerGlobalCodec() should not be called anymore from the constructor
**   of global objects.
**
** Revision 1.12  2000/03/08 16:26:39  meichel
** Updated copyright header.
**
** Revision 1.11  2000/02/10 16:04:07  joergr
** Fixed bug producing an error message when trying to create an empty
** PixelData element.
**
** Revision 1.10  2000/02/10 10:52:21  joergr
** Added new feature to dcmdump (enhanced print method of dcmdata): write
** pixel data/item value fields to raw files.
**
** Revision 1.9  2000/02/03 16:30:19  joergr
** Fixed bug: encapsulated data (pixel items) have never been loaded using
** method 'loadAllDataIntoMemory'. Therefore, encapsulated pixel data was
** never printed with 'dcmdump'.
**
** Revision 1.8  1999/04/21 15:48:15  meichel
** Fixed bug in DcmPixelData::findConformingEncapsulatedRepresentation
**   leading to an assertion failure when working with compressed data.
**
** Revision 1.7  1999/03/31 09:25:35  meichel
** Updated copyright header in module dcmdata
**
** Revision 1.6  1998/11/12 16:48:17  meichel
** Implemented operator= for all classes derived from DcmObject.
**
** Revision 1.5  1998/07/15 15:52:04  joergr
** Removed several compiler warnings reported by gcc 2.8.1 with
** additional options, e.g. missing copy constructors and assignment
** operators, initialization of member variables in the body of a
** constructor instead of the member initialization list, hiding of
** methods by use of identical names, uninitialized member variables,
** missing const declaration of char pointers. Replaced tabs by spaces.
**
** Revision 1.4  1997/09/18 08:10:55  meichel
** Many minor type conflicts (e.g. long passed as int) solved.
**
** Revision 1.3  1997/07/31 06:59:35  andreas
** Added initialization of error code in
** DcmPixelData::removeOriginalRepresentation()
**
** Revision 1.2  1997/07/24 13:08:28  andreas
** - Removed const for method DcmRepresentationParameter::copy
**
** Revision 1.1  1997/07/21 07:55:05  andreas
** - New environment for encapsulated pixel representations. DcmPixelData
**   can contain different representations and uses codecs to convert
**   between them. Codecs are derived from the DcmCodec class. New error
**   codes are introduced for handling of representations. New internal
**   value representation (only for ident()) for PixelData
**
*/
