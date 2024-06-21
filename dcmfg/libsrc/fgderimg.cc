/*
 *
 *  Copyright (C) 2016-2024, Open Connections GmbH
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  This software and supporting documentation are maintained by
 *
 *    OFFIS e.V.
 *    R&D Division Health
 *    Escherweg 2
 *    D-26121 Oldenburg, Germany
 *
 *
 *  Module:  dcmfg
 *
 *  Author:  Michael Onken
 *
 *  Purpose: Class for managing Derivation Image Functional Group
 *
 */

#include "dcmtk/config/osconfig.h"

#include "dcmtk/dcmdata/dcdatutl.h"
#include "dcmtk/dcmdata/dcitem.h"
#include "dcmtk/dcmdata/dcfilefo.h"
#include "dcmtk/dcmfg/fgderimg.h"
#include "dcmtk/dcmiod/iodutil.h"

FGDerivationImage::FGDerivationImage()
    : FGBase(DcmFGTypes::EFG_DERIVATIONIMAGE)
    , m_DerivationImageItems()
{
}

FGDerivationImage::~FGDerivationImage()
{
    DcmIODUtil::freeContainer(m_DerivationImageItems);
}

FGBase* FGDerivationImage::clone() const
{
    OFunique_ptr<FGDerivationImage> copy(new FGDerivationImage);
    if (copy)
    {
        if (DcmIODUtil::copyContainer(this->m_DerivationImageItems, copy->m_DerivationImageItems).good())
        {
            return copy.release();
        }
    }
    return OFnullptr;
}

FGDerivationImage* FGDerivationImage::createMinimal(const OFVector<ImageSOPInstanceReferenceMacro>& derivationImages,
                                                    const OFString& derivationDescription,
                                                    const CodeSequenceMacro& derivationCode,
                                                    const CodeSequenceMacro& purposeOfReference)
{
    OFCondition result;
    FGDerivationImage* group = new FGDerivationImage();
    if (group)
    {
        DerivationImageItem* derivItem = NULL;
        result = group->addDerivationImageItem(derivationCode, derivationDescription, derivItem);
        if (result.good())
        {
            OFVector<SourceImageItem*> srcImages                              = derivItem->getSourceImageItems();
            OFVector<ImageSOPInstanceReferenceMacro>::const_iterator imageRef = derivationImages.begin();
            while (imageRef != derivationImages.end())
            {
                SourceImageItem* item = new SourceImageItem();
                if (item)
                {
                    item->getPurposeOfReferenceCode()    = purposeOfReference;
                    item->getImageSOPInstanceReference() = *imageRef;
                    derivItem->getSourceImageItems().push_back(item);
                }
                else
                {
                    DCMFG_ERROR("Cannot create derivation description: Out of memory");
                    result = EC_MemoryExhausted;
                }
                imageRef++;
            }
        }
        else
        {
            DCMFG_ERROR("Cannot create derivation description: " << result.text());
        }
    }
    else
    {
        result = EC_MemoryExhausted;
    }

    if (result.bad())
    {
        delete group;
        group = NULL;
    }

    return group;
}

int FGDerivationImage::compare(const FGBase& rhs) const
{
    int result = FGBase::compare(rhs);
    if (result != 0)
        return result;

    const FGDerivationImage* myRhs = OFstatic_cast(const FGDerivationImage*, &rhs);
    if (!myRhs)
    {
        return -1;
    }

    size_t thisSize = m_DerivationImageItems.size();
    size_t rhsSize  = myRhs->m_DerivationImageItems.size();
    if (thisSize < rhsSize)
    {
        return 1;
    }
    else if (thisSize > rhsSize)
    {
        return -1;
    }

    for (size_t count = 0; count < thisSize; count++)
    {
        int error = (*(m_DerivationImageItems[count])).compare((*(myRhs->m_DerivationImageItems[count])));
        if (error != 0)
        {
            return error;
        }
    }

    return 0;
}

void FGDerivationImage::clearData()
{
    DcmIODUtil::freeContainer(m_DerivationImageItems);
}

