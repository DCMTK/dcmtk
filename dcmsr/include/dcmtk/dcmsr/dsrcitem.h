/*
 *
 *  Copyright (C) 2000-2010, OFFIS e.V.
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
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:16:32 $
 *  CVS/RCS Revision: $Revision: 1.19 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
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
#include "dcmtk/dcmsr/dsrtcovl.h"
#include "dcmtk/dcmsr/dsrcomvl.h"
#include "dcmtk/dcmsr/dsrimgvl.h"
#include "dcmtk/dcmsr/dsrwavvl.h"

#include "dcmtk/ofstd/ofstring.h"


/*---------------------*
 *  class declaration  *
 *---------------------*/

/** Interface class for content items.
 *  This class allows to access the document tree nodes without using any pointers.
 */
class DSRContentItem
  : protected DSRTypes
{
    // allow DSRDocumentTree to access protected method setTreeNode()
    friend class DSRDocumentTree;

  public:

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
     ** @return value type of current content item if valid, VT_invalid otherwise
     */
    E_ValueType getValueType() const;

    /** get relationship type.
     *  Applicable to all content items.
     ** @return relationship type of current content item if valid, RT_invalid otherwise
     */
    E_RelationshipType getRelationshipType() const;

    /** get ID of the referenced node.
     *  Applicable to: byReference relationships
     ** @return ID of the referenced node if valid, 0 otherwise
     */
    size_t getReferencedNodeID() const;

    /** get string value.
     *  Applicable to: TEXT, DATETIME, DATE, TIME, UIDREF, PNAME
     ** @return string value of current content item if valid, EmptyString otherwise
     */
    const OFString &getStringValue() const;

    /** set string value.  Please use the correct format for the string value depending on
     *  the corresponding content item (value type).
     *  Applicable to: TEXT, DATETIME, DATE, TIME, UIDREF, PNAME
     ** @param  stringValue  value to be set
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition setStringValue(const OFString &stringValue);

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
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition setCodeValue(const DSRCodedEntryValue &codeValue);

    /** get pointer to numeric value.
     *  Applicable to: NUM
     ** @return pointer to numeric value of current content item if valid, NULL otherwise
     */
    DSRNumericMeasurementValue *getNumericValuePtr();

    /** get numeric value.
     *  Applicable to: NUM
     ** @return numeric measurement value of current content item if valid, EmptyNumericMeasurement otherwise
     */
    const DSRNumericMeasurementValue &getNumericValue() const;

    /** get copy of numeric value.
     *  Applicable to: NUM
     ** @param  numericValue  variable where the copy should be stored (cleared if an error occurs)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition getNumericValue(DSRNumericMeasurementValue &numericValue) const;

    /** set numeric value.
     *  Applicable to: NUM
     ** @param  numericValue  value to be set
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition setNumericValue(const DSRNumericMeasurementValue &numericValue);

    /** get pointer to spatial coordinates.
     *  Applicable to: SCOORD
     ** @return pointer to spatial coordinates value of current content item if valid, NULL otherwise
     */
    DSRSpatialCoordinatesValue *getSpatialCoordinatesPtr();

    /** get spatial coordinates.
     *  Applicable to: SCOORD
     ** @return spatial coordinates value of current content item if valid, EmptySpatialCoordinates otherwise
     */
    const DSRSpatialCoordinatesValue &getSpatialCoordinates() const;

    /** get copy of spatial coordinates.
     *  Applicable to: SCOORD
     ** @param  coordinatesValue  variable where the copy should be stored (cleared if an error occurs)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition getSpatialCoordinates(DSRSpatialCoordinatesValue &coordinatesValue) const;

    /** set spatial coordinates.
     *  Applicable to: SCOORD
     ** @param  coordinatesValue  value to be set
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition setSpatialCoordinates(const DSRSpatialCoordinatesValue &coordinatesValue);

    /** get pointer to temporal coordinates.
     *  Applicable to: TCOORD
     ** @return pointer to temporal coordinates value of current content item if valid, NULL otherwise
     */
    DSRTemporalCoordinatesValue *getTemporalCoordinatesPtr();

    /** get temporal coordinates.
     *  Applicable to: TCOORD
     ** @return temporal coordinates value of current content item if valid, EmptyTemporalCoordinates otherwise
     */
    const DSRTemporalCoordinatesValue &getTemporalCoordinates() const;

    /** get copy of temporal coordinates.
     *  Applicable to: TCOORD
     ** @param  coordinatesValue  variable where the copy should be stored (cleared if an error occurs)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition getTemporalCoordinates(DSRTemporalCoordinatesValue &coordinatesValue) const;

    /** set temporal coordinates.
     *  Applicable to: TCOORD
     ** @param  coordinatesValue  value to be set
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition setTemporalCoordinates(const DSRTemporalCoordinatesValue &coordinatesValue);

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
     ** @param  referenceValue  variable where the copy should be stored (cleared if an error occurs)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition getCompositeReference(DSRCompositeReferenceValue &referenceValue) const;

    /** set composite reference.
     *  Applicable to: COMPOSITE
     ** @param  referenceValue  value to be set
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition setCompositeReference(const DSRCompositeReferenceValue &referenceValue);

    /** get pointer to image reference.
     *  Applicable to: IMAGE
     ** @return pointer to image reference value of current content item if valid, NULL otherwise
     */
    DSRImageReferenceValue *getImageReferencePtr();

    /** get image reference.
     *  Applicable to: IMAGE
     ** @return image reference value of current content item if valid, EmptyImageReference otherwise
     */
    const DSRImageReferenceValue &getImageReference() const;

    /** get copy of image reference.
     *  Applicable to: IMAGE
     ** @param  referenceValue  variable where the copy should be stored (cleared if an error occurs)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition getImageReference(DSRImageReferenceValue &referenceValue) const;

    /** set image reference.
     *  Applicable to: IMAGE
     ** @param  referenceValue  value to be set
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition setImageReference(const DSRImageReferenceValue &referenceValue);

    /** get pointer to waveform reference.
     *  Applicable to: WAVEFORM
     ** @return pointer to waveform reference value of current content item if valid, NULL otherwise
     */
    DSRWaveformReferenceValue *getWaveformReferencePtr();

    /** get waveform reference.
     *  Applicable to: WAVEFORM
     ** @return waveform reference value of current content item if valid, EmptyWaveformReference otherwise
     */
    const DSRWaveformReferenceValue &getWaveformReference() const;

    /** get copy of waveform reference.
     *  Applicable to: WAVEFORM
     ** @param  referenceValue  variable where the copy should be stored (cleared if an error occurs)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition getWaveformReference(DSRWaveformReferenceValue &referenceValue) const;

    /** set waveform reference.
     *  Applicable to: WAVEFORM
     ** @param  referenceValue  value to be set
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition setWaveformReference(const DSRWaveformReferenceValue &referenceValue);

    /** get continuity of content flag.
     *  This flag specifies whether or not its contained content items (child nodes) are
     *  logically linked in a continuous textual flow, or are sparate items.
     *  Applicable to: CONTAINER
     ** @return continuity of content flag if successful, COC_invalid otherwise
     */
    E_ContinuityOfContent getContinuityOfContent() const;

    /** set continuity of content flag.
     *  This flag specifies whether or not its contained content items (child nodes) are
     *  logically linked in a continuous textual flow, or are sparate items.
     *  Applicable to: CONTAINER
     ** @param  continuityOfContent  value to be set (should be different from COC_onvalid)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition setContinuityOfContent(const E_ContinuityOfContent continuityOfContent);

    /** get pointer to concept name.
     *  Code describing the concept represented by this content item.  Also conveys the value
     *  of document title and section headings in documents.
     *  Applicable to all content items (by-value only).
     ** @return pointer to comcept name value of current content item if valid, NULL otherwise
     */
    DSRCodedEntryValue *getConceptNamePtr();

    /** get concept name.
     *  Code describing the concept represented by this content item.  Also conveys the value
     *  of document title and section headings in documents.
     *  Applicable to all content items (by-value only).
     ** @return concept name value of current content item if valid, EmptyCodedEntry otherwise
     */
    const DSRCodedEntryValue &getConceptName() const;

    /** get copy of concept name.
     *  Code describing the concept represented by this content item.  Also conveys the value
     *  of document title and section headings in documents.
     *  Applicable to all content items (by-value only).
     ** @param  conceptName  variable where the copy should be stored (cleared if an error occurs)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition getConceptName(DSRCodedEntryValue &conceptName) const;

    /** set concept name.
     *  Code describing the concept represented by this content item.  Also conveys the value
     *  of document title and section headings in documents.
     *  Applicable to all content items (by-value only, optional/conditional for some value types).
     ** @param  conceptName  value to be set
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition setConceptName(const DSRCodedEntryValue &conceptName);

    /** get observation date time.
     *  This is the date and time on which this content item was completed.  Might be empty
     *  if the date and time do not differ from the content date and time, see DSRDocument.
     *  Applicable to all content items (by-value only, optional attribute).
     ** @return observation date and time of current content item if valid, EmptyString otherwise
     */
    const OFString &getObservationDateTime() const;

    /** set observation date time.
     *  This is the date and time on which this content item was completed.  Might be empty
     *  if the date and time do not differ from the content date and time, see DSRDocument.
     *  Please use the correct DICOM format (VR=DT).
     *  Applicable to all content items (by-value only).
     ** @param  observationDateTime  value to be set (might be an empty string)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition setObservationDateTime(const OFString &observationDateTime);

    /** get template identifier and mapping resource.
     *  This value pair identifies the template that was used to create this content item
     *  (and its children).  According to the DICOM standard is is "required if a template
     *  was used to define the content of this Item, and the template consists of a single
     *  CONTAINER with nested content, and it is the outermost invocation of a set of
     *  nested templates that start with the same CONTAINER."  However, this condition is
     *  currently not checked.  The identification is valid if both values are either present
     *  (non-empty) or absent (empty).
     *  Applicable to all content items (by-value only, optional attribute).
     ** @param  templateIdentifier  identifier of the template (might be empty)
     *  @param  mappingResource     mapping resource that defines the template (might be empty)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition getTemplateIdentification(OFString &templateIdentifier,
                                          OFString &mappingResource) const;

    /** set template identifier and mapping resource.
     *  The identification is valid if both values are either present (non-empty) or absent
     *  (empty).  See getTemplateIdentification() for details.
     *  Applicable to all content items (by-value only).
     ** @param  templateIdentifier  identifier of the template to be set (VR=CS)
     *  @param  mappingResource     mapping resource that defines the template (VR=CS)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition setTemplateIdentification(const OFString &templateIdentifier,
                                          const OFString &mappingResource);


  protected:

    /** default constructor (protected)
     */
    DSRContentItem();

    /** set internal tree node pointer (protected)
     *  @param  node  pointer to the document tree node (content item)
     */
    inline void setTreeNode(DSRDocumentTreeNode *node)
    {
        TreeNode = node;
    }


  private:

    /// internal tree node pointer (current conten item)
    DSRDocumentTreeNode *TreeNode;

    /// empty string value. Used as default return value for getStringValue()
    static const OFString                    EmptyString;
    /// empty coded entry value. Used as default return value for getCodeValue() and getConceptName()
    static const DSRCodedEntryValue          EmptyCodedEntry;
    /// empty numberic measurement value. Used as default return value for getNumericValue()
    static const DSRNumericMeasurementValue  EmptyNumericMeasurement;
    /// empty spatial coordinates value. Used as default return value for getSpatialCoordinates()
    static const DSRSpatialCoordinatesValue  EmptySpatialCoordinates;
    /// empty temporal coordinates value. Used as default return value for getTemporalCoordinates()
    static const DSRTemporalCoordinatesValue EmptyTemporalCoordinates;
    /// empty composite reference value. Used as default return value for getCompositeReference()
    static const DSRCompositeReferenceValue  EmptyCompositeReference;
    /// empty image reference value. Used as default return value for getImageReference()
    static const DSRImageReferenceValue      EmptyImageReference;
    /// empty waveform reference value. Used as default return value for getWaveformReference()
    static const DSRWaveformReferenceValue   EmptyWaveformReference;


 // --- declaration of copy constructor and assignment operator

    DSRContentItem(const DSRContentItem &);
    DSRContentItem &operator=(const DSRContentItem &);
};


