/*
 *
 *  Copyright (C) 1994-2001, OFFIS
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
 *  Author:  Gerd Ehlers
 *
 *  Purpose: List the contents of a dicom file
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 2001-06-01 15:48:28 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/apps/dcmdump.cc,v $
 *  CVS/RCS Revision: $Revision: 1.32 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#ifdef HAVE_STDLIB_H
#ifndef  _BCB4
/* workaround for bug in Borland C++ Builder 4 */
BEGIN_EXTERN_C
#endif
#include <stdlib.h>
#ifndef  _BCB4
END_EXTERN_C
#endif
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
		    const char *ifname,
		    const OFBool isDataset, 
		    const E_TransferSyntax xfer,
		    const OFBool showFullData,
		    const OFBool loadIntoMemory,
		    const OFBool stopOnErrors,
		    const OFBool writePixelData,
		    const char *pixelDirectory);

// ********************************************

static OFBool printAllInstances = OFTrue;
static OFBool prependSequenceHierarchy = OFFalse;
static int printTagCount = 0;
static const int MAX_PRINT_TAG_NAMES = 1024;
static const char* printTagNames[MAX_PRINT_TAG_NAMES];
static const DcmTagKey* printTagKeys[MAX_PRINT_TAG_NAMES];

static OFBool addPrintTagName(const char* tagName)
{
    if (printTagCount >= MAX_PRINT_TAG_NAMES) {
	CERR << "error: too many print Tag options (max: " << 
	    MAX_PRINT_TAG_NAMES << ")\n";
	return OFFalse;
    }

    int group = 0xffff;
    int elem = 0xffff;
    if (sscanf( tagName, "%x,%x", &group, &elem ) != 2 )
    {
	/* it is a name */
        const DcmDataDictionary& globalDataDict = dcmDataDict.rdlock();
	const DcmDictEntry *dicent = globalDataDict.findEntry(tagName);
	if( dicent == NULL ) {
	    CERR << "error: unrecognised tag name: '" << tagName << "'\n";
 	    dcmDataDict.unlock();
	    return OFFalse;
	} else {
	    /* note for later */
	    printTagKeys[printTagCount] = new DcmTagKey(dicent->getKey());
	}
	dcmDataDict.unlock();
    } else {
	/* tag name has format xxxx,xxxx */
	/* do not lookup in dictionary, tag could be private */
	printTagKeys[printTagCount] = NULL;
    }

    printTagNames[printTagCount] = strcpy((char*)malloc(strlen(tagName)+1),tagName);
    printTagCount++;
    return OFTrue;
}

#define SHORTCOL 3
#define LONGCOL 20

