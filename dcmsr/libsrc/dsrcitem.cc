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
 *  Module:  dcmsr
 *
 *  Author:  Joerg Riesmeier
 *
 *  Purpose:
 *    classes: DSRContentItem
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2000-10-13 07:52:16 $
 *  CVS/RCS Revision: $Revision: 1.1 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include "dsrcitem.h"
#include "dsrtextn.h"
#include "dsrcodtn.h"
#include "dsrnumtn.h"
#include "dsrdtitn.h"
#include "dsrdattn.h"
#include "dsrtimtn.h"
#include "dsruidtn.h"
#include "dsrpnmtn.h"
#include "dsrscotn.h"
#include "dsrcomtn.h"
#include "dsrimgtn.h"
#include "dsrwavtn.h"
#include "dsrcontn.h"


const OFString DSRContentItem::EmptyString;
const DSRCodedEntryValue DSRContentItem::EmptyCodedEntry;
const DSRNumericMeasurementValue DSRContentItem::EmptyNumericMeasurement;
const DSRSpatialCoordinatesValue DSRContentItem::EmptySpatialCoordinates;
const DSRReferenceValue DSRContentItem::EmptyReference;
const DSRImageReferenceValue DSRContentItem::EmptyImageReference;
const DSRWaveformReferenceValue DSRContentItem::EmptyWaveformReference;


DSRContentItem::DSRContentItem()
  : TreeNode(NULL)
{
}


DSRContentItem::~DSRContentItem()
{
}


DSRTypes::E_ValueType DSRContentItem::getValueType() const
{
    E_ValueType valueType = VT_invalid;
    if (TreeNode != NULL)
        valueType = TreeNode->getValueType();
    return valueType;
}


DSRTypes::E_RelationshipType DSRContentItem::getRelationshipType() const
{
    E_RelationshipType relationshipType = RT_invalid;
    if (TreeNode != NULL)
        relationshipType = TreeNode->getRelationshipType();
    return relationshipType;
}


const OFString &DSRContentItem::getStringValue() const
{
    if (TreeNode != NULL)
    {
        switch (TreeNode->getValueType())
        {
            case VT_Text:
            case VT_DateTime:
            case VT_Date:
            case VT_Time:
            case VT_UIDRef:
            case VT_PName:
                return ((DSRStringValue *)TreeNode)->getValue();
            default:
                break;
        }
    }
    return EmptyString;
}


E_Condition DSRContentItem::setStringValue(const OFString &stringValue)
{
    E_Condition result = EC_IllegalCall;
    if (TreeNode != NULL)
    {
        switch (TreeNode->getValueType())
        {
            case VT_Text:
                result = ((DSRTextTreeNode *)TreeNode)->setValue(stringValue);
                break;
            case VT_DateTime:
                result = ((DSRDateTimeTreeNode *)TreeNode)->setValue(stringValue);
                break;
            case VT_Date:
                result = ((DSRDateTreeNode *)TreeNode)->setValue(stringValue);
                break;
            case VT_Time:
                result = ((DSRTimeTreeNode *)TreeNode)->setValue(stringValue);
                break;
            case VT_UIDRef:
                result = ((DSRUIDRefTreeNode *)TreeNode)->setValue(stringValue);
                break;
            case VT_PName:
                result = ((DSRPNameTreeNode *)TreeNode)->setValue(stringValue);
                break;
            default:
                break;
        }
    }
    return result;
}


const DSRCodedEntryValue &DSRContentItem::getCodeValue() const
{
    if (TreeNode != NULL)
    {
        if (TreeNode->getValueType() == VT_Code)
            return ((DSRCodeTreeNode *)TreeNode)->getValue();
    }
    return EmptyCodedEntry;
}


E_Condition DSRContentItem::getCodeValue(DSRCodedEntryValue &codeValue) const
{
    E_Condition result = EC_IllegalCall;
    if (TreeNode != NULL)
    {
        if (TreeNode->getValueType() == VT_Code)
            result= ((DSRCodeTreeNode *)TreeNode)->getValue(codeValue);
        else
            codeValue.clear();
    } else
        codeValue.clear();
    return result;
}


