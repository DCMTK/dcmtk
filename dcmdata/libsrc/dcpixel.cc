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
 *  Author:  Andreas Barth
 *
 *  Purpose: class DcmPixelData
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 2001-05-25 09:53:54 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/libsrc/dcpixel.cc,v $
 *  CVS/RCS Revision: $Revision: 1.16 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcpixel.h"
#include "dccodec.h"
#include "dcpixseq.h"

//
// class DcmRepresentationEntry
//


class DcmRepresentationEntry
{
friend class DcmPixelData;
private:
    E_TransferSyntax repType;
    DcmRepresentationParameter * repParam;
    DcmPixelSequence * pixSeq;

    DcmRepresentationEntry(
        const DcmRepresentationEntry & oldEntry);
    DcmRepresentationEntry(
        const E_TransferSyntax rt,
        const DcmRepresentationParameter * rp,
        DcmPixelSequence * pixSeq);
    ~DcmRepresentationEntry();
    OFBool operator==(
        const DcmRepresentationEntry & x) const;
    OFBool operator!=(
        const DcmRepresentationEntry & x) const { return !(*this == x); }

 // --- declarations to avoid compiler warnings
 
    DcmRepresentationEntry &operator=(const DcmRepresentationEntry &);
};


DcmRepresentationEntry::DcmRepresentationEntry(
    const E_TransferSyntax rt,
    const DcmRepresentationParameter *rp,
    DcmPixelSequence * ps)
  : repType(rt),
    repParam(NULL),
    pixSeq(ps)
{
    if (rp)
        repParam = rp->copy();
}

DcmRepresentationEntry::DcmRepresentationEntry(
    const DcmRepresentationEntry & oldEntry) 
  : repType(oldEntry.repType),
    repParam(NULL),
    pixSeq(NULL)
{
    if (oldEntry.repParam)
        repParam = oldEntry.repParam->copy();
    pixSeq = new DcmPixelSequence(*(oldEntry.pixSeq));
}

