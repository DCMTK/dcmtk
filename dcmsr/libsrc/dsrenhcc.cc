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
 *    classes: DSREnhancedSRConstraintChecker
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/dsrenhcc.h"


DSREnhancedSRConstraintChecker::DSREnhancedSRConstraintChecker()
  : DSRIODConstraintChecker()
{
}


DSREnhancedSRConstraintChecker::~DSREnhancedSRConstraintChecker()
{
}


OFBool DSREnhancedSRConstraintChecker::isByReferenceAllowed() const
{
    return OFFalse;
}


OFBool DSREnhancedSRConstraintChecker::isTemplateSupportRequired() const
{
    return OFFalse;
}


OFCondition DSREnhancedSRConstraintChecker::getRootTemplateIdentification(OFString &templateIdentifier,
                                                                          OFString &mappingResource) const
{
    templateIdentifier.clear();
    mappingResource.clear();
    return EC_Normal;
}


DSRTypes::E_DocumentType DSREnhancedSRConstraintChecker::getDocumentType() const
{
    return DT_EnhancedSR;
}


OFBool DSREnhancedSRConstraintChecker::checkContentRelationship(const E_ValueType sourceValueType,
                                                                const E_RelationshipType relationshipType,
                                                                const E_ValueType targetValueType,
                                                                const OFBool byReference) const
{
    /* the following code implements the constraints of table A.35.2-2 in DICOM PS3.3 */
    OFBool result = OFFalse;
    /* by-reference relationships not allowed at all */
    if (!byReference)
    {
        /* row 1 of the table */
        if ((relationshipType == RT_contains) && (sourceValueType == VT_Container))
        {
            result = (targetValueType == VT_Text)     || (targetValueType == VT_Code)      || (targetValueType == VT_Num)    ||
                     (targetValueType == VT_DateTime) || (targetValueType == VT_Date)      || (targetValueType == VT_Time)   ||
                     (targetValueType == VT_UIDRef)   || (targetValueType == VT_PName)     || (targetValueType == VT_SCoord) ||
                     (targetValueType == VT_TCoord)   || (targetValueType == VT_Composite) || (targetValueType == VT_Image)  ||
                     (targetValueType == VT_Waveform) || (targetValueType == VT_Container);
        }
        /* row 2 of the table */
        else if ((relationshipType == RT_hasObsContext) && (sourceValueType == VT_Container))
        {
            result = (targetValueType == VT_Text)     || (targetValueType == VT_Code)  || (targetValueType == VT_Num)       ||
                     (targetValueType == VT_DateTime) || (targetValueType == VT_Date)  || (targetValueType == VT_Time)      ||
                     (targetValueType == VT_UIDRef)   || (targetValueType == VT_PName) || (targetValueType == VT_Composite) ||
                     (targetValueType == VT_Container);
        }
        /* row 3 of the table */
        else if ((relationshipType == RT_hasAcqContext) && ((sourceValueType == VT_Container) || (sourceValueType == VT_Image) ||
            (sourceValueType == VT_Waveform) || (sourceValueType == VT_Composite) || (sourceValueType == VT_Num)))
        {
            result = (targetValueType == VT_Text)     || (targetValueType == VT_Code) || (targetValueType == VT_Num)  ||
                     (targetValueType == VT_DateTime) || (targetValueType == VT_Date) || (targetValueType == VT_Time) ||
                     (targetValueType == VT_UIDRef)   || (targetValueType == VT_PName);
        }
        /* row 4 of the table */
        else if (relationshipType == RT_hasConceptMod)
        {
            result = (targetValueType == VT_Text) || (targetValueType == VT_Code);
        }
        /* row 5 of the table */
        else if ((relationshipType == RT_hasProperties) &&
            ((sourceValueType == VT_Text) || (sourceValueType == VT_Code) || (sourceValueType == VT_Num)))
        {
            result = (targetValueType == VT_Text)     || (targetValueType == VT_Code)      || (targetValueType == VT_Num)    ||
                     (targetValueType == VT_DateTime) || (targetValueType == VT_Date)      || (targetValueType == VT_Time)   ||
                     (targetValueType == VT_UIDRef)   || (targetValueType == VT_PName)     || (targetValueType == VT_Image)  ||
                     (targetValueType == VT_Waveform) || (targetValueType == VT_Composite) || (targetValueType == VT_SCoord) ||
                     (targetValueType == VT_TCoord);
        }
        /* row 6 of the table */
        else if ((relationshipType == RT_hasProperties) && (sourceValueType == VT_PName))
        {
            result = (targetValueType == VT_Text) || (targetValueType == VT_Code) || (targetValueType == VT_DateTime) ||
                     (targetValueType == VT_Date) || (targetValueType == VT_Time) || (targetValueType == VT_UIDRef)   ||
                     (targetValueType == VT_PName);
        }
        /* row 7 of the table */
        else if ((relationshipType == RT_inferredFrom) &&
            ((sourceValueType == VT_Text) || (sourceValueType == VT_Code) || (sourceValueType == VT_Num)))
        {
            result = (targetValueType == VT_Text)     || (targetValueType == VT_Code)      || (targetValueType == VT_Num)    ||
                     (targetValueType == VT_DateTime) || (targetValueType == VT_Date)      || (targetValueType == VT_Time)   ||
                     (targetValueType == VT_UIDRef)   || (targetValueType == VT_PName)     || (targetValueType == VT_Image)  ||
                     (targetValueType == VT_Waveform) || (targetValueType == VT_Composite) || (targetValueType == VT_SCoord) ||
                     (targetValueType == VT_TCoord);
        }
        /* row 8 of the table */
        else if ((relationshipType == RT_selectedFrom) && (sourceValueType == VT_SCoord))
        {
            result = (targetValueType == VT_Image);
        }
        /* row 9 of the table */
        else if ((relationshipType == RT_selectedFrom) && (sourceValueType == VT_TCoord))
        {
            result = (targetValueType == VT_SCoord) || (targetValueType == VT_Image) || (targetValueType == VT_Waveform);
        }
    }
    return result;
}
