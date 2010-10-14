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
 *  Module: dcmsr
 *
 *  Author: Joerg Riesmeier
 *
 *  Purpose:
 *    classes: DSRIODConstraintChecker
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:16:32 $
 *  CVS/RCS Revision: $Revision: 1.4 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#ifndef DSRIODCC_H
#define DSRIODCC_H

#include "dcmtk/config/osconfig.h"   /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/dsrtypes.h"


/*---------------------*
 *  class declaration  *
 *---------------------*/

/** Class for checking the content relationship constraints of an SR IOD (abstract)
 */
class DSRIODConstraintChecker
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

    /** get identifier of the root template
     ** @return root template identifier (TID) if required, NULL otherwise
     */
    virtual const char *getRootTemplateIdentifier() const = 0;

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


/*
 *  CVS/RCS Log:
 *  $Log: dsriodcc.h,v $
 *  Revision 1.4  2010-10-14 13:16:32  joergr
 *  Updated copyright header. Added reference to COPYRIGHT file.
 *
 *  Revision 1.3  2005-12-08 16:05:07  meichel
 *  Changed include path schema for all DCMTK header files
 *
 *  Revision 1.2  2003/10/09 12:56:42  joergr
 *  Added check for root template identifier when reading an SR document.
 *
 *  Revision 1.1  2003/09/15 14:22:04  joergr
 *  Introduced new class to facilitate checking of SR IOD relationship content
 *  constraints. Replaced old implementation distributed over numerous classes.
 *
 */
