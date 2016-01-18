/*
 *
 *  Copyright (C) 2015-2016, J. Riesmeier, Oldenburg, Germany
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  Header file for class CID4021_PETRadiopharmaceutical
 *
 *  Generated automatically from DICOM PS 3.16-2015c
 *  File created on 2016-01-16 12:26:36 by J. Riesmeier
 *
 */


#ifndef CMR_CID4021_H
#define CMR_CID4021_H

#include "dcmtk/config/osconfig.h"   /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/dsrctxgr.h"
#include "dcmtk/dcmsr/cmr/define.h"


/*---------------------*
 *  class declaration  *
 *---------------------*/

/** Implementation of DCMR Context Group:
 *  CID 4021 - PET Radiopharmaceutical.
 *  (type: extensible, version: 20141110)
 */
class DCMTK_CMR_EXPORT CID4021_PETRadiopharmaceutical
  : public DSRContextGroup
{

  public:

    /** types of PET radiopharmaceutical
     */
    enum EnumType
    {
        /// (126713,DCM,"2FA F^18^")
        _2FA_F18,
        /// (C-B1043,SRT,"Acetate C^11^")
        Acetate_C11,
        /// (C-B103C,SRT,"Ammonia N^13^")
        Ammonia_N13,
        /// (126700,DCM,"ATSM Cu^60^")
        ATSM_Cu60,
        /// (126701,DCM,"ATSM Cu^61^")
        ATSM_Cu61,
        /// (126702,DCM,"ATSM Cu^62^")
        ATSM_Cu62,
        /// (C-B07DB,SRT,"ATSM Cu^64^")
        ATSM_Cu64,
        /// (126516,DCM,"Bevacizumab ^89^Zr")
        Bevacizumab_89Zr,
        /// (C-B07DC,SRT,"Butanol O^15^")
        Butanol_O15,
        /// (C-B103B,SRT,"Carbon dioxide O^15^")
        CarbonDioxide_O15,
        /// (C-B1045,SRT,"Carbon monoxide C^11^")
        CarbonMonoxide_C11,
        /// (C-B103A,SRT,"Carbon monoxide O^15^")
        CarbonMonoxide_O15,
        /// (C-B103F,SRT,"Carfentanil C^11^")
        Carfentanil_C11,
        /// (126513,DCM,"Cetuximab ^89^Zr")
        Cetuximab_89Zr,
        /// (126703,DCM,"Choline C^11^")
        Choline_C11,
        /// (126517,DCM,"cG250-F(ab')(2) ^89^Zr")
        CG250Fab2_89Zr,
        /// (126715,DCM,"CLR1404 I^124^")
        CLR1404_I124,
        /// (126716,DCM,"CLR1404 I^131^")
        CLR1404_I131,
        /// (126515,DCM,"cU36 ^89^Zr")
        CU36_89Zr,
        /// (126520,DCM,"Df-CD45 ^89^Zr")
        DfCD45_89Zr,
        /// (126519,DCM,"E4G10 ^89^Zr")
        E4G10_89Zr,
        /// (C2713594,UMLS,"Edotreotide Ga^68^")
        Edotreotide_Ga68,
        /// (C-B07DD,SRT,"EDTA Ga^68^")
        EDTA_Ga68,
        /// (126704,DCM,"Fallypride C^11^")
        Fallypride_C11,
        /// (126705,DCM,"Fallypride F^18^")
        Fallypride_F18,
        /// (126706,DCM,"FLB 457 C^11^")
        FLB457_C11,
        /// (126503,DCM,"Flubatine F^18^")
        Flubatine_F18_126503,
        /// (126501,DCM,"Florbetaben F^18^")
        Florbetaben_F18,
        /// (C-E0269,SRT,"Florbetapir F^18^")
        Florbetapir_F18,
        /// (126712,DCM,"Flubatine F^18^")
        Flubatine_F18_126712,
        /// (C-E0265,SRT,"Fluciclatide F^18^")
        Fluciclatide_F18,
        /// (C-E026A,SRT,"Fluciclovine F^18^")
        Fluciclovine_F18,
        /// (C-B07DE,SRT,"Flumazenil C^11^")
        Flumazenil_C11,
        /// (C-B07DF,SRT,"Flumazenil F^18^")
        Flumazenil_F18,
        /// (C1831937,UMLS,"Fluoroestradiol (FES) F^18^")
        FluoroestradiolFES_F18,
        /// (C1541539,UMLS,"Fluoroetanidazole F^18^")
        Fluoroetanidazole_F18,
        /// (C-B07E0,SRT,"Fluorethyltyrosin F^18^")
        Fluorethyltyrosin_F18,
        /// (C-E0273,SRT,"Fluorocholine F^18^")
        Fluorocholine_F18,
        /// (C-B1031,SRT,"Fluorodeoxyglucose F^18^")
        Fluorodeoxyglucose_F18,
        /// (C-B07E1,SRT,"Fluoromisonidazole F^18^")
        Fluoromisonidazole_F18,
        /// (C-B07E2,SRT,"Fluoromethane F^18^")
        Fluoromethane_F18,
        /// (C2934038,UMLS,"Fluoropropyl-dihydrotetrabenazine (DTBZ) F^18^")
        FluoropropylDihydrotetrabenazineDTBZ_F18,
        /// (C-B07E3,SRT,"Fluorouracil F^18^")
        Fluorouracil_F18,
        /// (C-B07E4,SRT,"Fluorobenzothiazole F^18^")
        Fluorobenzothiazole_F18,
        /// (C-B1034,SRT,"Fluoro-L-dopa F^18^")
        FluoroLDopa_F18,
        /// (126707,DCM,"Fluorotriopride F^18^")
        Fluorotriopride_F18,
        /// (C-E0267,SRT,"Flutemetamol F^18^")
        Flutemetamol_F18,
        /// (C-B1046,SRT,"Germanium Ge^68^")
        Germanium_Ge68,
        /// (C-B103D,SRT,"Glutamate N^13^")
        Glutamate_N13,
        /// (126709,DCM,"Glutamine C^11^")
        Glutamine_C11,
        /// (126710,DCM,"Glutamine C^14^")
        Glutamine_C14,
        /// (126711,DCM,"Glutamine F^18^")
        Glutamine_F18,
        /// (C2981788,UMLS,"ISO-1 F^18^")
        ISO1_F18,
        /// (126514,DCM,"J591 ^89^Zr")
        J591_89Zr,
        /// (C-B07E5,SRT,"Mespiperone C^11^")
        Mespiperone_C11,
        /// (C-B103E,SRT,"Methionine C^11^")
        Methionine_C11,
        /// (C-B07E6,SRT,"Monoclonal antibody I^124^")
        MonoclonalAntibody_I124,
        /// (126510,DCM,"Monoclonal Antibody (mAb) ^64^Cu")
        MonoclonalAntibodymAb_64Cu,
        /// (126511,DCM,"Monoclonal Antibody (mAb) ^89^Zr")
        MonoclonalAntibodymAb_89Zr,
        /// (126714,DCM,"Nifene F^18^")
        Nifene_F18,
        /// (C-B1038,SRT,"Oxygen O^15^")
        Oxygen_O15,
        /// (C-B1039,SRT,"Oxygen-water O^15^")
        OxygenWater_O15,
        /// (C-B1044,SRT,"Palmitate C^11^")
        Palmitate_C11,
        /// (126500,DCM,"Pittsburgh compound B C^11^")
        PittsburghCompoundB_C11,
        /// (C-B07E7,SRT,"PTSM Cu^62^")
        PTSM_Cu62,
        /// (126518,DCM,"R1507 ^89^Zr")
        R1507_89Zr,
        /// (C-B1042,SRT,"Raclopride C^11^")
        Raclopride_C11,
        /// (C-B1037,SRT,"Rubidium chloride Rb^82^")
        RubidiumChloride_Rb82,
        /// (C-B1032,SRT,"Sodium fluoride F^18^")
        SodiumFluoride_F18,
        /// (C-B07E8,SRT,"Sodium iodide I^124^")
        SodiumIodide_I124,
        /// (C-B1047,SRT,"Sodium Na^22^")
        Sodium_Na22,
        /// (C-B1033,SRT,"Spiperone F^18^")
        Spiperone_F18,
        /// (126502,DCM,"T807 F^18^")
        T807_F18,
        /// (C-B1036,SRT,"Thymidine (FLT) F^18^")
        ThymidineFLT_F18,
        /// (126512,DCM,"Trastuzumab ^89^Zr")
        Trastuzumab_89Zr,
        /// (C1742831,UMLS,"tyrosine-3-octreotate Ga^68^")
        Tyrosine3Octreotate_Ga68
    };

    /** (default) constructor
     ** @param  selectedValue  coded entry to be selected as the current value (optional).
     *                         Should be a valid code according to the DICOM definitions
     *                         for this context group, i.e. no checks are performed.
     *                         Call DSRContextGroup::checkSelectedValue() if needed.
     */
    CID4021_PETRadiopharmaceutical(const DSRCodedEntryValue &selectedValue = DSRCodedEntryValue());

    /** constructor
     ** @param  selectedValue         type mapped to a coded entry that is selected as the
     *                                current value
     ** @param  enhancedEncodingMode  set enhanced encoding mode for coded entry (if enabled)
     */
    CID4021_PETRadiopharmaceutical(const EnumType selectedValue,
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
typedef CID4021_PETRadiopharmaceutical CMR_CID4021;


#endif
