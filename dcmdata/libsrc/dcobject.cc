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
 *  Purpose:
 *    This file contains the interface to routines which provide
 *    DICOM object encoding/decoding, search and lookup facilities.
 *
 */


#include "dcmtk/config/osconfig.h"     /* make sure OS specific configuration is included first */

#include "dcmtk/ofstd/ofstd.h"
#include "dcmtk/ofstd/ofstream.h"
#include "dcmtk/dcmdata/dcjson.h"
#include "dcmtk/dcmdata/dcobject.h"
#include "dcmtk/dcmdata/dcdeftag.h"
#include "dcmtk/dcmdata/dcvr.h"
#include "dcmtk/dcmdata/dcxfer.h"
#include "dcmtk/dcmdata/dcswap.h"
#include "dcmtk/dcmdata/dcistrma.h"    /* for class DcmInputStream */
#include "dcmtk/dcmdata/dcostrma.h"    /* for class DcmOutputStream */

#define INCLUDE_CSTDIO
#define INCLUDE_IOMANIP
#include "dcmtk/ofstd/ofstdinc.h"


// global flags

OFGlobal<OFBool>    dcmEnableAutomaticInputDataCorrection(OFTrue);
OFGlobal<OFBool>    dcmAcceptOddAttributeLength(OFTrue);
OFGlobal<OFBool>    dcmEnableCP246Support(OFTrue);
OFGlobal<OFBool>    dcmEnableOldSignatureFormat(OFFalse);
OFGlobal<OFBool>    dcmAutoDetectDatasetXfer(OFFalse);
OFGlobal<OFBool>    dcmAcceptUnexpectedImplicitEncoding(OFFalse);
OFGlobal<OFBool>    dcmPreferVRFromDataDictionary(OFFalse);
OFGlobal<OFBool>    dcmPreferLengthFieldSizeFromDataDictionary(OFFalse);
OFGlobal<OFBool>    dcmReadImplPrivAttribMaxLengthAsSQ(OFFalse);
OFGlobal<OFBool>    dcmIgnoreParsingErrors(OFFalse);
OFGlobal<DcmTagKey> dcmStopParsingAfterElement(DCM_UndefinedTagKey); // (0xffff,0xffff)
OFGlobal<OFBool>    dcmWriteOversizedSeqsAndItemsUndefined(OFTrue);
OFGlobal<OFBool>    dcmIgnoreFileMetaInformationGroupLength(OFFalse);
OFGlobal<OFBool>    dcmReplaceWrongDelimitationItem(OFFalse);
OFGlobal<OFBool>    dcmConvertUndefinedLengthOBOWtoSQ(OFFalse);
OFGlobal<OFBool>    dcmConvertVOILUTSequenceOWtoSQ(OFFalse);
OFGlobal<OFBool>    dcmUseExplLengthPixDataForEncTS(OFFalse);

// ****** public methods **********************************


DcmObject::DcmObject(const DcmTag &tag,
                     const Uint32 len)
: errorFlag(EC_Normal)
, Tag(tag)
, Length(len)
, fTransferState(ERW_init)
, fTransferredBytes(0)
, Parent(NULL)
{
}


DcmObject::DcmObject(const DcmObject &obj)
: errorFlag(obj.errorFlag)
, Tag(obj.Tag)
, Length(obj.Length)
, fTransferState(obj.fTransferState)
, fTransferredBytes(obj.fTransferredBytes)
, Parent(NULL)
{
}


DcmObject::~DcmObject()
{
}


DcmObject &DcmObject::operator=(const DcmObject &obj)
{
    if (this != &obj)
    {
        Tag = obj.Tag;
        Length = obj.Length;
        errorFlag = obj.errorFlag;
        fTransferState = obj.fTransferState;
        fTransferredBytes = obj.fTransferredBytes;
        Parent = NULL;
    }
    return *this;
}


// ********************************


void DcmObject::transferInit()
{
    fTransferState = ERW_init;
    fTransferredBytes = 0;
}


void DcmObject::transferEnd()
{
    fTransferState = ERW_notInitialized;
}


// ********************************


OFBool DcmObject::isNested() const
{
    OFBool nested = OFFalse;
    if (Parent != NULL)
    {
        // check for surrounding structure of item and sequence
        DcmEVR parentIdent = Parent->ident();
        if ((parentIdent == EVR_item) || (parentIdent == EVR_dirRecord))
        {
            if (Parent->getParent() != NULL)
            {
                parentIdent = Parent->getParent()->ident();
                if ((parentIdent == EVR_SQ) || (parentIdent == EVR_pixelSQ))
                    nested = OFTrue;
            }
        }
    }
    return nested;
}


