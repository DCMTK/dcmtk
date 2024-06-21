/*
 *
 *  Copyright (C) 2010-2024, OFFIS e.V.
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
 *    classes: DSRMacularGridThicknessAndVolumeReportConstraintChecker
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/dsrmaccc.h"


DSRMacularGridThicknessAndVolumeReportConstraintChecker::DSRMacularGridThicknessAndVolumeReportConstraintChecker()
  : DSRIODConstraintChecker()
{
}


DSRMacularGridThicknessAndVolumeReportConstraintChecker::~DSRMacularGridThicknessAndVolumeReportConstraintChecker()
{
}


OFBool DSRMacularGridThicknessAndVolumeReportConstraintChecker::isByReferenceAllowed() const
{
    return OFFalse;
}


OFBool DSRMacularGridThicknessAndVolumeReportConstraintChecker::isTemplateSupportRequired() const
{
    return OFTrue;
}


OFCondition DSRMacularGridThicknessAndVolumeReportConstraintChecker::getRootTemplateIdentification(OFString &templateIdentifier,
                                                                                                   OFString &mappingResource) const
{
    templateIdentifier = "2100";
    mappingResource = "DCMR";
    return EC_Normal;
}


DSRTypes::E_DocumentType DSRMacularGridThicknessAndVolumeReportConstraintChecker::getDocumentType() const
{
    return DT_MacularGridThicknessAndVolumeReport;
}


OFBool DSRMacularGridThicknessAndVolumeReportConstraintChecker::checkContentRelationship(const E_ValueType sourceValueType,
                                                                                         const E_RelationshipType relationshipType,
                                                                                         const E_ValueType targetValueType,
                                                                                         const OFBool byReference) const
{
    /* the following code implements the constraints of table A.35.11.3.1.2-1 in DICOM PS3.3 */
    OFBool result = OFFalse;
    /* by-reference relationships not allowed at all */
    if (!byReference)
    {
        /* row 1 of the table */
        if ((relationshipType == RT_hasObsContext) && (sourceValueType == VT_Container))
        {
            result = (targetValueType == VT_Code)   || (targetValueType == VT_PName) || (targetValueType == VT_Text) ||
                     (targetValueType == VT_UIDRef) || (targetValueType == VT_Date)  || (targetValueType == VT_Num)  ||
                     (targetValueType == VT_Container);
        }
        /* row 2 of the table */
        else if ((relationshipType == RT_contains) && (sourceValueType == VT_Container))
        {
            result = (targetValueType == VT_Container) || (targetValueType == VT_Num) || (targetValueType == VT_Text) ||
                     (targetValueType == VT_Code);
        }
        /* row 3 of the table */
        else if (relationshipType == RT_hasConceptMod)
        {
            result = (targetValueType == VT_Code);
        }
        /* row 4 of the table */
        else if ((relationshipType == RT_hasObsContext) && (sourceValueType == VT_Num))
        {
            result = (targetValueType == VT_Text);
        }
        /* row 5 of the table */
        else if ((relationshipType == RT_inferredFrom) && (sourceValueType == VT_Num))
        {
            result = (targetValueType == VT_Image);
        }
    }
    return result;
}
