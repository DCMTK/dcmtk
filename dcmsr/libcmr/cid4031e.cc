/*
 *
 *  Copyright (C) 2015-2018, J. Riesmeier, Oldenburg, Germany
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  Source file for class CID4031e_CommonAnatomicRegions
 *
 *  Author: Joerg Riesmeier
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/cmr/cid4031e.h"


/*---------------------*
 *  type declarations  *
 *---------------------*/

struct DefinedTermTypeMapStruct
{
    const char *DefinedTerm;
    CID4031_CommonAnatomicRegions::EnumType Type;
};


/*------------------------*
 *  constant definitions  *
 *------------------------*/

// mapping extracted from DICOM PS 3.16-2018a Table L-1

static const DefinedTermTypeMapStruct DefinedTermTypeMap[] =
{
    {"ABDOMEN",          CID4031_CommonAnatomicRegions::Abdomen},
    {"ABDOMENPELVIS",    CID4031_CommonAnatomicRegions::AbdomenAndPelvis},
    {"ACJOINT",          CID4031_CommonAnatomicRegions::AcromioclavicularJoint},
    {"ANKLE",            CID4031_CommonAnatomicRegions::AnkleJoint},
    {"BILEDUCT",         CID4031_CommonAnatomicRegions::BileDuct},
    {"BLADDER",          CID4031_CommonAnatomicRegions::Bladder},
    {"BREAST",           CID4031_CommonAnatomicRegions::Breast},
    {"BRONCHUS",         CID4031_CommonAnatomicRegions::Bronchus},
    {"CALCANEUS",        CID4031_CommonAnatomicRegions::Calcaneus},
    {"CSPINE",           CID4031_CommonAnatomicRegions::CervicalSpine},
    {"CTSPINE",          CID4031_CommonAnatomicRegions::CervicoThoracicSpine},
    {"CHEST",            CID4031_CommonAnatomicRegions::Chest},
    {"CHESTABDOMEN",     CID4031_CommonAnatomicRegions::ChestAndAbdomen},
    {"CHESTABDPELVIS",   CID4031_CommonAnatomicRegions::ChestAbdomenAndPelvis},
    {"CLAVICLE",         CID4031_CommonAnatomicRegions::Clavicle},
    {"COCCYX",           CID4031_CommonAnatomicRegions::Coccyx},
    {"COLON",            CID4031_CommonAnatomicRegions::Colon},
    {"DUODENUM",         CID4031_CommonAnatomicRegions::Duodenum},
    {"ELBOW",            CID4031_CommonAnatomicRegions::ElbowJoint},
    {"WHOLEBODY",        CID4031_CommonAnatomicRegions::EntireBody},
    {"ESOPHAGUS",        CID4031_CommonAnatomicRegions::Esophagus},
    {"EXTREMITY",        CID4031_CommonAnatomicRegions::Extremity},
    {"EYE",              CID4031_CommonAnatomicRegions::Eye},
    {"FEMUR",            CID4031_CommonAnatomicRegions::Femur},
    {"FIBULA",           CID4031_CommonAnatomicRegions::Fibula},
    {"FINGER",           CID4031_CommonAnatomicRegions::Finger},
    {"FOOT",             CID4031_CommonAnatomicRegions::Foot},
    {"FOREARM",          CID4031_CommonAnatomicRegions::Forearm},
    {"GALLBLADDER",      CID4031_CommonAnatomicRegions::Gallbladder},
    {"HAND",             CID4031_CommonAnatomicRegions::Hand},
    {"HEAD",             CID4031_CommonAnatomicRegions::Head},
    {"HEADNECK",         CID4031_CommonAnatomicRegions::HeadAndNeck},
    {"HEART",            CID4031_CommonAnatomicRegions::Heart},
    {"HIP",              CID4031_CommonAnatomicRegions::HipJoint},
    {"HUMERUS",          CID4031_CommonAnatomicRegions::Humerus},
    {"ILEUM",            CID4031_CommonAnatomicRegions::Ileum},
    {"ILIUM",            CID4031_CommonAnatomicRegions::Ilium},
    {"IAC",              CID4031_CommonAnatomicRegions::InternalAuditoryCanal},
    {"JAW",              CID4031_CommonAnatomicRegions::JawRegion},
    {"JEJUNUM",          CID4031_CommonAnatomicRegions::Jejunum},
    {"KNEE",             CID4031_CommonAnatomicRegions::Knee},
    {"LARGEINTESTINE",   CID4031_CommonAnatomicRegions::LargeIntestine},
    {"LARYNX",           CID4031_CommonAnatomicRegions::Larynx},
    {"LEG",              CID4031_CommonAnatomicRegions::LowerLeg},
    {"LSPINE",           CID4031_CommonAnatomicRegions::LumbarSpine},
    {"LSSPINE",          CID4031_CommonAnatomicRegions::LumboSacralSpine},
    {"JAW",              CID4031_CommonAnatomicRegions::Mandible},              // same Defined Term as for (T-D1213,SRT,"Jaw region")
    {"MASTOID",          CID4031_CommonAnatomicRegions::MastoidBone},
    {"MAXILLA",          CID4031_CommonAnatomicRegions::Maxilla},
    {"MEDIASTINUM",      CID4031_CommonAnatomicRegions::Mediastinum},
    {"NECK",             CID4031_CommonAnatomicRegions::Neck},
    {"NECKCHEST",        CID4031_CommonAnatomicRegions::NeckAndChest},
    {"NECKCHESTABDOMEN", CID4031_CommonAnatomicRegions::NeckChestAndAbdomen},
    {"NECKCHESTABDPELV", CID4031_CommonAnatomicRegions::NeckChestAbdomenAndPelvis},
    {"OPTICCANAL",       CID4031_CommonAnatomicRegions::OpticCanal},
    {"ORBIT",            CID4031_CommonAnatomicRegions::OrbitalStructure},
    {"PAROTID",          CID4031_CommonAnatomicRegions::ParotidGland},
    {"PATELLA",          CID4031_CommonAnatomicRegions::Patella},
    {"PELVIS",           CID4031_CommonAnatomicRegions::Pelvis},
    {"PROSTATE",         CID4031_CommonAnatomicRegions::Prostate},
    {"RECTUM",           CID4031_CommonAnatomicRegions::Rectum},
    {"RIB",              CID4031_CommonAnatomicRegions::Rib},
    {"SIJOINT",          CID4031_CommonAnatomicRegions::SacroiliacJoint},
    {"SSPINE",           CID4031_CommonAnatomicRegions::Sacrum},
    {"SCAPULA",          CID4031_CommonAnatomicRegions::Scapula},
    {"SELLA",            CID4031_CommonAnatomicRegions::SellaTurcica},
    {"SESAMOID",         CID4031_CommonAnatomicRegions::SesamoidBonesOfFoot},
    {"SHOULDER",         CID4031_CommonAnatomicRegions::Shoulder},
    {"SKULL",            CID4031_CommonAnatomicRegions::Skull},
    {"SMALLINTESTINE",   CID4031_CommonAnatomicRegions::SmallIntestine},
    {"SPINE",            CID4031_CommonAnatomicRegions::Spine},
    {"SCJOINT",          CID4031_CommonAnatomicRegions::SternoclavicularJoint},
    {"STERNUM",          CID4031_CommonAnatomicRegions::Sternum},
    {"STOMACH",          CID4031_CommonAnatomicRegions::Stomach},
    {"SUBMANDIBULAR",    CID4031_CommonAnatomicRegions::SubmandibularGland},
    {"TMJ",              CID4031_CommonAnatomicRegions::TemporomandibularJoint},
    {"THIGH",            CID4031_CommonAnatomicRegions::Thigh},
    {"TSPINE",           CID4031_CommonAnatomicRegions::ThoracicSpine},
    {"TLSPINE",          CID4031_CommonAnatomicRegions::ThoracoLumbarSpine},
    {"THUMB",            CID4031_CommonAnatomicRegions::Thumb},
    {"TOE",              CID4031_CommonAnatomicRegions::Toe},
    {"TRACHEA",          CID4031_CommonAnatomicRegions::Trachea},
    {"ARM",              CID4031_CommonAnatomicRegions::UpperArm},
    {"UPRURINARYTRACT",  CID4031_CommonAnatomicRegions::UpperUrinaryTract},
    {"URETER",           CID4031_CommonAnatomicRegions::Ureter},
    {"URETHRA",          CID4031_CommonAnatomicRegions::Urethra},
    {"WRIST",            CID4031_CommonAnatomicRegions::WristJoint},
    {"ZYGOMA",           CID4031_CommonAnatomicRegions::Zygoma}
};

