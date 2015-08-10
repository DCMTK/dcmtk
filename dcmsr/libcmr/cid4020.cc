/*
 *
 *  Copyright (C) 2015, OFFIS e.V.
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
 *    classes: CID4020_PETRadionuclide
 *
 *    Generated automatically from DICOM PS 3.16-2015c
 *    File created on 2015-08-10 16:46:55 by J. Riesmeier
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/cmr/cid4020.h"


// general information on CID 4020 (PET Radionuclide)
#define CONTEXT_GROUP_NUMBER  "4020"
#define CONTEXT_GROUP_VERSION "20141110"
#define CONTEXT_GROUP_UID     "1.2.840.10008.6.1.304"
#define CONTEXT_GROUP_TYPE    OFTrue  /* extensible */


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


OFCondition CID4020_PETRadionuclide::findCodedEntry(const DSRCodedEntryValue &codedEntryValue) const
{
    OFCondition result = SR_EC_CodedEntryNotInContextGroup;
    /* first, search for standard codes */
    OFMap<EnumType, DSRBasicCodedEntry>::const_iterator iter = getCodes().begin();
    OFMap<EnumType, DSRBasicCodedEntry>::const_iterator last = getCodes().end();
    /* iterate over coded entry list */
    while (iter != last)
    {
        /* if found, exit loop */
        if (codedEntryValue == iter->second)
        {
            result = SR_EC_CodedEntryInStandardContextGroup;
            break;
        }
        ++iter;
    }
    /* if not, continue with extended codes */
    if (result.bad())
        result = DSRContextGroup::findCodedEntry(codedEntryValue);
    return result;
}


void CID4020_PETRadionuclide::printCodes(STD_NAMESPACE ostream &stream) const
{
    /* print standard codes */
    stream << "Standard codes:" << OFendl;
    OFMap<EnumType, DSRBasicCodedEntry>::const_iterator iter = getCodes().begin();
    OFMap<EnumType, DSRBasicCodedEntry>::const_iterator last = getCodes().end();
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
            codedEntry.setEnhancedEncodingMode(CONTEXT_GROUP_NUMBER, "DCMR", CONTEXT_GROUP_VERSION, CONTEXT_GROUP_UID);
    }
    return codedEntry;
}


CID4020_PETRadionuclide::CodeList &CID4020_PETRadionuclide::getCodes()
{
    /* use a static variable for singleton pattern */
    static CodeList *codes = NULL;
    /* check whether code list has already been created and initialized */
    if (codes == NULL)
    {
        /* create a new code list (should never fail) */
        codes = new CodeList();
        /* and initialize it by adding the coded entries */
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(_11_Carbon, DSRBasicCodedEntry("C-105A1", "SRT", "^11^Carbon")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(_13_Nitrogen, DSRBasicCodedEntry("C-107A1", "SRT", "^13^Nitrogen")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(_14_Oxygen, DSRBasicCodedEntry("C-1018C", "SRT", "^14^Oxygen")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(_15_Oxygen, DSRBasicCodedEntry("C-B1038", "SRT", "^15^Oxygen")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(_18_Fluorine, DSRBasicCodedEntry("C-111A1", "SRT", "^18^Fluorine")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(_22_Sodium, DSRBasicCodedEntry("C-155A1", "SRT", "^22^Sodium")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(_38_Potassium, DSRBasicCodedEntry("C-135A4", "SRT", "^38^Potassium")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(_43_Scandium, DSRBasicCodedEntry("126605", "DCM", "^43^Scandium")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(_44_Scandium, DSRBasicCodedEntry("126600", "DCM", "^44^Scandium")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(_45_Titanium, DSRBasicCodedEntry("C-166A2", "SRT", "^45^Titanium")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(_51_Manganese, DSRBasicCodedEntry("126601", "DCM", "^51^Manganese")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(_52_Iron, DSRBasicCodedEntry("C-130A1", "SRT", "^52^Iron")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(_52_Manganese, DSRBasicCodedEntry("C-149A1", "SRT", "^52^Manganese")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(_60_Copper, DSRBasicCodedEntry("C-127A4", "SRT", "^60^Copper")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(_61_Copper, DSRBasicCodedEntry("C-127A1", "SRT", "^61^Copper")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(_62_Copper, DSRBasicCodedEntry("C-127A5", "SRT", "^62^Copper")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(_62_Zinc, DSRBasicCodedEntry("C-141A1", "SRT", "^62^Zinc")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(_64_Copper, DSRBasicCodedEntry("C-127A2", "SRT", "^64^Copper")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(_66_Gallium, DSRBasicCodedEntry("C-131A1", "SRT", "^66^Gallium")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(_68_Gallium, DSRBasicCodedEntry("C-131A3", "SRT", "^68^Gallium")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(_68_Germanium, DSRBasicCodedEntry("C-128A2", "SRT", "^68^Germanium")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(_70_Arsenic, DSRBasicCodedEntry("126602", "DCM", "^70^Arsenic")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(_72_Arsenic, DSRBasicCodedEntry("C-115A2", "SRT", "^72^Arsenic")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(_73_Selenium, DSRBasicCodedEntry("C-116A2", "SRT", "^73^Selenium")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(_75_Bromine, DSRBasicCodedEntry("C-113A1", "SRT", "^75^Bromine")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(_76_Bromine, DSRBasicCodedEntry("C-113A2", "SRT", "^76^Bromine")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(_77_Bromine, DSRBasicCodedEntry("C-113A3", "SRT", "^77^Bromine")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(_82_Rubidium, DSRBasicCodedEntry("C-159A2", "SRT", "^82^Rubidium")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(_86_Yttrium, DSRBasicCodedEntry("C-162A3", "SRT", "^86^Yttrium")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(_89_Zirconium, DSRBasicCodedEntry("C-168A4", "SRT", "^89^Zirconium")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(_90_Niobium, DSRBasicCodedEntry("126603", "DCM", "^90^Niobium")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(_90_Yttrium, DSRBasicCodedEntry("C-162A7", "SRT", "^90^Yttrium")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(_94m_Technetium, DSRBasicCodedEntry("C-163AA", "SRT", "^94m^Technetium")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(_124_Iodine, DSRBasicCodedEntry("C-114A5", "SRT", "^124^Iodine")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(_152_Terbium, DSRBasicCodedEntry("126606", "DCM", "^152^Terbium")));
    }
    /* should never be NULL */
    return *codes;
}
