/*
 *
 *  Copyright (C) 2011-2012, OFFIS e.V.
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
 *  Update Date:      $Date: 2012-06-29 13:05:37 $
 *  CVS/RCS Revision: $Revision: 1.2 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#define OFTEST_OFSTD_ONLY
#include "dcmtk/ofstd/oftest.h"
#include "dcmtk/ofstd/ofxml.h"


#define XML_TEST_DATA "<?xml version=\"1.0\"?>"                         \
    "<root>"                                                            \
      "<element attribute=\"value\">some text</element>"                \
      "<element attribute=\"other value\">some other text</element>"    \
      "<error>error message</error>"                                    \
      "<element>some totally different text</element>"                  \
      "<parent>"                                                        \
        "<child>no text</child>"                                        \
        "<child />"                                                     \
      "</parent>"                                                       \
    "</root>"


OFTEST(ofstd_xmlParser)
{
    int i = 0;

    /* perform some very simple tests on the XML data */
    XMLNode rootNode = XMLNode::parseString(XML_TEST_DATA, "root");

    OFCHECK_EQUAL(OFString(OFSTRING_GUARD(rootNode.getChildNode("element").getAttribute("attribute"))), "value");
    OFCHECK_EQUAL(OFString(OFSTRING_GUARD(rootNode.getChildNode("element").getText())), "some text");
    OFCHECK_EQUAL(OFString(OFSTRING_GUARD(rootNode.getChildNode("element", 1).getText())), "some other text");
    OFCHECK_EQUAL(OFString(OFSTRING_GUARD(rootNode.getChildNode("element", 2).getText())), "some totally different text");
    OFCHECK_EQUAL(OFString(OFSTRING_GUARD(rootNode.getChildNode("element", -1).getText())), "some totally different text");

    OFCHECK_EQUAL(OFString(OFSTRING_GUARD(rootNode.getChildNodeWithAttribute("element", "attribute").getText())), "some text");
    OFCHECK_EQUAL(OFString(OFSTRING_GUARD(rootNode.getChildNodeWithAttribute("element", "attribute", "other value").getText())), "some other text");
    rootNode.getChildNodeWithAttribute("element", "attribute", NULL, &i);
    OFCHECK_EQUAL(i, 1);
    rootNode.getChildNodeWithAttribute("element", "attribute", "other value", &i);
    OFCHECK_EQUAL(i, 2);

    OFCHECK_EQUAL(OFString(OFSTRING_GUARD(rootNode.getChildNodeByPath("element").getText())), "some text");
    OFCHECK_EQUAL(OFString(OFSTRING_GUARD(rootNode.getChildNodeByPath("parent/child").getText())), "no text");

    OFCHECK_EQUAL(rootNode.nChildNode(), 5);
    OFCHECK_EQUAL(rootNode.nChildNode("element"), 3);
    OFCHECK_EQUAL(rootNode.nChildNode("parent"), 1);
    OFCHECK_EQUAL(rootNode.nChildNode("child"), 0);
    OFCHECK_EQUAL(rootNode.getChildNode("parent").nChildNode(), 2);

    OFCHECK_EQUAL(OFString(OFSTRING_GUARD(rootNode.getChildNode("element").getAttributeName(0))), "attribute");
    OFCHECK_EQUAL(OFString(OFSTRING_GUARD(rootNode.getChildNode("element").getAttributeValue(0))), "value");
    OFCHECK_EQUAL(rootNode.getChildNode("element").nAttribute(), 1);
    OFCHECK_EQUAL(rootNode.getChildNode("element", 1).nAttribute(), 1);
    OFCHECK_EQUAL(rootNode.getChildNode("element", 2).nAttribute(), 0);
    OFCHECK_EQUAL(rootNode.getChildNode("error").nAttribute(), 0);
    OFCHECK_EQUAL(rootNode.getChildNode("parent").nAttribute(), 0);

    OFCHECK(!rootNode.getChildNode("parent").getChildNode("child", 1).isEmpty());
    OFCHECK(rootNode.getChildNode("parent").getChildNode("child" ,2).isEmpty());
}


/*
 *
 * CVS/RCS Log:
 * $Log: txml.cc,v $
 * Revision 1.2  2012-06-29 13:05:37  joergr
 * Added some more simple tests for the integrated XML parser.
 *
 * Revision 1.1  2011-11-24 09:53:43  joergr
 * Added first version of a simple non-validating XML parser written by Frank
 * Vanden Berghen. This parser is intended to be used for configuration files
 * and the like. Therefore, it is integrated into the DCMTK (unlike libxml2).
 *
 *
 */
