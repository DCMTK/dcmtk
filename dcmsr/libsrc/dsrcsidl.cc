/*
 *
 *  Copyright (C) 2003-2013, OFFIS e.V.
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
 *    classes: DSRCodingSchemeIdentificationList
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/dsrcsidl.h"
#include "dcmtk/dcmsr/dsrxmld.h"


DSRCodingSchemeIdentificationList::DSRCodingSchemeIdentificationList()
  : ItemList(),
    Iterator()
{
    /* initialize list cursor */
    Iterator = ItemList.end();
}


DSRCodingSchemeIdentificationList::~DSRCodingSchemeIdentificationList()
{
    /* clear list and delete allocated memory */
    clear();
}


void DSRCodingSchemeIdentificationList::clear()
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


OFBool DSRCodingSchemeIdentificationList::empty() const
{
    return ItemList.empty();
}


size_t DSRCodingSchemeIdentificationList::getNumberOfItems() const
{
    return ItemList.size();
}


OFCondition DSRCodingSchemeIdentificationList::read(DcmItem &dataset)
{
    /* first, check whether sequence is present and non-empty */
    DcmSequenceOfItems *sequence = NULL;
    OFCondition result = dataset.findAndGetSequence(DCM_CodingSchemeIdentificationSequence, sequence);
    checkElementValue(sequence, DCM_CodingSchemeIdentificationSequence, "1-n", "3", result, "SOPCommonModule");
    if (result.good())
    {
        ItemStruct *item = NULL;
        OFString codingSchemeDesignator;
        /* iterate over all sequence items */
        DcmObject *dobj = NULL;
        while ((dobj = sequence->nextInContainer(dobj)) != NULL)
        {
            DcmItem *ditem = OFstatic_cast(DcmItem *, dobj);
            /* get the coding scheme designator */
            if (getAndCheckStringValueFromDataset(*ditem, DCM_CodingSchemeDesignator, codingSchemeDesignator, "1", "1", "CodingSchemeIdentificationSequence").good())
            {
                /* add new item to the list */
                if (addItem(codingSchemeDesignator, item).good())
                {
                    /* read additional information */
                    getAndCheckStringValueFromDataset(*ditem, DCM_CodingSchemeRegistry, item->CodingSchemeRegistry, "1", "1C", "CodingSchemeIdentificationSequence");
                    getAndCheckStringValueFromDataset(*ditem, DCM_CodingSchemeUID, item->CodingSchemeUID, "1", "1C", "CodingSchemeIdentificationSequence");
                    getAndCheckStringValueFromDataset(*ditem, DCM_CodingSchemeExternalID, item->CodingSchemeExternalID, "1", "2C", "CodingSchemeIdentificationSequence");
                    getAndCheckStringValueFromDataset(*ditem, DCM_CodingSchemeName, item->CodingSchemeName, "1", "3", "CodingSchemeIdentificationSequence");
                    getAndCheckStringValueFromDataset(*ditem, DCM_CodingSchemeVersion, item->CodingSchemeVersion, "1", "3", "CodingSchemeIdentificationSequence");
                    getAndCheckStringValueFromDataset(*ditem, DCM_ResponsibleOrganization, item->ResponsibleOrganization, "1", "3", "CodingSchemeIdentificationSequence");
                }
            }
        }
    }
    return result;
}


OFCondition DSRCodingSchemeIdentificationList::write(DcmItem &dataset) const
{
    OFCondition result = EC_Normal;
    /* iterate over all list items */
    OFListConstIterator(ItemStruct *) iter = ItemList.begin();
    const OFListConstIterator(ItemStruct *) last = ItemList.end();
    while ((iter != last) && result.good())
    {
        ItemStruct *item = *iter;
        /* check whether list item really exists and is valid */
        if ((item != NULL) && !item->CodingSchemeDesignator.empty())
        {
            DcmItem *ditem = NULL;
            /* create a new item (and a sequence if required) */
            result = dataset.findOrCreateSequenceItem(DCM_CodingSchemeIdentificationSequence, ditem, -2 /*append new*/);
            /* write item data */
            if (result.good())
            {
                putStringValueToDataset(*ditem, DCM_CodingSchemeDesignator, item->CodingSchemeDesignator);
                putStringValueToDataset(*ditem, DCM_CodingSchemeRegistry, item->CodingSchemeRegistry, OFFalse /*allowEmpty*/);
                putStringValueToDataset(*ditem, DCM_CodingSchemeUID, item->CodingSchemeUID, OFFalse /*allowEmpty*/);
                if (item->CodingSchemeUID.empty())
                    putStringValueToDataset(*ditem, DCM_CodingSchemeExternalID, item->CodingSchemeExternalID, OFFalse /*allowEmpty*/);
                else if (!item->CodingSchemeExternalID.empty())
                {
                    DCMSR_WARN("Both CodingSchemeUID and CodingSchemeExternalID present for \""
                        << item->CodingSchemeDesignator << "\", the latter will be ignored");
                }
                putStringValueToDataset(*ditem, DCM_CodingSchemeName, item->CodingSchemeName, OFFalse /*allowEmpty*/);
                putStringValueToDataset(*ditem, DCM_CodingSchemeVersion, item->CodingSchemeVersion, OFFalse /*allowEmpty*/);
                putStringValueToDataset(*ditem, DCM_ResponsibleOrganization, item->ResponsibleOrganization, OFFalse /*allowEmpty*/);
            }
        }
        ++iter;
    }
    return result;
}


