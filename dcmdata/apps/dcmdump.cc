/*
**
** Author: Gerd Ehlers      Created:  16-04-94
**         Andrew Hewett    29-10-95 - Adapted for Loadable Data Dictionary
**
** Module: dcmdump.cc
**
** Purpose:
** List the contents of a dicom file to stdout
**
**
** Last Update:		$Author: hewett $
** Update Date:		$Date: 1996-03-12 15:11:38 $
** Source File:		$Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/apps/dcmdump.cc,v $
** CVS/RCS Revision:	$Revision: 1.4 $
** Status:		$State: Exp $
**
** CVS/RCS Log at end of file
**
*/

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif
#include <iostream.h>
#include <string.h>
#include "dctk.h"
#include "dcdebug.h"
#include "cmdlnarg.h"

static int dumpFile(const char* fname);

int main(int argc, char *argv[])
{
#ifdef HAVE_LIBIOSTREAM
    cin.sync_with_stdio();
    cout.sync_with_stdio();
    cerr.sync_with_stdio();
#endif
    SetDebugLevel(( 0 ));

    prepareCmdLineArgs(argc, argv);
    
    /* parse cmd line */
    if (argc < 2) {
        fprintf(stderr, "usage: dcmdump dicom-file-format ...\n");
        return 0;
    }

    /* make sure data dictionary is loaded */
    if (dcmDataDict.numberOfEntries() == 0) {
	fprintf(stderr, "Warning: no data dictionary loaded, check environment variable: %s\n",
		DCM_DICT_ENVIRONMENT_VARIABLE);
    }

    for (int i=1; i<argc; i++) {
	dumpFile(argv[i]);
    }

    return 0;
}

static int dumpFile(const char* ifname)
{
    DcmFileStream myin(ifname, DCM_ReadMode);
    if ( myin.GetError() != EC_Normal ) {
        fprintf(stderr, "dcmdump: cannot open file: %s\n", ifname);
        return 1;
    }

    DcmFileFormat dfile;
    dfile.transferInit();
    dfile.read(myin, EXS_Unknown, EGL_withGL );
    dfile.transferEnd();

    if (dfile.error() != EC_Normal) {
	fprintf(stderr, "dcmdump: error: %s: reading file: %s\n", 
		dcmErrorConditionToString(dfile.error()), ifname);
    }

    dfile.print();

    return 0;
}


/*
** CVS/RCS Log:
** $Log: dcmdump.cc,v $
** Revision 1.4  1996-03-12 15:11:38  hewett
** Added call to prepareCmdLineArgs to enable command line arguments
** in environments which do not provide them.
**
** Revision 1.3  1996/01/05 13:29:34  andreas
** - new streaming facilities
** - unique read/write methods for block and file transfer
** - more cleanups
**
** Revision 1.2  1995/11/23 17:10:31  hewett
** Updated for loadable data dictionary.
**
**
*/
