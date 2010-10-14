/*
 *
 *  Copyright (C) 2003-2010, OFFIS e.V.
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
 *  Module:  dcmsr
 *
 *  Author:  Joerg Riesmeier
 *
 *  Purpose:
 *    classes: DSRCodingSchemeIdentificationList
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:14:41 $
 *  CVS/RCS Revision: $Revision: 1.17 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
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
    DcmSequenceOfItems sequence(DCM_CodingSchemeIdentificationSequence);
    OFCondition result = getElementFromDataset(dataset, sequence);
    checkElementValue(sequence, "1-n", "3", result, "SOPCommonModule");
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
        ItemStruct *item = OFstatic_cast(ItemStruct *, *iter);
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
        const OFListIterator(ItemStruct *) last = ItemList.end();
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
    OFListConstIterator(ItemStruct *) it = Iterator;

    if (it != ItemList.end())
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
 *  Revision 1.17  2010-10-14 13:14:41  joergr
 *  Updated copyright header. Added reference to COPYRIGHT file.
 *
 *  Revision 1.16  2010-09-29 08:32:26  joergr
 *  Used more specific "moduleName" for getAndCheckElementFromDataset() and
 *  checkElementValue().
 *
 *  Revision 1.15  2009-10-14 10:49:32  joergr
 *  Fixed minor issues in log output. Also updated copyright date (if required).
 *
 *  Revision 1.14  2009-10-13 14:57:51  uli
 *  Switched to logging mechanism provided by the "new" oflog module.
 *
 *  Revision 1.13  2009-09-04 13:53:09  meichel
 *  Minor const iterator related changes needed to compile with VC6 with HAVE_STL
 *
 *  Revision 1.12  2008-07-17 12:00:09  joergr
 *  Replaced call to getSequenceFromDataset() by getElementFromDataset().
 *
 *  Revision 1.11  2008-04-29 13:59:21  joergr
 *  Harmonized capitalization of warning messages.
 *
 *  Revision 1.10  2007/11/15 16:45:42  joergr
 *  Added support for output in XHTML 1.1 format.
 *
 *  Revision 1.9  2006/08/15 16:40:03  meichel
 *  Updated the code in module dcmsr to correctly compile when
 *    all standard C++ classes remain in namespace std.
 *
 *  Revision 1.8  2006/05/11 09:16:49  joergr
 *  Moved containsExtendedCharacters() from dcmsr to dcmdata module.
 *
 *  Revision 1.7  2005/12/08 15:47:46  meichel
 *  Changed include path schema for all DCMTK header files
 *
 *  Revision 1.6  2004/11/22 16:39:12  meichel
 *  Added method that checks if the SR document contains non-ASCII characters
 *    in any of the strings affected by SpecificCharacterSet.
 *
 *  Revision 1.5  2004/01/16 10:02:19  joergr
 *  Report more warnings when reading from XML document.
 *
 *  Revision 1.4  2003/10/14 15:32:13  joergr
 *  Uncommented name of unused parameter to get rid of a warning reported by
 *  Borland C++ 5.5.
 *
 *  Revision 1.3  2003/10/09 17:47:54  joergr
 *  Renamed OFFIS_PRIVATE_CODING_xxx macros to OFFIS_CODING_xxx.
 *
 *  Revision 1.2  2003/09/15 13:55:45  joergr
 *  Made XML output more consistent: changed XML tag name from "identifier" to
 *  "id".
 *
 *  Revision 1.1  2003/09/10 13:18:43  joergr
 *  Replaced PrivateCodingSchemeUID by new CodingSchemeIdenticationSequence as
 *  required by CP 324.
 *
 *
 */
