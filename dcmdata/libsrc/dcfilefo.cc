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
 *  Purpose: class DcmFileFormat
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#define INCLUDE_CSTDLIB
#define INCLUDE_CSTDIO
#define INCLUDE_CSTRING
#define INCLUDE_CTIME
#define INCLUDE_UNISTD
#include "dcmtk/ofstd/ofstdinc.h"

#include "dcmtk/ofstd/ofstream.h"
#include "dcmtk/dcmdata/dcfilefo.h"
#include "dcmtk/dcmdata/dcitem.h"
#include "dcmtk/dcmdata/dcxfer.h"
#include "dcmtk/dcmdata/dcvrobow.h"
#include "dcmtk/dcmdata/dcvrui.h"
#include "dcmtk/dcmdata/dcvrul.h"
#include "dcmtk/dcmdata/dcvrus.h"
#include "dcmtk/dcmdata/dcvrae.h"
#include "dcmtk/dcmdata/dcvrsh.h"
#include "dcmtk/dcmdata/dcmetinf.h"

#include "dcmtk/dcmdata/dcdeftag.h"
#include "dcmtk/dcmdata/dcuid.h"
#include "dcmtk/dcmdata/dcostrma.h"    /* for class DcmOutputStream */
#include "dcmtk/dcmdata/dcostrmf.h"    /* for class DcmOutputFileStream */
#include "dcmtk/dcmdata/dcistrma.h"    /* for class DcmInputStream */
#include "dcmtk/dcmdata/dcistrmf.h"    /* for class DcmInputFileStream */
#include "dcmtk/dcmdata/dcwcache.h"    /* for class DcmWriteCache */
#include "dcmtk/dcmdata/dcjson.h"


// ********************************


DcmFileFormat::DcmFileFormat()
  : DcmSequenceOfItems(DCM_InternalUseTag),
    FileReadMode(ERM_autoDetect)
{
    DcmMetaInfo *MetaInfo = new DcmMetaInfo();
    DcmSequenceOfItems::itemList->insert(MetaInfo);
    MetaInfo->setParent(this);

    DcmDataset *Dataset = new DcmDataset();
    DcmSequenceOfItems::itemList->insert(Dataset);
    Dataset->setParent(this);
}


DcmFileFormat::DcmFileFormat(DcmDataset *dataset,
                             OFBool deepCopy)
  : DcmSequenceOfItems(DCM_InternalUseTag),
    FileReadMode(ERM_autoDetect)
{
    DcmMetaInfo *MetaInfo = new DcmMetaInfo();
    DcmSequenceOfItems::itemList->insert(MetaInfo);
    MetaInfo->setParent(this);

    DcmDataset* insertion;
    if (dataset == NULL)
    {
        insertion = new DcmDataset();
    }
    else if (deepCopy)
    {
        insertion = new DcmDataset(*dataset);
    }
    else // shallow copy
    {
        insertion = dataset;
    }
    insertion->setParent(this);
    DcmSequenceOfItems::itemList->insert(insertion);
}


DcmFileFormat::DcmFileFormat(const DcmFileFormat &old)
  : DcmSequenceOfItems(old),
    FileReadMode(old.FileReadMode)
{
}


OFCondition DcmFileFormat::copyFrom(const DcmObject& rhs)
{
  if (this != &rhs)
  {
    if (rhs.ident() != ident()) return EC_IllegalCall;
    *this = OFstatic_cast(const DcmFileFormat &, rhs);
  }
  return EC_Normal;
}


DcmFileFormat::~DcmFileFormat()
{
}


DcmFileFormat &DcmFileFormat::operator=(const DcmFileFormat &obj)
{
  if (this != &obj)
  {
    DcmSequenceOfItems::operator=(obj);
    FileReadMode = obj.FileReadMode;
  }

  return *this;
}


// ********************************


DcmEVR DcmFileFormat::ident() const
{
    return EVR_fileFormat;
}


// ********************************


void DcmFileFormat::print(STD_NAMESPACE ostream &out,
                          const size_t flags,
                          const int level,
                          const char *pixelFileName,
                          size_t *pixelCounter)
{
    out << OFendl;
    if (flags & DCMTypes::PF_useANSIEscapeCodes)
        out << DCMDATA_ANSI_ESCAPE_CODE_COMMENT;
    printNestingLevel(out, flags, level);
    out << "# Dicom-File-Format";
    if (flags & DCMTypes::PF_useANSIEscapeCodes)
        out << DCMDATA_ANSI_ESCAPE_CODE_RESET;
    out << OFendl;
    if (!itemList->empty())
    {
        DcmObject *dO;
        itemList->seek(ELP_first);
        do {
            dO = itemList->get();
            dO->print(out, flags, level, pixelFileName, pixelCounter);
        } while (itemList->seek(ELP_next));
    } else {
        if (flags & DCMTypes::PF_useANSIEscapeCodes)
            out << DCMDATA_ANSI_ESCAPE_CODE_COMMENT;
        printNestingLevel(out, flags, level);
        out << "# Dicom-File-Format has been erased";
        if (flags & DCMTypes::PF_useANSIEscapeCodes)
            out << DCMDATA_ANSI_ESCAPE_CODE_RESET;
        out << OFendl;
    }
}


