/*
 *
 *  Copyright (C) 2002-2022, OFFIS e.V.
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
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/ofstd/ofstd.h"

#define OFTEST_OFSTD_ONLY
#include "dcmtk/ofstd/oftest.h"

BEGIN_EXTERN_C
#ifdef HAVE_SYS_SOCKET_H
#include <sys/socket.h>
#endif
#ifdef HAVE_NETDB_H
#include <netdb.h>
#endif
END_EXTERN_C

#ifdef HAVE_WINDOWS_H
#include <winsock2.h>
#elif defined(HAVE_WINSOCK_H)
#include <winsock.h>
#endif

#define OFFILENAME_TO_OFSTRING(filename) \
    OFString(OFSTRING_GUARD(filename.getCharPointer()))

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
    OFString result;
    OFString slashFile;
    slashFile = PATH_SEPARATOR;
    slashFile += "file";
#ifdef _WIN32
    OFString slashFile2;
    slashFile2 = '/';
    slashFile2 += "file";
#endif

    OFStandard::normalizeDirName(result, input);
    OFCHECK_EQUAL(result, normalized);

    OFStandard::combineDirAndFilename(result, input, file);
    OFCHECK_EQUAL(result, combined);

    OFStandard::combineDirAndFilename(result, input, PATH_SEPARATOR + file);
    OFCHECK_EQUAL(result, slashFile);

#ifdef _WIN32
    OFStandard::combineDirAndFilename(result, input, '/' + file);
    OFCHECK_EQUAL(result, slashFile2);
#endif

}

OFTEST(ofstd_testPaths_1)
{
    OFString result;
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
    OFStandard::combineDirAndFilename(result, input, "file");
    OFCHECK_EQUAL(result, input + pathSeparator + "file");

    OFStandard::combineDirAndFilename(result, input, pathSeparator + "file");
    OFCHECK_EQUAL(result, pathSeparator + "file");

    OFStandard::combineDirAndFilename(result, "", "file");
    OFCHECK_EQUAL(result, "." + pathSeparator + "file");

    OFStandard::combineDirAndFilename(result, "", "file", OFTrue /*allowEmptyDirName*/);
    OFCHECK_EQUAL(result, "file");

    OFStandard::combineDirAndFilename(result, "", ".");
    OFCHECK_EQUAL(result, ".");

    OFStandard::combineDirAndFilename(result, "..", ".");
    OFCHECK_EQUAL(result, "..");

    OFStandard::combineDirAndFilename(result, "", "");
    OFCHECK_EQUAL(result, ".");

    OFStandard::combineDirAndFilename(result, "", "", OFTrue /*allowEmptyDirName*/);
    OFCHECK_EQUAL(result, "");

    // hidden files/directories start with a "." (on Unix systems)
    OFStandard::combineDirAndFilename(result, "", ".hidden", OFTrue /*allowEmptyDirName*/);
    OFCHECK_EQUAL(result, ".hidden");

