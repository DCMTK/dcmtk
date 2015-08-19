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
 *    classes: CID4031_CommonAnatomicRegions
 *
 *    Generated automatically from DICOM PS 3.16-2015c
 *    File created on 2015-08-19 16:32:49 by J. Riesmeier
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/cmr/cid4031.h"


// general information on CID 4031 (Common Anatomic Regions)
#define CONTEXT_GROUP_NUMBER  "4031"
#define CONTEXT_GROUP_VERSION "20130617"
#define CONTEXT_GROUP_UID     "1.2.840.10008.6.1.308"
#define CONTEXT_GROUP_TYPE    OFTrue  /* extensible */


CID4031_CommonAnatomicRegions::CID4031_CommonAnatomicRegions(const DSRCodedEntryValue &selectedValue)
  : DSRContextGroup(CONTEXT_GROUP_NUMBER, "DCMR", CONTEXT_GROUP_VERSION, CONTEXT_GROUP_UID, selectedValue)
{
    setExtensible(CONTEXT_GROUP_TYPE);
}


CID4031_CommonAnatomicRegions::CID4031_CommonAnatomicRegions(const EnumType selectedValue,
                                                             const OFBool enhancedEncodingMode)
  : DSRContextGroup(CONTEXT_GROUP_NUMBER, "DCMR", CONTEXT_GROUP_VERSION, CONTEXT_GROUP_UID, getCodedEntry(selectedValue, enhancedEncodingMode))
{
    setExtensible(CONTEXT_GROUP_TYPE);
}


OFCondition CID4031_CommonAnatomicRegions::selectValue(const EnumType selectedValue,
                                                       const OFBool enhancedEncodingMode)
{
    /* never check the coded entry */
    return DSRContextGroup::selectValue(getCodedEntry(selectedValue, enhancedEncodingMode), OFFalse /*check*/, OFFalse /*definedContextGroup*/);
}


