/*
 *
 *  Copyright (C) 2002-2011, OFFIS e.V.
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
 *  Purpose: test program for class OFStandard
 *
 *  Last Update:      $Author: uli $
 *  Update Date:      $Date: 2011-07-06 13:04:18 $
 *  CVS/RCS Revision: $Revision: 1.15 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/ofstd/ofstd.h"

#define OFTEST_OFSTD_ONLY
#include "dcmtk/ofstd/oftest.h"

static void checkExists(const OFString& input, OFBool pathExists,
        OFBool fileExists, OFBool dirExists)
{
    OFBool path = OFStandard::pathExists(input);
    OFBool file = OFStandard::fileExists(input);
    OFBool dir  = OFStandard::dirExists(input);

    if (pathExists != path)
        OFCHECK_FAIL("pathExists(\"" << input << "\"): expected: " << pathExists << ", got: " << path);
    if (fileExists != file)
        OFCHECK_FAIL("fileExists(\"" << input << "\"): expected: " << fileExists << ", got: " << file);
    if (dirExists != dir)
        OFCHECK_FAIL("dirExists(\"" << input << "\"): expected: " << dirExists << ", got: " << dir);
}

static void checkPathHandling(const OFString& input, const OFString& normalized,
        const OFString& combined, const OFString& file = "file")
{
    OFString tmpString;
    OFString slashFile;
    slashFile = PATH_SEPARATOR;
    slashFile += "file";

    OFStandard::normalizeDirName(tmpString, input);
    OFCHECK_EQUAL(tmpString, normalized);

    OFStandard::combineDirAndFilename(tmpString, input, file);
    OFCHECK_EQUAL(tmpString, combined);

    OFStandard::combineDirAndFilename(tmpString, input, PATH_SEPARATOR + file);
    OFCHECK_EQUAL(tmpString, slashFile);
}

OFTEST(ofstd_testPaths)
{
    OFString tmpString;
    OFString sourceRoot;
    OFString input;
    OFString normalized;
    OFString combined;
    OFString pathSeparator;

    pathSeparator += PATH_SEPARATOR;
    sourceRoot = ".." + pathSeparator + "..";

    normalized = sourceRoot;
    input = normalized + pathSeparator;
    combined = input + "file";

    checkExists(input, OFTrue, OFFalse, OFTrue);
    checkPathHandling(input, normalized, combined);

    // Trailing slashes shouldn't matter
    input += pathSeparator + pathSeparator + pathSeparator;

    checkExists(input, OFTrue, OFFalse, OFTrue);
    checkPathHandling(input, normalized, combined);

    // Now check a non-existent file
    normalized = sourceRoot + pathSeparator + "does_not_exist";
    input = normalized;
    combined = input + pathSeparator + "file";

    checkExists(input, OFFalse, OFFalse, OFFalse);
    checkPathHandling(input, normalized, combined);

    // Check the working dir handling
    input = normalized = ".";
    combined = "." + pathSeparator + "file";

    checkExists(input, OFTrue, OFFalse, OFTrue);
    checkPathHandling(input, normalized, combined);

    // How is the empty string treated?
    input = "";

    checkExists(input, OFFalse, OFFalse, OFFalse);
    checkPathHandling(input, normalized, combined);

    // Check what happens to "///"
    normalized = pathSeparator;
    input = pathSeparator + pathSeparator + pathSeparator;
    combined = normalized + "file";

    // No checkExists() since Windows doesn't have "/"
    checkPathHandling(input, normalized, combined);

    // TODO FIXME
    // These tests have problems. They assume that they are run from the
    // source dir, but with cmake they are usually run from elsewhere.
#if 0
    // Now let's check if the ofstd dir exists
    normalized = sourceRoot + pathSeparator + "ofstd";
    input = normalized;
    combined = input + pathSeparator + "file";

    checkExists(input, OFTrue, OFFalse, OFTrue);
    checkPathHandling(input, normalized, combined);

    // There should be a INSTALL file somewhere
    normalized = sourceRoot + pathSeparator + "INSTALL";
    input = normalized;
    combined = input + pathSeparator + "file";

    checkExists(input, OFTrue, OFTrue, OFFalse);
    checkPathHandling(input, normalized, combined);
#endif

    // Now come some special tests for combineDirAndFilename
    input = pathSeparator + pathSeparator + "caesar" + pathSeparator + "share";
    OFStandard::combineDirAndFilename(tmpString, input, "file");
    OFCHECK_EQUAL(tmpString, input + pathSeparator + "file");

    OFStandard::combineDirAndFilename(tmpString, input, pathSeparator + "file");
    OFCHECK_EQUAL(tmpString, pathSeparator + "file");

    OFStandard::combineDirAndFilename(tmpString, "", "file");
    OFCHECK_EQUAL(tmpString, "." + pathSeparator + "file");

    OFStandard::combineDirAndFilename(tmpString, "", "file", OFTrue);
    OFCHECK_EQUAL(tmpString, "file");

    OFStandard::combineDirAndFilename(tmpString, "", ".");
    OFCHECK_EQUAL(tmpString, ".");

    OFStandard::combineDirAndFilename(tmpString, "..", ".");
    OFCHECK_EQUAL(tmpString, "..");

    OFStandard::combineDirAndFilename(tmpString, "", "");
    OFCHECK_EQUAL(tmpString, ".");

    OFStandard::combineDirAndFilename(tmpString, "", "", OFTrue);
    OFCHECK_EQUAL(tmpString, "");
}

OFTEST(ofstd_OFStandard_isReadWriteable)
{
    // TODO FIXME
    // Same as above, this assumes that it's called from the source dir which
    // isn't necessarily true with cmake.
#if 0
    OFCHECK_EQUAL(OFStandard::isReadable("tofstd.cc"), OFTrue);
    OFCHECK_EQUAL(OFStandard::isWriteable("tofstd.cc"), OFTrue);
#endif
    OFCHECK_EQUAL(OFStandard::isReadable("does_not_exist"), OFFalse);
    OFCHECK_EQUAL(OFStandard::isWriteable("does_not_exist"), OFFalse);
}


/*
 *
 * CVS/RCS Log:
 * $Log: tofstd.cc,v $
 * Revision 1.15  2011-07-06 13:04:18  uli
 * Fixed some inconsistencies in test names.
 *
 * Revision 1.14  2011-05-25 10:05:57  uli
 * Imported oftest and converted existing tests to oftest.
 *
 * Revision 1.13  2010-10-14 13:15:16  joergr
 * Updated copyright header. Added reference to COPYRIGHT file.
 *
 * Revision 1.12  2009-04-27 14:21:43  joergr
 * Added further test for UNC syntax of path expressions.
 *
 * Revision 1.11  2008-04-18 09:14:02  joergr
 * Added further tests for combineDirAndFilename().
 *
 * Revision 1.10  2007/06/26 16:19:40  joergr
 * Added new variant of encodeBase64() method that outputs directly to a stream
 * (avoids using a memory buffer for large binary data).
 *
 * Revision 1.9  2006/08/14 16:42:48  meichel
 * Updated all code in module ofstd to correctly compile if the standard
 * namespace has not included into the global one with a "using" directive.
 *
 * Revision 1.8  2005/12/08 15:49:06  meichel
 * Changed include path schema for all DCMTK header files
 *
 * Revision 1.7  2004/01/16 10:37:23  joergr
 * Removed acknowledgements with e-mail addresses from CVS log.
 *
 * Revision 1.6  2003/09/17 17:01:44  joergr
 * Renamed variable "string" to avoid name clash with STL class.
 *
 * Revision 1.5  2003/08/14 09:01:20  meichel
 * Adapted type casts to new-style typecast operators defined in ofcast.h
 *
 * Revision 1.4  2003/08/12 13:11:46  joergr
 * Improved implementation of normalizeDirName().
 *
 * Revision 1.3  2002/05/14 08:13:55  joergr
 * Added support for Base64 (MIME) encoding and decoding.
 *
 * Revision 1.2  2002/04/16 13:37:01  joergr
 * Added configurable support for C++ ANSI standard includes (e.g. streams).
 *
 *
 */
