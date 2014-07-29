/*
 *
 *  Copyright (C) 2012-2014, OFFIS e.V.
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

OFTEST_REGISTER(dcmsr_addTreeNode);
OFTEST_REGISTER(dcmsr_treeWithoutRoot);
OFTEST_REGISTER(dcmsr_copyTree);
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
OFTEST_REGISTER(dcmsr_changeDocumentType_1);
OFTEST_REGISTER(dcmsr_changeDocumentType_2);
OFTEST_MAIN("dcmsr")
