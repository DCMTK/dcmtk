/*
 *
 *  Copyright (C) 2018, J. Riesmeier, Oldenburg, Germany
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  Source file for class TID4019_AlgorithmIdentification
 *
 *  Author: Joerg Riesmeier
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/cmr/tid4019.h"
#include "dcmtk/dcmsr/codes/dcm.h"

#include "dcmtk/dcmdata/dcuid.h"


// helper macros for checking the return value of API calls
#define CHECK_RESULT(call) if (result.good()) result = call
#define STORE_RESULT(call) result = call
#define GOOD_RESULT(call) if (result.good()) call
#define BAD_RESULT(call) if (result.bad()) call

// general information on TID 4019 (Algorithm Identification)
#define TEMPLATE_NUMBER      "4019"
#define MAPPING_RESOURCE     "DCMR"
#define MAPPING_RESOURCE_UID UID_DICOMContentMappingResource
#define TEMPLATE_TYPE        OFFalse  /* non-extensible */
#define TEMPLATE_ORDER       OFTrue   /* significant */


TID4019_AlgorithmIdentification::TID4019_AlgorithmIdentification()
  : DSRSubTemplate(TEMPLATE_NUMBER, MAPPING_RESOURCE, MAPPING_RESOURCE_UID)
{
    setExtensible(TEMPLATE_TYPE);
    setOrderSignificant(TEMPLATE_ORDER);
}


OFCondition TID4019_AlgorithmIdentification::setIdentification(const OFString &algorithmName,
                                                               const OFString &algorithmVersion,
                                                               const OFBool check)
{
    OFCondition result = EC_Normal;
    /* create a new subtree in order to "rollback" in case of error */
    DSRDocumentSubTree subTree;
    /* TID 4019 (Algorithm Identification) Row 1 */
    STORE_RESULT(subTree.addContentItem(RT_unknown, VT_Text, CODE_DCM_AlgorithmName, check));
    CHECK_RESULT(subTree.getCurrentContentItem().setStringValue(algorithmName, check));
    CHECK_RESULT(subTree.getCurrentContentItem().setAnnotationText("TID 4019 - Row 1"));
    /* TID 4019 (Algorithm Identification) Row 2 */
    CHECK_RESULT(subTree.addContentItem(RT_unknown, VT_Text, CODE_DCM_AlgorithmVersion, check));
    CHECK_RESULT(subTree.getCurrentContentItem().setStringValue(algorithmVersion, check));
    CHECK_RESULT(subTree.getCurrentContentItem().setAnnotationText("TID 4019 - Row 2"));
    /* if everything was OK, replace current subtree of the template */
    GOOD_RESULT(swap(subTree));
    return result;
}


OFCondition TID4019_AlgorithmIdentification::addParameter(const OFString &algorithmParameter,
                                                          const OFBool check)
{
    OFCondition result = SR_EC_InvalidTemplateStructure;
    /* check whether mandatory content items exist, i.e. the tree is not empty */
    if (!isEmpty())
    {
        /* TID 4019 (Algorithm Identification) Row 3 */
        STORE_RESULT(addContentItem(RT_unknown, VT_Text, CODE_DCM_AlgorithmParameters, check));
        CHECK_RESULT(getCurrentContentItem().setStringValue(algorithmParameter, check));
        CHECK_RESULT(getCurrentContentItem().setAnnotationText("TID 4019 - Row 3"));
    }
    return result;
}
