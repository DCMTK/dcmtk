/*
 *
 *  Copyright (C) 2015-2016, J. Riesmeier, Oldenburg, Germany
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  Header file for class CID10033e_CTReconstructionAlgorithm
 *
 *  Author: Joerg Riesmeier
 *
 */


#ifndef CMR_CID10033E_H
#define CMR_CID10033E_H

#include "dcmtk/config/osconfig.h"   /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/cmr/cid10033.h"


/*---------------------*
 *  class declaration  *
 *---------------------*/

/** DCMR Context Group with enhanced functionality:
 *  CID 10033 - CT Reconstruction Algorithm.
 */
class DCMTK_CMR_EXPORT CID10033e_CTReconstructionAlgorithm
  : public CID10033_CTReconstructionAlgorithm
{

  public:

    /** (default) constructor
     ** @param  selectedValue  coded entry to be selected as the current value (optional).
     *                         Should be a valid code according to the DICOM definitions
     *                         for this context group, i.e. no checks are performed.
     *                         Call DSRContextGroup::checkSelectedValue() if needed.
     */
    CID10033e_CTReconstructionAlgorithm(const DSRCodedEntryValue &selectedValue = DSRCodedEntryValue());

    /** constructor
     ** @param  selectedValue         type mapped to a coded entry that is selected as the
     *                                current value
     ** @param  enhancedEncodingMode  set enhanced encoding mode for coded entry (if enabled)
     */
    CID10033e_CTReconstructionAlgorithm(const EnumType selectedValue,
                                        const OFBool enhancedEncodingMode = OFFalse);

    /** constructor
     ** @param  selectedValue         defined term for Reconstruction Algorithm (0018,9315)
     *                                mapped to a coded entry that is selected as the current
     *                                value
     ** @param  enhancedEncodingMode  set enhanced encoding mode for coded entry (if enabled)
     */
    CID10033e_CTReconstructionAlgorithm(const OFString &selectedValue,
                                        const OFBool enhancedEncodingMode = OFFalse);

    /** select a coded entry given by its associated defined term as the current value
     ** @param  selectedValue         defined term for Reconstruction Algorithm (0018,9315)
     *                                mapped to a coded entry that is selected as the current
     *                                value
     *  @param  enhancedEncodingMode  set enhanced encoding mode for coded entry (if enabled)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition selectValue(const OFString &selectedValue,
                                    const OFBool enhancedEncodingMode = OFFalse);

  // --- static helper functions ---

    /** map a defined term for Reconstruction Algorithm (0018,9315) to the associated coded
     *  entry
     ** @param  definedTerm           defined term that should be mapped to a coded entry
     *  @param  enhancedEncodingMode  set enhanced encoding mode for coded entry (if enabled)
     ** @return coded entry that is associated with the given defined term, or an empty
     *          value if none was found
     */
    static DSRCodedEntryValue mapReconstructionAlgorithm(const OFString &definedTerm,
                                                         const OFBool enhancedEncodingMode = OFFalse);

    /** map a defined term for Reconstruction Algorithm (0018,9315) to the associated coded
     *  entry
     ** @param  definedTerm           defined term that should be mapped to a coded entry
     *  @param  codedEntryValue       reference to variable where the resulting coded entry is
     *                                stored.  This variable is not cleared in case of error.
     *  @param  enhancedEncodingMode  set enhanced encoding mode for coded entry (if enabled)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    static OFCondition mapReconstructionAlgorithm(const OFString &definedTerm,
                                                  DSRCodedEntryValue &codedEntryValue,
                                                  const OFBool enhancedEncodingMode = OFFalse);

  // --- reintroduce method from base class

    using CID10033_CTReconstructionAlgorithm::selectValue;
};


/*-------------------*
 *  type definition  *
 *-------------------*/

// define short name for the context group class
typedef CID10033e_CTReconstructionAlgorithm CMR_CID10033e;


#endif
