#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <iostream.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "dctk.h"
#include "dcdebug.h"



int main(int argc, char *argv[])
{
    cin.sync_with_stdio();
    cout.sync_with_stdio();
    cerr.sync_with_stdio();
    SetDebugLevel(( 0 ));

    /* parse cmd line */
    if (argc != 3) {
        cerr << "usage: " << argv[0] << " DICOMFormatFile DataSetFile\n";
        exit(0);
    }

    char* ifname = argv[1];
    char* ofname = argv[2];

    iDicomStream myin( ifname );
    if ( myin.fail() ) {
        cerr << argv[0] << ": cannot open file: " << ifname  << endl;
        return 1;
    }

    DcmFileFormat dfile( &myin );
    dfile.read();

    oDicomStream myout( ofname );
    if ( myout.fail() ) {
        cerr << argv[0] << ": cannot create file: " << ofname  << endl;
        return 1;
    }

    DcmDataset *dset = dfile.getDataset();
    dset->write( myout, EXS_LittleEndianImplicit, EET_ExplicitLength );

    return 0;
}

