/*
 *
 *  Copyright (C) 2000, OFFIS
 *
 *  This software and supporting documentation were developed by
 *
 *    Kuratorium OFFIS e.V.
 *    Healthcare Information and Communication Systems
 *    Escherweg 2
 *    D-26121 Oldenburg, Germany
 *
 *  THIS SOFTWARE IS MADE AVAILABLE,  AS IS,  AND OFFIS MAKES NO  WARRANTY
 *  REGARDING  THE  SOFTWARE,  ITS  PERFORMANCE,  ITS  MERCHANTABILITY  OR
 *  FITNESS FOR ANY PARTICULAR USE, FREEDOM FROM ANY COMPUTER DISEASES  OR
 *  ITS CONFORMITY TO ANY SPECIFICATION. THE ENTIRE RISK AS TO QUALITY AND
 *  PERFORMANCE OF THE SOFTWARE IS WITH THE USER.
 *
 *  Module: dcmsr
 *
 *  Author: Joerg Riesmeier
 *
 *  Purpose:
 *    classes: DSRContentItem
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2000-10-16 11:55:40 $
 *  CVS/RCS Revision: $Revision: 1.2 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#ifndef DSRCITEM_H
#define DSRCITEM_H

#include "osconfig.h"   /* make sure OS specific configuration is included first */

#include "dsrtypes.h"
#include "dsrdoctn.h"
#include "dsrcodvl.h"
#include "dsrnumvl.h"
#include "dsrscovl.h"
#include "dsrrefvl.h"
#include "dsrimgvl.h"
#include "dsrwavvl.h"

#include "ofstring.h"


/*---------------------*
 *  class declaration  *
 *---------------------*/

/** Interface class for content items.
 *  This class allows to access the document tree nodes without using any pointers.
 */