OFCondition FGDerivationImage::check() const
{
    OFCondition result;
    OFVector<DerivationImageItem*>::const_iterator it = m_DerivationImageItems.begin();
    while ((it != m_DerivationImageItems.end()) && result.good())
    {
        result = (*it)->check();
        it++;
    }

    return result;
}

DerivationImageItem& DerivationImageItem::operator=(const DerivationImageItem& rhs)
{
    m_DerivationDescription = rhs.m_DerivationDescription;
    // Copy nested structures: Derivation Codes
    OFVector<CodeSequenceMacro*>::const_iterator code = rhs.m_DerivationCodeItems.begin();
    while (code != rhs.m_DerivationCodeItems.end())
    {
        m_DerivationCodeItems.push_back(new CodeSequenceMacro(**code));
        code++;
    }
    // Copy nested structures: Derivation Codes
    OFVector<SourceImageItem*>::const_iterator source = rhs.m_SourceImageItems.begin();
    while (source != rhs.m_SourceImageItems.end())
    {
        m_SourceImageItems.push_back(new SourceImageItem(**source));
        source++;
    }
    return *this;
}

OFCondition FGDerivationImage::addDerivationImageItem(const CodeSequenceMacro& derivationCode,
                                                      const OFString& derivationDescription,
                                                      DerivationImageItem*& item)
{
    OFCondition result;
    item = new DerivationImageItem();
    if (item == NULL)
    {
        return EC_MemoryExhausted;
    }

    // Initialize with given data
    if (result.good())
    {
        result = item->setDerivationDescription(derivationDescription);
    }
    if (result.good())
    {
        item->getDerivationCodeItems().push_back(new CodeSequenceMacro(derivationCode));
        m_DerivationImageItems.push_back(item);
    }
    else
    {
        delete item;
        item   = NULL;
        result = FG_EC_InvalidData;
    }
    return result;
}

OFVector<DerivationImageItem*>& FGDerivationImage::getDerivationImageItems()
{
    return m_DerivationImageItems;
}

OFCondition FGDerivationImage::read(DcmItem& item)
{
    /* re-initialize object */
    clearData();

    DcmSequenceOfItems* derivationImageSequence = NULL;
    if (item.findAndGetSequence(DCM_DerivationImageSequence, derivationImageSequence).bad())
        return FG_EC_NoSuchGroup;

    /* Read Derivation Image Sequence */
    DcmIODUtil::readSubSequence<OFVector<DerivationImageItem*> >(derivationImageSequence,
                                                                 DCM_DerivationImageSequence,
                                                                 m_DerivationImageItems,
                                                                 "0-n",
                                                                 "2",
                                                                 "DerivationImageMacro");

    return EC_Normal;
}

OFCondition FGDerivationImage::write(DcmItem& item)
{
    OFCondition result = check();
    DcmIODUtil::writeSubSequence<OFVector<DerivationImageItem*> >(
        result, DCM_DerivationImageSequence, m_DerivationImageItems, item, "0-n", "2", "DerivationImageMacro");
    return result;
}

/* -- Class SourceImage Implementation -- */

SourceImageItem::SourceImageItem()
    : m_PurposeOfReferenceCode()
    , m_ImageSOPInstanceReference()
{
}

SourceImageItem::~SourceImageItem()
{
    clearData();
}

SourceImageItem& SourceImageItem::operator=(const SourceImageItem& rhs)
{
    m_PurposeOfReferenceCode    = rhs.m_PurposeOfReferenceCode;
    m_ImageSOPInstanceReference = rhs.m_ImageSOPInstanceReference;
    return *this;
}

void SourceImageItem::clearData()
{
    m_PurposeOfReferenceCode.clearData();
    m_ImageSOPInstanceReference.clear();
}

OFCondition SourceImageItem::check() const
{
    OFCondition result = OFconst_cast(ImageSOPInstanceReferenceMacro*, &m_ImageSOPInstanceReference)->check();
    if (result.bad())
    {
        return result;
    }

    return OFconst_cast(CodeSequenceMacro*, &m_PurposeOfReferenceCode)->check();
}

