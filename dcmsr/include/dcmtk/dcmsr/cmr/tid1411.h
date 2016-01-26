/*
 *
 *  Copyright (C) 2016, J. Riesmeier, Oldenburg, Germany
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  Header file for class TID1411_VolumetricROIMeasurements
 *
 *  Author: Joerg Riesmeier
 *
 */


#ifndef CMR_TID1411_H
#define CMR_TID1411_H

#include "dcmtk/config/osconfig.h"   /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/dsrstpl.h"

#include "dcmtk/dcmsr/cmr/define.h"
#include "dcmtk/dcmsr/cmr/srnumvlu.h"


// include this file in doxygen documentation

/** @file tid1411.h
 *  @brief Interface class and error constants for TID 1411 in module dcmsr/cmr
 */


/*------------------------*
 *  constant definitions  *
 *------------------------*/

/** @name specific error conditions for TID 1411 in module dcmsr/cmr
 */
//@{

/// error: there is no measurement group to add entries to
extern DCMTK_CMR_EXPORT const OFConditionConst CMR_EC_NoMeasurementGroup;
/// error: the given segmentation object does not conform to the template constraints
extern DCMTK_CMR_EXPORT const OFConditionConst CMR_EC_InvalidSegmentationObject;
/// error: the given DICOM object is not a real world value mapping object
extern DCMTK_CMR_EXPORT const OFConditionConst CMR_EC_InvalidRealWorldValueMappingObject;

//@}


/*---------------------*
 *  class declaration  *
 *---------------------*/

/** Implementation of DCMR Template:
 *  TID 1411 - Volumetric ROI Measurements (and included templates 1502 and 1419).
 *  All added content items are annotated with a text in the format "TID 1411 - Row [n]".
 ** @tparam  T_Measurement  concept names for the numeric measurements (context group)
 *  @tparam  T_Units        units of the numeric measurement values (context group)
 *  @tparam  T_Method       methods used for measuring the values (context group)
 *  @tparam  T_Derivation   methods of deriving or calculating the values (context group)
 ** @note Please note that currently only the mandatory (and some optional/conditional)
 *        content items and included templates are supported.
 *  @note Also note that this template class requires explicit instantiation for those
 *        combinations of the template parameters that are actually used.  This is
 *        because the implementation is "hidden" in a separate source file, which has
 *        some advantages over the usual header-only approach.
 */