DcmItem *DcmObject::getRootItem()
{
    DcmItem *rootItem = NULL;
    DcmObject *parent = this;
    // search for the root object
    do {
        // stop at top-level dataset/item
        if ((parent->getParent() == NULL) || (parent->getParent()->ident() == EVR_fileFormat))
            break;
        parent = parent->getParent();
    } while (parent != NULL);
    if (parent != NULL)
    {
        // make sure that it is really a class derived from DcmItem
        switch (parent->ident())
        {
            case EVR_metainfo:
            case EVR_dataset:
            case EVR_item:
            case EVR_dirRecord:
                rootItem = OFreinterpret_cast(DcmItem *, parent);
                break;
            default:
                // Don't generate a message when there is no root
                if (this != parent)
                {
                    DCMDATA_DEBUG("DcmObject::getRootItem() Root object has wrong class identifier: "
                        << OFstatic_cast(int, parent->ident())
                        << " (" << DcmVR(parent->ident()).getVRName() << ")");
                }
                break;
        }
    }
    return rootItem;
}


DcmItem *DcmObject::getParentItem()
{
    DcmItem *parentItem = NULL;
    if (Parent != NULL)
    {
        // make sure that it is really a class derived from DcmItem
        switch (Parent->ident())
        {
            case EVR_metainfo:
            case EVR_dataset:
            case EVR_item:
            case EVR_dirRecord:
                parentItem = OFreinterpret_cast(DcmItem *, Parent);
                break;
            default:
                DCMDATA_DEBUG("DcmObject::getParentItem() Parent object has wrong class identifier: "
                    << OFstatic_cast(int, Parent->ident())
                    << " (" << DcmVR(Parent->ident()).getVRName() << ")");
                break;
        }
    }
    return parentItem;
}


// ********************************


DcmObject *DcmObject::nextInContainer(const DcmObject * /*obj*/)
{
    return NULL;
}


OFCondition DcmObject::nextObject(DcmStack & /*stack*/,
                                  const OFBool /*intoSub*/)
{
    return EC_TagNotFound;
}


// ********************************


OFCondition DcmObject::search(const DcmTagKey &/*tag*/,
                              DcmStack &/*resultStack*/,
                              E_SearchMode /*mode*/,
                              OFBool /*searchIntoSub*/)
{
   return EC_TagNotFound;
}


// ********************************


OFCondition DcmObject::writeXML(STD_NAMESPACE ostream& /*out*/,
                                const size_t /*flags*/)
{
    return EC_IllegalCall;
}


// ********************************


OFCondition DcmObject::writeJson(STD_NAMESPACE ostream& /*out*/,
                                 DcmJsonFormat& /*format*/)
{
    return EC_IllegalCall;
}


// ***********************************************************
// ****** protected methods **********************************
// ***********************************************************


void DcmObject::printNestingLevel(STD_NAMESPACE ostream &out,
                                  const size_t flags,
                                  const int level)
{
    if (flags & DCMTypes::PF_showTreeStructure)
    {
        if (flags & DCMTypes::PF_useANSIEscapeCodes)
            out << DCMDATA_ANSI_ESCAPE_CODE_LINE;
        /* show vertical bar for the tree structure */
        for (int i = 1; i < level; i++)
            out << "| ";
    } else {
        /* show nesting level */
        for (int i = 1; i < level; i++)
            out << "  ";
    }
}


