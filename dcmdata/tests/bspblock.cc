// bspblock.cc    05-23-94  until  02-15-95

#include <stdio.h>
#include <sys/stat.h>
#include <stream.h>
#include "dctk.h"
#include "dcdebug.h"


void readObject( DcmObject **pobj, char *filename )
{
    struct stat file_stat;
    stat( filename, &file_stat );
    T_VR_UL filelen = file_stat.st_size;

    T_VR_UL buflen = 1020;
    T_VR_UL packetlen = 200;

    iDicomStream buf( filename );
    iDicomStream *myin = new iDicomStream( buflen );
    *pobj = new DcmDataset( myin );

    char buffer[ packetlen ];
    BOOL generateMoreData = TRUE;
    T_VR_UL actlen = packetlen;
    T_VR_UL eofCounter = 0;

    while ( 1 )
    {
	if ( generateMoreData && filelen > 0 )
	{
	    actlen = packetlen <= filelen ? packetlen : filelen;

            buf.read( buffer, (int)actlen ); // ReadPacketFromNet( buffer,...

	    generateMoreData = FALSE;
	    filelen -= actlen;
	}

	if ( !generateMoreData )
	{
	    if ( myin->fillBuffer( buffer, actlen ) == EC_BufferFull )
		cerr << endl << "Warning: Buffer Full!" << endl;
	    else
	    {
		generateMoreData = TRUE;
		if ( filelen == 0 )
                    myin->markBufferEOF();   // mark EOF
	    }
	}

        if ( (*pobj)->readBlock( EXS_UNKNOWN, EGL_withGL ) == EC_Normal )
            break;                    // EndOfFile from readBlock() detected
        if ( myin->eof() )
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
}


// ********************************************


void writeObject( DcmObject *pobj,
                  char *filename,
                  E_TransferSyntax xfer,
                  E_EncodingType enctype,
                  E_GrpLenEncoding gltype )
{
    T_VR_UL buflen = 1020;
    T_VR_UL packetlen = 200;

    oDicomStream buf( filename );
    oDicomStream myout( buflen );

    char *buffer = new char[ packetlen ];
    BOOL generateMoreData = TRUE;

    while ( 1 )
    {
	if (	generateMoreData
             && pobj->writeBlock( myout, xfer, enctype, gltype ) == EC_Normal )
	{
	    generateMoreData = FALSE;
            myout.markBufferEOF();     // No more data available
	}

	T_VR_UL maxreadlen = 0;
	E_Condition errBuf;
	errBuf = myout.readBuffer( buffer, packetlen, &maxreadlen );
        buf.write( buffer, (int)maxreadlen ); // putPacketIntoNet( buffer,...
	if ( errBuf == EC_EndOfBuffer )
            break;           // EndOfFile from readBuffer() detected
    }
    delete buffer;
}


// ********************************************


int main(int argc, char *argv[])
{
    SetDebugLevel(( 9 ));

    DcmObject *pobj;
    readObject( &pobj, "PRIMITIV.DIO" );

    writeObject( pobj, "PRIMITIV.OUT",
                 EXS_BigEndianExplicit, EET_ExplicitLength, EGL_withoutGL );
    cerr << "#Verify returned with: " << pobj->verify( TRUE ) << endl;
    pobj->print();
 
    delete pobj;
    return 0;
}
