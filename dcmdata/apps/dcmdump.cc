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
** Last Update:		$Author: andreas $
** Update Date:		$Date: 1996-08-05 08:43:36 $
** Source File:		$Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/apps/dcmdump.cc,v $
** CVS/RCS Revision:	$Revision: 1.5 $
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

static int dumpFile(ostream & out,
		    const char* ifname, const BOOL isDataset, 
		    const E_TransferSyntax xfer,
		    const BOOL showFullData,
		    const BOOL loadMode);

// ********************************************

static void
usage()
{
    fprintf(stderr, 
	   "dcmdump: dump dicom file and data set\n"
	   "usage: dcmdump [options] dcmfile-in [options] dcmfile-in\n"
	   "Options are valid if specified before filename.\n"
	   "options are:\n"
	   "  output option\n"
           "    +E    print to stderr\n"
	   "    -E    print to stdout (default)\n"
           "    +L    print long element values (default)\n"
           "    -L    do not print long element values\n"
           "  input option:\n"
           "    +D    print dataset\n"
           "    -D    print fileformat or dataset (default)\n"
	   "  input transfer syntax: use only after +D\n"
	   "    +t=   use transfer syntax recognition (default)\n"
	   "    +ti   read with little-endian implicit transfer syntax\n"
	   "    +te   read with little-endian explicit transfer syntax\n"
	   "    +tb   read with big-endian explicit transfer syntax\n"
	   "  other test/debug options:\n"
	   "    +l    force load of all input data into memory\n");
}

int main(int argc, char *argv[])
{
    BOOL showFullData = TRUE;
    BOOL loadMode = FALSE;
    BOOL isDataset = FALSE;
    BOOL perr = FALSE;
    E_TransferSyntax xfer = EXS_Unknown;
#ifdef HAVE_LIBIOSTREAM
    cin.sync_with_stdio();
    cout.sync_with_stdio();
    cerr.sync_with_stdio();
#endif
    SetDebugLevel(( 0 ));

    prepareCmdLineArgs(argc, argv);
    
    if (argc < 2) {
	usage();
        return 1;
    }

    /* make sure data dictionary is loaded */
    if (dcmDataDict.numberOfEntries() == 0) {
	fprintf(stderr, "Warning: no data dictionary loaded, check environment variable: %s\n",
		DCM_DICT_ENVIRONMENT_VARIABLE);
    }
    /* parse cmd line */
    for (int i=1; i<argc; i++) {
	char* arg = argv[i];
	if (arg[0] == '-' || arg[0] == '+') {
	    if (strlen(arg) < 2) {
		fprintf(stderr, "unknown argument: %s\n", arg);
		usage();
		return 1;
	    }
	    switch (arg[1]) {
	    case 'D':
		if (arg[0] == '+' && arg[2] == '\0')
		    isDataset = TRUE;
		else if (arg[0] == '-' && arg[2] == '\0') 
		    isDataset = FALSE;
		else {
		    fprintf(stderr, "unknown option: %s\n", arg);
		    return 1;
		}
		break;
	    case 'E':
		if (arg[0] == '+' && arg[2] == '\0')
		    perr = TRUE;
		else if (arg[0] == '-' && arg[2] == '\0') 
		    perr = FALSE;
		else {
		    fprintf(stderr, "unknown option: %s\n", arg);
		    return 1;
		}
		break;
	    case 'L':
		if (arg[0] == '+' && arg[2] == '\0') 
		    showFullData = TRUE;
		else if (arg[0] == '-' && arg[2] == '\0') 
		    showFullData = FALSE;
		else {
		    fprintf(stderr, "unknown option: %s\n", arg);
		    return 1;
		}
		break;
	    case 'h':
		usage();
		return 0;
	    case 'l':
		if (arg[0] == '+' && arg[2] == '\0') 
		    loadMode = TRUE;
		else if (arg[0] == '-' && arg[2] == '\0') 
		    loadMode = FALSE;
		else {
		    fprintf(stderr, "unknown option: %s\n", arg);
		    return 1;
		}
		break;
	    case 't':
		if (!isDataset)
		{
		    fprintf(stderr, 
			    "option %s is not allowed for fileformats\n", arg);
		    return 1;
		}
		else
		{
		    switch (arg[2]) {
		    case '=':
			xfer = EXS_Unknown;
			break;
		    case 'i':
			xfer = EXS_LittleEndianImplicit;
			break;
		    case 'e':
			xfer = EXS_LittleEndianExplicit;
			break;
		    case 'b':
			xfer = EXS_BigEndianExplicit;
			break;
		    default:
			fprintf(stderr, "unknown option: %s\n", arg);
			return 1;
		    }
		}
		break;
	    default:
		fprintf(stderr, "unknown option: %s\n", arg);
		return 1;
	    }
	}
	else
	{
	    if (!perr)
		dumpFile(cout, arg, isDataset, xfer, showFullData, loadMode);
	    else
		dumpFile(cerr, arg, isDataset, xfer, showFullData, loadMode);
	}
    }
	    
    return 0;
}


static int dumpFile(ostream & out,
		    const char* ifname, const BOOL isDataset, 
		    const E_TransferSyntax xfer,
		    const BOOL showFullData,
		    const BOOL loadMode)

{
    DcmFileStream myin(ifname, DCM_ReadMode);
    if ( myin.GetError() != EC_Normal ) {
        fprintf(stderr, "dcmdump: cannot open file: %s\n", ifname);
        return 1;
    }

    DcmObject * dfile = NULL;
    if (isDataset)
	dfile = new DcmDataset();
    else
	dfile = new DcmFileFormat();

    dfile->transferInit();
    dfile->read(myin, xfer, EGL_withGL );
    dfile->transferEnd();

    if (dfile->error() != EC_Normal) {
	fprintf(stderr, "dcmdump: error: %s: reading file: %s\n", 
		dcmErrorConditionToString(dfile->error()), ifname);
    }

    if (loadMode) {
        dfile->loadAllDataIntoMemory();
	if (dfile-> error() != EC_Normal)
	{
	    fprintf(stderr, "Error: %s: reading file: %s\n", 
		    dcmErrorConditionToString(dfile->error()), ifname);
	    return 1;
	}
    }
    dfile->print(out, showFullData);

    delete dfile;

    return 0;
}


/*
** CVS/RCS Log:
** $Log: dcmdump.cc,v $
** Revision 1.5  1996-08-05 08:43:36  andreas
** new print routine with additional parameters:
** 	- print into files
** 	- fix output length for elements
**
** Revision 1.4  1996/03/12 15:11:38  hewett
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
