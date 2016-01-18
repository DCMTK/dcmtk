/*
 *
 *  Copyright (C) 2015-2016, J. Riesmeier, Oldenburg, Germany
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  Header file for class TID1204_LanguageOfContentItemAndDescendants
 *
 *  Author: Joerg Riesmeier
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
 *  TID 1204 - Language of Content Item and Descendants.
 *  All added content items are annotated with a text in the format "TID 1204 - Row [n]".
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


/*-------------------*
 *  type definition  *
 *-------------------*/

// define short name for the SR template class
typedef TID1204_LanguageOfContentItemAndDescendants CMR_TID1204;


#endif