// ********************************


OFCondition DcmFileFormat::writeXML(STD_NAMESPACE ostream &out,
                                    const size_t flags)
{
    OFCondition l_error = EC_Normal;
    if (flags & DCMTypes::XF_useNativeModel)
    {
        /* in Native DICOM Model, there is no concept of a "file format" */
        DcmDataset *dset = getDataset();
        if (dset != NULL)
        {
            /* write content of dataset */
            l_error = dset->writeXML(out, flags);
        } else {
            l_error = makeOFCondition(OFM_dcmdata, EC_CODE_CannotConvertToXML, OF_error,
                "Cannot convert to Native DICOM Model: No data set present");
        }
    } else {
        /* XML start tag for "file-format" */
        out << "<file-format";
        if (flags & DCMTypes::XF_useXMLNamespace)
            out << " xmlns=\"" << DCMTK_XML_NAMESPACE_URI << "\"";
        out << ">" << OFendl;
        /* write content of file meta information and dataset */
        if (!itemList->empty())
        {
            /* write content of all children */
            DcmObject *dO;
            itemList->seek(ELP_first);
            do {
                dO = itemList->get();
                l_error = dO->writeXML(out, flags & ~DCMTypes::XF_useXMLNamespace);
            } while (l_error.good() && itemList->seek(ELP_next));
        } else {
            /* a file format should never be empty */
            l_error = EC_CorruptedData;
        }
        if (l_error.good())
        {
            /* XML end tag for "file-format" */
            out << "</file-format>" << OFendl;
        }
    }
    return l_error;
}


// ********************************


OFCondition DcmFileFormat::writeJson(STD_NAMESPACE ostream &out,
                                     DcmJsonFormat &format)
{
    if (format.printMetaheaderInformation)
    {
        if (!itemList->empty())
        {
            out << format.indent() << "{" << format.newline();
            // write content of all children (DcmObject)
            itemList->seek(ELP_first);
            OFCondition status = EC_Normal;
            status = itemList->get()->writeJson(out, format);
            while (status.good() && itemList->seek(ELP_next))
            {
                out << "," << format.newline();
                status = itemList->get()->writeJson(out, format);
            }
            out << format.newline() << format.indent() << "}" << format.newline();
            return status;
        }
        else
        {
            return EC_CorruptedData;
        }
    }
    else
    {
        if (DcmDataset *dset = getDataset())
        {
            out << format.indent() << "{" << format.newline();
            OFCondition status = EC_Normal;
            // write content of dataset
            status = dset->writeJson(out, format);
            out << format.newline() << format.indent() << "}" << format.newline();
            return status;
        }
        else
        {
            out << format.indent() << "{}" << format.newline();
            return EC_Normal;
        }
    }
    return EC_Normal;
}


// ********************************