template<typename T_Measurement, typename T_Units, typename T_Method, typename T_Derivation>
class DCMTK_CMR_EXPORT TID1411_VolumetricROIMeasurements
  : public DSRSubTemplate
{

  public:

    // type definition
    typedef CMR_SRNumericMeasurementValueWithUnits<T_Units> MeasurementValue;

    /** (default) constructor
     ** @param  createGroup  flag indicating whether to create an empty measurement group
     *                       by calling createMeasurementGroup() during startup
     */
    TID1411_VolumetricROIMeasurements(const OFBool createGroup = OFFalse);

    /** check whether the current internal state is valid.
     *  That means, whether the base class is valid and whether all mandatory content
     *  items and included templates are valid, i.e. hasMeasurementGroup(),
     *  hasTrackingIdentifier(), hasTrackingUniqueIdentifier(), hasReferencedSegment(),
     *  hasSourceSeriesForSegmentation() and hasROIMeasurements() return true.
     ** @return OFTrue if valid, OFFalse otherwise
     */
    OFBool isValid() const;

    /** check whether the 'Measurement Group' content item (TID 1411 - Row 1) is present.
     *  Initially, this mandatory content item is created by the constructor of this
     *  class (if not disabled).  After clear() has been called, it can be created again
     *  by calling createMeasurementGroup().
     ** @param  checkChildren  optional flag indicating whether to also check for any
     *                         children, i.e.\ whether the respective content item has
     *                         child nodes.  By default, the presence of the higher-level
     *                         CONTAINER is checked only.
     ** @return OFTrue if the measurement group is present, OFFalse otherwise
     */
    OFBool hasMeasurementGroup(const OFBool checkChildren = OFFalse) const;

    /** check whether the 'Tracking Identifier' content item (TID 1411 - Row 2) is
     *  present.  This content item is mandatory, i.e. should be present with a value.
     ** @return OFTrue if the tracking identifier is present, OFFalse otherwise
     */
    OFBool hasTrackingIdentifier() const;

    /** check whether the 'Tracking Unique Identifier' content item (TID 1411 - Row 3) is
     *  present.  This content item is mandatory, i.e. should be present with a value.
     ** @return OFTrue if the tracking unique identifier is present, OFFalse otherwise
     */
    OFBool hasTrackingUniqueIdentifier() const;

    /** check whether the 'Referenced Segment' content item (TID 1411 - Row 7) is present.
     *  This content item is conditional, but should be present to meet the conditions.
     ** @return OFTrue if the referenced segment is present, OFFalse otherwise
     */
    OFBool hasReferencedSegment() const;

    /** check whether the 'Source Series for Segmentation' content item (TID 1411 -
     *  Row 12) is present.  This content item is conditional, but should be present to
     *  meet the conditions.
     ** @return OFTrue if source series for segmentation is present, OFFalse otherwise
     */
    OFBool hasSourceSeriesForSegmentation() const;

    /** check whether there is an included 'ROI Measurements' template (TID 1411 -
     *  Row 15) in this measurement template.  This included template is mandatory.
     ** @return OFTrue if ROI measurements are present, OFFalse otherwise
     */
    OFBool hasROIMeasurements() const;

    /** set the value of the 'Activity Session' content item (TID 1411 - Row 1b).
     *  A measurement group is created automatically (if none is present).  If the
     *  content item already exists, its value is overwritten.
     ** @param  session  identifier of the session during which the measurements were made
     *  @param  check    if enabled, check values for validity before setting them
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition setActivitySession(const OFString &session,
                                   const OFBool check = OFTrue);

    /** set the value of the 'Tracking Identifier' content item (TID 1411 - Row 2).
     *  A measurement group is created automatically (if none is present).  If the
     *  content item already exists, its value is overwritten.
     ** @param  trackingID  a text label used for tracking a finding or feature
     *  @param  check       if enabled, check values for validity before setting them
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition setTrackingIdentifier(const OFString &trackingID,
                                      const OFBool check = OFTrue);

    /** set the value of the 'Tracking Unique Identifier' content item (TID 1411 - Row 3).
     *  A measurement group is created automatically (if none is present).  If the
     *  content item already exists, its value is overwritten.
     ** @param  trackingUID  a unique identifier used for tracking a finding or feature
     *                       (associated DICOM VR=UI)
     *  @param  check        if enabled, check values for validity before setting them
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition setTrackingUniqueIdentifier(const OFString &trackingUID,
                                            const OFBool check = OFTrue);

    /** set the value of the 'Finding' content item (TID 1411 - Row 3b).
     *  A measurement group is created automatically (if none is present).  If the
     *  content item already exists, its value is overwritten.
     ** @param  finding  coded entry that describes the type of the finding
     *  @param  check    if enabled, check values for validity before setting them
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition setFinding(const DSRCodedEntryValue &finding,
                           const OFBool check = OFTrue);

    /** set the value of the 'Time Point' content item (TID 1502 - Row 3).
     *  A measurement group is created automatically (if none is present).  If the
     *  content item already exists, its value is overwritten.
     ** @param  timePoint  a short pre-defined label that is human-readable
     *  @param  check      if enabled, check values for validity before setting them
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition setTimePoint(const OFString &timePoint,
                             const OFBool check = OFTrue);

    /** set the value of the 'Referenced Segment' content item (TID 1411 - Row 7).
     *  A measurement group is created automatically (if none is present).  If the
     *  content item already exists, its value is overwritten.
     ** @param  segment  reference to a segmentation object with a single selected
     *                   segment; specifically the pixels or voxels identified as
     *                   belonging to the classification of the identified segment
     *  @param  check    if enabled, check values for validity before setting them
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition setReferencedSegment(const DSRImageReferenceValue &segment,
                                     const OFBool check = OFTrue);

    /** set the value of the 'Referenced Segment' content item (TID 1411 - Row 7).
     *  A measurement group is created automatically (if none is present).  If the
     *  content item already exists, its value is overwritten.
     *  If 'copyTracking' is enabled and the 'dataset' contains appropriate tracking
     *  information for the given 'segmentNumber', setTrackingIdentifier() and
     *  setTrackingUniqueIdentifier() are also called by this method.
     ** @param  dataset        DICOM dataset from which the values for the referenced
     *                         segment (e.g. SOP class UID and SOP instance UID) should
     *                         be retrieved
     *  @param  segmentNumber  number of the segment that should be referenced.  The
     *                         value is only checked when 'copyTracking' is enabled.
     *  @param  copyTracking   flag indicating whether to copy tracking information
     *                         (tracking ID and UID) from the 'dataset', if available
     *  @param  check          if enabled, check values for validity before setting them
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition setReferencedSegment(DcmItem &dataset,
                                     const Uint16 segmentNumber,
                                     const OFBool copyTracking = OFTrue,
                                     const OFBool check = OFTrue);

    /** set the value of the 'Source series for segmentation' content item (TID 1411 -
     *  Row 12).  A measurement group is created automatically (if none is present).
     *  If the content item already exists, its value is overwritten.
     ** @param  seriesUID  the unique identifier of the source series of images that
     *                     were segmented to identify the ROI (associated DICOM VR=UI)
     *  @param  check      if enabled, check values for validity before setting them
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition setSourceSeriesForSegmentation(const OFString &seriesUID,
                                               const OFBool check = OFTrue);

    /** set the value of the 'Real World Value Map used for measurement' content item
     *  (TID 1411 - Row 14).  A measurement group is created automatically (if none is
     *  present).  If the content item already exists, its value is overwritten.
     ** @param  valueMap  reference to a real world value mapping object applied to the
     *                    stored image pixel values before their use for a measurement
     *  @param  check     if enabled, check values for validity before setting them
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition setRealWorldValueMap(const DSRCompositeReferenceValue &valueMap,
                                     const OFBool check = OFTrue);

    /** set the value of the 'Real World Value Map used for measurement' content item
     *  (TID 1411 - Row 14).  A measurement group is created automatically (if none is
     *  present).  If the content item already exists, its value is overwritten.
     ** @param  dataset  DICOM dataset from which the values for the reference to a
     *                   real world value mapping object (SOP class UID and SOP instance
     *                   UID) should be retrieved
     *  @param  check    if enabled, check values for validity before setting them
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition setRealWorldValueMap(DcmItem &dataset,
                                     const OFBool check = OFTrue);

    /** set the value of the 'Measurement Method' content item (TID 1419 - Row 1).
     *  A measurement group is created automatically (if none is present).  If the
     *  content item already exists, its value is overwritten.
     ** @param  method  coded entry describing the method used for measuring the values
     *                  (e.g.\ from the given context group 'T_Method')
     *  @param  check   if enabled, check values for validity before setting them
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition setMeasurementMethod(const T_Method &method,
                                     const OFBool check = OFTrue);

    /** set the value of the 'Finding Site' content item (TID 1419 - Row 2).
     *  A measurement group is created automatically (if none is present).  If the
     *  content item already exists, its value is overwritten.
     ** @param  site   coded entry describing the anatomic location of the measurements
     *  @param  check  if enabled, check values for validity before setting them
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition setFindingSite(const DSRCodedEntryValue &site,
                               const OFBool check = OFTrue);

    /** add a measurement as defined in 'ROI Measurements' (TID 1419 - Row 5, 7 and 8).
     *  There should be at least a single instance of the associated template.
     ** @param  conceptName   coded entry specifying the concept name of the measurement
     *                        (e.g.\ from the given context group 'T_Measurement')
     *  @param  numericValue  numeric measurement value to be set.  The measurement unit
     *                        could be taken from the baseline context group 'T_Units'.
     *  @param  method        optional method used for measuring the value
     *                        (e.g.\ from the given context group 'T_Method')
     *  @param  derivation    optional method of deriving or calculating the value
     *                        (e.g.\ from the context group 'T_Derivation')
     *  @param  check         if enabled, check value for validity before setting it
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition addMeasurement(const T_Measurement &conceptName,
                               const MeasurementValue &numericValue,
                               const T_Method &method = T_Method(),
                               const T_Derivation &derivation = T_Derivation(),
                               const OFBool check = OFTrue);


  protected:

    /** create the mandatory root content item of this template, i.e.\ TID 1411 - Row 1.
     *  It is expected that the tree is currently empty.
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition createMeasurementGroup();

    /** add a new or replace an existing content item with a given node ID.
     *  If the content item does not exist, it is added to the measurement group, i.e.
     *  below the root node.  If it does exist, both the 'valueType' and 'conceptName'
     *  are checked before replacing the value.  However, the value is not replaced by
     *  this method.  This is up to the caller after this method returned with success.
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
