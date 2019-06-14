/*
 *
 *  Copyright (C) 2012-2019, OFFIS e.V.
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
 *  Purpose: main test program
 *
 */

#include "dcmtk/config/osconfig.h"

#include "dcmtk/ofstd/oftest.h"

OFTEST_REGISTER(dcmsr_addItems);
OFTEST_REGISTER(dcmsr_getItems);
OFTEST_REGISTER(dcmsr_addTreeNode_1);
OFTEST_REGISTER(dcmsr_addTreeNode_2);
OFTEST_REGISTER(dcmsr_addTreeNode_3);
OFTEST_REGISTER(dcmsr_addTreeNode_4);
OFTEST_REGISTER(dcmsr_addTreeNode_5);
OFTEST_REGISTER(dcmsr_addTreeNode_6);
OFTEST_REGISTER(dcmsr_replaceTreeNode);
OFTEST_REGISTER(dcmsr_getPosition);
OFTEST_REGISTER(dcmsr_countChildNodes);
OFTEST_REGISTER(dcmsr_treeWithoutRoot);
OFTEST_REGISTER(dcmsr_copyTree);
OFTEST_REGISTER(dcmsr_assignTree);
OFTEST_REGISTER(dcmsr_cloneSubTree_1);
OFTEST_REGISTER(dcmsr_cloneSubTree_2);
OFTEST_REGISTER(dcmsr_extractSubTree);
OFTEST_REGISTER(dcmsr_gotoParentUntilRoot);
OFTEST_REGISTER(dcmsr_gotoAnnotatedTreeNode);
OFTEST_REGISTER(dcmsr_addContentItem_1);
OFTEST_REGISTER(dcmsr_addContentItem_2);
OFTEST_REGISTER(dcmsr_addContentItem_3);
OFTEST_REGISTER(dcmsr_copyContentItem);
OFTEST_REGISTER(dcmsr_getCurrentNode);
OFTEST_REGISTER(dcmsr_compareNodes);
OFTEST_REGISTER(dcmsr_gotoNodeByValue);
OFTEST_REGISTER(dcmsr_gotoMatchingNode);
OFTEST_REGISTER(dcmsr_gotoNamedNode);
OFTEST_REGISTER(dcmsr_gotoNamedChildNode);
OFTEST_REGISTER(dcmsr_gotoNamedNodeInSubTree);
OFTEST_REGISTER(dcmsr_gotoAnnotatedNode);
OFTEST_REGISTER(dcmsr_createDocSubTree);
OFTEST_REGISTER(dcmsr_copyDocSubTree);
OFTEST_REGISTER(dcmsr_cloneDocSubTree);
OFTEST_REGISTER(dcmsr_insertDocSubTree_1);
OFTEST_REGISTER(dcmsr_insertDocSubTree_2);
OFTEST_REGISTER(dcmsr_insertDocSubTree_3);
OFTEST_REGISTER(dcmsr_removeDocSubTree);
OFTEST_REGISTER(dcmsr_extractDocSubTree_1);
OFTEST_REGISTER(dcmsr_extractDocSubTree_2);
OFTEST_REGISTER(dcmsr_extractAndInsertDocSubTree_1);
OFTEST_REGISTER(dcmsr_extractAndInsertDocSubTree_2);
OFTEST_REGISTER(dcmsr_setSpecificCharacterSet);
OFTEST_REGISTER(dcmsr_changeDocumentType_1);
OFTEST_REGISTER(dcmsr_changeDocumentType_2);
OFTEST_REGISTER(dcmsr_setDocumentTree);
OFTEST_REGISTER(dcmsr_rootTemplate);
OFTEST_REGISTER(dcmsr_subTemplate_1);
OFTEST_REGISTER(dcmsr_subTemplate_2);
OFTEST_REGISTER(dcmsr_subTemplate_3);
OFTEST_REGISTER(dcmsr_createExpandedTree);
OFTEST_REGISTER(dcmsr_templateWithByReferenceRelationship_1);
OFTEST_REGISTER(dcmsr_templateWithByReferenceRelationship_2);
OFTEST_REGISTER(dcmsr_validCompleteOrEmptyCode);
OFTEST_REGISTER(dcmsr_setCodeValueType);
OFTEST_REGISTER(dcmsr_determineCodeValueType);
OFTEST_REGISTER(dcmsr_writeCodeSequence);
OFTEST_REGISTER(dcmsr_compareCodedEntry);
OFTEST_REGISTER(dcmsr_useBasicCodedEntry);
OFTEST_REGISTER(dcmsr_setNumericMeasurementValue);
OFTEST_REGISTER(dcmsr_CID29e_AcquisitionModality);
OFTEST_REGISTER(dcmsr_CID42_NumericValueQualifier);
OFTEST_REGISTER(dcmsr_CID244e_Laterality);
OFTEST_REGISTER(dcmsr_CID4031e_CommonAnatomicRegions);
OFTEST_REGISTER(dcmsr_CID7445_DeviceParticipatingRoles);
OFTEST_REGISTER(dcmsr_CID10013e_CTAcquisitionType);
OFTEST_REGISTER(dcmsr_CID10033e_CTReconstructionAlgorithm);
OFTEST_REGISTER(dcmsr_TID1001_ObservationContext);
OFTEST_REGISTER(dcmsr_TID1204_LanguageOfContentItemAndDescendants);
OFTEST_REGISTER(dcmsr_TID1411_VolumetricROIMeasurements);
OFTEST_REGISTER(dcmsr_TID1500_MeasurementReport);
OFTEST_REGISTER(dcmsr_TID1501_MeasurementGroup);
OFTEST_REGISTER(dcmsr_TID1600_ImageLibrary);
OFTEST_REGISTER(dcmsr_CMR_SRNumericMeasurementValue);
OFTEST_REGISTER(dcmsr_CMR_SRNumericMeasurementValueWithUnits_baselineGroup);
OFTEST_REGISTER(dcmsr_CMR_SRNumericMeasurementValueWithUnits_definedGroup);
/* the following should be the last test case */
OFTEST_REGISTER(dcmsr_cleanupContentMappingResource);
OFTEST_MAIN("dcmsr")
