/*
 *
 *  Copyright (C) 2002, OFFIS
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
 *  Update Date:      $Date: 2002-04-16 13:37:01 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/ofstd/tests/tofstd.cc,v $
 *  CVS/RCS Revision: $Revision: 1.2 $
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

int main()
{
    OFString string;
    
    COUT << "pathExists(" << pathname1 << ") = " << OFStandard::pathExists(pathname1) << endl;
    COUT << "fileExists(" << pathname1 << ") = " << OFStandard::fileExists(pathname1) << endl;
    COUT << "dirExists("  << pathname1 << ") = " << OFStandard::dirExists( pathname1) << endl << endl;

    COUT << "pathExists(" << pathname2 << ") = " << OFStandard::pathExists(pathname2) << endl;
    COUT << "fileExists(" << pathname2 << ") = " << OFStandard::fileExists(pathname2) << endl;
    COUT << "dirExists("  << pathname2 << ") = " << OFStandard::dirExists( pathname2) << endl << endl;

    COUT << "pathExists(" << pathname3 << ") = " << OFStandard::pathExists(pathname3) << endl;
    COUT << "fileExists(" << pathname3 << ") = " << OFStandard::fileExists(pathname3) << endl;
    COUT << "dirExists("  << pathname3 << ") = " << OFStandard::dirExists( pathname3) << endl << endl;

    COUT << "pathExists(" << pathname4 << ") = " << OFStandard::pathExists(pathname4) << endl;
    COUT << "fileExists(" << pathname4 << ") = " << OFStandard::fileExists(pathname4) << endl;
    COUT << "dirExists("  << pathname4 << ") = " << OFStandard::dirExists( pathname4) << endl << endl;

    COUT << "pathExists(" << pathname5 << ") = " << OFStandard::pathExists(pathname5) << endl;
    COUT << "fileExists(" << pathname5 << ") = " << OFStandard::fileExists(pathname5) << endl;
    COUT << "dirExists("  << pathname5 << ") = " << OFStandard::dirExists( pathname5) << endl << endl;

    COUT << "normalizeDirName(" << pathname1 << ") = " << OFStandard::normalizeDirName(string, pathname1) << endl;
    COUT << "normalizeDirName(" << pathname2 << ") = " << OFStandard::normalizeDirName(string, pathname2) << endl;
    COUT << "normalizeDirName(" << pathname3 << ") = " << OFStandard::normalizeDirName(string, pathname3) << endl;
    COUT << "normalizeDirName(" << pathname4 << ") = " << OFStandard::normalizeDirName(string, pathname4) << endl;
    COUT << "normalizeDirName(" << pathname5 << ") = " << OFStandard::normalizeDirName(string, pathname5) << endl;
    COUT << "normalizeDirName(" << pathname6 << ") = " << OFStandard::normalizeDirName(string, pathname6) << endl;
    COUT << "normalizeDirName() = " << OFStandard::normalizeDirName(string, "") << endl << endl;

    COUT << "combineDirAndFilename(" << pathname1 << ", \"file\") = " << OFStandard::combineDirAndFilename(string, pathname1, "file") << endl;
    COUT << "combineDirAndFilename(" << pathname2 << ", \"file\") = " << OFStandard::combineDirAndFilename(string, pathname2, "file") << endl;
    COUT << "combineDirAndFilename(" << pathname3 << ", \"file\") = " << OFStandard::combineDirAndFilename(string, pathname3, "file") << endl;
    COUT << "combineDirAndFilename(" << pathname4 << ", \"file\") = " << OFStandard::combineDirAndFilename(string, pathname4, "file") << endl;
    COUT << "combineDirAndFilename(" << pathname5 << ", \"file\") = " << OFStandard::combineDirAndFilename(string, pathname5, "file") << endl;
    COUT << "combineDirAndFilename(" << pathname6 << ", \"file\") = " << OFStandard::combineDirAndFilename(string, pathname6, "file") << endl;
    COUT << "combineDirAndFilename(\"\", \"file\") = " << OFStandard::combineDirAndFilename(string, "", "file") << endl;
    COUT << "combineDirAndFilename(\"\", \"\") = " << OFStandard::combineDirAndFilename(string, "", "") << endl;

    COUT << "isReadable() = " << OFStandard::isReadable("/home/joergr/tmp/test") << endl;
    COUT << "isWriteable() = " << OFStandard::isWriteable("/home/joergr/tmp/test") << endl;
    
    return 0;
}


/*
 *
 * CVS/RCS Log:
 * $Log: tofstd.cc,v $
 * Revision 1.2  2002-04-16 13:37:01  joergr
 * Added configurable support for C++ ANSI standard includes (e.g. streams).
 * Thanks to Andreas Barth <Andreas.Barth@bruker-biospin.de> for his
 * contribution.
 *
 *
 */
