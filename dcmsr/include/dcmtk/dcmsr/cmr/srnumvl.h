/*
 *
 *  Copyright (C) 2015-2016, J. Riesmeier, Oldenburg, Germany
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

/** Class for SR numeric values and measurements
 *  (extended version with additional support of the DICOM Content Mapping Resource).
 *  This class checks whether the optional value qualifier uses a coded entry from the
 *  Defined Context Group 42 (Numeric Value Qualifier), see DICOM PS 3.3 Table C.18.1-1.
 */
class DCMTK_CMR_EXPORT CMR_SRNumericMeasurementValue
  : public DSRNumericMeasurementValue
{

  public:

    /** default constructor
     */
    CMR_SRNumericMeasurementValue();

    /** constructor
     ** @param  numericValue     numeric value (VR=DS, mandatory)
     *  @param  measurementUnit  code representing the units of measurement (mandatory)
     *  @param  check            if enabled, check 'numericValue' and 'measurementUnit' for
     *                           validity before setting them.  See corresponding setValue()
     *                           method for details.
     */
    CMR_SRNumericMeasurementValue(const OFString &numericValue,
                                  const DSRCodedEntryValue &measurementUnit,
                                  const OFBool check = OFTrue);

    /** constructor
     ** @param  valueQualifier  code representing the numeric value qualifier.  Used to
     *                          specify the reason for the absence of the measured value
     *                          sequence, i.e. why the numeric value and measurement unit
     *                          are empty.
     *  @param  check           if enabled, check value for validity before setting it.
     *                          See corresponding setValue() method for details.
     */
    CMR_SRNumericMeasurementValue(const CID42_NumericValueQualifier &valueQualifier,
                                  const OFBool check = OFTrue);

    /** constructor
     ** @param  numericValue     numeric value (VR=DS, mandatory)
     *  @param  measurementUnit  code representing the units of measurement (mandatory)
     *  @param  valueQualifier   code representing the numeric value qualifier (optional).
     *                           Can also be used to specify the reason for the absence of
     *                           the measured value sequence (where 'numericValue' and
     *                           'measurementUnit' are stored).
     *  @param  check            if enabled, check values for validity before setting them.
     *                           See corresponding setValue() method for details.
     */
    CMR_SRNumericMeasurementValue(const OFString &numericValue,
                                  const DSRCodedEntryValue &measurementUnit,
                                  const CID42_NumericValueQualifier &valueQualifier,
                                  const OFBool check = OFTrue);

    /** copy constructor
     ** @param  numericMeasurement  numeric measurement value to be copied (not checked !)
     */
    CMR_SRNumericMeasurementValue(const CMR_SRNumericMeasurementValue &numericMeasurement);

    /** copy constructor
     ** @param  numericMeasurement  numeric measurement value to be copied (not checked !)
     */
    CMR_SRNumericMeasurementValue(const DSRNumericMeasurementValue &numericMeasurement);

    /** destructor
     */
    virtual ~CMR_SRNumericMeasurementValue();

    /** assignment operator
     ** @param  numericMeasurement  numeric measurement value to be copied (not checked !)
     ** @return reference to this numeric value after 'numericMeasurement' has been copied
     */
    CMR_SRNumericMeasurementValue &operator=(const CMR_SRNumericMeasurementValue &numericMeasurement);

    /** set empty numeric value and measurement unit with a numeric value qualifier.
     *  Before setting the value, it is usually checked.  If the value is invalid, the
     *  current numeric measurement value is not replaced and remains unchanged.
     ** @param  valueQualifier  numeric value qualifier to be set.  Used to specify the
     *                          reason for the absence of the measured value sequence,
     *                          i.e. why the numeric value and measurement unit are empty.
     *  @param  check           if enabled, check value for validity before setting it.
     *                          See checkNumericValueQualifier() method for details.
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition setValue(const CID42_NumericValueQualifier &valueQualifier,
                         const OFBool check = OFTrue);

    /** set numeric value, measurement unit and numeric value qualifier.
     *  Before setting the values, they are usually checked.  Please note that both
     *  'numericValue' and 'measurementUnit' either have to be empty or non-empty.
     *  If one of the three values is invalid, the current numeric measurement value is not
     *  replaced and remains unchanged.  If the values are replaced, the optional floating
     *  point and rational representations are cleared, i.e. they have to be set manually if
     *  needed.
     ** @param  numericValue     numeric value to be set (VR=DS, mandatory)
     *  @param  measurementUnit  measurement unit to be set (mandatory)
     *  @param  valueQualifier   numeric value qualifier to be set (optional).  Can also be
     *                           used to specify the reason for the absence of the measured
     *                           value sequence (where 'numericValue' and 'measurementUnit'
     *                           are stored).  Use an empty code to remove the current value.
     *  @param  check            if enabled, check values for validity before setting them.
     *                           See checkXXX() methods for details.
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition setValue(const OFString &numericValue,
                         const DSRCodedEntryValue &measurementUnit,
                         const CID42_NumericValueQualifier &valueQualifier,
                         const OFBool check = OFTrue);

    /** set numeric value qualifier.
     *  This optional code specifies the qualification of the Numeric Value in the Measured
     *  Value Sequence, or the reason for the absence of the Measured Value Sequence Item.
     *  Before setting the code, it is usually checked.  If the code is invalid the current
     *  code is not replaced and remains unchanged.
     ** @param  valueQualifier  numeric value qualifier to be set (optional).  Use an empty
     *                          code to remove the current value.
     *  @param  check           if enabled, check value for validity before setting it.
     *                          See checkNumericValueQualifier() method for details.
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition setNumericValueQualifier(const CID42_NumericValueQualifier &valueQualifier,
                                         const OFBool check = OFTrue);

  // --- reintroduce method from base class

    using DSRNumericMeasurementValue::setValue;
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