OFCondition DcmFileFormat::checkMetaHeaderValue(DcmMetaInfo *metainfo,
                                                DcmDataset *dataset,
                                                const DcmTagKey &atagkey,
                                                DcmObject *obj,
                                                const E_TransferSyntax oxfer,
                                                const E_FileWriteMode writeMode)
    /*
     * This function checks if a particular data element of the file meta information header is
     * existent.  If the element is not existent, it will be inserted.  Additionally, this function
     * makes sure that the corresponding data element will contain a correct value.
     *
     * Parameters:
     *   metainfo  - [in] The meta header information.
     *   dataset   - [in] The data set information.
     *   atagkey   - [in] Tag of the data element which shall be checked.
     *   obj       - [in] Data object from metainfo which represents the data element that shall be checked.
     *                    Equals NULL, if this data element is not existent in the meta header information.
     *   oxfer     - [in] The transfer syntax which shall be used.
     *   writeMode - [in] Flag indicating whether to update the file meta information or not.
     */
{
    /* initialize result value */
    OFCondition l_error = EC_Normal;

    /* if there is meta header information and also data set information, do something */
    if ((metainfo != NULL) && (dataset != NULL))
    {
        /* initialize variables */
        DcmStack stack;
        DcmTag tag(atagkey);
        if (obj != NULL)
            tag = obj->getTag();

        DcmTagKey xtag = tag.getXTag();
        DcmElement *elem = OFstatic_cast(DcmElement *, obj);

        /* go ahead and scrutinize one particular data element (depending on xtag) */
        if (xtag == DCM_FileMetaInformationGroupLength)     // (0002,0000)
        {
            if (elem == NULL)
            {
                elem = new DcmUnsignedLong(tag);
                metainfo->insert(elem, OFTrue);
            }
            Uint32 temp = 0;
            if ((elem->getLength() == 0) && (elem->ident() == EVR_UL))
                OFstatic_cast(DcmUnsignedLong *, elem)->putUint32Array(&temp, 1);
            // the calculation of actual group length value is contained in validateMetaInfo()
        }
        else if (xtag == DCM_FileMetaInformationVersion)    // (0002,0001)
        {
            if (elem == NULL)
            {
                elem = new DcmOtherByteOtherWord(tag);
                metainfo->insert(elem, OFTrue);
            }
            // supported version of this implementation: 00\01
            Uint8 version[2] = {0, 1};
            if ((elem->getLength() == 0) && (elem->ident() == EVR_OB))
                OFstatic_cast(DcmOtherByteOtherWord *, elem)->putUint8Array(version, 2);

            // check version of meta header
            Uint8 *currVers = NULL;
            l_error = OFstatic_cast(DcmOtherByteOtherWord *, elem)->getUint8Array(currVers);
            if (l_error.good() && (currVers != NULL))
            {
                // the version information is stored in a bit field
                if (((currVers[0] & version[0] & 0xff) == version[0]) &&
                    ((currVers[1] & version[1] & 0xff) == version[1]))
                {
                    DCMDATA_DEBUG("DcmFileFormat::checkMetaHeaderValue() Version of MetaHeader is ok: 0x"
                        << STD_NAMESPACE hex << STD_NAMESPACE setfill('0')
                        << STD_NAMESPACE setw(2) << OFstatic_cast(int, currVers[0])
                        << STD_NAMESPACE setw(2) << OFstatic_cast(int, currVers[1]));
                } else {
                    DCMDATA_WARN ("DcmFileFormat: Unknown Version of MetaHeader detected: 0x"
                        << STD_NAMESPACE hex << STD_NAMESPACE setfill('0')
                        << STD_NAMESPACE setw(2) << OFstatic_cast(int, currVers[0])
                        << STD_NAMESPACE setw(2) << OFstatic_cast(int, currVers[1])
                        << ", supported: 0x"
                        << STD_NAMESPACE setw(2) << OFstatic_cast(int, version[0])
                        << STD_NAMESPACE setw(2) << OFstatic_cast(int, version[1]));
                }
            } else {
                DCMDATA_ERROR("DcmFileFormat: Cannot determine Version of MetaHeader");
            }
        }
        else if (xtag == DCM_MediaStorageSOPClassUID)       // (0002,0002)
        {
            if (elem == NULL)
            {
                elem = new DcmUniqueIdentifier(tag);
                metainfo->insert(elem, OFTrue);
            }
            if (elem->ident() == EVR_UI)
            {
                if ((writeMode == EWM_updateMeta) || (elem->getLength() == 0))
                {
                    if (dataset->search(DCM_SOPClassUID, stack).good())
                    {
                        char *uid = NULL;
                        l_error = OFstatic_cast(DcmUniqueIdentifier *, stack.top())->getString(uid);
                        OFstatic_cast(DcmUniqueIdentifier *, elem)->putString(uid);
                        DCMDATA_DEBUG("DcmFileFormat::checkMetaHeaderValue() use SOPClassUID [" << uid << "] from Dataset");
                    }
                    else if (elem->getLength() == 0)
                    {
                        OFstatic_cast(DcmUniqueIdentifier *, elem)->putString(UID_PrivateGenericFileSOPClass);
                        DCMDATA_DEBUG("DcmFileFormat::checkMetaHeaderValue() no SOPClassUID in Dataset, using PrivateGenericFileSOPClass");
                    }
                }
                else if (DCM_dcmdataLogger.isEnabledFor(OFLogger::WARN_LOG_LEVEL))
                {
                    // check whether UID in meta-header is identical to the one in the dataset
                    if (dataset->search(DCM_SOPClassUID, stack).good())
                    {
                        OFString uidDataset, uidMetaHeader;
                        OFstatic_cast(DcmUniqueIdentifier *, stack.top())->getOFStringArray(uidDataset);
                        OFstatic_cast(DcmUniqueIdentifier *, elem)->getOFStringArray(uidMetaHeader);
                        if (uidDataset != uidMetaHeader)
                        {
                            DCMDATA_WARN("DcmFileFormat: Value of SOPClassUID in MetaHeader and Dataset is different");
                            DCMDATA_DEBUG("DcmFileFormat::checkMetaHeaderValue() value of MediaStorageSOPClassUID (MetaHeader) [" << uidMetaHeader << "]");
                            DCMDATA_DEBUG("DcmFileFormat::checkMetaHeaderValue() value of SOPClassUID (Dataset) [" << uidDataset << "]");
                        }
                    }
                }
            }
        }
        else if (xtag == DCM_MediaStorageSOPInstanceUID)    // (0002,0003)
        {
            if (elem == NULL)
            {
                elem = new DcmUniqueIdentifier(tag);
                metainfo->insert(elem, OFTrue);
            }
            if (elem->ident() == EVR_UI)
            {
                if ((writeMode == EWM_updateMeta) || (elem->getLength() == 0))
                {
                    if (dataset->search(DCM_SOPInstanceUID, stack).good())
                    {
                        char* uid = NULL;
                        l_error = OFstatic_cast(DcmUniqueIdentifier *, stack.top())->getString(uid);
                        OFstatic_cast(DcmUniqueIdentifier *, elem)->putString(uid);
                        DCMDATA_DEBUG("DcmFileFormat::checkMetaHeaderValue() use SOPInstanceUID [" << uid << "] from Dataset");
                    }
                    else if (elem->getLength() == 0)
                    {
                        char uid[128];
                        dcmGenerateUniqueIdentifier(uid);       // from dcuid.h
                        OFstatic_cast(DcmUniqueIdentifier *, elem)->putString(uid);
                        DCMDATA_DEBUG("DcmFileFormat::checkMetaHeaderValue() use new generated SOPInstanceUID [" << uid << "]");
                    }
                }
                else if (DCM_dcmdataLogger.isEnabledFor(OFLogger::WARN_LOG_LEVEL))
                {
                    // check whether UID in meta-header is identical to the one in the dataset
                    if (dataset->search(DCM_SOPInstanceUID, stack).good())
                    {
                        OFString uidDataset, uidMetaHeader;
                        OFstatic_cast(DcmUniqueIdentifier *, stack.top())->getOFStringArray(uidDataset);
                        OFstatic_cast(DcmUniqueIdentifier *, elem)->getOFStringArray(uidMetaHeader);
                        if (uidDataset != uidMetaHeader)
                        {
                            DCMDATA_WARN("DcmFileFormat: Value of SOPInstanceUID in MetaHeader and Dataset is different");
                            DCMDATA_DEBUG("DcmFileFormat::checkMetaHeaderValue() value of MediaStorageSOPInstanceUID (MetaHeader) [" << uidMetaHeader << "]");
                            DCMDATA_DEBUG("DcmFileFormat::checkMetaHeaderValue() value of SOPInstanceUID (Dataset) [" << uidDataset << "]");
                        }
                    }
                }
            }
        }
        else if (xtag == DCM_TransferSyntaxUID)             // (0002,0010)
        {
            if (elem == NULL)
            {
                elem = new DcmUniqueIdentifier(tag);
                metainfo->insert(elem, OFTrue);
            }
            if (elem->ident() == EVR_UI)
            {
#ifdef DEBUG
                char * uidtmp = NULL;
                OFstatic_cast(DcmUniqueIdentifier *, elem)->getString(uidtmp);
                if (uidtmp != NULL)
                    DCMDATA_DEBUG("DcmFileFormat::checkMetaHeaderValue() found old TransferSyntaxUID [" << uidtmp << "]");
#endif
                DcmXfer dcXfer(oxfer);
                const char *uid = dcXfer.getXferID();
                elem->putString(uid);
                DCMDATA_DEBUG("DcmFileFormat::checkMetaHeaderValue() use new TransferSyntaxUID ["
                    << dcXfer.getXferName() << "] on writing following Dataset");
            }
        }
        else if (xtag == DCM_ImplementationClassUID)        // (0002,0012)
        {
            if (elem == NULL)
            {
                elem = new DcmUniqueIdentifier(tag);
                metainfo->insert(elem, OFTrue);
            }
            if (elem->ident() == EVR_UI)
            {
                const char *uid = OFFIS_IMPLEMENTATION_CLASS_UID;
                OFstatic_cast(DcmUniqueIdentifier *, elem)->putString(uid);
            }
        }
        else if (xtag == DCM_ImplementationVersionName)     // (0002,0013)
        {
            if (elem == NULL)
            {
                elem = new DcmShortString(tag);
                metainfo->insert(elem, OFTrue);
            }
            if (elem->ident() == EVR_SH)
            {
                const char *uid = OFFIS_DTK_IMPLEMENTATION_VERSION_NAME;
                OFstatic_cast(DcmShortString *, elem)->putString(uid);
            }
        }
        else if ((xtag == DCM_SourceApplicationEntityTitle) ||  // (0002,0016)
                 (xtag == DCM_SendingApplicationEntityTitle) || // (0002,0017)
                 (xtag == DCM_ReceivingApplicationEntityTitle)) // (0002,0018)
        {
            if (elem == NULL)
            {
                elem = new DcmApplicationEntity(tag);
                metainfo->insert(elem, OFTrue);
            }
            DCMDATA_WARN("DcmFileFormat: Don't know how to handle " << tag.getTagName());
        }
        else if (xtag == DCM_PrivateInformationCreatorUID)  // (0002,0100)
        {
            if (elem == NULL)
            {
                elem = new DcmUniqueIdentifier(tag);
                metainfo->insert(elem, OFTrue);
            }
            DCMDATA_WARN("DcmFileFormat: Don't know how to handle PrivateInformationCreatorUID");
        }
        else if (xtag == DCM_PrivateInformation)            // (0002,0102)
        {
            if (elem == NULL)
            {
                elem = new DcmOtherByteOtherWord(tag);
                metainfo->insert(elem, OFTrue);
            }
            DCMDATA_WARN("DcmFileFormat: Don't know how to handle PrivateInformation");
        } else
            DCMDATA_ERROR("DcmFileFormat: Don't know how to handle " << tag.getTagName());

        /* if at this point elem still equals NULL, something is fishy */
        if (elem == NULL)
            l_error = EC_InvalidVR;
    } else {
        /* (i.e. there is either no meta header information or no data set information */
        l_error = EC_IllegalCall;
    }

    /* return result value */
    return l_error;
}


