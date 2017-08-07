/*
 *
 *  Copyright (C) 2017, J. Riesmeier, Oldenburg, Germany
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  Header file for class TID300_Measurement
 *
 *  Author: Joerg Riesmeier
 *
 */


#ifndef CMR_TID300_H
#define CMR_TID300_H

#include "dcmtk/config/osconfig.h"   /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/dsrstpl.h"

#include "dcmtk/dcmsr/cmr/define.h"
#include "dcmtk/dcmsr/cmr/srnumvlu.h"
#include "dcmtk/dcmsr/cmr/cid244e.h"


// include this file in doxygen documentation

/** @file tid300.h
 *  @brief Interface class for TID 300 in module dcmsr/cmr
 */


/*---------------------*
 *  class declaration  *
 *---------------------*/

/** Implementation of DCMR Template:
 *  TID 300 - Measurement (and included template 320).
 *  All added content items are annotated with a text in the format "TID 300 - Row [n]".
 ** @tparam  T_Measurement  concept names for the numeric measurements (context group)
 *  @tparam  T_Units        units of the numeric measurement values (context group)
 *  @tparam  T_Method       methods used for measuring the values (context group)
 *  @tparam  T_Derivation   methods of deriving or calculating the values (context group)
 ** @note Please note that currently only the mandatory and some optional/conditional
 *        content items and included templates are supported.
 *  @note Also note that this template class requires explicit instantiation for those
 *        combinations of the template parameters that are actually used.  This is
 *        because the implementation is "hidden" in a separate source file, which has
 *        some advantages over the usual header-only approach.
 */