void DcmObject::printInfoLineStart(STD_NAMESPACE ostream &out,
                                   const size_t flags,
                                   const int level,
                                   DcmTag *tag)
{
    /* default: use object's tag */
    if (tag == NULL)
        tag = &Tag;
    DcmVR vr(tag->getVR());
    /* show nesting level */
    printNestingLevel(out, flags, level);
    if (flags & DCMTypes::PF_showTreeStructure)
    {
        if (flags & DCMTypes::PF_useANSIEscapeCodes)
        {
            if (*tag == DCM_Item)
                out << DCMDATA_ANSI_ESCAPE_CODE_ITEM;
            else if ((vr.getEVR() == EVR_SQ) || (vr.getEVR() == EVR_pixelSQ))
            {
                if (level == 1)
                    out << DCMDATA_ANSI_ESCAPE_CODE_SEQUENCE_1;
                else
                    out << DCMDATA_ANSI_ESCAPE_CODE_SEQUENCE;
            } else if (level == 1)
                out << DCMDATA_ANSI_ESCAPE_CODE_NAME_1;
            else
                out << DCMDATA_ANSI_ESCAPE_CODE_NAME;
        }
        /* print tag name */
        out << tag->getTagName() << ' ';
        /* add padding spaces if required */
        const STD_NAMESPACE ptrdiff_t padLength = DCM_OptPrintAttributeNameLength - strlen(tag->getTagName()) - 2 * level;
        if (padLength > 0)
            out << OFString(OFstatic_cast(size_t, padLength), ' ');
    } else {
        if (flags & DCMTypes::PF_useANSIEscapeCodes)
            out << DCMDATA_ANSI_ESCAPE_CODE_TAG;
        /* print line start: tag */
        out << *tag << " ";
        if (flags & DCMTypes::PF_useANSIEscapeCodes)
            out << DCMDATA_ANSI_ESCAPE_CODE_VR;
        /* print line start: VR */
        out << vr.getVRName() << " "
            << STD_NAMESPACE dec << STD_NAMESPACE setfill(' ');
    }
    /* set color for subsequent element value */
    if (flags & DCMTypes::PF_useANSIEscapeCodes)
        out << DCMDATA_ANSI_ESCAPE_CODE_VALUE;
}


void DcmObject::printInfoLineEnd(STD_NAMESPACE ostream &out,
                                 const size_t flags,
                                 const unsigned long printedLength,
                                 DcmTag *tag)
{
    unsigned long vm = 0;
    unsigned long length = 0;
    /* default: use object's tag, VM and length */
    if (tag == NULL)
    {
        tag = &Tag;
        vm = getVM();
        length = Length;
    }
    if (!(flags & DCMTypes::PF_showTreeStructure))
    {
        /* fill with spaces if necessary */
        if (printedLength < DCM_OptPrintValueLength)
            out << OFString(OFstatic_cast(size_t, DCM_OptPrintValueLength - printedLength), ' ');
        if (flags & DCMTypes::PF_useANSIEscapeCodes)
            out << DCMDATA_ANSI_ESCAPE_CODE_COMMENT;
        out << " # ";
        /* print line end: length */
        if (flags & DCMTypes::PF_useANSIEscapeCodes)
            out << DCMDATA_ANSI_ESCAPE_CODE_LENGTH;
        if (length == DCM_UndefinedLength)
            out << "u/l";   // means "undefined/length"
        else
            out << STD_NAMESPACE setw(3) << length;
        if (flags & DCMTypes::PF_useANSIEscapeCodes)
            out << DCMDATA_ANSI_ESCAPE_CODE_COMMENT;
        out << ",";
        /* print line end: VM */
        if (flags & DCMTypes::PF_useANSIEscapeCodes)
            out << DCMDATA_ANSI_ESCAPE_CODE_VM;
        out << STD_NAMESPACE setw(2) << vm << " ";
        /* print line end: name */
        if (flags & DCMTypes::PF_useANSIEscapeCodes)
            out << DCMDATA_ANSI_ESCAPE_CODE_NAME;
        out << tag->getTagName();
    }
    /* reset all colors and styles */
    if (flags & DCMTypes::PF_useANSIEscapeCodes)
        out << DCMDATA_ANSI_ESCAPE_CODE_RESET;
    /* finish the current line */
    out << OFendl;
}


void DcmObject::printInfoLine(STD_NAMESPACE ostream &out,
                              const size_t flags,
                              const int level,
                              const char *info,
                              DcmTag *tag,
                              const OFBool isInfo)
{
    /* print tag and VR */
    printInfoLineStart(out, flags, level, tag);
    /* check whether info text fits into the limit */
    size_t printedLength = 0;
    /* check for valid info text */
    if (info != NULL)
    {
        if (isInfo && (flags & DCMTypes::PF_useANSIEscapeCodes))
            out << DCMDATA_ANSI_ESCAPE_CODE_INFO;
        /* check info text length */
        printedLength = strlen(info);
        if (printedLength > DCM_OptPrintValueLength)
        {
            /* check whether full info text should be printed */
            if ((flags & DCMTypes::PF_shortenLongTagValues) && (printedLength > DCM_OptPrintLineLength))
            {
                char output[DCM_OptPrintLineLength + 1];
                /* truncate info text and append "..." */
                OFStandard::strlcpy(output, info, OFstatic_cast(size_t, DCM_OptPrintLineLength) - 3 /* for "..." */ + 1);
                OFStandard::strlcat(output, "...", OFstatic_cast(size_t, DCM_OptPrintLineLength) + 1);
                out << output;
                printedLength = DCM_OptPrintLineLength;
            } else
                out << info;
        } else
            out << info;
    }
    /* print length, VM and tag name */
    printInfoLineEnd(out, flags, OFstatic_cast(unsigned long, printedLength), tag);
}