DcmRepresentationEntry::~DcmRepresentationEntry()
{
    if (repParam)
        delete repParam;
    if (pixSeq)
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
    unencapsulatedVR(EVR_UNKNOWN),
    pixelSeqForWrite(NULL)
{
    repListEnd = repList.end();
    current = original = repListEnd;
    if (Tag.getEVR() == EVR_ox) Tag.setVR(EVR_OW);
    unencapsulatedVR = Tag.getEVR();
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
    unencapsulatedVR(oldPixelData.unencapsulatedVR),
    pixelSeqForWrite(NULL)
{
    repListEnd = repList.end();
    original = repListEnd;
    current = original;
    recalcVR();
    DcmRepresentationListIterator oldEnd(oldPixelData.repList.end());
    for (DcmRepresentationListIterator it(oldPixelData.repList.begin());
         it != oldEnd;
         ++it)
    {
        DcmRepresentationEntry * repEnt = new DcmRepresentationEntry(**it);
        repList.push_back(repEnt);
        if (it == oldPixelData.original)
            original = --repList.end();
        if (it == current)
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
    unencapsulatedVR = obj.unencapsulatedVR;
    pixelSeqForWrite = NULL;
    repList.clear();
    repListEnd = repList.end();
    original = repListEnd;
    current = original;
    recalcVR();
    DcmRepresentationListIterator oldEnd(obj.repList.end());
    DcmRepresentationListIterator it(obj.repList.begin());
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

// methods in alphabetical order

Uint32 
DcmPixelData::calcElementLength(
    const E_TransferSyntax xfer,
    const E_EncodingType enctype)
{
    DcmXfer xferSyn(xfer);
    errorFlag = EC_Normal;
    Uint32 elementLength = 0;

    if (xferSyn.isEncapsulated())
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
        (toType.isEncapsulated() && 
         findRepresentationEntry(findEntry, resultIt) == EC_Normal))
    {
        // representation found
        result = OFTrue;
    }
    else
    {
        const DcmCodecStruct * codecStruct;
        if (original == repListEnd)
        {
            codecStruct = searchGlobalCodec(toType.getXfer());
            if (codecStruct)
                result = codecStruct->getCodec()->canChangeCoding(
                    EXS_LittleEndianExplicit, toType.getXfer());
        }
        else if (toType.isEncapsulated())
        {
            codecStruct = searchGlobalCodec(toType.getXfer());
            if (codecStruct)
            {
                result = codecStruct->getCodec()->canChangeCoding(
                    (*original)->repType, toType.getXfer());
                if (!result)
                {
                    result = canChooseRepresentation(EXS_LittleEndianExplicit, 
                                                     NULL);
                    if (result)
                        result = codecStruct->getCodec()->canChangeCoding(
                            EXS_LittleEndianExplicit, toType.getXfer());
                }
            }
        }
        else
        {
            codecStruct = searchGlobalCodec((*original)->repType);
            if (codecStruct && codecStruct->getCodec()->canChangeCoding(
                (*original)->repType, EXS_LittleEndianExplicit))
                result = OFTrue;
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
    OFBool result = !newXferSyn.isEncapsulated() && existUnencapsulated;

    if (!result && newXferSyn.isEncapsulated())
        result =
            findConformingEncapsulatedRepresentation(newXferSyn, NULL, found) 
            == EC_Normal;
    return result;
}

E_Condition 
DcmPixelData::chooseRepresentation(
        const E_TransferSyntax repType,
        const DcmRepresentationParameter * repParam,
        DcmStack & pixelStack)
{
    E_Condition l_error = EC_CannotChangeRepresentation;
    DcmXfer toType(repType);

    const DcmRepresentationEntry findEntry(repType, repParam, NULL);
    DcmRepresentationListIterator result(repListEnd);
    if ((!toType.isEncapsulated() && existUnencapsulated) ||
        (toType.isEncapsulated() && 
         findRepresentationEntry(findEntry, result) == EC_Normal))
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
    DcmRepresentationListIterator it(repList.begin());
    DcmRepresentationListIterator del;
    while (it != repListEnd)
    {
        if (it != leaveInList)
        {
            delete *it;
            del = it++;
            repList.erase(del);
        }
        else
            ++it;
    }
}

E_Condition
DcmPixelData::decode(
    const DcmXfer & fromType,
    const DcmRepresentationParameter * fromParam,
    DcmPixelSequence * fromPixSeq,
    DcmStack & pixelStack)
{
    if (existUnencapsulated) return EC_Normal;
    E_Condition l_error = EC_CannotChangeRepresentation;
    
    const DcmCodecStruct * codecStruct = searchGlobalCodec(fromType.getXfer());
    if (codecStruct)
    {
        const DcmCodec * codec = codecStruct->getCodec();
        if (codec && codec->canChangeCoding(fromType.getXfer(), 
                                            EXS_LittleEndianExplicit))
        {
            l_error = codec->decode(fromParam, fromPixSeq, *this, codecStruct->getCodecParameter(), pixelStack);
            if (l_error == EC_Normal)
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
        }
    }
    return l_error;
}
            


E_Condition 
DcmPixelData::encode(
    const DcmXfer & fromType, 
    const DcmRepresentationParameter * fromParam, 
    DcmPixelSequence * fromPixSeq, 
    const DcmXfer & toType, 
    const DcmRepresentationParameter *toParam,
    DcmStack & pixelStack)
{
    E_Condition l_error = EC_CannotChangeRepresentation;
    if (toType.isEncapsulated())
    {
        const DcmCodecStruct * codecStruct = searchGlobalCodec(toType.getXfer());
        if (codecStruct)
        {
            const DcmCodec * codec = codecStruct->getCodec();
            if (codec && codec->canChangeCoding(fromType.getXfer(), 
                                                toType.getXfer()))
            {
                if (!toParam)
                    toParam = codecStruct->getDefaultRepresentationParameter();
                    
                DcmPixelSequence * toPixSeq = NULL;
                if (fromType.isEncapsulated())
                    l_error = codec->encode(fromType.getXfer(), fromParam, 
                                            fromPixSeq,
                                            toParam, toPixSeq, 
                                            codecStruct->getCodecParameter(), 
                                            pixelStack);
                else
                {
                    Uint16 * pixelData;
                    l_error = 
                        DcmPolymorphOBOW::getUint16Array(pixelData);
                    Uint32 length = DcmPolymorphOBOW::getLength();
                    if (l_error == EC_Normal)
                        l_error = codec->encode(pixelData, length, toParam, 
                                                toPixSeq, 
                                                codecStruct->getCodecParameter(),
                                                pixelStack);
                }
                if (l_error == EC_Normal)
                {
                    current = insertRepresentationEntry(
                        new DcmRepresentationEntry(toType.getXfer(), toParam, 
                                                   toPixSeq));
                    recalcVR();
                }
            }
            // if it was possible to convert one encapsulated syntax into
            // another directly try it using decoding and encoding!
            if (l_error != EC_Normal && codec && fromType.isEncapsulated())
            {
                l_error = decode(fromType, fromParam, fromPixSeq, pixelStack);
                if (l_error == EC_Normal)
                    l_error = encode(EXS_LittleEndianExplicit, NULL, NULL, 
                                     toType, toParam, pixelStack);
            }
        }
    }
    return l_error;
}

E_Condition
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


E_Condition
DcmPixelData::findConformingEncapsulatedRepresentation(
    const DcmXfer & repTypeSyn,
    const DcmRepresentationParameter * repParam,
    DcmRepresentationListIterator & result)
{
    E_TransferSyntax repType = repTypeSyn.getXfer();
    result = repListEnd;
    E_Condition l_error = EC_RepresentationNotFound;
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
                } else ++it;
              } else ++it;
            }
        }
    }
    return l_error;
}


