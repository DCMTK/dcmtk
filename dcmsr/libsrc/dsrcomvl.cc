/*
 *
 *  Copyright (C) 2000-2013, OFFIS e.V.
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
 *    classes: DSRCompositeReferenceValue
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/dsrtypes.h"
#include "dcmtk/dcmsr/dsrcomvl.h"
#include "dcmtk/dcmsr/dsrxmld.h"


DSRCompositeReferenceValue::DSRCompositeReferenceValue()
  : SOPClassUID(),
    SOPInstanceUID()
{
}


DSRCompositeReferenceValue::DSRCompositeReferenceValue(const OFString &sopClassUID,
                                                       const OFString &sopInstanceUID,
                                                       const OFBool check)
  : SOPClassUID(),
    SOPInstanceUID()
{
    /* use the set method for checking purposes */
    setReference(sopClassUID, sopInstanceUID, check);
}


DSRCompositeReferenceValue::DSRCompositeReferenceValue(const DSRCompositeReferenceValue &referenceValue)
  : SOPClassUID(referenceValue.SOPClassUID),
    SOPInstanceUID(referenceValue.SOPInstanceUID)
{
    /* do not check since this would be unexpected to the user */
}


DSRCompositeReferenceValue::~DSRCompositeReferenceValue()
{
}


DSRCompositeReferenceValue &DSRCompositeReferenceValue::operator=(const DSRCompositeReferenceValue &referenceValue)
{
    /* do not check since this would be unexpected to the user */
    SOPClassUID = referenceValue.SOPClassUID;
    SOPInstanceUID = referenceValue.SOPInstanceUID;
    return *this;
}


void DSRCompositeReferenceValue::clear()
{
    SOPClassUID.clear();
    SOPInstanceUID.clear();
}


OFBool DSRCompositeReferenceValue::isValid() const
{
    return checkSOPClassUID(SOPClassUID).good() && checkSOPInstanceUID(SOPInstanceUID).good();
}


OFBool DSRCompositeReferenceValue::isEmpty() const
{
    return SOPClassUID.empty() && SOPInstanceUID.empty();
}


OFCondition DSRCompositeReferenceValue::print(STD_NAMESPACE ostream &stream,
                                              const size_t flags) const
{
    /* first, determine SOP class component */
    OFString sopClassString = "\"" + SOPClassUID + "\"";
    if (!(flags & DSRTypes::PF_printSOPClassUID))
    {
        /* look up name of known SOP classes */
        const char *className = dcmFindNameOfUID(SOPClassUID.c_str());
        if (className != NULL)
            sopClassString = className;
    }
    /* and then, print it */
    stream << "(" << sopClassString << ",";
    /* print SOP instance component (if desired) */
    if (flags & DSRTypes::PF_printSOPInstanceUID)
        stream << "\"" << SOPInstanceUID << "\"";
    stream << ")";
    return EC_Normal;
}


OFCondition DSRCompositeReferenceValue::readXML(const DSRXMLDocument &doc,
                                                DSRXMLCursor cursor)
{
    OFCondition result = SR_EC_CorruptedXMLStructure;
    /* go one node level down */
    if (cursor.gotoChild().valid())
    {
        /* retrieve SOP Class UID and SOP Instance UID from XML tag (required) */
        doc.getStringFromAttribute(doc.getNamedNode(cursor, "sopclass"), SOPClassUID, "uid");
        doc.getStringFromAttribute(doc.getNamedNode(cursor, "instance"), SOPInstanceUID, "uid");
        /* check whether value is valid */
        result = isValid() ? EC_Normal : SR_EC_InvalidValue;
    }
    return result;
}


OFCondition DSRCompositeReferenceValue::writeXML(STD_NAMESPACE ostream &stream,
                                                 const size_t flags) const
{
    if ((flags & DSRTypes::XF_writeEmptyTags) || !isEmpty())
    {
        stream << "<sopclass uid=\"" << SOPClassUID << "\">";
        /* retrieve name of SOP class (if known) */
        stream << dcmFindNameOfUID(SOPClassUID.c_str(), "" /* empty value as default */);
        stream << "</sopclass>" << OFendl;
        stream << "<instance uid=\"" << SOPInstanceUID << "\"/>" << OFendl;
    }
    return EC_Normal;
}


OFCondition DSRCompositeReferenceValue::readItem(DcmItem &dataset)
{
    /* read ReferencedSOPClassUID */
    OFCondition result = DSRTypes::getAndCheckStringValueFromDataset(dataset, DCM_ReferencedSOPClassUID, SOPClassUID, "1", "1", "ReferencedSOPSequence");
    /* read ReferencedSOPInstanceUID */
    if (result.good())
        result = DSRTypes::getAndCheckStringValueFromDataset(dataset, DCM_ReferencedSOPInstanceUID, SOPInstanceUID, "1", "1", "ReferencedSOPSequence");
    return result;
}


OFCondition DSRCompositeReferenceValue::writeItem(DcmItem &dataset) const
{
    /* write ReferencedSOPClassUID */
    OFCondition result = DSRTypes::putStringValueToDataset(dataset, DCM_ReferencedSOPClassUID, SOPClassUID);
    /* write ReferencedSOPInstanceUID */
    if (result.good())
        result = DSRTypes::putStringValueToDataset(dataset, DCM_ReferencedSOPInstanceUID, SOPInstanceUID);
    return result;
}


