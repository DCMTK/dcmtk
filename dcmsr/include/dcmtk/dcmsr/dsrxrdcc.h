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
 *  Module: dcmsr
 *
 *  Author: Joerg Riesmeier
 *
 *  Purpose:
 *    classes: DSRXRayRadiationDoseSRConstraintChecker
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:16:33 $
 *  CVS/RCS Revision: $Revision: 1.4 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#ifndef DSRXRDCC_H
#define DSRXRDCC_H

#include "dcmtk/config/osconfig.h"   /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/dsriodcc.h"


/*---------------------*
 *  class declaration  *
 *---------------------*/

/** Class for checking the content relationship constraints of the X-Ray Radiation Dose SR IOD
 */
class DSRXRayRadiationDoseSRConstraintChecker
  : public DSRIODConstraintChecker
{

  public:

    /** default constructor
     */
    DSRXRayRadiationDoseSRConstraintChecker();

    /** destructor
     */
    virtual ~DSRXRayRadiationDoseSRConstraintChecker();

    /** check whether by-reference relationships are allowed for this SR IOD
     ** @return always returns OFFalse, i.e. by-reference relationships are not allowed
     */
    virtual OFBool isByReferenceAllowed() const;

    /** check whether this SR IOD requires template support
     ** @return always returns OFFalse, i.e. template support is not required
     */
    virtual OFBool isTemplateSupportRequired() const;

    /** get identifier of the root template
     ** @return always returns NULL (no template required)
     */
    virtual const char *getRootTemplateIdentifier() const;

    /** get the associated document type of the SR IOD
     ** @return document type (DSRTypes::DT_XRayRadiationDoseSR)
     */
    virtual E_DocumentType getDocumentType() const;

    /** check whether specified content relationship is allowed for this IOD
     ** @param  sourceValueType   value type of the source content item to be checked
     *  @param  relationshipType  type of relationship between source and target item
     *  @param  targetValueType   value type of the target content item to be checked
     *  @param  byReference       optional flag indicating whether the node/relationship
     *                            should be added by-value (default) or by-reference
     ** @return OFTrue if content relationship is allowed, OFFalse otherwise
     */
    virtual OFBool checkContentRelationship(const E_ValueType sourceValueType,
                                            const E_RelationshipType relationshipType,
                                            const E_ValueType targetValueType,
                                            const OFBool byReference = OFFalse) const;
};


#endif


/*
 *  CVS/RCS Log:
 *  $Log: dsrxrdcc.h,v $
 *  Revision 1.4  2010-10-14 13:16:33  joergr
 *  Updated copyright header. Added reference to COPYRIGHT file.
 *
 *  Revision 1.3  2008-01-08 09:35:35  joergr
 *  Fixed reference to wrong SR IOD in API documentation.
 *
 *  Revision 1.2  2005/12/08 16:05:37  meichel
 *  Changed include path schema for all DCMTK header files
 *
 *  Revision 1.1  2005/11/30 12:05:59  joergr
 *  Added support for X-Ray Radiation Dose SR documents.
 *
 *
 */
