/*
 *
 *  Copyright (C) 2012-2015, OFFIS e.V.
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

OFTEST_REGISTER(dcmsr_addTreeNode_1);
OFTEST_REGISTER(dcmsr_addTreeNode_2);
OFTEST_REGISTER(dcmsr_addTreeNode_3);
OFTEST_REGISTER(dcmsr_getPosition);
OFTEST_REGISTER(dcmsr_countChildNodes);
OFTEST_REGISTER(dcmsr_treeWithoutRoot);
OFTEST_REGISTER(dcmsr_copyTree);
OFTEST_REGISTER(dcmsr_assignTree);
OFTEST_REGISTER(dcmsr_cloneSubTree_1);
OFTEST_REGISTER(dcmsr_cloneSubTree_2);
OFTEST_REGISTER(dcmsr_extractSubTree);
OFTEST_REGISTER(dcmsr_addContentItem);
OFTEST_REGISTER(dcmsr_copyContentItem);
OFTEST_REGISTER(dcmsr_gotoNamedNode);
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
OFTEST_REGISTER(dcmsr_changeDocumentType_1);
OFTEST_REGISTER(dcmsr_changeDocumentType_2);
OFTEST_REGISTER(dcmsr_setDocumentTree);
OFTEST_REGISTER(dcmsr_rootTemplate);
OFTEST_REGISTER(dcmsr_subTemplate_1);
OFTEST_REGISTER(dcmsr_subTemplate_2);
OFTEST_REGISTER(dcmsr_setCodeValueType);
OFTEST_REGISTER(dcmsr_determineCodeValueType);
OFTEST_REGISTER(dcmsr_writeCodeSequence);
OFTEST_REGISTER(dcmsr_useBasicCodedEntry);
OFTEST_REGISTER(dcmsr_CID42_NumericValueQualifier);
OFTEST_REGISTER(dcmsr_CID7445_DeviceParticipatingRoles);
OFTEST_REGISTER(dcmsr_TID1001_ObservationContext);
OFTEST_REGISTER(dcmsr_TID1204_LanguageOfContentItemAndDescendants);
OFTEST_MAIN("dcmsr")
