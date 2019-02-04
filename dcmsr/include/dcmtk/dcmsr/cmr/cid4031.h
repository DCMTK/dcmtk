/*
 *
 *  Copyright (C) 2015-2019, J. Riesmeier, Oldenburg, Germany
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  Header file for class CID4031_CommonAnatomicRegions
 *
 *  Generated automatically from DICOM PS 3.16-2019a
 *  File created on 2019-02-04 12:56:26 by J. Riesmeier
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
 *  (type: extensible, version: 20170914)
 */
class DCMTK_CMR_EXPORT CID4031_CommonAnatomicRegions
  : public DSRContextGroup
{

  public:

    /** types of common anatomic regions
     */
    enum EnumType
    {
        /// (T-D4000,SRT,"Abdomen")
        Abdomen,
        /// (R-FAB57,SRT,"Abdomen and Pelvis")
        AbdomenAndPelvis,
        /// (T-15420,SRT,"Acromioclavicular joint")
        AcromioclavicularJoint,
        /// (T-15750,SRT,"Ankle joint")
        AnkleJoint,
        /// (T-59900,SRT,"Anus")
        Anus,
        /// (T-280A0,SRT,"Apex of Lung")
        ApexOfLung,
        /// (T-60610,SRT,"Bile duct")
        BileDuct,
        /// (T-74000,SRT,"Bladder")
        Bladder,
        /// (T-12700,SRT,"Bone of lower limb")
        BoneOfLowerLimb,
        /// (T-D0821,SRT,"Bone of upper limb")
        BoneOfUpperLimb,
        /// (T-04000,SRT,"Breast")
        Breast,
        /// (T-26000,SRT,"Bronchus")
        Bronchus,
        /// (T-12770,SRT,"Calcaneus")
        Calcaneus,
        /// (T-11501,SRT,"Cervical spine")
        CervicalSpine,
        /// (T-D00F7,SRT,"Cervico-thoracic spine")
        CervicoThoracicSpine,
        /// (T-D3000,SRT,"Chest")
        Chest,
        /// (R-FAB55,SRT,"Chest and Abdomen")
        ChestAndAbdomen,
        /// (R-FAB56,SRT,"Chest, Abdomen and Pelvis")
        ChestAbdomenAndPelvis,
        /// (T-12310,SRT,"Clavicle")
        Clavicle,
        /// (T-11BF0,SRT,"Coccyx")
        Coccyx,
        /// (T-59300,SRT,"Colon")
        Colon,
        /// (T-58200,SRT,"Duodenum")
        Duodenum,
        /// (T-15430,SRT,"Elbow joint")
        ElbowJoint,
        /// (T-D0010,SRT,"Entire body")
        EntireBody,
        /// (T-56000,SRT,"Esophagus")
        Esophagus,
        /// (T-DD163,SRT,"Esophagus, stomach and duodenum")
        EsophagusStomachAndDuodenum,
        /// (T-D0300,SRT,"Extremity")
        Extremity,
        /// (T-AA000,SRT,"Eye")
        Eye,
        /// (T-D0801,SRT,"Eye region")
        EyeRegion,
        /// (T-11196,SRT,"Facial bones")
        FacialBones,
        /// (T-12710,SRT,"Femur")
        Femur,
        /// (T-12750,SRT,"Fibula")
        Fibula,
        /// (T-D8800,SRT,"Finger")
        Finger,
        /// (T-D9700,SRT,"Foot")
        Foot,
        /// (T-D8500,SRT,"Forearm")
        Forearm,
        /// (T-63000,SRT,"Gallbladder")
        Gallbladder,
        /// (T-D8700,SRT,"Hand")
        Hand,
        /// (T-D1100,SRT,"Head")
        Head,
        /// (T-D1000,SRT,"Head and Neck")
        HeadAndNeck,
        /// (T-32000,SRT,"Heart")
        Heart,
        /// (T-15710,SRT,"Hip joint")
        HipJoint,
        /// (T-12410,SRT,"Humerus")
        Humerus,
        /// (T-58600,SRT,"Ileum")
        Ileum,
        /// (T-12340,SRT,"Ilium")
        Ilium,
        /// (T-AB959,SRT,"Internal Auditory Canal")
        InternalAuditoryCanal,
        /// (T-D1213,SRT,"Jaw region")
        JawRegion,
        /// (T-58400,SRT,"Jejunum")
        Jejunum,
        /// (T-D9200,SRT,"Knee")
        Knee,
        /// (T-59000,SRT,"Large intestine")
        LargeIntestine,
        /// (T-24100,SRT,"Larynx")
        Larynx,
        /// (T-D9400,SRT,"Lower leg")
        LowerLeg,
        /// (T-D9000,SRT,"Lower limb")
        LowerLimb,
        /// (T-11503,SRT,"Lumbar spine")
        LumbarSpine,
        /// (T-D00F9,SRT,"Lumbo-sacral spine")
        LumboSacralSpine,
        /// (T-11180,SRT,"Mandible")
        Mandible,
        /// (T-11133,SRT,"Mastoid bone")
        MastoidBone,
        /// (T-11170,SRT,"Maxilla")
        Maxilla,
        /// (T-D3300,SRT,"Mediastinum")
        Mediastinum,
        /// (T-14668,SRT,"Muscle of lower limb")
        MuscleOfLowerLimb,
        /// (T-13600,SRT,"Muscle of upper limb")
        MuscleOfUpperLimb,
        /// (T-11149,SRT,"Nasal bone")
        NasalBone,
        /// (T-D1600,SRT,"Neck")
        Neck,
        /// (R-FAB52,SRT,"Neck and Chest")
        NeckAndChest,
        /// (R-FAB53,SRT,"Neck, Chest and Abdomen")
        NeckChestAndAbdomen,
        /// (R-FAB54,SRT,"Neck, Chest, Abdomen and Pelvis")
        NeckChestAbdomenAndPelvis,
        /// (T-11102,SRT,"Optic canal")
        OpticCanal,
        /// (T-D14AE,SRT,"Orbital structure")
        OrbitalStructure,
        /// (T-65600,SRT,"Pancreatic duct and bile duct systems")
        PancreaticDuctAndBileDuctSystems,
        /// (T-22000,SRT,"Paranasal sinus")
        ParanasalSinus,
        /// (T-61100,SRT,"Parotid gland")
        ParotidGland,
        /// (T-12730,SRT,"Patella")
        Patella,
        /// (T-D6000,SRT,"Pelvis")
        Pelvis,
        /// (R-FAB58,SRT,"Pelvis and lower extremities")
        PelvisAndLowerExtremities,
        /// (113681,DCM,"Phantom")
        Phantom,
        /// (T-92000,SRT,"Prostate")
        Prostate,
        /// (T-59600,SRT,"Rectum")
        Rectum,
        /// (T-11300,SRT,"Rib")
        Rib,
        /// (T-15680,SRT,"Sacroiliac joint")
        SacroiliacJoint,
        /// (T-11AD0,SRT,"Sacrum")
        Sacrum,
        /// (T-12280,SRT,"Scapula")
        Scapula,
        /// (T-D1460,SRT,"Sella turcica")
        SellaTurcica,
        /// (T-12980,SRT,"Sesamoid bones of foot")
        SesamoidBonesOfFoot,
        /// (T-D2220,SRT,"Shoulder")
        Shoulder,
        /// (T-11100,SRT,"Skull")
        Skull,
        /// (T-58000,SRT,"Small intestine")
        SmallIntestine,
        /// (T-D04FF,SRT,"Spine")
        Spine,
        /// (T-15610,SRT,"Sternoclavicular joint")
        SternoclavicularJoint,
        /// (T-11210,SRT,"Sternum")
        Sternum,
        /// (T-57000,SRT,"Stomach")
        Stomach,
        /// (T-61300,SRT,"Submandibular gland")
        SubmandibularGland,
        /// (T-15770,SRT,"Tarsal joint")
        TarsalJoint,
        /// (T-15290,SRT,"Temporomandibular joint")
        TemporomandibularJoint,
        /// (T-D9100,SRT,"Thigh")
        Thigh,
        /// (T-11502,SRT,"Thoracic spine")
        ThoracicSpine,
        /// (T-D00F8,SRT,"Thoraco-lumbar spine")
        ThoracoLumbarSpine,
        /// (T-D8810,SRT,"Thumb")
        Thumb,
        /// (T-D9800,SRT,"Toe")
        Toe,
        /// (T-25000,SRT,"Trachea")
        Trachea,
        /// (T-D8200,SRT,"Upper arm")
        UpperArm,
        /// (T-D8000,SRT,"Upper limb")
        UpperLimb,
        /// (T-7000B,SRT,"Upper urinary tract")
        UpperUrinaryTract,
        /// (T-73000,SRT,"Ureter")
        Ureter,
        /// (T-75000,SRT,"Urethra")
        Urethra,
        /// (T-88920,SRT,"Uterus and fallopian tubes")
        UterusAndFallopianTubes,
        /// (T-11011,SRT,"Vertebral column and cranium")
        VertebralColumnAndCranium,
        /// (T-15460,SRT,"Wrist joint")
        WristJoint,
        /// (T-11166,SRT,"Zygoma")
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
