/*
 *
 *  Copyright (C) 2003, OFFIS
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
 *  Module:  dcmsr
 *
 *  Author:  Joerg Riesmeier
 *
 *  Purpose:
 *    classes: DSRCodingSchemeIdentificationList
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2003-09-15 13:55:45 $
 *  CVS/RCS Revision: $Revision: 1.2 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include "dsrcsidl.h"
#include "dsrxmld.h"


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
    const OFListConstIterator(ItemStruct *) last = ItemList.end();
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


OFCondition DSRCodingSchemeIdentificationList::read(DcmItem &dataset,
                                                    OFConsole *logStream)
{
    /* first, check whether sequence is present and non-empty */
    DcmSequenceOfItems sequence(DCM_CodingSchemeIdentificationSequence);
    OFCondition result = getSequenceFromDataset(dataset, sequence);
    checkElementValue(sequence, "1-n", "3", logStream, result);
    if (result.good())
    {
        ItemStruct *item = NULL;
        OFString codingSchemeDesignator;
        /* iterate over all sequence items */
        for (unsigned long i = 0; i < sequence.card(); i++)
        {
            DcmItem *ditem = sequence.getItem(i);
            if (ditem != NULL)
            {
                /* get the coding scheme designator */
                if (getAndCheckStringValueFromDataset(*ditem, DCM_CodingSchemeDesignator, codingSchemeDesignator, "1", "1", logStream).good())
                {
                    /* add new item to the list */
                    if (addItem(codingSchemeDesignator, item, logStream).good())
                    {
                        /* read additional information */
                        getAndCheckStringValueFromDataset(*ditem, DCM_CodingSchemeRegistry, item->CodingSchemeRegistry, "1", "1C", logStream);
                        getAndCheckStringValueFromDataset(*ditem, DCM_CodingSchemeUID, item->CodingSchemeUID, "1", "1C", logStream);
                        getAndCheckStringValueFromDataset(*ditem, DCM_CodingSchemeExternalID, item->CodingSchemeExternalID, "", "2C", logStream);
                        getAndCheckStringValueFromDataset(*ditem, DCM_CodingSchemeName, item->CodingSchemeName, "1", "3", logStream);
                        getAndCheckStringValueFromDataset(*ditem, DCM_CodingSchemeVersion, item->CodingSchemeVersion, "1", "3", logStream);
                        getAndCheckStringValueFromDataset(*ditem, DCM_ResponsibleOrganization, item->ResponsibleOrganization, "1", "3", logStream);
                    }
                }
            }
        }
    }
    return result;
}


OFCondition DSRCodingSchemeIdentificationList::write(DcmItem &dataset,
                                                     OFConsole *logStream) const
{
    OFCondition result = EC_Normal;
    /* iterate over all list items */
    OFListConstIterator(ItemStruct *) iter = ItemList.begin();
    const OFListConstIterator(ItemStruct *) last = ItemList.end();
    while ((iter != last) && result.good())
    {
        ItemStruct *item = OFstatic_cast(ItemStruct *, *iter);
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
                    OFString message = "both CodingSchemeUID and CodingSchemeExternalID present for \"";
                    message += item->CodingSchemeDesignator;
                    message += "\", the latter will be ignored";
                    printWarningMessage(logStream, message.c_str());
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
                                                       const size_t flags)
{
    OFCondition result = SR_EC_InvalidDocument;
    ItemStruct *item = NULL;
    OFString codingSchemeDesignator;
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


OFCondition DSRCodingSchemeIdentificationList::writeXML(ostream &stream,
                                                        const size_t flags) const
{
    OFString tmpString;
    /* iterate over all list items */
    OFListConstIterator(ItemStruct *) iter = ItemList.begin();
    const OFListConstIterator(ItemStruct *) last = ItemList.end();
    while (iter != last)
    {
        ItemStruct *item = OFstatic_cast(ItemStruct *, *iter);
        /* check whether list item really exists */
        if (item != NULL)
        {
            stream << "<scheme designator=\"" << convertToMarkupString(item->CodingSchemeDesignator, tmpString, OFFalse /*convertNonASCII*/, OFFalse /*newlineAllowed*/, OFTrue /*xmlMode*/) << "\">" << endl;
            writeStringValueToXML(stream, convertToMarkupString(item->CodingSchemeRegistry, tmpString, OFFalse /*convertNonASCII*/, OFFalse /*newlineAllowed*/, OFTrue /*xmlMode*/), "registry", (flags & DSRTypes::XF_writeEmptyTags) > 0);
            writeStringValueToXML(stream, item->CodingSchemeUID, "uid", (flags & DSRTypes::XF_writeEmptyTags) > 0);
            writeStringValueToXML(stream, convertToMarkupString(item->CodingSchemeExternalID, tmpString, OFFalse /*convertNonASCII*/, OFFalse /*newlineAllowed*/, OFTrue /*xmlMode*/), "identifier", (flags & DSRTypes::XF_writeEmptyTags) > 0);
            writeStringValueToXML(stream, convertToMarkupString(item->CodingSchemeName, tmpString, OFFalse /*convertNonASCII*/, OFFalse /*newlineAllowed*/, OFTrue /*xmlMode*/), "name", (flags & DSRTypes::XF_writeEmptyTags) > 0);
            writeStringValueToXML(stream, convertToMarkupString(item->CodingSchemeVersion, tmpString, OFFalse /*convertNonASCII*/, OFFalse /*newlineAllowed*/, OFTrue /*xmlMode*/), "version", (flags & DSRTypes::XF_writeEmptyTags) > 0);
            writeStringValueToXML(stream, convertToMarkupString(item->ResponsibleOrganization, tmpString, OFFalse /*convertNonASCII*/, OFFalse /*newlineAllowed*/, OFTrue /*xmlMode*/), "organization", (flags & DSRTypes::XF_writeEmptyTags) > 0);
            stream << "</scheme>" << endl;
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
        item->CodingSchemeUID = OFFIS_PRIVATE_CODING_SCHEME_CREATOR_UID;
        item->CodingSchemeExternalID.clear();
        item->CodingSchemeName = OFFIS_CODING_SCHEME_NAME;
        item->CodingSchemeVersion.clear();
        item->ResponsibleOrganization = OFFIS_RESPONSIBLE_ORGANIZATION;
    }
    return result;
}


OFCondition DSRCodingSchemeIdentificationList::addItem(const OFString &codingSchemeDesignator,
                                                       ItemStruct *&item,
                                                       OFConsole *logStream)
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
            OFString message = "CodingSchemeDesignator \"";
            message += codingSchemeDesignator;
            message += "\" already exists in CodingSchemeIdentificationSequence ... overwriting";
            printWarningMessage(logStream, message.c_str());
            /* gotoItem() was successful, set item pointer */
            item = OFstatic_cast(ItemStruct *, *Iterator);
        }
    } else
        item = NULL;
    return result;
}