class DSRContentItem
  : public DSRTypes
{
    friend class DSRDocumentTree;

  public:

    /** destructor
     */
    virtual ~DSRContentItem();

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

    /** get string value.
     *  Applicable to: TEXT, DATETIME, DATE, TIME, UIDREF, PNAME
     ** @return string value of current content item if valid, EmptyString otherwise
     */
    const OFString &getStringValue() const;
    
    /** set string value.  Please use the correct format for the string value depending on
     *  the corresponding content item.
     *  Applicable to: TEXT, DATETIME, DATE, TIME, UIDREF, PNAME
     ** @param  stringValue  value to be set
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    E_Condition setStringValue(const OFString &stringValue);

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
    E_Condition getCodeValue(DSRCodedEntryValue &codeValue) const;

    /** set code value.
     *  Applicable to: CODE
     ** @param  codeValue  value to be set
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    E_Condition setCodeValue(const DSRCodedEntryValue &codeValue);

    /** get numeric value.
     *  Applicable to: NUM
     ** @return numeric measurement value of current content item if valid, EmptyNumericMeasurement otherwise
     */
    const DSRNumericMeasurementValue &getNumericValue() const;

    /** get copy of numeric value.
     *  Applicable to: NUM
     ** @param  numericValue  variable where the copy should be stored
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    E_Condition getNumericValue(DSRNumericMeasurementValue &numericValue) const;

    /** set numeric value.
     *  Applicable to: NUM
     ** @param  numericValue  value to be set
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    E_Condition setNumericValue(const DSRNumericMeasurementValue &numericValue);

    /** get pointer to spatial coordinates.
     *  Applicable to: SCOORD
     ** @return pointer spatial coordinates value of current content item if valid, NULL otherwise
     */
    DSRSpatialCoordinatesValue *getSpatialCoordinatesPtr();

    /** get spatial coordinates.
     *  Applicable to: SCOORD
     ** @return spatial coordinates value of current content item if valid, EmptySpatialCoordinates otherwise
     */
    const DSRSpatialCoordinatesValue &getSpatialCoordinates() const;

    /** get copy of spatial coordinates.
     *  Applicable to: SCOORD
     ** @param  coordinatesValue  variable where the copy should be stored
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    E_Condition getSpatialCoordinates(DSRSpatialCoordinatesValue &coordinatesValue) const;

    /** set spatial coordinates.
     *  Applicable to: SCOORD
     ** @param  coordinatesValue  value to be set
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    E_Condition setSpatialCoordinates(const DSRSpatialCoordinatesValue &coordinatesValue);

    /** get pointer to composite reference.
     *  Applicable to: COMPOSITE
     ** @return pointer reference value of current content item if valid, NULL otherwise
     */
    DSRReferenceValue *getReferencePtr();

    /** get composite reference.
     *  Applicable to: COMPOSITE
     ** @return reference value of current content item if valid, EmptyReference otherwise
     */
    const DSRReferenceValue &getReference() const;

    /** get copy of composite reference.
     *  Applicable to: COMPOSITE
     ** @param  referenceValue  variable where the copy should be stored
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    E_Condition getReference(DSRReferenceValue &referenceValue) const;

    /** set composite reference.
     *  Applicable to: COMPOSITE
     ** @param  referenceValue  value to be set
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    E_Condition setReference(const DSRReferenceValue &referenceValue);

    /** get pointer to image reference.
     *  Applicable to: IMAGE
     ** @return pointer image reference value of current content item if valid, NULL otherwise
     */
    DSRImageReferenceValue *getImageReferencePtr();

    /** get image reference.
     *  Applicable to: IMAGE
     ** @return image reference value of current content item if valid, EmptyImageReference otherwise
     */
    const DSRImageReferenceValue &getImageReference() const;

    /** get copy of image reference.
     *  Applicable to: IMAGE
     ** @param  referenceValue  variable where the copy should be stored
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    E_Condition getImageReference(DSRImageReferenceValue &referenceValue) const;

    /** set image reference.
     *  Applicable to: IMAGE
     ** @param  referenceValue  value to be set
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    E_Condition setImageReference(const DSRImageReferenceValue &referenceValue);

    /** get pointer to waveform reference.
     *  Applicable to: WAVEFORM
     ** @return pointer waveform reference value of current content item if valid, NULL otherwise
     */
    DSRWaveformReferenceValue *getWaveformReferencePtr();

    /** get waveform reference.
     *  Applicable to: WAVEFORM
     ** @return waveform reference value of current content item if valid, EmptyWaveformReference otherwise
     */
    const DSRWaveformReferenceValue &getWaveformReference() const;

    /** get copy of waveform reference.
     *  Applicable to: WAVEFORM
     ** @param  referenceValue  variable where the copy should be stored
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    E_Condition getWaveformReference(DSRWaveformReferenceValue &referenceValue) const;

    /** set waveform reference.
     *  Applicable to: WAVEFORM
     ** @param  referenceValue  value to be set
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    E_Condition setWaveformReference(const DSRWaveformReferenceValue &referenceValue);

    /** get continuity of content flag.
     *  Applicable to: CONTAINER
     ** @return continuity of content flag if successful, COC_invalid otherwise
     */
    E_ContinuityOfContent getContinuityOfContent() const;

    /** set continuity of content flag.
     *  Applicable to: CONTAINER
     ** @param  continuityOfContent  value to be set
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    E_Condition setContinuityOfContent(const E_ContinuityOfContent continuityOfContent);

    /** get concept name.
     *  Applicable to all content items.
     ** @return concept name value of current content item if valid, EmptyCodedEntry otherwise
     */
    const DSRCodedEntryValue &getConceptName() const;

    /** get copy of concept name.
     *  Applicable to all content items.
     ** @param  codeValue  variable where the copy should be stored
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    E_Condition getConceptName(DSRCodedEntryValue &conceptName) const;

    /** set concept name.
     *  Applicable to all content items.
     ** @param  conceptName  value to be set
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    E_Condition setConceptName(const DSRCodedEntryValue &conceptName);

    /** get observation date time.
     *  Applicable to all content items (optional attribute).
     ** @return string value of current content item if valid, EmptyString otherwise
     */
    const OFString &getObservationDateTime() const;

    /** set observation date time.  Please use the correct DICOM format (YYYYMMDDHHMMSS).
     *  Applicable to all content items.
     ** @param  observationDateTime  value to be set (might be an empty string)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    E_Condition setObservationDateTime(const OFString &observationDateTime);


  protected:

    /** default constructor (protected)
     */
    DSRContentItem();

    /** set internal tree node pointer
     *  @param  node  pointer to the document tree node (content item)
     */
    void setTreeNode(DSRDocumentTreeNode *node)
    {
        TreeNode = node;
    }


  private:

    /// internal tree node pointer (current conten item)
    DSRDocumentTreeNode *TreeNode;

    /// empty string value, used as default return value for getStringValue()
    static const OFString                   EmptyString;
    /// empty coded entry value, used as default return value for getCodeValue() and getConceptName()
    static const DSRCodedEntryValue         EmptyCodedEntry;
    /// empty numberic measurement value, used as default return value for getNumericValue()
    static const DSRNumericMeasurementValue EmptyNumericMeasurement;
    /// empty spatial coordinates value, used as default return value for getSpatialCoordinates()
    static const DSRSpatialCoordinatesValue EmptySpatialCoordinates;
    /// empty reference value, used as default return value for getReference()
    static const DSRReferenceValue          EmptyReference;
    /// empty image reference value, used as default return value for getImageReference()
    static const DSRImageReferenceValue     EmptyImageReference;
    /// empty waveform reference value, used as default return value for getWaveformReference()
    static const DSRWaveformReferenceValue  EmptyWaveformReference;


 // --- declaration of copy constructor and assignment operator

    DSRContentItem(const DSRContentItem &);
    DSRContentItem &operator=(const DSRContentItem &);
};


#endif


/*
 *  CVS/RCS Log:
 *  $Log: dsrcitem.h,v $
 *  Revision 1.2  2000-10-16 11:55:40  joergr
 *  Added doc++ comments.
 *  Added methods allowing direct access to certain content item values.
 *
 *  Revision 1.1  2000/10/13 07:49:23  joergr
 *  Added new module 'dcmsr' providing access to DICOM structured reporting
 *  documents (supplement 23).  Doc++ documentation not yet completed.
 *
 *
 */
