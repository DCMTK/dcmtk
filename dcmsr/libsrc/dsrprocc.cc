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
 *    classes: DSRProcedureLogConstraintChecker
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:14:41 $
 *  CVS/RCS Revision: $Revision: 1.3 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/dsrprocc.h"


DSRProcedureLogConstraintChecker::DSRProcedureLogConstraintChecker()
  : DSRIODConstraintChecker()
{
}


DSRProcedureLogConstraintChecker::~DSRProcedureLogConstraintChecker()
{
}


OFBool DSRProcedureLogConstraintChecker::isByReferenceAllowed() const
{
    return OFFalse;
}


OFBool DSRProcedureLogConstraintChecker::isTemplateSupportRequired() const
{
    return OFTrue;
}


const char *DSRProcedureLogConstraintChecker::getRootTemplateIdentifier() const
{
    return "3001";
}


DSRTypes::E_DocumentType DSRProcedureLogConstraintChecker::getDocumentType() const
{
    return DT_ProcedureLog;
}


OFBool DSRProcedureLogConstraintChecker::checkContentRelationship(const E_ValueType sourceValueType,
                                                                  const E_RelationshipType relationshipType,
                                                                  const E_ValueType targetValueType,
                                                                  const OFBool byReference) const
{
    /* the following code implements the constraints of table A.35.7-2 in DICOM PS3.3 */
    OFBool result = OFFalse;
    /* by-reference relationships not allowed at all */
    if (!byReference)
    {
        /* row 1 of the table */
        if ((relationshipType == RT_contains) && (sourceValueType == VT_Container))
        {
            result = (targetValueType == VT_Text)  || (targetValueType == VT_Code)      || (targetValueType == VT_Num)   ||
                     (targetValueType == VT_PName) || (targetValueType == VT_Composite) || (targetValueType == VT_Image) ||
                     (targetValueType == VT_Waveform);
        }
        /* row 2 of the table */
        else if (relationshipType == RT_hasObsContext)
        {
            result = (targetValueType == VT_Text)     || (targetValueType == VT_Code)   || (targetValueType == VT_Num)  ||
                     (targetValueType == VT_DateTime) || (targetValueType == VT_UIDRef) || (targetValueType == VT_PName);
        }
        /* row 3 of the table */
        else if ((relationshipType == RT_hasAcqContext) && ((sourceValueType == VT_Container) ||
            (sourceValueType == VT_Image) || (sourceValueType == VT_Waveform) || (sourceValueType == VT_Composite)))
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
        /* row 5 of the table */
        else if ((relationshipType == RT_hasProperties) && (sourceValueType != VT_Container))
        {
            result = (targetValueType == VT_Text)     || (targetValueType == VT_Code)   || (targetValueType == VT_Num)  ||
                     (targetValueType == VT_DateTime) || (targetValueType == VT_UIDRef) || (targetValueType == VT_PName);
        }
        /* row 6 of the table */
        else if ((relationshipType == RT_inferredFrom) &&
            ((sourceValueType == VT_Text) || (sourceValueType == VT_Code) || (sourceValueType == VT_Num)))
        {
            result = (targetValueType == VT_Composite) || (targetValueType == VT_Image) || (targetValueType == VT_Waveform);
        }
    }
    return result;
}


/*
 *  CVS/RCS Log:
 *  $Log: dsrprocc.cc,v $
 *  Revision 1.3  2010-10-14 13:14:41  joergr
 *  Updated copyright header. Added reference to COPYRIGHT file.
 *
 *  Revision 1.2  2005-12-08 15:48:02  meichel
 *  Changed include path schema for all DCMTK header files
 *
 *  Revision 1.1  2003/10/09 13:00:20  joergr
 *  Added support for Procedure Log.
 *
 *
 */