// ********************************


OFCondition DcmFileFormat::validateMetaInfo(const E_TransferSyntax oxfer,
                                            const E_FileWriteMode writeMode)
    /*
     * This function makes sure that all data elements of the meta header information are existent
     * in metainfo and contain correct values.
     *
     * Parameters:
     *   oxfer     - [in] The transfer syntax which shall be used.
     *   writeMode - [in] Flag indicating whether to update the file meta information or not.
     */
{
    /* initialize some variables */
    OFCondition l_error = EC_Normal;
    DcmMetaInfo *metinf = getMetaInfo();
    DcmDataset *datset = getDataset();

    /* if there is meta header information and data set information, do something */
    if (metinf != NULL && datset != NULL)
    {
        if (writeMode == EWM_dontUpdateMeta)
        {
            DCMDATA_WARN("DcmFileFormat: Meta Information Header is not updated!");
        } else {
            /* start with empty file meta information */
            if (writeMode == EWM_createNewMeta)
                metinf->clear();

            /* in the following, we want to make sure all elements of the meta header */
            /* are existent in metinf and contain correct values */
            DcmStack stack;

            /* DCM_FileMetaInformationGroupLength */
            metinf->search(DCM_FileMetaInformationGroupLength, stack, ESM_fromHere, OFFalse);
            checkMetaHeaderValue(metinf, datset, DCM_FileMetaInformationGroupLength, stack.top(), oxfer, writeMode);

            /* DCM_FileMetaInformationVersion */
            metinf->search(DCM_FileMetaInformationVersion, stack, ESM_fromHere, OFFalse);
            checkMetaHeaderValue(metinf, datset, DCM_FileMetaInformationVersion, stack.top(), oxfer, writeMode);

            /* DCM_MediaStorageSOPClassUID */
            metinf->search(DCM_MediaStorageSOPClassUID, stack, ESM_fromHere, OFFalse);
            checkMetaHeaderValue(metinf, datset, DCM_MediaStorageSOPClassUID, stack.top(), oxfer, writeMode);

            /* DCM_MediaStorageSOPInstanceUID */
            metinf->search(DCM_MediaStorageSOPInstanceUID, stack, ESM_fromHere, OFFalse);
            checkMetaHeaderValue(metinf, datset, DCM_MediaStorageSOPInstanceUID, stack.top(), oxfer, writeMode);

            /* DCM_TransferSyntaxUID */
            metinf->search(DCM_TransferSyntaxUID, stack, ESM_fromHere, OFFalse);
            checkMetaHeaderValue(metinf, datset, DCM_TransferSyntaxUID, stack.top(), oxfer, writeMode);

            /* DCM_ImplementationClassUID */
            metinf->search(DCM_ImplementationClassUID, stack, ESM_fromHere, OFFalse);
            checkMetaHeaderValue(metinf, datset, DCM_ImplementationClassUID, stack.top(), oxfer, writeMode);

            /* DCM_ImplementationVersionName */
            metinf->search(DCM_ImplementationVersionName, stack, ESM_fromHere, OFFalse);
            checkMetaHeaderValue(metinf, datset, DCM_ImplementationVersionName, stack.top(), oxfer, writeMode);

            /* dump some information if required */
            DCMDATA_DEBUG("DcmFileFormat::validateMetaInfo() found " << metinf->card() << " Elements in DcmMetaInfo 'metinf'");

            /* calculate new GroupLength for meta header */
            if (metinf->computeGroupLengthAndPadding(EGL_withGL, EPD_noChange,
                META_HEADER_DEFAULT_TRANSFERSYNTAX, EET_UndefinedLength).bad())
            {
                DCMDATA_ERROR("DcmFileFormat: Group length of Meta Information Header not adapted");
            }
        }
    } else {
        /* (i.e. there is either no meta header information or no data set information, or both are missing) */
        l_error = EC_CorruptedData;
    }

    /* return result value */
    return l_error;
}



