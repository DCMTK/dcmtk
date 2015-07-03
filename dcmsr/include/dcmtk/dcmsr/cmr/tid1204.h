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
 *    classes: TID1204_LanguageOfContentItemAndDescendants
 *
 */


#ifndef CMR_TID1204_H
#define CMR_TID1204_H

#include "dcmtk/config/osconfig.h"   /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/dsrstpl.h"

#include "dcmtk/dcmsr/cmr/define.h"
#include "dcmtk/dcmsr/cmr/cid5000.h"
#include "dcmtk/dcmsr/cmr/cid5001.h"


/*---------------------*
 *  class declaration  *
 *---------------------*/

/** Implementation of DCMR Template:
 *  TID 1204 - Language of Content Item and Descendants
 */
class DCMTK_CMR_EXPORT TID1204_LanguageOfContentItemAndDescendants
  : public DSRSubTemplate
{

  public:

    /** default constructor
     */
    TID1204_LanguageOfContentItemAndDescendants();

    /** set language, either by adding one content item (language only) or two content
     *  items (language and country).  Please note that there are also language variants
     *  that include the country.  See DICOM standard for more details.
     ** @param  language  language of the content, being a language that is primarily
     *                    used for human communication
     *  @param  country   country-specific variant of language (optional)
     *  @param  check     if enabled, check values for validity before setting them
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition setLanguage(const CID5000_Languages &language,
                            const CID5001_Countries &country = CID5001_Countries(),
                            const OFBool check = OFTrue);
};


#endif
