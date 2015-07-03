/*
 *
 *  Copyright (C) 2015, OFFIS e.V.
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
 *    classes: DSRTemplateCommon
 *
 */


#ifndef DSRCTPL_H
#define DSRCTPL_H

#include "dcmtk/config/osconfig.h"   /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/dsrtypes.h"


/*---------------------*
 *  class declaration  *
 *---------------------*/

/** Class managing common information on an SR template.
 *  This class is used as a base class only.
 */
class DCMTK_DCMSR_EXPORT DSRTemplateCommon
{

  public:

    /** check whether template identification is set
     ** @return OFTrue if set, OFFalse otherwise
     */
    virtual OFBool hasTemplateIdentification() const;

    /** check whether template identification is valid
     ** @param  check  check internally stored values for conformance with VR and VM
     *                 if enabled
     ** @return OFTrue if valid, OFFalse otherwise
     */
    virtual OFBool isTemplateIdentificationValid(const OFBool check = OFTrue) const;

    /** check whether template is extensible
     ** @return OFTrue if extensible, OFFalse otherwise
     */
    virtual OFBool isExtensible() const
    {
        return ExtensibleMode;
    }

    /** get template identifier
     ** @return identifier of the template
     */
    inline const OFString &getTemplateIdentifier() const
    {
        return TemplateIdentifier;
    }

    /** get mapping resource
     ** @return mapping resource that defines the template
     */
    inline const OFString &getMappingResource() const
    {
        return MappingResource;
    }

    /** get optional mapping resource UID
     ** @return unique identifier of the mapping resource (might be empty)
     */
    inline const OFString &getMappingResourceUID() const
    {
        return MappingResourceUID;
    }

    /** change mode specifying whether the template is extensible or non-extensible
     ** @param  mode  set template type to extensible if OFTrue (default)
     */
    virtual void setExtensible(const OFBool mode = OFTrue)
    {
        ExtensibleMode = mode;
    }


  protected:

    /** constructor
     ** @param  templateIdentifier  identifier of the template
     *  @param  mappingResource     mapping resource that defines the template
     *  @param  mappingResourceUID  uniquely identifies the mapping resource (optional)
     */
    DSRTemplateCommon(const OFString &templateIdentifier,
                      const OFString &mappingResource,
                      const OFString &mappingResourceUID = "");

    /** destructor
     */
    virtual ~DSRTemplateCommon();


  private:

    /// template identifier (VR=CS, mandatory)
    const OFString TemplateIdentifier;
    /// mapping resource (VR=CS, mandatory)
    const OFString MappingResource;
    /// mapping resource UID (VR=UI, optional)
    const OFString MappingResourceUID;
    /// mode indicating whether template is extensible (default: false)
    OFBool ExtensibleMode;

 // --- declaration of default constructor and assignment operator
 // --- (the assignment operator is defined implicitly)

    DSRTemplateCommon();
    DSRTemplateCommon &operator=(const DSRTemplateCommon &);
};


#endif
