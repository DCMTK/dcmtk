/*
 *
 *  Copyright (C) 2015-2023, J. Riesmeier, Oldenburg, Germany
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  Source file for class CID4031e_CommonAnatomicRegion
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
    CID4031_CommonAnatomicRegion::EnumType Type;
};


/*------------------------*
 *  constant definitions  *
 *------------------------*/

// mapping extracted manually from DICOM PS 3.16-2023b Table L-1

static const DefinedTermTypeMapStruct DefinedTermTypeMap[] =
{
    {"ABDOMEN",          CID4031_CommonAnatomicRegion::Abdomen},
    {"ABDOMENPELVIS",    CID4031_CommonAnatomicRegion::AbdomenAndPelvis},
    {"ACJOINT",          CID4031_CommonAnatomicRegion::AcromioclavicularJoint},
    {"ANKLE",            CID4031_CommonAnatomicRegion::AnkleJoint},
    {"BILEDUCT",         CID4031_CommonAnatomicRegion::BileDuct},
    {"BILIARYTRACT​",     CID4031_CommonAnatomicRegion::BiliaryTract},
    {"BLADDER",          CID4031_CommonAnatomicRegion::Bladder},
    {"BREAST",           CID4031_CommonAnatomicRegion::Breast},
    {"BRONCHUS",         CID4031_CommonAnatomicRegion::Bronchus},
    {"CALCANEUS",        CID4031_CommonAnatomicRegion::Calcaneus},
    {"CSPINE",           CID4031_CommonAnatomicRegion::CervicalSpine},
    {"CTSPINE",          CID4031_CommonAnatomicRegion::CervicoThoracicSpine},
    {"CHEST",            CID4031_CommonAnatomicRegion::Chest},
    {"CHESTABDOMEN",     CID4031_CommonAnatomicRegion::ChestAndAbdomen},
    {"CHESTABDPELVIS",   CID4031_CommonAnatomicRegion::ChestAbdomenAndPelvis},
    {"CLAVICLE",         CID4031_CommonAnatomicRegion::Clavicle},
    {"COCCYX",           CID4031_CommonAnatomicRegion::Coccyx},
    {"COLON",            CID4031_CommonAnatomicRegion::Colon},
    {"COMMONBILEDUCT",   CID4031_CommonAnatomicRegion::CommonBileDuct},
    {"DUODENUM",         CID4031_CommonAnatomicRegion::Duodenum},
    {"ELBOW",            CID4031_CommonAnatomicRegion::ElbowJoint},
    {"WHOLEBODY",        CID4031_CommonAnatomicRegion::EntireBody},
    {"ESOPHAGUS",        CID4031_CommonAnatomicRegion::Esophagus},
    {"EXTREMITY",        CID4031_CommonAnatomicRegion::Extremity},
    {"EYE",              CID4031_CommonAnatomicRegion::Eye},
    {"FEMUR",            CID4031_CommonAnatomicRegion::Femur},
    {"FIBULA",           CID4031_CommonAnatomicRegion::Fibula},
    {"FINGER",           CID4031_CommonAnatomicRegion::Finger},
    {"FOOT",             CID4031_CommonAnatomicRegion::Foot},
    {"FOREARM",          CID4031_CommonAnatomicRegion::Forearm},
    {"GALLBLADDER",      CID4031_CommonAnatomicRegion::Gallbladder},
    {"HAND",             CID4031_CommonAnatomicRegion::Hand},
    {"HEAD",             CID4031_CommonAnatomicRegion::Head},
    {"HEADNECK",         CID4031_CommonAnatomicRegion::HeadAndNeck},
    {"HEART",            CID4031_CommonAnatomicRegion::Heart},
    {"HIP",              CID4031_CommonAnatomicRegion::HipJoint},
    {"HUMERUS",          CID4031_CommonAnatomicRegion::Humerus},
    {"ILEUM",            CID4031_CommonAnatomicRegion::Ileum},
    {"ILIUM",            CID4031_CommonAnatomicRegion::Ilium},
    {"IAC",              CID4031_CommonAnatomicRegion::InternalAuditoryCanal},
    {"JAW",              CID4031_CommonAnatomicRegion::JawRegion},
    {"JEJUNUM",          CID4031_CommonAnatomicRegion::Jejunum},
    {"KNEE",             CID4031_CommonAnatomicRegion::Knee},
    {"LARGEINTESTINE",   CID4031_CommonAnatomicRegion::LargeIntestine},
    {"LARYNX",           CID4031_CommonAnatomicRegion::Larynx},
    {"LOWERLEG",         CID4031_CommonAnatomicRegion::LowerLeg},
    {"LOWERLIMB",        CID4031_CommonAnatomicRegion::LowerLimb},
    {"LSPINE",           CID4031_CommonAnatomicRegion::LumbarSpine},
    {"LSSPINE",          CID4031_CommonAnatomicRegion::LumboSacralSpine},
    {"MANDIBLE",         CID4031_CommonAnatomicRegion::Mandible},
    {"MASTOID",          CID4031_CommonAnatomicRegion::MastoidBone},
    {"MAXILLA",          CID4031_CommonAnatomicRegion::Maxilla},
    {"MEDIASTINUM",      CID4031_CommonAnatomicRegion::Mediastinum},
    {"NECK",             CID4031_CommonAnatomicRegion::Neck},
    {"NECKCHEST",        CID4031_CommonAnatomicRegion::NeckAndChest},
    {"NECKCHESTABDOMEN", CID4031_CommonAnatomicRegion::NeckChestAndAbdomen},
    {"NECKCHESTABDPELV", CID4031_CommonAnatomicRegion::NeckChestAbdomenAndPelvis},
    {"OPTICCANAL",       CID4031_CommonAnatomicRegion::OpticCanal},
    {"ORBIT",            CID4031_CommonAnatomicRegion::OrbitalStructure},
    {"PANCREAS",         CID4031_CommonAnatomicRegion::Pancreas},
    {"PANCREATICDUCT",   CID4031_CommonAnatomicRegion::PancreaticDuct},
    {"PANCBILEDUCT​",     CID4031_CommonAnatomicRegion::PancreaticDuctAndBileDuctSystems},
    {"PAROTID",          CID4031_CommonAnatomicRegion::ParotidGland},
    {"PATELLA",          CID4031_CommonAnatomicRegion::Patella},
    {"PELVIS",           CID4031_CommonAnatomicRegion::Pelvis},
    {"PELVISLOWEXTREMT​", CID4031_CommonAnatomicRegion::PelvisAndLowerExtremities},
    {"PHANTOM",          CID4031_CommonAnatomicRegion::Phantom},
    {"PROSTATE",         CID4031_CommonAnatomicRegion::Prostate},
    {"RECTUM",           CID4031_CommonAnatomicRegion::Rectum},
    {"RIB",              CID4031_CommonAnatomicRegion::Rib},
    {"SIJOINT",          CID4031_CommonAnatomicRegion::SacroiliacJoint},
    {"SSPINE",           CID4031_CommonAnatomicRegion::Sacrum},
    {"SCAPULA",          CID4031_CommonAnatomicRegion::Scapula},
    {"SELLA",            CID4031_CommonAnatomicRegion::SellaTurcica},
    {"SESAMOID",         CID4031_CommonAnatomicRegion::SesamoidBonesOfFoot},
    {"SHOULDER",         CID4031_CommonAnatomicRegion::Shoulder},
    {"SKULL",            CID4031_CommonAnatomicRegion::Skull},
    {"SMALLINTESTINE",   CID4031_CommonAnatomicRegion::SmallIntestine},
    {"SPINE",            CID4031_CommonAnatomicRegion::Spine},
    {"SCJOINT",          CID4031_CommonAnatomicRegion::SternoclavicularJoint},
    {"STERNUM",          CID4031_CommonAnatomicRegion::Sternum},
    {"STOMACH",          CID4031_CommonAnatomicRegion::Stomach},
    {"SUBMANDIBULAR",    CID4031_CommonAnatomicRegion::SubmandibularGland},
    {"TMJ",              CID4031_CommonAnatomicRegion::TemporomandibularJoint},
    {"THIGH",            CID4031_CommonAnatomicRegion::Thigh},
    {"TSPINE",           CID4031_CommonAnatomicRegion::ThoracicSpine},
    {"TLSPINE",          CID4031_CommonAnatomicRegion::ThoracoLumbarSpine},
    {"THUMB",            CID4031_CommonAnatomicRegion::Thumb},
    {"TOE",              CID4031_CommonAnatomicRegion::Toe},
    {"TRACHEA",          CID4031_CommonAnatomicRegion::Trachea},
    {"UPPERARM",         CID4031_CommonAnatomicRegion::UpperArm},
    {"UPPERLIMB",        CID4031_CommonAnatomicRegion::UpperLimb},
    {"UPRURINARYTRACT",  CID4031_CommonAnatomicRegion::UpperUrinaryTract},
    {"URETER",           CID4031_CommonAnatomicRegion::Ureter},
    {"URETHRA",          CID4031_CommonAnatomicRegion::Urethra},
    {"WRIST",            CID4031_CommonAnatomicRegion::WristJoint},
    {"ZYGOMA",           CID4031_CommonAnatomicRegion::Zygoma}
};

