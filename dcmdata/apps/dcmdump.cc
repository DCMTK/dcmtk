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
** Update Date:		$Date: 1997-04-18 08:04:47 $
** Source File:		$Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/apps/dcmdump.cc,v $
** CVS/RCS Revision:	$Revision: 1.9 $
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

#ifdef HAVE_GUSI_H
    /* needed for Macintosh */
#include <GUSI.h>
#include <SIOUX.h>
#endif

static int dumpFile(ostream & out,
		    const char* ifname, const BOOL isDataset, 
		    const E_TransferSyntax xfer,
		    const BOOL showFullData,
		    const BOOL loadMode);

// ********************************************

static BOOL printAllInstances = TRUE;
static BOOL prependSequenceHierarchy = FALSE;
static int printTagCount = 0;
static const int MAX_PRINT_TAG_NAMES = 1024;
static const char* printTagNames[MAX_PRINT_TAG_NAMES];
static const DcmDictEntry* printTagDictEntries[MAX_PRINT_TAG_NAMES];

static BOOL addPrintTagName(const char* tagName)
{
    if (printTagCount >= MAX_PRINT_TAG_NAMES) {
	fprintf(stderr, "error: too many print Tag options (max: %d)\n",
		MAX_PRINT_TAG_NAMES);
	return FALSE;
    }

    int group = 0xffff;
    int elem = 0xffff;
    if (sscanf( tagName, "%x,%x", &group, &elem ) != 2 ) {
	/* it is a name */
	const DcmDictEntry *dicent = dcmDataDict.findEntry(tagName);
	if( dicent == NULL ) {
	    fprintf(stderr, "error: unrecognised tag name: '%s'\n", tagName);
	    return FALSE;
	} else {
	    /* note for later */
	    printTagDictEntries[printTagCount] = dicent;
	}
    } else {
	/* tag name has format xxxx,xxxx */
	/* do not lookup in dictionary, tag could be private */
	printTagDictEntries[printTagCount] = NULL;
    }

    printTagNames[printTagCount] = strcpy((char*)malloc(strlen(tagName)+1),tagName);
    printTagCount++;
    return TRUE;
}

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
           "    +L    print long tag values (default)\n"
           "    -L    do not print long tag values\n"
           "    +P tag    print all encountered instances of \"tag\" (where\n"
	   "              tag is \"xxxx,xxxx\" or a data dictionary name)\n"
	   "              this option can be specified multiple times\n"
	   "              default: the complete file is printed\n"
           "    -P tag    only print first instance of \"tag\" (where\n"
	   "              tag is \"xxxx,xxxx\" or a data dictionary name)\n"
	   "              this option can be specified multiple times\n"
	   "              default: the complete file is printed\n"
	   "    +p    prepend sequence hierarchy to printed tag instance\n"
	   "          (only meaningful in conjunction with the +P or -P\n"
	   "          options), denoted by: (xxxx,xxxx).(xxxx,xxxx).*\n"
	   "    -p    do not prepend hierarchy to tag instance (default)\n"
           "  input options:\n"
           "    +D    read as a dataset\n"
           "    -D    read as a fileformat or dataset (default)\n"
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

#ifdef HAVE_GUSI_H
    /* needed for Macintosh */
    /* set options for the Metrowerks CodeWarrior SIOUX console */
    SIOUXSettings.autocloseonquit = FALSE;
    SIOUXSettings.asktosaveonclose = FALSE;
    SIOUXSettings.showstatusline = TRUE;
    SIOUXSettings.setupmenus = TRUE;
    /* set options for the GUSI sockets library */
    GUSISetup(GUSIwithSIOUXSockets);
    GUSISetup(GUSIwithInternetSockets);