int SourceImageItem::compare(const SourceImageItem& rhs) const
{
    int result = this->m_PurposeOfReferenceCode.compare(rhs.m_PurposeOfReferenceCode);
    if (result != 0)
    {
        result = this->m_ImageSOPInstanceReference.compare(rhs.m_ImageSOPInstanceReference);
    }

    return result;
}

CodeSequenceMacro& SourceImageItem::getPurposeOfReferenceCode()
{
    return m_PurposeOfReferenceCode;
}

ImageSOPInstanceReferenceMacro& SourceImageItem::getImageSOPInstanceReference()
{
    return m_ImageSOPInstanceReference;
}

OFCondition SourceImageItem::read(DcmItem& itemOfSourceImageSequence, const OFBool clearOldData)
{
    /* Re-initialize object */
    if (clearOldData)
        clearData();

    /* Read Purpose of Reference Code Sequence */
    DcmIODUtil::readSingleItem<CodeSequenceMacro>(itemOfSourceImageSequence,
                                                  DCM_PurposeOfReferenceCodeSequence,
                                                  m_PurposeOfReferenceCode,
                                                  "1",
                                                  "DerivationImageMacro");

    /* Read Source Image Sequence */
    m_ImageSOPInstanceReference.read(itemOfSourceImageSequence);

    return EC_Normal;
}

OFCondition SourceImageItem::write(DcmItem& itemOfSourceImageSequence)
{
    OFCondition result = EC_Normal;

    /* Write Purpose of Reference Code Sequence */
    DcmIODUtil::writeSingleItem<CodeSequenceMacro>(result,
                                                   DCM_PurposeOfReferenceCodeSequence,
                                                   m_PurposeOfReferenceCode,
                                                   itemOfSourceImageSequence,
                                                   "1",
                                                   "DerivationImageMacro");

    /* Write Image SOP Instance Reference Macro */
    if (result.good())
    {
        m_ImageSOPInstanceReference.write(itemOfSourceImageSequence);
    }

    return result;
}

/* -- Class DerivationImageItem Implementation -- */

DerivationImageItem::DerivationImageItem()
    : m_DerivationDescription(DCM_DerivationDescription)
    , m_DerivationCodeItems()
    , m_SourceImageItems()
{
}

DerivationImageItem::DerivationImageItem(const DerivationImageItem& rhs)
    : m_DerivationDescription(rhs.m_DerivationDescription)
    , m_DerivationCodeItems()
    , m_SourceImageItems()
{
    DcmIODUtil::copyContainer(rhs.m_DerivationCodeItems, this->m_DerivationCodeItems);
    DcmIODUtil::copyContainer(rhs.m_SourceImageItems, this->m_SourceImageItems);
}

void DerivationImageItem::clearData()
{
    m_DerivationDescription.clear();
    DcmIODUtil::freeContainer(m_DerivationCodeItems);
    DcmIODUtil::freeContainer(m_SourceImageItems);
}

int DerivationImageItem::compare(const DerivationImageItem& rhs) const
{
    if (this == &rhs)
        return OFTrue;

    int result = m_DerivationDescription.compare(rhs.m_DerivationDescription);
    if (result != 0)
        return result;

    size_t numThis = m_DerivationCodeItems.size();
    size_t numRhs  = rhs.m_DerivationCodeItems.size();
    size_t maxSize = (numThis > numRhs) ? numThis : numRhs;
    for (size_t count = 0; count < maxSize; count++)
    {
        int error = (*m_DerivationCodeItems[count]).compare(*(rhs.m_DerivationCodeItems[count]));
        if (error != 0)
        {
            return error;
        }
    }
    if (numThis < numRhs)
    {
        return 1;
    }
    else if (numThis > numRhs)
    {
        return -1;
    }

    numThis = m_SourceImageItems.size();
    numRhs  = rhs.m_SourceImageItems.size();
    maxSize = (numThis > numRhs) ? numThis : numRhs;
    for (size_t count = 0; count < maxSize; count++)
    {
        int error = (*m_SourceImageItems[count]).compare(*(rhs.m_SourceImageItems[count]));
        if (error != 0)
        {
            return error;
        }
    }
    if (numThis < numRhs)
    {
        return 1;
    }
    else if (numThis > numRhs)
    {
        return -1;
    }

    // If we get here, the object values must be identical
    return 0;
}

