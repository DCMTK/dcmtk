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
 *    classes: CID5001_Countries
 *
 */


#ifndef CMR_CID5001_H
#define CMR_CID5001_H

#include "dcmtk/config/osconfig.h"   /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/dsrctxgr.h"
#include "dcmtk/dcmsr/cmr/define.h"


/*---------------------*
 *  class declaration  *
 *---------------------*/

/** Implementation of DCMR Context Group:
 *  CID 5001 - Countries.
 */
class DCMTK_CMR_EXPORT CID5001_Countries
  : public DSRContextGroup
{

  public:

    /** available countries (in alphabetical order).
     *  Please note that this list is everything but complete.
     */
    enum EnumType
    {
        /// (AT,ISO3166_1,"Austria")
        Austria,
        /// (CA,ISO3166_1,"Canada")
        Canada,
        /// (FR,ISO3166_1,"France")
        France,
        /// (DE,ISO3166_1,"Germany")
        Germany,
        /// (CH,ISO3166_1,"Switzerland")
        Switzerland,
        /// (GB,ISO3166_1,"United Kingdom of Great Britain and Northern Ireland")
        UnitedKingdom,
        /// (US,ISO3166_1,"United States of America")
        UnitedStates
    };

    /** (default) constructor
     ** @param  selectedValue  coded entry to be selected as the current value (optional).
     *                         Should be a valid code according to the DICOM definitions
     *                         for this context group, i.e. should be based on ISO 3166.
     */
    CID5001_Countries(const DSRCodedEntryValue &selectedValue = DSRCodedEntryValue());

    /** constructor
     ** @param  selectedValue         country mapped to a coded entry that is selected as the
     *                                current value
     *  @param  enhancedEncodingMode  set enhanced encoding mode for coded entry (if enabled)
     */
    CID5001_Countries(const EnumType selectedValue,
                      const OFBool enhancedEncodingMode = OFFalse);

    /** select a coded entry given by its type as the current value
     ** @param  selectedValue         country mapped to a coded entry that is selected as
     *                                the current value
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

    /** map a given country to the associated coded entry
     ** @param  value                 country that should be mapped to a coded entry
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
