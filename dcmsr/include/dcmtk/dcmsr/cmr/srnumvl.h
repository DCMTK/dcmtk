/*
 *
 *  Copyright (C) 2015, J. Riesmeier, Oldenburg, Germany
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  Header file for class CMR_SRNumericMeasurementValue
 *
 *  Author: Joerg Riesmeier
 *
 */


#ifndef CMR_SRNUMVL_H
#define CMR_SRNUMVL_H

#include "dcmtk/config/osconfig.h"   /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/dsrnumvl.h"
#include "dcmtk/dcmsr/cmr/cid42.h"


/*---------------------*
 *  class declaration  *
 *---------------------*/

/** Class for SR numeric measurement values
 *  (extended version with additional support of the DICOM Content Mapping Resource)
 */
class DCMTK_CMR_EXPORT CMR_SRNumericMeasurementValue
  : public DSRNumericMeasurementValue
{

  public:

    /** default constructor
     */
    CMR_SRNumericMeasurementValue();

    /** constructor
     ** @param  numericValue     numeric measurement value (VR=DS, mandatory)
     *  @param  measurementUnit  code representing the units of measurement (mandatory)
     *  @param  check            if enabled, check 'numericValue' and 'measurementUnit' for
     *                           validity before setting them.  See corresponding setValue()
     *                           method for details.
     */
    CMR_SRNumericMeasurementValue(const OFString &numericValue,
                                  const DSRCodedEntryValue &measurementUnit,
                                  const OFBool check = OFTrue);

    /** constructor
     ** @param  numericValue     numeric measurement value (VR=DS, mandatory)
     *  @param  measurementUnit  code representing the units of measurement (mandatory)
     *  @param  valueQualifier   code representing the numeric value qualifier (optional).
     *                           Can also be used to specify the reason for the absence of
     *                           the measured value sequence.
     *  @param  check            if enabled, check values for validity before setting them.
     *                           See corresponding setValue() method for details.
     */
    CMR_SRNumericMeasurementValue(const OFString &numericValue,
                                  const DSRCodedEntryValue &measurementUnit,
                                  const DSRCodedEntryValue &valueQualifier,
                                  const OFBool check = OFTrue);

    /** copy constructor
     ** @param  numericMeasurement  numeric measurement value to be copied (not checked !)
     */
    CMR_SRNumericMeasurementValue(const CMR_SRNumericMeasurementValue &numericMeasurement);

    /** destructor
     */
    virtual ~CMR_SRNumericMeasurementValue();

    /** assignment operator
     ** @param  numericMeasurement  numeric measurement value to be copied (not checked !)
     ** @return reference to this numeric value after 'numericMeasurement' has been copied
     */
    CMR_SRNumericMeasurementValue &operator=(const CMR_SRNumericMeasurementValue &numericMeasurement);

    /** set numeric value qualifier.
     *  This optional code specifies the qualification of the Numeric Value in the Measured
     *  Value Sequence, or the reason for the absence of the Measured Value Sequence Item.
     ** @param  valueQualifier        numeric value qualifier to be set
     *  @param  enhancedEncodingMode  set enhanced encoding mode for coded entry (if enabled)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition setNumericValueQualifier(CID42_NumericValueQualifier::EnumType valueQualifier,
                                         const OFBool enhancedEncodingMode = OFFalse);

  // --- reintroduce method from base class

     using DSRNumericMeasurementValue::setNumericValueQualifier;


  protected:

    /** check the specified numeric value qualifier for validity.
     *  In addition to DSRNumericMeasurementValue::checkNumericValueQualifier(), this method
     *  also checks the conformance with the Context Group 42 (see CID42_NumericValueQualifier).
     ** @param  valueQualifier  numeric value qualifier to be checked
     ** @return status, EC_Normal if value qualifier is valid, an error code otherwise
     */
    virtual OFCondition checkNumericValueQualifier(const DSRCodedEntryValue &valueQualifier) const;
};


#endif
