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
 *  Purpose: test program for "convert to markup"-code in OFStandard
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2011-10-12 12:03:04 $
 *  CVS/RCS Revision: $Revision: 1.1 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#define OFTEST_OFSTD_ONLY
#include "dcmtk/ofstd/oftest.h"
#include "dcmtk/ofstd/ofstd.h"


OFTEST(ofstd_markup_1)
{
    OFString resultStr;
    // XML: source source string contains both LF and CR
    const OFString sourceStr = "This is a test, with <special> characters & \"some\" other 'dirty' tricks!\nJörg\r";
    const OFString markupStr1 = "This is a test, with &lt;special&gt; characters &amp; &quot;some&quot; other &apos;dirty&apos; tricks!&#10;J&#246;rg&#13;";
    const OFString markupStr2 = "This is a test, with &lt;special&gt; characters &amp; &quot;some&quot; other &apos;dirty&apos; tricks!&#10;Jörg&#13;";

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
    const OFString sourceStr = "This is a test, with <special> characters & \"some\" other 'dirty' tricks!\r\nJörg";
    const OFString markupStr1 = "This is a test, with &lt;special&gt; characters &amp; &quot;some&quot; other &apos;dirty&apos; tricks!&#13;&#10;J&#246;rg";
    const OFString markupStr2 = "This is a test, with &lt;special&gt; characters &amp; &quot;some&quot; other &apos;dirty&apos; tricks!&#13;&#10;Jörg";

    OFStandard::convertToMarkupString(sourceStr, resultStr, OFTrue /* convertNonASCII */, OFStandard::MM_XML);
    OFCHECK_EQUAL(resultStr, markupStr1);
    OFStandard::convertToMarkupString(sourceStr, resultStr, OFFalse /* convertNonASCII */, OFStandard::MM_XML);
    OFCHECK_EQUAL(resultStr, markupStr2);
}

OFTEST(ofstd_markup_3)
{
    OFString resultStr;
    // HTML: source source string contains LF+CR
    const OFString sourceStr = "This is a test, with <special> characters & \"some\" other 'dirty' tricks!\n\rJörg";
    const OFString markupStr1 = "This is a test, with &lt;special&gt; characters &amp; &quot;some&quot; other &#39;dirty&#39; tricks!<br>\nJ&#246;rg";
    const OFString markupStr2 = "This is a test, with &lt;special&gt; characters &amp; &quot;some&quot; other &#39;dirty&#39; tricks!&para;Jörg";

    OFStandard::convertToMarkupString(sourceStr, resultStr, OFTrue /* convertNonASCII */, OFStandard::MM_HTML, OFTrue /* newlineAllowed */);
    OFCHECK_EQUAL(resultStr, markupStr1);
    OFStandard::convertToMarkupString(sourceStr, resultStr, OFFalse /* convertNonASCII */, OFStandard::MM_HTML, OFFalse /* newlineAllowed */);
    OFCHECK_EQUAL(resultStr, markupStr2);
}

OFTEST(ofstd_markup_4)
{
    OFString resultStr;
    // XHTML: source source string contains CR+LF
    const OFString sourceStr = "This is a test, with <special> characters & \"some\" other 'dirty' tricks!\r\nJörg";
    const OFString markupStr1 = "This is a test, with &lt;special&gt; characters &amp; &quot;some&quot; other &apos;dirty&apos; tricks!<br />\nJ&#246;rg";
    const OFString markupStr2 = "This is a test, with &lt;special&gt; characters &amp; &quot;some&quot; other &apos;dirty&apos; tricks!&para;Jörg";

    OFStandard::convertToMarkupString(sourceStr, resultStr, OFTrue /* convertNonASCII */, OFStandard::MM_XHTML, OFTrue /* newlineAllowed */);
    OFCHECK_EQUAL(resultStr, markupStr1);
    OFStandard::convertToMarkupString(sourceStr, resultStr, OFFalse /* convertNonASCII */, OFStandard::MM_XHTML, OFFalse /* newlineAllowed */);
    OFCHECK_EQUAL(resultStr, markupStr2);
}

OFTEST(ofstd_markup_5)
{
    OFString resultStr;
    const size_t sourceLen = 79;
    // HTML 3.2: the source string contains a NULL byte!
    const OFString sourceStr(("This is a test\0, with <special> characters & \"some\" other 'dirty' tricks!\rJörg\n"), sourceLen);
    const OFString markupStr1 = "This is a test&#0;, with &lt;special&gt; characters &amp; &#34;some&#34; other &#39;dirty&#39; tricks!<br>\nJ&#246;rg<br>\n";
    const OFString markupStr2 = "This is a test&#0;, with &lt;special&gt; characters &amp; &#34;some&#34; other &#39;dirty&#39; tricks!&para;J&#246;rg&para;";
    const OFString markupStr3 = "This is a test\0, with &lt;special&gt; characters &amp; &#34;some&#34; other &#39;dirty&#39; tricks!&para;J&#246;rg&para;";

    OFCHECK_EQUAL(sourceStr.length(), sourceLen);
    OFCHECK(OFStandard::checkForMarkupConversion(sourceStr, OFTrue /* convertNonASCII */));

    OFStandard::convertToMarkupString(sourceStr, resultStr, OFTrue /* convertNonASCII */, OFStandard::MM_HTML32, OFTrue /* newlineAllowed */);
    OFCHECK_EQUAL(resultStr, markupStr1);
    OFStandard::convertToMarkupString(sourceStr, resultStr, OFFalse /* convertNonASCII */, OFStandard::MM_HTML32, OFFalse /* newlineAllowed */);
    OFCHECK_EQUAL(resultStr, markupStr2);
    OFStandard::convertToMarkupString(sourceStr, resultStr, OFFalse /* convertNonASCII */, OFStandard::MM_XHTML, OFFalse /* newlineAllowed */);
    OFCHECK_EQUAL(resultStr, "This is a test" /* string is truncated at the NULL byte */);
}


/*
 *
 * CVS/RCS Log:
 * $Log: tmarkup.cc,v $
 * Revision 1.1  2011-10-12 12:03:04  joergr
 * Added tests for convertToMarkupString() method.
 *
 *
 *
 */