#endif


/*
 *  CVS/RCS Log:
 *  $Log: dsrcitem.h,v $
 *  Revision 1.19  2010-10-14 13:16:32  joergr
 *  Updated copyright header. Added reference to COPYRIGHT file.
 *
 *  Revision 1.18  2005-12-08 16:04:50  meichel
 *  Changed include path schema for all DCMTK header files
 *
 *  Revision 1.17  2003/12/16 15:57:51  joergr
 *  Added note that the condition for the Content Template Sequence is currently
 *  not checked.
 *
 *  Revision 1.16  2003/10/30 17:54:09  joergr
 *  Added full support for the ContentTemplateSequence (read/write, get/set
 *  template identification). Template constraints are not checked yet.
 *
 *  Revision 1.15  2003/08/07 12:18:57  joergr
 *  Updated documentation to get rid of doxygen warnings.
 *
 *  Revision 1.14  2001/09/26 13:04:04  meichel
 *  Adapted dcmsr to class OFCondition
 *
 *  Revision 1.13  2001/05/07 16:13:23  joergr
 *  Updated CVS header.
 *
 *  Revision 1.12  2001/01/18 15:53:32  joergr
 *  Added support for digital signatures.
 *
 *  Revision 1.11  2000/11/13 14:19:05  joergr
 *  Updated comments.
 *
 *  Revision 1.10  2000/11/07 18:11:29  joergr
 *  Enhanced support for by-reference relationships.
 *
 *  Revision 1.9  2000/11/01 16:11:26  joergr
 *  Now derive "protected" from base class DSRTypes instead of "public".
 *
 *  Revision 1.8  2000/10/26 14:16:18  joergr
 *  Added support for "Comprehensive SR".
 *  Added support for TCOORD content item.
 *
 *  Revision 1.7  2000/10/23 15:06:37  joergr
 *  Added/updated doc++ comments.
 *
 *  Revision 1.6  2000/10/20 10:15:42  joergr
 *  Renamed class DSRReferenceValue to DSRCompositeReferenceValue.
 *
 *  Revision 1.5  2000/10/18 16:58:27  joergr
 *  Added methods allowing direct access to certain content item values.
 *
 *  Revision 1.4  2000/10/17 12:34:31  joergr
 *  Added method checking content item for validity/completeness.
 *  Renamed methods for composite objects.
 *
 *  Revision 1.3  2000/10/16 16:29:36  joergr
 *  Updated comments.
 *
 *  Revision 1.2  2000/10/16 11:55:40  joergr
 *  Added doc++ comments.
 *  Added methods allowing direct access to certain content item values.
 *
 *  Revision 1.1  2000/10/13 07:49:23  joergr
 *  Added new module 'dcmsr' providing access to DICOM structured reporting
 *  documents (supplement 23).  Doc++ documentation not yet completed.
 *
 *
 */