// ********************************


E_TransferSyntax DcmFileFormat::lookForXfer(DcmMetaInfo *metainfo)
{
    E_TransferSyntax newxfer = EXS_Unknown;
    DcmStack stack;
    /* check whether meta header is present (and non-empty, i.e. contains elements) */
    if (metainfo && !metainfo->isEmpty())
    {
        if (metainfo->search(DCM_TransferSyntaxUID, stack).good())
        {
            DcmUniqueIdentifier *xferUI = OFstatic_cast(DcmUniqueIdentifier *, stack.top());
            if (xferUI->getTag().getXTag() == DCM_TransferSyntaxUID)
            {
                char *xferid = NULL;
                xferUI->getString(xferid);
                DcmXfer localXfer(xferid);      // decode to E_TransferSyntax
                newxfer = localXfer.getXfer();
                DCMDATA_TRACE("DcmFileFormat::lookForXfer() TransferSyntax=\""
                    << localXfer.getXferName() << "\" in MetaInfo");
            }
        } else {
            /* there is no transfer syntax UID element in the meta header */
            DCMDATA_DEBUG("DcmFileFormat::lookForXfer() no TransferSyntax in MetaInfo");
        }
    } else {
        /* no meta header present at all (or it is empty, i.e. contains no elements) */
        DCMDATA_DEBUG("DcmFileFormat::lookForXfer() no MetaInfo found");
    }
    return newxfer;
}


// ********************************


