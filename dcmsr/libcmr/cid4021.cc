/*
 *
 *  Copyright (C) 2015-2019, J. Riesmeier, Oldenburg, Germany
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  Source file for class CID4021_PETRadiopharmaceutical
 *
 *  Generated automatically from DICOM PS 3.16-2019a
 *  File created on 2019-02-04 12:56:24 by J. Riesmeier
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/cmr/cid4021.h"


// general information on CID 4021 (PET Radiopharmaceutical)
#define CONTEXT_GROUP_NUMBER  "4021"
#define CONTEXT_GROUP_VERSION "20190124"
#define CONTEXT_GROUP_UID     "1.2.840.10008.6.1.305"
#define CONTEXT_GROUP_TYPE    OFTrue  /* extensible */

// initialize global/static variable
CID4021_PETRadiopharmaceutical::CodeList *CID4021_PETRadiopharmaceutical::Codes = NULL;


CID4021_PETRadiopharmaceutical::CID4021_PETRadiopharmaceutical(const DSRCodedEntryValue &selectedValue)
  : DSRContextGroup(CONTEXT_GROUP_NUMBER, "DCMR", CONTEXT_GROUP_VERSION, CONTEXT_GROUP_UID, selectedValue)
{
    setExtensible(CONTEXT_GROUP_TYPE);
}


CID4021_PETRadiopharmaceutical::CID4021_PETRadiopharmaceutical(const EnumType selectedValue,
                                                               const OFBool enhancedEncodingMode)
  : DSRContextGroup(CONTEXT_GROUP_NUMBER, "DCMR", CONTEXT_GROUP_VERSION, CONTEXT_GROUP_UID, getCodedEntry(selectedValue, enhancedEncodingMode))
{
    setExtensible(CONTEXT_GROUP_TYPE);
}


OFCondition CID4021_PETRadiopharmaceutical::selectValue(const EnumType selectedValue,
                                                        const OFBool enhancedEncodingMode)
{
    /* never check the coded entry */
    return DSRContextGroup::selectValue(getCodedEntry(selectedValue, enhancedEncodingMode), OFFalse /*check*/, OFFalse /*definedContextGroup*/);
}


