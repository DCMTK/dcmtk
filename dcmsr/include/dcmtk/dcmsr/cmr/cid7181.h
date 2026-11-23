/*
 *
 *  Copyright (C) 2015-2026, J. Riesmeier, Oldenburg, Germany
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  Header file for class CID7181_AbstractMultiDimensionalImageModelComponentUnit
 *
 *  Generated automatically from DICOM PS 3.16-2026e
 *  File created on 2026-11-23 13:08:44 by J. Riesmeier
 *
 */


#ifndef CMR_CID7181_H
#define CMR_CID7181_H

#include "dcmtk/config/osconfig.h"   /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/dsrctxgr.h"
#include "dcmtk/dcmsr/cmr/define.h"

#include "dcmtk/ofstd/ofmap.h"


/*---------------------*
 *  class declaration  *
 *---------------------*/

/** Implementation of DCMR Context Group:
 *  CID 7181 - Abstract Multi-dimensional Image Model Component Unit.
 *  (type: extensible, version: 20261115)
 */
class DCMTK_CMR_EXPORT CID7181_AbstractMultiDimensionalImageModelComponentUnit
  : public DSRContextGroup
{

  public:

    /** types of abstract multi-dimensional image model component unit
     */
    enum EnumType
    {
        /// (mm[Hg],UCUM,"mmHg"), included from CID 3500
        mmHg,
        /// (kPa,UCUM,"kPa"), included from CID 3500
        kPa,
        /// ([PRU],UCUM,"P.R.U."), included from CID 3502
        PRU,
        /// ([wood'U],UCUM,"Wood U"), included from CID 3502
        WoodU,
        /// (dyn.s.cm-5,UCUM,"dyn.s.cm-5"), included from CID 3502
        dynscm5,
        /// ([PRU]/m2,UCUM,"P.R.U./m2"), included from CID 3503
        PRUPerM2,
        /// ([wood'U]/m2,UCUM,"Wood U/m2"), included from CID 3503
        WoodUPerM2,
        /// (dyn.s.cm-5/m2,UCUM,"dyn.s.cm-5/m2"), included from CID 3503
        dynscm5PerM2,
        /// (cm,UCUM,"cm"), included from CID 7460
        cm,
        /// (mm,UCUM,"mm"), included from CID 7460
        mm,
        /// (um,UCUM,"um"), included from CID 7460
        um,
        /// (cm2,UCUM,"cm2"), included from CID 7461
        cm2,
        /// (mm2,UCUM,"mm2"), included from CID 7461
        mm2,
        /// (um2,UCUM,"um2"), included from CID 7461
        um2,
        /// (dm3,UCUM,"dm3"), included from CID 7462
        dm3,
        /// (cm3,UCUM,"cm3"), included from CID 7462
        cm3,
        /// (mm3,UCUM,"mm3"), included from CID 7462
        mm3,
        /// (um3,UCUM,"um3"), included from CID 7462
        um3,
        /// (g/ml{SUVbw},UCUM,"Standardized Uptake Value body weight"), included from CID 85
        StandardizedUptakeValueBodyWeight,
        /// (g/ml{SUVlbm},UCUM,"Standardized Uptake Value lean body mass (James)"), included from CID 85
        StandardizedUptakeValueLeanBodyMassJames,
        /// (g/ml{SUVlbm(James128)},UCUM,"Standardized Uptake Value lean body mass (James 128 multiplier)"), included from CID 85
        StandardizedUptakeValueLeanBodyMassJames128Multiplier,
        /// (g/ml{SUVlbm(Janma)},UCUM,"Standardized Uptake Value lean body mass (Janma)"), included from CID 85
        StandardizedUptakeValueLeanBodyMassJanma,
        /// (cm2/ml{SUVbsa},UCUM,"Standardized Uptake Value body surface area"), included from CID 85
        StandardizedUptakeValueBodySurfaceArea,
        /// (g/ml{SUVibw},UCUM,"Standardized Uptake Value ideal body weight"), included from CID 85
        StandardizedUptakeValueIdealBodyWeight,
        /// ({counts},UCUM,"counts"), included from CID 84
        counts,
        /// ({counts}/s,UCUM,"counts/s"), included from CID 84
        countsPerS,
        /// ({propcounts},UCUM,"Proportional to counts"), included from CID 84
        ProportionalToCounts,
        /// ({propcounts}/s,UCUM,"Proportional to counts per second"), included from CID 84
        ProportionalToCountsPerSecond,
        /// (cm2/ml,UCUM,"cm2/ml"), included from CID 84
        cm2PerMl,
        /// (%,UCUM,"%"), included from CID 84
        Percent,
        /// (Bq/ml,UCUM,"Bq/ml"), included from CID 84
        BqPerMl,
        /// (mg/min/ml,UCUM,"mg/min/ml"), included from CID 84
        mgPerMinPerMl,
        /// (umol/min/ml,UCUM,"umol/min/ml"), included from CID 84
        umolPerMinPerMl,
        /// (ml/min/g,UCUM,"ml/min/g"), included from CID 84
        mlPerMinPerG,
        /// (ml/g,UCUM,"ml/g"), included from CID 84
        mlPerG,
        /// (/cm,UCUM,"/cm"), included from CID 84
        PerCm,
        /// (umol/ml,UCUM,"umol/ml"), included from CID 84
        umolPerMl,
        /// (mm2/s,UCUM,"mm2/s"), included from CID 7277
        mm2PerS,
        /// (um2/ms,UCUM,"um2/ms"), included from CID 7277
        um2PerMs,
        /// (um2/s,UCUM,"um2/s"), included from CID 7277
        um2PerS,
        /// (10-6.mm2/s,UCUM,"10-6.mm2/s"), included from CID 7277
        _106mm2PerS,
        /// (Gy,UCUM,"Gy"), included from CID 10071
        Gy,
        /// (Sv,UCUM,"Sv"), included from CID 10071
        Sv,
        /// (1,UCUM,"no units")
        noUnits,
        /// ({ratio},UCUM,"ratio")
        ratio,
        /// ([hnsf'U],UCUM,"Hounsfield unit")
        HounsfieldUnit,
        /// ({counts},UCUM,"Counts")
        Counts,
        /// ([arb'U],UCUM,"arbitrary unit")
        arbitraryUnit,
        /// ([ppm],UCUM,"ppm")
        ppm,
        /// (cm/s,UCUM,"cm/s")
        cmPerS,
        /// (mm/s,UCUM,"mm/s")
        mmPerS,
        /// (dB,UCUM,"dB")
        dB,
        /// (Cel,UCUM,"Cel")
        Cel,
        /// (ml/min,UCUM,"ml/min")
        mlPerMin,
        /// (ml/s,UCUM,"ml/s")
        mlPerS,
        /// (ms,UCUM,"ms")
        ms,
        /// (s,UCUM,"s")
        s,
        /// (Hz,UCUM,"Hz")
        Hz,
        /// (mT,UCUM,"mT")
        mT,
        /// ({Particles}/(100)g{Tissue},UCUM,"Particles/100g tissue")
        ParticlesPer100gTissue,
        /// (s/mm2,UCUM,"s/mm2")
        sPerMm2,
        /// (ml/(100)g/min,UCUM,"ml/(100)g/min")
        mlPer100gPerMin,
        /// (ml/(100)ml,UCUM,"ml/(100)ml")
        mlPer100ml,
        /// (mmol/kg{WetWeight},UCUM,"mmol/kg wet weight")
        mmolPerKgWetWeight,
        /// (/min,UCUM,"/min")
        PerMin,
        /// (/s,UCUM,"/s")
        PerS
    };

    /** (default) constructor
     ** @param  selectedValue  coded entry to be selected as the current value (optional).
     *                         Should be a valid code according to the DICOM definitions
     *                         for this context group, i.e. no checks are performed.
     *                         Call DSRContextGroup::checkSelectedValue() if needed.
     */
    CID7181_AbstractMultiDimensionalImageModelComponentUnit(const DSRCodedEntryValue &selectedValue = DSRCodedEntryValue());

    /** constructor
     ** @param  selectedValue         type mapped to a coded entry that is selected as the
     *                                current value
     ** @param  enhancedEncodingMode  set enhanced encoding mode for coded entry (if enabled)
     */
    CID7181_AbstractMultiDimensionalImageModelComponentUnit(const EnumType selectedValue,
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
typedef CID7181_AbstractMultiDimensionalImageModelComponentUnit CMR_CID7181;


#endif
