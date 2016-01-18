/*
 *
 *  Copyright (C) 2015-2016, J. Riesmeier, Oldenburg, Germany
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  Header file for class CID4031_CommonAnatomicRegions_Enhanced
 *
 *  Author: Joerg Riesmeier
 *
 */


#ifndef CMR_CID4031E_H
#define CMR_CID4031E_H

#include "dcmtk/config/osconfig.h"   /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/cmr/cid4031.h"


/*---------------------*
 *  class declaration  *
 *---------------------*/

/** DCMR Context Group with enhanced functionality:
 *  CID 4031 - Common Anatomic Regions.
 */
class DCMTK_CMR_EXPORT CID4031e_CommonAnatomicRegions
  : public CID4031_CommonAnatomicRegions
{

  public:

    /** (default) constructor
     ** @param  selectedValue  coded entry to be selected as the current value (optional).
     *                         Should be a valid code according to the DICOM definitions
     *                         for this context group, i.e. no checks are performed.
     *                         Call DSRContextGroup::checkSelectedValue() if needed.
     */
    CID4031e_CommonAnatomicRegions(const DSRCodedEntryValue &selectedValue = DSRCodedEntryValue());

    /** constructor
     ** @param  selectedValue         type mapped to a coded entry that is selected as the
     *                                current value
     ** @param  enhancedEncodingMode  set enhanced encoding mode for coded entry (if enabled)
     */
    CID4031e_CommonAnatomicRegions(const EnumType selectedValue,
                                   const OFBool enhancedEncodingMode = OFFalse);

    /** constructor
     ** @param  selectedValue         defined term for Body Part Examined (0018,0015) mapped
     *                                to a coded entry that is selected as the current value
     ** @param  enhancedEncodingMode  set enhanced encoding mode for coded entry (if enabled)
     */
    CID4031e_CommonAnatomicRegions(const OFString &selectedValue,
                                   const OFBool enhancedEncodingMode = OFFalse);

    /** select a coded entry given by its associated defined term as the current value
     ** @param  selectedValue         defined term for Body Part Examined (0018,0015) mapped
     *                                to a coded entry that is selected as the current value
     *  @param  enhancedEncodingMode  set enhanced encoding mode for coded entry (if enabled)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition selectValue(const OFString &selectedValue,
                                    const OFBool enhancedEncodingMode = OFFalse);

  // --- static helper functions ---

    /** map a defined term for Body Part Examined (0018,0015) to the associated coded entry.
     *  See DICOM PS 3.16 Annex L (Correspondence of Anatomic Region Codes and Body Part
     *  Examined Defined Terms) for details on this mapping.
     ** @param  definedTerm           defined term that should be mapped to a coded entry
     *  @param  enhancedEncodingMode  set enhanced encoding mode for coded entry (if enabled)
     ** @return coded entry that is associated with the given defined term, or an empty value
     *          if none was found
     */
    static DSRCodedEntryValue mapBodyPartExamined(const OFString &definedTerm,
                                                  const OFBool enhancedEncodingMode = OFFalse);

    /** map a defined term for Body Part Examined (0018,0015) to the associated coded entry.
     *  See DICOM PS 3.16 Annex L (Correspondence of Anatomic Region Codes and Body Part
     *  Examined Defined Terms) for details on this mapping.
     ** @param  definedTerm           defined term that should be mapped to a coded entry
     *  @param  codedEntryValue       reference to variable where the resulting coded entry is
     *                                stored.  This variable is not cleared in case of error.
     *  @param  enhancedEncodingMode  set enhanced encoding mode for coded entry (if enabled)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    static OFCondition mapBodyPartExamined(const OFString &definedTerm,
                                           DSRCodedEntryValue &codedEntryValue,
                                           const OFBool enhancedEncodingMode = OFFalse);

  // --- reintroduce method from base class

    using CID4031_CommonAnatomicRegions::selectValue;
};


/*-------------------*
 *  type definition  *
 *-------------------*/

// define short name for the context group class
typedef CID4031e_CommonAnatomicRegions CMR_CID4031e;


#endif
