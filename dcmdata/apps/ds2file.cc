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

    char*            ifname = (char*)NULL;
    char*            ofname = (char*)NULL;
    E_EncodingType   enctype = EET_ExplicitLength;
    E_GrpLenEncoding gltype = EGL_withoutGL;
    int              arg_offset = 0;

    /* parse cmd line */
    if ( argc >= 2+arg_offset && argv[1+arg_offset][0] != '-' )
    {
        ifname = argv[1+arg_offset];
        arg_offset++;
    }
    else
        ifname = (char*)NULL;
    if (    argc >= 2+arg_offset
         && strcmp( argv[1+arg_offset], "-UndefinedLength" ) == 0 )
    {
        enctype = EET_UndefinedLength;
        arg_offset++;
    }
    if (    argc >= 2+arg_offset
         && strcmp( argv[1+arg_offset], "-WithGroupLength" ) == 0 )
    {
        gltype = EGL_withGL;
        arg_offset++;
    }
    if ( argc >= 2+arg_offset && argv[1+arg_offset][0] != '-' )
    {
        ofname = argv[1+arg_offset];
        arg_offset++;
    }
    else
        ifname = (char*)NULL;

    if ( ifname == (char*)NULL || ofname == (char*)NULL ) {
        cerr << "usage: "
             << argv[0]
             << " DataSetFile [-UndefinedLength] [-WithGroupLength]"
                " DICOMFormatFile\n";
        return 0;
    }

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

    dfile.write( myout,
                 dfile.getDataset()->getOriginalXfer(),
                 enctype,
                 gltype );

    return 0;
}

