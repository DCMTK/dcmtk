/*
 *
 *  Copyright (C) 2011-2013, OFFIS e.V.
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
 *  Purpose: test program for "convert to markup"-code in OFStandard
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#define OFTEST_OFSTD_ONLY
#include "dcmtk/ofstd/oftest.h"
#include "dcmtk/ofstd/ofstd.h"


/*
 *  Note: \366 is the a "o umlaut" encoded in ISO 8859-1 (Latin-1)
 */

OFTEST(ofstd_markup_1)
{
    OFString resultStr;
    // XML: source source string contains both LF and CR
    const OFString sourceStr = "This is a test, with <special> characters & \"some\" other 'dirty' tricks!\nJ\366rg\r";
    const OFString markupStr1 = "This is a test, with &lt;special&gt; characters &amp; &quot;some&quot; other &apos;dirty&apos; tricks!&#10;J&#246;rg&#13;";
    const OFString markupStr2 = "This is a test, with &lt;special&gt; characters &amp; &quot;some&quot; other &apos;dirty&apos; tricks!&#10;J\366rg&#13;";

    OFCHECK(OFStandard::checkForMarkupConversion(sourceStr, OFTrue /* convertNonASCII */));
    OFCHECK(!OFStandard::checkForMarkupConversion(sourceStr, OFTrue /* convertNonASCII */, 20 /* maxLength */));

    OFStandard::convertToMarkupString(sourceStr, resultStr, OFTrue /* convertNonASCII */, OFStandard::MM_XML);
    OFCHECK_EQUAL(resultStr, markupStr1);
    OFStandard::convertToMarkupString(sourceStr, resultStr, OFFalse /* convertNonASCII */, OFStandard::MM_XML);
    OFCHECK_EQUAL(resultStr, markupStr2);
}

OFTEST(ofstd_markup_2)
{
    OFString resultStr;
    // XML: source source string contains CR+LF
    const OFString sourceStr = "This is a test, with <special> characters & \"some\" other 'dirty' tricks!\r\nJ\366rg";
    const OFString markupStr1 = "This is a test, with &lt;special&gt; characters &amp; &quot;some&quot; other &apos;dirty&apos; tricks!&#13;&#10;J&#246;rg";
    const OFString markupStr2 = "This is a test, with &lt;special&gt; characters &amp; &quot;some&quot; other &apos;dirty&apos; tricks!&#13;&#10;J\366rg";

    OFStandard::convertToMarkupString(sourceStr, resultStr, OFTrue /* convertNonASCII */, OFStandard::MM_XML);
    OFCHECK_EQUAL(resultStr, markupStr1);
    OFStandard::convertToMarkupString(sourceStr, resultStr, OFFalse /* convertNonASCII */, OFStandard::MM_XML);
    OFCHECK_EQUAL(resultStr, markupStr2);
}

OFTEST(ofstd_markup_3)
{
    OFString resultStr;
    // HTML: source source string contains LF+CR
    const OFString sourceStr = "This is a test, with <special> characters & \"some\" other 'dirty' tricks!\n\rJ\366rg";
    const OFString markupStr1 = "This is a test, with &lt;special&gt; characters &amp; &quot;some&quot; other &#39;dirty&#39; tricks!<br>\nJ&#246;rg";
    const OFString markupStr2 = "This is a test, with &lt;special&gt; characters &amp; &quot;some&quot; other &#39;dirty&#39; tricks!&para;J\366rg";

    OFStandard::convertToMarkupString(sourceStr, resultStr, OFTrue /* convertNonASCII */, OFStandard::MM_HTML, OFTrue /* newlineAllowed */);
    OFCHECK_EQUAL(resultStr, markupStr1);
    OFStandard::convertToMarkupString(sourceStr, resultStr, OFFalse /* convertNonASCII */, OFStandard::MM_HTML, OFFalse /* newlineAllowed */);
    OFCHECK_EQUAL(resultStr, markupStr2);
}

OFTEST(ofstd_markup_4)
{
    OFString resultStr;
    // XHTML: source source string contains CR+LF
    const OFString sourceStr = "This is a test, with <special> characters & \"some\" other 'dirty' tricks!\r\nJ\366rg";
    const OFString markupStr1 = "This is a test, with &lt;special&gt; characters &amp; &quot;some&quot; other &apos;dirty&apos; tricks!<br />\nJ&#246;rg";
    const OFString markupStr2 = "This is a test, with &lt;special&gt; characters &amp; &quot;some&quot; other &apos;dirty&apos; tricks!&para;J\366rg";

    OFStandard::convertToMarkupString(sourceStr, resultStr, OFTrue /* convertNonASCII */, OFStandard::MM_XHTML, OFTrue /* newlineAllowed */);
    OFCHECK_EQUAL(resultStr, markupStr1);
    OFStandard::convertToMarkupString(sourceStr, resultStr, OFFalse /* convertNonASCII */, OFStandard::MM_XHTML, OFFalse /* newlineAllowed */);
    OFCHECK_EQUAL(resultStr, markupStr2);
}

OFTEST(ofstd_markup_5)
{
    OFString resultStr;
    const size_t sourceLen = 79;
    // HTML 3.2: source string contains a NULL byte!
    const OFString sourceStr(("This is a test\0, with <special> characters & \"some\" other 'dirty' tricks!\rJ\366rg\n"), sourceLen);
    const OFString markupStr1 = "This is a test&#0;, with &lt;special&gt; characters &amp; &#34;some&#34; other &#39;dirty&#39; tricks!<br>\nJ&#246;rg<br>\n";
    const OFString markupStr2 = "This is a test&#0;, with &lt;special&gt; characters &amp; &#34;some&#34; other &#39;dirty&#39; tricks!&para;J&#246;rg&para;";
    const OFString markupStr3 = "This is a test, with &lt;special&gt; characters &amp; &quot;some&quot; other &apos;dirty&apos; tricks!&para;J\366rg&para;";

    OFCHECK_EQUAL(sourceStr.length(), sourceLen);
    OFCHECK(OFStandard::checkForMarkupConversion(sourceStr, OFTrue /* convertNonASCII */));

    OFStandard::convertToMarkupString(sourceStr, resultStr, OFTrue /* convertNonASCII */, OFStandard::MM_HTML32, OFTrue /* newlineAllowed */);
    OFCHECK_EQUAL(resultStr, markupStr1);
    OFStandard::convertToMarkupString(sourceStr, resultStr, OFFalse /* convertNonASCII */, OFStandard::MM_HTML32, OFFalse /* newlineAllowed */);
    OFCHECK_EQUAL(resultStr, markupStr2);
    OFStandard::convertToMarkupString(sourceStr, resultStr, OFFalse /* convertNonASCII */, OFStandard::MM_XHTML, OFFalse /* newlineAllowed */);
    OFCHECK_EQUAL(resultStr, markupStr3);
}

OFTEST(ofstd_markup_6)
{
    OFString resultStr;
    const size_t sourceLen = 34;
    // XML: source string contains a NULL byte!
    const OFString sourceStr(("This is a test\0, with a NULL byte."), sourceLen);
    const OFString markupStr = "This is a test, with a NULL byte.";

    OFCHECK_EQUAL(sourceStr.length(), sourceLen);
    OFCHECK(OFStandard::checkForMarkupConversion(sourceStr));

    OFStandard::convertToMarkupString(sourceStr, resultStr);
    OFCHECK_EQUAL(resultStr, markupStr);
}
