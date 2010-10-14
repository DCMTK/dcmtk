/*
 *
 *  Copyright (C) 2005-2010, OFFIS e.V.
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
 *    classes: DSRXRayRadiationDoseSRConstraintChecker
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:14:42 $
 *  CVS/RCS Revision: $Revision: 1.6 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/dsrxrdcc.h"


DSRXRayRadiationDoseSRConstraintChecker::DSRXRayRadiationDoseSRConstraintChecker()
  : DSRIODConstraintChecker()
{
}


DSRXRayRadiationDoseSRConstraintChecker::~DSRXRayRadiationDoseSRConstraintChecker()
{
}


OFBool DSRXRayRadiationDoseSRConstraintChecker::isByReferenceAllowed() const
{
    return OFFalse;
}


OFBool DSRXRayRadiationDoseSRConstraintChecker::isTemplateSupportRequired() const
{
    return OFFalse;
}


const char *DSRXRayRadiationDoseSRConstraintChecker::getRootTemplateIdentifier() const
{
    return NULL;
}


DSRTypes::E_DocumentType DSRXRayRadiationDoseSRConstraintChecker::getDocumentType() const
{
    return DT_XRayRadiationDoseSR;
}


OFBool DSRXRayRadiationDoseSRConstraintChecker::checkContentRelationship(const E_ValueType sourceValueType,
                                                                         const E_RelationshipType relationshipType,
                                                                         const E_ValueType targetValueType,
                                                                         const OFBool byReference) const
{
    /* the following code implements the constraints of table A.35.8-2 in DICOM PS3.3 */
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
            result = (targetValueType == VT_Text)   || (targetValueType == VT_Code)      || (targetValueType == VT_DateTime) ||
                     (targetValueType == VT_UIDRef) || (targetValueType == VT_PName);
        }
        /* row 3 of the table */
        else if ((relationshipType == RT_hasObsContext) && ((sourceValueType == VT_Text) || (sourceValueType == VT_Code) ||
            (sourceValueType == VT_Num)))
        {
            result = (targetValueType == VT_Text)     || (targetValueType == VT_Code)   || (targetValueType == VT_Num)   ||
                     (targetValueType == VT_DateTime) || (targetValueType == VT_UIDRef) || (targetValueType == VT_PName) ||
                     (targetValueType == VT_Composite);
        }
        /* row 4 of the table */
        else if ((relationshipType == RT_hasAcqContext) && ((sourceValueType == VT_Container) || (sourceValueType == VT_Image) ||
            (sourceValueType == VT_Composite)))
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
        /* row 6 of the table */
        else if ((relationshipType == RT_hasProperties) && ((sourceValueType == VT_Text) || (sourceValueType == VT_Code) ||
            (sourceValueType == VT_Num)))
        {
            result = (targetValueType == VT_Text)     || (targetValueType == VT_Code)      || (targetValueType == VT_Num)   ||
                     (targetValueType == VT_DateTime) || (targetValueType == VT_UIDRef)    || (targetValueType == VT_PName) ||
                     (targetValueType == VT_Image)    || (targetValueType == VT_Composite) || (targetValueType == VT_Container);
        }
        /* row 7 of the table */
        else if ((relationshipType == RT_inferredFrom) && ((sourceValueType == VT_Text) || (sourceValueType == VT_Code) ||
            (sourceValueType == VT_Num)))
        {
            result = (targetValueType == VT_Text)      || (targetValueType == VT_Code)   || (targetValueType == VT_Num)   ||
                     (targetValueType == VT_DateTime)  || (targetValueType == VT_UIDRef) || (targetValueType == VT_Image) ||
                     (targetValueType == VT_Composite) || (targetValueType == VT_Container);
        }
    }
    return result;
}


/*
 *  CVS/RCS Log:
 *  $Log: dsrxrdcc.cc,v $
 *  Revision 1.6  2010-10-14 13:14:42  joergr
 *  Updated copyright header. Added reference to COPYRIGHT file.
 *
 *  Revision 1.5  2010-02-05 15:18:36  joergr
 *  Updated reference to row numbers in comments (based on the current edition
 *  of the DICOM standard). Added missing "else" statement.
 *
 *  Revision 1.4  2008-03-11 11:10:17  joergr
 *  Fixed wrong CVS log entry.
 *
 *  Revision 1.3  2007/11/29 13:48:12  joergr
 *  Added support for Supplement 127 (CT Radiation Dose Reporting).
 *
 *  Revision 1.2  2005/12/08 15:48:26  meichel
 *  Changed include path schema for all DCMTK header files
 *
 *  Revision 1.1  2005/11/30 12:06:26  joergr
 *  Added support for X-Ray Radiation Dose SR documents.
 *
 *
 */