OFCondition DerivationImageItem::check() const
{
    if (m_DerivationCodeItems.size() == 0)
    {
        DCMFG_ERROR("Derivation Code Sequence in Derivation Image Functional Group Macro must have one or more items");
        return FG_EC_InvalidData;
    }

    OFCondition result;
    OFVector<CodeSequenceMacro*>::const_iterator it = m_DerivationCodeItems.begin();
    while (it != m_DerivationCodeItems.end() && result.good())
    {
        result = (*it)->check();
        it++;
    }

    OFVector<SourceImageItem*>::const_iterator ref = m_SourceImageItems.begin();
    while ((ref != m_SourceImageItems.end()) && result.good())
    {
        result = (*ref)->check();
        ref++;
    }
    return result;
}

DerivationImageItem::~DerivationImageItem()
{
    clearData();
}

OFCondition DerivationImageItem::addSourceImageItem(const OFString& file,
                                                    const CodeSequenceMacro& purposeOfReference,
                                                    SourceImageItem*& resultSourceImageItem)
{
    DcmFileFormat dcmff;
    DcmDataset* dataset = NULL;
    OFCondition result  = dcmff.loadFile(file.c_str());
    if (result.bad())
    {
        DCMFG_ERROR("Could not load file " << file << ": " << result.text());
        return result;
    }
    dataset = dcmff.getDataset();
    return addSourceImageItem(dataset, purposeOfReference, resultSourceImageItem);
}

OFCondition DerivationImageItem::addSourceImageItem(DcmDataset* dataset,
                                                    const CodeSequenceMacro& purposeOfReference,
                                                    SourceImageItem*& resultSourceImageItem)
{
    // Create new source image item
    SourceImageItem* item = new SourceImageItem();
    if (item == NULL)
    {
        return EC_MemoryExhausted;
    }

    // check code validity
    if (OFconst_cast(CodeSequenceMacro*, &purposeOfReference)->check().bad())
    {
        DCMFG_ERROR("Purpose of Reference code within item of Derivation Image Sequence is invalid");
        return FG_EC_InvalidData;
    }

    item->getPurposeOfReferenceCode() = purposeOfReference;
    resultSourceImageItem             = NULL;
    OFString sopClass, sopInstance, ts;
    OFCondition result
        = DcmDataUtil::getSOPInstanceFromDataset(dataset, EXS_Unknown, sopClass, sopInstance, ts /*ignored*/);
    {
        if (result.good())
            result = item->getImageSOPInstanceReference().setReferencedSOPClassUID(sopClass);
        if (result.good())
            result = item->getImageSOPInstanceReference().setReferencedSOPInstanceUID(sopInstance);
    }
    if (result.good())
    {
        m_SourceImageItems.push_back(item);
        resultSourceImageItem = item;
    }
    else
    {
        delete item;
    }

    return result;
}

OFCondition DerivationImageItem::addSourceImageItems(const OFVector<OFString>& files,
                                                     const CodeSequenceMacro& purposeOfReference,
                                                     OFVector<SourceImageItem*>& resultSourceImageItems,
                                                     const OFBool skipErrors)
{
    DcmFileFormat dcmff;
    OFCondition result;
    DcmDataset* dataset = NULL;
    OFVector<DcmDataset*> datasets;
    OFVector<OFString>::const_iterator it = files.begin();
    while (it != files.end())
    {
        result = dcmff.loadFile((*it).c_str());
        if (result.bad())
        {
            DCMFG_ERROR("Could not load file " << (*it) << ": " << result.text());
            return result;
        }
        dataset = dcmff.getDataset();
        datasets.push_back(dataset);
        it++;
    }
    return addSourceImageItems(datasets, purposeOfReference, resultSourceImageItems, skipErrors);
}

