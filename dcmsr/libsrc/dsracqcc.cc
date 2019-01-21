/*
 *
 *  Copyright (C) 2015-2019, J. Riesmeier, Oldenburg, Germany
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
 *    classes: DSRAcquisitionContextSRConstraintChecker
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/dsracqcc.h"


DSRAcquisitionContextSRConstraintChecker::DSRAcquisitionContextSRConstraintChecker()
  : DSRIODConstraintChecker()
{
}


DSRAcquisitionContextSRConstraintChecker::~DSRAcquisitionContextSRConstraintChecker()
{
}


OFBool DSRAcquisitionContextSRConstraintChecker::isByReferenceAllowed() const
{
    return OFFalse;
}


OFBool DSRAcquisitionContextSRConstraintChecker::isTemplateSupportRequired() const
{
    return OFFalse;
}


OFCondition DSRAcquisitionContextSRConstraintChecker::getRootTemplateIdentification(OFString &templateIdentifier,
                                                                                    OFString &mappingResource) const
{
    templateIdentifier.clear();
    mappingResource.clear();
    return EC_Normal;
}


DSRTypes::E_DocumentType DSRAcquisitionContextSRConstraintChecker::getDocumentType() const
{
    return DT_AcquisitionContextSR;
}


OFBool DSRAcquisitionContextSRConstraintChecker::checkContentRelationship(const E_ValueType sourceValueType,
                                                                          const E_RelationshipType relationshipType,
                                                                          const E_ValueType targetValueType,
                                                                          const OFBool byReference) const
{
    /* the following code implements the constraints of table A.35.16-2 in DICOM PS3.3 */
    OFBool result = OFFalse;
    /* by-reference relationships not allowed at all */
    if (!byReference)
    {
        /* row 1 of the table */
        if ((relationshipType == RT_contains) && (sourceValueType == VT_Container))
        {
            result = (targetValueType == VT_Code) || (targetValueType == VT_Container) || (targetValueType == VT_DateTime) ||
                     (targetValueType == VT_Num)  || (targetValueType == VT_PName)     || (targetValueType == VT_Text)     ||
                     (targetValueType == VT_Time) || (targetValueType == VT_UIDRef);
        }
        /* row 2 of the table */
        else if ((relationshipType == RT_hasObsContext) && ((sourceValueType == VT_Container)))
        {
            result = (targetValueType == VT_Code) || (targetValueType == VT_Date)  || (targetValueType == VT_DateTime) ||
                     (targetValueType == VT_Num)  || (targetValueType == VT_PName) || (targetValueType == VT_Text)     ||
                     (targetValueType == VT_Time) || (targetValueType == VT_UIDRef);
        }
        /* row 3 of the table */
        else if ((relationshipType == RT_hasObsContext) && (sourceValueType == VT_Code))
        {
            result = (targetValueType == VT_Code);
        }
        /* row 4 of the table */
        else if (relationshipType == RT_hasConceptMod)
        {
            result = (targetValueType == VT_Code) || (targetValueType == VT_Text);
        }
        /* row 5 of the table */
        else if ((relationshipType == RT_hasProperties) && (sourceValueType == VT_Code))
        {
            result = (targetValueType == VT_Code)     || (targetValueType == VT_DateTime) || (targetValueType == VT_Num) ||
                     (targetValueType == VT_SCoord3D) || (targetValueType == VT_Text);
        }
    }
    return result;
}
