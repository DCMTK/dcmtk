/*
 *
 *  Copyright (C) 1994-99, OFFIS
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
 *  Module:  dcmdata
 *
 *  Author:  Andrew Hewett
 *
 *  Purpose: Test if a file uses DICOM Part 10 format.
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 1999-03-31 09:24:19 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/apps/dcmftest.cc,v $
 *  CVS/RCS Revision: $Revision: 1.7 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include <stdio.h>
#include <string.h>

#ifdef HAVE_LIBC_H
#include <libc.h>
#endif
#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#include "dcmetinf.h"
#include "ofconapp.h"
#include "dcuid.h"    /* for dcmtk version name */

#define OFFIS_CONSOLE_APPLICATION "dcmftest"

static char rcsid[] = "$dcmtk: " OFFIS_CONSOLE_APPLICATION " v"
  OFFIS_DCMTK_VERSION " " OFFIS_DCMTK_RELEASEDATE " $";

// ********************************************

int main(int argc, char *argv[])
{
    OFConsoleApplication app(OFFIS_CONSOLE_APPLICATION , "test if file uses dicom part 10 format", rcsid);
    OFCommandLine cmd;

    /* evaluate command line */                           
    app.parseCommandLine(cmd, argc, argv, "file ...", 1, -1, OFCommandLine::ExpandWildcards);
	
    int badCount = 0;
    int count = cmd.getParamCount();
    const char *fname = NULL;
    for (int i=1; i<=count; i++) {
        cmd.getParam(i, fname);
	OFBool ok = OFFalse;
	FILE* f = fopen(fname, "rb");

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
	    printf("yes: %s\n", fname);
	} else {
	    printf("no: %s\n", fname);
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
** Revision 1.7  1999-03-31 09:24:19  meichel
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
