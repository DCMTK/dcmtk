/*
 *
 *  Copyright (C) 2015, OFFIS e.V.
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  This software and supporting documentation were developed by
 *
 *    OFFIS e.V.
 *    R&D Division Health
 *    Escherweg 2
 *    D-26121 Oldenburg, Germany
 *
 *
 *  Module: dcmsr
 *
 *  Author: Joerg Riesmeier
 *
 *  Purpose:
 *    classes: CID42_NumericValueQualifier
 *
 *    Generated automatically from DICOM PS 3.16-2015c
 *    File created on 2015-07-20 09:43:00 by J. Riesmeier
 *
 */


#ifndef CMR_CID42_H
#define CMR_CID42_H

#include "dcmtk/config/osconfig.h"   /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/dsrctxgr.h"
#include "dcmtk/dcmsr/cmr/define.h"


/*---------------------*
 *  class declaration  *
 *---------------------*/

/** Implementation of DCMR Context Group:
 *  CID 42 - Numeric Value Qualifier.
 *  (type: extensible, version: 20020114)
 */
class DCMTK_CMR_EXPORT CID42_NumericValueQualifier
  : public DSRContextGroup
{

  public:

    /** types of numeric value qualifier
     */
    enum EnumType
    {
        /// (114000,DCM,"Not a number")
        NotANumber,
        /// (114001,DCM,"Negative Infinity")
        NegativeInfinity,
        /// (114002,DCM,"Positive Infinity")
        PositiveInfinity,
        /// (114003,DCM,"Divide by zero")
        DivideByZero,
        /// (114004,DCM,"Underflow")
        Underflow,
        /// (114005,DCM,"Overflow")
        Overflow,
        /// (114006,DCM,"Measurement failure")
        MeasurementFailure,
        /// (114007,DCM,"Measurement not attempted")
        MeasurementNotAttempted,
        /// (114008,DCM,"Calculation failure")
        CalculationFailure,
        /// (114009,DCM,"Value out of range")
        ValueOutOfRange,
        /// (114010,DCM,"Value unknown")
        ValueUnknown,
        /// (114011,DCM,"Value indeterminate")
        ValueIndeterminate
    };

    /** (default) constructor
     ** @param  selectedValue  coded entry to be selected as the current value (optional).
     *                         Should be a valid code according to the DICOM definitions
     *                         for this context group, i.e. no checks are performed.
     *                         Call DSRContextGroup::checkSelectedValue() if needed.
     */
    CID42_NumericValueQualifier(const DSRCodedEntryValue &selectedValue = DSRCodedEntryValue());

    /** constructor
     ** @param  selectedValue         type mapped to a coded entry that is selected as the
     *                                current value
     ** @param  enhancedEncodingMode  set enhanced encoding mode for coded entry (if enabled)
     */
    CID42_NumericValueQualifier(const EnumType selectedValue,
                                const OFBool enhancedEncodingMode = OFFalse);

    /** select a coded entry given by its type as the current value
     ** @param  selectedValue         type mapped to a coded entry that is selected as the
     *                                current value
     *  @param  enhancedEncodingMode  set enhanced encoding mode for coded entry (if enabled)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
   virtual OFCondition selectValue(const EnumType selectedValue,
                                   const OFBool enhancedEncodingMode = OFFalse);

    /** search for a given coded entry in this context group.  If found, the type of the
     *  entry (standard or extension) is returned.
     *  See DSRContextGroup::findCodedEntry() for details.
     ** @param  codedEntryValue  coded entry to be searched for
     ** @return result of the search process.  Use OFCondition::good() or OFCondition::bad()
     *          to check whether the coded entry has been found or not.
     */
   virtual OFCondition findCodedEntry(const DSRCodedEntryValue &codedEntryValue) const;

  // --- static helper functions ---

    /** initialize this context group explicitly.  Internally, the list of standard coded
     *  entries is created and initialized by calling getCodes().
     ** @note This function can be called multiple times but in case of multi-threaded
     *        applications should be called at least once before any instance of this class
     *        is actually used.  For single-threaded applications, there is no need to call
     *        it since the initialization is done implicitly.
     */
    static void initialize();

    /** map a given type to the associated coded entry
     ** @param  value                 type that should be mapped to a coded entry
     *  @param  enhancedEncodingMode  set enhanced encoding mode for coded entry (if enabled)
     ** @return coded entry that is associated with the given type
     */
    static DSRCodedEntryValue getCodedEntry(const EnumType value,
                                            const OFBool enhancedEncodingMode = OFFalse);

  // --- reintroduce method from base class

     using DSRContextGroup::selectValue;


  protected:

    /// type used for storing and managing the coded entries
    typedef OFMap<EnumType, DSRBasicCodedEntry> CodeList;

    /** print details on coded entries that are contained in this context group.
     *  See DSRCodedEntryValue::print() for details of the coded entry output.
     ** @param  stream  stream to which the output should be printed
     */
    virtual void printCodes(STD_NAMESPACE ostream &stream) const;

  // --- static helper function ---

    /** get list of standard coded entries that are managed by this context group.
     *  Internally, the singleton pattern is used, so the list is initialized only once.
     *  Please note that this function is not thread-safe.  Therefore, the initialize()
     *  function can be called before any instance of this class is actually used.
     ** @return reference to list of coded entries managed by this context group
     */
    static CodeList &getCodes();
};

#endif
