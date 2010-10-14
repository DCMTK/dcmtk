/*
 *
 *  Copyright (C) 2002-2010, OFFIS e.V.
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
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:15:16 $
 *  CVS/RCS Revision: $Revision: 1.13 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/ofstd/ofstd.h"
#include "dcmtk/ofstd/ofconsol.h"


#define pathname1 "/home/joergr/source/dcmtk/"
#define pathname2 "/home/joergr/source/dcmtk/INSTALL"
#define pathname3 "/home/joergr/source/dcmtk///"
#define pathname4 "/home/joergr/source/dcmtk"
#define pathname5 "/home/joergr/source/dcmt"
#define pathname6 "///"
#define pathname7 "."
#define pathname8 "/home/joergr/tmp/test"
#define pathname9 "//caesar/share"

static const size_t bin_len = 10;
static const unsigned char bin_data[bin_len] = {10, 5, 88, 99, 255, 250, 150, 128, 0, 254};

static const OFString txt_data1 = ".AB_Cab$$c123-";
static const OFString txt_data2 = "ABC\nabc\n123";


int main()
{
    OFString tmpString;
    unsigned char *buffer;
    size_t i, length;

    /* file system tests */

    COUT << "pathExists(\"" << pathname1 << "\") = " << OFStandard::pathExists(pathname1) << OFendl;
    COUT << "fileExists(\"" << pathname1 << "\") = " << OFStandard::fileExists(pathname1) << OFendl;
    COUT << "dirExists(\""  << pathname1 << "\") = " << OFStandard::dirExists( pathname1) << OFendl << OFendl;

    COUT << "pathExists(\"" << pathname2 << "\") = " << OFStandard::pathExists(pathname2) << OFendl;
    COUT << "fileExists(\"" << pathname2 << "\") = " << OFStandard::fileExists(pathname2) << OFendl;
    COUT << "dirExists(\""  << pathname2 << "\") = " << OFStandard::dirExists( pathname2) << OFendl << OFendl;

    COUT << "pathExists(\"" << pathname3 << "\") = " << OFStandard::pathExists(pathname3) << OFendl;
    COUT << "fileExists(\"" << pathname3 << "\") = " << OFStandard::fileExists(pathname3) << OFendl;
    COUT << "dirExists(\""  << pathname3 << "\") = " << OFStandard::dirExists( pathname3) << OFendl << OFendl;

    COUT << "pathExists(\"" << pathname4 << "\") = " << OFStandard::pathExists(pathname4) << OFendl;
    COUT << "fileExists(\"" << pathname4 << "\") = " << OFStandard::fileExists(pathname4) << OFendl;
    COUT << "dirExists(\""  << pathname4 << "\") = " << OFStandard::dirExists( pathname4) << OFendl << OFendl;

    COUT << "pathExists(\"" << pathname5 << "\") = " << OFStandard::pathExists(pathname5) << OFendl;
    COUT << "fileExists(\"" << pathname5 << "\") = " << OFStandard::fileExists(pathname5) << OFendl;
    COUT << "dirExists(\""  << pathname5 << "\") = " << OFStandard::dirExists( pathname5) << OFendl << OFendl;

    COUT << "normalizeDirName(\"" << pathname1 << "\") = " << OFStandard::normalizeDirName(tmpString, pathname1) << OFendl;
    COUT << "normalizeDirName(\"" << pathname2 << "\") = " << OFStandard::normalizeDirName(tmpString, pathname2) << OFendl;
    COUT << "normalizeDirName(\"" << pathname3 << "\") = " << OFStandard::normalizeDirName(tmpString, pathname3) << OFendl;
    COUT << "normalizeDirName(\"" << pathname4 << "\") = " << OFStandard::normalizeDirName(tmpString, pathname4) << OFendl;
    COUT << "normalizeDirName(\"" << pathname5 << "\") = " << OFStandard::normalizeDirName(tmpString, pathname5) << OFendl;
    COUT << "normalizeDirName(\"" << pathname6 << "\") = " << OFStandard::normalizeDirName(tmpString, pathname6) << OFendl;
    COUT << "normalizeDirName(\"" << pathname7 << "\") = " << OFStandard::normalizeDirName(tmpString, pathname7) << OFendl;
    COUT << "normalizeDirName(\"" << pathname9 << "\") = " << OFStandard::normalizeDirName(tmpString, pathname9) << OFendl;
    COUT << "normalizeDirName(\"\") = " << OFStandard::normalizeDirName(tmpString, "") << OFendl << OFendl;

    COUT << "combineDirAndFilename(\"" << pathname1 << "\", \"file\") = " << OFStandard::combineDirAndFilename(tmpString, pathname1, "file") << OFendl;
    COUT << "combineDirAndFilename(\"" << pathname2 << "\", \"file\") = " << OFStandard::combineDirAndFilename(tmpString, pathname2, "file") << OFendl;
    COUT << "combineDirAndFilename(\"" << pathname3 << "\", \"file\") = " << OFStandard::combineDirAndFilename(tmpString, pathname3, "file") << OFendl;
    COUT << "combineDirAndFilename(\"" << pathname4 << "\", \"file\") = " << OFStandard::combineDirAndFilename(tmpString, pathname4, "file") << OFendl;
    COUT << "combineDirAndFilename(\"" << pathname5 << "\", \"file\") = " << OFStandard::combineDirAndFilename(tmpString, pathname5, "file") << OFendl;
    COUT << "combineDirAndFilename(\"" << pathname5 << "\", \"/file\") = " << OFStandard::combineDirAndFilename(tmpString, pathname5, "/file") << OFendl;
    COUT << "combineDirAndFilename(\"" << pathname6 << "\", \"file\") = " << OFStandard::combineDirAndFilename(tmpString, pathname6, "file") << OFendl;
    COUT << "combineDirAndFilename(\"" << pathname9 << "\", \"file\") = " << OFStandard::combineDirAndFilename(tmpString, pathname9, "file") << OFendl;
    COUT << "combineDirAndFilename(\"\", \"file\") = " << OFStandard::combineDirAndFilename(tmpString, "", "file") << OFendl;
    COUT << "combineDirAndFilename(\"\", \"file\", OFTrue) = " << OFStandard::combineDirAndFilename(tmpString, "", "file", OFTrue) << OFendl;
    COUT << "combineDirAndFilename(\"\", \".\") = " << OFStandard::combineDirAndFilename(tmpString, "", ".") << OFendl;
    COUT << "combineDirAndFilename(\"..\", \".\") = " << OFStandard::combineDirAndFilename(tmpString, "..", ".") << OFendl;
    COUT << "combineDirAndFilename(\"\", \"\") = " << OFStandard::combineDirAndFilename(tmpString, "", "") << OFendl;
    COUT << "combineDirAndFilename(\"\", \"\", OFTrue) = " << OFStandard::combineDirAndFilename(tmpString, "", "", OFTrue) << OFendl;

    COUT << "isReadable() = " << OFStandard::isReadable(pathname8) << OFendl;
    COUT << "isWriteable() = " << OFStandard::isWriteable(pathname8) << OFendl;

    /* Base64 encoding/decoding */

    COUT << "original data: ";
    for (i = 0; i < bin_len; i++)
        COUT << OFstatic_cast(int, bin_data[i]) << " ";
    COUT << OFendl;
    COUT << "base64 encoded (buffer): " << OFStandard::encodeBase64(bin_data, bin_len, tmpString) << OFendl;
    COUT << "base64 encoded (stream): "; OFStandard::encodeBase64(COUT, bin_data, bin_len); COUT << OFendl;
    length = OFStandard::decodeBase64(tmpString, buffer);
    COUT << "base64 decoded: ";
    for (i = 0; i < length; i++)
        COUT << OFstatic_cast(int, buffer[i]) << " ";
    COUT << OFendl << OFendl;
    delete[] buffer;

    buffer = new unsigned char[511];
    for (i = 0; i < 256; i++)
        buffer[i] = OFstatic_cast(unsigned char, i);
    for (i = 256; i < 511; i++)
        buffer[i] = OFstatic_cast(unsigned char, 510 - i);
    COUT << "original data: ";
    for (i = 0; i < 511; i++)
        COUT << OFstatic_cast(int, buffer[i]) << " ";
    COUT << OFendl;
    COUT << "base64 encoded (buffer): " << OFendl;
    COUT << OFStandard::encodeBase64(buffer, 511, tmpString) << OFendl;
    COUT << "base64 encoded (stream): " << OFendl;
    OFStandard::encodeBase64(COUT, buffer, 511); COUT << OFendl;
    COUT << "base64 with line breaks (buffer):" << OFendl;
    COUT << OFStandard::encodeBase64(buffer, 511, tmpString, 72) << OFendl;
    COUT << "base64 with line breaks (stream):" << OFendl;
    OFStandard::encodeBase64(COUT, buffer, 511, 72); COUT << OFendl;
    delete[] buffer;
    length = OFStandard::decodeBase64(tmpString, buffer);
    COUT << "base64 decoded: " << OFendl;
    for (i = 0; i < length; i++)
        COUT << OFstatic_cast(int, buffer[i]) << " ";
    COUT << OFendl << OFendl;
    delete[] buffer;

    COUT << "original data: " << txt_data1 << OFendl;
    length = OFStandard::decodeBase64(txt_data1, buffer);
    COUT << "base64 decoded: ";
    for (i = 0; i < length; i++)
        COUT << OFstatic_cast(int, buffer[i]) << " ";
    COUT << OFendl << OFendl;
    delete[] buffer;

    COUT << "original data: " << txt_data2 << OFendl;
    length = OFStandard::decodeBase64(txt_data2, buffer);
    COUT << "base64 decoded: ";
    for (i = 0; i < length; i++)
        COUT << OFstatic_cast(int, buffer[i]) << " ";
    COUT << OFendl << OFendl;
    delete[] buffer;

    return 0;
}


/*
 *
 * CVS/RCS Log:
 * $Log: tofstd.cc,v $
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
