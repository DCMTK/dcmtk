/*
 *
 *  Copyright (C) 2019, J. Riesmeier, Oldenburg, Germany
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  This software and supporting documentation are maintained by
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
 *    classes: DSRPlannedImagingAgentAdministrationSRConstraintChecker
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/dsrplicc.h"


DSRPlannedImagingAgentAdministrationSRConstraintChecker::DSRPlannedImagingAgentAdministrationSRConstraintChecker()
  : DSRIODConstraintChecker()
{
}


DSRPlannedImagingAgentAdministrationSRConstraintChecker::~DSRPlannedImagingAgentAdministrationSRConstraintChecker()
{
}


OFBool DSRPlannedImagingAgentAdministrationSRConstraintChecker::isByReferenceAllowed() const
{
    return OFFalse;
}


OFBool DSRPlannedImagingAgentAdministrationSRConstraintChecker::isTemplateSupportRequired() const
{
    return OFTrue;
}


OFCondition DSRPlannedImagingAgentAdministrationSRConstraintChecker::getRootTemplateIdentification(OFString &templateIdentifier,
                                                                                                   OFString &mappingResource) const
{
    templateIdentifier = "11001";
    mappingResource = "DCMR";
    return EC_Normal;
}


DSRTypes::E_DocumentType DSRPlannedImagingAgentAdministrationSRConstraintChecker::getDocumentType() const
{
    return DT_PlannedImagingAgentAdministrationSR;
}


OFBool DSRPlannedImagingAgentAdministrationSRConstraintChecker::checkContentRelationship(const E_ValueType sourceValueType,
                                                                                         const E_RelationshipType relationshipType,
                                                                                         const E_ValueType targetValueType,
                                                                                         const OFBool byReference) const
{
    /* the following code implements the constraints of table A.35.19-2 in DICOM PS3.3 */
    OFBool result = OFFalse;
    /* by-reference relationships not allowed at all */
    if (!byReference)
    {
        /* row 1 of the table */
        if ((relationshipType == RT_contains) && (sourceValueType == VT_Container))
        {
            result = (targetValueType == VT_Text)     || (targetValueType == VT_Code) || (targetValueType == VT_Num)    ||
                     (targetValueType == VT_DateTime) || (targetValueType == VT_Date) || (targetValueType == VT_UIDRef) ||
                     (targetValueType == VT_PName)    || (targetValueType == VT_Container);
        }
        /* row 2 of the table */
        else if ((relationshipType == RT_hasObsContext) &&
            ((sourceValueType == VT_Text) || (sourceValueType == VT_Code) || (sourceValueType == VT_Num) || (sourceValueType == VT_Container)))
        {
            result = (targetValueType == VT_Text)     || (targetValueType == VT_Code) || (targetValueType == VT_Num)    ||
                     (targetValueType == VT_DateTime) || (targetValueType == VT_Date) || (targetValueType == VT_UIDRef) ||
                     (targetValueType == VT_PName);
        }
        /* row 3 of the table */
        else if ((relationshipType == RT_hasAcqContext) &&
            ((sourceValueType == VT_Container) || (sourceValueType == VT_Num)))
        {
            result = (targetValueType == VT_Text)     || (targetValueType == VT_Code) || (targetValueType == VT_Num)    ||
                     (targetValueType == VT_DateTime) || (targetValueType == VT_Date) || (targetValueType == VT_UIDRef) ||
                     (targetValueType == VT_PName)    || (targetValueType == VT_Container);
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
            result = (targetValueType == VT_Text)     || (targetValueType == VT_Code) || (targetValueType == VT_Num)    ||
                     (targetValueType == VT_DateTime) || (targetValueType == VT_Date) || (targetValueType == VT_UIDRef) ||
                     (targetValueType == VT_PName)    || (targetValueType == VT_Container);
        }
        /* row 6 of the table */
        else if ((relationshipType == RT_hasProperties) && (sourceValueType == VT_PName))
        {
            result = (targetValueType == VT_Text) || (targetValueType == VT_Code)   || (targetValueType == VT_DateTime) ||
                     (targetValueType == VT_Date) || (targetValueType == VT_UIDRef) ||(targetValueType == VT_PName);
        }
        /* row 7 of the table */
        else if ((relationshipType == RT_inferredFrom) &&
            ((sourceValueType == VT_Text) || (sourceValueType == VT_Code) || (sourceValueType == VT_Num)))
        {
            result = (targetValueType == VT_Text)     || (targetValueType == VT_Code) || (targetValueType == VT_Num)    ||
                     (targetValueType == VT_DateTime) || (targetValueType == VT_Date) || (targetValueType == VT_UIDRef) ||
                     (targetValueType == VT_PName)    || (targetValueType == VT_Container);
        }
    }
    return result;
}
