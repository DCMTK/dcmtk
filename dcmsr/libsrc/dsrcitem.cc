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
 *  Module:  dcmsr
 *
 *  Author:  Joerg Riesmeier
 *
 *  Purpose:
 *    classes: DSRContentItem
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:14:40 $
 *  CVS/RCS Revision: $Revision: 1.15 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
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
#include "dcmtk/dcmsr/dsrtcotn.h"
#include "dcmtk/dcmsr/dsrcomtn.h"
#include "dcmtk/dcmsr/dsrimgtn.h"
#include "dcmtk/dcmsr/dsrwavtn.h"
#include "dcmtk/dcmsr/dsrcontn.h"
#include "dcmtk/dcmsr/dsrreftn.h"


// definition of empty default objects
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


OFCondition DSRContentItem::setStringValue(const OFString &stringValue)
{
    OFCondition result = EC_IllegalCall;
    if (TreeNode != NULL)
    {
        switch (TreeNode->getValueType())
        {
            case VT_Text:
                result = OFstatic_cast(DSRTextTreeNode *, TreeNode)->setValue(stringValue);
                break;
            case VT_DateTime:
                result = OFstatic_cast(DSRDateTimeTreeNode *, TreeNode)->setValue(stringValue);
                break;
            case VT_Date:
                result = OFstatic_cast(DSRDateTreeNode *, TreeNode)->setValue(stringValue);
                break;
            case VT_Time:
                result = OFstatic_cast(DSRTimeTreeNode *, TreeNode)->setValue(stringValue);
                break;
            case VT_UIDRef:
                result = OFstatic_cast(DSRUIDRefTreeNode *, TreeNode)->setValue(stringValue);
                break;
            case VT_PName:
                result = OFstatic_cast(DSRPNameTreeNode *, TreeNode)->setValue(stringValue);
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


OFCondition DSRContentItem::setCodeValue(const DSRCodedEntryValue &codeValue)
{
    OFCondition result = EC_IllegalCall;
    if (TreeNode != NULL)
    {
        if (TreeNode->getValueType() == VT_Code)
            result = OFstatic_cast(DSRCodeTreeNode *, TreeNode)->setValue(codeValue);
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


OFCondition DSRContentItem::setNumericValue(const DSRNumericMeasurementValue &numericValue)
{
    OFCondition result = EC_IllegalCall;
    if (TreeNode != NULL)
    {
        if (TreeNode->getValueType() == VT_Num)
            result = OFstatic_cast(DSRNumTreeNode *, TreeNode)->setValue(numericValue);
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


OFCondition DSRContentItem::setSpatialCoordinates(const DSRSpatialCoordinatesValue &coordinatesValue)
{
    OFCondition result = EC_IllegalCall;
    if (TreeNode != NULL)
    {
        if (TreeNode->getValueType() == VT_SCoord)
            result = OFstatic_cast(DSRSCoordTreeNode *, TreeNode)->setValue(coordinatesValue);
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


OFCondition DSRContentItem::setTemporalCoordinates(const DSRTemporalCoordinatesValue &coordinatesValue)
{
    OFCondition result = EC_IllegalCall;
    if (TreeNode != NULL)
    {
        if (TreeNode->getValueType() == VT_TCoord)
            result = OFstatic_cast(DSRTCoordTreeNode *, TreeNode)->setValue(coordinatesValue);
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


OFCondition DSRContentItem::setCompositeReference(const DSRCompositeReferenceValue &referenceValue)
{
    OFCondition result = EC_IllegalCall;
    if (TreeNode != NULL)
    {
        if (TreeNode->getValueType() == VT_Composite)
        {
            OFstatic_cast(DSRCompositeTreeNode *, TreeNode)->setValue(referenceValue);
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


OFCondition DSRContentItem::setImageReference(const DSRImageReferenceValue &referenceValue)
{
    OFCondition result = EC_IllegalCall;
    if (TreeNode != NULL)
    {
        if (TreeNode->getValueType() == VT_Image)
        {
            OFstatic_cast(DSRImageTreeNode *, TreeNode)->setValue(referenceValue);
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


OFCondition DSRContentItem::setWaveformReference(const DSRWaveformReferenceValue &referenceValue)
{
    OFCondition result = EC_IllegalCall;
    if (TreeNode != NULL)
    {
        if (TreeNode->getValueType() == VT_Waveform)
        {
            OFstatic_cast(DSRWaveformTreeNode *, TreeNode)->setValue(referenceValue);
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
            result = OFstatic_cast(DSRContainerTreeNode *, TreeNode)->getContinuityOfContent();
    }
    return result;
}


OFCondition DSRContentItem::setContinuityOfContent(const E_ContinuityOfContent continuityOfContent)
{
    OFCondition result = EC_IllegalCall;
    if (TreeNode != NULL)
    {
        if (TreeNode->getValueType() == VT_Container)
            result = OFstatic_cast(DSRContainerTreeNode *, TreeNode)->setContinuityOfContent(continuityOfContent);
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


OFCondition DSRContentItem::setConceptName(const DSRCodedEntryValue &conceptName)
{
    OFCondition result = EC_IllegalCall;
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


OFCondition DSRContentItem::setObservationDateTime(const OFString &observationDateTime)
{
    OFCondition result = EC_IllegalCall;
    if (TreeNode != NULL)
        result = TreeNode->setObservationDateTime(observationDateTime);
    return result;
}


OFCondition DSRContentItem::getTemplateIdentification(OFString &templateIdentifier,
                                                      OFString &mappingResource) const
{
    OFCondition result = EC_IllegalCall;
    if (TreeNode != NULL)
        result = TreeNode->getTemplateIdentification(templateIdentifier, mappingResource);
    return result;
}


OFCondition DSRContentItem::setTemplateIdentification(const OFString &templateIdentifier,
                                                      const OFString &mappingResource)
{
    OFCondition result = EC_IllegalCall;
    if (TreeNode != NULL)
        result = TreeNode->setTemplateIdentification(templateIdentifier, mappingResource);
    return result;
}


/*
 *  CVS/RCS Log:
 *  $Log: dsrcitem.cc,v $
 *  Revision 1.15  2010-10-14 13:14:40  joergr
 *  Updated copyright header. Added reference to COPYRIGHT file.
 *
 *  Revision 1.14  2005-12-08 15:47:39  meichel
 *  Changed include path schema for all DCMTK header files
 *
 *  Revision 1.13  2003/10/30 17:59:37  joergr
 *  Added full support for the ContentTemplateSequence (read/write, get/set
 *  template identification). Template constraints are not checked yet.
 *
 *  Revision 1.12  2003/08/07 13:11:03  joergr
 *  Adapted type casts to new-style typecast operators defined in ofcast.h.
 *
 *  Revision 1.11  2001/09/26 13:04:17  meichel
 *  Adapted dcmsr to class OFCondition
 *
 *  Revision 1.10  2001/05/07 16:14:21  joergr
 *  Updated CVS header.
 *
 *  Revision 1.9  2001/01/18 15:54:12  joergr
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