int main(int argc, char *argv[])
{
    int opt_debugMode = 0;
    OFBool loadIntoMemory = OFTrue;
    OFBool showFullData = OFFalse;
    OFBool isDataset = OFFalse;
    OFBool writePixelData = OFFalse;
    E_TransferSyntax xfer = EXS_Unknown;
    OFBool stopOnErrors = OFTrue;
    const char *current = NULL;
    const char *pixelDirectory = NULL;
    
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

    SetDebugLevel(( 0 ));

    OFConsoleApplication app(OFFIS_CONSOLE_APPLICATION, "Dump DICOM file and data set", rcsid);
    OFCommandLine cmd;
    cmd.setOptionColumns(LONGCOL, SHORTCOL);
    cmd.setParamColumn(LONGCOL + SHORTCOL + 4);
    
    cmd.addParam("dcmfile-in", "DICOM input filename to be dumped", OFCmdParam::PM_MultiMandatory);
    
    cmd.addGroup("general options:", LONGCOL, SHORTCOL + 2);
      cmd.addOption("--help",                 "-h",        "print this help text and exit");
      cmd.addOption("--debug",                "-d",        "debug mode, print debug information");
   
    cmd.addGroup("input options:");
      cmd.addSubGroup("input file format:");
        cmd.addOption("--read-file",          "+f",        "read file format or data set (default)");
        cmd.addOption("--read-dataset",       "-f",        "read data set without file meta information");
      cmd.addSubGroup("input transfer syntax (only with --read-dataset):");
        cmd.addOption("--read-xfer-auto",     "-t=",       "use TS recognition (default)");
        cmd.addOption("--read-xfer-little",   "-te",       "read with explicit VR little endian TS");
        cmd.addOption("--read-xfer-big",      "-tb",       "read with explicit VR big endian TS");
        cmd.addOption("--read-xfer-implicit", "-ti",       "read with implicit VR little endian TS");
  
    cmd.addGroup("output options:");
      cmd.addSubGroup("printing:");
        cmd.addOption("--load-all",           "+M",        "load very long tag values (default)");
        cmd.addOption("--load-short",         "-M",        "do not load very long values (e.g. pixel data)");
        cmd.addOption("--print-all",          "+L",        "print long tag values completely");
        cmd.addOption("--print-short",        "-L",        "print long tag values shortened (default)");
  
      cmd.addSubGroup("error handling:");
        cmd.addOption("--stop-on-error",      "-E",        "do not print if file is damaged (default)");
        cmd.addOption("--ignore-errors",      "+E",        "attempt to print even if file is damaged");
  
      cmd.addSubGroup("searching:");
        cmd.addOption("--search",             "+P",    1,  "[t]ag: \"xxxx,xxxx\" or a data dictionary name",
                                                           "print the value of tag t\nthis option can be specified multiple times\n(default: the complete file is printed)");
  
        cmd.addOption("--search-all",         "+s",        "print all instances of searched tags (default)");
        cmd.addOption("--search-first",       "-s",        "only print first instance of searched tags");
   
        cmd.addOption("--prepend",            "+p",        "prepend sequence hierarchy to printed tag,\ndenoted by: (xxxx,xxxx).(xxxx,xxxx).*\n(only with --search-all or --search-first)");
        cmd.addOption("--no-prepend",         "-p",        "do not prepend hierarchy to tag (default)");
  
      cmd.addSubGroup("writing:");
        cmd.addOption("--write-pixel",        "+W",    1,  "[d]irectory : string",
                                                           "write pixel data to a .raw file stored in d\n(little endian, filename created automatically)");

    /* evaluate command line */                           
    prepareCmdLineArgs(argc, argv, OFFIS_CONSOLE_APPLICATION);
    if (app.parseCommandLine(cmd, argc, argv, OFCommandLine::ExpandWildcards))
    {
      if (cmd.findOption("--debug")) opt_debugMode = 5;
      
      cmd.beginOptionBlock();
      if (cmd.findOption("--read-file")) isDataset = OFFalse;
      if (cmd.findOption("--read-dataset")) isDataset = OFTrue;
      cmd.endOptionBlock();

      cmd.beginOptionBlock();
      if (cmd.findOption("--read-xfer-auto"))
      {
      	app.checkDependence("--read-xfer-auto", "--read-dataset", isDataset);
      	xfer = EXS_Unknown;
      }
      if (cmd.findOption("--read-xfer-little"))
      {
        app.checkDependence("--read-xfer-little", "--read-dataset", isDataset);
      	xfer = EXS_LittleEndianExplicit;
      }
      if (cmd.findOption("--read-xfer-big"))
      {
        app.checkDependence("--read-xfer-big", "--read-dataset", isDataset);
      	xfer = EXS_BigEndianExplicit;
      }
      if (cmd.findOption("--read-xfer-implicit"))
      {
        app.checkDependence("--read-xfer-implicit", "--read-dataset", isDataset);
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
        app.checkDependence("--search-all", "--search", printTagCount>0);
      	printAllInstances = OFTrue;
      }
      if (cmd.findOption("--search-first"))
      {
        app.checkDependence("--search-first", "--search", printTagCount>0);
      	printAllInstances = OFFalse;
      }
      cmd.endOptionBlock();

      cmd.beginOptionBlock();
      if (cmd.findOption("--prepend"))
      {
        app.checkDependence("--prepend", "--search", printTagCount>0);
      	printAllInstances = OFTrue;
      }
      if (cmd.findOption("--no-prepend"))
      {
        app.checkDependence("--no-prepend", "--search", printTagCount>0);
      	printAllInstances = OFFalse;
      }
      cmd.endOptionBlock();

      if (cmd.findOption("--write-pixel"))
      {
        app.checkValue(cmd.getValue(pixelDirectory));
        writePixelData = OFTrue;
      }
    }

    SetDebugLevel((opt_debugMode));

    /* make sure data dictionary is loaded */
    if (!dcmDataDict.isDictionaryLoaded())
    {
	CERR << "Warning: no data dictionary loaded, "
	     << "check environment variable: "
	     << DCM_DICT_ENVIRONMENT_VARIABLE;
    }
    
    int errorCount = 0;
    int count = cmd.getParamCount();
    for (int i=1; i<=count; i++) 
    {
      cmd.getParam(i, current);
      errorCount += dumpFile(COUT, current, isDataset, xfer, showFullData, loadIntoMemory, stopOnErrors,
        writePixelData, pixelDirectory);
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
		    const char *ifname,
		    const OFBool isDataset, 
		    const E_TransferSyntax xfer,
		    const OFBool showFullData,
		    const OFBool loadIntoMemory,
		    const OFBool stopOnErrors,
		    const OFBool writePixelData,
		    const char *pixelDirectory)
{
    int result = 0;
    
    if ((ifname == NULL) || (strlen(ifname) == 0))
    {
        CERR << OFFIS_CONSOLE_APPLICATION << ": invalid filename: <empty string>" << endl;
        return 1;
    }

    DcmFileStream myin(ifname, DCM_ReadMode);
    if ( myin.GetError() != EC_Normal ) {
        CERR << OFFIS_CONSOLE_APPLICATION << ": cannot open file: " << ifname << endl;
        return 1;
    }

    DcmObject * dfile = NULL;
    if (isDataset) dfile = new DcmDataset(); else dfile = new DcmFileFormat();

    dfile->transferInit();
    dfile->read(myin, xfer, EGL_noChange);
    dfile->transferEnd();

    if (dfile->error() != EC_Normal)
    {
    	CERR << OFFIS_CONSOLE_APPLICATION << ": error: " << dcmErrorConditionToString(dfile->error()) 
	         << ": reading file: "<< ifname << endl;
	
	    result = 1;
	    if (stopOnErrors) return result;
    }

    if (loadIntoMemory) dfile->loadAllDataIntoMemory();

    if (printTagCount == 0)
    {
        if (writePixelData)
        {
            OFString str = ifname;
            OFString rname = pixelDirectory;
            if ((rname.length() > 0) && (rname[rname.length() - 1] != PATH_SEPARATOR))
             	rname += PATH_SEPARATOR;
            size_t pos = str.find_last_of(PATH_SEPARATOR);
            if (pos == OFString_npos)
            	rname += str;
            else
            	rname += str.substr(pos + 1);
            size_t counter = 0;
            dfile->print(out, showFullData, 0 /*level*/, rname.c_str(), &counter);
        } else
            dfile->print(out, showFullData);
    } else {
    	/* only print specified tags */
    	for (int i=0; i<printTagCount; i++)
    	{
	        int group = 0xffff;
    	    int elem = 0xffff;
    	    DcmTagKey searchKey;
    	    const char* tagName = printTagNames[i];
    	    if (printTagKeys[i]) searchKey = *printTagKeys[i];
            else if (sscanf( tagName, "%x,%x", &group, &elem ) == 2 ) searchKey.set(group, elem);
            else {
        		CERR << "Internal ERROR in File " << __FILE__ << ", Line "
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
 * CVS/RCS Log:
 * $Log: dcmdump.cc,v $
 * Revision 1.32  2001-06-01 15:48:28  meichel
 * Updated copyright header
 *
 * Revision 1.31  2000/06/14 11:26:47  joergr
 * Corrected wrong filename creation for option +W.
 *
 * Revision 1.30  2000/05/03 14:19:05  meichel
 * Added new class GlobalDcmDataDictionary which implements read/write lock
 *   semantics for safe access to the DICOM dictionary from multiple threads
 *   in parallel. The global dcmDataDict now uses this class.
 *
 * Revision 1.29  2000/03/08 16:26:05  meichel
 * Updated copyright header.
 *
 * Revision 1.28  2000/03/06 18:09:37  joergr
 * Avoid empty statement in the body of if-statements (MSVC6 reports warnings).
 *
 * Revision 1.27  2000/03/03 14:05:15  meichel
 * Implemented library support for redirecting error messages into memory
 *   instead of printing them to stdout/stderr for GUI applications.
 *
 * Revision 1.26  2000/02/23 15:11:31  meichel
 * Corrected macro for Borland C++ Builder 4 workaround.
 *
 * Revision 1.25  2000/02/10 11:05:25  joergr
 * Modified help text for new command line option.
 *
 * Revision 1.23  2000/02/01 10:11:57  meichel
 * Avoiding to include <stdlib.h> as extern "C" on Borland C++ Builder 4,
 *   workaround for bug in compiler header files.
 *
 * Revision 1.22  1999/04/27 17:50:50  joergr
 * Adapted console applications to new OFCommandLine and OFConsoleApplication
 * functionality.
 *
 * Revision 1.21  1999/04/26 16:38:25  joergr
 * Removed bug: empty parameters have always been interpreted as options.
 * Added support to check dependences between different options and report
 * error messages if necessary.
 *
 * Revision 1.20  1999/03/31 09:24:19  meichel
 * Updated copyright header in module dcmdata
 *
 * Revision 1.19  1999/03/29 10:14:12  meichel
 * Adapted command line options of dcmdata applications to new scheme.
 *
 * Revision 1.18  1999/03/22 16:12:16  meichel
 * Added -d <debuglevel> flag to dcmdump.
 *
 * Revision 1.17  1997/07/21 08:04:24  andreas
 * - Replace all boolean types (BOOLEAN, CTNBOOLEAN, DICOM_BOOL, BOOL)
 *   with one unique boolean type OFBool.
 * - With flag DEBUG dcmdump now tries to print the DICOM file even if
 *   an error in reading the file was detected.
 *
 * Revision 1.16  1997/07/03 15:09:38  andreas
 * - removed debugging functions Bdebug() and Edebug() since
 *   they write a static array and are not very useful at all.
 *   Cdebug and Vdebug are merged since they have the same semantics.
 *   The debugging functions in dcmdata changed their interfaces
 *   (see dcmdata/include/dcdebug.h)
 *
 * Revision 1.15  1997/05/29 15:52:50  meichel
 * Added constant for dcmtk release date in dcuid.h.
 * All dcmtk applications now contain a version string
 * which is displayed with the command line options ("usage" message)
 * and which can be queried in the binary with the "ident" command.
 *
 * Revision 1.14  1997/05/27 13:46:53  andreas
 * - Corrected usage string in dcmdump
 *
 * Revision 1.13  1997/05/22 16:53:32  andreas
 * - Changed default options and documentation for dcmdump.
 *
 * Revision 1.12  1997/05/22 13:26:23  hewett
 * Modified the test for presence of a data dictionary to use the
 * method DcmDataDictionary::isDictionaryLoaded().
 *
 * Revision 1.11  1997/05/20 07:57:11  andreas
 * - Removed obsolete applications file2ds and ds2file. The functionality of these
 *   applications is now peformed by dcmconv. Unified calling parameters
 *   are implemented in dump2dcm, dcmdump and dcmconv.
 *
 * Revision 1.10  1997/05/16 08:31:04  andreas
 * - Revised handling of GroupLength elements and support of
 *   DataSetTrailingPadding elements. The enumeratio E_GrpLenEncoding
 *   got additional enumeration values (for a description see dctypes.h).
 *   addGroupLength and removeGroupLength methods are replaced by
 *   computeGroupLengthAndPadding. To support Padding, the parameters of
 *   element and sequence write functions changed.
 *
 * Revision 1.9  1997/04/18 08:04:47  andreas
 * - Minor corrections: correct some warnings of the SUN-C++ Compiler
 *   concerning the assignments of wrong types and inline compiler
 *   errors
 *
 * Revision 1.8  1997/02/06 11:19:22  hewett
 * Update for CodeWarrior 11 on Macintrosh.  Now explicitly sets flags
 * for the SIOUX console.
 *
 * Revision 1.7  1996/09/24 16:13:50  hewett
 * Added preliminary support for the Macintosh environment (GUSI library).
 *
 * Revision 1.6  1996/09/18 16:34:16  hewett
 * Added optional code to the dcmdump program to restrict its
 * output to specified named tags.  Based on a suggestion from
 * Larry V. Streepy, Jr.  (mailto:streepy@healthcare.com).
 *
 * Revision 1.5  1996/08/05 08:43:36  andreas
 * new print routine with additional parameters:
 * 	- print into files
 * 	- fix output length for elements
 *
 * Revision 1.4  1996/03/12 15:11:38  hewett
 * Added call to prepareCmdLineArgs to enable command line arguments
 * in environments which do not provide them.
 *
 * Revision 1.3  1996/01/05 13:29:34  andreas
 * - new streaming facilities
 * - unique read/write methods for block and file transfer
 * - more cleanups
 *
 * Revision 1.2  1995/11/23 17:10:31  hewett
 * Updated for loadable data dictionary.
 *
 */
