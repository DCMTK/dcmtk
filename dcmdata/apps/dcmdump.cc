#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <iostream.h>
#include "dctk.h"
#include "dcdebug.h"



int main(int argc, char *argv[])
{
    cin.sync_with_stdio();
    cout.sync_with_stdio();
    cerr.sync_with_stdio();
    SetDebugLevel(( 0 ));

    /* parse cmd line */
    if (argc != 2) {
        cerr << "usage: " << argv[0] << " DICOMFormatFile\n";
        return 0;
    }

    char* ifname = argv[1];

    iDicomStream myin( ifname );
    if ( myin.fail() ) {
        cerr << argv[0] << ": cannot open file: " << ifname  << endl;
        return 1;
    }

    DcmFileFormat dfile( &myin );
    dfile.read( EXS_UNKNOWN, EGL_withGL );

    dfile.print();

    return 0;
}

