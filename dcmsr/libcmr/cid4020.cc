/*
 *
 *  Copyright (C) 2015-2019, J. Riesmeier, Oldenburg, Germany
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  Source file for class CID4020_PETRadionuclide
 *
 *  Generated automatically from DICOM PS 3.16-2019b
 *  File created on 2019-04-26 17:15:18 by J. Riesmeier
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/cmr/cid4020.h"


// general information on CID 4020 (PET Radionuclide)
#define CONTEXT_GROUP_NUMBER  "4020"
#define CONTEXT_GROUP_VERSION "20160119"
#define CONTEXT_GROUP_UID     "1.2.840.10008.6.1.304"
#define CONTEXT_GROUP_TYPE    OFTrue  /* extensible */

// initialize global/static variable
CID4020_PETRadionuclide::CodeList *CID4020_PETRadionuclide::Codes = NULL;


CID4020_PETRadionuclide::CID4020_PETRadionuclide(const DSRCodedEntryValue &selectedValue)
  : DSRContextGroup(CONTEXT_GROUP_NUMBER, "DCMR", CONTEXT_GROUP_VERSION, CONTEXT_GROUP_UID, selectedValue)
{
    setExtensible(CONTEXT_GROUP_TYPE);
}


CID4020_PETRadionuclide::CID4020_PETRadionuclide(const EnumType selectedValue,
                                                 const OFBool enhancedEncodingMode)
  : DSRContextGroup(CONTEXT_GROUP_NUMBER, "DCMR", CONTEXT_GROUP_VERSION, CONTEXT_GROUP_UID, getCodedEntry(selectedValue, enhancedEncodingMode))
{
    setExtensible(CONTEXT_GROUP_TYPE);
}


OFCondition CID4020_PETRadionuclide::selectValue(const EnumType selectedValue,
                                                 const OFBool enhancedEncodingMode)
{
    /* never check the coded entry */
    return DSRContextGroup::selectValue(getCodedEntry(selectedValue, enhancedEncodingMode), OFFalse /*check*/, OFFalse /*definedContextGroup*/);
}


