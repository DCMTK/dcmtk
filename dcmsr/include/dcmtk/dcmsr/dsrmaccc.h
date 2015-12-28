/*
 *
 *  Copyright (C) 2010-2015, OFFIS e.V.
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
 *    classes: DSRMacularGridThicknessAndVolumeReportConstraintChecker
 *
 */


#ifndef DSRMACCC_H
#define DSRMACCC_H

#include "dcmtk/config/osconfig.h"   /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/dsriodcc.h"


/*---------------------*
 *  class declaration  *
 *---------------------*/

/** Class for checking the relationship content constraints of the Macular Grid
 *  Thickness And Volume Report IOD.
 *  According to DICOM PS 3.3: "The document shall be constructed from TID 2100
 *  (Macular Grid Thickness and Volume Report) invoked at the root node."
 */
class DCMTK_DCMSR_EXPORT DSRMacularGridThicknessAndVolumeReportConstraintChecker
  : public DSRIODConstraintChecker
{

  public:

    /** default constructor
     */
    DSRMacularGridThicknessAndVolumeReportConstraintChecker();

    /** destructor
     */
    virtual ~DSRMacularGridThicknessAndVolumeReportConstraintChecker();

    /** check whether by-reference relationships are allowed for this SR IOD
     ** @return always returns OFFalse, i.e. by-reference relationships are not allowed
     */
    virtual OFBool isByReferenceAllowed() const;

    /** check whether this SR IOD requires template support
     ** @return always returns OFTrue, i.e. template support is required
     */
    virtual OFBool isTemplateSupportRequired() const;

    /** get identifier and mapping resource of the root template (if any)
     ** @param  templateIdentifier  identifier of the root template (might be empty)
     *  @param  mappingResource     mapping resource that defines the root template
     *                              (might be empty)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getRootTemplateIdentification(OFString &templateIdentifier,
                                                      OFString &mappingResource) const;

    /** get the associated document type of the SR IOD
     ** @return document type (DSRTypes::DT_MacularGridThicknessAndVolumeReport)
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
