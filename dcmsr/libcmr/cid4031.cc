/*
 *
 *  Copyright (C) 2015-2019, J. Riesmeier, Oldenburg, Germany
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  Source file for class CID4031_CommonAnatomicRegions
 *
 *  Generated automatically from DICOM PS 3.16-2019b
 *  File created on 2019-04-26 17:15:22 by J. Riesmeier
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/cmr/cid4031.h"


// general information on CID 4031 (Common Anatomic Regions)
#define CONTEXT_GROUP_NUMBER  "4031"
#define CONTEXT_GROUP_VERSION "20170914"
#define CONTEXT_GROUP_UID     "1.2.840.10008.6.1.308"
#define CONTEXT_GROUP_TYPE    OFTrue  /* extensible */

// initialize global/static variable
CID4031_CommonAnatomicRegions::CodeList *CID4031_CommonAnatomicRegions::Codes = NULL;


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


void CID4031_CommonAnatomicRegions::printCodes(STD_NAMESPACE ostream &stream) const
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

void CID4031_CommonAnatomicRegions::initialize()
{
    /* create and initialize code list */
    getCodes();
}


void CID4031_CommonAnatomicRegions::cleanup()
{
    /* delete code list, it will be recreated automatically when needed */
    delete Codes;
    Codes = NULL;
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
    /* check whether code list has already been created and initialized */
    if (Codes == NULL)
    {
        /* create a new code list (should never fail) */
        Codes = new CodeList();
        /* and initialize it by adding the coded entries */
        Codes->insert(OFMake_pair(Abdomen, DSRBasicCodedEntry("113345001", "SCT", "Abdomen")));
        Codes->insert(OFMake_pair(AbdomenAndPelvis, DSRBasicCodedEntry("416949008", "SCT", "Abdomen and Pelvis")));
        Codes->insert(OFMake_pair(AcromioclavicularJoint, DSRBasicCodedEntry("85856004", "SCT", "Acromioclavicular joint")));
        Codes->insert(OFMake_pair(AnkleJoint, DSRBasicCodedEntry("70258002", "SCT", "Ankle joint")));
        Codes->insert(OFMake_pair(Anus, DSRBasicCodedEntry("53505006", "SCT", "Anus")));
        Codes->insert(OFMake_pair(ApexOfLung, DSRBasicCodedEntry("86598002", "SCT", "Apex of Lung")));
        Codes->insert(OFMake_pair(BileDuct, DSRBasicCodedEntry("28273000", "SCT", "Bile duct")));
        Codes->insert(OFMake_pair(Bladder, DSRBasicCodedEntry("89837001", "SCT", "Bladder")));
        Codes->insert(OFMake_pair(BoneOfLowerLimb, DSRBasicCodedEntry("72001000", "SCT", "Bone of lower limb")));
        Codes->insert(OFMake_pair(BoneOfUpperLimb, DSRBasicCodedEntry("371195002", "SCT", "Bone of upper limb")));
        Codes->insert(OFMake_pair(Breast, DSRBasicCodedEntry("76752008", "SCT", "Breast")));
        Codes->insert(OFMake_pair(Bronchus, DSRBasicCodedEntry("955009", "SCT", "Bronchus")));
        Codes->insert(OFMake_pair(Calcaneus, DSRBasicCodedEntry("80144004", "SCT", "Calcaneus")));
        Codes->insert(OFMake_pair(CervicalSpine, DSRBasicCodedEntry("122494005", "SCT", "Cervical spine")));
        Codes->insert(OFMake_pair(CervicoThoracicSpine, DSRBasicCodedEntry("297171002", "SCT", "Cervico-thoracic spine")));
        Codes->insert(OFMake_pair(Chest, DSRBasicCodedEntry("51185008", "SCT", "Chest")));
        Codes->insert(OFMake_pair(ChestAndAbdomen, DSRBasicCodedEntry("416550000", "SCT", "Chest and Abdomen")));
        Codes->insert(OFMake_pair(ChestAbdomenAndPelvis, DSRBasicCodedEntry("416775004", "SCT", "Chest, Abdomen and Pelvis")));
        Codes->insert(OFMake_pair(Clavicle, DSRBasicCodedEntry("51299004", "SCT", "Clavicle")));
        Codes->insert(OFMake_pair(Coccyx, DSRBasicCodedEntry("64688005", "SCT", "Coccyx")));
        Codes->insert(OFMake_pair(Colon, DSRBasicCodedEntry("71854001", "SCT", "Colon")));
        Codes->insert(OFMake_pair(Duodenum, DSRBasicCodedEntry("38848004", "SCT", "Duodenum")));
        Codes->insert(OFMake_pair(ElbowJoint, DSRBasicCodedEntry("16953009", "SCT", "Elbow joint")));
        Codes->insert(OFMake_pair(EntireBody, DSRBasicCodedEntry("38266002", "SCT", "Entire body")));
        Codes->insert(OFMake_pair(Esophagus, DSRBasicCodedEntry("32849002", "SCT", "Esophagus")));
        Codes->insert(OFMake_pair(EsophagusStomachAndDuodenum, DSRBasicCodedEntry("110861005", "SCT", "Esophagus, stomach and duodenum")));
        Codes->insert(OFMake_pair(Extremity, DSRBasicCodedEntry("66019005", "SCT", "Extremity")));
        Codes->insert(OFMake_pair(Eye, DSRBasicCodedEntry("81745001", "SCT", "Eye")));
        Codes->insert(OFMake_pair(EyeRegion, DSRBasicCodedEntry("371398005", "SCT", "Eye region")));
        Codes->insert(OFMake_pair(FacialBones, DSRBasicCodedEntry("91397008", "SCT", "Facial bones")));
        Codes->insert(OFMake_pair(Femur, DSRBasicCodedEntry("71341001", "SCT", "Femur")));
        Codes->insert(OFMake_pair(Fibula, DSRBasicCodedEntry("87342007", "SCT", "Fibula")));
        Codes->insert(OFMake_pair(Finger, DSRBasicCodedEntry("7569003", "SCT", "Finger")));
        Codes->insert(OFMake_pair(Foot, DSRBasicCodedEntry("56459004", "SCT", "Foot")));
        Codes->insert(OFMake_pair(Forearm, DSRBasicCodedEntry("14975008", "SCT", "Forearm")));
        Codes->insert(OFMake_pair(Gallbladder, DSRBasicCodedEntry("28231008", "SCT", "Gallbladder")));
        Codes->insert(OFMake_pair(Hand, DSRBasicCodedEntry("85562004", "SCT", "Hand")));
        Codes->insert(OFMake_pair(Head, DSRBasicCodedEntry("69536005", "SCT", "Head")));
        Codes->insert(OFMake_pair(HeadAndNeck, DSRBasicCodedEntry("774007", "SCT", "Head and Neck")));
        Codes->insert(OFMake_pair(Heart, DSRBasicCodedEntry("80891009", "SCT", "Heart")));
        Codes->insert(OFMake_pair(HipJoint, DSRBasicCodedEntry("29836001", "SCT", "Hip joint")));
        Codes->insert(OFMake_pair(Humerus, DSRBasicCodedEntry("85050009", "SCT", "Humerus")));
        Codes->insert(OFMake_pair(Ileum, DSRBasicCodedEntry("34516001", "SCT", "Ileum")));
        Codes->insert(OFMake_pair(Ilium, DSRBasicCodedEntry("22356005", "SCT", "Ilium")));
        Codes->insert(OFMake_pair(InternalAuditoryCanal, DSRBasicCodedEntry("361078006", "SCT", "Internal Auditory Canal")));
        Codes->insert(OFMake_pair(JawRegion, DSRBasicCodedEntry("661005", "SCT", "Jaw region")));
        Codes->insert(OFMake_pair(Jejunum, DSRBasicCodedEntry("21306003", "SCT", "Jejunum")));
        Codes->insert(OFMake_pair(Knee, DSRBasicCodedEntry("72696002", "SCT", "Knee")));
        Codes->insert(OFMake_pair(LargeIntestine, DSRBasicCodedEntry("14742008", "SCT", "Large intestine")));
        Codes->insert(OFMake_pair(Larynx, DSRBasicCodedEntry("4596009", "SCT", "Larynx")));
        Codes->insert(OFMake_pair(LowerLeg, DSRBasicCodedEntry("30021000", "SCT", "Lower leg")));
        Codes->insert(OFMake_pair(LowerLimb, DSRBasicCodedEntry("61685007", "SCT", "Lower limb")));
        Codes->insert(OFMake_pair(LumbarSpine, DSRBasicCodedEntry("122496007", "SCT", "Lumbar spine")));
        Codes->insert(OFMake_pair(LumboSacralSpine, DSRBasicCodedEntry("297173004", "SCT", "Lumbo-sacral spine")));
        Codes->insert(OFMake_pair(Mandible, DSRBasicCodedEntry("91609006", "SCT", "Mandible")));
        Codes->insert(OFMake_pair(MastoidBone, DSRBasicCodedEntry("59066005", "SCT", "Mastoid bone")));
        Codes->insert(OFMake_pair(Maxilla, DSRBasicCodedEntry("70925003", "SCT", "Maxilla")));
        Codes->insert(OFMake_pair(Mediastinum, DSRBasicCodedEntry("72410000", "SCT", "Mediastinum")));
        Codes->insert(OFMake_pair(MuscleOfLowerLimb, DSRBasicCodedEntry("102292000", "SCT", "Muscle of lower limb")));
        Codes->insert(OFMake_pair(MuscleOfUpperLimb, DSRBasicCodedEntry("30608006", "SCT", "Muscle of upper limb")));
        Codes->insert(OFMake_pair(NasalBone, DSRBasicCodedEntry("74386004", "SCT", "Nasal bone")));
        Codes->insert(OFMake_pair(Neck, DSRBasicCodedEntry("45048000", "SCT", "Neck")));
        Codes->insert(OFMake_pair(NeckAndChest, DSRBasicCodedEntry("417437006", "SCT", "Neck and Chest")));
        Codes->insert(OFMake_pair(NeckChestAndAbdomen, DSRBasicCodedEntry("416152001", "SCT", "Neck, Chest and Abdomen")));
        Codes->insert(OFMake_pair(NeckChestAbdomenAndPelvis, DSRBasicCodedEntry("416319003", "SCT", "Neck, Chest, Abdomen and Pelvis")));
        Codes->insert(OFMake_pair(OpticCanal, DSRBasicCodedEntry("55024004", "SCT", "Optic canal")));
        Codes->insert(OFMake_pair(OrbitalStructure, DSRBasicCodedEntry("363654007", "SCT", "Orbital structure")));
        Codes->insert(OFMake_pair(PancreaticDuctAndBileDuctSystems, DSRBasicCodedEntry("110621006", "SCT", "Pancreatic duct and bile duct systems")));
        Codes->insert(OFMake_pair(ParanasalSinus, DSRBasicCodedEntry("2095001", "SCT", "Paranasal sinus")));
        Codes->insert(OFMake_pair(ParotidGland, DSRBasicCodedEntry("45289007", "SCT", "Parotid gland")));
        Codes->insert(OFMake_pair(Patella, DSRBasicCodedEntry("64234005", "SCT", "Patella")));
        Codes->insert(OFMake_pair(Pelvis, DSRBasicCodedEntry("12921003", "SCT", "Pelvis")));
        Codes->insert(OFMake_pair(PelvisAndLowerExtremities, DSRBasicCodedEntry("416631005", "SCT", "Pelvis and lower extremities")));
        Codes->insert(OFMake_pair(Phantom, DSRBasicCodedEntry("113681", "DCM", "Phantom")));
        Codes->insert(OFMake_pair(Prostate, DSRBasicCodedEntry("41216001", "SCT", "Prostate")));
        Codes->insert(OFMake_pair(Rectum, DSRBasicCodedEntry("34402009", "SCT", "Rectum")));
        Codes->insert(OFMake_pair(Rib, DSRBasicCodedEntry("113197003", "SCT", "Rib")));
        Codes->insert(OFMake_pair(SacroiliacJoint, DSRBasicCodedEntry("39723000", "SCT", "Sacroiliac joint")));
        Codes->insert(OFMake_pair(Sacrum, DSRBasicCodedEntry("54735007", "SCT", "Sacrum")));
        Codes->insert(OFMake_pair(Scapula, DSRBasicCodedEntry("79601000", "SCT", "Scapula")));
        Codes->insert(OFMake_pair(SellaTurcica, DSRBasicCodedEntry("42575006", "SCT", "Sella turcica")));
        Codes->insert(OFMake_pair(SesamoidBonesOfFoot, DSRBasicCodedEntry("58742003", "SCT", "Sesamoid bones of foot")));
        Codes->insert(OFMake_pair(Shoulder, DSRBasicCodedEntry("16982005", "SCT", "Shoulder")));
        Codes->insert(OFMake_pair(Skull, DSRBasicCodedEntry("89546000", "SCT", "Skull")));
        Codes->insert(OFMake_pair(SmallIntestine, DSRBasicCodedEntry("30315005", "SCT", "Small intestine")));
        Codes->insert(OFMake_pair(Spine, DSRBasicCodedEntry("421060004", "SCT", "Spine")));
        Codes->insert(OFMake_pair(SternoclavicularJoint, DSRBasicCodedEntry("7844006", "SCT", "Sternoclavicular joint")));
        Codes->insert(OFMake_pair(Sternum, DSRBasicCodedEntry("56873002", "SCT", "Sternum")));
        Codes->insert(OFMake_pair(Stomach, DSRBasicCodedEntry("69695003", "SCT", "Stomach")));
        Codes->insert(OFMake_pair(SubmandibularGland, DSRBasicCodedEntry("54019009", "SCT", "Submandibular gland")));
        Codes->insert(OFMake_pair(TarsalJoint, DSRBasicCodedEntry("27949001", "SCT", "Tarsal joint")));
        Codes->insert(OFMake_pair(TemporomandibularJoint, DSRBasicCodedEntry("53620006", "SCT", "Temporomandibular joint")));
        Codes->insert(OFMake_pair(Thigh, DSRBasicCodedEntry("68367000", "SCT", "Thigh")));
        Codes->insert(OFMake_pair(ThoracicSpine, DSRBasicCodedEntry("122495006", "SCT", "Thoracic spine")));
        Codes->insert(OFMake_pair(ThoracoLumbarSpine, DSRBasicCodedEntry("297172009", "SCT", "Thoraco-lumbar spine")));
        Codes->insert(OFMake_pair(Thumb, DSRBasicCodedEntry("76505004", "SCT", "Thumb")));
        Codes->insert(OFMake_pair(Toe, DSRBasicCodedEntry("29707007", "SCT", "Toe")));
        Codes->insert(OFMake_pair(Trachea, DSRBasicCodedEntry("44567001", "SCT", "Trachea")));
        Codes->insert(OFMake_pair(UpperArm, DSRBasicCodedEntry("40983000", "SCT", "Upper arm")));
        Codes->insert(OFMake_pair(UpperLimb, DSRBasicCodedEntry("53120007", "SCT", "Upper limb")));
        Codes->insert(OFMake_pair(UpperUrinaryTract, DSRBasicCodedEntry("431491007", "SCT", "Upper urinary tract")));
        Codes->insert(OFMake_pair(Ureter, DSRBasicCodedEntry("87953007", "SCT", "Ureter")));
        Codes->insert(OFMake_pair(Urethra, DSRBasicCodedEntry("13648007", "SCT", "Urethra")));
        Codes->insert(OFMake_pair(UterusAndFallopianTubes, DSRBasicCodedEntry("110639002", "SCT", "Uterus and fallopian tubes")));
        Codes->insert(OFMake_pair(VertebralColumnAndCranium, DSRBasicCodedEntry("110517009", "SCT", "Vertebral column and cranium")));
        Codes->insert(OFMake_pair(WristJoint, DSRBasicCodedEntry("74670003", "SCT", "Wrist joint")));
        Codes->insert(OFMake_pair(Zygoma, DSRBasicCodedEntry("13881006", "SCT", "Zygoma")));
    }
    /* should never be NULL */
    return *Codes;
}


OFCondition CID4031_CommonAnatomicRegions::setEnhancedEncodingMode(DSRCodedEntryValue &codedEntryValue)
{
    return codedEntryValue.setEnhancedEncodingMode(CONTEXT_GROUP_NUMBER, "DCMR", CONTEXT_GROUP_VERSION, CONTEXT_GROUP_UID);
}
