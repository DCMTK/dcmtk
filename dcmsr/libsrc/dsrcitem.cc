/*
 *
 *  Copyright (C) 2000-2017, OFFIS e.V.
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


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/dsrcitem.h"
#include "dcmtk/dcmsr/dsrtextn.h"
#include "dcmtk/dcmsr/dsrcodtn.h"
#include "dcmtk/dcmsr/dsrnumtn.h"
#include "dcmtk/dcmsr/dsrdtitn.h"
#include "dcmtk/dcmsr/dsrdattn.h"
#include "dcmtk/dcmsr/dsrtimtn.h"
#include "dcmtk/dcmsr/dsruidtn.h"
#include "dcmtk/dcmsr/dsrpnmtn.h"
#include "dcmtk/dcmsr/dsrscotn.h"
#include "dcmtk/dcmsr/dsrsc3tn.h"
#include "dcmtk/dcmsr/dsrtcotn.h"
#include "dcmtk/dcmsr/dsrcomtn.h"
#include "dcmtk/dcmsr/dsrimgtn.h"
#include "dcmtk/dcmsr/dsrwavtn.h"
#include "dcmtk/dcmsr/dsrcontn.h"
#include "dcmtk/dcmsr/dsrreftn.h"


// definition of empty default objects
const OFString                     DSRContentItem::EmptyString;
const DSRCodedEntryValue           DSRContentItem::EmptyCodedEntry;
const DSRSpatialCoordinatesValue   DSRContentItem::EmptySpatialCoordinates;
const DSRSpatialCoordinates3DValue DSRContentItem::EmptySpatialCoordinates3D;
const DSRTemporalCoordinatesValue  DSRContentItem::EmptyTemporalCoordinates;
const DSRCompositeReferenceValue   DSRContentItem::EmptyCompositeReference;
const DSRImageReferenceValue       DSRContentItem::EmptyImageReference;
const DSRWaveformReferenceValue    DSRContentItem::EmptyWaveformReference;


DSRContentItem::DSRContentItem()
  : TreeNode(NULL),
    DeleteTreeNode(OFFalse),
    EmptyNumericMeasurement()
{
}


DSRContentItem::DSRContentItem(const DSRContentItem &item)
  : TreeNode(NULL),
    DeleteTreeNode(OFTrue),
    EmptyNumericMeasurement()
{
    if (item.TreeNode != NULL)
        TreeNode = item.TreeNode->clone();
}


DSRContentItem::~DSRContentItem()
{
    if (DeleteTreeNode)
        delete TreeNode;
}


OFBool DSRContentItem::operator==(const DSRContentItem &item) const
{
    OFBool result = (TreeNode == item.TreeNode);
    /* call comparison operator (if referenced tree nodes are not the same) */
    if (!result && (TreeNode != NULL) && (item.TreeNode != NULL))
        result = TreeNode->operator==(*item.TreeNode);
    return result;
}


OFBool DSRContentItem::operator!=(const DSRContentItem &item) const
{
    OFBool result = (TreeNode != item.TreeNode);
    /* call comparison operator (if referenced tree nodes are not the same) */
    if (result && (TreeNode != NULL) && (item.TreeNode != NULL))
        result = TreeNode->operator!=(*item.TreeNode);
    return result;
}


OFBool DSRContentItem::isValid() const
{
    OFBool result = OFFalse;
    if (TreeNode != NULL)
        result = TreeNode->isValid();
    return result;
}


OFBool DSRContentItem::isMarked() const
{
    OFBool result = OFFalse;
    if (TreeNode != NULL)
        result = TreeNode->isMarked();
    return result;
}


