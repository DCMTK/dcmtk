/*
 *
 *  Copyright (C) 2022, J. Riesmeier, Oldenburg, Germany
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  Header file for class CID218e_QuantitativeImageFeature
 *
 *  Author: Joerg Riesmeier
 *
 */


#ifndef CMR_CID218E_H
#define CMR_CID218E_H

#include "dcmtk/config/osconfig.h"   /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/cmr/cid218.h"
#include "dcmtk/dcmsr/cmr/cid7469.h"


/*---------------------*
 *  class declaration  *
 *---------------------*/

/** DCMR Context Group with enhanced functionality:
 *  CID 218 - Quantitative Image Feature.
 */
class DCMTK_CMR_EXPORT CID218e_QuantitativeImageFeature
  : public CID218_QuantitativeImageFeature
{

  public:

    /** (default) constructor
     ** @param  selectedValue  coded entry to be selected as the current value (optional).
     *                         Should be a valid code according to the DICOM definitions
     *                         for this context group, i.e. no checks are performed.
     *                         Call DSRContextGroup::checkSelectedValue() if needed.
     */
    CID218e_QuantitativeImageFeature(const DSRCodedEntryValue &selectedValue = DSRCodedEntryValue());

    /** constructor
     ** @param  selectedValue         type mapped to a coded entry that is selected as the
     *                                current value
     ** @param  enhancedEncodingMode  set enhanced encoding mode for coded entry (if enabled)
     */
    CID218e_QuantitativeImageFeature(const EnumType selectedValue,
                                     const OFBool enhancedEncodingMode = OFFalse);

    /** constructor.
     *  The type that is passed to this constructor is from CID 7469 - Generic Intensity and
     *  Size Measurements, which is included in this context group.
     *  @note This method is provided for reasons of backward compatibility.
     ** @param  selectedValue         type (from CID 7469) mapped to a coded entry that is
     *                                selected as the current value
     ** @param  enhancedEncodingMode  set enhanced encoding mode for coded entry (if enabled)
     */
    CID218e_QuantitativeImageFeature(const CMR_CID7469::EnumType selectedValue,
                                     const OFBool enhancedEncodingMode = OFFalse);

    /** select a coded entry given by its type as the current value.
     *  The type from CID 7469 - Generic Intensity and Size Measurement, which is included
     *  in this context group.
     *  @note This method is provided for reasons of backward compatibility.
     ** @param  selectedValue         type (from CID 7469) mapped to a coded entry that is
     *                                selected as the current value
     *  @param  enhancedEncodingMode  set enhanced encoding mode for coded entry (if enabled)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition selectValue(const CMR_CID7469::EnumType selectedValue,
                                    const OFBool enhancedEncodingMode = OFFalse);

  // --- reintroduce method from base class

    using CID218_QuantitativeImageFeature::selectValue;
};


/*-------------------*
 *  type definition  *
 *-------------------*/

// define short name for the context group class
typedef CID218e_QuantitativeImageFeature CMR_CID218e;


#endif
