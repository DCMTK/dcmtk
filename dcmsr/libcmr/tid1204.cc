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


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/cmr/tid1204.h"
#include "dcmtk/dcmsr/codes/dcm.h"


// helper macros for checking the return value of API calls
#define CHECK_RESULT(call) if (result.good()) result = call
#define STORE_RESULT(call) result = call


TID1204_LanguageOfContentItemAndDescendants::TID1204_LanguageOfContentItemAndDescendants()
  : DSRSubTemplate("1204", "DCMR", UID_DICOMContentMappingResource)
{
}


OFCondition TID1204_LanguageOfContentItemAndDescendants::setLanguage(const CID5000_Languages &language,
                                                                     const CID5001_Countries &country,
                                                                     const OFBool check)
{
    OFCondition result = EC_Normal;
    /* create a new subtree in order to "rollback" in case of error */
    DSRDocumentSubTree subTree;
    /* TID 1204 (Language of Content Item and Descendants) Row 1 */
    STORE_RESULT(subTree.addContentItem(RT_hasConceptMod, VT_Code, CODE_DCM_LanguageOfContentItemAndDescendants));
    CHECK_RESULT(subTree.getCurrentContentItem().setCodeValue(language.getSelectedValue(), check));
    /* TID 1204 (Language of Content Item and Descendants) Row 2 */
    if (country.hasSelectedValue())
    {
        CHECK_RESULT(subTree.addChildContentItem(RT_hasConceptMod, VT_Code, CODE_DCM_CountryOfLanguage));
        CHECK_RESULT(subTree.getCurrentContentItem().setCodeValue(country.getSelectedValue(), check));
    }
    /* if everything was OK, insert new subtree into the template */
    if (result.good())
    {
        /* replace currently stored subtree (if any) */
        swap(subTree);
    }
    return result;
}
