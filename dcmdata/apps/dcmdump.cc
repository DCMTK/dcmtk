/*
**
** Author: Gerd Ehlers      Created:  16-04-94
**
** Module: dcmdump.cc
**
** Purpose:
** List the contents of a dicom file to stdout
**
**
** Last Update:		$Author: meichel $
** Update Date:		$Date: 1999-03-29 10:14:12 $
** Source File:		$Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/apps/dcmdump.cc,v $
** CVS/RCS Revision:	$Revision: 1.19 $
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
#include "ofconapp.h"
#include "dcuid.h"    /* for dcmtk version name */

#define OFFIS_CONSOLE_APPLICATION "dcmdump"

static char rcsid[] = "$dcmtk: " OFFIS_CONSOLE_APPLICATION " v"
  OFFIS_DCMTK_VERSION " " OFFIS_DCMTK_RELEASEDATE " $";

#ifdef HAVE_GUSI_H
    /* needed for Macintosh */
#include <GUSI.h>
#include <SIOUX.h>
#endif

static int dumpFile(ostream & out,
		    const char* ifname, const OFBool isDataset, 
		    const E_TransferSyntax xfer,
		    const OFBool showFullData,
		    const OFBool loadIntoMemory,
		    const OFBool stopOnErrors);

// ********************************************

static OFBool printAllInstances = OFTrue;
static OFBool prependSequenceHierarchy = OFFalse;
static int printTagCount = 0;
static const int MAX_PRINT_TAG_NAMES = 1024;
static const char* printTagNames[MAX_PRINT_TAG_NAMES];
static const DcmDictEntry* printTagDictEntries[MAX_PRINT_TAG_NAMES];

