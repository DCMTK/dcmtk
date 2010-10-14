/*
 *
 *  Copyright (C) 1994-2010, OFFIS e.V.
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
 *  Module:  dcmdata
 *
 *  Author:  Andrew Hewett
 *
 *  Purpose: Test if a file uses DICOM Part 10 format.
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:13:30 $
 *  CVS/RCS Revision: $Revision: 1.22 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#define INCLUDE_CSTDLIB
#define INCLUDE_CSTDIO
#define INCLUDE_CSTRING
#define INCLUDE_LIBC
#define INCLUDE_UNISTD
#include "dcmtk/ofstd/ofstdinc.h"

#include "dcmtk/dcmdata/dcmetinf.h"
#include "dcmtk/ofstd/ofconapp.h"
#include "dcmtk/dcmdata/dcuid.h"    /* for dcmtk version name */

#define OFFIS_CONSOLE_APPLICATION "dcmftest"

static char rcsid[] = "$dcmtk: " OFFIS_CONSOLE_APPLICATION " v"
  OFFIS_DCMTK_VERSION " " OFFIS_DCMTK_RELEASEDATE " $";

// ********************************************

int main(int argc, char *argv[])
{
    OFConsoleApplication app(OFFIS_CONSOLE_APPLICATION , "Test if file uses DICOM part 10 format", rcsid);
    OFCommandLine cmd;

    /* evaluate command line */
    cmd.addParam("file", OFCmdParam::PM_MultiMandatory);
    app.parseCommandLine(cmd, argc, argv, OFCommandLine::PF_ExpandWildcards);

    int badCount = 0;
    int count = cmd.getParamCount();
    const char *fname = NULL;
    for (int i=1; i<=count; i++) {
        cmd.getParam(i, fname);
        OFBool ok = OFFalse;
        FILE* f = NULL;

        if (fname && (strlen(fname) > 0)) f = fopen(fname, "rb");
        if (f == 0) {
            ok = OFFalse;
        } else {
            char signature[4];
            if ((fseek(f, DCM_PreambleLen, SEEK_SET) < 0) ||
                (fread(signature, 1, DCM_MagicLen, f) != DCM_MagicLen)) {
                ok = OFFalse;
            } else if (strncmp(signature, DCM_Magic, DCM_MagicLen) != 0) {
                ok = OFFalse;
            } else {
                /* looks ok */
                ok = OFTrue;
            }
            fclose(f);
        }
        if (ok) {
            COUT << "yes: " << fname << OFendl;
        } else {
            COUT << "no: " << fname << OFendl;
            badCount++;
        }
    }

    /*
    ** if all files are ok then return zero, otherwise the
    ** number of non-dicom files.
    */
    return badCount;
}

/*
** CVS/RCS Log:
** $Log: dcmftest.cc,v $
** Revision 1.22  2010-10-14 13:13:30  joergr
** Updated copyright header. Added reference to COPYRIGHT file.
**
** Revision 1.21  2009-11-04 09:58:06  uli
** Switched to logging mechanism provided by the "new" oflog module
**
** Revision 1.20  2006-08-15 15:50:56  meichel
** Updated all code in module dcmdata to correctly compile when
**   all standard C++ classes remain in namespace std.
**
** Revision 1.19  2006/07/27 13:34:18  joergr
** Prepended prefix "PF_" to parseLine() flags.
**
** Revision 1.18  2005/12/08 15:40:47  meichel
** Changed include path schema for all DCMTK header files
**
** Revision 1.17  2004/08/03 11:41:04  meichel
** Headers libc.h and unistd.h are now included via ofstdinc.h
**
** Revision 1.16  2003/11/13 10:53:26  joergr
** Made help text consistent with revised man page. Replaced tabs by spaces.
**
** Revision 1.15  2002/11/27 12:07:17  meichel
** Adapted module dcmdata to use of new header file ofstdinc.h
**
** Revision 1.14  2001/06/01 15:48:28  meichel
** Updated copyright header
**
** Revision 1.13  2000/03/08 16:26:05  meichel
** Updated copyright header.
**
** Revision 1.12  2000/03/03 14:05:15  meichel
** Implemented library support for redirecting error messages into memory
**   instead of printing them to stdout/stderr for GUI applications.
**
** Revision 1.11  2000/02/23 15:11:32  meichel
** Corrected macro for Borland C++ Builder 4 workaround.
**
** Revision 1.10  2000/02/01 10:11:57  meichel
** Avoiding to include <stdlib.h> as extern "C" on Borland C++ Builder 4,
**   workaround for bug in compiler header files.
**
** Revision 1.9  1999/04/27 17:50:50  joergr
** Adapted console applications to new OFCommandLine and OFConsoleApplication
** functionality.
**
** Revision 1.8  1999/04/27 12:23:24  meichel
** Prevented dcmdata applications from opening a file with empty filename,
**   leads to application crash on Win32.
**
** Revision 1.7  1999/03/31 09:24:19  meichel
** Updated copyright header in module dcmdata
**
** Revision 1.6  1999/03/29 10:14:13  meichel
** Adapted command line options of dcmdata applications to new scheme.
**
** Revision 1.5  1997/08/06 12:20:01  andreas
** - Using Windows NT with Visual C++ 4.x the standard open mode for files
**   is TEXT with conversions. For binary files (image files, imagectn database
**   index) this must be changed (e.g. fopen(filename, "...b"); or
**   open(filename, ..... |O_BINARY);)
**
** Revision 1.4  1997/07/21 08:25:00  andreas
** - Replace all boolean types (BOOLEAN, CTNBOOLEAN, DICOM_BOOL, BOOL)
**   with one unique boolean type OFBool.
**
** Revision 1.3  1997/05/29 17:16:38  meichel
** dcmftest.cc now includes <unistd.h> if available. Needed for SunOS.
**
** Revision 1.2  1997/05/29 15:52:51  meichel
** Added constant for dcmtk release date in dcuid.h.
** All dcmtk applications now contain a version string
** which is displayed with the command line options ("usage" message)
** and which can be queried in the binary with the "ident" command.
**
** Revision 1.1  1997/05/06 11:53:07  hewett
** Added program (dcmftest) to test if a file uses the DICOM Part 10
** Format.  The program terminates with exit code 0 if file looks
** Part 10 conform.  This program is useful in Unix shell scripts.
**
*/
