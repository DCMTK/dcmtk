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
 *  Update Date:      $Date: 2001-01-18 15:54:12 $
 *  CVS/RCS Revision: $Revision: 1.9 $
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
#include "dsrtcotn.h"
#include "dsrcomtn.h"
#include "dsrimgtn.h"
#include "dsrwavtn.h"
#include "dsrcontn.h"
#include "dsrreftn.h"


const OFString DSRContentItem::EmptyString;
const DSRCodedEntryValue DSRContentItem::EmptyCodedEntry;
const DSRNumericMeasurementValue DSRContentItem::EmptyNumericMeasurement;
const DSRSpatialCoordinatesValue DSRContentItem::EmptySpatialCoordinates;
const DSRTemporalCoordinatesValue DSRContentItem::EmptyTemporalCoordinates;
const DSRCompositeReferenceValue DSRContentItem::EmptyCompositeReference;
const DSRImageReferenceValue DSRContentItem::EmptyImageReference;
const DSRWaveformReferenceValue DSRContentItem::EmptyWaveformReference;


DSRContentItem::DSRContentItem()
  : TreeNode(NULL)
{
}


DSRContentItem::~DSRContentItem()
{
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
            nodeID = ((DSRByReferenceTreeNode *)TreeNode)->getReferencedNodeID();
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
                return ((DSRTextTreeNode *)TreeNode)->getValue();
            case VT_DateTime:
                return ((DSRDateTimeTreeNode *)TreeNode)->getValue();
            case VT_Date:
                return ((DSRDateTreeNode *)TreeNode)->getValue();
            case VT_Time:
                return ((DSRTimeTreeNode *)TreeNode)->getValue();
            case VT_UIDRef:
                return ((DSRUIDRefTreeNode *)TreeNode)->getValue();
            case VT_PName:
                return ((DSRPNameTreeNode *)TreeNode)->getValue();
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


DSRCodedEntryValue *DSRContentItem::getCodeValuePtr()
{
    DSRCodedEntryValue *pointer = NULL;
    if (TreeNode != NULL)
    {
        if (TreeNode->getValueType() == VT_Code)
            pointer = ((DSRCodeTreeNode *)TreeNode)->getValuePtr();
    }
    return pointer;
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


DSRNumericMeasurementValue *DSRContentItem::getNumericValuePtr()
{
    DSRNumericMeasurementValue *pointer = NULL;
    if (TreeNode != NULL)
    {
        if (TreeNode->getValueType() == VT_Num)
            pointer = ((DSRNumTreeNode *)TreeNode)->getValuePtr();
    }
    return pointer;
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


DSRSpatialCoordinatesValue *DSRContentItem::getSpatialCoordinatesPtr()
{
    DSRSpatialCoordinatesValue *pointer = NULL;
    if (TreeNode != NULL)
    {
        if (TreeNode->getValueType() == VT_SCoord)
            pointer = ((DSRSCoordTreeNode *)TreeNode)->getValuePtr();
    }
    return pointer;
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


DSRTemporalCoordinatesValue *DSRContentItem::getTemporalCoordinatesPtr()
{
    DSRTemporalCoordinatesValue *pointer = NULL;
    if (TreeNode != NULL)
    {
        if (TreeNode->getValueType() == VT_TCoord)
            pointer = ((DSRTCoordTreeNode *)TreeNode)->getValuePtr();
    }
    return pointer;
}


const DSRTemporalCoordinatesValue &DSRContentItem::getTemporalCoordinates() const
{
    if (TreeNode != NULL)
    {
        if (TreeNode->getValueType() == VT_TCoord)
            return ((DSRTCoordTreeNode *)TreeNode)->getValue();
    }
    return EmptyTemporalCoordinates;
}


E_Condition DSRContentItem::getTemporalCoordinates(DSRTemporalCoordinatesValue &coordinatesValue) const
{
    E_Condition result = EC_IllegalCall;
    if (TreeNode != NULL)
    {
        if (TreeNode->getValueType() == VT_TCoord)
            result= ((DSRTCoordTreeNode *)TreeNode)->getValue(coordinatesValue);
        else
            coordinatesValue.clear();
    } else
        coordinatesValue.clear();
    return result;
}


E_Condition DSRContentItem::setTemporalCoordinates(const DSRTemporalCoordinatesValue &coordinatesValue)
{
    E_Condition result = EC_IllegalCall;
    if (TreeNode != NULL)
    {
        if (TreeNode->getValueType() == VT_TCoord)
            result = ((DSRTCoordTreeNode *)TreeNode)->setValue(coordinatesValue);
    }
    return result;
}


DSRCompositeReferenceValue *DSRContentItem::getCompositeReferencePtr()
{
    DSRCompositeReferenceValue *pointer = NULL;
    if (TreeNode != NULL)
    {
        if (TreeNode->getValueType() == VT_Composite)
            pointer = ((DSRCompositeTreeNode *)TreeNode)->getValuePtr();
    }
    return pointer;
}


const DSRCompositeReferenceValue &DSRContentItem::getCompositeReference() const
{
    if (TreeNode != NULL)
    {
        if (TreeNode->getValueType() == VT_Composite)
            return ((DSRCompositeTreeNode *)TreeNode)->getValue();
    }
    return EmptyCompositeReference;
}


E_Condition DSRContentItem::getCompositeReference(DSRCompositeReferenceValue &referenceValue) const
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


E_Condition DSRContentItem::setCompositeReference(const DSRCompositeReferenceValue &referenceValue)
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


DSRImageReferenceValue *DSRContentItem::getImageReferencePtr()
{
    DSRImageReferenceValue *pointer = NULL;
    if (TreeNode != NULL)
    {
        if (TreeNode->getValueType() == VT_Image)
            pointer = ((DSRImageTreeNode *)TreeNode)->getValuePtr();
    }
    return pointer;
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


DSRWaveformReferenceValue *DSRContentItem::getWaveformReferencePtr()
{
    DSRWaveformReferenceValue *pointer = NULL;
    if (TreeNode != NULL)
    {
        if (TreeNode->getValueType() == VT_Waveform)
            pointer = ((DSRWaveformTreeNode *)TreeNode)->getValuePtr();
    }
    return pointer;
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
 *  Revision 1.9  2001-01-18 15:54:12  joergr
 *  Added support for digital signatures.
 *
 *  Revision 1.8  2000/11/20 13:22:23  joergr
 *  Fixed minor bugs (string related memory problems when used with JNI).
 *
 *  Revision 1.7  2000/11/07 18:26:45  joergr
 *  Enhanced support for by-reference relationships.
 *
 *  Revision 1.6  2000/10/26 14:26:00  joergr
 *  Added support for "Comprehensive SR".
 *  Added support for TCOORD content item.
 *
 *  Revision 1.5  2000/10/20 10:14:57  joergr
 *  Renamed class DSRReferenceValue to DSRCompositeReferenceValue.
 *
 *  Revision 1.4  2000/10/18 17:12:06  joergr
 *  Added methods allowing direct access to certain content item values.
 *
 *  Revision 1.3  2000/10/17 12:35:41  joergr
 *  Added method checking content item for validity/completeness.
 *  Renamed methods for composite objects.
 *
 *  Revision 1.2  2000/10/16 12:01:03  joergr
 *  Added methods allowing direct access to certain content item values.
 *
 *  Revision 1.1  2000/10/13 07:52:16  joergr
 *  Added new module 'dcmsr' providing access to DICOM structured reporting
 *  documents (supplement 23).  Doc++ documentation not yet completed.
 *
 *
 */
