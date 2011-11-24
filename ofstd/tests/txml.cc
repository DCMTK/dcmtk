/*
 *
 *  Copyright (C) 2011, OFFIS e.V.
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
 *  Module:  ofstd
 *
 *  Author:  Joerg Riesmeier
 *
 *  Purpose: test program for xmlParser
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2011-11-24 09:53:43 $
 *  CVS/RCS Revision: $Revision: 1.1 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#define OFTEST_OFSTD_ONLY
#include "dcmtk/ofstd/oftest.h"
#include "dcmtk/ofstd/ofxml.h"


#define XML_TEST_DATA "<?xml version=\"1.0\"?><root><element attribute=\"value\">some text</element></root>"

OFTEST(ofstd_xmlParser)
{
    /* perform some very simple tests on the XML data */
    XMLNode rootNode = XMLNode::parseString(XML_TEST_DATA, "root");
    OFCHECK_EQUAL(OFString(OFSTRING_GUARD(rootNode.getChildNode("element").getAttribute("attribute"))), "value");
    OFCHECK_EQUAL(OFString(OFSTRING_GUARD(rootNode.getChildNode("element").getText())), "some text");
}


/*
 *
 * CVS/RCS Log:
 * $Log: txml.cc,v $
 * Revision 1.1  2011-11-24 09:53:43  joergr
 * Added first version of a simple non-validating XML parser written by Frank
 * Vanden Berghen. This parser is intended to be used for configuration files
 * and the like. Therefore, it is integrated into the DCMTK (unlike libxml2).
 *
 *
 */
