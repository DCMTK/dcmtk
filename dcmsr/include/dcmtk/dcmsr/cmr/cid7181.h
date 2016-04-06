/*
 *
 *  Copyright (C) 2015-2016, J. Riesmeier, Oldenburg, Germany
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  Header file for class CID7181_AbstractMultiDimensionalImageModelComponentUnits
 *
 *  Generated automatically from DICOM PS 3.16-2016b
 *  File created on 2016-04-06 16:26:49 by J. Riesmeier
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
 *  CID 7181 - Abstract Multi-dimensional Image Model Component Units.
 *  (type: extensible, version: 20141110)
 */
class DCMTK_CMR_EXPORT CID7181_AbstractMultiDimensionalImageModelComponentUnits
  : public DSRContextGroup
{

  public:

    /** types of abstract multi-dimensional image model component units
     */
    enum EnumType
    {
        /// (mm[Hg],UCUM,"mmHg"), included from CID 3500
        MmHg,
        /// (kPa,UCUM,"kPa"), included from CID 3500
        KPa,
        /// ([PRU],UCUM,"P.R.U."), included from CID 3502
        PRU,
        /// ([wood'U],UCUM,"Wood U"), included from CID 3502
        WoodU,
        /// (dyn.s.cm-5,UCUM,"dyn.s.cm-5"), included from CID 3502
        Dynscm5,
        /// ([PRU]/m2,UCUM,"P.R.U./m2"), included from CID 3503
        PRUPerM2,
        /// ([wood'U]/m2,UCUM,"Wood U/m2"), included from CID 3503
        WoodUPerM2,
        /// (dyn.s.cm-5/m2,UCUM,"dyn.s.cm-5/m2"), included from CID 3503
        Dynscm5PerM2,
        /// (cm,UCUM,"centimeter"), included from CID 7460
        Centimeter,
        /// (mm,UCUM,"millimeter"), included from CID 7460
        Millimeter,
        /// (um,UCUM,"micrometer"), included from CID 7460
        Micrometer,
        /// (cm2,UCUM,"square centimeter"), included from CID 7461
        SquareCentimeter,
        /// (mm2,UCUM,"square millimeter"), included from CID 7461
        SquareMillimeter,
        /// (um2,UCUM,"square micrometer"), included from CID 7461
        SquareMicrometer,
        /// (dm3,UCUM,"cubic decimeter"), included from CID 7462
        CubicDecimeter,
        /// (cm3,UCUM,"cubic centimeter"), included from CID 7462
        CubicCentimeter,
        /// (mm3,UCUM,"cubic millimeter"), included from CID 7462
        CubicMillimeter,
        /// (um3,UCUM,"cubic micrometer"), included from CID 7462
        CubicMicrometer,
        /// (g/ml{SUVbw},UCUM,"Standardized Uptake Value body weight"), included from CID 85
        StandardizedUptakeValueBodyWeight,
        /// (g/ml{SUVlbm},UCUM,"Standardized Uptake Value lean body mass (James)"), included from CID 85
        StandardizedUptakeValueLeanBodyMassJames,
        /// (g/ml{SUVlbm(Janma)},UCUM,"Standardized Uptake Value lean body mass (Janma)"), included from CID 85
        StandardizedUptakeValueLeanBodyMassJanma,
        /// (cm2/ml{SUVbsa},UCUM,"Standardized Uptake Value body surface area"), included from CID 85
        StandardizedUptakeValueBodySurfaceArea,
        /// (g/ml{SUVibw},UCUM,"Standardized Uptake Value ideal body weight"), included from CID 85
        StandardizedUptakeValueIdealBodyWeight,
        /// ({counts},UCUM,"Counts"), included from CID 84
        Counts,
        /// ({counts}/s,UCUM,"Counts per second"), included from CID 84
        CountsPerSecond,
        /// ({propcounts},UCUM,"Proportional to counts"), included from CID 84
        ProportionalToCounts,
        /// ({propcounts}/s,UCUM,"Proportional to counts per second"), included from CID 84
        ProportionalToCountsPerSecond,
        /// (%,UCUM,"Percent"), included from CID 84
        Percent,
        /// (Bq/ml,UCUM,"Becquerels/milliliter"), included from CID 84
        BecquerelsPerMilliliter,
        /// (mg/min/ml,UCUM,"Milligrams/minute/milliliter"), included from CID 84
        MilligramsPerMinutePerMilliliter,
        /// (umol/min/ml,UCUM,"Micromole/minute/milliliter"), included from CID 84
        MicromolePerMinutePerMilliliter,
        /// (ml/min/g,UCUM,"Milliliter/minute/gram"), included from CID 84
        MilliliterPerMinutePerGram,
        /// (ml/g,UCUM,"Milliliter/gram"), included from CID 84
        MilliliterPerGram,
        /// (/cm,UCUM,"/Centimeter"), included from CID 84
        PerCentimeter,
        /// (umol/ml,UCUM,"Micromole/milliliter"), included from CID 84
        MicromolePerMilliliter,
        /// (1,UCUM,"no units")
        NoUnits,
        /// ({ratio},UCUM,"ratio")
        Ratio,
        /// ([hnsf'U],UCUM,"Hounsfield Unit")
        HounsfieldUnit,
        /// ([arb'U],UCUM,"arbitrary unit")
        ArbitraryUnit,
        /// (cm/s,UCUM,"centimeter/second")
        CentimeterPerSecond,
        /// (mm/s,UCUM,"millimeter/second")
        MillimeterPerSecond,
        /// (dB,UCUM,"decibel")
        Decibel,
        /// (Cel,UCUM,"degrees Celsius")
        DegreesCelsius,
        /// (ml/min,UCUM,"milliliter per minute")
        MilliliterPerMinute,
        /// (ml/s,UCUM,"milliliter per second")
        MilliliterPerSecond,
        /// (ms,UCUM,"millisecond")
        Millisecond,
        /// (s,UCUM,"second")
        Second,
        /// (Hz,UCUM,"Hertz")
        Hertz,
        /// (mT,UCUM,"milliTesla")
        MilliTesla,
        /// ({Particles}/[100]g{Tissue},UCUM,"number particles per 100 gram of tissue")
        NumberParticlesPer100GramOfTissue,
        /// (mm2/s,UCUM,"square millimeter per second")
        SquareMillimeterPerSecond,
        /// (s/mm2,UCUM,"second per square millimeter")
        SecondPerSquareMillimeter,
        /// (ml/[100]g/min,UCUM,"milliliter per 100 gram per minute")
        MilliliterPer100GramPerMinute,
        /// (ml/[100]ml,UCUM,"milliliter per 100 milliliter")
        MilliliterPer100Milliliter,
        /// (mmol/kg{WetWeight},UCUM,"millimoles per kg wet weight")
        MillimolesPerKgWetWeight,
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
    CID7181_AbstractMultiDimensionalImageModelComponentUnits(const DSRCodedEntryValue &selectedValue = DSRCodedEntryValue());

    /** constructor
     ** @param  selectedValue         type mapped to a coded entry that is selected as the
     *                                current value
     ** @param  enhancedEncodingMode  set enhanced encoding mode for coded entry (if enabled)
     */
    CID7181_AbstractMultiDimensionalImageModelComponentUnits(const EnumType selectedValue,
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
typedef CID7181_AbstractMultiDimensionalImageModelComponentUnits CMR_CID7181;


#endif