// ********************************


OFCondition DcmObject::writeTag(DcmOutputStream &outStream,
                                const DcmTag &tag,
                                const E_TransferSyntax oxfer)
    /*
     * This function writes the tag information which was passed to the stream. When
     * writing information, the transfer syntax which was passed is accounted for.
     *
     * Parameters:
     *   outStream - [out] The stream that the information will be written to.
     *   tag       - [in] The tag which shall be written.
     *   oxfer     - [in] The transfer syntax which shall be used.
     */
{
    /* create an object which represents the transfer syntax */
    DcmXfer outXfer(oxfer);
    /* determine the byte ordering */
    const E_ByteOrder outByteOrder = outXfer.getByteOrder();
    /* if the byte ordering is unknown, this is an illegal call (return error) */
    if (outByteOrder == EBO_unknown)
        return EC_IllegalCall;
    /* determine the group number, mind the transfer syntax and */
    /* write the group number value (2 bytes) to the stream */
    Uint16 groupTag = tag.getGTag();
    swapIfNecessary(outByteOrder, gLocalByteOrder, &groupTag, 2, 2);
    outStream.write(&groupTag, 2);
    /* determine the element number, mind the transfer syntax and */
    /* write the element number value (2 bytes) to the stream */
    Uint16 elementTag = tag.getETag();    // 2 byte length;
    swapIfNecessary(outByteOrder, gLocalByteOrder, &elementTag, 2, 2);
    outStream.write(&elementTag, 2);
    /* if the stream reports an error return this error, else return ok */
    return outStream.status();
}


Uint32 DcmObject::getTagAndLengthSize(const E_TransferSyntax oxfer) const
{
    /* create an object which represents the transfer syntax */
    DcmXfer oxferSyn(oxfer);

    if (oxferSyn.isExplicitVR())
    {
       /* map "UN" to "OB" if generation of "UN" is disabled */
       DcmVR outvr(getTag().getVR().getValidEVR());

       if (Length > 0xffff || outvr.usesExtendedLengthEncoding())
       {
         // we are either using extended length encoding or the
         // element length is > 64k (i.e. we have to convert to OB/UN).
         // In any case we need a 12-byte header field.
         // This is also the case for any object with undefined length,
         // so we don't need to check that as a special case.
          return 12;
       }
    }
    return 8;
}


