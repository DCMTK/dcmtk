/*
 *
 *  Copyright (C) 2010, OFFIS e.V.
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
 *    classes: DSRColonCadSRConstraintChecker
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:14:40 $
 *  CVS/RCS Revision: $Revision: 1.3 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/dsrcolcc.h"


DSRColonCadSRConstraintChecker::DSRColonCadSRConstraintChecker()
  : DSRIODConstraintChecker()
{
}


DSRColonCadSRConstraintChecker::~DSRColonCadSRConstraintChecker()
{
}


OFBool DSRColonCadSRConstraintChecker::isByReferenceAllowed() const
{
    return OFTrue;
}


OFBool DSRColonCadSRConstraintChecker::isTemplateSupportRequired() const
{
    return OFTrue;
}


const char *DSRColonCadSRConstraintChecker::getRootTemplateIdentifier() const
{
    return "4120";
}


DSRTypes::E_DocumentType DSRColonCadSRConstraintChecker::getDocumentType() const
{
    return DT_ColonCadSR;
}


OFBool DSRColonCadSRConstraintChecker::checkContentRelationship(const E_ValueType sourceValueType,
                                                                const E_RelationshipType relationshipType,
                                                                const E_ValueType targetValueType,
                                                                const OFBool byReference) const
{
    /* the following code implements the constraints of table A.35.10-2 in DICOM PS3.3 */
    OFBool result = OFFalse;
    /* row 1 of the table */
    if ((relationshipType == RT_contains) && !byReference && (sourceValueType == VT_Container))
    {
        result = (targetValueType == VT_Code) || (targetValueType == VT_Num) || (targetValueType == VT_Image) ||
                 (targetValueType == VT_Container) || (targetValueType == VT_UIDRef) || (targetValueType == VT_Date) ||
                 (targetValueType == VT_Time);
    }
    /* row 2 of the table */
    else if ((relationshipType == RT_hasObsContext) && !byReference && ((sourceValueType == VT_Container) ||
        (sourceValueType == VT_Text) || (sourceValueType == VT_Code) || (sourceValueType == VT_Num)))
    {
        result = (targetValueType == VT_Text) || (targetValueType == VT_Code) || (targetValueType == VT_Num) ||
                 (targetValueType == VT_Date) || (targetValueType == VT_Time) || (targetValueType == VT_PName) ||
                 (targetValueType == VT_UIDRef) || (targetValueType == VT_Composite);
    }
    /* row 3 of the table */
    else if ((relationshipType == RT_hasAcqContext) && (sourceValueType == VT_Image))
    {
        /* by-reference allowed */
        result = (targetValueType == VT_Text) || (targetValueType == VT_Code) || (targetValueType == VT_Date) ||
                 (targetValueType == VT_Time) || (targetValueType == VT_Num) || (targetValueType == VT_Container);
    }
    /* row 4 of the table */
    else if ((relationshipType == RT_hasConceptMod) && !byReference && ((sourceValueType == VT_Container) ||
        (sourceValueType == VT_Code) || (sourceValueType == VT_Composite) || (sourceValueType == VT_Num)))
    {
        result = (targetValueType == VT_Text) || (targetValueType == VT_Code);
    }
    /* row 5 the table */
    else if ((relationshipType == RT_hasProperties) && !byReference &&
        ((sourceValueType == VT_Text) || (sourceValueType == VT_Code) || (sourceValueType == VT_Num)))
    {
        result = (targetValueType == VT_Container) || (targetValueType == VT_Text) || (targetValueType == VT_Code) ||
                 (targetValueType == VT_Num) || (targetValueType == VT_Date) || (targetValueType == VT_Image) ||
                 (targetValueType == VT_SCoord) || (targetValueType == VT_SCoord3D) || (targetValueType == VT_UIDRef);
    }
    /* row 6 of the table */
    else if ((relationshipType == RT_inferredFrom) && ((sourceValueType == VT_Code) || (sourceValueType == VT_Num)))
    {
        /* by-reference allowed */
        result = (targetValueType == VT_Code) || (targetValueType == VT_Num) || (targetValueType == VT_Image) ||
                 (targetValueType == VT_SCoord) || (targetValueType == VT_SCoord3D) || (targetValueType == VT_Container) ||
                 (targetValueType == VT_Text);
    }
    /* row 7 of the table */
    else if ((relationshipType == RT_selectedFrom) && !byReference && (sourceValueType == VT_SCoord))
    {
        result = (targetValueType == VT_Image);
    }
    return result;
}


/*
 *  CVS/RCS Log:
 *  $Log: dsrcolcc.cc,v $
 *  Revision 1.3  2010-10-14 13:14:40  joergr
 *  Updated copyright header. Added reference to COPYRIGHT file.
 *
 *  Revision 1.2  2010-09-29 16:32:56  joergr
 *  Fixed wrong by-reference relationships in constraint checker for Colon CAD.
 *
 *  Revision 1.1  2010-09-28 14:07:29  joergr
 *  Added support for Colon CAD SR which requires a new value type (SCOORD3D).
 *
 *
 */
