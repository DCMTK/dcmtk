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
** Update Date:		$Date: 1995-11-23 17:10:31 $
** Source File:		$Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/apps/dcmdump.cc,v $
** CVS/RCS Revision:	$Revision: 1.2 $
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


#ifdef HAVE_EMPTY_ARGC_ARGV
static void prepareCmdLineArgs(int& argc, char* argv[])
{
    char buf[2024];

    argv[0] = "dcmdump";
    argc = 1;
	
    cout << "CmdLineArgs-> ";
    cin >> buf;
    argv[1] = new char[strlen(buf)+1];
    strcpy(argv[1], buf);
    argc++;
	
}
#else
static void prepareCmdLineArgs(int& /* argc */, char** /* argv */)
{
    /* do nothing */
}
#endif


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
    if (argc != 2) {
        fprintf(stderr, "usage: dcmdump dicom-file-format\n");
        return 0;
    }

    /* make sure data dictionary is loaded */
    if (dcmDataDict.numberOfEntries() == 0) {
	fprintf(stderr, "Warning: no data dictionary loaded, check environment variable: %s\n",
		DCM_DICT_ENVIRONMENT_VARIABLE);
    }

    char* ifname = argv[1];

    iDicomStream myin( ifname );
    if ( myin.fail() ) {
        fprintf(stderr, "dcmdump: cannot open file: %s\n", ifname);
        return 1;
    }

    DcmFileFormat dfile( &myin );
    dfile.read( EXS_UNKNOWN, EGL_withGL );

    if (dfile.error() != EC_Normal) {
	fprintf(stderr, "Error: %s: reading file: %s\n", 
		dcmErrorConditionToString(dfile.error()), ifname);
	return 1;
    }

    dfile.print();

    return 0;
}

/*
** CVS/RCS Log:
** $Log: dcmdump.cc,v $
** Revision 1.2  1995-11-23 17:10:31  hewett
** Updated for loadable data dictionary.
**
**
*/
