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
 *  Module:  dcmsr
 *
 *  Author:  Joerg Riesmeier
 *
 *  Purpose:
 *    classes: DSRProcedureLogConstraintChecker
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/dsrprocc.h"


DSRProcedureLogConstraintChecker::DSRProcedureLogConstraintChecker()
  : DSRIODConstraintChecker()
{
}


DSRProcedureLogConstraintChecker::~DSRProcedureLogConstraintChecker()
{
}


OFBool DSRProcedureLogConstraintChecker::isByReferenceAllowed() const
{
    return OFFalse;
}


OFBool DSRProcedureLogConstraintChecker::isTemplateSupportRequired() const
{
    return OFFalse;
}


OFCondition DSRProcedureLogConstraintChecker::getRootTemplateIdentification(OFString &templateIdentifier,
                                                                            OFString &mappingResource) const
{
    templateIdentifier.clear();
    mappingResource.clear();
    return EC_Normal;
}


DSRTypes::E_DocumentType DSRProcedureLogConstraintChecker::getDocumentType() const
{
    return DT_ProcedureLog;
}


OFBool DSRProcedureLogConstraintChecker::checkContentRelationship(const E_ValueType sourceValueType,
                                                                  const E_RelationshipType relationshipType,
                                                                  const E_ValueType targetValueType,
                                                                  const OFBool byReference) const
{
    /* the following code implements the constraints of table A.35.7-2 in DICOM PS3.3 */
    OFBool result = OFFalse;
    /* by-reference relationships not allowed at all */
    if (!byReference)
    {
        /* row 1 of the table */
        if ((relationshipType == RT_contains) && (sourceValueType == VT_Container))
        {
            result = (targetValueType == VT_Text)  || (targetValueType == VT_Code)      || (targetValueType == VT_Num)   ||
                     (targetValueType == VT_PName) || (targetValueType == VT_Composite) || (targetValueType == VT_Image) ||
                     (targetValueType == VT_Waveform);
        }
        /* row 2 of the table (CONTAINER-related constraints are checked with row 3) */
        else if ((relationshipType == RT_hasObsContext) && (sourceValueType != VT_Container))
        {
            result = (targetValueType == VT_Text)     || (targetValueType == VT_Code)   || (targetValueType == VT_Num)  ||
                     (targetValueType == VT_DateTime) || (targetValueType == VT_UIDRef) || (targetValueType == VT_PName);
        }
        /* row 3 of the table (including CONTAINER-related constraints from row 2) */
        else if ((relationshipType == RT_hasObsContext) && (sourceValueType == VT_Container))
        {
            result = (targetValueType == VT_Text)     || (targetValueType == VT_Code)   || (targetValueType == VT_Num)   ||
                     (targetValueType == VT_DateTime) || (targetValueType == VT_UIDRef) || (targetValueType == VT_PName) ||
                     (targetValueType == VT_Container);
        }
        /* row 4 of the table */
        else if ((relationshipType == RT_hasAcqContext) && ((sourceValueType == VT_Container) ||
            (sourceValueType == VT_Image) || (sourceValueType == VT_Waveform) || (sourceValueType == VT_Composite)))
        {
            result = (targetValueType == VT_Text)     || (targetValueType == VT_Code) || (targetValueType == VT_Num)  ||
                     (targetValueType == VT_DateTime) || (targetValueType == VT_Date) || (targetValueType == VT_Time) ||
                     (targetValueType == VT_UIDRef)   || (targetValueType == VT_PName);
        }
        /* row 5 of the table */
        else if (relationshipType == RT_hasConceptMod)
        {
            result = (targetValueType == VT_Text) || (targetValueType == VT_Code);
        }
        /* row 6 of the table */
        else if ((relationshipType == RT_hasProperties) && (sourceValueType != VT_Container))
        {
            result = (targetValueType == VT_Text)     || (targetValueType == VT_Code)   || (targetValueType == VT_Num)  ||
                     (targetValueType == VT_DateTime) || (targetValueType == VT_UIDRef) || (targetValueType == VT_PName);
        }
        /* row 7 of the table */
        else if ((relationshipType == RT_inferredFrom) &&
            ((sourceValueType == VT_Text) || (sourceValueType == VT_Code) || (sourceValueType == VT_Num)))
        {
            result = (targetValueType == VT_Composite) || (targetValueType == VT_Image) || (targetValueType == VT_Waveform);
        }
    }
    return result;
}