#ifdef _WIN32
    // now some special cases for Windows, where both slash and backslash are
    // supported as path separators
    pathSeparator = '/';
    sourceRoot = ".." + pathSeparator + "..";

    normalized = sourceRoot;
    input = normalized + pathSeparator;
    combined = normalized + PATH_SEPARATOR + "file";

    checkExists(input, OFTrue, OFFalse, OFTrue);
    checkPathHandling(input, normalized, combined);

    // Trailing slashes shouldn't matter
    input += pathSeparator + pathSeparator + pathSeparator;

    checkExists(input, OFTrue, OFFalse, OFTrue);
    checkPathHandling(input, normalized, combined);

    // Now check a non-existent file
    normalized = sourceRoot + pathSeparator + "does_not_exist";
    input = normalized;
    combined = input + PATH_SEPARATOR + "file";

    checkExists(input, OFFalse, OFFalse, OFFalse);
    checkPathHandling(input, normalized, combined);

    // Check the working dir handling
    input = normalized = ".";
    combined = ".";
    combined += PATH_SEPARATOR;
    combined += "file";

    checkExists(input, OFTrue, OFFalse, OFTrue);
    checkPathHandling(input, normalized, combined);

    // Check what happens to "///"
    normalized = pathSeparator;
    input = pathSeparator + pathSeparator + pathSeparator;
    combined = normalized + "file";

    // No checkExists() since Windows doesn't have "/"
    checkPathHandling(input, normalized, combined);

    // Now come some special tests for combineDirAndFilename
    input = pathSeparator + pathSeparator + "caesar" + pathSeparator + "share";
    OFStandard::combineDirAndFilename(result, input, "file");
    OFCHECK_EQUAL(result, input + PATH_SEPARATOR + "file");

    OFStandard::combineDirAndFilename(result, input, pathSeparator + "file");
    OFCHECK_EQUAL(result, pathSeparator + "file");

    combined = ".";
    combined += PATH_SEPARATOR;
    combined += "file";
    OFStandard::combineDirAndFilename(result, "", "file");
    OFCHECK_EQUAL(result, combined);

    OFStandard::combineDirAndFilename(result, "", "file", OFTrue /*allowEmptyDirName*/);
    OFCHECK_EQUAL(result, "file");

    OFStandard::combineDirAndFilename(result, "", ".");
    OFCHECK_EQUAL(result, ".");

    OFStandard::combineDirAndFilename(result, "..", ".");
    OFCHECK_EQUAL(result, "..");

    OFStandard::combineDirAndFilename(result, "", "");
    OFCHECK_EQUAL(result, ".");

    OFStandard::combineDirAndFilename(result, "", "", OFTrue /*allowEmptyDirName*/);
    OFCHECK_EQUAL(result, "");

    // hidden files/directories start with a "." (on Unix systems)
    OFStandard::combineDirAndFilename(result, "", ".hidden", OFTrue /*allowEmptyDirName*/);
    OFCHECK_EQUAL(result, ".hidden");
#endif
}

