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
 *  Update Date:      $Date: 2000-10-13 07:49:23 $
 *  CVS/RCS Revision: $Revision: 1.1 $
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

    virtual ~DSRContentItem();

    E_ValueType getValueType() const;
    E_RelationshipType getRelationshipType() const;

    const OFString &getStringValue() const;
    E_Condition setStringValue(const OFString &stringValue);

    const DSRCodedEntryValue &getCodeValue() const;
    E_Condition getCodeValue(DSRCodedEntryValue &codeValue) const;
    E_Condition setCodeValue(const DSRCodedEntryValue &codeValue);

    const DSRNumericMeasurementValue &getNumericValue() const;
    E_Condition getNumericValue(DSRNumericMeasurementValue &numericValue) const;
    E_Condition setNumericValue(const DSRNumericMeasurementValue &numericValue);

    const DSRSpatialCoordinatesValue &getSpatialCoordinates() const;
    E_Condition getSpatialCoordinates(DSRSpatialCoordinatesValue &coordinatesValue) const;
    E_Condition setSpatialCoordinates(const DSRSpatialCoordinatesValue &coordinatesValue);

    const DSRReferenceValue &getReference() const;
    E_Condition getReference(DSRReferenceValue &referenceValue) const;
    E_Condition setReference(const DSRReferenceValue &referenceValue);

    const DSRImageReferenceValue &getImageReference() const;
    E_Condition getImageReference(DSRImageReferenceValue &referenceValue) const;
    E_Condition setImageReference(const DSRImageReferenceValue &referenceValue);

    const DSRWaveformReferenceValue &getWaveformReference() const;
    E_Condition getWaveformReference(DSRWaveformReferenceValue &referenceValue) const;
    E_Condition setWaveformReference(const DSRWaveformReferenceValue &referenceValue);

    E_ContinuityOfContent getContinuityOfContent() const;
    E_Condition setContinuityOfContent(const E_ContinuityOfContent continuityOfContent);

    const DSRCodedEntryValue &getConceptName() const;
    E_Condition getConceptName(DSRCodedEntryValue &conceptName) const;
    E_Condition setConceptName(const DSRCodedEntryValue &conceptName);

    const OFString &getObservationDateTime() const;
    E_Condition setObservationDateTime(const OFString &observationDateTime);


  protected:

    DSRContentItem();

    void setTreeNode(DSRDocumentTreeNode *node)
    {
        TreeNode = node;
    }


  private:

    DSRDocumentTreeNode *TreeNode;

    static const OFString                   EmptyString;
    static const DSRCodedEntryValue         EmptyCodedEntry;
    static const DSRNumericMeasurementValue EmptyNumericMeasurement;
    static const DSRSpatialCoordinatesValue EmptySpatialCoordinates;
    static const DSRReferenceValue          EmptyReference;
    static const DSRImageReferenceValue     EmptyImageReference;
    static const DSRWaveformReferenceValue  EmptyWaveformReference;


 // --- declaration of copy constructor and assignment operator

    DSRContentItem(const DSRContentItem &);
    DSRContentItem &operator=(const DSRContentItem &);
};


#endif


/*
 *  CVS/RCS Log:
 *  $Log: dsrcitem.h,v $
 *  Revision 1.1  2000-10-13 07:49:23  joergr
 *  Added new module 'dcmsr' providing access to DICOM structured reporting
 *  documents (supplement 23).  Doc++ documentation not yet completed.
 *
 *
 */
