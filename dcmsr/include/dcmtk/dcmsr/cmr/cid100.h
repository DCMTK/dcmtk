/*
 *
 *  Copyright (C) 2015-2021, J. Riesmeier, Oldenburg, Germany
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  Header file for class CID100_QuantitativeDiagnosticImagingProcedures
 *
 *  Generated automatically from DICOM PS 3.16-2021a
 *  File created on 2021-02-03 21:06:15 by J. Riesmeier
 *
 */


#ifndef CMR_CID100_H
#define CMR_CID100_H

#include "dcmtk/config/osconfig.h"   /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/dsrctxgr.h"
#include "dcmtk/dcmsr/cmr/define.h"

#include "dcmtk/ofstd/ofmap.h"


/*---------------------*
 *  class declaration  *
 *---------------------*/

/** Implementation of DCMR Context Group:
 *  CID 100 - Quantitative Diagnostic Imaging Procedures.
 *  (type: extensible, version: 20190817)
 */
class DCMTK_CMR_EXPORT CID100_QuantitativeDiagnosticImagingProcedures
  : public DSRContextGroup
{

  public:

    /** types of quantitative diagnostic imaging procedures
     */
    enum EnumType
    {
        /// (363679005,SCT,"Imaging procedure")
        ImagingProcedure,
        /// (258177008,SCT,"Magnetic resonance imaging guidance")
        MagneticResonanceImagingGuidance,
        /// (126020,DCM,"Multiparametric MRI")
        MultiparametricMRI,
        /// (126021,DCM,"Multiparametric MRI of prostate")
        MultiparametricMRIOfProstate,
        /// (126022,DCM,"Multiparametric MRI of whole body")
        MultiparametricMRIOfWholeBody,
        /// (433139009,SCT,"Dynamic magnetic resonance imaging of knee")
        DynamicMagneticResonanceImagingOfKnee,
        /// (446315002,SCT,"Dynamic magnetic resonance imaging of pelvis")
        DynamicMagneticResonanceImagingOfPelvis,
        /// (25045-6,LN,"CT unspecified body region")
        CTUnspecifiedBodyRegion,
        /// (25056-3,LN,"MRI unspecified body region")
        MRIUnspecifiedBodyRegion,
        /// (49118-3,LN,"NM unspecified body region")
        NMUnspecifiedBodyRegion,
        /// (44136-0,LN,"PET unspecified body region")
        PETUnspecifiedBodyRegion,
        /// (44139-4,LN,"PET whole body")
        PETWholeBody,
        /// (443271005,SCT,"PET/CT FDG imaging of whole body")
        PETCT_FDGImagingOfWholeBody,
        /// (764704008,SCT,"PET/CT MET imaging of whole body")
        PETCT_METImagingOfWholeBody,
        /// (39142-5,LN,"CT perfusion head with contrast IV")
        CTPerfusionHeadWithContrastIV,
        /// (39632-5,LN,"SPECT brain")
        SPECTBrain,
        /// (RPID5427,RADLEX,"NM head perfusion brain PET-CT AV-45")
        NMHeadPerfusionBrainPET_CT_AV45
    };

    /** (default) constructor
     ** @param  selectedValue  coded entry to be selected as the current value (optional).
     *                         Should be a valid code according to the DICOM definitions
     *                         for this context group, i.e. no checks are performed.
     *                         Call DSRContextGroup::checkSelectedValue() if needed.
     */
    CID100_QuantitativeDiagnosticImagingProcedures(const DSRCodedEntryValue &selectedValue = DSRCodedEntryValue());

    /** constructor
     ** @param  selectedValue         type mapped to a coded entry that is selected as the
     *                                current value
     ** @param  enhancedEncodingMode  set enhanced encoding mode for coded entry (if enabled)
     */
    CID100_QuantitativeDiagnosticImagingProcedures(const EnumType selectedValue,
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
typedef CID100_QuantitativeDiagnosticImagingProcedures CMR_CID100;


#endif
