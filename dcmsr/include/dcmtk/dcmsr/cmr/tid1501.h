/*
 *
 *  Copyright (C) 2017-2021, J. Riesmeier, Oldenburg, Germany
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  Header file for class TID1501_MeasurementGroup
 *
 *  Author: Joerg Riesmeier
 *
 */


#ifndef CMR_TID1501_H
#define CMR_TID1501_H

#include "dcmtk/config/osconfig.h"   /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/dsrstpl.h"

#include "dcmtk/dcmsr/cmr/define.h"
#include "dcmtk/dcmsr/cmr/tid300.h"
#include "dcmtk/dcmsr/cmr/cid244e.h"
#include "dcmtk/dcmsr/cmr/cid6147.h"
#include "dcmtk/dcmsr/cmr/cid7181.h"
#include "dcmtk/dcmsr/cmr/cid7464.h"
#include "dcmtk/dcmsr/cmr/cid7469.h"


// include this file in doxygen documentation

/** @file tid1501.h
 *  @brief Interface class for TID 1501 in module dcmsr/cmr
 */


/*---------------------*
 *  class declaration  *
 *---------------------*/

/** Implementation of DCMR Template:
 *  TID 1501 - Measurement Group (and included templates 1502 and 300).
 *  All added content items are annotated with a text in the format "TID 1501 - Row [n]".
 ** @tparam  T_Measurement  concept names for the numeric measurements (context group)
 *  @tparam  T_Units        units of the numeric measurement values (context group)
 *  @tparam  T_Method       methods used for measuring the values (context group)
 *  @tparam  T_Derivation   methods of deriving or calculating the values (context group)
 *  @note Please note that this template class requires explicit instantiation for those
 *        combinations of the template parameters that are actually used.  This is
 *        because the implementation is "hidden" in a separate source file, which has
 *        some advantages over the usual header-only approach.
 */
