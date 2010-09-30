/*
 *
 *  Copyright (C) 2010, OFFIS
 *
 *  This software and supporting documentation were developed by
 *
 *    Kuratorium OFFIS e.V.
 *    Healthcare Information and Communication Systems
 *    Escherweg 2
 *    D-26121 Oldenburg, Germany
 *
 *  THIS SOFTWARE IS MADE AVAILABLE,  AS IS,  AND OFFIS MAKES NO  WARRANTY
 *  REGARDING  THE  SOFTWARE,  ITS  PERFORMANCE,  ITS  MERCHANTABILITY  OR
 *  FITNESS FOR ANY PARTICULAR USE, FREEDOM FROM ANY COMPUTER DISEASES  OR
 *  ITS CONFORMITY TO ANY SPECIFICATION. THE ENTIRE RISK AS TO QUALITY AND
 *  PERFORMANCE OF THE SOFTWARE IS WITH THE USER.
 *
 *  Module:  dcmsr
 *
 *  Author:  Joerg Riesmeier
 *
 *  Purpose:
 *    classes: DSRMacularGridThicknessAndVolumeReportConstraintChecker
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-09-30 08:56:46 $
 *  CVS/RCS Revision: $Revision: 1.1 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
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


const char *DSRMacularGridThicknessAndVolumeReportConstraintChecker::getRootTemplateIdentifier() const
{
    return "2100";
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
            result = (targetValueType == VT_Code) || (targetValueType == VT_PName) ||
                     (targetValueType == VT_Text) || (targetValueType == VT_UIDRef) ||
                     (targetValueType == VT_Date) || (targetValueType == VT_Num);
        }
        /* row 2 of the table */
        else if ((relationshipType == RT_contains) && (sourceValueType == VT_Container))
        {
            result = (targetValueType == VT_Container) || (targetValueType == VT_Num) ||
                     (targetValueType == VT_Text) || (targetValueType == VT_Code);
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


/*
 *  CVS/RCS Log:
 *  $Log: dsrmaccc.cc,v $
 *  Revision 1.1  2010-09-30 08:56:46  joergr
 *  Added support for the Macular Grid Thickness and Volume Report IOD.
 *
 *
 */