OFCondition DSRCodingSchemeIdentificationList::readXML(const DSRXMLDocument &doc,
                                                       DSRXMLCursor cursor,
                                                       const size_t /*flags*/)
{
    OFCondition result = SR_EC_InvalidDocument;
    ItemStruct *item = NULL;
    OFString codingSchemeDesignator;
    /* iterate over all nodes */
    while (cursor.valid())
    {
        /* check for known element tags */
        if (doc.checkNode(cursor, "scheme").good())
        {
            /* retrieve coding scheme designator */
            if (!doc.getStringFromAttribute(cursor, codingSchemeDesignator, "designator", OFTrue /*encoding*/).empty())
            {
                result = addItem(codingSchemeDesignator, item);
                if (result.good())
                {
                    DSRXMLCursor childCursor = cursor.getChild();
                    /* clear any previously stored information */
                    item->clear();
                    while (childCursor.valid())
                    {
                        /* check for known element tags */
                        doc.getStringFromNodeContent(childCursor, item->CodingSchemeRegistry, "registry", OFTrue /*encoding*/, OFFalse /*clearString*/);
                        doc.getStringFromNodeContent(childCursor, item->CodingSchemeUID, "uid", OFFalse /*encoding*/, OFFalse /*clearString*/);
                        doc.getStringFromNodeContent(childCursor, item->CodingSchemeExternalID, "id", OFTrue /*encoding*/, OFFalse /*clearString*/);
                        doc.getStringFromNodeContent(childCursor, item->CodingSchemeName, "name", OFTrue /*encoding*/, OFFalse /*clearString*/);
                        doc.getStringFromNodeContent(childCursor, item->CodingSchemeVersion, "version", OFTrue /*encoding*/, OFFalse /*clearString*/);
                        doc.getStringFromNodeContent(childCursor, item->ResponsibleOrganization, "organization", OFTrue /*encoding*/, OFFalse /*clearString*/);
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


OFCondition DSRCodingSchemeIdentificationList::writeXML(STD_NAMESPACE ostream &stream,
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
            stream << "<scheme designator=\"" << convertToXMLString(item->CodingSchemeDesignator, tmpString) << "\">" << OFendl;
            writeStringValueToXML(stream, convertToXMLString(item->CodingSchemeRegistry, tmpString), "registry", (flags & DSRTypes::XF_writeEmptyTags) > 0);
            writeStringValueToXML(stream, item->CodingSchemeUID, "uid", (flags & DSRTypes::XF_writeEmptyTags) > 0);
            writeStringValueToXML(stream, convertToXMLString(item->CodingSchemeExternalID, tmpString), "identifier", (flags & DSRTypes::XF_writeEmptyTags) > 0);
            writeStringValueToXML(stream, convertToXMLString(item->CodingSchemeName, tmpString), "name", (flags & DSRTypes::XF_writeEmptyTags) > 0);
            writeStringValueToXML(stream, convertToXMLString(item->CodingSchemeVersion, tmpString), "version", (flags & DSRTypes::XF_writeEmptyTags) > 0);
            writeStringValueToXML(stream, convertToXMLString(item->ResponsibleOrganization, tmpString), "organization", (flags & DSRTypes::XF_writeEmptyTags) > 0);
            stream << "</scheme>" << OFendl;
        }
        ++iter;
    }
    return EC_Normal;
}


OFCondition DSRCodingSchemeIdentificationList::addPrivateDcmtkCodingScheme()
{
    ItemStruct *item = NULL;
    /* add private coding scheme (if not already existent) */
    OFCondition result = addItem(OFFIS_CODING_SCHEME_DESIGNATOR, item);
    if (result.good())
    {
        /* set additional information */
        item->CodingSchemeRegistry.clear();
        item->CodingSchemeUID = OFFIS_CODING_SCHEME_UID;
        item->CodingSchemeExternalID.clear();
        item->CodingSchemeName = OFFIS_CODING_SCHEME_NAME;
        item->CodingSchemeVersion.clear();  // there are currently no different versions
        item->ResponsibleOrganization = OFFIS_RESPONSIBLE_ORGANIZATION;
    }
    return result;
}


OFCondition DSRCodingSchemeIdentificationList::addItem(const OFString &codingSchemeDesignator,
                                                       ItemStruct *&item)
{
    OFCondition result = EC_IllegalParameter;
    /* check parameter first */
    if (!codingSchemeDesignator.empty())
    {
        result = EC_Normal;
        OFListIterator(ItemStruct *) oldCursor = Iterator;
        /* check whether item already exists */
        if (gotoItem(codingSchemeDesignator).bad())
        {
            /* if not create new item and add it to the list */
            item = new ItemStruct(codingSchemeDesignator);
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
            DCMSR_WARN("CodingSchemeDesignator \"" << codingSchemeDesignator
                << "\" already exists in CodingSchemeIdentificationSequence ... overwriting");
            /* gotoItem() was successful, set item pointer */
            item = *Iterator;
        }
    } else
        item = NULL;
    return result;
}


OFCondition DSRCodingSchemeIdentificationList::addItem(const OFString &codingSchemeDesignator,
                                                       const OFBool check)
{
    OFCondition result = EC_Normal;
    /* make sure that the mandatory value is non-empty */
    if (codingSchemeDesignator.empty())
        result = EC_IllegalParameter;
    else if (check)
    {
        /* check whether the passed value is valid */
        result = DcmShortString::checkStringValue(codingSchemeDesignator, "1");
    }
    if (result.good())
    {
        ItemStruct *item = NULL;
        /* call the "real" function */
        result = addItem(codingSchemeDesignator, item);
    }
    return result;
}


OFCondition DSRCodingSchemeIdentificationList::removeItem()
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


OFCondition DSRCodingSchemeIdentificationList::removeItem(const OFString &codingSchemeDesignator)
{
    /* goto specified item ... */
    OFCondition result = gotoItem(codingSchemeDesignator);
    /* ... and remove it */
    if (result.good())
        result = removeItem();
    return result;
}


OFCondition DSRCodingSchemeIdentificationList::gotoItem(const OFString &codingSchemeDesignator)
{
    OFCondition result = EC_IllegalParameter;
    /* check parameter first */
    if (!codingSchemeDesignator.empty())
    {
        result = SR_EC_CodingSchemeNotFound;
        /* start with first item */
        Iterator = ItemList.begin();
        const OFListIterator(ItemStruct *) last = ItemList.end();
        /* search for given item */
        while ((Iterator != last) && result.bad())
        {
            if ((*Iterator != NULL) && ((*Iterator)->CodingSchemeDesignator == codingSchemeDesignator))
                result = EC_Normal;
            else
                ++Iterator;
        }
    }
    return result;
}


OFCondition DSRCodingSchemeIdentificationList::gotoFirstItem()
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


OFCondition DSRCodingSchemeIdentificationList::gotoNextItem()
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


DSRCodingSchemeIdentificationList::ItemStruct *DSRCodingSchemeIdentificationList::getCurrentItem() const
{
    ItemStruct *item = NULL;
    /* check whether current item is valid */
    OFListConstIterator(ItemStruct *) it = Iterator;
    if (it != ItemList.end())
        item = *Iterator;
    return item;
}


const OFString &DSRCodingSchemeIdentificationList::getCodingSchemeDesignator(OFString &stringValue) const
{
    /* check whether current item is valid */
    ItemStruct *item = getCurrentItem();
    /* get requested value or clear string if invalid */
    if (item != NULL)
        stringValue = item->CodingSchemeDesignator;
    else
        stringValue.clear();
    return stringValue;
}


const OFString &DSRCodingSchemeIdentificationList::getCodingSchemeRegistry(OFString &stringValue) const
{
    /* check whether current item is valid */
    ItemStruct *item = getCurrentItem();
    /* get requested value or clear string if invalid */
    if (item != NULL)
        stringValue = item->CodingSchemeRegistry;
    else
        stringValue.clear();
    return stringValue;
}


const OFString &DSRCodingSchemeIdentificationList::getCodingSchemeUID(OFString &stringValue) const
{
    /* check whether current item is valid */
    ItemStruct *item = getCurrentItem();
    /* get requested value or clear string if invalid */
    if (item != NULL)
        stringValue = item->CodingSchemeUID;
    else
        stringValue.clear();
    return stringValue;
}


const OFString &DSRCodingSchemeIdentificationList::getCodingSchemeExternalID(OFString &stringValue) const
{
    /* check whether current item is valid */
    ItemStruct *item = getCurrentItem();
    /* get requested value or clear string if invalid */
    if (item != NULL)
        stringValue = item->CodingSchemeExternalID;
    else
        stringValue.clear();
    return stringValue;
}


const OFString &DSRCodingSchemeIdentificationList::getCodingSchemeName(OFString &stringValue) const
{
    /* check whether current item is valid */
    ItemStruct *item = getCurrentItem();
    /* get requested value or clear string if invalid */
    if (item != NULL)
        stringValue = item->CodingSchemeName;
    else
        stringValue.clear();
    return stringValue;
}


const OFString &DSRCodingSchemeIdentificationList::getCodingSchemeVersion(OFString &stringValue) const
{
    /* check whether current item is valid */
    ItemStruct *item = getCurrentItem();
    /* get requested value or clear string if invalid */
    if (item != NULL)
        stringValue = item->CodingSchemeVersion;
    else
        stringValue.clear();
    return stringValue;
}


const OFString &DSRCodingSchemeIdentificationList::getResponsibleOrganization(OFString &stringValue) const
{
    /* check whether current item is valid */
    ItemStruct *item = getCurrentItem();
    /* get requested value or clear string if invalid */
    if (item != NULL)
        stringValue = item->ResponsibleOrganization;
    else
        stringValue.clear();
    return stringValue;
}


OFCondition DSRCodingSchemeIdentificationList::setCodingSchemeRegistry(const OFString &value,
                                                                       const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    /* check whether current item is valid */
    ItemStruct *item = getCurrentItem();
    if (item != NULL)
    {
        /* set the value (if valid) */
        result = (check) ? DcmLongString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            item->CodingSchemeRegistry = value;
    }
    return result;
}


OFCondition DSRCodingSchemeIdentificationList::setCodingSchemeUID(const OFString &value,
                                                                  const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    /* check whether current item is valid */
    ItemStruct *item = getCurrentItem();
    if (item != NULL)
    {
        /* set the value (if valid) */
        result = (check) ? DcmUniqueIdentifier::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            item->CodingSchemeUID = value;
    }
    return result;
}


OFCondition DSRCodingSchemeIdentificationList::setCodingSchemeExternalID(const OFString &value,
                                                                         const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    /* check whether current item is valid */
    ItemStruct *item = getCurrentItem();
    if (item != NULL)
    {
        /* set the value (if valid) */
        result = (check) ? DcmShortText::checkStringValue(value) : EC_Normal;
        if (result.good())
            item->CodingSchemeExternalID = value;
    }
    return result;
}


OFCondition DSRCodingSchemeIdentificationList::setCodingSchemeName(const OFString &value,
                                                                   const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    /* check whether current item is valid */
    ItemStruct *item = getCurrentItem();
    if (item != NULL)
    {
        /* set the value (if valid) */
        result = (check) ? DcmShortText::checkStringValue(value) : EC_Normal;
        if (result.good())
            item->CodingSchemeName = value;
    }
    return result;
}


OFCondition DSRCodingSchemeIdentificationList::setCodingSchemeVersion(const OFString &value,
                                                                      const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    /* check whether current item is valid */
    ItemStruct *item = getCurrentItem();
    if (item != NULL)
    {
        /* set the value (if valid) */
        result = (check) ? DcmShortString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            item->CodingSchemeVersion = value;
    }
    return result;
}


OFCondition DSRCodingSchemeIdentificationList::setResponsibleOrganization(const OFString &value,
                                                                          const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    /* check whether current item is valid */
    ItemStruct *item = getCurrentItem();
    if (item != NULL)
    {
        /* set the value (if valid) */
        result = (check) ? DcmShortText::checkStringValue(value) : EC_Normal;
        if (result.good())
            item->ResponsibleOrganization = value;
    }
    return result;
}
