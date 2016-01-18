/*
 *
 *  Copyright (C) 2015-2016, J. Riesmeier, Oldenburg, Germany
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  Header file for class CID10013e_CTAcquisitionType
 *
 *  Author: Joerg Riesmeier
 *
 */


#ifndef CMR_CID10013E_H
#define CMR_CID10013E_H

#include "dcmtk/config/osconfig.h"   /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/cmr/cid10013.h"


/*---------------------*
 *  class declaration  *
 *---------------------*/

/** DCMR Context Group with enhanced functionality:
 *  CID 10013 - CT Acquisition Type.
 */
class DCMTK_CMR_EXPORT CID10013e_CTAcquisitionType
  : public CID10013_CTAcquisitionType
{

  public:

    /** (default) constructor
     ** @param  selectedValue  coded entry to be selected as the current value (optional).
     *                         Should be a valid code according to the DICOM definitions
     *                         for this context group, i.e. no checks are performed.
     *                         Call DSRContextGroup::checkSelectedValue() if needed.
     */
    CID10013e_CTAcquisitionType(const DSRCodedEntryValue &selectedValue = DSRCodedEntryValue());

    /** constructor
     ** @param  selectedValue         type mapped to a coded entry that is selected as the
     *                                current value
     ** @param  enhancedEncodingMode  set enhanced encoding mode for coded entry (if enabled)
     */
    CID10013e_CTAcquisitionType(const EnumType selectedValue,
                                const OFBool enhancedEncodingMode = OFFalse);

    /** constructor
     ** @param  selectedValue         defined term for Acquisition Type (0018,9302) mapped
     *                                to a coded entry that is selected as the current value
     ** @param  enhancedEncodingMode  set enhanced encoding mode for coded entry (if enabled)
     */
    CID10013e_CTAcquisitionType(const OFString &selectedValue,
                                const OFBool enhancedEncodingMode = OFFalse);

    /** select a coded entry given by its associated defined term as the current value
     ** @param  selectedValue         defined term for Acquisition Type (0018,9302) mapped
     *                                to a coded entry that is selected as the current value
     *  @param  enhancedEncodingMode  set enhanced encoding mode for coded entry (if enabled)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition selectValue(const OFString &selectedValue,
                                    const OFBool enhancedEncodingMode = OFFalse);

  // --- static helper functions ---

    /** map a defined term for Acquisition Type (0018,9302) to the associated coded entry
     ** @param  definedTerm           defined term that should be mapped to a coded entry
     *  @param  enhancedEncodingMode  set enhanced encoding mode for coded entry (if enabled)
     ** @return coded entry that is associated with the given defined term, or an empty
     *          value if none was found
     */
    static DSRCodedEntryValue mapAcquisitionType(const OFString &definedTerm,
                                                 const OFBool enhancedEncodingMode = OFFalse);

    /** map a defined term for Acquisition Type (0018,9302) to the associated coded entry
     ** @param  definedTerm           defined term that should be mapped to a coded entry
     *  @param  codedEntryValue       reference to variable where the resulting coded entry is
     *                                stored.  This variable is not cleared in case of error.
     *  @param  enhancedEncodingMode  set enhanced encoding mode for coded entry (if enabled)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    static OFCondition mapAcquisitionType(const OFString &definedTerm,
                                          DSRCodedEntryValue &codedEntryValue,
                                          const OFBool enhancedEncodingMode = OFFalse);

  // --- reintroduce method from base class

    using CID10013_CTAcquisitionType::selectValue;
};


/*-------------------*
 *  type definition  *
 *-------------------*/

// define short name for the context group class
typedef CID10013e_CTAcquisitionType CMR_CID10013e;


#endif
