/*
 *
 *  Copyright (C) 2015-2022, J. Riesmeier, Oldenburg, Germany
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  Source file for class CID7551_GenericPurposeOfReferenceToImagesAndCoordinatesInMeasurements
 *
 *  Generated automatically from DICOM PS 3.16-2022b
 *  File created on 2022-05-24 17:20:00 by J. Riesmeier
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/cmr/cid7551.h"


// general information on CID 7551 (Generic Purpose of Reference to Images and Coordinates in Measurements)
#define CONTEXT_GROUP_NUMBER  "7551"
#define CONTEXT_GROUP_VERSION "20200920"
#define CONTEXT_GROUP_UID     "1.2.840.10008.6.1.1343"
#define CONTEXT_GROUP_TYPE    OFTrue  /* extensible */

// initialize global/static variable
CID7551_GenericPurposeOfReferenceToImagesAndCoordinatesInMeasurements::CodeList *CID7551_GenericPurposeOfReferenceToImagesAndCoordinatesInMeasurements::Codes = NULL;


CID7551_GenericPurposeOfReferenceToImagesAndCoordinatesInMeasurements::CID7551_GenericPurposeOfReferenceToImagesAndCoordinatesInMeasurements(const DSRCodedEntryValue &selectedValue)
  : DSRContextGroup(CONTEXT_GROUP_NUMBER, "DCMR", CONTEXT_GROUP_VERSION, CONTEXT_GROUP_UID, selectedValue)
{
    setExtensible(CONTEXT_GROUP_TYPE);
}


CID7551_GenericPurposeOfReferenceToImagesAndCoordinatesInMeasurements::CID7551_GenericPurposeOfReferenceToImagesAndCoordinatesInMeasurements(const EnumType selectedValue,
                                                                                                                                             const OFBool enhancedEncodingMode)
  : DSRContextGroup(CONTEXT_GROUP_NUMBER, "DCMR", CONTEXT_GROUP_VERSION, CONTEXT_GROUP_UID, getCodedEntry(selectedValue, enhancedEncodingMode))
{
    setExtensible(CONTEXT_GROUP_TYPE);
}


OFCondition CID7551_GenericPurposeOfReferenceToImagesAndCoordinatesInMeasurements::selectValue(const EnumType selectedValue,
                                                                                               const OFBool enhancedEncodingMode)
{
    /* never check the coded entry */
    return DSRContextGroup::selectValue(getCodedEntry(selectedValue, enhancedEncodingMode), OFFalse /*check*/, OFFalse /*definedContextGroup*/);
}


OFCondition CID7551_GenericPurposeOfReferenceToImagesAndCoordinatesInMeasurements::findCodedEntry(const DSRCodedEntryValue &searchForCodedEntry,
                                                                                                  DSRCodedEntryValue *foundCodedEntry,
                                                                                                  const OFBool enhancedEncodingMode) const
{
    OFCondition result = SR_EC_CodedEntryNotInContextGroup;
    /* first, search for standard codes */
    CodeList::const_iterator iter = getCodes().begin();
    CodeList::const_iterator last = getCodes().end();
    /* iterate over coded entry list */
    while (iter != last)
    {
        /* if found, exit loop */
        if (searchForCodedEntry == iter->second)
        {
            /* return coded entry (if requested) */
            if (foundCodedEntry != NULL)
            {
                *foundCodedEntry = iter->second;
                /* also set enhanced encoding mode (if enabled) */
                if (!foundCodedEntry->isEmpty() && enhancedEncodingMode)
                    setEnhancedEncodingMode(*foundCodedEntry);
            }
            result = SR_EC_CodedEntryInStandardContextGroup;
            break;
        }
        ++iter;
    }
    /* if not, continue with extended codes */
    if (result.bad())
    {
        result = DSRContextGroup::findCodedEntry(searchForCodedEntry, foundCodedEntry);
        /* tbd: set "enhanced encoding mode" to mark a local/extended version? */
    }
    return result;
}


void CID7551_GenericPurposeOfReferenceToImagesAndCoordinatesInMeasurements::printCodes(STD_NAMESPACE ostream &stream) const
{
    /* print standard codes */
    stream << "Standard codes:" << OFendl;
    CodeList::const_iterator iter = getCodes().begin();
    CodeList::const_iterator last = getCodes().end();
    /* iterate over coded entry list */
    while (iter != last)
    {
        stream << "  ";
        /* print coded entry */
        DSRCodedEntryValue(iter->second).print(stream);
        stream << OFendl;
        ++iter;
    }
    /* print extended codes */
    DSRContextGroup::printCodes(stream);
}


// static functions

void CID7551_GenericPurposeOfReferenceToImagesAndCoordinatesInMeasurements::initialize()
{
    /* create and initialize code list */
    getCodes();
}


void CID7551_GenericPurposeOfReferenceToImagesAndCoordinatesInMeasurements::cleanup()
{
    /* delete code list, it will be recreated automatically when needed */
    delete Codes;
    Codes = NULL;
}


DSRCodedEntryValue CID7551_GenericPurposeOfReferenceToImagesAndCoordinatesInMeasurements::getCodedEntry(const EnumType value,
                                                                                                        const OFBool enhancedEncodingMode)
{
    DSRCodedEntryValue codedEntry;
    /* search for given enumerated value */
    CodeList::iterator iter = getCodes().find(value);
    /* if found, set the coded entry */
    if (iter != getCodes().end())
    {
        codedEntry = iter->second;
        /* also set enhanced encoding mode (if enabled) */
        if (!codedEntry.isEmpty() && enhancedEncodingMode)
            setEnhancedEncodingMode(codedEntry);
    }
    return codedEntry;
}


CID7551_GenericPurposeOfReferenceToImagesAndCoordinatesInMeasurements::CodeList &CID7551_GenericPurposeOfReferenceToImagesAndCoordinatesInMeasurements::getCodes()
{
    /* check whether code list has already been created and initialized */
    if (Codes == NULL)
    {
        /* create a new code list (should never fail) */
        Codes = new CodeList();
        /* and initialize it by adding the coded entries */
        Codes->insert(OFMake_pair(SourceOfMeasurement, DSRBasicCodedEntry("121112", "DCM", "Source of Measurement")));
        Codes->insert(OFMake_pair(ArmOfAngle, DSRBasicCodedEntry("121223", "DCM", "Arm of angle")));
        Codes->insert(OFMake_pair(Vector, DSRBasicCodedEntry("121225", "DCM", "Vector")));
        Codes->insert(OFMake_pair(ApproximateSpatialLocation, DSRBasicCodedEntry("121226", "DCM", "Approximate spatial location")));
    }
    /* should never be NULL */
    return *Codes;
}


OFCondition CID7551_GenericPurposeOfReferenceToImagesAndCoordinatesInMeasurements::setEnhancedEncodingMode(DSRCodedEntryValue &codedEntryValue)
{
    return codedEntryValue.setEnhancedEncodingMode(CONTEXT_GROUP_NUMBER, "DCMR", CONTEXT_GROUP_VERSION, CONTEXT_GROUP_UID);
}
