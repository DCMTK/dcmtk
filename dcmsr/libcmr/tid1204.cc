/*
 *
 *  Copyright (C) 2015-2022, J. Riesmeier, Oldenburg, Germany
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  Source file for class TID1204_LanguageOfContentItemAndDescendants
 *
 *  Author: Joerg Riesmeier
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/cmr/tid1204.h"
#include "dcmtk/dcmsr/codes/dcm.h"

#include "dcmtk/dcmdata/dcuid.h"


// helper macros for checking the return value of API calls
#define CHECK_RESULT(call) if (result.good()) result = call
#define STORE_RESULT(call) result = call
#define GOOD_RESULT(call) if (result.good()) call
#define BAD_RESULT(call) if (result.bad()) call

// general information on TID 1204 (Language of Content Item and Descendants)
#define TEMPLATE_NUMBER      "1204"
#define MAPPING_RESOURCE     "DCMR"
#define MAPPING_RESOURCE_UID UID_DICOMContentMappingResource
#define TEMPLATE_TYPE        OFFalse  /* non-extensible */
#define TEMPLATE_ORDER       OFTrue   /* significant */


TID1204_LanguageOfContentItemAndDescendants::TID1204_LanguageOfContentItemAndDescendants()
  : DSRSubTemplate(TEMPLATE_NUMBER, MAPPING_RESOURCE, MAPPING_RESOURCE_UID)
{
    setExtensible(TEMPLATE_TYPE);
    setOrderSignificant(TEMPLATE_ORDER);
}


OFCondition TID1204_LanguageOfContentItemAndDescendants::setLanguage(const CID5000_Language &language,
                                                                     const CID5001_Country &country,
                                                                     const OFBool check)
{
    OFCondition result = EC_Normal;
    /* create a new subtree in order to "rollback" in case of error */
    DSRDocumentSubTree subTree;
    /* TID 1204 (Language of Content Item and Descendants) Row 1 */
    STORE_RESULT(subTree.addContentItem(RT_hasConceptMod, VT_Code, CODE_DCM_LanguageOfContentItemAndDescendants, check));
    CHECK_RESULT(subTree.getCurrentContentItem().setCodeValue(language, check));
    CHECK_RESULT(subTree.getCurrentContentItem().setAnnotationText("TID 1204 - Row 1"));
    /* TID 1204 (Language of Content Item and Descendants) Row 2 */
    if (country.hasSelectedValue())
    {
        CHECK_RESULT(subTree.addChildContentItem(RT_hasConceptMod, VT_Code, CODE_DCM_CountryOfLanguage, check));
        CHECK_RESULT(subTree.getCurrentContentItem().setCodeValue(country, check));
        CHECK_RESULT(subTree.getCurrentContentItem().setAnnotationText("TID 1204 - Row 2"));
    }
    /* if everything was OK, replace current subtree of the template */
    GOOD_RESULT(swap(subTree));
    return result;
}