OFCondition DSRCompositeReferenceValue::readSequence(DcmItem &dataset,
                                                     const DcmTagKey &tagKey,
                                                     const OFString &type)
{
    /* read specified sequence with its item */
    DcmSequenceOfItems *dseq = NULL;
    OFCondition result = dataset.findAndGetSequence(tagKey, dseq);
    DSRTypes::checkElementValue(dseq, tagKey, "1", type, result, "content item");
    if (result.good())
    {
        /* read first item */
        DcmItem *ditem = dseq->getItem(0);
        if (ditem != NULL)
            result = readItem(*ditem);
        else
            result = SR_EC_InvalidDocumentTree;
    }
    return result;
}


OFCondition DSRCompositeReferenceValue::writeSequence(DcmItem &dataset,
                                                      const DcmTagKey &tagKey) const
{
    OFCondition result = EC_MemoryExhausted;
    /* write specified sequence with its item */
    DcmSequenceOfItems *dseq = new DcmSequenceOfItems(tagKey);
    if (dseq != NULL)
    {
        DcmItem *ditem = new DcmItem();
        if (ditem != NULL)
        {
            /* write item */
            result = writeItem(*ditem);
            if (result.good())
                dseq->insert(ditem);
            else
                delete ditem;
        } else
            result = EC_MemoryExhausted;
        /* write sequence */
        if (result.good())
            result = dataset.insert(dseq, OFTrue /*replaceOld*/);
        if (result.bad())
            delete dseq;
    }
    return result;
}


OFCondition DSRCompositeReferenceValue::renderHTML(STD_NAMESPACE ostream &docStream,
                                                   STD_NAMESPACE ostream & /*annexStream*/,
                                                   size_t & /*annexNumber*/,
                                                   const size_t /*flags*/) const
{
    /* render reference */
    docStream << "<a href=\"" << HTML_HYPERLINK_PREFIX_FOR_CGI;
    docStream << "?composite=" << SOPClassUID << "+" << SOPInstanceUID << "\">";
    /* retrieve name of SOP class (if known) */
    docStream << dcmFindNameOfUID(SOPClassUID.c_str(), "unknown composite object");
    docStream << "</a>";
    return EC_Normal;
}


OFCondition DSRCompositeReferenceValue::getValue(DSRCompositeReferenceValue &referenceValue) const
{
    referenceValue = *this;
    return EC_Normal;
}


OFCondition DSRCompositeReferenceValue::setValue(const DSRCompositeReferenceValue &referenceValue,
                                                 const OFBool check)
{
    return setReference(referenceValue.SOPClassUID, referenceValue.SOPInstanceUID, check);
}


OFCondition DSRCompositeReferenceValue::setReference(const OFString &sopClassUID,
                                                     const OFString &sopInstanceUID,
                                                     const OFBool check)
{
    OFCondition result = EC_Normal;
    /* first, make sure that the mandatory values are non-empty */
    if (sopClassUID.empty() || sopInstanceUID.empty())
        result = EC_IllegalParameter;
    else if (check)
    {
        /* then, check whether the passed values are valid */
        result = checkSOPClassUID(sopClassUID);
        if (result.good())
            result = checkSOPInstanceUID(sopInstanceUID);
    }
    if (result.good())
    {
        SOPClassUID = sopClassUID;
        SOPInstanceUID = sopInstanceUID;
    }
    return result;
}


OFCondition DSRCompositeReferenceValue::setSOPClassUID(const OFString &sopClassUID,
                                                       const OFBool check)
{
    OFCondition result = EC_Normal;
    /* first, make sure that the mandatory value is non-empty */
    if (sopClassUID.empty())
        result = EC_IllegalParameter;
    else if (check)
    {
        /* then, check whether the passed value is valid */
        result = checkSOPClassUID(sopClassUID);
    }
    if (result.good())
        SOPClassUID = sopClassUID;
    return result;
}


OFCondition DSRCompositeReferenceValue::setSOPInstanceUID(const OFString &sopInstanceUID,
                                                          const OFBool check)
{
    OFCondition result = EC_Normal;
    /* first, make sure that the mandatory value is non-empty */
    if (sopInstanceUID.empty())
        result = EC_IllegalParameter;
    else if (check)
    {
        /* then, check whether the passed value is valid */
        result = checkSOPInstanceUID(sopInstanceUID);
    }
    if (result.good())
        SOPInstanceUID = sopInstanceUID;
    return result;
}


OFCondition DSRCompositeReferenceValue::checkSOPClassUID(const OFString &sopClassUID) const
{
    OFCondition result = sopClassUID.empty() ? SR_EC_InvalidValue : EC_Normal;
    if (result.good())
        result = DcmUniqueIdentifier::checkStringValue(sopClassUID, "1");
    return result;
}


OFCondition DSRCompositeReferenceValue::checkSOPInstanceUID(const OFString &sopInstanceUID) const
{
    OFCondition result = sopInstanceUID.empty() ? SR_EC_InvalidValue : EC_Normal;
    if (result.good())
        result = DcmUniqueIdentifier::checkStringValue(sopInstanceUID, "1");
    return result;
}


OFCondition DSRCompositeReferenceValue::checkCurrentValue() const
{
    OFCondition result = checkSOPClassUID(SOPClassUID);
    if (result.good())
        result = checkSOPInstanceUID(SOPInstanceUID);
    return result;
}
