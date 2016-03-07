/*
 *
 *  Copyright (C) 2000-2016, OFFIS e.V.
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  This software and supporting documentation were developed by
 *
 *    OFFIS e.V.
 *    R&D Division Health
 *    Escherweg 2
 *    D-26121 Oldenburg, Germany
 *
 *
 *  Module: dcmsr
 *
 *  Author: Joerg Riesmeier
 *
 *  Purpose:
 *    classes: DSRContentItem
 *
 */


#ifndef DSRCITEM_H
#define DSRCITEM_H

#include "dcmtk/config/osconfig.h"   /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/dsrtypes.h"
#include "dcmtk/dcmsr/dsrdoctn.h"
#include "dcmtk/dcmsr/dsrcodvl.h"
#include "dcmtk/dcmsr/dsrnumvl.h"
#include "dcmtk/dcmsr/dsrscovl.h"
#include "dcmtk/dcmsr/dsrsc3vl.h"
#include "dcmtk/dcmsr/dsrtcovl.h"
#include "dcmtk/dcmsr/dsrcomvl.h"
#include "dcmtk/dcmsr/dsrimgvl.h"
#include "dcmtk/dcmsr/dsrwavvl.h"


/*---------------------*
 *  class declaration  *
 *---------------------*/

/** Interface class for content items.
 *  This class allows to access the document tree nodes without using any pointers.
 */
