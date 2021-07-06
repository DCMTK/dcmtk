/*
 *
 *  Copyright (C) 1996-2021, OFFIS e.V.
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
 *  Module:  dcmimgle
 *
 *  Author:  Joerg Riesmeier
 *
 *  Purpose: DicomDocument (Source)
 *
 */


#include "dcmtk/config/osconfig.h"

#include "dcmtk/dcmdata/dctk.h"
#include "dcmtk/ofstd/ofstring.h"

#include "dcmtk/dcmimgle/didocu.h"
#include "dcmtk/dcmimgle/diutils.h"


/*----------------*
 *  constructors  *
 *----------------*/

DiDocument::DiDocument(const char *filename,
                       const unsigned long flags,
                       const unsigned long fstart,
                       const unsigned long fcount)
  : Object(NULL),
    FileFormat(new DcmFileFormat()),
    PixelData(NULL),
    Xfer(EXS_Unknown),
    FrameStart(fstart),
    FrameCount(fcount),
    Flags(flags),
    PhotometricInterpretation()
{
    if (FileFormat)
    {
        if (FileFormat->loadFile(filename).bad())
        {
            DCMIMGLE_ERROR("can't read file '" << filename << "'");
            delete FileFormat;
            FileFormat = NULL;
        } else {
            Object = FileFormat->getDataset();
            if (Object != NULL)
            {
                Xfer = OFstatic_cast(DcmDataset *, Object)->getOriginalXfer();
                convertPixelData();
            }
        }
    }
}


DiDocument::DiDocument(DcmObject *object,
                       const E_TransferSyntax xfer,
                       const unsigned long flags,
                       const unsigned long fstart,
                       const unsigned long fcount)
  : Object(NULL),
    FileFormat(NULL),
    PixelData(NULL),
    Xfer(xfer),
    FrameStart(fstart),
    FrameCount(fcount),
    Flags(flags),
    PhotometricInterpretation()
{
    if (object != NULL)
    {
        const DcmEVR classType = object->ident();
        // check whether given DICOM object has a valid type
        if (classType == EVR_fileFormat)
        {
            // store reference to DICOM file format to be deleted on object destruction
            if (Flags & CIF_TakeOverExternalDataset)
                FileFormat = OFstatic_cast(DcmFileFormat *, object);
            Object = OFstatic_cast(DcmFileFormat *, object)->getDataset();
        }
        else if ((classType == EVR_dataset) || (classType == EVR_item))
            Object = object;
        else
            DCMIMGLE_ERROR("invalid DICOM object passed to constructor (wrong class)");
        if (Object != NULL)
        {
            // try to determine the transfer syntax from the given object
            if (Xfer == EXS_Unknown)
            {
                // check type before casting the object
                if (Object->ident() == EVR_dataset)
                    Xfer = OFstatic_cast(DcmDataset *, Object)->getOriginalXfer();
                else // could only be an item
                    DCMIMGLE_WARN("can't determine original transfer syntax from given DICOM object");
            }
            convertPixelData();
        }
    }
}


