// mknulldicomdir1.cc
// Erzeuge ein leeres DICOM-Verzeichnis mit Hilfe der Klasse DcmDicomDir.

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include <iostream.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#ifdef HAVE_GUSI_H
#include <GUSI.h>
#endif

#include "dctk.h"
#include "dcdebug.h"
#include "cmdlnarg.h"

int main(int argc, char *argv[])
{

#ifdef HAVE_GUSI_H
    GUSISetup(GUSIwithSIOUXSockets);
    GUSISetup(GUSIwithInternetSockets);
#endif

    prepareCmdLineArgs(argc, argv, "mknldir");

    SetDebugLevel(( 0 ));

    char* ofname = "DICOMDIR";
    char* fileSetID = "EXAMPLE";
    E_TransferSyntax xfer = EXS_LittleEndianExplicit;
    E_EncodingType   enctype = EET_ExplicitLength;

    /* make sure data dictionary is loaded */
    if (dcmDataDict.numberOfEntries() == 0) {
	cerr << "Warning: no data dictionary loaded, check environment variable: " 
	     << DCM_DICT_ENVIRONMENT_VARIABLE << endl;
	return 1; /* DcmDicomDir class dumps core when no data dictionary */
    }

    /* parse cmd line */
    if (argc != 3) {
        cerr << "usage: " << argv[0] << " File-set-ID DICOMDIR\n";
        return 0;
    }
 
    fileSetID = argv[1];
    ofname = argv[2];

    /* create a minimal DICOMDIR data set */
    DcmDicomDir *dir = new DcmDicomDir( ofname, fileSetID );
    dir->write( xfer, enctype );
    delete dir;

    return 0;
}

