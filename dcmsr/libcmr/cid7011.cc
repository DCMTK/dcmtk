/*
 *
 *  Copyright (C) 2015-2026, J. Riesmeier, Oldenburg, Germany
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  Source file for class CID7011_RejectedForQualityReason
 *
 *  Generated automatically from DICOM PS 3.16-2026d
 *  File created on 2026-11-12 15:49:16 by J. Riesmeier
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/cmr/cid7011.h"


// general information on CID 7011 (Rejected for Quality Reason)
#define CONTEXT_GROUP_NUMBER  "7011"
#define CONTEXT_GROUP_KEYWORD "RejectedForQualityReason"
#define CONTEXT_GROUP_VERSION "20020904"
#define CONTEXT_GROUP_UID     "1.2.840.10008.6.1.491"
#define CONTEXT_GROUP_TYPE    OFTrue  /* extensible */

// initialize global/static variable
CID7011_RejectedForQualityReason::CodeList *CID7011_RejectedForQualityReason::Codes = NULL;


CID7011_RejectedForQualityReason::CID7011_RejectedForQualityReason(const DSRCodedEntryValue &selectedValue)
  : DSRContextGroup(CONTEXT_GROUP_NUMBER, "DCMR", CONTEXT_GROUP_KEYWORD, CONTEXT_GROUP_VERSION, CONTEXT_GROUP_UID, selectedValue)
{
    setExtensible(CONTEXT_GROUP_TYPE);
}


CID7011_RejectedForQualityReason::CID7011_RejectedForQualityReason(const EnumType selectedValue,
                                                                   const OFBool enhancedEncodingMode)
  : DSRContextGroup(CONTEXT_GROUP_NUMBER, "DCMR", CONTEXT_GROUP_KEYWORD, CONTEXT_GROUP_VERSION, CONTEXT_GROUP_UID, getCodedEntry(selectedValue, enhancedEncodingMode))
{
    setExtensible(CONTEXT_GROUP_TYPE);
}


OFCondition CID7011_RejectedForQualityReason::selectValue(const EnumType selectedValue,
                                                          const OFBool enhancedEncodingMode)
{
    /* never check the coded entry */
    return DSRContextGroup::selectValue(getCodedEntry(selectedValue, enhancedEncodingMode), OFFalse /*check*/, OFFalse /*definedContextGroup*/);
}


OFCondition CID7011_RejectedForQualityReason::findCodedEntry(const DSRCodedEntryValue &searchForCodedEntry,
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


void CID7011_RejectedForQualityReason::printCodes(STD_NAMESPACE ostream &stream) const
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

void CID7011_RejectedForQualityReason::initialize()
{
    /* create and initialize code list */
    getCodes();
}


void CID7011_RejectedForQualityReason::cleanup()
{
    /* delete code list, it will be recreated automatically when needed */
    delete Codes;
    Codes = NULL;
}


DSRCodedEntryValue CID7011_RejectedForQualityReason::getCodedEntry(const EnumType value,
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


CID7011_RejectedForQualityReason::CodeList &CID7011_RejectedForQualityReason::getCodes()
{
    /* check whether code list has already been created and initialized */
    if (Codes == NULL)
    {
        /* create a new code list (should never fail) */
        Codes = new CodeList();
        /* and initialize it by adding the coded entries */
        Codes->insert(OFMake_pair(ImageArtifacts, DSRBasicCodedEntry("111207", "DCM", "Image artifact(s)")));
        Codes->insert(OFMake_pair(GridArtifacts, DSRBasicCodedEntry("111208", "DCM", "Grid artifact(s)")));
        Codes->insert(OFMake_pair(Positioning, DSRBasicCodedEntry("111209", "DCM", "Positioning")));
        Codes->insert(OFMake_pair(MotionBlur, DSRBasicCodedEntry("111210", "DCM", "Motion blur")));
        Codes->insert(OFMake_pair(UnderExposed, DSRBasicCodedEntry("111211", "DCM", "Under exposed")));
        Codes->insert(OFMake_pair(OverExposed, DSRBasicCodedEntry("111212", "DCM", "Over exposed")));
        Codes->insert(OFMake_pair(NoImage, DSRBasicCodedEntry("111213", "DCM", "No image")));
        Codes->insert(OFMake_pair(DetectorArtifacts, DSRBasicCodedEntry("111214", "DCM", "Detector artifact(s)")));
        Codes->insert(OFMake_pair(ArtifactsOtherThanGridOrDetectorArtifact, DSRBasicCodedEntry("111215", "DCM", "Artifact(s) other than grid or detector artifact")));
        Codes->insert(OFMake_pair(MechanicalFailure, DSRBasicCodedEntry("111216", "DCM", "Mechanical failure")));
        Codes->insert(OFMake_pair(ElectricalFailure, DSRBasicCodedEntry("111217", "DCM", "Electrical failure")));
        Codes->insert(OFMake_pair(SoftwareFailure, DSRBasicCodedEntry("111218", "DCM", "Software failure")));
        Codes->insert(OFMake_pair(InappropriateImageProcessing, DSRBasicCodedEntry("111219", "DCM", "Inappropriate image processing")));
        Codes->insert(OFMake_pair(OtherFailure, DSRBasicCodedEntry("111220", "DCM", "Other failure")));
        Codes->insert(OFMake_pair(UnknownFailure, DSRBasicCodedEntry("111221", "DCM", "Unknown failure")));
        Codes->insert(OFMake_pair(DoubleExposure, DSRBasicCodedEntry("113026", "DCM", "Double exposure")));
    }
    /* should never be NULL */
    return *Codes;
}


OFCondition CID7011_RejectedForQualityReason::setEnhancedEncodingMode(DSRCodedEntryValue &codedEntryValue)
{
    return codedEntryValue.setEnhancedEncodingMode(CONTEXT_GROUP_NUMBER, "DCMR", CONTEXT_GROUP_VERSION, CONTEXT_GROUP_UID);
}
