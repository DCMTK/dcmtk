/*
 *
 *  Copyright (C) 2017-2024, J. Riesmeier, Oldenburg, Germany
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
 *  Module: dcmsr
 *
 *  Author: Joerg Riesmeier
 *
 *  Purpose:
 *    classes: DSRPatientRadiationDoseSRConstraintChecker
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/dsrprdcc.h"


DSRPatientRadiationDoseSRConstraintChecker::DSRPatientRadiationDoseSRConstraintChecker()
  : DSRIODConstraintChecker()
{
}


DSRPatientRadiationDoseSRConstraintChecker::~DSRPatientRadiationDoseSRConstraintChecker()
{
}


OFBool DSRPatientRadiationDoseSRConstraintChecker::isByReferenceAllowed() const
{
    return OFFalse;
}


OFBool DSRPatientRadiationDoseSRConstraintChecker::isTemplateSupportRequired() const
{
    return OFTrue;
}


OFCondition DSRPatientRadiationDoseSRConstraintChecker::getRootTemplateIdentification(OFString &templateIdentifier,
                                                                                      OFString &mappingResource) const
{
    /* tbc: According to PS 3.3 "The document shall be constructed from _Baseline_ TID 10030..." */
    templateIdentifier = "10030";
    mappingResource = "DCMR";
    return EC_Normal;
}


DSRTypes::E_DocumentType DSRPatientRadiationDoseSRConstraintChecker::getDocumentType() const
{
    return DT_PatientRadiationDoseSR;
}


OFBool DSRPatientRadiationDoseSRConstraintChecker::checkContentRelationship(const E_ValueType sourceValueType,
                                                                            const E_RelationshipType relationshipType,
                                                                            const E_ValueType targetValueType,
                                                                            const OFBool byReference) const
{
    /* the following code implements the constraints of table A.35.18-2 in DICOM PS3.3 */
    OFBool result = OFFalse;
    /* by-reference relationships not allowed at all */
    if (!byReference)
    {
        /* row 1 of the table */
        if ((relationshipType == RT_contains) && (sourceValueType == VT_Container))
        {
            result = (targetValueType == VT_Text)     || (targetValueType == VT_Code)      || (targetValueType == VT_Num)   ||
                     (targetValueType == VT_DateTime) || (targetValueType == VT_UIDRef)    || (targetValueType == VT_PName) ||
                     (targetValueType == VT_Image)    || (targetValueType == VT_Composite) || (targetValueType == VT_Container);
        }
        /* row 2 of the table */
        else if ((relationshipType == RT_hasObsContext) && (sourceValueType == VT_Container))
        {
            result = (targetValueType == VT_DateTime) || (targetValueType == VT_Code)  || (targetValueType == VT_Text) ||
                     (targetValueType == VT_UIDRef)   || (targetValueType == VT_PName) || (targetValueType == VT_Container);
        }
        /* row 3 of the table */
        else if ((relationshipType == RT_hasObsContext) &&
            ((sourceValueType == VT_Text) || (sourceValueType == VT_Code) || (sourceValueType == VT_Num) || (sourceValueType == VT_Composite)))
        {
            result = (targetValueType == VT_Text)     || (targetValueType == VT_Code)   || (targetValueType == VT_Num)   ||
                     (targetValueType == VT_DateTime) || (targetValueType == VT_UIDRef) || (targetValueType == VT_PName) ||
                     (targetValueType == VT_Composite);
        }
        /* row 4 of the table */
        else if ((relationshipType == RT_hasAcqContext) &&
            ((sourceValueType == VT_Container) || (sourceValueType == VT_Image) || (sourceValueType == VT_Composite)))
        {
            result = (targetValueType == VT_Text)     || (targetValueType == VT_Code)   || (targetValueType == VT_Num)   ||
                     (targetValueType == VT_DateTime) || (targetValueType == VT_UIDRef) || (targetValueType == VT_PName) ||
                     (targetValueType == VT_Container);
        }
        /* row 5 of the table */
        else if (relationshipType == RT_hasConceptMod)
        {
            result = (targetValueType == VT_Text) || (targetValueType == VT_Code);
        }
        /* row 6 the table */
        else if ((relationshipType == RT_hasProperties) &&
            ((sourceValueType == VT_Text) || (sourceValueType == VT_Code) || (sourceValueType == VT_Num) || (sourceValueType == VT_Composite)))
        {
            result = (targetValueType == VT_Text)     || (targetValueType == VT_Code)      || (targetValueType == VT_Num)   ||
                     (targetValueType == VT_DateTime) || (targetValueType == VT_UIDRef)    || (targetValueType == VT_PName) ||
                     (targetValueType == VT_Image)    || (targetValueType == VT_Composite) || (targetValueType == VT_Container);
        }
        /* row 7 of the table */
        else if ((relationshipType == RT_hasProperties) && (sourceValueType == VT_PName))
        {
            result = (targetValueType == VT_Text) || (targetValueType == VT_Code) || (targetValueType == VT_DateTime) ||
                     (targetValueType == VT_Date) || (targetValueType == VT_Time) || (targetValueType == VT_UIDRef)   ||
                     (targetValueType == VT_PName);
        }
        /* row 8 of the table */
        else if ((relationshipType == RT_inferredFrom) &&
            ((sourceValueType == VT_Text) || (sourceValueType == VT_Code) || (sourceValueType == VT_Num)))
        {
            result = (targetValueType == VT_Text)      || (targetValueType == VT_Code)   || (targetValueType == VT_Num)   ||
                     (targetValueType == VT_DateTime)  || (targetValueType == VT_UIDRef) || (targetValueType == VT_Image) ||
                     (targetValueType == VT_Composite) || (targetValueType == VT_Container);
        }
    }
    return result;
}