OFCondition CID4021_PETRadiopharmaceutical::findCodedEntry(const DSRCodedEntryValue &searchForCodedEntry,
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


void CID4021_PETRadiopharmaceutical::printCodes(STD_NAMESPACE ostream &stream) const
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

void CID4021_PETRadiopharmaceutical::initialize()
{
    /* create and initialize code list */
    getCodes();
}


void CID4021_PETRadiopharmaceutical::cleanup()
{
    /* delete code list, it will be recreated automatically when needed */
    delete Codes;
    Codes = NULL;
}


DSRCodedEntryValue CID4021_PETRadiopharmaceutical::getCodedEntry(const EnumType value,
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


CID4021_PETRadiopharmaceutical::CodeList &CID4021_PETRadiopharmaceutical::getCodes()
{
    /* check whether code list has already been created and initialized */
    if (Codes == NULL)
    {
        /* create a new code list (should never fail) */
        Codes = new CodeList();
        /* and initialize it by adding the coded entries */
        Codes->insert(OFMake_pair(_28H1_89Zr, DSRBasicCodedEntry("126752", "DCM", "28H1 ^89^Zr")));
        Codes->insert(OFMake_pair(_2FA_F18, DSRBasicCodedEntry("126713", "DCM", "2FA F^18^")));
        Codes->insert(OFMake_pair(_7D12_89Zr, DSRBasicCodedEntry("126751", "DCM", "7D12 ^89^Zr")));
        Codes->insert(OFMake_pair(_7E11_89Zr, DSRBasicCodedEntry("126750", "DCM", "7E11 ^89^Zr")));
        Codes->insert(OFMake_pair(Acetate_C11, DSRBasicCodedEntry("C-B1043", "SRT", "Acetate C^11^")));
        Codes->insert(OFMake_pair(AGN150998_89Zr, DSRBasicCodedEntry("126729", "DCM", "AGN-150998 ^89^Zr")));
        Codes->insert(OFMake_pair(Ammonia_N13, DSRBasicCodedEntry("C-B103C", "SRT", "Ammonia N^13^")));
        Codes->insert(OFMake_pair(AntiB220_89Zr, DSRBasicCodedEntry("126754", "DCM", "Anti-B220 ^89^Zr")));
        Codes->insert(OFMake_pair(ATSM_Cu60, DSRBasicCodedEntry("126700", "DCM", "ATSM Cu^60^")));
        Codes->insert(OFMake_pair(ATSM_Cu61, DSRBasicCodedEntry("126701", "DCM", "ATSM Cu^61^")));
        Codes->insert(OFMake_pair(ATSM_Cu62, DSRBasicCodedEntry("126702", "DCM", "ATSM Cu^62^")));
        Codes->insert(OFMake_pair(ATSM_Cu64, DSRBasicCodedEntry("C-B07DB", "SRT", "ATSM Cu^64^")));
        Codes->insert(OFMake_pair(Benralizumab_89Zr, DSRBasicCodedEntry("126722", "DCM", "Benralizumab ^89^Zr")));
        Codes->insert(OFMake_pair(Bevacizumab_89Zr, DSRBasicCodedEntry("126516", "DCM", "Bevacizumab ^89^Zr")));
        Codes->insert(OFMake_pair(Blinatumomab_89Zr, DSRBasicCodedEntry("126727", "DCM", "Blinatumomab ^89^Zr")));
        Codes->insert(OFMake_pair(Brentuximab_89Zr, DSRBasicCodedEntry("126735", "DCM", "Brentuximab ^89^Zr")));
        Codes->insert(OFMake_pair(Butanol_O15, DSRBasicCodedEntry("C-B07DC", "SRT", "Butanol O^15^")));
        Codes->insert(OFMake_pair(CarbonDioxide_O15, DSRBasicCodedEntry("C-B103B", "SRT", "Carbon dioxide O^15^")));
        Codes->insert(OFMake_pair(CarbonMonoxide_C11, DSRBasicCodedEntry("C-B1045", "SRT", "Carbon monoxide C^11^")));
        Codes->insert(OFMake_pair(CarbonMonoxide_O15, DSRBasicCodedEntry("C-B103A", "SRT", "Carbon monoxide O^15^")));
        Codes->insert(OFMake_pair(Carfentanil_C11, DSRBasicCodedEntry("C-B103F", "SRT", "Carfentanil C^11^")));
        Codes->insert(OFMake_pair(Cetuximab_89Zr, DSRBasicCodedEntry("126513", "DCM", "Cetuximab ^89^Zr")));
        Codes->insert(OFMake_pair(CG250Fab2_89Zr, DSRBasicCodedEntry("126517", "DCM", "cG250-F(ab')(2) ^89^Zr")));
        Codes->insert(OFMake_pair(Choline_C11, DSRBasicCodedEntry("126703", "DCM", "Choline C^11^")));
        Codes->insert(OFMake_pair(CLR1404_I124, DSRBasicCodedEntry("126715", "DCM", "CLR1404 I^124^")));
        Codes->insert(OFMake_pair(CLR1404_I131, DSRBasicCodedEntry("126716", "DCM", "CLR1404 I^131^")));
        Codes->insert(OFMake_pair(CMAbU36_89Zr, DSRBasicCodedEntry("126746", "DCM", "cMAb U36 ^89^Zr")));
        Codes->insert(OFMake_pair(CU36_89Zr, DSRBasicCodedEntry("126515", "DCM", "cU36 ^89^Zr")));
        Codes->insert(OFMake_pair(DCFBC_F18, DSRBasicCodedEntry("C96234", "NCIt", "DCFBC F^18^")));
        Codes->insert(OFMake_pair(DCFPyL_F18, DSRBasicCodedEntry("C116352", "NCIt", "DCFPyL F^18^")));
        Codes->insert(OFMake_pair(DfFK2_89Zr, DSRBasicCodedEntry("126762", "DCM", "Df-[FK](2) ^89^Zr")));
        Codes->insert(OFMake_pair(DfFK23PEG4_89Zr, DSRBasicCodedEntry("126763", "DCM", "Df-[FK](2)-3PEG(4) ^89^Zr")));
        Codes->insert(OFMake_pair(DfCD45_89Zr, DSRBasicCodedEntry("126520", "DCM", "Df-CD45 ^89^Zr")));
        Codes->insert(OFMake_pair(DfFK_89Zr, DSRBasicCodedEntry("126760", "DCM", "Df-FK ^89^Zr")));
        Codes->insert(OFMake_pair(DfFKPEG3_89Zr, DSRBasicCodedEntry("126761", "DCM", "Df-FK-PEG(3) ^89^Zr")));
        Codes->insert(OFMake_pair(DN30_89Zr, DSRBasicCodedEntry("126747", "DCM", "DN30 ^89^Zr")));
        Codes->insert(OFMake_pair(E4G10_89Zr, DSRBasicCodedEntry("126519", "DCM", "E4G10 ^89^Zr")));
        Codes->insert(OFMake_pair(Ecromeximab_89Zr, DSRBasicCodedEntry("126732", "DCM", "Ecromeximab ^89^Zr")));
        Codes->insert(OFMake_pair(Edotreotide_Ga68, DSRBasicCodedEntry("C2713594", "UMLS", "Edotreotide Ga^68^")));
        Codes->insert(OFMake_pair(EDTA_Ga68, DSRBasicCodedEntry("C-B07DD", "SRT", "EDTA Ga^68^")));
        Codes->insert(OFMake_pair(Fallypride_C11, DSRBasicCodedEntry("126704", "DCM", "Fallypride C^11^")));
        Codes->insert(OFMake_pair(Fallypride_F18, DSRBasicCodedEntry("126705", "DCM", "Fallypride F^18^")));
        Codes->insert(OFMake_pair(FLB457_C11, DSRBasicCodedEntry("126706", "DCM", "FLB 457 C^11^")));
        Codes->insert(OFMake_pair(Florbetaben_F18, DSRBasicCodedEntry("C-D6858", "SRT", "Florbetaben F^18^")));
        Codes->insert(OFMake_pair(Florbetapir_F18, DSRBasicCodedEntry("C-E0269", "SRT", "Florbetapir F^18^")));
        Codes->insert(OFMake_pair(Flortaucipir_F18, DSRBasicCodedEntry("C4547429", "UMLS", "Flortaucipir F^18^")));
        Codes->insert(OFMake_pair(Flubatine_F18, DSRBasicCodedEntry("126503", "DCM", "Flubatine F^18^")));
        Codes->insert(OFMake_pair(Fluciclatide_F18, DSRBasicCodedEntry("C-E0265", "SRT", "Fluciclatide F^18^")));
        Codes->insert(OFMake_pair(Fluciclovine_F18, DSRBasicCodedEntry("C-E026A", "SRT", "Fluciclovine F^18^")));
        Codes->insert(OFMake_pair(Flumazenil_C11, DSRBasicCodedEntry("C-B07DE", "SRT", "Flumazenil C^11^")));
        Codes->insert(OFMake_pair(Flumazenil_F18, DSRBasicCodedEntry("C-B07DF", "SRT", "Flumazenil F^18^")));
        Codes->insert(OFMake_pair(Fluorethyltyrosin_F18, DSRBasicCodedEntry("C-B07E0", "SRT", "Fluorethyltyrosin F^18^")));
        Codes->insert(OFMake_pair(Fluorobenzothiazole_F18, DSRBasicCodedEntry("C-B07E4", "SRT", "Fluorobenzothiazole F^18^")));
        Codes->insert(OFMake_pair(Fluorocholine_F18, DSRBasicCodedEntry("C-E0273", "SRT", "Fluorocholine F^18^")));
        Codes->insert(OFMake_pair(Fluorodeoxyglucose_F18, DSRBasicCodedEntry("C-B1031", "SRT", "Fluorodeoxyglucose F^18^")));
        Codes->insert(OFMake_pair(FluoroestradiolFES_F18, DSRBasicCodedEntry("C1831937", "UMLS", "Fluoroestradiol (FES) F^18^")));
        Codes->insert(OFMake_pair(Fluoroetanidazole_F18, DSRBasicCodedEntry("C1541539", "UMLS", "Fluoroetanidazole F^18^")));
        Codes->insert(OFMake_pair(FluoroLDopa_F18, DSRBasicCodedEntry("C-B1034", "SRT", "Fluoro-L-dopa F^18^")));
        Codes->insert(OFMake_pair(Fluoromethane_F18, DSRBasicCodedEntry("C-B07E2", "SRT", "Fluoromethane F^18^")));
        Codes->insert(OFMake_pair(Fluoromisonidazole_F18, DSRBasicCodedEntry("C-B07E1", "SRT", "Fluoromisonidazole F^18^")));
        Codes->insert(OFMake_pair(FluoropropylDihydrotetrabenazineDTBZ_F18, DSRBasicCodedEntry("C2934038", "UMLS", "Fluoropropyl-dihydrotetrabenazine (DTBZ) F^18^")));
        Codes->insert(OFMake_pair(Fluorotriopride_F18, DSRBasicCodedEntry("126707", "DCM", "Fluorotriopride F^18^")));
        Codes->insert(OFMake_pair(Fluorouracil_F18, DSRBasicCodedEntry("C-B07E3", "SRT", "Fluorouracil F^18^")));
        Codes->insert(OFMake_pair(Flurpiridaz_F18, DSRBasicCodedEntry("126718", "DCM", "Flurpiridaz F^18^")));
        Codes->insert(OFMake_pair(Flutemetamol_F18, DSRBasicCodedEntry("C-E0267", "SRT", "Flutemetamol F^18^")));
        Codes->insert(OFMake_pair(Fresolimumab_89Zr, DSRBasicCodedEntry("126748", "DCM", "Fresolimumab ^89^Zr")));
        Codes->insert(OFMake_pair(GA201_89Zr, DSRBasicCodedEntry("126731", "DCM", "GA201 ^89^Zr")));
        Codes->insert(OFMake_pair(Germanium_Ge68, DSRBasicCodedEntry("C-B1046", "SRT", "Germanium Ge^68^")));
        Codes->insert(OFMake_pair(GlembatumumabVedotin_89Zr, DSRBasicCodedEntry("126724", "DCM", "Glembatumumab vedotin ^89^Zr")));
        Codes->insert(OFMake_pair(Glutamate_N13, DSRBasicCodedEntry("C-B103D", "SRT", "Glutamate N^13^")));
        Codes->insert(OFMake_pair(Glutamine_C11, DSRBasicCodedEntry("126709", "DCM", "Glutamine C^11^")));
        Codes->insert(OFMake_pair(Glutamine_C14, DSRBasicCodedEntry("126710", "DCM", "Glutamine C^14^")));
        Codes->insert(OFMake_pair(Glutamine_F18, DSRBasicCodedEntry("126711", "DCM", "Glutamine F^18^")));
        Codes->insert(OFMake_pair(ISO1_F18, DSRBasicCodedEntry("C2981788", "UMLS", "ISO-1 F^18^")));
        Codes->insert(OFMake_pair(J591_89Zr, DSRBasicCodedEntry("126514", "DCM", "J591 ^89^Zr")));
        Codes->insert(OFMake_pair(Margetuximab_89Zr, DSRBasicCodedEntry("126740", "DCM", "Margetuximab ^89^Zr")));
        Codes->insert(OFMake_pair(MEDI551_89Zr, DSRBasicCodedEntry("126730", "DCM", "MEDI-551 ^89^Zr")));
        Codes->insert(OFMake_pair(Mespiperone_C11, DSRBasicCodedEntry("C-B07E5", "SRT", "Mespiperone C^11^")));
        Codes->insert(OFMake_pair(Methionine_C11, DSRBasicCodedEntry("C-B103E", "SRT", "Methionine C^11^")));
        Codes->insert(OFMake_pair(MK6240_F18, DSRBasicCodedEntry("C4506764", "UMLS", "MK-6240 F^18^")));
        Codes->insert(OFMake_pair(Mogamulizumab_89Zr, DSRBasicCodedEntry("126738", "DCM", "Mogamulizumab ^89^Zr")));
        Codes->insert(OFMake_pair(MonoclonalAntibodymAb_64Cu, DSRBasicCodedEntry("126510", "DCM", "Monoclonal Antibody (mAb) ^64^Cu")));
        Codes->insert(OFMake_pair(MonoclonalAntibodymAb_89Zr, DSRBasicCodedEntry("126511", "DCM", "Monoclonal Antibody (mAb) ^89^Zr")));
        Codes->insert(OFMake_pair(MonoclonalAntibody_I124, DSRBasicCodedEntry("C-B07E6", "SRT", "Monoclonal antibody I^124^")));
        Codes->insert(OFMake_pair(NanocolloidalAlbumin_89Zr, DSRBasicCodedEntry("126753", "DCM", "Nanocolloidal albumin ^89^Zr")));
        Codes->insert(OFMake_pair(Nifene_F18, DSRBasicCodedEntry("126714", "DCM", "Nifene F^18^")));
        Codes->insert(OFMake_pair(Obinituzimab_89Zr, DSRBasicCodedEntry("126721", "DCM", "Obinituzimab ^89^Zr")));
        Codes->insert(OFMake_pair(Ocaratuzumab_89Zr, DSRBasicCodedEntry("126723", "DCM", "Ocaratuzumab ^89^Zr")));
        Codes->insert(OFMake_pair(Oxygen_O15, DSRBasicCodedEntry("C-B1038", "SRT", "Oxygen O^15^")));
        Codes->insert(OFMake_pair(OxygenWater_O15, DSRBasicCodedEntry("C-B1039", "SRT", "Oxygen-water O^15^")));
        Codes->insert(OFMake_pair(Palmitate_C11, DSRBasicCodedEntry("C-B1044", "SRT", "Palmitate C^11^")));
        Codes->insert(OFMake_pair(Panitumumab_89Zr, DSRBasicCodedEntry("126736", "DCM", "Panitumumab ^89^Zr")));
        Codes->insert(OFMake_pair(Pegdinetanib_89Zr, DSRBasicCodedEntry("126728", "DCM", "Pegdinetanib ^89^Zr")));
        Codes->insert(OFMake_pair(PinatuzumabVedotin_89Zr, DSRBasicCodedEntry("126725", "DCM", "Pinatuzumab vedotin ^89^Zr")));
        Codes->insert(OFMake_pair(PittsburghCompoundB_C11, DSRBasicCodedEntry("126500", "DCM", "Pittsburgh compound B C^11^")));
        Codes->insert(OFMake_pair(PolatuzumabVedotin_89Zr, DSRBasicCodedEntry("126726", "DCM", "Polatuzumab vedotin ^89^Zr")));
        Codes->insert(OFMake_pair(PSMA1007_F18, DSRBasicCodedEntry("126758", "DCM", "PSMA-1007 F^18^")));
        Codes->insert(OFMake_pair(PSMA11_Ga68, DSRBasicCodedEntry("C118961", "NCIt", "PSMA-11 Ga^68^")));
        Codes->insert(OFMake_pair(PSMA617_Ga68, DSRBasicCodedEntry("126759", "DCM", "PSMA-617 Ga^68^")));
        Codes->insert(OFMake_pair(PTSM_Cu62, DSRBasicCodedEntry("C-B07E7", "SRT", "PTSM Cu^62^")));
        Codes->insert(OFMake_pair(R1507_89Zr, DSRBasicCodedEntry("126518", "DCM", "R1507 ^89^Zr")));
        Codes->insert(OFMake_pair(Raclopride_C11, DSRBasicCodedEntry("C-B1042", "SRT", "Raclopride C^11^")));
        Codes->insert(OFMake_pair(Ranibizumab_89Zr, DSRBasicCodedEntry("126742", "DCM", "Ranibizumab ^89^Zr")));
        Codes->insert(OFMake_pair(Rituximab_89Zr, DSRBasicCodedEntry("126737", "DCM", "Rituximab ^89^Zr")));
        Codes->insert(OFMake_pair(RO5323441_89Zr, DSRBasicCodedEntry("126755", "DCM", "RO5323441 ^89^Zr")));
        Codes->insert(OFMake_pair(RO542908_89Zr, DSRBasicCodedEntry("126756", "DCM", "RO542908 ^89^Zr")));
        Codes->insert(OFMake_pair(RO6924963_11C, DSRBasicCodedEntry("126719", "DCM", "RO6924963 ^11^C")));
        Codes->insert(OFMake_pair(RO6931643_11C, DSRBasicCodedEntry("126720", "DCM", "RO6931643 ^11^C")));
        Codes->insert(OFMake_pair(RO6958948_18F, DSRBasicCodedEntry("126757", "DCM", "RO6958948 ^18^F")));
        Codes->insert(OFMake_pair(Roledumab_89Zr, DSRBasicCodedEntry("126733", "DCM", "Roledumab ^89^Zr")));
        Codes->insert(OFMake_pair(RubidiumChloride_Rb82, DSRBasicCodedEntry("C-B1037", "SRT", "Rubidium chloride Rb^82^")));
        Codes->insert(OFMake_pair(SAR3419_89Zr, DSRBasicCodedEntry("126741", "DCM", "SAR3419 ^89^Zr")));
        Codes->insert(OFMake_pair(Sarcosine_C11, DSRBasicCodedEntry("C122684", "NCIt", "Sarcosine C^11^")));
        Codes->insert(OFMake_pair(SodiumFluoride_F18, DSRBasicCodedEntry("C-B1032", "SRT", "Sodium fluoride F^18^")));
        Codes->insert(OFMake_pair(SodiumIodide_I124, DSRBasicCodedEntry("C-B07E8", "SRT", "Sodium iodide I^124^")));
        Codes->insert(OFMake_pair(Sodium_Na22, DSRBasicCodedEntry("C-B1047", "SRT", "Sodium Na^22^")));
        Codes->insert(OFMake_pair(Spiperone_F18, DSRBasicCodedEntry("C-B1033", "SRT", "Spiperone F^18^")));
        Codes->insert(OFMake_pair(T807_F18, DSRBasicCodedEntry("126502", "DCM", "T807 F^18^")));
        Codes->insert(OFMake_pair(THK5317_F18, DSRBasicCodedEntry("C4550127", "UMLS", "THK5317 F^18^")));
        Codes->insert(OFMake_pair(THK5351_F18, DSRBasicCodedEntry("C4279748", "UMLS", "THK5351 F^18^")));
        Codes->insert(OFMake_pair(ThymidineFLT_F18, DSRBasicCodedEntry("C-B1036", "SRT", "Thymidine (FLT) F^18^")));
        Codes->insert(OFMake_pair(Trastuzumab_89Zr, DSRBasicCodedEntry("126512", "DCM", "Trastuzumab ^89^Zr")));
        Codes->insert(OFMake_pair(TRC105_89Zr, DSRBasicCodedEntry("126749", "DCM", "TRC105 ^89^Zr")));
        Codes->insert(OFMake_pair(Tyrosine3Octreotate_Ga68, DSRBasicCodedEntry("C1742831", "UMLS", "tyrosine-3-octreotate Ga^68^")));
        Codes->insert(OFMake_pair(Ublituximab_89Zr, DSRBasicCodedEntry("126739", "DCM", "Ublituximab ^89^Zr")));
        Codes->insert(OFMake_pair(UCBJ_C11, DSRBasicCodedEntry("C4506788", "UMLS", "UCB-J C^11^")));
        Codes->insert(OFMake_pair(XmAb5574_89Zr, DSRBasicCodedEntry("126734", "DCM", "XmAb5574 ^89^Zr")));
    }
    /* should never be NULL */
    return *Codes;
}


OFCondition CID4021_PETRadiopharmaceutical::setEnhancedEncodingMode(DSRCodedEntryValue &codedEntryValue)
{
    return codedEntryValue.setEnhancedEncodingMode(CONTEXT_GROUP_NUMBER, "DCMR", CONTEXT_GROUP_VERSION, CONTEXT_GROUP_UID);
}