OFCondition DerivationImageItem::addSourceImageItems(const OFVector<DcmDataset*>& datasets,
                                                     const CodeSequenceMacro& purposeOfReference,
                                                     OFVector<SourceImageItem*>& resultSourceImageItems,
                                                     const OFBool skipErrors)
{
    OFCondition result;
    OFVector<DcmDataset*>::const_iterator it = datasets.begin();
    while (it != datasets.end())
    {
        SourceImageItem* resultItem = NULL;
        result                      = addSourceImageItem((*it), purposeOfReference, resultItem);
        if (result.good())
        {
            resultSourceImageItems.push_back(resultItem);
        }
        else if (result.bad() && !skipErrors)
        {
            DCMFG_ERROR("Could not add Source Image Sequence item for file " << *it << ": " << result.text());
            break;
        }
        else if (result.bad())
        {
            DCMFG_WARN("Could not add Source Image Sequence item for file " << *it << ": " << result.text());
        }
        it++;
    }

    return result;
}

OFVector<CodeSequenceMacro*>& DerivationImageItem::getDerivationCodeItems()
{
    return m_DerivationCodeItems;
}

OFCondition DerivationImageItem::getDerivationDescription(OFString& value, const long signed int pos) const
{
    return DcmIODUtil::getStringValueFromElement(m_DerivationDescription, value, pos);
}

OFCondition DerivationImageItem::setDerivationDescription(const OFString& value, const OFBool checkValue)
{
    (void)checkValue;
    return m_DerivationDescription.putOFStringArray(value);
}

OFVector<SourceImageItem*>& DerivationImageItem::getSourceImageItems()
{
    return m_SourceImageItems;
}

OFCondition DerivationImageItem::read(DcmItem& itemOfDerivationImageSequence, const OFBool clearOldData)
{
    OFCondition result = EC_Normal;

    /* re-initialize object */
    if (clearOldData)
        clearData();

    /* Derivation Description */
    DcmIODUtil::getAndCheckElementFromDataset(
        itemOfDerivationImageSequence, m_DerivationDescription, "1" /* vm */, "3" /* type */, "DerivationImageMacro");

    /* Derivation Code Sequence */
    DcmIODUtil::readSubSequence<OFVector<CodeSequenceMacro*> >(itemOfDerivationImageSequence,
                                                               DCM_DerivationCodeSequence,
                                                               m_DerivationCodeItems,
                                                               "1" /* vm */,
                                                               "3" /* type */,
                                                               "DerivationImageMacro");

    /* Source Image Sequence */
    DcmIODUtil::readSubSequence<OFVector<SourceImageItem*> >(itemOfDerivationImageSequence,
                                                             DCM_SourceImageSequence,
                                                             m_SourceImageItems,
                                                             "0-n" /* vm */,
                                                             "2" /* type */,
                                                             "DerivationImageMacro");

    return result;
}

OFCondition DerivationImageItem::write(DcmItem& itemOfDerivationImageSequence)
{
    OFCondition result = EC_Normal;

    /* Write Derivation Description */
    DcmIODUtil::copyElementToDataset(
        result, itemOfDerivationImageSequence, m_DerivationDescription, "1", "3", "DerivationImageMacro");

    /* Write Derivation Code Sequence */
    DcmIODUtil::writeSubSequence<OFVector<CodeSequenceMacro*> >(result,
                                                                DCM_DerivationCodeSequence,
                                                                m_DerivationCodeItems,
                                                                itemOfDerivationImageSequence,
                                                                "1-n",
                                                                "1",
                                                                "DerivationImageMacro");

    /* Write Source Image Sequence */
    DcmIODUtil::writeSubSequence<OFVector<SourceImageItem*> >(result,
                                                              DCM_SourceImageSequence,
                                                              m_SourceImageItems,
                                                              itemOfDerivationImageSequence,
                                                              "0-n",
                                                              "2",
                                                              "DerivationImageMacro");

    return result;
}
