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
 *  Module: dcmsr
 *
 *  Author: Joerg Riesmeier
 *
 *  Purpose:
 *    classes: DSRImplantationPlanSRDocumentConstraintChecker
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-11-05 11:06:53 $
 *  CVS/RCS Revision: $Revision: 1.1 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#ifndef DSRIMPCC_H
#define DSRIMPCC_H

#include "dcmtk/config/osconfig.h"   /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/dsriodcc.h"


/*---------------------*
 *  class declaration  *
 *---------------------*/

/** Class for checking the content relationship constraints of the
 *  Implantation Plan SR Document IOD
 */
class DSRImplantationPlanSRDocumentConstraintChecker
  : public DSRIODConstraintChecker
{

  public:

    /** default constructor
     */
    DSRImplantationPlanSRDocumentConstraintChecker();

    /** destructor
     */
    virtual ~DSRImplantationPlanSRDocumentConstraintChecker();

    /** check whether by-reference relationships are allowed for this SR IOD
     ** @return always returns OFFalse, i.e. by-reference relationships are not allowed
     */
    virtual OFBool isByReferenceAllowed() const;

    /** check whether this SR IOD requires template support
     ** @return always returns OFTrue, i.e. template support is required
     */
    virtual OFBool isTemplateSupportRequired() const;

    /** get identifier of the root template
     ** @return root template identifier (TID) "7000"
     */
    virtual const char *getRootTemplateIdentifier() const;

    /** get the associated document type of the SR IOD
     ** @return document type (DSRTypes::DT_ImplantationPlanSRDocument)
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
 *  $Log: dsrimpcc.h,v $
 *  Revision 1.1  2010-11-05 11:06:53  joergr
 *  Added support for new Implantation Plan SR Document Storage SOP Class.
 *
 *
 *
 */