Uint32 DcmFileFormat::calcElementLength(const E_TransferSyntax xfer,
                                        const E_EncodingType enctype)
{
    return getMetaInfo()->calcElementLength(xfer, enctype) +
        getDataset()->calcElementLength(xfer, enctype);
}


// ********************************


OFBool DcmFileFormat::canWriteXfer(const E_TransferSyntax newXfer,
                                   const E_TransferSyntax oldXfer)
{
    DcmDataset *dataset = getDataset();

    if (dataset)
        return dataset->canWriteXfer(newXfer, oldXfer);
    else
        return OFFalse;
}


// ********************************


OFCondition DcmFileFormat::read(DcmInputStream &inStream,
                                const E_TransferSyntax xfer,
                                const E_GrpLenEncoding glenc,
                                const Uint32 maxReadLength)
{
  return DcmFileFormat::readUntilTag(inStream,xfer,glenc,maxReadLength,DCM_UndefinedTagKey);
}

OFCondition DcmFileFormat::readUntilTag(DcmInputStream &inStream,
                                        const E_TransferSyntax xfer,
                                        const E_GrpLenEncoding glenc,
                                        const Uint32 maxReadLength,
                                        const DcmTagKey &stopParsingAtElement)
{
    if (getTransferState() == ERW_notInitialized)
        errorFlag = EC_IllegalCall;
    else
    {
        errorFlag = inStream.status();

        E_TransferSyntax newxfer = xfer;
        DcmDataset *dataset = NULL;

        if (errorFlag.good() && inStream.eos())
            errorFlag = EC_EndOfStream;
        else if (errorFlag.good() && getTransferState() != ERW_ready)
        {
            // the new data is added to the end
            itemList->seek(ELP_last);

            DcmMetaInfo *metaInfo = getMetaInfo();
            if (metaInfo == NULL && getTransferState() == ERW_init)
            {
                metaInfo = new DcmMetaInfo();
                itemList->insert(metaInfo, ELP_first);
                // remember the parent
                metaInfo->setParent(this);
            }
            if (metaInfo && metaInfo->transferState() != ERW_ready)
            {
                // do read meta header not in given transfer syntax (always Little Endian Explicit)
                errorFlag = metaInfo->read(inStream, EXS_Unknown, glenc, maxReadLength);
            }

            // determine xfer from tag (0002,0010) in the meta header
            newxfer = lookForXfer(metaInfo);
            if ((FileReadMode == ERM_fileOnly) || (FileReadMode == ERM_metaOnly))
            {
                // reject file if no meta header present
                if (errorFlag.good() && (newxfer == EXS_Unknown))
                    errorFlag = EC_FileMetaInfoHeaderMissing;
            }
            if (errorFlag.good() && (!metaInfo || metaInfo->transferState() == ERW_ready))
            {
                dataset = getDataset();
                if (dataset == NULL && getTransferState() == ERW_init)
                {
                    dataset = new DcmDataset();
                    itemList->seek (ELP_first);
                    itemList->insert(dataset, ELP_next);
                    // remember the parent
                    dataset->setParent(this);
                }
                // check whether to read the dataset at all
                if (FileReadMode != ERM_metaOnly)
                {
                    if (dataset && dataset->transferState() != ERW_ready)
                    {
                        errorFlag = dataset->readUntilTag(inStream, newxfer, glenc, maxReadLength, stopParsingAtElement);
                    }
                }
            }
        }
        if (getTransferState() == ERW_init)
            setTransferState(ERW_inWork);

        if (dataset && dataset->transferState() == ERW_ready)
            setTransferState(ERW_ready);
    }
    return errorFlag;
}  // DcmFileFormat::readUntilTag()


// ********************************


OFCondition DcmFileFormat::write(DcmOutputStream &outStream,
                                 const E_TransferSyntax oxfer,
                                 const E_EncodingType enctype,
                                 DcmWriteCache *wcache)
{
    return write(outStream, oxfer, enctype, wcache, EGL_recalcGL, EPD_noChange);
}