OFTEST(ofstd_testPaths_2)
{
    OFString result;
    OFString pathSeparator;

    pathSeparator += PATH_SEPARATOR;

    // Check getDirNameFromPath()
    OFStandard::getDirNameFromPath(result, "dirname" + pathSeparator + "filename");
    OFCHECK_EQUAL(result, "dirname");
    OFStandard::getDirNameFromPath(result, pathSeparator + "dirname" + pathSeparator);
    OFCHECK_EQUAL(result, pathSeparator + "dirname");
    OFStandard::getDirNameFromPath(result, "dirname");
    OFCHECK_EQUAL(result, "dirname");
    OFStandard::getDirNameFromPath(result, "dirname", OFFalse /*assumeDirName*/);
    OFCHECK_EQUAL(result, "");
#ifdef _WIN32
    // on Windows, we also check paths containing '/' as a path separator,
    // as well as mixed paths containing both separators
    OFStandard::getDirNameFromPath(result, "dirname/filename");
    OFCHECK_EQUAL(result, "dirname");
    OFStandard::getDirNameFromPath(result, "dirname/dirname" + pathSeparator +"filename");
    OFCHECK_EQUAL(result, "dirname/dirname");
    OFStandard::getDirNameFromPath(result, "dirname" + pathSeparator + "dirname/filename");
    OFCHECK_EQUAL(result, "dirname" + pathSeparator + "dirname");
    OFStandard::getDirNameFromPath(result, "/dirname/");
    OFCHECK_EQUAL(result, "/dirname");
#endif

    // Check getFilenameFromPath()
    OFStandard::getFilenameFromPath(result, "dirname" + pathSeparator + "filename");
    OFCHECK_EQUAL(result, "filename");
    OFStandard::getFilenameFromPath(result, pathSeparator + "dirname" + pathSeparator);
    OFCHECK_EQUAL(result, "");
    OFStandard::getFilenameFromPath(result, pathSeparator + "filename");
    OFCHECK_EQUAL(result, "filename");
    OFStandard::getFilenameFromPath(result, "filename");
    OFCHECK_EQUAL(result, "filename");
    OFStandard::getFilenameFromPath(result, "filename", OFFalse /*assumeFilename*/);
    OFCHECK_EQUAL(result, "");
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

OFTEST(ofstd_OFStandard_appendFilenameExtension)
{
    OFFilename result;
    const char *nullPtr = NULL;

    OFCHECK_EQUAL(OFFILENAME_TO_OFSTRING(OFStandard::appendFilenameExtension(result, "file", ".ext")), "file.ext");
    OFCHECK_EQUAL(OFFILENAME_TO_OFSTRING(OFStandard::appendFilenameExtension(result, "", ".ext")), ".ext");
    OFCHECK_EQUAL(OFFILENAME_TO_OFSTRING(OFStandard::appendFilenameExtension(result, "file", "")), "file");
    OFCHECK_EQUAL(OFFILENAME_TO_OFSTRING(OFStandard::appendFilenameExtension(result, "file", nullPtr)), "file");
    OFCHECK_EQUAL(OFFILENAME_TO_OFSTRING(OFStandard::appendFilenameExtension(result, nullPtr, "")), "");
    OFCHECK_EQUAL(OFFILENAME_TO_OFSTRING(OFStandard::appendFilenameExtension(result, nullPtr, nullPtr)), "");
}

OFTEST(ofstd_OFStandard_removeRootDirFromPathname)
{
    OFFilename result;
    const char *nullPtr = NULL;

    OFString testPath = "/root";
    testPath += PATH_SEPARATOR;
    testPath += "path";
    OFString resultPath;
    resultPath += PATH_SEPARATOR;
    resultPath += "path";

    OFCHECK(OFStandard::removeRootDirFromPathname(result, "/root", testPath, OFTrue /*allowLeadingPathSeparator*/).good());
    OFCHECK_EQUAL(OFFILENAME_TO_OFSTRING(result), resultPath);
    OFCHECK(OFStandard::removeRootDirFromPathname(result, "/root", testPath, OFFalse /*allowLeadingPathSeparator*/).good());
    OFCHECK_EQUAL(OFFILENAME_TO_OFSTRING(result), "path");

    OFCHECK(OFStandard::removeRootDirFromPathname(result, "/root", "/no_root/path").bad());
    OFCHECK(result.isEmpty());
    OFCHECK(OFStandard::removeRootDirFromPathname(result, "", "/root/path").good());
    OFCHECK_EQUAL(OFFILENAME_TO_OFSTRING(result), "/root/path");
    OFCHECK(OFStandard::removeRootDirFromPathname(result, "/root", "").bad());
    OFCHECK_EQUAL(OFFILENAME_TO_OFSTRING(result), "");
    OFCHECK(OFStandard::removeRootDirFromPathname(result, "", "").good());
    OFCHECK_EQUAL(OFFILENAME_TO_OFSTRING(result), "");
    OFCHECK(OFStandard::removeRootDirFromPathname(result, nullPtr, "/path").good());
    OFCHECK_EQUAL(OFFILENAME_TO_OFSTRING(result), "/path");
    OFCHECK(OFStandard::removeRootDirFromPathname(result, "/root", nullPtr).bad());
    OFCHECK_EQUAL(result.getCharPointer(), nullPtr);
    OFCHECK(OFStandard::removeRootDirFromPathname(result, nullPtr, nullPtr).good());
}

OFTEST(ofstd_safeSubtractAddMult)
{
  // --------------- Subtraction ----------------

  unsigned int a = 1;
  unsigned int b = 2;
  // check whether underflow occurs (it should)
  OFCHECK(OFStandard::safeSubtract(a, b, a) == OFFalse);
  // check whether a has not been modified
  OFCHECK_EQUAL(a, 1);

  a = OFnumeric_limits<unsigned int>::max();
  b = OFnumeric_limits<unsigned int>::max()-1;
  // check whether no underflow occurred (it shouldn't)
  OFCHECK(OFStandard::safeSubtract(a, b, a) == OFTrue);
  // check whether the result a was computed as expected
  OFCHECK_EQUAL(a, 1);

  // --------------- Addition ----------------

  a = OFnumeric_limits<unsigned int>::max()-1;
  b = OFnumeric_limits<unsigned int>::max()-1;
  // check whether overflow occurred (it should)
  OFCHECK(OFStandard::safeAdd(a, b, a) == OFFalse);
  // check whether a has not been modified
  OFCHECK_EQUAL(a, OFnumeric_limits<unsigned int>::max()-1);

  b = 1; // a still equals max-1
  // check whether no overflow occurred (it shouldn't)
  OFCHECK(OFStandard::safeAdd(a, b, a) == OFTrue);
  // check whether the result a was computed as expected.
  // dividing and then multiplying by 2 is required since max may be an
  // odd number so that max/2 is rounded to the floor number.
  OFCHECK_EQUAL(a, OFnumeric_limits<unsigned int>::max());

  // --------------- Multiplication ----------------
  a = OFnumeric_limits<unsigned int>::max() / 2;
  // check whether overflow occurred (it should)
  OFCHECK( OFStandard::safeMult(a, OFstatic_cast(unsigned int, 3), a) == OFFalse);
  // check whether no overflow occurred (it shouldn't)
  OFCHECK_EQUAL(a, OFnumeric_limits<unsigned int>::max() / 2);

  b = 2; // a still equals max / 2
  OFCHECK( OFStandard::safeMult(a, b, a) == OFTrue);
  if ( (OFnumeric_limits<unsigned int>::max() %2 == 1) )
      OFCHECK_EQUAL(a+1, OFnumeric_limits<unsigned int>::max());
  else
      OFCHECK_EQUAL(a, OFnumeric_limits<unsigned int>::max());
}

OFTEST(ofstd_snprintf)
{
  // This test tests whether OFStandard::snprintf() properly "cuts"
  // the formatted output string based on the given buffer size
  // and whether it returns the correct return value, i.e.
  // the number of characters that SHOULD have been written.
  // We only exercise this for a formatted integer and not
  // for all other possible types of arguments

  char buf[10]; // a buffer for 10 characters (including terminating NUL)
  const char *s = "987654321"; // a 10 character string (including terminating NUL)
  int i = 12345;
  int count = 0;
  OFString zero;
  OFString one = "1";
  OFString two = "12";
  OFString three = "123";
  OFString four = "1234";
  OFString five = "12345";

  // initialize buffer with a zero terminated string
  // that is definitely not what we expect
  memcpy(buf, s, 10);

  // snprintf() into the buffer, check result and return value
  count = OFStandard::snprintf(buf, 1, "%i", i);
  OFCHECK((count == 5) && (zero ==  buf));

  // repeat the same with increasing buffer size...
  memcpy(buf, s, 10);
  count = OFStandard::snprintf(buf, 2, "%i", i);
  OFCHECK((count == 5) && (one ==  buf));

  memcpy(buf, s, 10);
  count = OFStandard::snprintf(buf, 3, "%i", i);
  OFCHECK((count == 5) && (two ==  buf));

  memcpy(buf, s, 10);
  count = OFStandard::snprintf(buf, 4, "%i", i);
  OFCHECK((count == 5) && (three ==  buf));

  memcpy(buf, s, 10);
  count = OFStandard::snprintf(buf, 5, "%i", i);
  OFCHECK((count == 5) && (four ==  buf));

  // ...until finally the formatted string fits completely into the buffer.
  memcpy(buf, s, 10);
  count = OFStandard::snprintf(buf, 10, "%i", i);
  OFCHECK((count == 5) && (five ==  buf));
}

OFTEST(ofstd_gethostnamebyaddress)
{
  char buf[] = { 127, 0 , 0, 1 };
  OFString hostname = OFStandard::getHostnameByAddress(buf, 4, AF_INET);
  // most platforms will return "localhost", Cygwin will return the real hostname
  OFCHECK(hostname != "127.0.0.1");
}