OFCondition DcmObject::writeTagAndLength(DcmOutputStream &outStream,
                                         const E_TransferSyntax oxfer,
                                         Uint32 &writtenBytes) const
    /*
     * This function writes this DICOM object's tag and length information to the stream. When
     * writing information, the transfer syntax which was passed is accounted for. If the transfer
     * syntax shows an explicit value representation, the data type of this object is also written
     * to the stream. In general, this function follows the rules which are specified in the DICOM
     * standard (see DICOM standard part 5, section 7) concerning the encoding of a data set which
     * shall be transmitted.
     *
     * Parameters:
     *   outStream    - [out] The stream that the information will be written to.
     *   oxfer        - [in] The transfer syntax which shall be used.
     *   writtenBytes - [out] Contains in the end the amount of bytes which have been written to the stream.
     */
{
    /* check the error status of the stream. If it is not ok, nothing can be done */
    OFCondition l_error = outStream.status();
    if (l_error.bad())
    {
        writtenBytes = 0;
    } else {
        /* if the stream is ok, we need to do something */

        /* write the tag information (a total of 4 bytes, group number and element */
        /* number) to the stream. Mind the transfer syntax's byte ordering. */
        l_error = writeTag(outStream, getTag(), oxfer);
        writtenBytes = 4;

        /* create an object which represents the transfer syntax */
        DcmXfer oxferSyn(oxfer);

        /* determine the byte ordering */
        const E_ByteOrder oByteOrder = oxferSyn.getByteOrder();

        /* if the byte ordering is unknown, this is an illegal call (return error) */
        if (oByteOrder == EBO_unknown)
            return EC_IllegalCall;

        /* if the transfer syntax is one with explicit value representation */
        /* this value's data type also has to be written to the stream. Do so */
        /* and also write the length information to the stream. */
        if (oxferSyn.isExplicitVR())
        {
            /* Create an object that represents this object's data type */
            DcmVR myvr(getVR());

            /* getValidEVR() will convert datatype "UN" to "OB" if generation of "UN" is disabled */
            DcmEVR vr = myvr.getValidEVR();
            myvr.setVR(vr);

            if (Length > 0xffff && (!myvr.usesExtendedLengthEncoding()))
            {
              // Attribute length is larger than 64 kBytes.
              // We need to encode this as UN (or OB, if generation of UN is disabled
              if (dcmEnableUnknownVRGeneration.get()) vr = EVR_UN; else vr = EVR_OB;
              myvr.setVR(vr);
            }

            /* get name of data type */
            const char *vrname = myvr.getValidVRName();

            /* write data type name to the stream (a total of 2 bytes) */
            outStream.write(vrname, 2);
            writtenBytes += 2;

            /* create another data type object on the basis of the above created object */
            DcmVR outvr(vr);

            /* in case we are dealing with a transfer syntax with explicit VR (see if above) */
            /* and the actual VR uses extended length encoding (see DICOM standard part 5, */
            /* section 7.1.2) we have to add 2 reserved bytes (set to a value of 00H) to the */
            /* data type field and the actual length field is 4 bytes wide. Write the */
            /* corresponding information to the stream. */
            if (outvr.usesExtendedLengthEncoding())
            {
                Uint16 reserved = 0;
                outStream.write(&reserved, 2);                                      // write 2 reserved bytes to stream
                Uint32 valueLength = Length;                                        // determine length
                swapIfNecessary(oByteOrder, gLocalByteOrder, &valueLength, 4, 4);   // mind transfer syntax
                outStream.write(&valueLength, 4);                                   // write length, 4 bytes wide
                writtenBytes += 6;                                                  // remember that 6 bytes were written in total
            }
            /* in case that we are dealing with a transfer syntax with explicit VR (see if above) and */
            /* the actual VR does not use extended length encoding (see DICOM standard part 5, section */
            /* 7.1.2) we do not have to add reserved bytes to the data type field and the actual length */
            /* is 2 bytes wide. Write the corresponding information to the stream. But, make sure that */
            /* the length really fits into the 2-byte field ... */
            else if (Length <= 0xffff)
            {
                Uint16 valueLength = OFstatic_cast(Uint16, Length);                 // determine length (cast to 16 bit)
                swapIfNecessary(oByteOrder, gLocalByteOrder, &valueLength, 2, 2);   // mind transfer syntax
                outStream.write(&valueLength, 2);                                   // write length, 2 bytes wide
                writtenBytes += 2;                                                  // remember that 2 bytes were written in total
            }
            /* ... if not, report an error message and return an error code.
             * This should never happen because we automatically convert such
             * elements to UN/OB, but just in case, we leave the check in here.
             */
            else {
                DcmTag tag(Tag);
                DCMDATA_ERROR("DcmObject: Length of element " << tag.getTagName() << " " << tag
                    << " exceeds maximum of 16-bit length field");
                l_error = EC_ElemLengthExceeds16BitField;
            }
         }
         /* if the transfer syntax is one with implicit value representation this value's data type */
         /* does not have to be written to the stream. Only the length information has to be written */
         /* to the stream. According to the DICOM standard the length field is in this case always 4 */
         /* byte wide. (see DICOM standard part 5, section 7.1.2) */
         else {
            Uint32 valueLength = Length;                                          // determine length
            swapIfNecessary(oByteOrder, gLocalByteOrder, &valueLength, 4, 4);     // mind transfer syntax
            outStream.write(&valueLength, 4);                                     // write length, 4 bytes wide
            writtenBytes += 4;                                                    // remember that 4 bytes were written in total
         }
    }

    /* return result */
    return l_error;
}


OFBool DcmObject::isSignable() const
{
    return Tag.isSignable();
}


OFBool DcmObject::containsUnknownVR() const
{
    return Tag.isUnknownVR();
}


OFBool DcmObject::containsExtendedCharacters(const OFBool /*checkAllStrings*/)
{
    return OFFalse;
}


OFBool DcmObject::isAffectedBySpecificCharacterSet() const
{
    return OFFalse;
}


OFCondition DcmObject::convertCharacterSet(DcmSpecificCharacterSet & /*converter*/)
{
    return EC_Normal;
}


OFBool DcmObject::isEmpty(const OFBool /*normalize*/)
{
    return (Length == 0);
}
