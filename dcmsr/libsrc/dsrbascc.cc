/*
 *
 *  Copyright (C) 2003-2024, OFFIS e.V.
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
 *    classes: DSRBasicTextSRConstraintChecker
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/dsrbascc.h"


DSRBasicTextSRConstraintChecker::DSRBasicTextSRConstraintChecker()
  : DSRIODConstraintChecker()
{
}


DSRBasicTextSRConstraintChecker::~DSRBasicTextSRConstraintChecker()
{
}


OFBool DSRBasicTextSRConstraintChecker::isByReferenceAllowed() const
{
    return OFFalse;
}


OFBool DSRBasicTextSRConstraintChecker::isTemplateSupportRequired() const
{
    return OFFalse;
}


OFCondition DSRBasicTextSRConstraintChecker::getRootTemplateIdentification(OFString &templateIdentifier,
                                                                           OFString &mappingResource) const
{
    templateIdentifier.clear();
    mappingResource.clear();
    return EC_Normal;
}


DSRTypes::E_DocumentType DSRBasicTextSRConstraintChecker::getDocumentType() const
{
    return DT_BasicTextSR;
}


OFBool DSRBasicTextSRConstraintChecker::checkContentRelationship(const E_ValueType sourceValueType,
                                                                 const E_RelationshipType relationshipType,
                                                                 const E_ValueType targetValueType,
                                                                 const OFBool byReference) const
{
    /* the following code implements the constraints of table A.35.1-2 in DICOM PS3.3 */
    OFBool result = OFFalse;
    /* by-reference relationships not allowed at all */
    if (!byReference)
    {
        /* row 1 of the table */
        if ((relationshipType == RT_contains) && (sourceValueType == VT_Container))
        {
            result = (targetValueType == VT_Text)     || (targetValueType == VT_Code)      || (targetValueType == VT_DateTime) ||
                     (targetValueType == VT_Date)     || (targetValueType == VT_Time)      || (targetValueType == VT_UIDRef)   ||
                     (targetValueType == VT_PName)    || (targetValueType == VT_Composite) || (targetValueType == VT_Image)    ||
                     (targetValueType == VT_Waveform) || (targetValueType == VT_Container);
        }
        /* row 2 of the table */
        else if ((relationshipType == RT_hasObsContext) && (sourceValueType == VT_Container))
        {
            result = (targetValueType == VT_Text)  || (targetValueType == VT_Code)      || (targetValueType == VT_DateTime) ||
                     (targetValueType == VT_Date)  || (targetValueType == VT_Time)      || (targetValueType == VT_UIDRef)   ||
                     (targetValueType == VT_PName) || (targetValueType == VT_Composite) || (targetValueType == VT_Container);
        }
        /* row 3 of the table */
        else if ((relationshipType == RT_hasAcqContext) && ((sourceValueType == VT_Container) ||
            (sourceValueType == VT_Image) || (sourceValueType == VT_Waveform) || (sourceValueType == VT_Composite)))
        {
            result = (targetValueType == VT_Text) || (targetValueType == VT_Code) || (targetValueType == VT_DateTime) ||
                     (targetValueType == VT_Date) || (targetValueType == VT_Time) || (targetValueType == VT_UIDRef)   ||
                     (targetValueType == VT_PName);
        }
        /* row 4 of the table */
        else if (relationshipType == RT_hasConceptMod)
        {
            result = (targetValueType == VT_Text) || (targetValueType == VT_Code);
        }
        /* row 5 of the table */
        else if ((relationshipType == RT_hasProperties) && (sourceValueType == VT_Text))
        {
            result = (targetValueType == VT_Text)  || (targetValueType == VT_Code)  || (targetValueType == VT_DateTime) ||
                     (targetValueType == VT_Date)  || (targetValueType == VT_Time)  || (targetValueType == VT_UIDRef)   ||
                     (targetValueType == VT_PName) || (targetValueType == VT_Image) || (targetValueType == VT_Waveform) ||
                     (targetValueType == VT_Composite);
        }
        /* row 6 of the table */
        else if ((relationshipType == RT_hasProperties) && (sourceValueType == VT_PName))
        {
            result = (targetValueType == VT_Text) || (targetValueType == VT_Code) || (targetValueType == VT_DateTime) ||
                     (targetValueType == VT_Date) || (targetValueType == VT_Time) || (targetValueType == VT_UIDRef)   ||
                     (targetValueType == VT_PName);
        }
        /* row 7 of the table */
        else if ((relationshipType == RT_inferredFrom) && (sourceValueType == VT_Text))
        {
            result = (targetValueType == VT_Text)  || (targetValueType == VT_Code)  || (targetValueType == VT_DateTime) ||
                     (targetValueType == VT_Date)  || (targetValueType == VT_Time)  || (targetValueType == VT_UIDRef)   ||
                     (targetValueType == VT_PName) || (targetValueType == VT_Image) || (targetValueType == VT_Waveform) ||
                     (targetValueType == VT_Composite);
        }
    }
    return result;
}