E_Condition DSRContentItem::setCodeValue(const DSRCodedEntryValue &codeValue)
{
    E_Condition result = EC_IllegalCall;
    if (TreeNode != NULL)
    {
        if (TreeNode->getValueType() == VT_Code)
            result = ((DSRCodeTreeNode *)TreeNode)->setValue(codeValue);
    }
    return result;
}


const DSRNumericMeasurementValue &DSRContentItem::getNumericValue() const
{
    if (TreeNode != NULL)
    {
        if (TreeNode->getValueType() == VT_Num)
            return ((DSRNumTreeNode *)TreeNode)->getValue();
    }
    return EmptyNumericMeasurement;
}


E_Condition DSRContentItem::getNumericValue(DSRNumericMeasurementValue &numericValue) const
{
    E_Condition result = EC_IllegalCall;
    if (TreeNode != NULL)
    {
        if (TreeNode->getValueType() == VT_Num)
            result= ((DSRNumTreeNode *)TreeNode)->getValue(numericValue);
        else
            numericValue.clear();
    } else
        numericValue.clear();
    return result;
}


E_Condition DSRContentItem::setNumericValue(const DSRNumericMeasurementValue &numericValue)
{
    E_Condition result = EC_IllegalCall;
    if (TreeNode != NULL)
    {
        if (TreeNode->getValueType() == VT_Num)
            result = ((DSRNumTreeNode *)TreeNode)->setValue(numericValue);
    }
    return result;
}


const DSRSpatialCoordinatesValue &DSRContentItem::getSpatialCoordinates() const
{
    if (TreeNode != NULL)
    {
        if (TreeNode->getValueType() == VT_SCoord)
            return ((DSRSCoordTreeNode *)TreeNode)->getValue();
    }
    return EmptySpatialCoordinates;
}


E_Condition DSRContentItem::getSpatialCoordinates(DSRSpatialCoordinatesValue &coordinatesValue) const
{
    E_Condition result = EC_IllegalCall;
    if (TreeNode != NULL)
    {
        if (TreeNode->getValueType() == VT_SCoord)
            result= ((DSRSCoordTreeNode *)TreeNode)->getValue(coordinatesValue);
        else
            coordinatesValue.clear();
    } else
        coordinatesValue.clear();
    return result;
}


E_Condition DSRContentItem::setSpatialCoordinates(const DSRSpatialCoordinatesValue &coordinatesValue)
{
    E_Condition result = EC_IllegalCall;
    if (TreeNode != NULL)
    {
        if (TreeNode->getValueType() == VT_SCoord)
            result = ((DSRSCoordTreeNode *)TreeNode)->setValue(coordinatesValue);
    }
    return result;
}


const DSRReferenceValue &DSRContentItem::getReference() const
{
    if (TreeNode != NULL)
    {
        if (TreeNode->getValueType() == VT_Composite)
            return ((DSRCompositeTreeNode *)TreeNode)->getValue();
    }
    return EmptyReference;
}


E_Condition DSRContentItem::getReference(DSRReferenceValue &referenceValue) const
{
    E_Condition result = EC_IllegalCall;
    if (TreeNode != NULL)
    {
        if (TreeNode->getValueType() == VT_Composite)
            result = ((DSRCompositeTreeNode *)TreeNode)->getValue(referenceValue);
        else
            referenceValue.clear();
    } else
        referenceValue.clear();
    return result;
}


E_Condition DSRContentItem::setReference(const DSRReferenceValue &referenceValue)
{
    E_Condition result = EC_IllegalCall;
    if (TreeNode != NULL)
    {
        if (TreeNode->getValueType() == VT_Composite)
        {
            ((DSRCompositeTreeNode *)TreeNode)->setValue(referenceValue);
            result = EC_Normal;
        }
    }
    return result;
}


const DSRImageReferenceValue &DSRContentItem::getImageReference() const
{
    if (TreeNode != NULL)
    {
        if (TreeNode->getValueType() == VT_Image)
            return ((DSRImageTreeNode *)TreeNode)->getValue();
    }
    return EmptyImageReference;
}