E_Condition
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
    else
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
        return findConformingEncapsulatedRepresentation(
            repTypeSyn, repParam, found) != EC_Normal;
    else 
        return OFFalse;
}
        

Uint32 
DcmPixelData::getLength(const E_TransferSyntax xfer,
                        const E_EncodingType enctype)
{
    DcmXfer xferSyn(xfer);
    errorFlag = EC_Normal;
    Uint32 valueLength = 0;

    if (xferSyn.isEncapsulated())
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
    if (findRepresentationEntry(*repEntry, result) 
        == EC_Normal)
    {
        insertedEntry = repList.insert(result, repEntry);
        repList.erase(result);
    }
    else
        insertedEntry = repList.insert(result,repEntry);
    return insertedEntry;
}

void
DcmPixelData::print(
    ostream & out, 
    const OFBool showFullData, 
    const int level,
    const char *pixelFileName,
    size_t *pixelCounter)
{
    if (current == repListEnd)
    {
        printPixel(out, showFullData, level, pixelFileName, pixelCounter);
    } else
        (*current)->pixSeq->print(out, showFullData, level, pixelFileName, pixelCounter);
}

E_Condition
DcmPixelData::putUint16Array(
    const Uint16 * wordValue,
    const unsigned long length)
{
    // clear RepresentationList
    clearRepresentationList(repListEnd);
    E_Condition l_error = DcmPolymorphOBOW::putUint16Array(wordValue, length);
    original = current = repListEnd;
    recalcVR();
    existUnencapsulated = OFTrue;
    return l_error;
}

