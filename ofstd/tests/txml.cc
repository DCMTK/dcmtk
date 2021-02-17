/*
 *
 *  Copyright (C) 2011-2020, OFFIS e.V.
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

// we cannot run this test if the XML parser is configured for wide characters
// because the OFTest classes and macros do not support wide strings.
#ifndef _XMLWIDECHAR

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
    OFCHECK(rootNode.getChildNode("element").isAttributeSet("attribute"));
}

#else

int ofstd_txml_cc_dummy_to_keep_linker_from_moaning = 0;

#endif