static const size_t NumberOfDefinedTerms = sizeof(DefinedTermTypeMap) / sizeof(DefinedTermTypeMapStruct);


/*------------------*
 *  implementation  *
 *------------------*/

CID4031e_CommonAnatomicRegions::CID4031e_CommonAnatomicRegions(const DSRCodedEntryValue &selectedValue)
  : CID4031_CommonAnatomicRegions(selectedValue)
{
}


CID4031e_CommonAnatomicRegions::CID4031e_CommonAnatomicRegions(const EnumType selectedValue,
                                                               const OFBool enhancedEncodingMode)
  : CID4031_CommonAnatomicRegions(selectedValue, enhancedEncodingMode)
{
}


CID4031e_CommonAnatomicRegions::CID4031e_CommonAnatomicRegions(const OFString &selectedValue,
                                                               const OFBool enhancedEncodingMode)
  : CID4031_CommonAnatomicRegions(mapBodyPartExamined(selectedValue, enhancedEncodingMode))
{
}


OFCondition CID4031e_CommonAnatomicRegions::selectValue(const OFString &selectedValue,
                                                        const OFBool enhancedEncodingMode)
{
    DSRCodedEntryValue codedEntry;
    /* map defined term to coded entry */
    OFCondition result = mapBodyPartExamined(selectedValue, codedEntry, enhancedEncodingMode);
    if (result.good())
    {
        /* select but never check the coded entry */
        result = selectValue(codedEntry, OFFalse /*check*/, OFFalse /*definedContextGroup*/);
    }
    return result;
}