void DiDocument::convertPixelData()
{
    DcmStack pstack;
    OFCondition status;
    DcmXfer xfer(Xfer);
    DCMIMGLE_DEBUG("transfer syntax of DICOM dataset: " << xfer.getXferName() << " (" << xfer.getXferID() << ")");
    // only search on main dataset level
    if (search(DCM_PixelData, pstack))
    {
        DcmObject *pobject = pstack.top();
        if (pobject != NULL)
        {
            // check for correct class before type casting
            if (pobject->ident() == EVR_PixelData)
            {
                PixelData = OFstatic_cast(DcmPixelData *, pobject);
                // check for a special (faulty) case where the original pixel data is uncompressed and
                // the transfer syntax of the dataset refers to encapsulated format (i.e. compression)
                if (Object->ident() == EVR_dataset)
                {
                    E_TransferSyntax repType = EXS_Unknown;
                    const DcmRepresentationParameter *repParam = NULL;
                    PixelData->getOriginalRepresentationKey(repType, repParam);
                    if (xfer.isEncapsulated() && !DcmXfer(repType).isEncapsulated())
                    {
                        DCMIMGLE_WARN("pixel data is stored in uncompressed format, although "
                            << "the transfer syntax of the dataset refers to encapsulated format");
                    }
                }
                // convert pixel data to uncompressed format (if required)
                if ((Flags & CIF_DecompressCompletePixelData) || !(Flags & CIF_UsePartialAccessToPixelData))
                {
                    if (Object->ident() == EVR_dataset)
                    {
                        // Call DcmDataset::chooseRepresentation() to enable an update
                        // of the transfer syntax attributes of the dataset instance
                        status = OFstatic_cast(DcmDataset *, Object)->chooseRepresentation(EXS_LittleEndianExplicit, NULL);
                    }
                    else
                    {
                        // Object is a DcmItem instance. Directly call DcmPixelData::chooseRepresentation().
                        pstack.clear();
                        // push reference to DICOM dataset on the stack (required for decompression process)
                        pstack.push(Object);
                        // dummy stack entry
                        pstack.push(PixelData);
                        status = PixelData->chooseRepresentation(EXS_LittleEndianExplicit, NULL, pstack);
                    }
                    if (status.good())
                    {
                        // set transfer syntax to unencapsulated/uncompressed
                        if (xfer.isEncapsulated())
                        {
                            Xfer = EXS_LittleEndianExplicit;
                            DCMIMGLE_DEBUG("decompressed complete pixel data in memory: " << PixelData->getLength(Xfer) << " bytes");
                        }
                    } else {
                        DCMIMGLE_ERROR("can't change to unencapsulated representation for pixel data");
                        DCMIMGLE_DEBUG("DcmPixelData::chooseRepresentation() returned: " << status.text());
                    }
                }
                // determine color model of the decompressed image
                status = PixelData->getDecompressedColorModel(OFstatic_cast(DcmItem *, Object), PhotometricInterpretation);
                if (status.bad())
                {
                    DCMIMGLE_ERROR("can't determine 'PhotometricInterpretation' of decompressed image");
                    DCMIMGLE_DEBUG("DcmPixelData::getDecompressedColorModel() returned: " << status.text());
                }
            } else {
                DCMIMGLE_ERROR("invalid pixel data in DICOM dataset (wrong class)");
                DCMIMGLE_DEBUG("found PixelData " << DCM_PixelData << " as an instance of the class for VR '"
                    << OFSTRING_GUARD(DcmVR(pobject->ident()).getVRName()) << "' instead of '"
                    << OFSTRING_GUARD(DcmVR(EVR_PixelData).getVRName()) << "'");
            }
        } else
            DCMIMGLE_ERROR("invalid pixel data in DICOM dataset");
    } else
        DCMIMGLE_ERROR("no pixel data found in DICOM dataset");
}


/*--------------*
 *  destructor  *
 *--------------*/

DiDocument::~DiDocument()
{
    // DICOM image loaded from file: delete file format (and data set)
    if (FileFormat != NULL)
        delete FileFormat;
    // DICOM image loaded from external data set: only delete if flag is set
    else if (Flags & CIF_TakeOverExternalDataset)
        delete Object;
}


/********************************************************************/


DcmElement *DiDocument::search(const DcmTagKey &tag,
                               DcmObject *obj) const
{
    DcmStack stack;
    if (obj == NULL)
        obj = Object;
    // only search on main dataset level
    if ((obj != NULL) && (obj->search(tag, stack, ESM_fromHere, OFFalse /* searchIntoSub */) == EC_Normal) &&
        (stack.top()->getLength(Xfer) > 0))
    {
        return OFstatic_cast(DcmElement *, stack.top());
    }
    return NULL;
}


/********************************************************************/


int DiDocument::search(const DcmTagKey &tag,
                       DcmStack &pstack) const
{
    if (pstack.empty())
        pstack.push(Object);
    DcmObject *obj = pstack.top();
    if ((obj != NULL) && (obj->search(tag, pstack, ESM_fromHere, OFFalse /* searchIntoSub */) == EC_Normal) &&
        (pstack.top()->getLength(Xfer) > 0))
            return 1;
    return 0;
}


/********************************************************************/


unsigned long DiDocument::getVM(const DcmTagKey &tag) const
{
    DcmElement *elem = search(tag);
    if (elem != NULL)
        return elem->getVM();
    return 0;
}


unsigned long DiDocument::getValue(const DcmTagKey &tag,
                                   Uint16 &returnVal,
                                   const unsigned long pos,
                                   DcmItem *item,
                                   const OFBool allowSigned) const
{
    return getElemValue(search(tag, item), returnVal, pos, allowSigned);
}


unsigned long DiDocument::getValue(const DcmTagKey &tag,
                                   Sint16 &returnVal,
                                   const unsigned long pos,
                                   DcmItem *item) const
{
    DcmElement *elem = search(tag, item);
    if (elem != NULL)
    {
        if (elem->getSint16(returnVal, pos).good())
            return elem->getVM();
    }
    return 0;
}


unsigned long DiDocument::getValue(const DcmTagKey &tag,
                                   Uint32 &returnVal,
                                   const unsigned long pos,
                                   DcmItem *item) const
{
    DcmElement *elem = search(tag, item);
    if (elem != NULL)
    {
        if (elem->getUint32(returnVal, pos).good())
            return elem->getVM();
    }
    return 0;
}


