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
 *    classes: DSREnhancedSRConstraintChecker
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:14:41 $
 *  CVS/RCS Revision: $Revision: 1.5 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/dsrenhcc.h"


DSREnhancedSRConstraintChecker::DSREnhancedSRConstraintChecker()
  : DSRIODConstraintChecker()
{
}


DSREnhancedSRConstraintChecker::~DSREnhancedSRConstraintChecker()
{
}


OFBool DSREnhancedSRConstraintChecker::isByReferenceAllowed() const
{
    return OFFalse;
}


OFBool DSREnhancedSRConstraintChecker::isTemplateSupportRequired() const
{
    return OFFalse;
}


const char *DSREnhancedSRConstraintChecker::getRootTemplateIdentifier() const
{
    return NULL;
}


DSRTypes::E_DocumentType DSREnhancedSRConstraintChecker::getDocumentType() const
{
    return DT_EnhancedSR;
}


OFBool DSREnhancedSRConstraintChecker::checkContentRelationship(const E_ValueType sourceValueType,
                                                                const E_RelationshipType relationshipType,
                                                                const E_ValueType targetValueType,
                                                                const OFBool byReference) const
{
    /* the following code implements the constraints of table A.35.2-2 in DICOM PS3.3 */
    OFBool result = OFFalse;
    /* by-reference relationships not allowed at all */
    if (!byReference)
    {
        /* row 1 of the table */
        if ((relationshipType == RT_contains) && (sourceValueType == VT_Container))
        {
            result = (targetValueType == VT_Text)     || (targetValueType == VT_Code)      || (targetValueType == VT_Num)    ||
                     (targetValueType == VT_DateTime) || (targetValueType == VT_Date)      || (targetValueType == VT_Time)   ||
                     (targetValueType == VT_UIDRef)   || (targetValueType == VT_PName)     || (targetValueType == VT_SCoord) ||
                     (targetValueType == VT_TCoord)   || (targetValueType == VT_Composite) || (targetValueType == VT_Image)  ||
                     (targetValueType == VT_Waveform) || (targetValueType == VT_Container);
        }
        /* row 2 of the table */
        else if ((relationshipType == RT_hasObsContext) && (sourceValueType == VT_Container))
        {
            result = (targetValueType == VT_Text)     || (targetValueType == VT_Code)  || (targetValueType == VT_Num)  ||
                     (targetValueType == VT_DateTime) || (targetValueType == VT_Date)  || (targetValueType == VT_Time) ||
                     (targetValueType == VT_UIDRef)   || (targetValueType == VT_PName) || (targetValueType == VT_Composite);
        }
        /* row 3 of the table */
        else if ((relationshipType == RT_hasAcqContext) && ((sourceValueType == VT_Container) || (sourceValueType == VT_Image) ||
            (sourceValueType == VT_Waveform) || (sourceValueType == VT_Composite) || (sourceValueType == VT_Num /* see CP 571 */)))
        {
            result = (targetValueType == VT_Text)     || (targetValueType == VT_Code) || (targetValueType == VT_Num)  ||
                     (targetValueType == VT_DateTime) || (targetValueType == VT_Date) || (targetValueType == VT_Time) ||
                     (targetValueType == VT_UIDRef)   || (targetValueType == VT_PName);
        }
        /* row 4 of the table */
        else if (relationshipType == RT_hasConceptMod)
        {
            result = (targetValueType == VT_Text) || (targetValueType == VT_Code);
        }
        /* row 5 and 6 of the table */
        else if (((relationshipType == RT_hasProperties) || (relationshipType == RT_inferredFrom)) &&
            ((sourceValueType == VT_Text) || (sourceValueType == VT_Code) || (sourceValueType == VT_Num)))
        {
            result = (targetValueType == VT_Text)     || (targetValueType == VT_Code)     || (targetValueType == VT_Num)       ||
                     (targetValueType == VT_DateTime) || (targetValueType == VT_Date)     || (targetValueType == VT_Time)      ||
                     (targetValueType == VT_UIDRef)   || (targetValueType == VT_PName)    || (targetValueType == VT_Composite) ||
                     (targetValueType == VT_Image)    || (targetValueType == VT_Waveform) || (targetValueType == VT_SCoord)    ||
                     (targetValueType == VT_TCoord);
        }
        /* row 7 of the table */
        else if ((relationshipType == RT_selectedFrom) && (sourceValueType == VT_SCoord))
        {
            result = (targetValueType == VT_Image);
        }
        /* row 8 of the table */
        else if ((relationshipType == RT_selectedFrom) && (sourceValueType == VT_TCoord))
        {
            result = (targetValueType == VT_SCoord) || (targetValueType == VT_Image) || (targetValueType == VT_Waveform);
        }
    }
    return result;
}


/*
 *  CVS/RCS Log:
 *  $Log: dsrenhcc.cc,v $
 *  Revision 1.5  2010-10-14 13:14:41  joergr
 *  Updated copyright header. Added reference to COPYRIGHT file.
 *
 *  Revision 1.4  2005-12-08 15:47:52  meichel
 *  Changed include path schema for all DCMTK header files
 *
 *  Revision 1.3  2005/07/27 16:55:46  joergr
 *  Added provisional support for CP571, i.e. allow certain relationships needed
 *  for TID 5203 (Echo Measurement).
 *
 *  Revision 1.2  2003/10/09 13:00:41  joergr
 *  Added check for root template identifier when reading an SR document.
 *
 *  Revision 1.1  2003/09/15 14:16:50  joergr
 *  Introduced new class to facilitate checking of SR IOD relationship content
 *  constraints. Replaced old implementation distributed over numerous classes.
 *
 */