void DSRContentItem::setMark(const OFBool flag)
{
    if (TreeNode != NULL)
        TreeNode->setMark(flag);
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


size_t DSRContentItem::getReferencedNodeID() const
{
    size_t nodeID = 0;
    if (TreeNode != NULL)
    {
        if (TreeNode->getValueType() == VT_byReference)
            nodeID = OFstatic_cast(DSRByReferenceTreeNode *, TreeNode)->getReferencedNodeID();
    }
    return nodeID;
}


const OFString &DSRContentItem::getStringValue() const
{
    if (TreeNode != NULL)
    {
        switch (TreeNode->getValueType())
        {
            case VT_Text:
                return OFstatic_cast(DSRTextTreeNode *, TreeNode)->getValue();
            case VT_DateTime:
                return OFstatic_cast(DSRDateTimeTreeNode *, TreeNode)->getValue();
            case VT_Date:
                return OFstatic_cast(DSRDateTreeNode *, TreeNode)->getValue();
            case VT_Time:
                return OFstatic_cast(DSRTimeTreeNode *, TreeNode)->getValue();
            case VT_UIDRef:
                return OFstatic_cast(DSRUIDRefTreeNode *, TreeNode)->getValue();
            case VT_PName:
                return OFstatic_cast(DSRPNameTreeNode *, TreeNode)->getValue();
            default:
                break;
        }
    }
    return EmptyString;
}


OFCondition DSRContentItem::setStringValue(const OFString &stringValue,
                                           const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (TreeNode != NULL)
    {
        switch (TreeNode->getValueType())
        {
            case VT_Text:
                result = OFstatic_cast(DSRTextTreeNode *, TreeNode)->setValue(stringValue, check);
                break;
            case VT_DateTime:
                result = OFstatic_cast(DSRDateTimeTreeNode *, TreeNode)->setValue(stringValue, check);
                break;
            case VT_Date:
                result = OFstatic_cast(DSRDateTreeNode *, TreeNode)->setValue(stringValue, check);
                break;
            case VT_Time:
                result = OFstatic_cast(DSRTimeTreeNode *, TreeNode)->setValue(stringValue, check);
                break;
            case VT_UIDRef:
                result = OFstatic_cast(DSRUIDRefTreeNode *, TreeNode)->setValue(stringValue, check);
                break;
            case VT_PName:
                result = OFstatic_cast(DSRPNameTreeNode *, TreeNode)->setValue(stringValue, check);
                break;
            default:
                break;
        }
    }
    return result;
}


DSRCodedEntryValue *DSRContentItem::getCodeValuePtr()
{
    DSRCodedEntryValue *pointer = NULL;
    if (TreeNode != NULL)
    {
        if (TreeNode->getValueType() == VT_Code)
            pointer = OFstatic_cast(DSRCodeTreeNode *, TreeNode)->getValuePtr();
    }
    return pointer;
}


const DSRCodedEntryValue &DSRContentItem::getCodeValue() const
{
    if (TreeNode != NULL)
    {
        if (TreeNode->getValueType() == VT_Code)
            return OFstatic_cast(DSRCodeTreeNode *, TreeNode)->getValue();
    }
    return EmptyCodedEntry;
}


OFCondition DSRContentItem::getCodeValue(DSRCodedEntryValue &codeValue) const
{
    OFCondition result = EC_IllegalCall;
    if (TreeNode != NULL)
    {
        if (TreeNode->getValueType() == VT_Code)
            result= OFstatic_cast(DSRCodeTreeNode *, TreeNode)->getValue(codeValue);
        else
            codeValue.clear();
    } else
        codeValue.clear();
    return result;
}


OFCondition DSRContentItem::setCodeValue(const DSRCodedEntryValue &codeValue,
                                         const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (TreeNode != NULL)
    {
        if (TreeNode->getValueType() == VT_Code)
            result = OFstatic_cast(DSRCodeTreeNode *, TreeNode)->setValue(codeValue, check);
    }
    return result;
}


DSRNumericMeasurementValue *DSRContentItem::getNumericValuePtr()
{
    DSRNumericMeasurementValue *pointer = NULL;
    if (TreeNode != NULL)
    {
        if (TreeNode->getValueType() == VT_Num)
            pointer = OFstatic_cast(DSRNumTreeNode *, TreeNode)->getValuePtr();
    }
    return pointer;
}


const DSRNumericMeasurementValue &DSRContentItem::getNumericValue() const
{
    if (TreeNode != NULL)
    {
        if (TreeNode->getValueType() == VT_Num)
            return OFstatic_cast(DSRNumTreeNode *, TreeNode)->getValue();
    }
    return EmptyNumericMeasurement;
}


OFCondition DSRContentItem::getNumericValue(DSRNumericMeasurementValue &numericValue) const
{
    OFCondition result = EC_IllegalCall;
    if (TreeNode != NULL)
    {
        if (TreeNode->getValueType() == VT_Num)
            result= OFstatic_cast(DSRNumTreeNode *, TreeNode)->getValue(numericValue);
        else
            numericValue.clear();
    } else
        numericValue.clear();
    return result;
}


OFCondition DSRContentItem::setNumericValue(const DSRNumericMeasurementValue &numericValue,
                                            const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (TreeNode != NULL)
    {
        if (TreeNode->getValueType() == VT_Num)
            result = OFstatic_cast(DSRNumTreeNode *, TreeNode)->setValue(numericValue, check);
    }
    return result;
}


DSRSpatialCoordinatesValue *DSRContentItem::getSpatialCoordinatesPtr()
{
    DSRSpatialCoordinatesValue *pointer = NULL;
    if (TreeNode != NULL)
    {
        if (TreeNode->getValueType() == VT_SCoord)
            pointer = OFstatic_cast(DSRSCoordTreeNode *, TreeNode)->getValuePtr();
    }
    return pointer;
}


const DSRSpatialCoordinatesValue &DSRContentItem::getSpatialCoordinates() const
{
    if (TreeNode != NULL)
    {
        if (TreeNode->getValueType() == VT_SCoord)
            return OFstatic_cast(DSRSCoordTreeNode *, TreeNode)->getValue();
    }
    return EmptySpatialCoordinates;
}


OFCondition DSRContentItem::getSpatialCoordinates(DSRSpatialCoordinatesValue &coordinatesValue) const
{
    OFCondition result = EC_IllegalCall;
    if (TreeNode != NULL)
    {
        if (TreeNode->getValueType() == VT_SCoord)
            result= OFstatic_cast(DSRSCoordTreeNode *, TreeNode)->getValue(coordinatesValue);
        else
            coordinatesValue.clear();
    } else
        coordinatesValue.clear();
    return result;
}


OFCondition DSRContentItem::setSpatialCoordinates(const DSRSpatialCoordinatesValue &coordinatesValue,
                                                  const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (TreeNode != NULL)
    {
        if (TreeNode->getValueType() == VT_SCoord)
            result = OFstatic_cast(DSRSCoordTreeNode *, TreeNode)->setValue(coordinatesValue, check);
    }
    return result;
}


DSRSpatialCoordinates3DValue *DSRContentItem::getSpatialCoordinates3DPtr()
{
    DSRSpatialCoordinates3DValue *pointer = NULL;
    if (TreeNode != NULL)
    {
        if (TreeNode->getValueType() == VT_SCoord3D)
            pointer = OFstatic_cast(DSRSCoord3DTreeNode *, TreeNode)->getValuePtr();
    }
    return pointer;
}


const DSRSpatialCoordinates3DValue &DSRContentItem::getSpatialCoordinates3D() const
{
    if (TreeNode != NULL)
    {
        if (TreeNode->getValueType() == VT_SCoord3D)
            return OFstatic_cast(DSRSCoord3DTreeNode *, TreeNode)->getValue();
    }
    return EmptySpatialCoordinates3D;
}


OFCondition DSRContentItem::getSpatialCoordinates3D(DSRSpatialCoordinates3DValue &coordinatesValue) const
{
    OFCondition result = EC_IllegalCall;
    if (TreeNode != NULL)
    {
        if (TreeNode->getValueType() == VT_SCoord3D)
            result= OFstatic_cast(DSRSCoord3DTreeNode *, TreeNode)->getValue(coordinatesValue);
        else
            coordinatesValue.clear();
    } else
        coordinatesValue.clear();
    return result;
}


OFCondition DSRContentItem::setSpatialCoordinates3D(const DSRSpatialCoordinates3DValue &coordinatesValue,
                                                    const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (TreeNode != NULL)
    {
        if (TreeNode->getValueType() == VT_SCoord3D)
            result = OFstatic_cast(DSRSCoord3DTreeNode *, TreeNode)->setValue(coordinatesValue, check);
    }
    return result;
}


DSRTemporalCoordinatesValue *DSRContentItem::getTemporalCoordinatesPtr()
{
    DSRTemporalCoordinatesValue *pointer = NULL;
    if (TreeNode != NULL)
    {
        if (TreeNode->getValueType() == VT_TCoord)
            pointer = OFstatic_cast(DSRTCoordTreeNode *, TreeNode)->getValuePtr();
    }
    return pointer;
}


const DSRTemporalCoordinatesValue &DSRContentItem::getTemporalCoordinates() const
{
    if (TreeNode != NULL)
    {
        if (TreeNode->getValueType() == VT_TCoord)
            return OFstatic_cast(DSRTCoordTreeNode *, TreeNode)->getValue();
    }
    return EmptyTemporalCoordinates;
}


OFCondition DSRContentItem::getTemporalCoordinates(DSRTemporalCoordinatesValue &coordinatesValue) const
{
    OFCondition result = EC_IllegalCall;
    if (TreeNode != NULL)
    {
        if (TreeNode->getValueType() == VT_TCoord)
            result= OFstatic_cast(DSRTCoordTreeNode *, TreeNode)->getValue(coordinatesValue);
        else
            coordinatesValue.clear();
    } else
        coordinatesValue.clear();
    return result;
}


OFCondition DSRContentItem::setTemporalCoordinates(const DSRTemporalCoordinatesValue &coordinatesValue,
                                                   const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (TreeNode != NULL)
    {
        if (TreeNode->getValueType() == VT_TCoord)
            result = OFstatic_cast(DSRTCoordTreeNode *, TreeNode)->setValue(coordinatesValue, check);
    }
    return result;
}


DSRCompositeReferenceValue *DSRContentItem::getCompositeReferencePtr()
{
    DSRCompositeReferenceValue *pointer = NULL;
    if (TreeNode != NULL)
    {
        if (TreeNode->getValueType() == VT_Composite)
            pointer = OFstatic_cast(DSRCompositeTreeNode *, TreeNode)->getValuePtr();
    }
    return pointer;
}


const DSRCompositeReferenceValue &DSRContentItem::getCompositeReference() const
{
    if (TreeNode != NULL)
    {
        if (TreeNode->getValueType() == VT_Composite)
            return OFstatic_cast(DSRCompositeTreeNode *, TreeNode)->getValue();
    }
    return EmptyCompositeReference;
}


OFCondition DSRContentItem::getCompositeReference(DSRCompositeReferenceValue &referenceValue) const
{
    OFCondition result = EC_IllegalCall;
    if (TreeNode != NULL)
    {
        if (TreeNode->getValueType() == VT_Composite)
            result = OFstatic_cast(DSRCompositeTreeNode *, TreeNode)->getValue(referenceValue);
        else
            referenceValue.clear();
    } else
        referenceValue.clear();
    return result;
}


OFCondition DSRContentItem::setCompositeReference(const DSRCompositeReferenceValue &referenceValue,
                                                  const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (TreeNode != NULL)
    {
        if (TreeNode->getValueType() == VT_Composite)
            result = OFstatic_cast(DSRCompositeTreeNode *, TreeNode)->setValue(referenceValue, check);
    }
    return result;
}


DSRImageReferenceValue *DSRContentItem::getImageReferencePtr()
{
    DSRImageReferenceValue *pointer = NULL;
    if (TreeNode != NULL)
    {
        if (TreeNode->getValueType() == VT_Image)
            pointer = OFstatic_cast(DSRImageTreeNode *, TreeNode)->getValuePtr();
    }
    return pointer;
}


const DSRImageReferenceValue &DSRContentItem::getImageReference() const
{
    if (TreeNode != NULL)
    {
        if (TreeNode->getValueType() == VT_Image)
            return OFstatic_cast(DSRImageTreeNode *, TreeNode)->getValue();
    }
    return EmptyImageReference;
}


OFCondition DSRContentItem::getImageReference(DSRImageReferenceValue &referenceValue) const
{
    OFCondition result = EC_IllegalCall;
    if (TreeNode != NULL)
    {
        if (TreeNode->getValueType() == VT_Image)
            result = OFstatic_cast(DSRImageTreeNode *, TreeNode)->getValue(referenceValue);
        else
            referenceValue.clear();
    } else
        referenceValue.clear();
    return result;
}


OFCondition DSRContentItem::setImageReference(const DSRImageReferenceValue &referenceValue,
                                              const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (TreeNode != NULL)
    {
        if (TreeNode->getValueType() == VT_Image)
            result = OFstatic_cast(DSRImageTreeNode *, TreeNode)->setValue(referenceValue, check);
    }
    return result;
}


DSRWaveformReferenceValue *DSRContentItem::getWaveformReferencePtr()
{
    DSRWaveformReferenceValue *pointer = NULL;
    if (TreeNode != NULL)
    {
        if (TreeNode->getValueType() == VT_Waveform)
            pointer = OFstatic_cast(DSRWaveformTreeNode *, TreeNode)->getValuePtr();
    }
    return pointer;
}


const DSRWaveformReferenceValue &DSRContentItem::getWaveformReference() const
{
    if (TreeNode != NULL)
    {
        if (TreeNode->getValueType() == VT_Waveform)
            return OFstatic_cast(DSRWaveformTreeNode *, TreeNode)->getValue();
    }
    return EmptyWaveformReference;
}


OFCondition DSRContentItem::getWaveformReference(DSRWaveformReferenceValue &referenceValue) const
{
    OFCondition result = EC_IllegalCall;
    if (TreeNode != NULL)
    {
        if (TreeNode->getValueType() == VT_Waveform)
            result = OFstatic_cast(DSRWaveformTreeNode *, TreeNode)->getValue(referenceValue);
        else
            referenceValue.clear();
    } else
        referenceValue.clear();
    return result;
}


OFCondition DSRContentItem::setWaveformReference(const DSRWaveformReferenceValue &referenceValue,
                                                 const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (TreeNode != NULL)
    {
        if (TreeNode->getValueType() == VT_Waveform)
            result = OFstatic_cast(DSRWaveformTreeNode *, TreeNode)->setValue(referenceValue, check);
    }
    return result;
}


DSRTypes::E_ContinuityOfContent DSRContentItem::getContinuityOfContent() const
{
    E_ContinuityOfContent result = COC_invalid;
    if (TreeNode != NULL)
    {
        if (TreeNode->getValueType() == VT_Container)
            result = OFstatic_cast(DSRContainerTreeNode *, TreeNode)->getContinuityOfContent();
    }
    return result;
}


OFCondition DSRContentItem::setContinuityOfContent(const E_ContinuityOfContent continuityOfContent,
                                                   const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (TreeNode != NULL)
    {
        if (TreeNode->getValueType() == VT_Container)
            result = OFstatic_cast(DSRContainerTreeNode *, TreeNode)->setContinuityOfContent(continuityOfContent, check);
    }
    return result;
}


OFCondition DSRContentItem::getTemplateIdentification(OFString &templateIdentifier,
                                                      OFString &mappingResource) const
{
    OFCondition result = EC_IllegalCall;
    if (TreeNode != NULL)
    {
        if (TreeNode->getValueType() == VT_Container)
            result = TreeNode->getTemplateIdentification(templateIdentifier, mappingResource);
    }
    return result;
}


OFCondition DSRContentItem::getTemplateIdentification(OFString &templateIdentifier,
                                                      OFString &mappingResource,
                                                      OFString &mappingResourceUID) const
{
    OFCondition result = EC_IllegalCall;
    if (TreeNode != NULL)
    {
        if (TreeNode->getValueType() == VT_Container)
            result = TreeNode->getTemplateIdentification(templateIdentifier, mappingResource, mappingResourceUID);
    }
    return result;
}


OFCondition DSRContentItem::setTemplateIdentification(const OFString &templateIdentifier,
                                                      const OFString &mappingResource,
                                                      const OFString &mappingResourceUID,
                                                      const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (TreeNode != NULL)
    {
        if (TreeNode->getValueType() == VT_Container)
            result = TreeNode->setTemplateIdentification(templateIdentifier, mappingResource, mappingResourceUID, check);
    }
    return result;
}


DSRCodedEntryValue *DSRContentItem::getConceptNamePtr()
{
    DSRCodedEntryValue *pointer = NULL;
    if (TreeNode != NULL)
        pointer = TreeNode->getConceptNamePtr();
    return pointer;
}


const DSRCodedEntryValue &DSRContentItem::getConceptName() const
{
    if (TreeNode != NULL)
        return TreeNode->getConceptName();
    return EmptyCodedEntry;
}


OFCondition DSRContentItem::getConceptName(DSRCodedEntryValue &conceptName) const
{
    OFCondition result = EC_IllegalCall;
    if (TreeNode != NULL)
        result = TreeNode->getConceptName(conceptName);
    else
        conceptName.clear();
    return result;
}


OFCondition DSRContentItem::setConceptName(const DSRCodedEntryValue &conceptName,
                                           const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (TreeNode != NULL)
        result = TreeNode->setConceptName(conceptName, check);
    return result;
}


const OFString &DSRContentItem::getObservationDateTime() const
{
    if (TreeNode != NULL)
        return TreeNode->getObservationDateTime();
    return EmptyString;
}


OFCondition DSRContentItem::setObservationDateTime(const OFString &observationDateTime,
                                                   const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (TreeNode != NULL)
        result = TreeNode->setObservationDateTime(observationDateTime, check);
    return result;
}


const OFString &DSRContentItem::getObservationUID() const
{
    if (TreeNode != NULL)
        return TreeNode->getObservationUID();
    return EmptyString;
}


OFCondition DSRContentItem::setObservationUID(const OFString &observationUID,
                                              const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (TreeNode != NULL)
        result = TreeNode->setObservationUID(observationUID, check);
    return result;
}


const OFString &DSRContentItem::getAnnotationText() const
{
    if (TreeNode != NULL)
        return TreeNode->getAnnotation();
    return EmptyString;
}


OFCondition DSRContentItem::setAnnotationText(const OFString &annotationText) const
{
    OFCondition result = EC_IllegalCall;
    if (TreeNode != NULL)
    {
        TreeNode->setAnnotation(annotationText);
        result = EC_Normal;
    }
    return result;
}


// protected methods

void DSRContentItem::swap(DSRContentItem &item)
{
    /* swap members */
    OFswap(TreeNode, item.TreeNode);
    OFswap(DeleteTreeNode, item.DeleteTreeNode);
}
