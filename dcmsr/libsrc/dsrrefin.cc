/*
 *
 *  Copyright (C) 2011-2019, OFFIS e.V.
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
 *  Module: dcmsr
 *
 *  Author: Joerg Riesmeier
 *
 *  Purpose:
 *    classes: DSRReferencedInstanceList
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/dsrrefin.h"
#include "dcmtk/dcmsr/dsrxmld.h"

#include "dcmtk/dcmdata/dcdeftag.h"
#include "dcmtk/dcmdata/dcuid.h"
#include "dcmtk/dcmdata/dcvrui.h"


DSRReferencedInstanceList::DSRReferencedInstanceList()
  : ItemList(),
    Iterator()
{
    /* initialize list cursor */
    Iterator = ItemList.end();
}


DSRReferencedInstanceList::~DSRReferencedInstanceList()
{
    /* clear list and delete allocated memory */
    clear();
}


void DSRReferencedInstanceList::clear()
{
    Iterator = ItemList.begin();
    const OFListIterator(ItemStruct *) last = ItemList.end();
    /* delete all items and free memory */
    while (Iterator != last)
    {
        delete (*Iterator);
        Iterator = ItemList.erase(Iterator);
    }
    /* make sure that the list is empty */
    ItemList.clear();
    Iterator = ItemList.end();
}


OFBool DSRReferencedInstanceList::isEmpty() const
{
    return ItemList.empty();
}


size_t DSRReferencedInstanceList::getNumberOfItems() const
{
    return ItemList.size();
}


OFCondition DSRReferencedInstanceList::read(DcmItem &dataset,
                                            const size_t flags)
{
    /* first, check whether sequence is present and non-empty */
    DcmSequenceOfItems sequence(DCM_ReferencedInstanceSequence);
    OFCondition result = getElementFromDataset(dataset, sequence);
    checkElementValue(sequence, "1-n", "1C", result, "SRDocumentGeneralModule");
    if (result.good())
    {
        ItemStruct *item = NULL;
        OFString sopClassUID, instanceUID;
        /* iterate over all sequence items */
        DcmObject *dobj = NULL;
        while ((dobj = sequence.nextInContainer(dobj)) != NULL)
        {
            DcmItem *ditem = OFstatic_cast(DcmItem *, dobj);
            /* get the SOP class and instance UID */
            if (getAndCheckStringValueFromDataset(*ditem, DCM_ReferencedSOPClassUID, sopClassUID, "1", "1", "ReferencedInstanceSequence").good() &&
                getAndCheckStringValueFromDataset(*ditem, DCM_ReferencedSOPInstanceUID, instanceUID, "1", "1", "ReferencedInstanceSequence").good())
            {
                /* add new item to the list */
                if (addItem(sopClassUID, instanceUID, item).good())
                {
                    /* read additional information */
                    item->PurposeOfReference.readSequence(*ditem, DCM_PurposeOfReferenceCodeSequence, "1", flags);
                }
            }
        }
    }
    return result;
}


OFCondition DSRReferencedInstanceList::write(DcmItem &dataset) const
{
    OFCondition result = EC_Normal;
    /* iterate over all list items */
    OFListConstIterator(ItemStruct *) iter = ItemList.begin();
    const OFListConstIterator(ItemStruct *) last = ItemList.end();
    while ((iter != last) && result.good())
    {
        ItemStruct *item = *iter;
        /* check whether list item really exists and is valid */
        if ((item != NULL) && !item->SOPClassUID.empty() && !item->InstanceUID.empty())
        {
            DcmItem *ditem = NULL;
            /* create a new item (and a sequence if required) */
            result = dataset.findOrCreateSequenceItem(DCM_ReferencedInstanceSequence, ditem, -2 /*append new*/);
            /* write item data */
            if (result.good())
            {
                /* open question: should we check the return values? */
                putStringValueToDataset(*ditem, DCM_ReferencedSOPClassUID, item->SOPClassUID);
                putStringValueToDataset(*ditem, DCM_ReferencedSOPInstanceUID, item->InstanceUID);
                item->PurposeOfReference.writeSequence(*ditem, DCM_PurposeOfReferenceCodeSequence);
            }
        }
        ++iter;
    }
    return result;
}