OFCondition CID4020_PETRadionuclide::findCodedEntry(const DSRCodedEntryValue &searchForCodedEntry,
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


void CID4020_PETRadionuclide::printCodes(STD_NAMESPACE ostream &stream) const
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

void CID4020_PETRadionuclide::initialize()
{
    /* create and initialize code list */
    getCodes();
}


void CID4020_PETRadionuclide::cleanup()
{
    /* delete code list, it will be recreated automatically when needed */
    delete Codes;
    Codes = NULL;
}


DSRCodedEntryValue CID4020_PETRadionuclide::getCodedEntry(const EnumType value,
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


CID4020_PETRadionuclide::CodeList &CID4020_PETRadionuclide::getCodes()
{
    /* check whether code list has already been created and initialized */
    if (Codes == NULL)
    {
        /* create a new code list (should never fail) */
        Codes = new CodeList();
        /* and initialize it by adding the coded entries */
        Codes->insert(OFMake_pair(_11_Carbon, DSRBasicCodedEntry("40565003", "SCT", "^11^Carbon")));
        Codes->insert(OFMake_pair(_13_Nitrogen, DSRBasicCodedEntry("21576001", "SCT", "^13^Nitrogen")));
        Codes->insert(OFMake_pair(_14_Oxygen, DSRBasicCodedEntry("424875009", "SCT", "^14^Oxygen")));
        Codes->insert(OFMake_pair(_15_Oxygen, DSRBasicCodedEntry("129504001", "SCT", "^15^Oxygen")));
        Codes->insert(OFMake_pair(_18_Fluorine, DSRBasicCodedEntry("77004003", "SCT", "^18^Fluorine")));
        Codes->insert(OFMake_pair(_22_Sodium, DSRBasicCodedEntry("71633006", "SCT", "^22^Sodium")));
        Codes->insert(OFMake_pair(_38_Potassium, DSRBasicCodedEntry("423764008", "SCT", "^38^Potassium")));
        Codes->insert(OFMake_pair(_43_Scandium, DSRBasicCodedEntry("126605", "DCM", "^43^Scandium")));
        Codes->insert(OFMake_pair(_44_Scandium, DSRBasicCodedEntry("126600", "DCM", "^44^Scandium")));
        Codes->insert(OFMake_pair(_45_Titanium, DSRBasicCodedEntry("75696008", "SCT", "^45^Titanium")));
        Codes->insert(OFMake_pair(_51_Manganese, DSRBasicCodedEntry("126601", "DCM", "^51^Manganese")));
        Codes->insert(OFMake_pair(_52_Iron, DSRBasicCodedEntry("69089000", "SCT", "^52^Iron")));
        Codes->insert(OFMake_pair(_52_Manganese, DSRBasicCodedEntry("37225000", "SCT", "^52^Manganese")));
        Codes->insert(OFMake_pair(_52m_Manganese, DSRBasicCodedEntry("126607", "DCM", "^52m^Manganese")));
        Codes->insert(OFMake_pair(_60_Copper, DSRBasicCodedEntry("425364008", "SCT", "^60^Copper")));
        Codes->insert(OFMake_pair(_61_Copper, DSRBasicCodedEntry("71425003", "SCT", "^61^Copper")));
        Codes->insert(OFMake_pair(_62_Copper, DSRBasicCodedEntry("422934004", "SCT", "^62^Copper")));
        Codes->insert(OFMake_pair(_62_Zinc, DSRBasicCodedEntry("65054007", "SCT", "^62^Zinc")));
        Codes->insert(OFMake_pair(_64_Copper, DSRBasicCodedEntry("3932008", "SCT", "^64^Copper")));
        Codes->insert(OFMake_pair(_66_Gallium, DSRBasicCodedEntry("79477007", "SCT", "^66^Gallium")));
        Codes->insert(OFMake_pair(_68_Gallium, DSRBasicCodedEntry("35337001", "SCT", "^68^Gallium")));
        Codes->insert(OFMake_pair(_68_Germanium, DSRBasicCodedEntry("53315004", "SCT", "^68^Germanium")));
        Codes->insert(OFMake_pair(_70_Arsenic, DSRBasicCodedEntry("126602", "DCM", "^70^Arsenic")));
        Codes->insert(OFMake_pair(_72_Arsenic, DSRBasicCodedEntry("2705002", "SCT", "^72^Arsenic")));
        Codes->insert(OFMake_pair(_73_Selenium, DSRBasicCodedEntry("87437000", "SCT", "^73^Selenium")));
        Codes->insert(OFMake_pair(_75_Bromine, DSRBasicCodedEntry("17910003", "SCT", "^75^Bromine")));
        Codes->insert(OFMake_pair(_76_Bromine, DSRBasicCodedEntry("79523006", "SCT", "^76^Bromine")));
        Codes->insert(OFMake_pair(_77_Bromine, DSRBasicCodedEntry("86521004", "SCT", "^77^Bromine")));
        Codes->insert(OFMake_pair(_82_Rubidium, DSRBasicCodedEntry("79197006", "SCT", "^82^Rubidium")));
        Codes->insert(OFMake_pair(_86_Yttrium, DSRBasicCodedEntry("10738001", "SCT", "^86^Yttrium")));
        Codes->insert(OFMake_pair(_89_Zirconium, DSRBasicCodedEntry("63360001", "SCT", "^89^Zirconium")));
        Codes->insert(OFMake_pair(_90_Niobium, DSRBasicCodedEntry("126603", "DCM", "^90^Niobium")));
        Codes->insert(OFMake_pair(_90_Yttrium, DSRBasicCodedEntry("14691008", "SCT", "^90^Yttrium")));
        Codes->insert(OFMake_pair(_94m_Technetium, DSRBasicCodedEntry("424079002", "SCT", "^94m^Technetium")));
        Codes->insert(OFMake_pair(_124_Iodine, DSRBasicCodedEntry("40937006", "SCT", "^124^Iodine")));
        Codes->insert(OFMake_pair(_152_Terbium, DSRBasicCodedEntry("126606", "DCM", "^152^Terbium")));
    }
    /* should never be NULL */
    return *Codes;
}


OFCondition CID4020_PETRadionuclide::setEnhancedEncodingMode(DSRCodedEntryValue &codedEntryValue)
{
    return codedEntryValue.setEnhancedEncodingMode(CONTEXT_GROUP_NUMBER, "DCMR", CONTEXT_GROUP_VERSION, CONTEXT_GROUP_UID);
}