E_Condition DSRContentItem::getImageReference(DSRImageReferenceValue &referenceValue) const
{
    E_Condition result = EC_IllegalCall;
    if (TreeNode != NULL)
    {
        if (TreeNode->getValueType() == VT_Image)
            result = ((DSRImageTreeNode *)TreeNode)->getValue(referenceValue);
        else
            referenceValue.clear();
    } else
        referenceValue.clear();
    return result;
}


E_Condition DSRContentItem::setImageReference(const DSRImageReferenceValue &referenceValue)
{
    E_Condition result = EC_IllegalCall;
    if (TreeNode != NULL)
    {
        if (TreeNode->getValueType() == VT_Image)
        {
            ((DSRImageTreeNode *)TreeNode)->setValue(referenceValue);
            result = EC_Normal;
        }
    }
    return result;
}


const DSRWaveformReferenceValue &DSRContentItem::getWaveformReference() const
{
    if (TreeNode != NULL)
    {
        if (TreeNode->getValueType() == VT_Waveform)
            return ((DSRWaveformTreeNode *)TreeNode)->getValue();
    }
    return EmptyWaveformReference;
}


E_Condition DSRContentItem::getWaveformReference(DSRWaveformReferenceValue &referenceValue) const
{
    E_Condition result = EC_IllegalCall;
    if (TreeNode != NULL)
    {
        if (TreeNode->getValueType() == VT_Waveform)
            result = ((DSRWaveformTreeNode *)TreeNode)->getValue(referenceValue);
        else
            referenceValue.clear();
    } else
        referenceValue.clear();
    return result;
}


E_Condition DSRContentItem::setWaveformReference(const DSRWaveformReferenceValue &referenceValue)
{
    E_Condition result = EC_IllegalCall;
    if (TreeNode != NULL)
    {
        if (TreeNode->getValueType() == VT_Waveform)
        {
            ((DSRWaveformTreeNode *)TreeNode)->setValue(referenceValue);
            result = EC_Normal;
        }
    }
    return result;
}


DSRTypes::E_ContinuityOfContent DSRContentItem::getContinuityOfContent() const
{
    E_ContinuityOfContent result = COC_invalid;
    if (TreeNode != NULL)
    {
        if (TreeNode->getValueType() == VT_Container)
            result = ((DSRContainerTreeNode *)TreeNode)->getContinuityOfContent();
    }
    return result;
}


E_Condition DSRContentItem::setContinuityOfContent(const E_ContinuityOfContent continuityOfContent)
{
    E_Condition result = EC_IllegalCall;
    if (TreeNode != NULL)
    {
        if (TreeNode->getValueType() == VT_Container)
            result = ((DSRContainerTreeNode *)TreeNode)->setContinuityOfContent(continuityOfContent);
    }
    return result;
}


const DSRCodedEntryValue &DSRContentItem::getConceptName() const
{
    if (TreeNode != NULL)
        return TreeNode->getConceptName();
    return EmptyCodedEntry;
}


E_Condition DSRContentItem::getConceptName(DSRCodedEntryValue &conceptName) const
{
    E_Condition result = EC_IllegalCall;
    if (TreeNode != NULL)
        result = TreeNode->getConceptName(conceptName);
    else
        conceptName.clear();
    return result;
}


E_Condition DSRContentItem::setConceptName(const DSRCodedEntryValue &conceptName)
{
    E_Condition result = EC_IllegalCall;
    if (TreeNode != NULL)
        result = TreeNode->setConceptName(conceptName);
    return result;
}


const OFString &DSRContentItem::getObservationDateTime() const
{
    if (TreeNode != NULL)
        return TreeNode->getObservationDateTime();
    return EmptyString;
}


E_Condition DSRContentItem::setObservationDateTime(const OFString &observationDateTime)
{
    E_Condition result = EC_IllegalCall;
    if (TreeNode != NULL)
        result = TreeNode->setObservationDateTime(observationDateTime);
    return result;
}




/*
 *  CVS/RCS Log:
 *  $Log: dsrcitem.cc,v $
 *  Revision 1.1  2000-10-13 07:52:16  joergr
 *  Added new module 'dcmsr' providing access to DICOM structured reporting
 *  documents (supplement 23).  Doc++ documentation not yet completed.
 *
 *
 */
