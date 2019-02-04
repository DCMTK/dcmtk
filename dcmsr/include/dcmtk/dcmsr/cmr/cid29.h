/*
 *
 *  Copyright (C) 2015-2019, J. Riesmeier, Oldenburg, Germany
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  Header file for class CID29_AcquisitionModality
 *
 *  Generated automatically from DICOM PS 3.16-2019a
 *  File created on 2019-02-04 12:56:14 by J. Riesmeier
 *
 */


#ifndef CMR_CID29_H
#define CMR_CID29_H

#include "dcmtk/config/osconfig.h"   /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/dsrctxgr.h"
#include "dcmtk/dcmsr/cmr/define.h"

#include "dcmtk/ofstd/ofmap.h"


/*---------------------*
 *  class declaration  *
 *---------------------*/

/** Implementation of DCMR Context Group:
 *  CID 29 - Acquisition Modality.
 *  (type: extensible, version: 20180605)
 */
class DCMTK_CMR_EXPORT CID29_AcquisitionModality
  : public DSRContextGroup
{

  public:

    /** types of acquisition modality
     */
    enum EnumType
    {
        /// (AR,DCM,"Autorefraction")
        Autorefraction,
        /// (BMD,DCM,"Bone Mineral Densitometry")
        BoneMineralDensitometry,
        /// (BDUS,DCM,"Ultrasound Bone Densitometry")
        UltrasoundBoneDensitometry,
        /// (EPS,DCM,"Cardiac Electrophysiology")
        CardiacElectrophysiology,
        /// (CR,DCM,"Computed Radiography")
        ComputedRadiography,
        /// (CT,DCM,"Computed Tomography")
        ComputedTomography,
        /// (DX,DCM,"Digital Radiography")
        DigitalRadiography,
        /// (ECG,DCM,"Electrocardiography")
        Electrocardiography,
        /// (ES,DCM,"Endoscopy")
        Endoscopy,
        /// (XC,DCM,"External-camera Photography")
        ExternalCameraPhotography,
        /// (GM,DCM,"General Microscopy")
        GeneralMicroscopy,
        /// (HD,DCM,"Hemodynamic Waveform")
        HemodynamicWaveform,
        /// (IO,DCM,"Intra-oral Radiography")
        IntraOralRadiography,
        /// (IVOCT,DCM,"Intravascular Optical Coherence Tomography")
        IntravascularOpticalCoherenceTomography,
        /// (IVUS,DCM,"Intravascular Ultrasound")
        IntravascularUltrasound,
        /// (KER,DCM,"Keratometry")
        Keratometry,
        /// (LEN,DCM,"Lensometry")
        Lensometry,
        /// (MR,DCM,"Magnetic Resonance")
        MagneticResonance,
        /// (MG,DCM,"Mammography")
        Mammography,
        /// (NM,DCM,"Nuclear Medicine")
        NuclearMedicine,
        /// (OAM,DCM,"Ophthalmic Axial Measurements")
        OphthalmicAxialMeasurements,
        /// (OCT,DCM,"Optical Coherence Tomography")
        OpticalCoherenceTomography,
        /// (OPM,DCM,"Ophthalmic Mapping")
        OphthalmicMapping,
        /// (OP,DCM,"Ophthalmic Photography")
        OphthalmicPhotography,
        /// (OPR,DCM,"Ophthalmic Refraction")
        OphthalmicRefraction,
        /// (OPT,DCM,"Ophthalmic Tomography")
        OphthalmicTomography,
        /// (OPTBSV,DCM,"Ophthalmic Tomography B-scan Volume Analysis")
        OphthalmicTomographyBScanVolumeAnalysis,
        /// (OPTENF,DCM,"Ophthalmic Tomography En Face")
        OphthalmicTomographyEnFace,
        /// (OPV,DCM,"Ophthalmic Visual Field")
        OphthalmicVisualField,
        /// (OSS,DCM,"Optical Surface Scanner")
        OpticalSurfaceScanner,
        /// (PX,DCM,"Panoramic X-Ray")
        PanoramicXRay,
        /// (PT,DCM,"Positron emission tomography")
        PositronEmissionTomography,
        /// (RF,DCM,"Radiofluoroscopy")
        Radiofluoroscopy,
        /// (RG,DCM,"Radiographic imaging")
        RadiographicImaging,
        /// (SM,DCM,"Slide Microscopy")
        SlideMicroscopy,
        /// (SRF,DCM,"Subjective Refraction")
        SubjectiveRefraction,
        /// (US,DCM,"Ultrasound")
        Ultrasound,
        /// (VA,DCM,"Visual Acuity")
        VisualAcuity,
        /// (XA,DCM,"X-Ray Angiography")
        XRayAngiography
    };

    /** (default) constructor
     ** @param  selectedValue  coded entry to be selected as the current value (optional).
     *                         Should be a valid code according to the DICOM definitions
     *                         for this context group, i.e. no checks are performed.
     *                         Call DSRContextGroup::checkSelectedValue() if needed.
     */
    CID29_AcquisitionModality(const DSRCodedEntryValue &selectedValue = DSRCodedEntryValue());

    /** constructor
     ** @param  selectedValue         type mapped to a coded entry that is selected as the
     *                                current value
     ** @param  enhancedEncodingMode  set enhanced encoding mode for coded entry (if enabled)
     */
    CID29_AcquisitionModality(const EnumType selectedValue,
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
typedef CID29_AcquisitionModality CMR_CID29;


#endif
