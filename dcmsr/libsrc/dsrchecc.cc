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
 *    classes: DSRChestCadSRConstraintChecker
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:14:40 $
 *  CVS/RCS Revision: $Revision: 1.8 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/dsrchecc.h"


DSRChestCadSRConstraintChecker::DSRChestCadSRConstraintChecker()
  : DSRIODConstraintChecker()
{
}


DSRChestCadSRConstraintChecker::~DSRChestCadSRConstraintChecker()
{
}


OFBool DSRChestCadSRConstraintChecker::isByReferenceAllowed() const
{
    return OFTrue;
}


OFBool DSRChestCadSRConstraintChecker::isTemplateSupportRequired() const
{
    return OFTrue;
}


const char *DSRChestCadSRConstraintChecker::getRootTemplateIdentifier() const
{
    return "4100";
}


DSRTypes::E_DocumentType DSRChestCadSRConstraintChecker::getDocumentType() const
{
    return DT_ChestCadSR;
}


OFBool DSRChestCadSRConstraintChecker::checkContentRelationship(const E_ValueType sourceValueType,
                                                                const E_RelationshipType relationshipType,
                                                                const E_ValueType targetValueType,
                                                                const OFBool byReference) const
{
    /* the following code implements the constraints of table A.35.6-2 in DICOM PS3.3 */
    OFBool result = OFFalse;
    /* row 1 of the table */
    if ((relationshipType == RT_contains) && !byReference && (sourceValueType == VT_Container))
    {
        result = (targetValueType == VT_Code) || (targetValueType == VT_Num) || (targetValueType == VT_Image) ||
                 (targetValueType == VT_Container);
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
    else if ((relationshipType == RT_hasAcqContext) && !byReference &&
        ((sourceValueType == VT_Image) || (sourceValueType == VT_Waveform)))
    {
        result = (targetValueType == VT_Text) || (targetValueType == VT_Code) || (targetValueType == VT_Num) ||
                 (targetValueType == VT_Date) || (targetValueType == VT_Time);
    }
    /* row 4 of the table */
    else if ((relationshipType == RT_hasConceptMod) && !byReference && ((sourceValueType == VT_Container) ||
        (sourceValueType == VT_Code) || (sourceValueType == VT_Composite) || (sourceValueType == VT_Num)))
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
                 (targetValueType == VT_Waveform) || (targetValueType == VT_SCoord) || (targetValueType == VT_TCoord) ||
                 (targetValueType == VT_UIDRef);
    }
    /* row 6 of the table */
    else if ((relationshipType == RT_inferredFrom) && ((sourceValueType == VT_Code) || (sourceValueType == VT_Num)))
    {
        /* by-reference allowed */
        result = (targetValueType == VT_Code) || (targetValueType == VT_Num) || (targetValueType == VT_Image) ||
                 (targetValueType == VT_Waveform) || (targetValueType == VT_SCoord) || (targetValueType == VT_TCoord) ||
                 (targetValueType == VT_Container) || (targetValueType == VT_Text);
    }
    /* row 7 of the table */
    else if ((relationshipType == RT_selectedFrom) && (sourceValueType == VT_SCoord))
    {
        /* by-reference allowed */
        result = (targetValueType == VT_Image);
    }
    /* row 8 of the table */
    else if ((relationshipType == RT_selectedFrom) && (sourceValueType == VT_TCoord))
    {
        /* by-reference allowed */
        result = (targetValueType == VT_SCoord) || (targetValueType == VT_Image) || (targetValueType == VT_Waveform);
    }
    return result;
}


/*
 *  CVS/RCS Log:
 *  $Log: dsrchecc.cc,v $
 *  Revision 1.8  2010-10-14 13:14:40  joergr
 *  Updated copyright header. Added reference to COPYRIGHT file.
 *
 *  Revision 1.7  2010-06-09 16:35:12  joergr
 *  Removed references to CP 767.
 *
 *  Revision 1.6  2009-08-07 08:36:36  joergr
 *  Added missing relationship content constraint introduced with DICOM 2007.
 *
 *  Revision 1.5  2007-11-30 16:54:28  joergr
 *  Updated relationship content constraints according to CP 767.
 *
 *  Revision 1.4  2005/12/08 15:47:38  meichel
 *  Changed include path schema for all DCMTK header files
 *
 *  Revision 1.3  2003/10/09 13:00:41  joergr
 *  Added check for root template identifier when reading an SR document.
 *
 *  Revision 1.2  2003/10/06 09:55:13  joergr
 *  Corrected source code formatting.
 *
 *  Revision 1.1  2003/09/15 14:15:36  joergr
 *  Added content relationship constraint checking support for Mammography CAD
 *  SR and Chest CAD SR.
 *
 */