OFCondition DSRReferencedInstanceList::readXML(const DSRXMLDocument &doc,
                                               DSRXMLCursor cursor,
                                               const size_t flags)
{
    OFCondition result = SR_EC_InvalidDocument;
    ItemStruct *item = NULL;
    OFString sopClassUID, instanceUID;
    /* iterate over all nodes */
    while (cursor.valid())
    {
        /* check for known element tags */
        if (doc.checkNode(cursor, "value").good())
        {
            /* retrieve SOP class and instance UID */
            if (!doc.getStringFromAttribute(doc.getNamedChildNode(cursor, "sopclass"), sopClassUID, "uid").empty() &&
                !doc.getStringFromAttribute(doc.getNamedChildNode(cursor, "instance"), instanceUID, "uid").empty())
            {
                result = addItem(sopClassUID, instanceUID, item);
                if (result.good())
                {
                    DSRXMLCursor childCursor = cursor.getChild();
                    /* clear any previously stored information */
                    item->clear();
                    while (childCursor.valid())
                    {
                        /* check for known element tags */
                        if (doc.matchNode(childCursor, "purpose"))
                            item->PurposeOfReference.readXML(doc, childCursor, flags);
                        /* proceed with next node */
                        childCursor.gotoNext();
                    }
                }
            }
        }
        /* proceed with next node */
        cursor.gotoNext();
    }
    return result;
}


OFCondition DSRReferencedInstanceList::writeXML(STD_NAMESPACE ostream &stream,
                                                const size_t flags) const
{
    OFString tmpString;
    /* iterate over all list items */
    OFListConstIterator(ItemStruct *) iter = ItemList.begin();
    const OFListConstIterator(ItemStruct *) last = ItemList.end();
    while (iter != last)
    {
        ItemStruct *item = *iter;
        /* check whether list item really exists */
        if (item != NULL)
        {
            /* write instance level */
            stream << "<value>" << OFendl;
            stream << "<sopclass uid=\"" << item->SOPClassUID << "\">";
            /* retrieve name of SOP class (if known) */
            stream << dcmFindNameOfUID(item->SOPClassUID.c_str(), "" /* empty value as default */);
            stream << "</sopclass>" << OFendl;
            stream << "<instance uid=\"" << item->InstanceUID << "\"/>" << OFendl;
            if (flags & DSRTypes::XF_codeComponentsAsAttribute)
                stream << "<purpose";     // bracket ">" is closed in next writeXML() call
            else
                stream << "<purpose>" << OFendl;
            item->PurposeOfReference.writeXML(stream, flags);
            stream << "</purpose>" << OFendl;
            stream << "</value>" << OFendl;
        }
        ++iter;
    }
    return EC_Normal;
}


OFCondition DSRReferencedInstanceList::addItem(const OFString &sopClassUID,
                                               const OFString &instanceUID,
                                               ItemStruct *&item)
{
    OFCondition result = EC_IllegalParameter;
    /* check parameters first */
    if (!sopClassUID.empty() && !instanceUID.empty())
    {
        result = EC_Normal;
        OFListIterator(ItemStruct *) oldCursor = Iterator;
        /* check whether item already exists */
        if (gotoItem(sopClassUID, instanceUID).bad())
        {
            /* if not create new item and add it to the list */
            item = new ItemStruct(sopClassUID, instanceUID);
            if (item != NULL)
            {
                ItemList.push_back(item);
                /* set cursor to new position */
                Iterator = --ItemList.end();
            } else {
                /* restore old cursor */
                Iterator = oldCursor;
                result = EC_MemoryExhausted;
            }
        } else {
            DCMSR_WARN("SOP instance " << instanceUID
                << " already exists in ReferencedInstanceSequence ... overwriting");
            /* gotoItem() was successful, set item pointer */
            item = *Iterator;
        }
    } else
        item = NULL;
    return result;
}


OFCondition DSRReferencedInstanceList::addItem(const OFString &sopClassUID,
                                               const OFString &instanceUID,
                                               const OFBool check)
{
    OFCondition result = EC_Normal;
    /* check whether the passed values are valid */
    if (check)
        result = checkSOPInstance(sopClassUID, instanceUID);
    /* call the "real" function (empty parameters are rejected) */
    if (result.good())
    {
        ItemStruct *item = NULL;
        result = addItem(sopClassUID, instanceUID, item);
    }
    return result;
}


OFCondition DSRReferencedInstanceList::removeItem()
{
    OFCondition result = EC_IllegalCall;
    /* check whether list is empty or iterator is invalid */
    if (!ItemList.empty() && (Iterator != ItemList.end()))
    {
        /* free memory */
        delete (*Iterator);
        /* remove item from list */
        Iterator = ItemList.erase(Iterator);
        result = EC_Normal;
    }
    return result;
}


OFCondition DSRReferencedInstanceList::removeItem(const OFString &sopClassUID,
                                                  const OFString &instanceUID)
{
    /* goto specified item ... */
    OFCondition result = gotoItem(sopClassUID, instanceUID);
    /* ... and remove it */
    if (result.good())
        result = removeItem();
    return result;
}