template<typename T_Measurement, typename T_Units, typename T_Method, typename T_Derivation>
class DCMTK_CMR_EXPORT TID1501_MeasurementGroup
  : public DSRSubTemplate
{

  public:

    // type definitions
    typedef CMR_SRNumericMeasurementValueWithUnits<T_Units> MeasurementValue;

    typedef ::TID300_Measurement<CID7469_GenericIntensityAndSizeMeasurements,
                                 CID7181_AbstractMultiDimensionalImageModelComponentUnits,
                                 CID6147_ResponseCriteria,
                                 CID7464_GeneralRegionOfInterestMeasurementModifiers>
            TID300_Measurement;

    /** (default) constructor
     ** @param  createGroup  flag indicating whether to create an empty measurement group
     *                       by calling createMeasurementGroup() during startup
     */
    TID1501_MeasurementGroup(const OFBool createGroup = OFFalse);

    /** clear internal member variables.
     *  Also see notes on the clear() method of the base class.
     */
    virtual void clear();

    /** check whether the current internal state is valid.
     *  That means, whether the base class is valid and whether all mandatory content
     *  items and included templates are valid (present), i.e. hasMeasurementGroup()
     *  and hasMeasurements() return true.
     ** @return OFTrue if valid, OFFalse otherwise
     */
    OFBool isValid() const;

    /** check whether the 'Measurement Group' content item (TID 1501 - Row 1) is present.
     *  This mandatory content item can be created by the constructor of this class (if
     *  not disabled, which is the default) or internally by createMeasurementGroup().
     *  After clear() has been called, the content item has to be recreated, which is
     *  done automatically when needed.
     ** @param  checkChildren  optional flag indicating whether to also check for any
     *                         children, i.e.\ whether the respective content item has
     *                         child nodes.  By default, the presence of the higher-level
     *                         CONTAINER is checked only.
     ** @return OFTrue if the measurement group is present, OFFalse otherwise
     */
    OFBool hasMeasurementGroup(const OFBool checkChildren = OFFalse) const;

    /** check whether the 'Tracking Identifier' content item (TID 1501 - Row 2) is
     *  present.  This content item is optional, i.e. might be absent.
     ** @return OFTrue if the tracking identifier is present, OFFalse otherwise
     */
    OFBool hasTrackingIdentifier() const;

    /** check whether the 'Tracking Unique Identifier' content item (TID 1501 - Row 3) is
     *  present.  This content item is optional, i.e. might be absent.
     ** @return OFTrue if the tracking unique identifier is present, OFFalse otherwise
     */
    OFBool hasTrackingUniqueIdentifier() const;

    /** check whether there is an included 'Measurement' template (TID 1501 - Row 10) in
     *  this measurement template.  Initially, the mandatory sub-template TID 300 is
     *  created and included by the constructor of this class.  After clear() has been
     *  called, the content item has to be recreated, which is done automatically when
     *  needed.
     ** @param  checkChildren  flag, which is enabled by default, indicating whether to
     *                         check for any children, i.e.\ whether the respective
     *                         sub-template has any content (child nodes).  If OFFalse,
     *                         the "included template" content item is checked only.
     ** @return OFTrue if measurements are present, OFFalse otherwise
     */
    OFBool hasMeasurements(const OFBool checkChildren = OFTrue) const;

    /** get current measurement value of this measurement group as defined by TID 300
     *  (Measurement), i.e.\ the current instance of TID 1501 - Row 10.
     *  This included template is mandatory, i.e. should be present and not be empty.
     *  Further instances can be added by calling addMeasurement().
     ** @return reference to internally managed SR template (current instance)
     */
    inline TID300_Measurement &getMeasurement() const
    {
        return *OFstatic_cast(TID300_Measurement *, Measurement.get());
    }

    /** set the value of the 'Activity Session' content item (TID 1501 - Row 1b).
     *  A measurement group is created automatically (if none is present).  If the
     *  content item already exists, its value is overwritten.
     ** @param  session  identifier of the session during which the measurements were made
     *  @param  check    if enabled, check values for validity before setting them
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition setActivitySession(const OFString &session,
                                   const OFBool check = OFTrue);

    /** set the value of the 'Tracking Identifier' content item (TID 1501 - Row 2).
     *  A measurement group is created automatically (if none is present).  If the
     *  content item already exists, its value is overwritten.
     ** @param  trackingID  a text label used for tracking a finding or feature
     *  @param  check       if enabled, check values for validity before setting them
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition setTrackingIdentifier(const OFString &trackingID,
                                      const OFBool check = OFTrue);

    /** set the value of the 'Tracking Unique Identifier' content item (TID 1501 - Row 3).
     *  A measurement group is created automatically (if none is present).  If the
     *  content item already exists, its value is overwritten.
     ** @param  trackingUID  a unique identifier used for tracking a finding or feature
     *                       (associated DICOM VR=UI)
     *  @param  check        if enabled, check values for validity before setting them
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition setTrackingUniqueIdentifier(const OFString &trackingUID,
                                            const OFBool check = OFTrue);

    /** set the value of the 'Finding' content item (TID 1501 - Row 3b).
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

    /** set the value of the 'Measurement Method' content item (TID 1501 - Row 5).
     *  A measurement group is created automatically (if none is present).  If the
     *  content item already exists, its value is overwritten.
     ** @param  method  coded entry describing the method used for measuring the values
     *                  in the group (e.g.\ from the given context group 'T_Method')
     *  @param  check   if enabled, check values for validity before setting them
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition setMeasurementMethod(const T_Method &method,
                                     const OFBool check = OFTrue);

    /** add a 'Finding Site' content item (TID 1501 - Row 6, 7 and 8).
     *  A measurement group is created automatically (if none is present).
     ** @param  site          coded entry describing the anatomic location of the
     *                        measurements in the current group
     *  @param  laterality    laterality associated with the 'site' (optional)
     *  @param  siteModifier  coded entry describing the topographical modifier of the
     *                        'site' (optional)
     *  @param  check  if enabled, check values for validity before setting them
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition addFindingSite(const DSRCodedEntryValue &site,
                               const CID244e_Laterality &laterality = CID244e_Laterality(),
                               const DSRCodedEntryValue &siteModifier = DSRCodedEntryValue(),
                               const OFBool check = OFTrue);

    /** set the value of the 'Real World Value Map used for measurement' content item
     *  (TID 1501 - Row 9).  A measurement group is created automatically (if none is
     *  present).  If the content item already exists, its value is overwritten.
     ** @param  valueMap  reference to a real world value mapping object applied to the
     *                    stored image pixel values before their use for a measurement
     *  @param  check     if enabled, check values for validity before setting them
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition setRealWorldValueMap(const DSRCompositeReferenceValue &valueMap,
                                     const OFBool check = OFTrue);

    /** set the value of the 'Real World Value Map used for measurement' content item
     *  (TID 1501 - Row 9).  A measurement group is created automatically (if none is
     *  present).  If the content item already exists, its value is overwritten.
     ** @param  dataset  DICOM dataset from which the values for the reference to a
     *                   real world value mapping object (SOP class UID and SOP instance
     *                   UID) should be retrieved
     *  @param  check    if enabled, check values for validity before setting them
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition setRealWorldValueMap(DcmItem &dataset,
                                     const OFBool check = OFTrue);

    /** add a measurement as defined in 'Measurement' (TID 300 - Row 1).
     *  A measurement group is created automatically (if none is present).  There should
     *  be at least a single instance of the associated template.  Access to the current
     *  instance is available through getMeasurement().
     ** @param  conceptName   coded entry specifying the concept name of the measurement
     *                        (e.g.\ from the given context group 'T_Measurement')
     *  @param  numericValue  numeric measurement value to be set.  The measurement unit
     *                        could be taken from the baseline context group 'T_Units'.
     ** @param  checkEmpty    by default, it is checked whether the current instance of
     *                        TID 300 is empty, and thus no new instance is created.
     *                        Setting this parameter to OFFalse disables this check and
     *                        always creates and adds a new instance of this sub-template.
     *  @param  checkValue    if enabled, check values for validity before setting them
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition addMeasurement(const T_Measurement &conceptName,
                               const MeasurementValue &numericValue,
                               const OFBool checkEmpty = OFTrue,
                               const OFBool checkValue = OFTrue);

    /** add a qualitative evaluation related to the subject of the measurement group as a
     *  coded entry (TID 1501 - Row 11).
     *  A measurement group is created automatically (if none is present).
     ** @param  conceptName  coded entry to be set as the concept name
     *  @param  codeValue    coded entry to be set as the value of the new content item
     *  @param  check        if enabled, check values for validity before setting them
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition addQualitativeEvaluation(const DSRCodedEntryValue &conceptName,
                                         const DSRCodedEntryValue &codeValue,
                                         const OFBool check = OFTrue);

    /** add a qualitative evaluation related to the subject of the measurement group in
     *  text form (TID 1501 - Row 12).
     *  A measurement group is created automatically (if none is present).
     ** @param  conceptName  coded entry to be set as the concept name
     *  @param  stringValue  character string to be set as the value of the content item
     *  @param  check        if enabled, check values for validity before setting them
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition addQualitativeEvaluation(const DSRCodedEntryValue &conceptName,
                                         const OFString &stringValue,
                                         const OFBool check = OFTrue);


  protected:

    /** create the mandatory root content item of this template, i.e.\ TID 1501 - Row 1.
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


  private:

    // shared pointer to included template "Measurement" (TID 300)
    DSRSharedSubTemplate Measurement;
};


#endif