template<typename T_Measurement, typename T_Units, typename T_Method, typename T_Derivation>
class DCMTK_CMR_EXPORT TID300_Measurement
  : public DSRSubTemplate
{

  public:

    // type definition
    typedef CMR_SRNumericMeasurementValueWithUnits<T_Units> MeasurementValue;

    /** default constructor
     */
    TID300_Measurement();

    /** constructor.
     *  Also creates an initial measurement by calling createMeasurement() internally.
     ** @param  conceptName   coded entry specifying the concept name of the measurement
     *                        (e.g.\ from the given context group 'T_Measurement')
     *  @param  numericValue  numeric measurement value to be set.  The measurement unit
     *                        could be taken from the baseline context group 'T_Units'.
     *  @param  check         if enabled, check value for validity before setting it
     */
    TID300_Measurement(const T_Measurement &conceptName,
                       const MeasurementValue &numericValue,
                       const OFBool check = OFTrue);

    /** clear internal member variables.
     *  Also see notes on the clear() method of the base class.
     */
    virtual void clear();

    /** check whether the current internal state is valid.
     *  That means, whether the base class is valid and whether all mandatory content
     *  items and included templates are valid, i.e. hasMeasurement() returns true.
     ** @return OFTrue if valid, OFFalse otherwise
     */
    OFBool isValid() const;

    /** check whether the 'Measurement' content item (TID 300 - Row 1) is present.
     *  This content item is mandatory, i.e. should be present with a value.
     ** @return OFTrue if the measurement is present, OFFalse otherwise
     */
    OFBool hasMeasurement() const;

    /** create a new measurement.
     *  Clear the entire measurement and create the mandatory content item of this
     *  template, i.e.\ TID 300 - Row 1.
     ** @param  conceptName   coded entry specifying the concept name of the measurement
     *                        (e.g.\ from the given context group 'T_Measurement')
     *  @param  numericValue  numeric measurement value to be set.  The measurement unit
     *                        could be taken from the baseline context group 'T_Units'.
     *  @param  check         if enabled, check value for validity before setting it
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition createNewMeasurement(const T_Measurement &conceptName,
                                     const MeasurementValue &numericValue,
                                     const OFBool check = OFTrue);

    /** add a 'Modifier' content item (TID 300 - Row 2) with its concept name and value
     ** @param  conceptName  coded entry describing the modifier type for the concept
     *                       name of the measurement
     *  @param  modifier     coded entry describing the modifier value for the concept
     *                       name of the measurement
     *  @param  check        if enabled, check values for validity before setting them
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition addModifier(const DSRCodedEntryValue &conceptName,
                            const DSRCodedEntryValue &modifier,
                            const OFBool check = OFTrue);

    /** set the value of the 'Measurement Method' content item (TID 300 - Row 3).
     *  If the content item already exists, its value is overwritten.
     ** @param  method  coded entry describing the method used for measuring the value
     *                  (e.g.\ from the given context group 'T_Method')
     *  @param  check   if enabled, check values for validity before setting them
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition setMeasurementMethod(const T_Method &method,
                                     const OFBool check = OFTrue);

    /** set the value of the 'Derivation' content item (TID 300 - Row 4).
     *  If the content item already exists, its value is overwritten.
     ** @param  derivation  coded entry describing the method of deriving or calculating
     *                      the value (e.g.\ from the context group 'T_Derivation')
     *  @param  check       if enabled, check values for validity before setting them
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition setDerivation(const T_Derivation &derivation,
                              const OFBool check = OFTrue);

    /** add a 'Finding Site' content item (TID 300 - Row 5, 6 and 7)
     ** @param  site          coded entry describing the anatomic location of the
     *                        measurement
     *  @param  laterality    laterality associated with the 'site' (optional)
     *  @param  siteModifier  coded entry describing the topographical modifier of the
     *                        'site' (optional)
     *  @param  check         if enabled, check values for validity before setting them
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition addFindingSite(const DSRCodedEntryValue &site,
                               const CID244e_Laterality &laterality = CID244e_Laterality(),
                               const DSRCodedEntryValue &siteModifier = DSRCodedEntryValue(),
                               const OFBool check = OFTrue);

    /** add a 'Derivation Parameter' content item (TID 300 - Row 9).
     *  This is a by-value relationship.
     ** @param  conceptName   coded entry describing the concept name of the derivation
     *                        parameter
     *  @param  numericValue  numeric measurement value of the derivation parameter
     *  @param  check         if enabled, check values for validity before setting them
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition addDerivationParameter(const DSRCodedEntryValue &conceptName,
                                       const CMR_SRNumericMeasurementValue &numericValue,
                                       const OFBool check = OFTrue);

    /** add an 'Image' content item (TID 320 - Row 1) referencing the image used as a
     *  source of the measurement.  This is a by-value relationship.
     ** @param  conceptName     coded entry describing the purpose of reference
     *  @param  imageReference  reference to image used as a source of the measurement
     *  @param  check           if enabled, check values for validity before setting them
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition addImage(const DSRCodedEntryValue &conceptName,
                         const DSRImageReferenceValue &imageReference,
                         const OFBool check = OFTrue);

    /** add an 'Image' content item (TID 320 - Row 1) referencing the image used as a
     *  source of the measurement.  This is a by-value relationship.
     ** @param  conceptName  coded entry describing the purpose of reference
     *  @param  dataset      DICOM dataset from which the values for the reference to an
     *                       image (SOP class UID and SOP instance UID), which was used
     *                       as a source of the measurement, should be retrieved
     *  @param  check        if enabled, check values for validity before setting them
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition addImage(const DSRCodedEntryValue &conceptName,
                         DcmItem &dataset,
                         const OFBool check = OFTrue);

    /** add a 'Spatial Coordinates' content item (TID 320 - Row 3 and 4) referencing the
     *  image used as a source of the measurement.  This is a by-value relationship.
     ** @param  conceptName       coded entry describing the purpose of reference
     *  @param  coordinatesValue  spatial coordinates used as a source of the measurement
     *  @param  imageReference    reference to image the spatial coordinates are selected
     *                            from
     *  @param  check             if enabled, check values for validity before setting
     *                            them
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition addSpatialCoordinates(const DSRCodedEntryValue &conceptName,
                                      const DSRSpatialCoordinatesValue &coordinatesValue,
                                      const DSRImageReferenceValue &imageReference,
                                      const OFBool check = OFTrue);

    /** set the value of the 'Equivalent Meaning of Concept Name' content item (TID 300 -
     *  Row 16).  If the content item already exists, its value is overwritten.
     ** @param  meaning  human-readable meaning of the concept name of the measurement
     *                   that is equivalent to the post-coordinated meaning conveyed by
     *                   the coded name and its concept modifier children
     *  @param  check    if enabled, check values for validity before setting them
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition setEquivalentMeaningOfConceptName(const OFString &meaning,
                                                  const OFBool check = OFTrue);

    /** set the value of the 'Real World Value Map used for measurement' content item
     *  (TID 300 - Row 18).
     *  If the content item already exists, its value is overwritten.
     ** @param  valueMap  reference to a real world value mapping object applied to the
     *                    stored image pixel values before their use for a measurement
     *  @param  check     if enabled, check values for validity before setting them
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition setRealWorldValueMap(const DSRCompositeReferenceValue &valueMap,
                                     const OFBool check = OFTrue);

    /** set the value of the 'Real World Value Map used for measurement' content item
     *  (TID 300 - Row 18).
     *  If the content item already exists, its value is overwritten.
     ** @param  dataset  DICOM dataset from which the values for the reference to a
     *                   real world value mapping object (SOP class UID and SOP instance
     *                   UID) should be retrieved
     *  @param  check    if enabled, check values for validity before setting them
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition setRealWorldValueMap(DcmItem &dataset,
                                     const OFBool check = OFTrue);


  protected:

    /** create the mandatory root content item of this template, i.e.\ TID 300 - Row 1.
     *  It is expected that the tree is currently empty.
     ** @param  conceptName   coded entry specifying the concept name of the measurement
     *                        (e.g.\ from the given context group 'T_Measurement')
     *  @param  numericValue  numeric measurement value to be set.  The measurement unit
     *                        could be taken from the baseline context group 'T_Units'.
     *  @param  check         if enabled, check value for validity before setting it
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition createMeasurement(const T_Measurement &conceptName,
                                  const MeasurementValue &numericValue,
                                  const OFBool check);

    /** add a new or replace an existing content item with a given node ID.
     *  If the content item does not exist, it is added to the measurement, i.e. below
     *  the root node.  If it does exist, both the 'valueType' and 'conceptName' are
     *  checked before replacing the value.  However, the value is not replaced by this
     *  method.  This is up to the caller after this method returned with success.
     ** @param  nodePos           index of the list entry that stores the ID of the node
     *                            to search for
     *  @param  relationshipType  relationship type of the content item to be added/replaced
     *  @param  valueType         value type of the content item to be added/replaced
     *  @param  conceptName       concept name of the content item to be added/replaced
     *  @param  annotationText    optional text used to annotate the content item
     *  @param  check             if enabled, check values for validity before setting them
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition addOrReplaceContentItem(const size_t nodePos,
                                        const E_RelationshipType relationshipType,
                                        const E_ValueType valueType,
                                        const DSRCodedEntryValue &conceptName,
                                        const OFString &annotationText,
                                        const OFBool check);
};


#endif