static OFBool addPrintTagName(const char* tagName)
{
    if (printTagCount >= MAX_PRINT_TAG_NAMES) {
	cerr << "error: too many print Tag options (max: " << 
	    MAX_PRINT_TAG_NAMES << ")\n";
	return OFFalse;
    }

    int group = 0xffff;
    int elem = 0xffff;
    if (sscanf( tagName, "%x,%x", &group, &elem ) != 2 ) {
	/* it is a name */
	const DcmDictEntry *dicent = dcmDataDict.findEntry(tagName);
	if( dicent == NULL ) {
	    cerr << "error: unrecognised tag name: '" << tagName << "'\n";
	    return OFFalse;
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
    return OFTrue;
}

#define SHORTCOL 3
#define LONGCOL 20

int main(int argc, char *argv[])
{
    OFBool loadIntoMemory = OFTrue;
    OFBool showFullData = OFFalse;
    OFBool isDataset = OFFalse;
    E_TransferSyntax xfer = EXS_Unknown;
    OFBool stopOnErrors = OFTrue;
    const char *current = NULL;
    
#ifdef HAVE_GUSI_H
    /* needed for Macintosh */
    /* set options for the Metrowerks CodeWarrior SIOUX console */
    SIOUXSettings.autocloseonquit = OFFalse;
    SIOUXSettings.asktosaveonclose = OFFalse;
    SIOUXSettings.showstatusline = OFTrue;
    SIOUXSettings.setupmenus = OFTrue;
    /* set options for the GUSI sockets library */
    GUSISetup(GUSIwithSIOUXSockets);
    GUSISetup(GUSIwithInternetSockets);
#endif

    SetDebugLevel(0);

  OFConsoleApplication app(OFFIS_CONSOLE_APPLICATION , "Dump DICOM file and data set", rcsid);
  OFCommandLine cmd;
  
  cmd.addGroup("general options:", LONGCOL, SHORTCOL+2);
   cmd.addOption("--help",                      "-h",        "print this help text and exit");
   cmd.addOption("--debug",                     "-d",        "debug mode, print debug information");
 
  cmd.addGroup("input options:");
    cmd.addSubGroup("input file format:", LONGCOL, SHORTCOL);
      cmd.addOption("--read-file",              "+f",        "read file format or data set (default)");
      cmd.addOption("--read-dataset",           "-f",        "read data set without file meta information");
    cmd.addSubGroup("input transfer syntax (only with --read-dataset):", LONGCOL, SHORTCOL);
     cmd.addOption("--read-xfer-auto",          "-t=",       "use TS recognition (default)");
     cmd.addOption("--read-xfer-little",        "-te",       "read with explicit VR little endian TS");
     cmd.addOption("--read-xfer-big",           "-tb",       "read with explicit VR big endian TS");
     cmd.addOption("--read-xfer-implicit",      "-ti",       "read with implicit VR little endian TS");

  cmd.addGroup("output options:");
    cmd.addSubGroup("printing:", LONGCOL, SHORTCOL);
      cmd.addOption("--load-all",               "+M",        "load very long tag values (default)");
      cmd.addOption("--load-short",             "-M",        "do not load very long values (e.g. pixel data)");
      cmd.addOption("--print-all",              "+L",        "print long tag values completely");
      cmd.addOption("--print-short",            "-L",        "print long tag values shortened (default)");

    cmd.addSubGroup("error handling:", LONGCOL, SHORTCOL);
      cmd.addOption("--stop-on-error",          "-E",        "do not print if file is damaged (default)");
      cmd.addOption("--ignore-errors",          "+E",        "attempt to print even if file is damaged");

    cmd.addSubGroup("searching:", LONGCOL, SHORTCOL);
      cmd.addOption("--search",                 "+P",    1,  "[t]ag: \"xxxx,xxxx\" or a data dictionary name", "print the value of tag t\nthis option can be specified multiple times\n(default: the complete file is printed)");

      cmd.addOption("--search-all",             "+s",        "print all instances of searched tags (default)");
      cmd.addOption("--search-first",           "-s",        "only print first instance of searched tags");
 
      cmd.addOption("--prepend",               "+p",         "prepend sequence hierarchy to printed tag,\ndenoted by: (xxxx,xxxx).(xxxx,xxxx).*\n(only with --search-all or --search-first)");
      cmd.addOption("--no-prepend",            "-p",         "do not prepend hierarchy to tag (default)");

    /* evaluate command line */                           
    prepareCmdLineArgs(argc, argv, OFFIS_CONSOLE_APPLICATION);
    if (app.parseCommandLine(cmd, argc, argv, "dcmfile-in dcmfile-in...", 1, -1, OFCommandLine::ExpandWildcards))
    {
      if (cmd.findOption("--help")) app.printUsage(OFFIS_CONSOLE_APPLICATION);
      if (cmd.findOption("--debug")) SetDebugLevel(5);
      
      cmd.beginOptionBlock();
      if (cmd.findOption("--read-file")) isDataset = OFFalse;
      if (cmd.findOption("--read-dataset")) isDataset = OFTrue;
      cmd.endOptionBlock();

      cmd.beginOptionBlock();
      if (cmd.findOption("--read-xfer-auto"))
      {
      	if (! isDataset) app.printError("--read-xfer-auto only allowed with --read-dataset");
      	xfer = EXS_Unknown;
      }
      if (cmd.findOption("--read-xfer-little"))
      {
      	if (! isDataset) app.printError("--read-xfer-little only allowed with --read-dataset");
      	xfer = EXS_LittleEndianExplicit;
      }
      if (cmd.findOption("--read-xfer-big"))
      {
      	if (! isDataset) app.printError("--read-xfer-big only allowed with --read-dataset");
      	xfer = EXS_BigEndianExplicit;
      }
      if (cmd.findOption("--read-xfer-implicit"))
      {
      	if (! isDataset) app.printError("--read-xfer-implicit only allowed with --read-dataset");
      	xfer = EXS_LittleEndianImplicit;
      }
      cmd.endOptionBlock();

      cmd.beginOptionBlock();
      if (cmd.findOption("--load-all")) loadIntoMemory = OFTrue;
      if (cmd.findOption("--load-short")) loadIntoMemory = OFFalse;
      cmd.endOptionBlock();

      cmd.beginOptionBlock();
      if (cmd.findOption("--print-all")) showFullData = OFTrue;
      if (cmd.findOption("--print-short")) showFullData = OFFalse;
      cmd.endOptionBlock();

      cmd.beginOptionBlock();
      if (cmd.findOption("--stop-on-error")) stopOnErrors = OFTrue;
      if (cmd.findOption("--ignore-errors")) stopOnErrors = OFFalse;
      cmd.endOptionBlock();

      if (cmd.findOption("--search", 0, OFCommandLine::FOM_First))
      {
        do
        {
          app.checkValue(cmd.getValue(current));
          if (!addPrintTagName(current)) return 1;
        } while (cmd.findOption("--search", 0, OFCommandLine::FOM_Next));
      }

      cmd.beginOptionBlock();
      if (cmd.findOption("--search-all"))
      {
      	if (printTagCount==0) app.printError("--search-all only allowed with --search");
      	printAllInstances = OFTrue;
      }
      if (cmd.findOption("--search-first"))
      {
      	if (printTagCount==0) app.printError("--search-first only allowed with --search");
      	printAllInstances = OFFalse;
      }
      cmd.endOptionBlock();

      cmd.beginOptionBlock();
      if (cmd.findOption("--prepend"))
      {
      	if (printTagCount==0) app.printError("--prepend only allowed with --search");
      	printAllInstances = OFTrue;
      }
      if (cmd.findOption("--no-prepend"))
      {
      	if (printTagCount==0) app.printError("--no-prepend only allowed with --search");
      	printAllInstances = OFFalse;
      }
      cmd.endOptionBlock();

    }

    /* make sure data dictionary is loaded */
    if (!dcmDataDict.isDictionaryLoaded())
    {
	cerr << "Warning: no data dictionary loaded, "
	     << "check environment variable: "
	     << DCM_DICT_ENVIRONMENT_VARIABLE;
    }
    
    int errorCount = 0;
    int count = cmd.getParamCount();
    for (int i=1; i<=count; i++) 
    {
      cmd.getParam(i, current);
      errorCount += dumpFile(cout, current, isDataset, xfer, showFullData, loadIntoMemory, stopOnErrors);
    }
        
    return errorCount;
}

static void printResult(ostream& out, DcmStack& stack, OFBool showFullData)
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
		    const char* ifname, const OFBool isDataset, 
		    const E_TransferSyntax xfer,
		    const OFBool showFullData,
		    const OFBool loadIntoMemory,
		    const OFBool stopOnErrors)
{
    int result = 0;
    
    DcmFileStream myin(ifname, DCM_ReadMode);
    if ( myin.GetError() != EC_Normal ) {
        cerr << "dcmdump: cannot open file: " << ifname << endl;
        return 1;
    }

    DcmObject * dfile = NULL;
    if (isDataset) dfile = new DcmDataset(); else dfile = new DcmFileFormat();

    dfile->transferInit();
    dfile->read(myin, xfer, EGL_noChange);
    dfile->transferEnd();

    if (dfile->error() != EC_Normal)
    {
	cerr << "dcmdump: error: " << dcmErrorConditionToString(dfile->error()) 
	     << ": reading file: "<< ifname << endl;
	
	result = 1;
	if (stopOnErrors) return result;
    }

    if (loadIntoMemory) dfile->loadAllDataIntoMemory();

    if (printTagCount == 0) dfile->print(out, showFullData);
    else {
	/* only print specified tags */
	for (int i=0; i<printTagCount; i++)
	{
	    int group = 0xffff;
	    int elem = 0xffff;
	    DcmTagKey searchKey;
	    const char* tagName = printTagNames[i];
	    const DcmDictEntry* dictEntry = printTagDictEntries[i];

	    if (dictEntry != NULL) searchKey = dictEntry->getKey();
	    else if (sscanf( tagName, "%x,%x", &group, &elem ) == 2 ) searchKey.set(group, elem);
            else {
		cerr << "Internal ERROR in File " << __FILE__ << ", Line "
		     << __LINE__ << endl 
		     << "-- Named tag inconsistency\n";
		abort();
	    }

	    DcmStack stack;
	    if (dfile->search(searchKey, stack, ESM_fromHere, OFTrue) == EC_Normal)
	    {
		printResult(out, stack, showFullData);
		if (printAllInstances)
		{
		    while (dfile->search(searchKey, stack, ESM_afterStackTop, OFTrue)  == EC_Normal) 
		      printResult(out, stack, showFullData);
		}
	    }
	}
    }

    delete dfile;
    return result;
}


/*
** CVS/RCS Log:
** $Log: dcmdump.cc,v $
** Revision 1.19  1999-03-29 10:14:12  meichel
** Adapted command line options of dcmdata applications to new scheme.
**
** Revision 1.18  1999/03/22 16:12:16  meichel
** Added -d <debuglevel> flag to dcmdump.
**
** Revision 1.17  1997/07/21 08:04:24  andreas
** - Replace all boolean types (BOOLEAN, CTNBOOLEAN, DICOM_BOOL, BOOL)
**   with one unique boolean type OFBool.
** - With flag DEBUG dcmdump now tries to print the DICOM file even if
**   an error in reading the file was detected.
**
** Revision 1.16  1997/07/03 15:09:38  andreas
** - removed debugging functions Bdebug() and Edebug() since
**   they write a static array and are not very useful at all.
**   Cdebug and Vdebug are merged since they have the same semantics.
**   The debugging functions in dcmdata changed their interfaces
**   (see dcmdata/include/dcdebug.h)
**
** Revision 1.15  1997/05/29 15:52:50  meichel
** Added constant for dcmtk release date in dcuid.h.
** All dcmtk applications now contain a version string
** which is displayed with the command line options ("usage" message)
** and which can be queried in the binary with the "ident" command.
**
** Revision 1.14  1997/05/27 13:46:53  andreas
** - Corrected usage string in dcmdump
**
** Revision 1.13  1997/05/22 16:53:32  andreas
** - Changed default options and documentation for dcmdump.
**
** Revision 1.12  1997/05/22 13:26:23  hewett
** Modified the test for presence of a data dictionary to use the
** method DcmDataDictionary::isDictionaryLoaded().
**
** Revision 1.11  1997/05/20 07:57:11  andreas
** - Removed obsolete applications file2ds and ds2file. The functionality of these
**   applications is now peformed by dcmconv. Unified calling parameters
**   are implemented in dump2dcm, dcmdump and dcmconv.
**
** Revision 1.10  1997/05/16 08:31:04  andreas
** - Revised handling of GroupLength elements and support of
**   DataSetTrailingPadding elements. The enumeratio E_GrpLenEncoding
**   got additional enumeration values (for a description see dctypes.h).
**   addGroupLength and removeGroupLength methods are replaced by
**   computeGroupLengthAndPadding. To support Padding, the parameters of
**   element and sequence write functions changed.
**
** Revision 1.9  1997/04/18 08:04:47  andreas
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