// static functions

DSRCodedEntryValue CID4031e_CommonAnatomicRegions::mapBodyPartExamined(const OFString &definedTerm,
                                                                       const OFBool enhancedEncodingMode)
{
    DSRCodedEntryValue codedEntry;
    /* call the real function (no need to check the returned status) */
    mapBodyPartExamined(definedTerm, codedEntry, enhancedEncodingMode);
    /* return coded entry (by-value) */
    return codedEntry;
}


OFCondition CID4031e_CommonAnatomicRegions::mapBodyPartExamined(const OFString &definedTerm,
                                                                DSRCodedEntryValue &codedEntryValue,
                                                                const OFBool enhancedEncodingMode)
{
    OFCondition result = SR_EC_UnsupportedValue;
    /* determine region code from CID 4031 (based on the mapping in PS 3.16 Annex L) */
    for (size_t i = 0; i < NumberOfDefinedTerms; ++i)
    {
        /* use first match */
        if (definedTerm == DefinedTermTypeMap[i].DefinedTerm)
        {
            codedEntryValue = getCodedEntry(DefinedTermTypeMap[i].Type);
            result = EC_Normal;
            break;
        }
    }
    /* set enhanced encoding mode (if enabled) */
    if (result.good() && enhancedEncodingMode)
        setEnhancedEncodingMode(codedEntryValue);
    return result;
}