class DCMTK_DCMSR_EXPORT DSRContentItem
  : protected DSRTypes
{
    // allow DSRDocumentSubTree to access protected method setTreeNode()
    friend class DSRDocumentSubTree;

  public:

    /** copy constructor.
     *  Internally, the copy constructor of the respective tree node class is used, so the
     *  corresponding comments apply.  This also means that the copy describes a separate
     *  content item, i.e. without relationships to other content items.
     ** @param  item  content item to be copied
     */
    DSRContentItem(const DSRContentItem &item);

    /** destructor
     */
    virtual ~DSRContentItem();

    /** check for validity/completeness.
     *  Applicable to all content items.
     ** @return OFTrue if current content item is valid, OFFalse otherwise
     */
    OFBool isValid() const;

    /** check for mark flag.
     *  Applicable to all content items.
     ** @return OFTrue if current content item is marked, OFFalse otherwise
     */
    OFBool isMarked() const;

    /** mark/unmark item.
     *  Applicable to all content items.
     ** @param  flag  mark item if OFTrue, unmark otherwise
     */
    void setMark(const OFBool flag);

    /** get value type.
     *  Applicable to all content items.
     ** @return value type of current content item if valid, DSRTypes::VT_invalid otherwise
     */
    E_ValueType getValueType() const;

    /** get relationship type.
     *  Applicable to all content items.
     ** @return relationship type of current content item if valid, DSRTypes::RT_invalid
     *          otherwise
     */
    E_RelationshipType getRelationshipType() const;

    /** get ID of the referenced node.
     *  Applicable to: by-reference relationships
     ** @return ID of the referenced node if valid, 0 otherwise
     */
    size_t getReferencedNodeID() const;

    /** get string value.
     *  Applicable to: TEXT, DATETIME, DATE, TIME, UIDREF, PNAME
     ** @return string value of current content item if valid, EmptyString otherwise
     */
    const OFString &getStringValue() const;

    /** set string value.
     *  Please use the correct format for the string value depending on the corresponding
     *  content item (value type).
     *
     *  Applicable to: TEXT, DATETIME, DATE, TIME, UIDREF, PNAME
     ** @param  stringValue  value to be set
     *  @param  check        if enabled, check value for validity before setting it
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition setStringValue(const OFString &stringValue,
                               const OFBool check = OFTrue);

    /** get pointer to code value.
     *  Applicable to: CODE
     ** @return pointer to code value of current content item if valid, NULL otherwise
     */
    DSRCodedEntryValue *getCodeValuePtr();

    /** get code value.
     *  Applicable to: CODE
     ** @return coded entry value of current content item if valid, EmptyCodedEntry otherwise
     */
    const DSRCodedEntryValue &getCodeValue() const;

    /** get copy of code value.
     *  Applicable to: CODE
     ** @param  codeValue  variable where the copy should be stored
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition getCodeValue(DSRCodedEntryValue &codeValue) const;

    /** set code value.
     *  Applicable to: CODE
     ** @param  codeValue  value to be set
     *  @param  check      if enabled, check value for validity before setting it
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition setCodeValue(const DSRCodedEntryValue &codeValue,
                             const OFBool check = OFTrue);

    /** get pointer to numeric value.
     *  Applicable to: NUM
     ** @return pointer to numeric value of current content item if valid, NULL otherwise
     */
    DSRNumericMeasurementValue *getNumericValuePtr();

    /** get numeric value.
     *  Applicable to: NUM
     ** @return numeric measurement value of current content item if valid,
     *    EmptyNumericMeasurement otherwise
     */
    const DSRNumericMeasurementValue &getNumericValue() const;

    /** get copy of numeric value.
     *  Applicable to: NUM
     ** @param  numericValue  variable where the copy should be stored
     *                        (cleared if an error occurs)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition getNumericValue(DSRNumericMeasurementValue &numericValue) const;

    /** set numeric value.
     *  Applicable to: NUM
     ** @param  numericValue  value to be set
     *  @param  check         if enabled, check value for validity before setting it
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition setNumericValue(const DSRNumericMeasurementValue &numericValue,
                                const OFBool check = OFTrue);

    /** get pointer to spatial coordinates.
     *  Applicable to: SCOORD
     ** @return pointer to spatial coordinates value of current content item if valid,
     *          NULL otherwise
     */
    DSRSpatialCoordinatesValue *getSpatialCoordinatesPtr();

    /** get spatial coordinates.
     *  Applicable to: SCOORD
     ** @return spatial coordinates value of current content item if valid,
     *          EmptySpatialCoordinates otherwise
     */
    const DSRSpatialCoordinatesValue &getSpatialCoordinates() const;

    /** get copy of spatial coordinates.
     *  Applicable to: SCOORD
     ** @param  coordinatesValue  variable where the copy should be stored
     *                            (cleared if an error occurs)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition getSpatialCoordinates(DSRSpatialCoordinatesValue &coordinatesValue) const;

    /** set spatial coordinates.
     *  Applicable to: SCOORD
     ** @param  coordinatesValue  value to be set
     *  @param  check             if enabled, check value for validity before setting it
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition setSpatialCoordinates(const DSRSpatialCoordinatesValue &coordinatesValue,
                                      const OFBool check = OFTrue);

    /** get pointer to spatial coordinates 3D.
     *  Applicable to: SCOORD3D
     ** @return pointer to spatial coordinates 3D value of current content item if valid,
     *          NULL otherwise
     */
    DSRSpatialCoordinates3DValue *getSpatialCoordinates3DPtr();

    /** get spatial coordinates 3D.
     *  Applicable to: SCOORD3D
     ** @return spatial coordinates 3D value of current content item if valid,
     *          EmptySpatialCoordinates3D otherwise
     */
    const DSRSpatialCoordinates3DValue &getSpatialCoordinates3D() const;

    /** get copy of spatial coordinates 3D.
     *  Applicable to: SCOORD3D
     ** @param  coordinatesValue  variable where the copy should be stored
     *                            (cleared if an error occurs)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition getSpatialCoordinates3D(DSRSpatialCoordinates3DValue &coordinatesValue) const;

    /** set spatial coordinates 3D.
     *  Applicable to: SCOORD3D
     ** @param  coordinatesValue  value to be set
     *  @param  check             if enabled, check value for validity before setting it
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition setSpatialCoordinates3D(const DSRSpatialCoordinates3DValue &coordinatesValue,
                                        const OFBool check = OFTrue);

    /** get pointer to temporal coordinates.
     *  Applicable to: TCOORD
     ** @return pointer to temporal coordinates value of current content item if valid,
     *          NULL otherwise
     */
    DSRTemporalCoordinatesValue *getTemporalCoordinatesPtr();

    /** get temporal coordinates.
     *  Applicable to: TCOORD
     ** @return temporal coordinates value of current content item if valid,
     *          EmptyTemporalCoordinates otherwise
     */
    const DSRTemporalCoordinatesValue &getTemporalCoordinates() const;

    /** get copy of temporal coordinates.
     *  Applicable to: TCOORD
     ** @param  coordinatesValue  variable where the copy should be stored
     *                            (cleared if an error occurs)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition getTemporalCoordinates(DSRTemporalCoordinatesValue &coordinatesValue) const;

    /** set temporal coordinates.
     *  Applicable to: TCOORD
     ** @param  coordinatesValue  value to be set
     *  @param  check             if enabled, check value for validity before setting it
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition setTemporalCoordinates(const DSRTemporalCoordinatesValue &coordinatesValue,
                                       const OFBool check = OFTrue);

    /** get pointer to composite reference.
     *  Applicable to: COMPOSITE
     ** @return pointer to reference value of current content item if valid, NULL otherwise
     */
    DSRCompositeReferenceValue *getCompositeReferencePtr();

    /** get composite reference.
     *  Applicable to: COMPOSITE
     ** @return reference value of current content item if valid, EmptyReference otherwise
     */
    const DSRCompositeReferenceValue &getCompositeReference() const;

    /** get copy of composite reference.
     *  Applicable to: COMPOSITE
     ** @param  referenceValue  variable where the copy should be stored
     *                          (cleared if an error occurs)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition getCompositeReference(DSRCompositeReferenceValue &referenceValue) const;

    /** set composite reference.
     *  Applicable to: COMPOSITE
     ** @param  referenceValue  value to be set
     *  @param  check           if enabled, check value for validity before setting it
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition setCompositeReference(const DSRCompositeReferenceValue &referenceValue,
                                      const OFBool check = OFTrue);

    /** get pointer to image reference.
     *  Applicable to: IMAGE
     ** @return pointer to image reference value of current content item if valid,
     *          NULL otherwise
     */
    DSRImageReferenceValue *getImageReferencePtr();

    /** get image reference.
     *  Applicable to: IMAGE
     ** @return image reference value of current content item if valid,
     *          EmptyImageReference otherwise
     */
    const DSRImageReferenceValue &getImageReference() const;

    /** get copy of image reference.
     *  Applicable to: IMAGE
     ** @param  referenceValue  variable where the copy should be stored
     *                          (cleared if an error occurs)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition getImageReference(DSRImageReferenceValue &referenceValue) const;

    /** set image reference.
     *  Applicable to: IMAGE
     ** @param  referenceValue  value to be set
     *  @param  check           if enabled, check value for validity before setting it
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition setImageReference(const DSRImageReferenceValue &referenceValue,
                                  const OFBool check = OFTrue);

    /** get pointer to waveform reference.
     *  Applicable to: WAVEFORM
     ** @return pointer to waveform reference value of current content item if valid,
     *          NULL otherwise
     */
    DSRWaveformReferenceValue *getWaveformReferencePtr();

    /** get waveform reference.
     *  Applicable to: WAVEFORM
     ** @return waveform reference value of current content item if valid,
     *          EmptyWaveformReference otherwise
     */
    const DSRWaveformReferenceValue &getWaveformReference() const;

    /** get copy of waveform reference.
     *  Applicable to: WAVEFORM
     ** @param  referenceValue  variable where the copy should be stored
     *                          (cleared if an error occurs)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition getWaveformReference(DSRWaveformReferenceValue &referenceValue) const;

    /** set waveform reference.
     *  Applicable to: WAVEFORM
     ** @param  referenceValue  value to be set
     *  @param  check           if enabled, check value for validity before setting it
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition setWaveformReference(const DSRWaveformReferenceValue &referenceValue,
                                     const OFBool check = OFTrue);

    /** get continuity of content flag.
     *  This flag specifies whether or not its contained content items (child nodes) are
     *  logically linked in a continuous textual flow, or are separate items.
     *
     *  Applicable to: CONTAINER
     ** @return continuity of content flag if successful, DSRTypes::COC_invalid otherwise
     */
    E_ContinuityOfContent getContinuityOfContent() const;

    /** set continuity of content flag.
     *  This flag specifies whether or not its contained content items (child nodes) are
     *  logically linked in a continuous textual flow, or are separate items.
     *
     *  Applicable to: CONTAINER
     ** @param  continuityOfContent  value to be set
     *                               (should be different from DSRTypes::COC_invalid)
     *  @param  check                if enabled, check value for validity before setting it
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition setContinuityOfContent(const E_ContinuityOfContent continuityOfContent,
                                       const OFBool check = OFTrue);

    /** get template identifier and mapping resource.
     *  This value pair identifies the template that was used to create this content item
     *  (and its children).  According to the DICOM standard this is "required if a template
     *  was used to define the content of this Item, and the template consists of a single
     *  CONTAINER with nested content, and it is the outermost invocation of a set of
     *  nested templates that start with the same CONTAINER."
     *  The identification is valid if both values are either present (non-empty) or absent
     *  (empty).
     *
     *  Applicable to: CONTAINER
     ** @param  templateIdentifier  identifier of the template (might be empty)
     *  @param  mappingResource     mapping resource that defines the template
     *                              (might be empty)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition getTemplateIdentification(OFString &templateIdentifier,
                                          OFString &mappingResource) const;

    /** get template identifier and mapping resource and optional mapping resource UID.
     *  This value triple identifies the template that was used to create this content item
     *  (and its children).  According to the DICOM standard, this is "required if a template
     *  was used to define the content of this Item, and the template consists of a single
     *  CONTAINER with nested content, and it is the outermost invocation of a set of
     *  nested templates that start with the same CONTAINER."
     *  The identification is valid if the first two values are either present (non-empty) or
     *  all three are absent (empty).
     *
     *  Applicable to: CONTAINER
     ** @param  templateIdentifier  identifier of the template (might be empty)
     *  @param  mappingResource     mapping resource that defines the template
     *                              (might be empty)
     *  @param  mappingResourceUID  uniquely identifies the mapping resource
     *                              (might be empty)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition getTemplateIdentification(OFString &templateIdentifier,
                                          OFString &mappingResource,
                                          OFString &mappingResourceUID) const;

    /** set template identifier and mapping resource.
     *  The identification is valid if the first two values are either present (non-empty)
     *  or all three values are absent (empty).  See getTemplateIdentification() for details.
     *  Please use the correct DICOM format for all values (VR=CS,CS,UI).
     *
     *  Applicable to: CONTAINER
     ** @param  templateIdentifier  identifier of the template to be set
     *  @param  mappingResource     mapping resource that defines the template
     *  @param  mappingResourceUID  uniquely identifies the mapping resource (optional)
     *  @param  check               if enabled, check value for validity before setting it
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition setTemplateIdentification(const OFString &templateIdentifier,
                                          const OFString &mappingResource,
                                          const OFString &mappingResourceUID = "",
                                          const OFBool check = OFTrue);

    /** get pointer to concept name.
     *  Code describing the concept represented by this content item.  Also conveys the value
     *  of document title and section headings in documents.
     *  Applicable to all content items (by-value only).
     ** @return pointer to concept name value of current content item if valid, NULL otherwise
     */
    DSRCodedEntryValue *getConceptNamePtr();

    /** get concept name.
     *  Code describing the concept represented by this content item.  Also conveys the value
     *  of document title and section headings in documents.
     *
     *  Applicable to all content items (by-value only).
     ** @return concept name value of current content item if valid, EmptyCodedEntry otherwise
     */
    const DSRCodedEntryValue &getConceptName() const;

    /** get copy of concept name.
     *  Code describing the concept represented by this content item.  Also conveys the value
     *  of document title and section headings in documents.
     *
     *  Applicable to all content items (by-value only).
     ** @param  conceptName  variable where the copy should be stored
     *                       (cleared if an error occurs)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition getConceptName(DSRCodedEntryValue &conceptName) const;

    /** set concept name.
     *  Code describing the concept represented by this content item.  Also conveys the value
     *  of document title and section headings in documents.
     *
     *  Applicable to all content items (by-value only, optional/conditional for some value
     *  types).
     ** @param  conceptName  value to be set
     *  @param  check        if enabled, check value for validity before setting it
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition setConceptName(const DSRCodedEntryValue &conceptName,
                               const OFBool check = OFTrue);

    /** get observation date/time.
     *  This is the date and time on which this content item was completed.  Might be empty
     *  if the date and time do not differ from the content date and time, see DSRDocument.
     *
     *  Applicable to all content items (by-value only, optional attribute).
     ** @return observation date/time of current content item if valid, EmptyString otherwise
     */
    const OFString &getObservationDateTime() const;

    /** set observation date/time.
     *  This is the date and time on which this content item was completed.  Might be empty
     *  if the date and time do not differ from the content date and time, see DSRDocument.
     *  Please use the correct DICOM format (VR=DT).
     *
     *  Applicable to all content items (by-value only).
     ** @param  observationDateTime  value to be set (might be an empty string)
     *  @param  check                if enabled, check value for validity before setting it
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition setObservationDateTime(const OFString &observationDateTime,
                                       const OFBool check = OFTrue);

    /** get observation unique identifier.
     *  The UID represents the semantic content of the observation; an encoding of the same
     *  observation with the same context into another representation may use the same UID.
     *
     *  Applicable to all content items (by-value only, optional attribute).
     ** @return observation unique identifier of current content item if valid,
     *          EmptyString otherwise
     */
    const OFString &getObservationUID() const;

    /** set observation unique identifier.
     *  The UID represents the semantic content of the observation; an encoding of the same
     *  observation with the same context into another representation may use the same UID.
     *  Please use the correct DICOM format (VR=UI).
     *
     *  Applicable to all content items (by-value only).
     ** @param  observationUID  value to be set (might be an empty string)
     *  @param  check           if enabled, check value for validity before setting it
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition setObservationUID(const OFString &observationUID,
                                  const OFBool check = OFTrue);

    /** get annotation text.
     *  The optional annotation text allows for labeling a content item with an arbitrary
     *  character string.  This might be useful to navigate more easily within the document
     *  tree.
     *
     *  Applicable to all content items (optional).
     ** @return annotation text of current content item if valid,  EmptyString otherwise
     */
    const OFString &getAnnotationText() const;

    /** set annotation text.
     *  The optional annotation text allows for labeling a content item with an arbitrary
     *  character string.  This might be useful to navigate more easily within the document
     *  tree.
     *
     *  Applicable to all content items (optional).
     ** @param  annotationText  value to be set (might be an empty string)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition setAnnotationText(const OFString &annotationText) const;


  protected:

    /** default constructor (protected)
     */
    DSRContentItem();

    /** set internal tree node pointer (protected)
     ** @param  node  pointer to the document tree node (content item)
     */
    inline void setTreeNode(DSRDocumentTreeNode *node)
    {
        TreeNode = node;
    }

    /** fast, non-throwing swap function.
     *  The time complexity of this function is constant.
     ** @param  item  content item to swap with
     */
    void swap(DSRContentItem &item);


  private:

    /// internal tree node pointer to current content item
    DSRDocumentTreeNode *TreeNode;
    /// flag indicating whether to delete the 'TreeNode' during destruction (or not)
    /*const*/ OFBool DeleteTreeNode;    // removed "const" to support swap() method

    /// empty numeric measurement value. Used as default return value for getNumericValue().
    /// A static member variable (as for the other values below) cannot be used because this
    /// class contains members that again contain OFCondition instances that cannot be defined
    /// before the main() function starts.
    const DSRNumericMeasurementValue EmptyNumericMeasurement;

    /// empty string value.
    /// Used as default return value for getStringValue()
    static const OFString                     EmptyString;
    /// empty coded entry value.
    /// Used as default return value for getCodeValue() and getConceptName()
    static const DSRCodedEntryValue           EmptyCodedEntry;
    /// empty spatial coordinates value.
    /// Used as default return value for getSpatialCoordinates()
    static const DSRSpatialCoordinatesValue   EmptySpatialCoordinates;
    /// empty spatial coordinates 3D value.
    /// Used as default return value for getSpatialCoordinates3D()
    static const DSRSpatialCoordinates3DValue EmptySpatialCoordinates3D;
    /// empty temporal coordinates value.
    /// Used as default return value for getTemporalCoordinates()
    static const DSRTemporalCoordinatesValue  EmptyTemporalCoordinates;
    /// empty composite reference value.
    /// Used as default return value for getCompositeReference()
    static const DSRCompositeReferenceValue   EmptyCompositeReference;
    /// empty image reference value.
    /// Used as default return value for getImageReference()
    static const DSRImageReferenceValue       EmptyImageReference;
    /// empty waveform reference value.
    /// Used as default return value for getWaveformReference()
    static const DSRWaveformReferenceValue    EmptyWaveformReference;


 // --- declaration of assignment operator

    DSRContentItem &operator=(const DSRContentItem &);
};


#endif
