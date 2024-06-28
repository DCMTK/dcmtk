/*
 *
 *  Copyright (C) 2015-2024, OFFIS e.V.
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
 *    classes: DSRTreeNodeAnnotation
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/dsrtnant.h"


DSRTreeNodeAnnotation::DSRTreeNodeAnnotation(const OFString &text)
  : Text(text)
{
}


DSRTreeNodeAnnotation::~DSRTreeNodeAnnotation()
{
}


// comparison operators

OFBool operator==(const DSRTreeNodeAnnotation &lhs,
                  const DSRTreeNodeAnnotation &rhs)
{
    return lhs.isEqual(rhs);
}


OFBool operator!=(const DSRTreeNodeAnnotation &lhs,
                  const DSRTreeNodeAnnotation &rhs)
{
    return lhs.isNotEqual(rhs);
}