OFCondition DSRCodingSchemeIdentificationList::addItem(const OFString &codingSchemeDesignator)
{
    ItemStruct *item = NULL;
    /* call the "real" function */
    return addItem(codingSchemeDesignator, item);
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
        const OFListConstIterator(ItemStruct *) last = ItemList.end();
        /* search for given item */
        while ((Iterator != last) && result.bad())
        {
            if ((*Iterator != NULL) && (OFstatic_cast(ItemStruct *, *Iterator)->CodingSchemeDesignator == codingSchemeDesignator))
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
    /* check for empty study list */
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
    if (Iterator != ItemList.end())
        item = OFstatic_cast(ItemStruct *, *Iterator);
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


OFCondition DSRCodingSchemeIdentificationList::setCodingSchemeRegistry(const OFString &value)
{
    OFCondition result = EC_IllegalCall;
    /* check whether current item is valid */
    ItemStruct *item = getCurrentItem();
    if (item != NULL)
    {
        /* set the value */
        item->CodingSchemeRegistry = value;
        result = EC_Normal;
    }
    return result;
}


OFCondition DSRCodingSchemeIdentificationList::setCodingSchemeUID(const OFString &value)
{
    OFCondition result = EC_IllegalCall;
    /* check whether current item is valid */
    ItemStruct *item = getCurrentItem();
    if (item != NULL)
    {
        /* set the value */
        item->CodingSchemeUID = value;
        result = EC_Normal;
    }
    return result;
}


OFCondition DSRCodingSchemeIdentificationList::setCodingSchemeExternalID(const OFString &value)
{
    OFCondition result = EC_IllegalCall;
    /* check whether current item is valid */
    ItemStruct *item = getCurrentItem();
    if (item != NULL)
    {
        /* set the value */
        item->CodingSchemeExternalID = value;
        result = EC_Normal;
    }
    return result;
}


OFCondition DSRCodingSchemeIdentificationList::setCodingSchemeName(const OFString &value)
{
    OFCondition result = EC_IllegalCall;
    /* check whether current item is valid */
    ItemStruct *item = getCurrentItem();
    if (item != NULL)
    {
        /* set the value */
        item->CodingSchemeName = value;
        result = EC_Normal;
    }
    return result;
}


OFCondition DSRCodingSchemeIdentificationList::setCodingSchemeVersion(const OFString &value)
{
    OFCondition result = EC_IllegalCall;
    /* check whether current item is valid */
    ItemStruct *item = getCurrentItem();
    if (item != NULL)
    {
        /* set the value */
        item->CodingSchemeVersion = value;
        result = EC_Normal;
    }
    return result;
}


OFCondition DSRCodingSchemeIdentificationList::setResponsibleOrganization(const OFString &value)
{
    OFCondition result = EC_IllegalCall;
    /* check whether current item is valid */
    ItemStruct *item = getCurrentItem();
    if (item != NULL)
    {
        /* set the value */
        item->ResponsibleOrganization = value;
        result = EC_Normal;
    }
    return result;
}


/*
 *  CVS/RCS Log:
 *  $Log: dsrcsidl.cc,v $
 *  Revision 1.2  2003-09-15 13:55:45  joergr
 *  Made XML output more consistent: changed XML tag name from "identifier" to
 *  "id".
 *
 *  Revision 1.1  2003/09/10 13:18:43  joergr
 *  Replaced PrivateCodingSchemeUID by new CodingSchemeIdenticationSequence as
 *  required by CP 324.
 *
 *
 *
 */
