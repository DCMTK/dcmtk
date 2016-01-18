/*
 *
 *  Copyright (C) 2016, J. Riesmeier, Oldenburg, Germany
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  Header file for class CMR_SRNumericMeasurementValueWithUnits
 *
 *  Author: Joerg Riesmeier
 *
 */


#ifndef CMR_SRNUMVLU_H
#define CMR_SRNUMVLU_H

#include "dcmtk/config/osconfig.h"   /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/cmr/srnumvl.h"


/*---------------------*
 *  class declaration  *
 *---------------------*/

/** Class for SR numeric values and measurements
 *  (extended version with additional support of the DICOM Content Mapping Resource).
 *  This class checks whether the optional value qualifier uses a coded entry from the
 *  Defined Context Group 42 (Numeric Value Qualifier), see DICOM PS 3.3 Table C.18.1-1.
 *  It also provides a means of specifying the Context Group for the measurement unit
 *  as a C++ template type parameter.  Both Baseline (BCID) and Defined Context Groups
 *  (DCID) are supported.
 ** @tparam  T_Units         template type used for the measurement units (context group)
 *  @tparam  T_DefinedGroup  flag indicating whether 'T_Units' is used as a baseline
 *                           (default) or defined context group
 */
template <typename T_Units, OFBool T_DefinedGroup = OFFalse>
class CMR_SRNumericMeasurementValueWithUnits
  : public CMR_SRNumericMeasurementValue
{

  public:

    /** default constructor
     */
    CMR_SRNumericMeasurementValueWithUnits();

    /** constructor
     ** @param  numericValue     numeric value (VR=DS, mandatory)
     *  @param  measurementUnit  code representing the units of measurement (mandatory)
     *  @param  check            if enabled, check 'numericValue' and 'measurementUnit' for
     *                           validity before setting them.  See corresponding setValue()
     *                           method for details.
     */
    CMR_SRNumericMeasurementValueWithUnits(const OFString &numericValue,
                                           const T_Units &measurementUnit,
                                           const OFBool check = OFTrue);

    /** constructor
     ** @param  valueQualifier  code representing the numeric value qualifier.  Used to
     *                          specify the reason for the absence of the measured value
     *                          sequence, i.e. why the numeric value and measurement unit
     *                          are empty.
     *  @param  check           if enabled, check value for validity before setting it.
     *                          See corresponding setValue() method for details.
     */
    CMR_SRNumericMeasurementValueWithUnits(const CID42_NumericValueQualifier &valueQualifier,
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
    CMR_SRNumericMeasurementValueWithUnits(const OFString &numericValue,
                                           const T_Units &measurementUnit,
                                           const CID42_NumericValueQualifier &valueQualifier,
                                           const OFBool check = OFTrue);

    /** copy constructor
     ** @param  numericMeasurement  numeric measurement value to be copied (not checked !)
     */
    CMR_SRNumericMeasurementValueWithUnits(const CMR_SRNumericMeasurementValueWithUnits<T_Units, T_DefinedGroup> &numericMeasurement);

    /** copy constructor
     ** @param  numericMeasurement  numeric measurement value to be copied (not checked !)
     */
    CMR_SRNumericMeasurementValueWithUnits(const CMR_SRNumericMeasurementValue &numericMeasurement);

    /** destructor
     */
    virtual ~CMR_SRNumericMeasurementValueWithUnits();

    /** assignment operator
     ** @param  numericMeasurement  numeric measurement value to be copied (not checked !)
     ** @return reference to this numeric value after 'numericMeasurement' has been copied
     */
    CMR_SRNumericMeasurementValueWithUnits<T_Units, T_DefinedGroup> &operator=(const CMR_SRNumericMeasurementValueWithUnits<T_Units, T_DefinedGroup> &numericMeasurement);

    /** set numeric value and measurement unit.
     *  Before setting the values, they are usually checked.  Please note that both values
     *  (i.e. 'numericValue' and 'measurementUnit') either have to be empty or non-empty.
     *  If the value pair is invalid, the current value pair is not replaced and remains
     *  unchanged.  If the value pair is replaced, the optional floating point and rational
     *  representations are cleared, i.e. they have to be set manually if needed.
     ** @param  numericValue     numeric value to be set (VR=DS, mandatory)
     *  @param  measurementUnit  measurement unit to be set (mandatory)
     *  @param  check            if enabled, check values for validity before setting them.
     *                           See checkXXX() methods for details.
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition setValue(const OFString &numericValue,
                         const T_Units &measurementUnit,
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
                         const T_Units &measurementUnit,
                         const CID42_NumericValueQualifier &valueQualifier,
                         const OFBool check = OFTrue);

    /** set measurement unit.
     *  Before setting the code, it is usually checked.  If the code is invalid the current
     *  code is not replaced and remains unchanged.
     ** @param  measurementUnit  measurement unit to be set (mandatory)
     *  @param  check            if enabled, check value for validity before setting it.
     *                           See checkMeasurementUnit() method for details.  An empty
     *                           value is never accepted, use the clear() method instead.
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition setMeasurementUnit(const T_Units &measurementUnit,
                                   const OFBool check = OFTrue);


  protected:

    /** check the specified measurement unit for validity.
     *  First, the inherited method CMR_SRNumericMeasurementValue::checkMeasurementUnit()
     *  is called.  Then, conformance with any Defined Context Group (T_Units) is checked.
     ** @param  measurementUnit  measurement unit to be checked
     ** @return status, EC_Normal if measurement unit is valid, an error code otherwise
     */
    virtual OFCondition checkMeasurementUnit(const DSRCodedEntryValue &measurementUnit) const;

    /** check the specified measurement unit for validity.
     *  This method also checks conformance with any Defined Context Group (T_Units).
     ** @param  measurementUnit  measurement unit to be checked
     ** @return status, EC_Normal if measurement unit is valid, an error code otherwise
     */
    virtual OFCondition checkMeasurementUnit(const T_Units &measurementUnit) const;
};


/*------------------*
 *  implementation  *
 *------------------*/

template <typename T_Units, OFBool T_DefinedGroup>
CMR_SRNumericMeasurementValueWithUnits<T_Units, T_DefinedGroup>::CMR_SRNumericMeasurementValueWithUnits()
  : CMR_SRNumericMeasurementValue()
{
}

template <typename T_Units, OFBool T_DefinedGroup>
CMR_SRNumericMeasurementValueWithUnits<T_Units, T_DefinedGroup>::CMR_SRNumericMeasurementValueWithUnits(const OFString &numericValue,
                                                                                                        const T_Units &measurementUnit,
                                                                                                        const OFBool check)
  : CMR_SRNumericMeasurementValue()
{
    /* use this method in order to pass T_Units parameter */
    setValue(numericValue, measurementUnit, check);
}


template <typename T_Units, OFBool T_DefinedGroup>
CMR_SRNumericMeasurementValueWithUnits<T_Units, T_DefinedGroup>::CMR_SRNumericMeasurementValueWithUnits(const CID42_NumericValueQualifier &valueQualifier,
                                                                                                        const OFBool check)
  : CMR_SRNumericMeasurementValue(valueQualifier, check)
{
}


template <typename T_Units, OFBool T_DefinedGroup>
CMR_SRNumericMeasurementValueWithUnits<T_Units, T_DefinedGroup>::CMR_SRNumericMeasurementValueWithUnits(const OFString &numericValue,
                                                                                                        const T_Units &measurementUnit,
                                                                                                        const CID42_NumericValueQualifier &valueQualifier,
                                                                                                        const OFBool check)
  : CMR_SRNumericMeasurementValue()
{
    /* use this method in order to pass T_Units parameter */
    setValue(numericValue, measurementUnit, valueQualifier, check);
}


template <typename T_Units, OFBool T_DefinedGroup>
CMR_SRNumericMeasurementValueWithUnits<T_Units, T_DefinedGroup>::CMR_SRNumericMeasurementValueWithUnits(const CMR_SRNumericMeasurementValueWithUnits<T_Units, T_DefinedGroup> &numericMeasurement)
  : CMR_SRNumericMeasurementValue(numericMeasurement)
{
}


template <typename T_Units, OFBool T_DefinedGroup>
CMR_SRNumericMeasurementValueWithUnits<T_Units, T_DefinedGroup>::CMR_SRNumericMeasurementValueWithUnits(const CMR_SRNumericMeasurementValue &numericMeasurement)
  : CMR_SRNumericMeasurementValue(numericMeasurement)
{
}


template <typename T_Units, OFBool T_DefinedGroup>
CMR_SRNumericMeasurementValueWithUnits<T_Units, T_DefinedGroup>::~CMR_SRNumericMeasurementValueWithUnits()
{
}


template <typename T_Units, OFBool T_DefinedGroup>
CMR_SRNumericMeasurementValueWithUnits<T_Units, T_DefinedGroup> &CMR_SRNumericMeasurementValueWithUnits<T_Units, T_DefinedGroup>::operator=(const CMR_SRNumericMeasurementValueWithUnits<T_Units, T_DefinedGroup> &numericMeasurement)
{
    CMR_SRNumericMeasurementValue::operator=(numericMeasurement);
    return *this;
}


template <typename T_Units, OFBool T_DefinedGroup>
OFCondition CMR_SRNumericMeasurementValueWithUnits<T_Units, T_DefinedGroup>::setValue(const OFString &numericValue,
                                                                                      const T_Units &measurementUnit,
                                                                                      const OFBool check)
{
    const DSRCodedEntryValue valueQualifier;
    /* call the function doing the real work */
    return CMR_SRNumericMeasurementValueWithUnits<T_Units, T_DefinedGroup>::setValue(numericValue, measurementUnit, valueQualifier, check);
}


template <typename T_Units, OFBool T_DefinedGroup>
OFCondition CMR_SRNumericMeasurementValueWithUnits<T_Units, T_DefinedGroup>::setValue(const OFString &numericValue,
                                                                                      const T_Units &measurementUnit,
                                                                                      const CID42_NumericValueQualifier &valueQualifier,
                                                                                      const OFBool check)
{
    OFCondition result = EC_Normal;
    if (check)
    {
        /* only check if at least one of the two values is non-empty */
        if (!numericValue.empty() || !measurementUnit.hasSelectedValue())
        {
            /* check whether the passed values are valid */
            result = checkNumericValue(numericValue);
            if (result.good())
                result = checkMeasurementUnit(measurementUnit);
        }
        if (result.good())
            result = checkNumericValueQualifier(valueQualifier);
    }
    /* set the value (without checking all the details again) */
    if (result.good())
        result = CMR_SRNumericMeasurementValue::setValue(numericValue, measurementUnit, valueQualifier, OFFalse /*check*/);
    return result;
}


template <typename T_Units, OFBool T_DefinedGroup>
OFCondition CMR_SRNumericMeasurementValueWithUnits<T_Units, T_DefinedGroup>::setMeasurementUnit(const T_Units &measurementUnit,
                                                                                                const OFBool check)
{
    /* first, check given measurement unit for validity (if not disabled) */
    OFCondition result = (check) ? checkMeasurementUnit(measurementUnit) : EC_Normal;
    /* then, set the measurement unit (without checking the coded entry again) */
    if (result.good())
        result = CMR_SRNumericMeasurementValue::setMeasurementUnit(measurementUnit, OFFalse /*check*/);
    return result;
}


template <typename T_Units, OFBool T_DefinedGroup>
OFCondition CMR_SRNumericMeasurementValueWithUnits<T_Units, T_DefinedGroup>::checkMeasurementUnit(const DSRCodedEntryValue &measurementUnit) const
{
    /* first, check coded entry for basic validity */
    OFCondition result = CMR_SRNumericMeasurementValue::checkMeasurementUnit(measurementUnit);
    /* then, check whether coded entry is allowed (if defined context group) */
    if (result.good() && T_DefinedGroup)
    {
        const T_Units contextGroup;
        if (!contextGroup.hasCodedEntry(measurementUnit))
            result = SR_EC_CodedEntryNotInContextGroup;
    }
    return result;
}


template <typename T_Units, OFBool T_DefinedGroup>
OFCondition CMR_SRNumericMeasurementValueWithUnits<T_Units, T_DefinedGroup>::checkMeasurementUnit(const T_Units &measurementUnit) const
{
    /* measurement unit should never be empty */
    return measurementUnit.hasSelectedValue() ? measurementUnit.checkSelectedValue(T_DefinedGroup)
                                              : SR_EC_InvalidValue;
}


#endif
