/*
**
** Author: Andrew Hewett    02-04-97
**         
**
** Module: dcmftest.cc
**
** Purpose:
** Test if a file uses DICOM Part 10 format.
**
**
** Last Update:		$Author: andreas $
** Update Date:		$Date: 1997-08-06 12:20:01 $
** Source File:		$Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/apps/dcmftest.cc,v $
** CVS/RCS Revision:	$Revision: 1.5 $
** Status:		$State: Exp $
**
** CVS/RCS Log at end of file
**
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
#include "dcuid.h"    /* for dcmtk version name */

static char rcsid[] = "$dcmtk: dcmftest v"
  OFFIS_DCMTK_VERSION " " OFFIS_DCMTK_RELEASEDATE " $";

// ********************************************

static void
usage()
{
    fprintf(stderr, 
"%s\n\n" 
"dcmftest: test if file uses dicom part 10 format\n"
"usage: dcmftest file ...\n", rcsid);
}

int main(int argc, char *argv[])
{
    /* parse cmd line */
    if (argc < 2) {
        usage();
        return 0;
    }

    int badCount = 0;
    for (int i=1; i<argc; i++) {
	char* fname = argv[i];
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
** Revision 1.5  1997-08-06 12:20:01  andreas
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
