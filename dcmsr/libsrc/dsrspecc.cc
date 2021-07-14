/*
 *
 *  Copyright (C) 2010-2021, OFFIS e.V.
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
 *    classes: DSRSpectaclePrescriptionReportConstraintChecker
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/dsrspecc.h"


DSRSpectaclePrescriptionReportConstraintChecker::DSRSpectaclePrescriptionReportConstraintChecker()
  : DSRIODConstraintChecker()
{
}


DSRSpectaclePrescriptionReportConstraintChecker::~DSRSpectaclePrescriptionReportConstraintChecker()
{
}


OFBool DSRSpectaclePrescriptionReportConstraintChecker::isByReferenceAllowed() const
{
    return OFFalse;
}


OFBool DSRSpectaclePrescriptionReportConstraintChecker::isTemplateSupportRequired() const
{
    return OFTrue;
}


OFCondition DSRSpectaclePrescriptionReportConstraintChecker::getRootTemplateIdentification(OFString &templateIdentifier,
                                                                                           OFString &mappingResource) const
{
    templateIdentifier = "2020";
    mappingResource = "DCMR";
    return EC_Normal;
}


DSRTypes::E_DocumentType DSRSpectaclePrescriptionReportConstraintChecker::getDocumentType() const
{
    return DT_SpectaclePrescriptionReport;
}


OFBool DSRSpectaclePrescriptionReportConstraintChecker::checkContentRelationship(const E_ValueType sourceValueType,
                                                                                 const E_RelationshipType relationshipType,
                                                                                 const E_ValueType targetValueType,
                                                                                 const OFBool byReference) const
{
    /* the following code implements the constraints of table A.35.9-2 in DICOM PS3.3 */
    OFBool result = OFFalse;
    /* by-reference relationships not allowed at all */
    if (!byReference)
    {
        /* row 1 of the table */
        if ((relationshipType == RT_contains) && (sourceValueType == VT_Container))
        {
            result = (targetValueType == VT_Container) || (targetValueType == VT_Code) ||
                     (targetValueType == VT_Num)       || (targetValueType == VT_Text);
        }
    }
    return result;
}