OFCondition DcmFileFormat::write(DcmOutputStream &outStream,
                                 const E_TransferSyntax oxfer,
                                 const E_EncodingType enctype,
                                 DcmWriteCache *wcache,
                                 const E_GrpLenEncoding glenc,
                                 const E_PaddingEncoding padenc,
                                 const Uint32 padlen,
                                 const Uint32 subPadlen,
                                 Uint32 instanceLength,
                                 const E_FileWriteMode writeMode)
    /*
     * This function writes data values which are contained in this to the stream which is
     * passed as first argument. With regard to the writing of information, the other parameters
     * which are passed are accounted for. The function will return EC_Normal, if the information
     * from all elements of this data set has been written to the buffer, and it will return some
     * other (error) value if there was an error.
     *
     * Parameters:
     *   outStream      - [inout] The stream that the information will be written to.
     *   oxfer          - [in] The transfer syntax which shall be used.
     *   enctype        - [in] Encoding type for sequences. Specifies how sequences will be handled.
     *   glenc          - [in] Encoding type for group length. Specifies what will be done with group length tags.
     *   padenc         - [in] Encoding type for padding. Specifies what will be done with padding tags.
     *   padlen         - [in] The length up to which the dataset shall be padded, if padding is desired.
     *   subPadlen      - [in] For sequences (i.e. sub elements), the length up to which item shall be padded,
     *                         if padding is desired.
     *   instanceLength - [in] Number of extra bytes added to the item/dataset length used when computing the
     *                         padding. This parameter is for instance used to pass the length of the file meta
     *                         header from the DcmFileFormat to the DcmDataset object.
     *   writeMode      - [in] Write file with or without meta header. Also allows for updating the information
     *                         in the file meta information header.
     */
{
    /* if the transfer state of this is not initialized, this is an illegal call */
    if (getTransferState() == ERW_notInitialized)
        errorFlag = EC_IllegalCall;
    else
    {
        /* if this is not an illegal call, do something */

        /* assign data set and the meta information header to local variables */
        DcmDataset *dataset = getDataset();
        DcmMetaInfo *metainfo = getMetaInfo();
        /* Determine the transfer syntax which shall be used. Either we use the one which was passed, */
        /* or (if it's an unknown transfer syntax) we use the data set's original transfer syntax. */
        E_TransferSyntax outxfer = oxfer;
        if (outxfer == EXS_Unknown && dataset)
            outxfer = dataset->getOriginalXfer();
        /* check if the stream reported an error so far */
        errorFlag = outStream.status();
        /* check if we can actually write data to the stream; in certain cases we cannot. */
        if (outxfer == EXS_Unknown || outxfer == EXS_BigEndianImplicit)
            errorFlag = EC_IllegalCall;
        else if (itemList->empty())
            errorFlag = EC_CorruptedData;
        else if (errorFlag.good() && getTransferState() != ERW_ready)
        {
            /* in this case we can write data to the stream */

            /* if this function was called for the first time for the dataset object, the transferState is */
            /* still set to ERW_init. In this case, we need to validate the meta header information, set the */
            /* item list pointer to the fist element and we need to set the transfer state to ERW_inWork. */
            if (getTransferState() == ERW_init)
            {
                validateMetaInfo(outxfer, writeMode);
                itemList->seek(ELP_first);
                setTransferState(ERW_inWork);
            }
            /* if the transfer state is set to ERW_inWork, we need to write the */
            /* information which is included in this to the buffer which was passed. */
            if (getTransferState() == ERW_inWork)
            {
                /* write meta header information */
                errorFlag = metainfo->write(outStream, outxfer, enctype, wcache);
                /* recalculate the instance length */
                instanceLength += metainfo->calcElementLength(outxfer, enctype);
                /* if everything is ok, write the data set */
                if (errorFlag.good())
                    errorFlag = dataset->write(outStream, outxfer, enctype, wcache, glenc, padenc, padlen,
                                               subPadlen, instanceLength);
                /* if everything is ok, set the transfer state to ERW_ready */
                if (errorFlag.good())
                    setTransferState(ERW_ready);
            }
        }
        /* in case the transfer syntax which shall be used is indeed the */
        /* BigEndianImplicit transfer syntax dump some error information */
        if (outxfer == EXS_BigEndianImplicit)
            DCMDATA_ERROR("DcmFileFormat: Illegal TransferSyntax (BigEndianImplicit) used in write method");
    }
    /* return result value */
    return errorFlag;
}


// ********************************

OFCondition DcmFileFormat::loadFile(const OFFilename &fileName,
                                    const E_TransferSyntax readXfer,
                                    const E_GrpLenEncoding groupLength,
                                    const Uint32 maxReadLength,
                                    const E_FileReadMode readMode)
{
  return DcmFileFormat::loadFileUntilTag(fileName, readXfer, groupLength, maxReadLength, readMode, DCM_UndefinedTagKey);
}


OFCondition DcmFileFormat::loadFileUntilTag(
                                    const OFFilename &fileName,
                                    const E_TransferSyntax readXfer,
                                    const E_GrpLenEncoding groupLength,
                                    const Uint32 maxReadLength,
                                    const E_FileReadMode readMode,
                                    const DcmTagKey &stopParsingAtElement)
{
    if (readMode == ERM_dataset)
        return getDataset()->loadFileUntilTag(fileName, readXfer, groupLength, maxReadLength, stopParsingAtElement);

    OFCondition l_error = EC_InvalidFilename;
    /* check parameters first */
    if (!fileName.isEmpty())
    {
        /* open file for input */
        DcmInputFileStream fileStream(fileName);
        /* check stream status */
        l_error = fileStream.status();
        if (l_error.good())
        {
            /* clear this object */
            l_error = clear();
            if (l_error.good())
            {
                /* save old value */
                const E_FileReadMode oldMode = FileReadMode;
                FileReadMode = readMode;
                /* read data from file */
                transferInit();
                l_error = readUntilTag(fileStream, readXfer, groupLength, maxReadLength, stopParsingAtElement);
                transferEnd();
                /* restore old value */
                FileReadMode = oldMode;
            }
        }
    }
    return l_error;
}