E_Condition
DcmPixelData::putUint8Array(
    const Uint8 * byteValue,
    const unsigned long length)
{
    // clear RepresentationList
    clearRepresentationList(repListEnd);
    E_Condition l_error = DcmPolymorphOBOW::putUint8Array(byteValue, length);
    original = current = repListEnd;
    recalcVR();
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

E_Condition
DcmPixelData::read(
    DcmStream & inStream,
    const E_TransferSyntax ixfer,
    const E_GrpLenEncoding glenc,
    const Uint32 maxReadLength)
{
    if (fTransferState == ERW_notInitialized)
        errorFlag = EC_IllegalCall;
    else
    {
        if (fTransferState == ERW_init)
            clearRepresentationList(repListEnd);

        DcmXfer ixferSyn(ixfer);
        if (!ixferSyn.isEncapsulated())
        {
            if (fTransferState == ERW_init)
            {
                current = original = repListEnd;
                unencapsulatedVR = Tag.getEVR();
                recalcVR();
                existUnencapsulated = OFTrue;
            }
            errorFlag = 
                DcmPolymorphOBOW::read(inStream, ixfer, glenc, maxReadLength);
        }
        else
        {
            if (fTransferState == ERW_init)
            {
                current = insertRepresentationEntry(
                    new DcmRepresentationEntry(
                        ixfer, NULL, new DcmPixelSequence(Tag, Length)));
                recalcVR();
                original = current;
                existUnencapsulated = OFFalse;
                fTransferState = ERW_inWork;
            }

            errorFlag = 
                (*current)->pixSeq->read(inStream, ixfer, glenc, maxReadLength);
            if (errorFlag == EC_Normal)
                fTransferState = ERW_ready;
        }
    }
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
    
E_Condition
DcmPixelData::removeOriginalRepresentation(
    const E_TransferSyntax repType,
    const DcmRepresentationParameter * repParam)
{
    E_Condition l_error = EC_Normal;
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

E_Condition
DcmPixelData::removeRepresentation(
    const E_TransferSyntax repType,
    const DcmRepresentationParameter * repParam)
{
    E_Condition l_error = EC_Normal;
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


E_Condition 
DcmPixelData::setCurrentRepresentationParameter(
    const DcmRepresentationParameter * repParam)
{
    if (current != repListEnd)
    {
        if (repParam == NULL)
            (*current)->repParam = NULL;
        else
            (*current)->repParam = repParam->copy();
        return EC_Normal;
    }
    return EC_RepresentationNotFound;
}

E_Condition
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

E_Condition DcmPixelData::write(
    DcmStream & outStream,
    const E_TransferSyntax oxfer,
    const E_EncodingType enctype)
{
  errorFlag = EC_Normal;
  if (fTransferState == ERW_notInitialized) errorFlag = EC_IllegalCall;
  else
  {
    DcmXfer xferSyn(oxfer);
    if (xferSyn.isEncapsulated())
    {
      if (fTransferState == ERW_init)
      {
        DcmRepresentationListIterator found; 
        errorFlag = findConformingEncapsulatedRepresentation(xferSyn, NULL, found);
        if (errorFlag == EC_Normal)
        {
          current = found;
          recalcVR();
          pixelSeqForWrite = (*found)->pixSeq;
          fTransferState = ERW_inWork;
        }
      }
      if (errorFlag == EC_Normal && pixelSeqForWrite) errorFlag = pixelSeqForWrite->write(outStream, oxfer, enctype);
      if (errorFlag == EC_Normal) fTransferState = ERW_ready;
    }
    else if (existUnencapsulated)
    {
      current = repListEnd;
      recalcVR();
      errorFlag = DcmPolymorphOBOW::write(outStream, oxfer, enctype);
    }
    else if (getValue() == NULL)
    {
      errorFlag = DcmPolymorphOBOW::write(outStream, oxfer, enctype);
    } else errorFlag = EC_RepresentationNotFound;
  }
  return errorFlag;
}

E_Condition DcmPixelData::writeSignatureFormat(
    DcmStream & outStream,
    const E_TransferSyntax oxfer,
    const E_EncodingType enctype)
{
  errorFlag = EC_Normal;
  if (fTransferState == ERW_notInitialized) errorFlag = EC_IllegalCall;
  else if (Tag.isSignable())
  {
    DcmXfer xferSyn(oxfer);
    if (xferSyn.isEncapsulated())
    {
      if (fTransferState == ERW_init)
      {
        DcmRepresentationListIterator found; 
        errorFlag = findConformingEncapsulatedRepresentation(xferSyn, NULL, found);
        if (errorFlag == EC_Normal)
        {
          current = found;
          recalcVR();
          pixelSeqForWrite = (*found)->pixSeq;
          fTransferState = ERW_inWork;
        }
      }
      if (errorFlag == EC_Normal && pixelSeqForWrite) errorFlag = pixelSeqForWrite->writeSignatureFormat(outStream, oxfer, enctype);
      if (errorFlag == EC_Normal) fTransferState = ERW_ready;
    }
    else if (existUnencapsulated)
    {
      current = repListEnd;
      recalcVR();
      errorFlag = DcmPolymorphOBOW::writeSignatureFormat(outStream, oxfer, enctype);
    }
    else if (getValue() == NULL)
    {
      errorFlag = DcmPolymorphOBOW::writeSignatureFormat(outStream, oxfer, enctype);
    } else errorFlag = EC_RepresentationNotFound;
  } else errorFlag = EC_Normal;
  return errorFlag;
}

E_Condition DcmPixelData::loadAllDataIntoMemory(void)
{
    if (current == repListEnd)
        return DcmElement::loadAllDataIntoMemory();
    else
        return (*current)->pixSeq->loadAllDataIntoMemory();
}

/*
** CVS/RCS Log:
** $Log: dcpixel.cc,v $
** Revision 1.16  2001-05-25 09:53:54  meichel
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
