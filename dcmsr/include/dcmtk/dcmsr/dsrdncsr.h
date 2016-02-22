/*
 *
 *  Copyright (C) 2016, J. Riesmeier, Oldenburg, Germany
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  This software and supporting documentation are maintained by
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
 *    classes: DSRDocumentTreeNodeCursor, DSRIncludedTemplateNodeCursor
 *
 */


#ifndef DSRDOCSR_H
#define DSRDOCSR_H

#include "dcmtk/config/osconfig.h"   /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/dsdefine.h"
#include "dcmtk/dcmsr/dsrtncsr.h"


/*-----------------------*
 *  forward declaration  *
 *-----------------------*/

class DSRDocumentTreeNode;


/*--------------------*
 *  type definitions  *
 *--------------------*/

typedef DSRTreeNodeCursor<DSRDocumentTreeNode, OFFalse> DSRDocumentTreeNodeCursor;
typedef DSRTreeNodeCursor<DSRDocumentTreeNode, OFTrue> DSRIncludedTemplateNodeCursor;


/*-------------------------------------------*
 *  declaration of template specializations  *
 *-------------------------------------------*/

DCMTK_EXPLICIT_SPECIALIZATION DCMTK_DCMSR_EXPORT
const DSRDocumentTreeNode *DSRTreeNodeCursor<DSRDocumentTreeNode, OFTrue>::getChildNode() const;

DCMTK_EXPLICIT_SPECIALIZATION DCMTK_DCMSR_EXPORT
size_t DSRTreeNodeCursor<DSRDocumentTreeNode, OFTrue>::countChildNodes(const OFBool searchIntoSub) const;

DCMTK_EXPLICIT_SPECIALIZATION DCMTK_DCMSR_EXPORT
size_t DSRTreeNodeCursor<DSRDocumentTreeNode, OFTrue>::goDown();

DCMTK_EXPLICIT_SPECIALIZATION DCMTK_DCMSR_EXPORT
size_t DSRTreeNodeCursor<DSRDocumentTreeNode, OFTrue>::iterate(const OFBool searchIntoSub);


#endif
