/*
 *
 *  Copyright (C) 2002-2003, OFFIS
 *
 *  This software and supporting documentation were developed by
 *
 *    Kuratorium OFFIS e.V.
 *    Healthcare Information and Communication Systems
 *    Escherweg 2
 *    D-26121 Oldenburg, Germany
 *
 *  THIS SOFTWARE IS MADE AVAILABLE,  AS IS,  AND OFFIS MAKES NO  WARRANTY
 *  REGARDING  THE  SOFTWARE,  ITS  PERFORMANCE,  ITS  MERCHANTABILITY  OR
 *  FITNESS FOR ANY PARTICULAR USE, FREEDOM FROM ANY COMPUTER DISEASES  OR
 *  ITS CONFORMITY TO ANY SPECIFICATION. THE ENTIRE RISK AS TO QUALITY AND
 *  PERFORMANCE OF THE SOFTWARE IS WITH THE USER.
 *
 *  Module:  ofstd
 *
 *  Author:  Joerg Riesmeier
 *
 *  Purpose: test program for class OFStandard
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2003-09-17 17:01:44 $
 *  CVS/RCS Revision: $Revision: 1.6 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include "ofstd.h"
#include "ofconsol.h"


#define pathname1 "/home/joergr/source/dcmtk/"
#define pathname2 "/home/joergr/source/dcmtk/INSTALL"
#define pathname3 "/home/joergr/source/dcmtk///"
#define pathname4 "/home/joergr/source/dcmtk"
#define pathname5 "/home/joergr/source/dcmt"
#define pathname6 "///"
#define pathname7 "."
#define pathname8 "/home/joergr/tmp/test"

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

    COUT << "pathExists(\"" << pathname1 << "\") = " << OFStandard::pathExists(pathname1) << endl;
    COUT << "fileExists(\"" << pathname1 << "\") = " << OFStandard::fileExists(pathname1) << endl;
    COUT << "dirExists(\""  << pathname1 << "\") = " << OFStandard::dirExists( pathname1) << endl << endl;

    COUT << "pathExists(\"" << pathname2 << "\") = " << OFStandard::pathExists(pathname2) << endl;
    COUT << "fileExists(\"" << pathname2 << "\") = " << OFStandard::fileExists(pathname2) << endl;
    COUT << "dirExists(\""  << pathname2 << "\") = " << OFStandard::dirExists( pathname2) << endl << endl;

    COUT << "pathExists(\"" << pathname3 << "\") = " << OFStandard::pathExists(pathname3) << endl;
    COUT << "fileExists(\"" << pathname3 << "\") = " << OFStandard::fileExists(pathname3) << endl;
    COUT << "dirExists(\""  << pathname3 << "\") = " << OFStandard::dirExists( pathname3) << endl << endl;

    COUT << "pathExists(\"" << pathname4 << "\") = " << OFStandard::pathExists(pathname4) << endl;
    COUT << "fileExists(\"" << pathname4 << "\") = " << OFStandard::fileExists(pathname4) << endl;
    COUT << "dirExists(\""  << pathname4 << "\") = " << OFStandard::dirExists( pathname4) << endl << endl;

    COUT << "pathExists(\"" << pathname5 << "\") = " << OFStandard::pathExists(pathname5) << endl;
    COUT << "fileExists(\"" << pathname5 << "\") = " << OFStandard::fileExists(pathname5) << endl;
    COUT << "dirExists(\""  << pathname5 << "\") = " << OFStandard::dirExists( pathname5) << endl << endl;

    COUT << "normalizeDirName(\"" << pathname1 << "\") = " << OFStandard::normalizeDirName(tmpString, pathname1) << endl;
    COUT << "normalizeDirName(\"" << pathname2 << "\") = " << OFStandard::normalizeDirName(tmpString, pathname2) << endl;
    COUT << "normalizeDirName(\"" << pathname3 << "\") = " << OFStandard::normalizeDirName(tmpString, pathname3) << endl;
    COUT << "normalizeDirName(\"" << pathname4 << "\") = " << OFStandard::normalizeDirName(tmpString, pathname4) << endl;
    COUT << "normalizeDirName(\"" << pathname5 << "\") = " << OFStandard::normalizeDirName(tmpString, pathname5) << endl;
    COUT << "normalizeDirName(\"" << pathname6 << "\") = " << OFStandard::normalizeDirName(tmpString, pathname6) << endl;
    COUT << "normalizeDirName(\"" << pathname7 << "\") = " << OFStandard::normalizeDirName(tmpString, pathname7) << endl;
    COUT << "normalizeDirName(\"\") = " << OFStandard::normalizeDirName(tmpString, "") << endl << endl;

    COUT << "combineDirAndFilename(\"" << pathname1 << "\", \"file\") = " << OFStandard::combineDirAndFilename(tmpString, pathname1, "file") << endl;
    COUT << "combineDirAndFilename(\"" << pathname2 << "\", \"file\") = " << OFStandard::combineDirAndFilename(tmpString, pathname2, "file") << endl;
    COUT << "combineDirAndFilename(\"" << pathname3 << "\", \"file\") = " << OFStandard::combineDirAndFilename(tmpString, pathname3, "file") << endl;
    COUT << "combineDirAndFilename(\"" << pathname4 << "\", \"file\") = " << OFStandard::combineDirAndFilename(tmpString, pathname4, "file") << endl;
    COUT << "combineDirAndFilename(\"" << pathname5 << "\", \"file\") = " << OFStandard::combineDirAndFilename(tmpString, pathname5, "file") << endl;
    COUT << "combineDirAndFilename(\"" << pathname5 << "\", \"/file\") = " << OFStandard::combineDirAndFilename(tmpString, pathname5, "/file") << endl;
    COUT << "combineDirAndFilename(\"" << pathname6 << "\", \"file\") = " << OFStandard::combineDirAndFilename(tmpString, pathname6, "file") << endl;
    COUT << "combineDirAndFilename(\"\", \"file\") = " << OFStandard::combineDirAndFilename(tmpString, "", "file") << endl;
    COUT << "combineDirAndFilename(\"\", \"file\", OFTrue) = " << OFStandard::combineDirAndFilename(tmpString, "", "file", OFTrue) << endl;
    COUT << "combineDirAndFilename(\"\", \"\") = " << OFStandard::combineDirAndFilename(tmpString, "", "") << endl;
    COUT << "combineDirAndFilename(\"\", \"\", OFTrue) = " << OFStandard::combineDirAndFilename(tmpString, "", "", OFTrue) << endl;

    COUT << "isReadable() = " << OFStandard::isReadable(pathname8) << endl;
    COUT << "isWriteable() = " << OFStandard::isWriteable(pathname8) << endl;

    /* Base64 encoding/decoding */

    COUT << "original data: ";
    for (i = 0; i < bin_len; i++)
        COUT << OFstatic_cast(int, bin_data[i]) << " ";
    COUT << endl;
    COUT << "base64 encoded: " << OFStandard::encodeBase64(bin_data, bin_len, tmpString) << endl;
    length = OFStandard::decodeBase64(tmpString, buffer);
    COUT << "base64 decoded: ";
    for (i = 0; i < length; i++)
        COUT << OFstatic_cast(int, buffer[i]) << " ";
    COUT << endl << endl;
    delete[] buffer;

    buffer = new unsigned char[511];
    for (i = 0; i < 256; i++)
        buffer[i] = OFstatic_cast(unsigned char, i);
    for (i = 256; i < 511; i++)
        buffer[i] = OFstatic_cast(unsigned char, 510 - i);
    COUT << "original data: ";
    for (i = 0; i < 511; i++)
        COUT << OFstatic_cast(int, buffer[i]) << " ";
    COUT << endl;
    COUT << "base64 encoded: " << OFStandard::encodeBase64(buffer, 511, tmpString) << endl;
    COUT << "base64 with line breaks:" << endl;
    COUT << OFStandard::encodeBase64(buffer, 511, tmpString, 72) << endl;
    delete[] buffer;
    length = OFStandard::decodeBase64(tmpString, buffer);
    COUT << "base64 decoded: ";
    for (i = 0; i < length; i++)
        COUT << OFstatic_cast(int, buffer[i]) << " ";
    COUT << endl << endl;
    delete[] buffer;

    COUT << "original data: " << txt_data1 << endl;
    length = OFStandard::decodeBase64(txt_data1, buffer);
    COUT << "base64 decoded: ";
    for (i = 0; i < length; i++)
        COUT << OFstatic_cast(int, buffer[i]) << " ";
    COUT << endl << endl;
    delete[] buffer;

    COUT << "original data: " << txt_data2 << endl;
    length = OFStandard::decodeBase64(txt_data2, buffer);
    COUT << "base64 decoded: ";
    for (i = 0; i < length; i++)
        COUT << OFstatic_cast(int, buffer[i]) << " ";
    COUT << endl << endl;
    delete[] buffer;

    return 0;
}


/*
 *
 * CVS/RCS Log:
 * $Log: tofstd.cc,v $
 * Revision 1.6  2003-09-17 17:01:44  joergr
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
 * Thanks to Andreas Barth <Andreas.Barth@bruker-biospin.de> for his
 * contribution.
 *
 *
 */