#endif

    SetDebugLevel(( 0 ));

    prepareCmdLineArgs(argc, argv, "dcmdump");
    
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
	    case 'P':
		if (arg[0] == '+' && arg[2] == '\0') 
		    printAllInstances = TRUE;
		else if (arg[0] == '-' && arg[2] == '\0') 
		    printAllInstances = FALSE;
		else {
		    fprintf(stderr, "unknown option: %s\n", arg);
		    return 1;
		}
		if (!addPrintTagName(argv[i+1])) {
		    usage();
		    return 1;
		}
		i++; /* eat the elem name argument */
		break;
	    case 'p':
		if (arg[0] == '+' && arg[2] == '\0') 
		    prependSequenceHierarchy = TRUE;
		else if (arg[0] == '-' && arg[2] == '\0') 
		    prependSequenceHierarchy = FALSE;
		else {
		    fprintf(stderr, "unknown option: %s\n", arg);
		    return 1;
		}
		break;
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

static void printResult(ostream& out, DcmStack& stack, BOOL showFullData)
{
    unsigned long n = stack.card();
    if (n == 0) {
	return;
    }

    if (prependSequenceHierarchy) {
	/* print the path leading up to the top stack elem */
	for (unsigned long i=n-1; i>=1; i--) {
	    DcmObject *dobj = stack.elem(i);
	    /* do not print if a DCM_Item as this is not 
	     * very helpful to distinguish instances.
	     */
	    if (dobj != NULL && dobj->getTag().getXTag() != DCM_Item) {
		char buf[128];
		sprintf(buf, "(%x,%x).", 
			(unsigned)dobj->getGTag(), 
			(unsigned)dobj->getETag());
		out << buf;
	    }
	}
    }

    /* print the tag and its value */
    DcmObject *dobj = stack.top();
    dobj->print(out, showFullData);
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
    
    if (printTagCount == 0) {
	/* print everything */
	dfile->print(out, showFullData);
    } else {
	/* only print specified tags */
	for (int i=0; i<printTagCount; i++) {
	    int group = 0xffff;
	    int elem = 0xffff;
	    DcmTagKey searchKey;
	    const char* tagName = printTagNames[i];
	    const DcmDictEntry* dictEntry = printTagDictEntries[i];

	    if (dictEntry != NULL) {
		/* we have already done a lookup */
		searchKey = dictEntry->getKey();
	    } else if (sscanf( tagName, "%x,%x", &group, &elem ) == 2 ) {
		searchKey.set(group, elem);
	    } else {
		fprintf(stderr, "Internal ERROR in File %s, Line %d\n",
			__FILE__, __LINE__);
		fprintf(stderr, "-- Named tag inconsistency\n");
		abort();
	    }

	    DcmStack stack;
	    if (dfile->search(searchKey, stack, 
			      ESM_fromHere, TRUE) == EC_Normal) {

		printResult(out, stack, showFullData);

		if (printAllInstances) {
		    while (dfile->search(searchKey, stack, 
					 ESM_afterStackTop, TRUE) 
			   == EC_Normal) {
			printResult(out, stack, showFullData);
		    }
		}
	    }

	}

    }

    delete dfile;

    return 0;
}


/*
** CVS/RCS Log:
** $Log: dcmdump.cc,v $
** Revision 1.9  1997-04-18 08:04:47  andreas
** - Minor corrections: correct some warnings of the SUN-C++ Compiler
**   concerning the assignments of wrong types and inline compiler
**   errors
**
** Revision 1.8  1997/02/06 11:19:22  hewett
** Update for CodeWarrior 11 on Macintrosh.  Now explicitly sets flags
** for the SIOUX console.
**
** Revision 1.7  1996/09/24 16:13:50  hewett
** Added preliminary support for the Macintosh environment (GUSI library).
**
** Revision 1.6  1996/09/18 16:34:16  hewett
** Added optional code to the dcmdump program to restrict its
** output to specified named tags.  Based on a suggestion from
** Larry V. Streepy, Jr.  (mailto:streepy@healthcare.com).
**
** Revision 1.5  1996/08/05 08:43:36  andreas
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
