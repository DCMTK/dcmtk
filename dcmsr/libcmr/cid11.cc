/*
 *
 *  Copyright (C) 2015-2019, J. Riesmeier, Oldenburg, Germany
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  Source file for class CID11_RouteOfAdministration
 *
 *  Generated automatically from DICOM PS 3.16-2019b
 *  File created on 2019-04-26 17:15:09 by J. Riesmeier
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
        Codes->insert(OFMake_pair(IntravenousRoute, DSRBasicCodedEntry("47625008", "SCT", "Intravenous route")));
        Codes->insert(OFMake_pair(IntraArterialRoute, DSRBasicCodedEntry("58100008", "SCT", "Intra-arterial route")));
        Codes->insert(OFMake_pair(IntramuscularRoute, DSRBasicCodedEntry("78421000", "SCT", "Intramuscular route")));
        Codes->insert(OFMake_pair(SubcutaneousRoute, DSRBasicCodedEntry("34206005", "SCT", "Subcutaneous route")));
        Codes->insert(OFMake_pair(IntracutaneousRoute, DSRBasicCodedEntry("372464004", "SCT", "Intracutaneous route")));
        Codes->insert(OFMake_pair(IntraperitonealRoute, DSRBasicCodedEntry("38239002", "SCT", "Intraperitoneal route")));
        Codes->insert(OFMake_pair(IntramedullaryRoute, DSRBasicCodedEntry("60213007", "SCT", "Intramedullary route")));
        Codes->insert(OFMake_pair(IntrathecalRoute, DSRBasicCodedEntry("72607000", "SCT", "Intrathecal route")));
        Codes->insert(OFMake_pair(IntraArticularRoute, DSRBasicCodedEntry("12130007", "SCT", "Intra-articular route")));
        Codes->insert(OFMake_pair(IntraepithelialRoute, DSRBasicCodedEntry("C38244", "NCIt", "Intraepithelial route")));
        Codes->insert(OFMake_pair(TopicalRoute, DSRBasicCodedEntry("6064005", "SCT", "Topical route")));
        Codes->insert(OFMake_pair(OralRoute, DSRBasicCodedEntry("26643006", "SCT", "Oral route")));
        Codes->insert(OFMake_pair(TransluminalRoute, DSRBasicCodedEntry("C38306", "NCIt", "Transluminal route")));
        Codes->insert(OFMake_pair(IntraluminalRoute, DSRBasicCodedEntry("37737002", "SCT", "Intraluminal route")));
        Codes->insert(OFMake_pair(ExtraluminalRoute, DSRBasicCodedEntry("C38213", "NCIt", "Extraluminal route")));
        Codes->insert(OFMake_pair(ByInhalation, DSRBasicCodedEntry("446406008", "SCT", "By inhalation")));
        Codes->insert(OFMake_pair(PerRectum, DSRBasicCodedEntry("37161004", "SCT", "Per rectum")));
        Codes->insert(OFMake_pair(VaginalRoute, DSRBasicCodedEntry("16857009", "SCT", "Vaginal route")));
        Codes->insert(OFMake_pair(IntracoronaryRoute, DSRBasicCodedEntry("372463005", "SCT", "Intracoronary route")));
        Codes->insert(OFMake_pair(IntracardiacRoute, DSRBasicCodedEntry("372460008", "SCT", "Intracardiac route")));
        Codes->insert(OFMake_pair(IntraventricularRouteCardiac, DSRBasicCodedEntry("420287000", "SCT", "Intraventricular route - cardiac")));
        Codes->insert(OFMake_pair(RetroOrbitalRoute, DSRBasicCodedEntry("127070", "DCM", "Retro-orbital route")));
        Codes->insert(OFMake_pair(NasalRoute, DSRBasicCodedEntry("46713006", "SCT", "Nasal route")));
        Codes->insert(OFMake_pair(IntradermalRoute, DSRBasicCodedEntry("372464004", "SCT", "Intradermal route")));
        Codes->insert(OFMake_pair(IntratumorRoute, DSRBasicCodedEntry("447122006", "SCT", "Intratumor route")));
    }
    /* should never be NULL */
    return *Codes;
}


OFCondition CID11_RouteOfAdministration::setEnhancedEncodingMode(DSRCodedEntryValue &codedEntryValue)
{
    return codedEntryValue.setEnhancedEncodingMode(CONTEXT_GROUP_NUMBER, "DCMR", CONTEXT_GROUP_VERSION, CONTEXT_GROUP_UID);
}