unsigned long DiDocument::getValue(const DcmTagKey &tag,
                                   Sint32 &returnVal,
                                   const unsigned long pos,
                                   DcmItem *item) const
{
    DcmElement *elem = search(tag, item);
    if (elem != NULL)
    {
        if (elem->getSint32(returnVal, pos).good())
            return elem->getVM();
    }
    return 0;
}


unsigned long DiDocument::getValue(const DcmTagKey &tag,
                                   double &returnVal,
                                   const unsigned long pos,
                                   DcmItem *item) const
{
    DcmElement *elem = search(tag, item);
    if (elem != NULL)
    {
        if (elem->getFloat64(returnVal, pos).good())
            return elem->getVM();
    }
    return 0;
}


unsigned long DiDocument::getValue(const DcmTagKey &tag,
                                   const Uint16 *&returnVal,
                                   DcmItem *item) const
{
    DcmElement *elem = search(tag, item);
    if (elem != NULL)
    {
        Uint16 *val;
        if (elem->getUint16Array(val).good())
        {
            returnVal = val;
            const DcmEVR vr = elem->getVR();
            if ((vr == EVR_OB) || (vr == EVR_OW) || (vr == EVR_lt))
                return elem->getLength(Xfer) / sizeof(Uint16);
            return elem->getVM();
        }
    }
    return 0;
}


unsigned long DiDocument::getValue(const DcmTagKey &tag,
                                   const char *&returnVal,
                                   DcmItem *item) const
{
    return getElemValue(search(tag, item), returnVal);
}


unsigned long DiDocument::getValue(const DcmTagKey &tag,
                                   OFString &returnVal,
                                   const unsigned long pos,
                                   DcmItem *item) const
{
    return getElemValue(search(tag, item), returnVal, pos);
}


unsigned long DiDocument::getSequence(const DcmTagKey &tag,
                                      DcmSequenceOfItems *&seq,
                                      DcmItem *item) const
{
    DcmElement *elem = search(tag, item);
    if ((elem != NULL) && (elem->ident() == EVR_SQ))
        return (seq = OFstatic_cast(DcmSequenceOfItems *, elem))->card();
    return 0;
}


unsigned long DiDocument::getElemValue(const DcmElement *elem,
                                       Uint16 &returnVal,
                                       const unsigned long pos,
                                       const OFBool allowSigned)
{
    if (elem != NULL)
    {
        // remove 'const' to use non-const methods
        if (OFconst_cast(DcmElement *, elem)->getUint16(returnVal, pos).good())
            return OFconst_cast(DcmElement *, elem)->getVM();
        else if (allowSigned)
        {
            // try to retrieve signed value ...
            Sint16 value = 0;
            if (OFconst_cast(DcmElement *, elem)->getSint16(value, pos).good())
            {
                // ... and cast to unsigned type
                returnVal = OFstatic_cast(Uint16, value);
                DCMIMGLE_TRACE("retrieved signed value (" << value << ") at position " << pos
                    << " from element " << OFconst_cast(DcmElement *, elem)->getTag()
                    << ", VR=" << DcmVR(OFconst_cast(DcmElement *, elem)->getVR()).getVRName()
                    << ", VM=" << OFconst_cast(DcmElement *, elem)->getVM());
                return OFconst_cast(DcmElement *, elem)->getVM();
            }
        }
    }
    return 0;
}


unsigned long DiDocument::getElemValue(const DcmElement *elem,
                                       const Uint16 *&returnVal)
{
    if (elem != NULL)
    {
        Uint16 *val;
        // remove 'const' to use non-const methods
        if (OFconst_cast(DcmElement *, elem)->getUint16Array(val).good())
        {
            returnVal = val;
            const DcmEVR vr = OFconst_cast(DcmElement *, elem)->getVR();
            if ((vr == EVR_OW) || (vr == EVR_lt))
                return OFconst_cast(DcmElement *, elem)->getLength(/*Xfer*/) / sizeof(Uint16);
            return OFconst_cast(DcmElement *, elem)->getVM();
        }
    }
    return 0;
}


unsigned long DiDocument::getElemValue(const DcmElement *elem,
                                       const char *&returnVal)
{
    if (elem != NULL)
    {
        char *val;
        // remove 'const' to use non-const methods
        if (OFconst_cast(DcmElement *, elem)->getString(val).good())
        {
            returnVal = val;
            return OFconst_cast(DcmElement *, elem)->getVM();
        }
    }
    return 0;
}


unsigned long DiDocument::getElemValue(const DcmElement *elem,
                                       OFString &returnVal,
                                       const unsigned long pos)
{
    if (elem != NULL)
    {
        // remove 'const' to use non-const methods
        if (OFconst_cast(DcmElement *, elem)->getOFString(returnVal, pos).good())
            return OFconst_cast(DcmElement *, elem)->getVM();
    }
    return 0;
}