static const size_t NumberOfDefinedTerms = sizeof(DefinedTermTypeMap) / sizeof(DefinedTermTypeMapStruct);


/*------------------*
 *  implementation  *
 *------------------*/

CID4031e_CommonAnatomicRegion::CID4031e_CommonAnatomicRegion(const DSRCodedEntryValue &selectedValue)
  : CID4031_CommonAnatomicRegion(selectedValue)
{
}


CID4031e_CommonAnatomicRegion::CID4031e_CommonAnatomicRegion(const EnumType selectedValue,
                                                             const OFBool enhancedEncodingMode)
  : CID4031_CommonAnatomicRegion(selectedValue, enhancedEncodingMode)
{
}


CID4031e_CommonAnatomicRegion::CID4031e_CommonAnatomicRegion(const OFString &selectedValue,
                                                             const OFBool enhancedEncodingMode)
  : CID4031_CommonAnatomicRegion(mapBodyPartExamined(selectedValue, enhancedEncodingMode))
{
}


OFCondition CID4031e_CommonAnatomicRegion::selectValue(const OFString &selectedValue,
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

DSRCodedEntryValue CID4031e_CommonAnatomicRegion::mapBodyPartExamined(const OFString &definedTerm,
                                                                      const OFBool enhancedEncodingMode)
{
    DSRCodedEntryValue codedEntry;
    /* call the real function (no need to check the returned status) */
    mapBodyPartExamined(definedTerm, codedEntry, enhancedEncodingMode);
    /* return coded entry (by-value) */
    return codedEntry;
}


OFCondition CID4031e_CommonAnatomicRegion::mapBodyPartExamined(const OFString &definedTerm,
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
