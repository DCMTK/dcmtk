/*
 *
 *  Copyright (C) 2015-2019, J. Riesmeier, Oldenburg, Germany
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  Source file for class CID11_RouteOfAdministration
 *
 *  Generated automatically from DICOM PS 3.16-2019a
 *  File created on 2019-02-04 12:56:13 by J. Riesmeier
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/cmr/cid11.h"


// general information on CID 11 (Route of Administration)
#define CONTEXT_GROUP_NUMBER  "11"
#define CONTEXT_GROUP_VERSION "20160314"
#define CONTEXT_GROUP_UID     "1.2.840.10008.6.1.9"
#define CONTEXT_GROUP_TYPE    OFTrue  /* extensible */

// initialize global/static variable
CID11_RouteOfAdministration::CodeList *CID11_RouteOfAdministration::Codes = NULL;


CID11_RouteOfAdministration::CID11_RouteOfAdministration(const DSRCodedEntryValue &selectedValue)
  : DSRContextGroup(CONTEXT_GROUP_NUMBER, "DCMR", CONTEXT_GROUP_VERSION, CONTEXT_GROUP_UID, selectedValue)
{
    setExtensible(CONTEXT_GROUP_TYPE);
}


CID11_RouteOfAdministration::CID11_RouteOfAdministration(const EnumType selectedValue,
                                                         const OFBool enhancedEncodingMode)
  : DSRContextGroup(CONTEXT_GROUP_NUMBER, "DCMR", CONTEXT_GROUP_VERSION, CONTEXT_GROUP_UID, getCodedEntry(selectedValue, enhancedEncodingMode))
{
    setExtensible(CONTEXT_GROUP_TYPE);
}


OFCondition CID11_RouteOfAdministration::selectValue(const EnumType selectedValue,
                                                     const OFBool enhancedEncodingMode)
{
    /* never check the coded entry */
    return DSRContextGroup::selectValue(getCodedEntry(selectedValue, enhancedEncodingMode), OFFalse /*check*/, OFFalse /*definedContextGroup*/);
}


OFCondition CID11_RouteOfAdministration::findCodedEntry(const DSRCodedEntryValue &searchForCodedEntry,
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


void CID11_RouteOfAdministration::printCodes(STD_NAMESPACE ostream &stream) const
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

void CID11_RouteOfAdministration::initialize()
{
    /* create and initialize code list */
    getCodes();
}


void CID11_RouteOfAdministration::cleanup()
{
    /* delete code list, it will be recreated automatically when needed */
    delete Codes;
    Codes = NULL;
}


DSRCodedEntryValue CID11_RouteOfAdministration::getCodedEntry(const EnumType value,
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


CID11_RouteOfAdministration::CodeList &CID11_RouteOfAdministration::getCodes()
{
    /* check whether code list has already been created and initialized */
    if (Codes == NULL)
    {
        /* create a new code list (should never fail) */
        Codes = new CodeList();
        /* and initialize it by adding the coded entries */
        Codes->insert(OFMake_pair(IntravenousRoute, DSRBasicCodedEntry("G-D101", "SRT", "Intravenous route")));
        Codes->insert(OFMake_pair(IntraArterialRoute, DSRBasicCodedEntry("G-D102", "SRT", "Intra-arterial route")));
        Codes->insert(OFMake_pair(IntramuscularRoute, DSRBasicCodedEntry("G-D103", "SRT", "Intramuscular route")));
        Codes->insert(OFMake_pair(SubcutaneousRoute, DSRBasicCodedEntry("G-D104", "SRT", "Subcutaneous route")));
        Codes->insert(OFMake_pair(IntracutaneousRoute, DSRBasicCodedEntry("G-D17D", "SRT", "Intracutaneous route")));
        Codes->insert(OFMake_pair(IntraperitonealRoute, DSRBasicCodedEntry("G-D106", "SRT", "Intraperitoneal route")));
        Codes->insert(OFMake_pair(IntramedullaryRoute, DSRBasicCodedEntry("G-D107", "SRT", "Intramedullary route")));
        Codes->insert(OFMake_pair(IntrathecalRoute, DSRBasicCodedEntry("G-D108", "SRT", "Intrathecal route")));
        Codes->insert(OFMake_pair(IntraArticularRoute, DSRBasicCodedEntry("G-D109", "SRT", "Intra-articular route")));
        Codes->insert(OFMake_pair(IntraepithelialRoute, DSRBasicCodedEntry("C38244", "NCIt", "Intraepithelial route")));
        Codes->insert(OFMake_pair(TopicalRoute, DSRBasicCodedEntry("G-D112", "SRT", "Topical route")));
        Codes->insert(OFMake_pair(OralRoute, DSRBasicCodedEntry("G-D140", "SRT", "Oral route")));
        Codes->insert(OFMake_pair(TransluminalRoute, DSRBasicCodedEntry("C38306", "NCIt", "Transluminal route")));
        Codes->insert(OFMake_pair(IntraluminalRoute, DSRBasicCodedEntry("G-D144", "SRT", "Intraluminal route")));
        Codes->insert(OFMake_pair(ExtraluminalRoute, DSRBasicCodedEntry("C38213", "NCIt", "Extraluminal route")));
        Codes->insert(OFMake_pair(ByInhalation, DSRBasicCodedEntry("R-40B32", "SRT", "By inhalation")));
        Codes->insert(OFMake_pair(PerRectum, DSRBasicCodedEntry("G-D160", "SRT", "Per rectum")));
        Codes->insert(OFMake_pair(VaginalRoute, DSRBasicCodedEntry("G-D164", "SRT", "Vaginal route")));
        Codes->insert(OFMake_pair(IntracoronaryRoute, DSRBasicCodedEntry("G-D17C", "SRT", "Intracoronary route")));
        Codes->insert(OFMake_pair(IntracardiacRoute, DSRBasicCodedEntry("G-D173", "SRT", "Intracardiac route")));
        Codes->insert(OFMake_pair(IntraventricularRouteCardiac, DSRBasicCodedEntry("R-F2C86", "SRT", "Intraventricular route - cardiac")));
        Codes->insert(OFMake_pair(RetroOrbitalRoute, DSRBasicCodedEntry("127070", "DCM", "Retro-orbital route")));
        Codes->insert(OFMake_pair(NasalRoute, DSRBasicCodedEntry("G-D172", "SRT", "Nasal route")));
        Codes->insert(OFMake_pair(IntradermalRoute, DSRBasicCodedEntry("G-D17D", "SRT", "Intradermal route")));
        Codes->insert(OFMake_pair(IntratumorRoute, DSRBasicCodedEntry("R-F2CD4", "SRT", "Intratumor route")));
    }
    /* should never be NULL */
    return *Codes;
}


OFCondition CID11_RouteOfAdministration::setEnhancedEncodingMode(DSRCodedEntryValue &codedEntryValue)
{
    return codedEntryValue.setEnhancedEncodingMode(CONTEXT_GROUP_NUMBER, "DCMR", CONTEXT_GROUP_VERSION, CONTEXT_GROUP_UID);
}
