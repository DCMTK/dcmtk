// bspblock.cc    05-23-94  until  02-15-95

#include <stdio.h>
#include <sys/stat.h>
#include <stream.h>
#include "dctk.h"
#include "dcdebug.h"


BOOL readObject( DcmDataset **pobj, const char *filename )
{
    struct stat file_stat;
    stat( filename, &file_stat );
    Uint32 filelen = file_stat.st_size;

    const Uint32 packetlen = 200;

    DcmFileStream buf(filename, DCM_ReadMode);
    if ( buf.Fail() ) {
        fprintf(stderr, "cannot open file: %s\n", filename);
        return FALSE;
    }

    DcmBufferStream *myin = new DcmBufferStream(DCM_ReadMode);
    *pobj = new DcmDataset();

    char buffer[ packetlen ];
    BOOL generateMoreData = TRUE;
    Uint32 actlen = packetlen;
    Uint32 eofCounter = 0;

    while ( 1 )
    {
		if ( generateMoreData && filelen > 0 )
		{
			actlen = packetlen <= filelen ? packetlen : filelen;

			myin->ReleaseBuffer();
            buf.ReadBytes( buffer, actlen ); // ReadPacketFromNet( buffer,...

			generateMoreData = FALSE;
			filelen -= actlen;
		}

		if ( !generateMoreData )
		{
			myin->SetBuffer( buffer, actlen );
			generateMoreData = TRUE;
			if ( filelen == 0 )
				myin->SetEndOfStream();   // mark EOF
		}

        if ( (*pobj)->read(*myin, EXS_Unknown, EGL_withGL ) == EC_Normal )
            break;                    // EndOfFile from readBlock() detected
        if ( myin->EndOfStream() )
		{
			eofCounter++;
			if ( eofCounter > 2 )
            {                         // emergency halt
				cerr << endl;
                cerr << " Error: end of Buffer detected." << endl;
				break;
			}
		}
    }
    delete myin;
    return TRUE;
}


// ********************************************


BOOL writeObject( DcmDataset *pobj,
                  const char *filename,
                  E_TransferSyntax xfer,
                  E_EncodingType enctype,
                  E_GrpLenEncoding gltype )
{
    const Uint32 packetlen = 200;

    DcmFileStream buf(filename, DCM_WriteMode);
    if ( buf.Fail() ) {
        fprintf(stderr, "dcmdump: cannot create file: %s\n", filename);
        return FALSE;
    }

	DcmBufferStream myout(DCM_WriteMode);

    char *buffer = new char[ packetlen ];
    void * getPtr = NULL;
    BOOL generateMoreData = TRUE;
	myout.SetBuffer(buffer, packetlen);

    while ( 1 )
    {
		if (generateMoreData && 
			pobj->write(myout, xfer, enctype, gltype ) == EC_Normal )
		{
			generateMoreData = FALSE;
            myout.SetEndOfStream();     // No more data available
		}

		Uint32 maxreadlen = 0;
		myout.GetBuffer(getPtr, maxreadlen);
		buf.WriteBytes(getPtr, maxreadlen); // putPacketIntoNet( buffer,...
		if (myout.EndOfStream())
            break;           // EndOfFile from readBuffer() detected
    }
    delete buffer;

    return TRUE;
}


// ********************************************


int main(int argc, char *argv[])
{
    if ( argc != 3 ) {
	fprintf(stderr, "usage: %s file-in file-out\n", argv[0]);
	return 1;
    }

    SetDebugLevel(( 9 ));

    DcmDataset *pobj = NULL;
    BOOL ok = FALSE;

    ok = readObject( &pobj, argv[1] );

    if ( ok ) {
	writeObject( pobj, argv[2],
		     pobj->getOriginalXfer(), EET_ExplicitLength, 
		     EGL_withoutGL );
	cerr << "#Verify returned with: " << pobj->verify( TRUE ) << endl;
	pobj->print();
    }

    delete pobj;

    return 0;
}
