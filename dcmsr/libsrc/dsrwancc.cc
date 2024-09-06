/*
 *
 *  Copyright (C) 2024, J. Riesmeier, Oldenburg, Germany
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
 *    classes: DSRWaveformAnnotationSRConstraintChecker
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/dsrwancc.h"


DSRWaveformAnnotationSRConstraintChecker::DSRWaveformAnnotationSRConstraintChecker()
  : DSRIODConstraintChecker()
{
}


DSRWaveformAnnotationSRConstraintChecker::~DSRWaveformAnnotationSRConstraintChecker()
{
}


OFBool DSRWaveformAnnotationSRConstraintChecker::isByReferenceAllowed() const
{
    return OFTrue;
}


OFBool DSRWaveformAnnotationSRConstraintChecker::isTemplateSupportRequired() const
{
    return OFTrue;
}


OFCondition DSRWaveformAnnotationSRConstraintChecker::getRootTemplateIdentification(OFString &templateIdentifier,
                                                                                    OFString &mappingResource) const
{
    templateIdentifier = "3750";
    mappingResource = "DCMR";
    return EC_Normal;
}


DSRTypes::E_DocumentType DSRWaveformAnnotationSRConstraintChecker::getDocumentType() const
{
    return DT_WaveformAnnotationSR;
}


OFBool DSRWaveformAnnotationSRConstraintChecker::checkContentRelationship(const E_ValueType sourceValueType,
                                                                          const E_RelationshipType relationshipType,
                                                                          const E_ValueType targetValueType,
                                                                          const OFBool byReference) const
{
    /* the following code implements the constraints of table A.35.23-2 in DICOM PS3.3 */
    OFBool result = OFFalse;
    /* row 1 of the table */
    if ((relationshipType == RT_contains) && !byReference && (sourceValueType == VT_Container))
    {
        result = (targetValueType == VT_Text)   || (targetValueType == VT_Code)     || (targetValueType == VT_Num)      ||
                 (targetValueType == VT_TCoord) || (targetValueType == VT_Waveform) || (targetValueType == VT_Container);
    }
    /* row 2 of the table */
    else if ((relationshipType == RT_hasObsContext) && !byReference && ((sourceValueType == VT_Container) ||
        (sourceValueType == VT_Code) || (sourceValueType == VT_Num) || (sourceValueType == VT_Text)))
    {
        result = (targetValueType == VT_Code)   || (targetValueType == VT_PName) || (targetValueType == VT_Text) ||
                 (targetValueType == VT_UIDRef) || (targetValueType == VT_Date)  || (targetValueType == VT_Num)  ||
                 (targetValueType == VT_Container);
    }
    /* row 3 of the table */
    else if ((relationshipType == RT_hasAcqContext) && !byReference &&
        ((sourceValueType == VT_Container) || (sourceValueType == VT_Waveform)))
    {
        result = (targetValueType == VT_Code)     || (targetValueType == VT_Date) || (targetValueType == VT_Time)  ||
                 (targetValueType == VT_DateTime) || (targetValueType == VT_Num)  || (targetValueType == VT_UIDRef);
    }
    /* row 4 of the table */
    else if ((relationshipType == RT_hasConceptMod) && !byReference && ((sourceValueType == VT_Container) ||
        (sourceValueType == VT_Code) || (sourceValueType == VT_Num) || (sourceValueType == VT_Text)))
    {
        result = (targetValueType == VT_Text) || (targetValueType == VT_Code);
    }
    /* row 5 of the table */
    else if ((relationshipType == RT_hasProperties) && !byReference &&
        ((sourceValueType == VT_Code) || (sourceValueType == VT_Num) || (sourceValueType == VT_Text)))
    {
        result = (targetValueType == VT_Code) || (targetValueType == VT_Text) || (targetValueType == VT_Num);
    }
    /* row 6 of the table */
    else if ((relationshipType == RT_inferredFrom) &&
        ((sourceValueType == VT_Code) || (sourceValueType == VT_Num) || (sourceValueType == VT_Text)))
    {
        /* by-reference allowed */
        result = (targetValueType == VT_Waveform) || (targetValueType == VT_TCoord);
    }
    /* row 7 of the table */
    else if ((relationshipType == RT_selectedFrom) && (sourceValueType == VT_TCoord))
    {
        /* by-reference allowed */
        result = (targetValueType == VT_Waveform);
    }
    return result;
}