OFCondition DcmFileFormat::saveFile(const OFFilename &fileName,
                                    const E_TransferSyntax writeXfer,
                                    const E_EncodingType encodingType,
                                    const E_GrpLenEncoding groupLength,
                                    const E_PaddingEncoding padEncoding,
                                    const Uint32 padLength,
                                    const Uint32 subPadLength,
                                    const E_FileWriteMode writeMode)
{
    if (writeMode == EWM_dataset)
    {
        return getDataset()->saveFile(fileName, writeXfer, encodingType, groupLength,
            padEncoding, padLength, subPadLength);
    }
    OFCondition l_error = EC_InvalidFilename;
    /* check parameters first */
    if (!fileName.isEmpty())
    {
        DcmWriteCache wcache;

        /* open file for output */
        DcmOutputFileStream fileStream(fileName);

        /* check stream status */
        l_error = fileStream.status();
        if (l_error.good())
        {
            /* write data to file */
            transferInit();
            l_error = write(fileStream, writeXfer, encodingType, &wcache, groupLength,
                padEncoding, padLength, subPadLength, 0 /*instanceLength*/, writeMode);
            transferEnd();
        }
    }
    return l_error;
}


// ********************************


OFCondition DcmFileFormat::insertItem(DcmItem * /*item*/,
                                      const unsigned long /*where*/)
{
    DCMDATA_WARN("Illegal call of DcmFileFormat::insert(DcmItem *, unsigned long)");
    errorFlag = EC_IllegalCall;
    return errorFlag;
}


// ********************************


void DcmFileFormat::removeInvalidGroups()
{
    getMetaInfo()->removeInvalidGroups();
    getDataset()->removeInvalidGroups();
}


// ********************************


DcmItem *DcmFileFormat::remove(const unsigned long /*num*/)
{
    DCMDATA_WARN("Illegal call of DcmFileFormat::remove(unsigned long)");
    errorFlag = EC_IllegalCall;
    return NULL;
}


// ********************************


DcmItem *DcmFileFormat::remove(DcmItem* /*item*/)
{
    DCMDATA_WARN("Illegal call of DcmFileFormat::remove(DcmItem *)");
    errorFlag = EC_IllegalCall;
    return NULL;
}


// ********************************


OFCondition DcmFileFormat::clear()
{
    getMetaInfo()->clear();
    return getDataset()->clear();
}


// ********************************


DcmMetaInfo *DcmFileFormat::getMetaInfo()
{
    errorFlag = EC_Normal;
    DcmMetaInfo *meta = NULL;
    if (itemList->seek_to(0) != NULL && itemList->get()->ident() == EVR_metainfo)
        meta = OFstatic_cast(DcmMetaInfo *, itemList->get());
    else
        errorFlag = EC_IllegalCall;
    return meta;
}


// ********************************


DcmDataset *DcmFileFormat::getDataset()
{
    errorFlag = EC_Normal;
    DcmDataset *data = NULL;
    if (itemList->seek_to(1) != NULL && itemList->get()->ident() == EVR_dataset)
        data = OFstatic_cast(DcmDataset *, itemList->get());
    else
        errorFlag = EC_IllegalCall;
    return data;
}


// ********************************


DcmDataset *DcmFileFormat::getAndRemoveDataset()
{
    errorFlag = EC_Normal;
    DcmDataset *data = NULL;
    if (itemList->seek_to(1) != NULL && itemList->get()->ident() == EVR_dataset)
    {
        data = OFstatic_cast(DcmDataset *, itemList->remove());
        // forget about the parent
        data->setParent(NULL);
        DcmDataset *Dataset = new DcmDataset();
        DcmSequenceOfItems::itemList->insert(Dataset, ELP_last);
        // remember the parent
        Dataset->setParent(this);
    }
    else
        errorFlag = EC_IllegalCall;
    return data;
}


// ********************************


OFCondition DcmFileFormat::convertCharacterSet(const OFString &fromCharset,
                                               const OFString &toCharset,
                                               const size_t flags)
{
    // convert the dataset associated with this object
    return getDataset()->convertCharacterSet(fromCharset, toCharset, flags);
}


OFCondition DcmFileFormat::convertCharacterSet(const OFString &toCharset,
                                               const size_t flags)
{
    OFString sopClass;
    OFBool ignoreCharset = OFFalse;
    // check whether this dataset belongs to a DICOMDIR,
    // because the Basic Directory IOD has no SOP Common Module
    if (getMetaInfo()->findAndGetOFString(DCM_MediaStorageSOPClassUID, sopClass).good() &&
        (sopClass == UID_MediaStorageDirectoryStorage))
    {
        DCMDATA_DEBUG("DcmFileFormat::convertCharacterSet() according to the value of MediaStorageSOPClassUID "
            << DCM_MediaStorageSOPClassUID << " this is a DICOMDIR, which has no SOP Common Module");
        ignoreCharset = OFTrue;
    }
    // usually, we check for Specific Character Set (0008,0005) element in the dataset
    return getDataset()->convertCharacterSet(toCharset, flags, ignoreCharset);
}


OFCondition DcmFileFormat::convertCharacterSet(DcmSpecificCharacterSet &converter)
{
    // convert the dataset associated with this object
    return getDataset()->convertCharacterSet(converter);
}


OFCondition DcmFileFormat::convertToUTF8()
{
    // the DICOM defined term "ISO_IR 192" is used for "UTF-8"
    return convertCharacterSet("ISO_IR 192", 0 /*flags*/);
}
