/*
 *
 *  Copyright (C) 2020-2024, J. Riesmeier, Oldenburg, Germany
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
 *    classes: DSRRenditionSelectionDocumentConstraintChecker
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/dsrrsdcc.h"


DSRRenditionSelectionDocumentConstraintChecker::DSRRenditionSelectionDocumentConstraintChecker()
  : DSRIODConstraintChecker()
{
}


DSRRenditionSelectionDocumentConstraintChecker::~DSRRenditionSelectionDocumentConstraintChecker()
{
}


OFBool DSRRenditionSelectionDocumentConstraintChecker::isByReferenceAllowed() const
{
    return OFFalse;
}


OFBool DSRRenditionSelectionDocumentConstraintChecker::isTemplateSupportRequired() const
{
    return OFTrue;
}


OFCondition DSRRenditionSelectionDocumentConstraintChecker::getRootTemplateIdentification(OFString &templateIdentifier,
                                                                                          OFString &mappingResource) const
{
    templateIdentifier = "2010";
    mappingResource = "DCMR";
    return EC_Normal;
}


DSRTypes::E_DocumentType DSRRenditionSelectionDocumentConstraintChecker::getDocumentType() const
{
    return DT_RenditionSelectionDocument;
}


OFBool DSRRenditionSelectionDocumentConstraintChecker::checkContentRelationship(const E_ValueType sourceValueType,
                                                                                const E_RelationshipType relationshipType,
                                                                                const E_ValueType targetValueType,
                                                                                const OFBool byReference) const
{
    /* the following code implements the constraints of table A.35.21-2 in DICOM PS3.3 */
    OFBool result = OFFalse;
    /* by-reference relationships not allowed at all */
    if (!byReference)
    {
        /* row 1 of the table */
        if ((relationshipType == RT_contains) && (sourceValueType == VT_Container))
        {
            result = (targetValueType == VT_Text) || (targetValueType == VT_Image) || (targetValueType == VT_Waveform) ||
                     (targetValueType == VT_Composite);
        }
        /* row 2 of the table */
        else if ((relationshipType == RT_hasObsContext) && (sourceValueType == VT_Container))
        {
            result = (targetValueType == VT_Text)  || (targetValueType == VT_Code) || (targetValueType == VT_UIDRef) ||
                     (targetValueType == VT_PName) || (targetValueType == VT_Container);
        }
        /* row 3 of the table */
        else if ((relationshipType == RT_hasConceptMod) && (sourceValueType == VT_Container))
        {
            result = (targetValueType == VT_Code);
        }
    }
    return result;
}