OFCondition DSRReferencedInstanceList::gotoItem(const OFString &sopClassUID,
                                                const OFString &instanceUID)
{
    OFCondition result = EC_IllegalParameter;
    /* check parameters first */
    if (!sopClassUID.empty() && !instanceUID.empty())
    {
        OFBool sopClassMatch = OFFalse;
        result = SR_EC_SOPInstanceNotFound;
        /* start with first item */
        Iterator = ItemList.begin();
        const OFListIterator(ItemStruct *) last = ItemList.end();
        /* search for given item */
        while ((Iterator != last) && result.bad())
        {
            /* if instance found, exit loop */
            if ((*Iterator != NULL) && ((*Iterator)->InstanceUID == instanceUID))
            {
                /* finally, check whether SOP class matches */
                sopClassMatch = ((*Iterator)->SOPClassUID == sopClassUID);
                result = EC_Normal;
            } else
                ++Iterator;
        }
        /* report an error in case of SOP class mismatch */
        if (result.good() && !sopClassMatch)
            result = SR_EC_DifferentSOPClassesForAnInstance;
    }
    return result;
}


OFCondition DSRReferencedInstanceList::gotoFirstItem()
{
    OFCondition result = EC_IllegalCall;
    /* check for empty item list */
    if (!ItemList.empty())
    {
        /* set cursor to first list item */
        Iterator = ItemList.begin();
        result = EC_Normal;
    }
    return result;
}


OFCondition DSRReferencedInstanceList::gotoNextItem()
{
    OFCondition result = EC_IllegalCall;
    /* goto next list item */
    if (++Iterator != ItemList.end())
    {
        /* check whether list item is valid */
        if (*Iterator != NULL)
            result = EC_Normal;
        else
            result = EC_CorruptedData;
    }
    return result;
}


DSRReferencedInstanceList::ItemStruct *DSRReferencedInstanceList::getCurrentItem() const
{
    ItemStruct *item = NULL;
    /* check whether current item is valid */
    OFListConstIterator(ItemStruct *) it = Iterator;
    if (it != ItemList.end())
        item = *Iterator;
    return item;
}


const OFString &DSRReferencedInstanceList::getSOPClassUID(OFString &sopClassUID) const
{
    /* check whether current item is valid */
    ItemStruct *item = getCurrentItem();
    /* get requested value or clear string if invalid */
    if (item != NULL)
        sopClassUID = item->SOPClassUID;
    else
        sopClassUID.clear();
    return sopClassUID;
}


const OFString &DSRReferencedInstanceList::getSOPInstanceUID(OFString &instanceUID) const
{
    /* check whether current item is valid */
    ItemStruct *item = getCurrentItem();
    /* get requested value or clear string if invalid */
    if (item != NULL)
        instanceUID = item->InstanceUID;
    else
        instanceUID.clear();
    return instanceUID;
}


OFCondition DSRReferencedInstanceList::getPurposeOfReference(DSRCodedEntryValue &purposeOfReference) const
{
    OFCondition result = EC_IllegalCall;
    /* check whether current item is valid */
    ItemStruct *item = getCurrentItem();
    /* get requested value or clear code value if invalid */
    if (item != NULL)
    {
        purposeOfReference = item->PurposeOfReference;
        result = EC_Normal;
    } else
        purposeOfReference.clear();
    return result;
}


OFCondition DSRReferencedInstanceList::setPurposeOfReference(const DSRCodedEntryValue &purposeOfReference,
                                                             const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    /* check whether current item is valid */
    ItemStruct *item = getCurrentItem();
    if (item != NULL)
    {
        if (check)
        {
            /* check whether the passed value is valid */
            result = checkPurposeOfReference(purposeOfReference);
        } else {
            /* make sure that the mandatory values are non-empty */
            result = purposeOfReference.isEmpty() ? SR_EC_InvalidValue : EC_Normal;
        }
        if (result.good())
            item->PurposeOfReference = purposeOfReference;
    }
    return result;
}


OFCondition DSRReferencedInstanceList::checkSOPInstance(const OFString &sopClassUID,
                                                        const OFString &instanceUID) const
{
    OFCondition result = EC_Normal;
    /* the two UID values should never be empty */
    if (sopClassUID.empty() || instanceUID.empty())
        result = SR_EC_InvalidValue;
    /* check for conformance with VR and VM */
    if (result.good())
        result = DcmUniqueIdentifier::checkStringValue(sopClassUID, "1");
    if (result.good())
        result = DcmUniqueIdentifier::checkStringValue(instanceUID, "1");
    return result;
}


OFCondition DSRReferencedInstanceList::checkPurposeOfReference(const DSRCodedEntryValue &purposeOfReference) const
{
    /* purpose of reference should never be empty */
    return purposeOfReference.isEmpty() ? SR_EC_InvalidValue
                                        : purposeOfReference.checkCurrentValue();
}
