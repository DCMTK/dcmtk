/*
 *
 *  Copyright (C) 2003-2010, OFFIS e.V.
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
 *    classes: DSRMammographyCadSRConstraintChecker
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:14:41 $
 *  CVS/RCS Revision: $Revision: 1.12 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/dsrmamcc.h"


DSRMammographyCadSRConstraintChecker::DSRMammographyCadSRConstraintChecker()
  : DSRIODConstraintChecker()
{
}


DSRMammographyCadSRConstraintChecker::~DSRMammographyCadSRConstraintChecker()
{
}


OFBool DSRMammographyCadSRConstraintChecker::isByReferenceAllowed() const
{
    return OFTrue;
}


OFBool DSRMammographyCadSRConstraintChecker::isTemplateSupportRequired() const
{
    return OFTrue;
}


const char *DSRMammographyCadSRConstraintChecker::getRootTemplateIdentifier() const
{
    return "4000";
}


DSRTypes::E_DocumentType DSRMammographyCadSRConstraintChecker::getDocumentType() const
{
    return DT_MammographyCadSR;
}


OFBool DSRMammographyCadSRConstraintChecker::checkContentRelationship(const E_ValueType sourceValueType,
                                                                      const E_RelationshipType relationshipType,
                                                                      const E_ValueType targetValueType,
                                                                      const OFBool byReference) const
{
    /* the following code implements the constraints of table A.35.5-2 in DICOM PS3.3 */
    OFBool result = OFFalse;
    /* row 1 of the table */
    if ((relationshipType == RT_contains) && !byReference && (sourceValueType == VT_Container))
    {
        result = (targetValueType == VT_Code) || (targetValueType == VT_Num) || (targetValueType == VT_SCoord) ||
                 (targetValueType == VT_Image) || (targetValueType == VT_Container) ||
                 (targetValueType == VT_Text) || (targetValueType == VT_Date);
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
    else if ((relationshipType == RT_hasAcqContext) && !byReference && (sourceValueType == VT_Image))
    {
        result = (targetValueType == VT_Text) || (targetValueType == VT_Code) || (targetValueType == VT_Date) ||
                 (targetValueType == VT_Time) || (targetValueType == VT_Num);
    }
    /* row 4 of the table */
    else if ((relationshipType == RT_hasConceptMod) && !byReference &&
        ((sourceValueType == VT_Container) || (sourceValueType == VT_Code) ||
        (sourceValueType == VT_Num) || (sourceValueType == VT_Composite)))
    {
        result = (targetValueType == VT_Text) || (targetValueType == VT_Code);
    }
    /* row 5 the table */
    else if ((relationshipType == RT_hasProperties) &&
        ((sourceValueType == VT_Text) || (sourceValueType == VT_Code) || (sourceValueType == VT_Num)))
    {
        /* by-reference allowed */
        result = (targetValueType == VT_Container) || (targetValueType == VT_Text) || (targetValueType == VT_Code) ||
                 (targetValueType == VT_Num) || (targetValueType == VT_Date) || (targetValueType == VT_Image) ||
                 (targetValueType == VT_SCoord) || (targetValueType == VT_UIDRef);
    }
    /* row 6 of the table */
    else if ((relationshipType == RT_inferredFrom) && ((sourceValueType == VT_Code) || (sourceValueType == VT_Num)))
    {
        /* by-reference allowed */
        result = (targetValueType == VT_Code) || (targetValueType == VT_Num) || (targetValueType == VT_SCoord) ||
                 (targetValueType == VT_Container) || (targetValueType == VT_Text) ||
                 (targetValueType == VT_Image);
    }
    /* row 7 of the table */
    else if ((relationshipType == RT_selectedFrom) && (sourceValueType == VT_SCoord))
    {
        /* by-reference allowed */
        result = (targetValueType == VT_Image);
    }
    return result;
}


/*
 *  CVS/RCS Log:
 *  $Log: dsrmamcc.cc,v $
 *  Revision 1.12  2010-10-14 13:14:41  joergr
 *  Updated copyright header. Added reference to COPYRIGHT file.
 *
 *  Revision 1.11  2010-06-09 16:35:12  joergr
 *  Removed references to CP 767.
 *
 *  Revision 1.10  2009-08-07 08:36:36  joergr
 *  Added missing relationship content constraint introduced with DICOM 2007.
 *
 *  Revision 1.9  2007-11-30 16:57:50  joergr
 *  Updated relationship content constraints according to CP 767.
 *
 *  Revision 1.8  2007/11/15 16:57:31  joergr
 *  Removed preliminary patch for TID 4023 (since not yet approved). Documented
 *  modification required for CP 492 (has already been added by the last commit).
 *
 *  Revision 1.7  2007/11/15 16:45:42  joergr
 *  Added support for output in XHTML 1.1 format. -> incorrect log entry
 *
 *  Revision 1.6  2006/03/24 11:11:45  joergr
 *  Added full support for CP 545, i.e. allow by-reference HAS PROPERTIES
 *  relationships.
 *
 *  Revision 1.5  2005/12/08 15:47:58  meichel
 *  Changed include path schema for all DCMTK header files
 *
 *  Revision 1.4  2005/07/27 16:55:04  joergr
 *  Added workaround to allow by-reference relationships as needed for some
 *  Mammography CAD SR templates.
 *
 *  Revision 1.3  2004/11/18 13:55:52  joergr
 *  Added workaround to fix issue with inconsistent definitions in DICOM part 3
 *  and part 16 regarding Mammography CAD SR.
 *
 *  Revision 1.2  2003/10/09 13:00:41  joergr
 *  Added check for root template identifier when reading an SR document.
 *
 *  Revision 1.1  2003/09/15 14:15:36  joergr
 *  Added content relationship constraint checking support for Mammography CAD
 *  SR and Chest CAD SR.
 *
 */
