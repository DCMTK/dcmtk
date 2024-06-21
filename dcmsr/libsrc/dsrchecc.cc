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
 *    classes: DSRChestCadSRConstraintChecker
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/dsrchecc.h"


DSRChestCadSRConstraintChecker::DSRChestCadSRConstraintChecker()
  : DSRIODConstraintChecker()
{
}


DSRChestCadSRConstraintChecker::~DSRChestCadSRConstraintChecker()
{
}


OFBool DSRChestCadSRConstraintChecker::isByReferenceAllowed() const
{
    return OFTrue;
}


OFBool DSRChestCadSRConstraintChecker::isTemplateSupportRequired() const
{
    return OFTrue;
}


OFCondition DSRChestCadSRConstraintChecker::getRootTemplateIdentification(OFString &templateIdentifier,
                                                                          OFString &mappingResource) const
{
    templateIdentifier = "4100";
    mappingResource = "DCMR";
    return EC_Normal;
}


DSRTypes::E_DocumentType DSRChestCadSRConstraintChecker::getDocumentType() const
{
    return DT_ChestCadSR;
}


OFBool DSRChestCadSRConstraintChecker::checkContentRelationship(const E_ValueType sourceValueType,
                                                                const E_RelationshipType relationshipType,
                                                                const E_ValueType targetValueType,
                                                                const OFBool byReference) const
{
    /* the following code implements the constraints of table A.35.6-2 in DICOM PS3.3 */
    OFBool result = OFFalse;
    /* row 1 of the table */
    if ((relationshipType == RT_contains) && !byReference && (sourceValueType == VT_Container))
    {
        result = (targetValueType == VT_Code) || (targetValueType == VT_Num) || (targetValueType == VT_Image) ||
                 (targetValueType == VT_Container);
    }
    /* row 2 of the table (CONTAINER-related constraints are checked with row 3) */
    else if ((relationshipType == RT_hasObsContext) && !byReference &&
        ((sourceValueType == VT_Text) || (sourceValueType == VT_Code) || (sourceValueType == VT_Num)))
    {
        result = (targetValueType == VT_Text)   || (targetValueType == VT_Code) || (targetValueType == VT_Num)   ||
                 (targetValueType == VT_Date)   || (targetValueType == VT_Time) || (targetValueType == VT_PName) ||
                 (targetValueType == VT_UIDRef) || (targetValueType == VT_Composite);
    }
    /* row 3 of the table (including CONTAINER-related constraints from row 2) */
    else if ((relationshipType == RT_hasObsContext) && !byReference && (sourceValueType == VT_Container))
    {
        result = (targetValueType == VT_Text)   || (targetValueType == VT_Code)      || (targetValueType == VT_Num)   ||
                 (targetValueType == VT_Date)   || (targetValueType == VT_Time)      || (targetValueType == VT_PName) ||
                 (targetValueType == VT_UIDRef) || (targetValueType == VT_Composite) || (targetValueType == VT_Container);
    }
    /* row 4 of the table */
    else if ((relationshipType == RT_hasAcqContext) && !byReference &&
        ((sourceValueType == VT_Image) || (sourceValueType == VT_Waveform)))
    {
        result = (targetValueType == VT_Text) || (targetValueType == VT_Code) || (targetValueType == VT_Num) ||
                 (targetValueType == VT_Date) || (targetValueType == VT_Time);
    }
    /* row 5 of the table */
    else if ((relationshipType == RT_hasConceptMod) && !byReference && ((sourceValueType == VT_Container) ||
        (sourceValueType == VT_Code) || (sourceValueType == VT_Composite) || (sourceValueType == VT_Num)))
    {
        result = (targetValueType == VT_Text) || (targetValueType == VT_Code);
    }
    /* row 6 the table */
    else if ((relationshipType == RT_hasProperties) &&
        ((sourceValueType == VT_Text) || (sourceValueType == VT_Code) || (sourceValueType == VT_Num)))
    {
        /* by-reference allowed */
        result = (targetValueType == VT_Container) || (targetValueType == VT_Text)   || (targetValueType == VT_Code)   ||
                 (targetValueType == VT_Num)       || (targetValueType == VT_Date)   || (targetValueType == VT_Image)  ||
                 (targetValueType == VT_Waveform)  || (targetValueType == VT_SCoord) || (targetValueType == VT_TCoord) ||
                 (targetValueType == VT_UIDRef);
    }
    /* row 7 of the table */
    else if ((relationshipType == RT_inferredFrom) && ((sourceValueType == VT_Code) || (sourceValueType == VT_Num)))
    {
        /* by-reference allowed */
        result = (targetValueType == VT_Code)      || (targetValueType == VT_Num)    || (targetValueType == VT_Image)  ||
                 (targetValueType == VT_Waveform)  || (targetValueType == VT_SCoord) || (targetValueType == VT_TCoord) ||
                 (targetValueType == VT_Container) || (targetValueType == VT_Text);
    }
    /* row 8 of the table */
    else if ((relationshipType == RT_selectedFrom) && (sourceValueType == VT_SCoord))
    {
        /* by-reference allowed */
        result = (targetValueType == VT_Image);
    }
    /* row 9 of the table */
    else if ((relationshipType == RT_selectedFrom) && (sourceValueType == VT_TCoord))
    {
        /* by-reference allowed */
        result = (targetValueType == VT_SCoord) || (targetValueType == VT_Image) || (targetValueType == VT_Waveform);
    }
    return result;
}
