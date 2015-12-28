/*
 *
 *  Copyright (C) 2003-2015, OFFIS e.V.
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
 *    classes: DSRIODConstraintChecker
 *
 */


#ifndef DSRIODCC_H
#define DSRIODCC_H

#include "dcmtk/config/osconfig.h"   /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/dsrtypes.h"


/*---------------------*
 *  class declaration  *
 *---------------------*/

/** Class for checking the relationship content constraints of an SR IOD (abstract)
 */
class DCMTK_DCMSR_EXPORT DSRIODConstraintChecker
  : public DSRTypes
{

  public:

    /** default constructor
     */
    DSRIODConstraintChecker();

    /** destructor
     */
    virtual ~DSRIODConstraintChecker();

    /** check whether by-reference relationships are allowed for this SR IOD (abstract)
     ** @return OFTrue if by-reference relationships are allowed, OFFalse otherwise
     */
    virtual OFBool isByReferenceAllowed() const = 0;

    /** check whether this SR IOD requires template support (abstract)
     ** @return OFTrue if template support is required, OFFalse otherwise
     */
    virtual OFBool isTemplateSupportRequired() const = 0;

    /** get identifier and mapping resource of the root template (if any)
     ** @param  templateIdentifier  identifier of the root template (might be empty)
     *  @param  mappingResource     mapping resource that defines the root template
     *                              (might be empty)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getRootTemplateIdentification(OFString &templateIdentifier,
                                                      OFString &mappingResource) const = 0;

    /** get the associated document type of the SR IOD (abstract)
     ** @return document type (see DSRTypes::E_DocumentType)
     */
    virtual E_DocumentType getDocumentType() const = 0;

    /** check whether specified content relationship is allowed for this IOD (abstract)
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
                                            const OFBool byReference = OFFalse) const = 0;
};


#endif
