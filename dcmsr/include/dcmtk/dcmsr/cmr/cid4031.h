/*
 *
 *  Copyright (C) 2015-2022, J. Riesmeier, Oldenburg, Germany
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  Header file for class CID4031_CommonAnatomicRegions
 *
 *  Generated automatically from DICOM PS 3.16-2022b
 *  File created on 2022-04-12 13:01:14 by J. Riesmeier
 *
 */


#ifndef CMR_CID4031_H
#define CMR_CID4031_H

#include "dcmtk/config/osconfig.h"   /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/dsrctxgr.h"
#include "dcmtk/dcmsr/cmr/define.h"

#include "dcmtk/ofstd/ofmap.h"


/*---------------------*
 *  class declaration  *
 *---------------------*/

/** Implementation of DCMR Context Group:
 *  CID 4031 - Common Anatomic Regions.
 *  (type: extensible, version: 20210120)
 */
class DCMTK_CMR_EXPORT CID4031_CommonAnatomicRegions
  : public DSRContextGroup
{

  public:

    /** types of common anatomic regions
     */
    enum EnumType
    {
        /// (818981001,SCT,"Abdomen")
        Abdomen,
        /// (818982008,SCT,"Abdomen and Pelvis")
        AbdomenAndPelvis,
        /// (85856004,SCT,"Acromioclavicular joint")
        AcromioclavicularJoint,
        /// (70258002,SCT,"Ankle joint")
        AnkleJoint,
        /// (53505006,SCT,"Anus")
        Anus,
        /// (86598002,SCT,"Apex of Lung")
        ApexOfLung,
        /// (28273000,SCT,"Bile duct")
        BileDuct,
        /// (34707002,SCT,"Biliary tract")
        BiliaryTract,
        /// (89837001,SCT,"Bladder")
        Bladder,
        /// (72001000,SCT,"Bone of lower limb")
        BoneOfLowerLimb,
        /// (371195002,SCT,"Bone of upper limb")
        BoneOfUpperLimb,
        /// (76752008,SCT,"Breast")
        Breast,
        /// (955009,SCT,"Bronchus")
        Bronchus,
        /// (80144004,SCT,"Calcaneus")
        Calcaneus,
        /// (122494005,SCT,"Cervical spine")
        CervicalSpine,
        /// (297171002,SCT,"Cervico-thoracic spine")
        CervicoThoracicSpine,
        /// (816094009,SCT,"Chest")
        Chest,
        /// (416550000,SCT,"Chest and Abdomen")
        ChestAndAbdomen,
        /// (416775004,SCT,"Chest, Abdomen and Pelvis")
        ChestAbdomenAndPelvis,
        /// (51299004,SCT,"Clavicle")
        Clavicle,
        /// (64688005,SCT,"Coccyx")
        Coccyx,
        /// (71854001,SCT,"Colon")
        Colon,
        /// (79741001,SCT,"Common bile duct")
        CommonBileDuct,
        /// (38848004,SCT,"Duodenum")
        Duodenum,
        /// (16953009,SCT,"Elbow joint")
        ElbowJoint,
        /// (38266002,SCT,"Entire body")
        EntireBody,
        /// (32849002,SCT,"Esophagus")
        Esophagus,
        /// (110861005,SCT,"Esophagus, stomach and duodenum")
        EsophagusStomachAndDuodenum,
        /// (66019005,SCT,"Extremity")
        Extremity,
        /// (81745001,SCT,"Eye")
        Eye,
        /// (371398005,SCT,"Eye region")
        EyeRegion,
        /// (91397008,SCT,"Facial bones")
        FacialBones,
        /// (71341001,SCT,"Femur")
        Femur,
        /// (87342007,SCT,"Fibula")
        Fibula,
        /// (7569003,SCT,"Finger")
        Finger,
        /// (56459004,SCT,"Foot")
        Foot,
        /// (14975008,SCT,"Forearm")
        Forearm,
        /// (28231008,SCT,"Gallbladder")
        Gallbladder,
        /// (85562004,SCT,"Hand")
        Hand,
        /// (69536005,SCT,"Head")
        Head,
        /// (774007,SCT,"Head and Neck")
        HeadAndNeck,
        /// (80891009,SCT,"Heart")
        Heart,
        /// (29836001,SCT,"Hip")
        Hip,
        /// (24136001,SCT,"Hip Joint")
        HipJoint,
        /// (85050009,SCT,"Humerus")
        Humerus,
        /// (34516001,SCT,"Ileum")
        Ileum,
        /// (22356005,SCT,"Ilium")
        Ilium,
        /// (361078006,SCT,"Internal Auditory Canal")
        InternalAuditoryCanal,
        /// (661005,SCT,"Jaw region")
        JawRegion,
        /// (21306003,SCT,"Jejunum")
        Jejunum,
        /// (72696002,SCT,"Knee")
        Knee,
        /// (14742008,SCT,"Large intestine")
        LargeIntestine,
        /// (4596009,SCT,"Larynx")
        Larynx,
        /// (303270005,SCT,"Liver and biliary structure")
        LiverAndBiliaryStructure,
        /// (30021000,SCT,"Lower leg")
        LowerLeg,
        /// (61685007,SCT,"Lower limb")
        LowerLimb,
        /// (122496007,SCT,"Lumbar spine")
        LumbarSpine,
        /// (297173004,SCT,"Lumbo-sacral spine")
        LumboSacralSpine,
        /// (91609006,SCT,"Mandible")
        Mandible,
        /// (59066005,SCT,"Mastoid bone")
        MastoidBone,
        /// (70925003,SCT,"Maxilla")
        Maxilla,
        /// (72410000,SCT,"Mediastinum")
        Mediastinum,
        /// (102292000,SCT,"Muscle of lower limb")
        MuscleOfLowerLimb,
        /// (30608006,SCT,"Muscle of upper limb")
        MuscleOfUpperLimb,
        /// (74386004,SCT,"Nasal bone")
        NasalBone,
        /// (45048000,SCT,"Neck")
        Neck,
        /// (417437006,SCT,"Neck and Chest")
        NeckAndChest,
        /// (416152001,SCT,"Neck, Chest and Abdomen")
        NeckChestAndAbdomen,
        /// (416319003,SCT,"Neck, Chest, Abdomen and Pelvis")
        NeckChestAbdomenAndPelvis,
        /// (55024004,SCT,"Optic canal")
        OpticCanal,
        /// (363654007,SCT,"Orbital structure")
        OrbitalStructure,
        /// (15776009,SCT,"Pancreas")
        Pancreas,
        /// (69930009,SCT,"Pancreatic duct")
        PancreaticDuct,
        /// (110621006,SCT,"Pancreatic duct and bile duct systems")
        PancreaticDuctAndBileDuctSystems,
        /// (2095001,SCT,"Paranasal sinus")
        ParanasalSinus,
        /// (45289007,SCT,"Parotid gland")
        ParotidGland,
        /// (64234005,SCT,"Patella")
        Patella,
        /// (816092008,SCT,"Pelvis")
        Pelvis,
        /// (416631005,SCT,"Pelvis and lower extremities")
        PelvisAndLowerExtremities,
        /// (113681,DCM,"Phantom")
        Phantom,
        /// (41216001,SCT,"Prostate")
        Prostate,
        /// (34402009,SCT,"Rectum")
        Rectum,
        /// (113197003,SCT,"Rib")
        Rib,
        /// (297174005,SCT,"Sacro-coccygeal Spine")
        SacroCoccygealSpine,
        /// (39723000,SCT,"Sacroiliac joint")
        SacroiliacJoint,
        /// (54735007,SCT,"Sacrum")
        Sacrum,
        /// (79601000,SCT,"Scapula")
        Scapula,
        /// (42575006,SCT,"Sella turcica")
        SellaTurcica,
        /// (58742003,SCT,"Sesamoid bones of foot")
        SesamoidBonesOfFoot,
        /// (16982005,SCT,"Shoulder")
        Shoulder,
        /// (89546000,SCT,"Skull")
        Skull,
        /// (30315005,SCT,"Small intestine")
        SmallIntestine,
        /// (421060004,SCT,"Spine")
        Spine,
        /// (7844006,SCT,"Sternoclavicular joint")
        SternoclavicularJoint,
        /// (56873002,SCT,"Sternum")
        Sternum,
        /// (69695003,SCT,"Stomach")
        Stomach,
        /// (54019009,SCT,"Submandibular gland")
        SubmandibularGland,
        /// (27949001,SCT,"Tarsal joint")
        TarsalJoint,
        /// (53620006,SCT,"Temporomandibular joint")
        TemporomandibularJoint,
        /// (68367000,SCT,"Thigh")
        Thigh,
        /// (122495006,SCT,"Thoracic spine")
        ThoracicSpine,
        /// (297172009,SCT,"Thoraco-lumbar spine")
        ThoracoLumbarSpine,
        /// (76505004,SCT,"Thumb")
        Thumb,
        /// (29707007,SCT,"Toe")
        Toe,
        /// (44567001,SCT,"Trachea")
        Trachea,
        /// (40983000,SCT,"Upper arm")
        UpperArm,
        /// (53120007,SCT,"Upper limb")
        UpperLimb,
        /// (431491007,SCT,"Upper urinary tract")
        UpperUrinaryTract,
        /// (87953007,SCT,"Ureter")
        Ureter,
        /// (13648007,SCT,"Urethra")
        Urethra,
        /// (110639002,SCT,"Uterus and fallopian tubes")
        UterusAndFallopianTubes,
        /// (110517009,SCT,"Vertebral column and cranium")
        VertebralColumnAndCranium,
        /// (74670003,SCT,"Wrist joint")
        WristJoint,
        /// (13881006,SCT,"Zygoma")
        Zygoma
    };

    /** (default) constructor
     ** @param  selectedValue  coded entry to be selected as the current value (optional).
     *                         Should be a valid code according to the DICOM definitions
     *                         for this context group, i.e. no checks are performed.
     *                         Call DSRContextGroup::checkSelectedValue() if needed.
     */
    CID4031_CommonAnatomicRegions(const DSRCodedEntryValue &selectedValue = DSRCodedEntryValue());

    /** constructor
     ** @param  selectedValue         type mapped to a coded entry that is selected as the
     *                                current value
     ** @param  enhancedEncodingMode  set enhanced encoding mode for coded entry (if enabled)
     */
    CID4031_CommonAnatomicRegions(const EnumType selectedValue,
                                  const OFBool enhancedEncodingMode = OFFalse);

    /** select a coded entry given by its type as the current value
     ** @param  selectedValue         type mapped to a coded entry that is selected as the
     *                                current value
     *  @param  enhancedEncodingMode  set enhanced encoding mode for coded entry (if enabled)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition selectValue(const EnumType selectedValue,
                                    const OFBool enhancedEncodingMode = OFFalse);

  // --- static helper functions ---

    /** initialize this context group explicitly.  Internally, the list of standard coded
     *  entries is created and initialized by calling getCodes().
     ** @note This function can be called multiple times but in case of multi-threaded
     *        applications should be called at least once before any instance of this class
     *        is actually used.  For single-threaded applications, there is no need to call
     *        it since the initialization is done implicitly.
     */
    static void initialize();

    /** cleanup this context group, i.e.\ delete the internal list of standard coded entries.
     *  Usually, there is no need to call this method, but it might be useful in order to
     *  explicitly free the associated memory, e.g. when checking for memory leaks.  The list
     *  will be recreated automatically when needed (or when initialize() is called).
     */
    static void cleanup();

    /** map a given type to the associated coded entry
     ** @param  value                 type that should be mapped to a coded entry
     *  @param  enhancedEncodingMode  set enhanced encoding mode for coded entry (if enabled)
     ** @return coded entry that is associated with the given type
     */
    static DSRCodedEntryValue getCodedEntry(const EnumType value,
                                            const OFBool enhancedEncodingMode = OFFalse);

  // --- reintroduce methods from base class

     using DSRContextGroup::selectValue;
     using DSRContextGroup::findCodedEntry;


  protected:

    /// type used for storing and managing the coded entries
    typedef OFMap<EnumType, DSRBasicCodedEntry> CodeList;

    /** print details on coded entries that are contained in this context group.
     *  See DSRCodedEntryValue::print() for details of the coded entry output.
     ** @param  stream  stream to which the output should be printed
     */
    virtual void printCodes(STD_NAMESPACE ostream &stream) const;

    /** search for a given coded entry in this context group
     ** @param  searchForCodedEntry   coded entry to be searched for
     *  @param  foundCodedEntry       pointer to variable that will store the coded entry
     *                                (if found and pointer is not NULL)
     *  @param  enhancedEncodingMode  set enhanced encoding mode for coded entry (if enabled)
     ** @return result of the search process, also defines the type of the entry
     */
    virtual OFCondition findCodedEntry(const DSRCodedEntryValue &searchForCodedEntry,
                                       DSRCodedEntryValue *foundCodedEntry,
                                       const OFBool enhancedEncodingMode = OFFalse) const;

  // --- static helper function ---

    /** get list of standard coded entries that are managed by this context group.
     *  Internally, the singleton pattern is used, so the list is initialized only once.
     *  Please note that this function is not thread-safe.  Therefore, the initialize()
     *  function can be called before any instance of this class is actually used.
     ** @return reference to list of coded entries managed by this context group
     */
    static CodeList &getCodes();

    /** set the "Enhanced Encoding Mode" for a given coded entry
     ** @param  codedEntryValue  coded entry for which the enhanced encoding mode is set
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    static OFCondition setEnhancedEncodingMode(DSRCodedEntryValue &codedEntryValue);


  private:

    /// pointer to internal code list (use a static variable for singleton pattern)
    static CodeList *Codes;
};


/*-------------------*
 *  type definition  *
 *-------------------*/

// define short name for the context group class
typedef CID4031_CommonAnatomicRegions CMR_CID4031;


#endif
