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
 *    classes: DSRSpectaclePrescriptionReportConstraintChecker
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:14:42 $
 *  CVS/RCS Revision: $Revision: 1.2 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
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


const char *DSRSpectaclePrescriptionReportConstraintChecker::getRootTemplateIdentifier() const
{
    return "2020";
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
                     (targetValueType == VT_Num) || (targetValueType == VT_Text);
        }
    }
    return result;
}


/*
 *  CVS/RCS Log:
 *  $Log: dsrspecc.cc,v $
 *  Revision 1.2  2010-10-14 13:14:42  joergr
 *  Updated copyright header. Added reference to COPYRIGHT file.
 *
 *  Revision 1.1  2010-09-30 08:56:28  joergr
 *  Added support for the Spectacle Prescription Report IOD.
 *
 *
 */