OFCondition CID4031_CommonAnatomicRegions::findCodedEntry(const DSRCodedEntryValue &searchForCodedEntry,
                                                          DSRCodedEntryValue *foundCodedEntry,
                                                          const OFBool enhancedEncodingMode) const
{
    OFCondition result = SR_EC_CodedEntryNotInContextGroup;
    /* first, search for standard codes */
    OFMap<EnumType, DSRBasicCodedEntry>::const_iterator iter = getCodes().begin();
    OFMap<EnumType, DSRBasicCodedEntry>::const_iterator last = getCodes().end();
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


void CID4031_CommonAnatomicRegions::printCodes(STD_NAMESPACE ostream &stream) const
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

void CID4031_CommonAnatomicRegions::initialize()
{
    /* create and initialize code list */
    getCodes();
}


DSRCodedEntryValue CID4031_CommonAnatomicRegions::getCodedEntry(const EnumType value,
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


CID4031_CommonAnatomicRegions::CodeList &CID4031_CommonAnatomicRegions::getCodes()
{
    /* use a static variable for singleton pattern */
    static CodeList *codes = NULL;
    /* check whether code list has already been created and initialized */
    if (codes == NULL)
    {
        /* create a new code list (should never fail) */
        codes = new CodeList();
        /* and initialize it by adding the coded entries */
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(Abdomen, DSRBasicCodedEntry("T-D4000", "SRT", "Abdomen")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(AbdomenAndPelvis, DSRBasicCodedEntry("R-FAB57", "SRT", "Abdomen and Pelvis")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(AcromioclavicularJoint, DSRBasicCodedEntry("T-15420", "SRT", "Acromioclavicular joint")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(AnkleJoint, DSRBasicCodedEntry("T-15750", "SRT", "Ankle joint")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(Anus, DSRBasicCodedEntry("T-59900", "SRT", "Anus")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(ApexOfLung, DSRBasicCodedEntry("T-280A0", "SRT", "Apex of Lung")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(BileDuct, DSRBasicCodedEntry("T-60610", "SRT", "Bile duct")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(Bladder, DSRBasicCodedEntry("T-74000", "SRT", "Bladder")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(BoneOfLowerLimb, DSRBasicCodedEntry("T-12700", "SRT", "Bone of lower limb")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(BoneOfUpperLimb, DSRBasicCodedEntry("T-D0821", "SRT", "Bone of upper limb")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(Breast, DSRBasicCodedEntry("T-04000", "SRT", "Breast")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(Bronchus, DSRBasicCodedEntry("T-26000", "SRT", "Bronchus")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(Calcaneus, DSRBasicCodedEntry("T-12770", "SRT", "Calcaneus")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(CervicalSpine, DSRBasicCodedEntry("T-11501", "SRT", "Cervical spine")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(CervicoThoracicSpine, DSRBasicCodedEntry("T-D00F7", "SRT", "Cervico-thoracic spine")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(Chest, DSRBasicCodedEntry("T-D3000", "SRT", "Chest")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(ChestAndAbdomen, DSRBasicCodedEntry("R-FAB55", "SRT", "Chest and Abdomen")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(ChestAbdomenAndPelvis, DSRBasicCodedEntry("R-FAB56", "SRT", "Chest, Abdomen and Pelvis")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(Clavicle, DSRBasicCodedEntry("T-12310", "SRT", "Clavicle")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(Coccyx, DSRBasicCodedEntry("T-11BF0", "SRT", "Coccyx")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(Colon, DSRBasicCodedEntry("T-59300", "SRT", "Colon")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(Duodenum, DSRBasicCodedEntry("T-58200", "SRT", "Duodenum")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(ElbowJoint, DSRBasicCodedEntry("T-15430", "SRT", "Elbow joint")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(EntireBody, DSRBasicCodedEntry("T-D0010", "SRT", "Entire body")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(Esophagus, DSRBasicCodedEntry("T-56000", "SRT", "Esophagus")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(EsophagusStomachAndDuodenum, DSRBasicCodedEntry("T-DD163", "SRT", "Esophagus, stomach and duodenum")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(Extremity, DSRBasicCodedEntry("T-D0300", "SRT", "Extremity")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(Eye, DSRBasicCodedEntry("T-AA000", "SRT", "Eye")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(EyeRegion, DSRBasicCodedEntry("T-D0801", "SRT", "Eye region")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(FacialBones, DSRBasicCodedEntry("T-11196", "SRT", "Facial bones")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(Femur, DSRBasicCodedEntry("T-12710", "SRT", "Femur")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(Fibula, DSRBasicCodedEntry("T-12750", "SRT", "Fibula")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(Finger, DSRBasicCodedEntry("T-D8800", "SRT", "Finger")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(Foot, DSRBasicCodedEntry("T-D9700", "SRT", "Foot")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(Forearm, DSRBasicCodedEntry("T-D8500", "SRT", "Forearm")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(Gallbladder, DSRBasicCodedEntry("T-63000", "SRT", "Gallbladder")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(Hand, DSRBasicCodedEntry("T-D8700", "SRT", "Hand")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(Head, DSRBasicCodedEntry("T-D1100", "SRT", "Head")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(HeadAndNeck, DSRBasicCodedEntry("T-D1000", "SRT", "Head and Neck")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(Heart, DSRBasicCodedEntry("T-32000", "SRT", "Heart")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(HipJoint, DSRBasicCodedEntry("T-15710", "SRT", "Hip joint")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(Humerus, DSRBasicCodedEntry("T-12410", "SRT", "Humerus")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(Ileum, DSRBasicCodedEntry("T-58600", "SRT", "Ileum")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(Ilium, DSRBasicCodedEntry("T-12340", "SRT", "Ilium")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(InternalAuditoryCanal, DSRBasicCodedEntry("T-AB959", "SRT", "Internal Auditory Canal")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(JawRegion, DSRBasicCodedEntry("T-D1213", "SRT", "Jaw region")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(Jejunum, DSRBasicCodedEntry("T-58400", "SRT", "Jejunum")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(Knee, DSRBasicCodedEntry("T-D9200", "SRT", "Knee")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(LargeIntestine, DSRBasicCodedEntry("T-59000", "SRT", "Large intestine")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(Larynx, DSRBasicCodedEntry("T-24100", "SRT", "Larynx")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(LowerLeg, DSRBasicCodedEntry("T-D9400", "SRT", "Lower leg")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(LowerLimb, DSRBasicCodedEntry("T-D9000", "SRT", "Lower limb")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(LumbarSpine, DSRBasicCodedEntry("T-11503", "SRT", "Lumbar spine")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(LumboSacralSpine, DSRBasicCodedEntry("T-D00F9", "SRT", "Lumbo-sacral spine")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(Mandible, DSRBasicCodedEntry("T-11180", "SRT", "Mandible")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(MastoidBone, DSRBasicCodedEntry("T-11133", "SRT", "Mastoid bone")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(Maxilla, DSRBasicCodedEntry("T-11170", "SRT", "Maxilla")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(Mediastinum, DSRBasicCodedEntry("T-D3300", "SRT", "Mediastinum")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(MuscleOfLowerLimb, DSRBasicCodedEntry("T-14668", "SRT", "Muscle of lower limb")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(MuscleOfUpperLimb, DSRBasicCodedEntry("T-13600", "SRT", "Muscle of upper limb")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(NasalBone, DSRBasicCodedEntry("T-11149", "SRT", "Nasal bone")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(Neck, DSRBasicCodedEntry("T-D1600", "SRT", "Neck")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(NeckAndChest, DSRBasicCodedEntry("R-FAB52", "SRT", "Neck and Chest")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(NeckChestAndAbdomen, DSRBasicCodedEntry("R-FAB53", "SRT", "Neck, Chest and Abdomen")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(NeckChestAbdomenAndPelvis, DSRBasicCodedEntry("R-FAB54", "SRT", "Neck, Chest, Abdomen and Pelvis")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(OpticCanal, DSRBasicCodedEntry("T-11102", "SRT", "Optic canal")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(OrbitalStructure, DSRBasicCodedEntry("T-D14AE", "SRT", "Orbital structure")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(PancreaticDuctAndBileDuctSystems, DSRBasicCodedEntry("T-65600", "SRT", "Pancreatic duct and bile duct systems")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(ParanasalSinus, DSRBasicCodedEntry("T-22000", "SRT", "Paranasal sinus")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(ParotidGland, DSRBasicCodedEntry("T-61100", "SRT", "Parotid gland")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(Patella, DSRBasicCodedEntry("T-12730", "SRT", "Patella")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(Pelvis, DSRBasicCodedEntry("T-D6000", "SRT", "Pelvis")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(PelvisAndLowerExtremities, DSRBasicCodedEntry("R-FAB58", "SRT", "Pelvis and lower extremities")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(Phantom, DSRBasicCodedEntry("113681", "DCM", "Phantom")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(Prostate, DSRBasicCodedEntry("T-9200B", "SRT", "Prostate")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(Rectum, DSRBasicCodedEntry("T-59600", "SRT", "Rectum")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(Rib, DSRBasicCodedEntry("T-11300", "SRT", "Rib")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(SacroiliacJoint, DSRBasicCodedEntry("T-15680", "SRT", "Sacroiliac joint")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(Sacrum, DSRBasicCodedEntry("T-11AD0", "SRT", "Sacrum")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(Scapula, DSRBasicCodedEntry("T-12280", "SRT", "Scapula")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(SellaTurcica, DSRBasicCodedEntry("T-D1460", "SRT", "Sella turcica")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(SesamoidBonesOfFoot, DSRBasicCodedEntry("T-12980", "SRT", "Sesamoid bones of foot")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(Shoulder, DSRBasicCodedEntry("T-D2220", "SRT", "Shoulder")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(Skull, DSRBasicCodedEntry("T-11100", "SRT", "Skull")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(SmallIntestine, DSRBasicCodedEntry("T-58000", "SRT", "Small intestine")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(Spine, DSRBasicCodedEntry("T-D0146", "SRT", "Spine")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(SternoclavicularJoint, DSRBasicCodedEntry("T-15610", "SRT", "Sternoclavicular joint")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(Sternum, DSRBasicCodedEntry("T-11210", "SRT", "Sternum")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(Stomach, DSRBasicCodedEntry("T-57000", "SRT", "Stomach")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(SubmandibularGland, DSRBasicCodedEntry("T-61300", "SRT", "Submandibular gland")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(TarsalJoint, DSRBasicCodedEntry("T-15770", "SRT", "Tarsal joint")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(TemporomandibularJoint, DSRBasicCodedEntry("T-15290", "SRT", "Temporomandibular joint")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(Thigh, DSRBasicCodedEntry("T-D9100", "SRT", "Thigh")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(ThoracicSpine, DSRBasicCodedEntry("T-11502", "SRT", "Thoracic spine")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(ThoracoLumbarSpine, DSRBasicCodedEntry("T-D00F8", "SRT", "Thoraco-lumbar spine")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(Thumb, DSRBasicCodedEntry("T-D8810", "SRT", "Thumb")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(Toe, DSRBasicCodedEntry("T-D9800", "SRT", "Toe")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(Trachea, DSRBasicCodedEntry("T-25000", "SRT", "Trachea")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(UpperArm, DSRBasicCodedEntry("T-D8200", "SRT", "Upper arm")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(UpperLimb, DSRBasicCodedEntry("T-D8000", "SRT", "Upper limb")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(UpperUrinaryTract, DSRBasicCodedEntry("T-70010", "SRT", "Upper urinary tract")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(Ureter, DSRBasicCodedEntry("T-73000", "SRT", "Ureter")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(Urethra, DSRBasicCodedEntry("T-75000", "SRT", "Urethra")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(UterusAndFallopianTubes, DSRBasicCodedEntry("T-88920", "SRT", "Uterus and fallopian tubes")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(VertebralColumnAndCranium, DSRBasicCodedEntry("T-11011", "SRT", "Vertebral column and cranium")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(WristJoint, DSRBasicCodedEntry("T-15460", "SRT", "Wrist joint")));
        codes->insert(OFMake_pair<EnumType, DSRBasicCodedEntry>(Zygoma, DSRBasicCodedEntry("T-11166", "SRT", "Zygoma")));
    }
    /* should never be NULL */
    return *codes;
}


OFCondition CID4031_CommonAnatomicRegions::setEnhancedEncodingMode(DSRCodedEntryValue &codedEntryValue)
{
    return codedEntryValue.setEnhancedEncodingMode(CONTEXT_GROUP_NUMBER, "DCMR", CONTEXT_GROUP_VERSION, CONTEXT_GROUP_UID);
}
