/*
 *
 * Author: Gerd Ehlers      Created:  05-23-94
 *                          Modified: 01-29-95
 *
 * Module: tstblock.cc
 *
 * Purpose:
 * test the class DcmObject with read/writeBlock
 *
 *
 * Last Update:   $Author: hewett $
 * Revision:      $Revision: 1.1 $
 * Status:	  $State: Exp $
 *
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <stream.h>
#include "dctk.h"
#include "dcvr.h"
#include "dcdebug.h"


// ********************************************


void printstack( DcmStack &stack )
{
Bdebug((5, "tstblock:printstack(DcmStack&)" ));

    cout << "#Stack Frame:" << endl;
    DcmObject *obj;
    T_VR_UL i = stack.card();
    while ( i > 0 )
    {
	obj = stack.elem( i-1 );

	printf
		 (" elem(%d):%p tag(%4.4x,%4.4x) vr(%s) err(%d) \"%s\"",
		    i-1,
		    obj,
		    obj->getGTag(),
		    obj->getETag(),
		    DcmVR( obj->getVR() ).getVRName(),
		    obj->error(),
                    obj->getTag().getTagName() );
	cout << endl;
	i--;
    }
Edebug(());

}


// ********************************************


E_Condition search( DcmObject* pobj, ETag xtag )
{
Bdebug((5, "tstblock:search(pobj*,xtag=%d)", xtag ));

    E_Condition error = EC_Normal;
    DcmTag s_tag( xtag );
    DcmStack stack;
    int count = 0;

    error = pobj->search( s_tag, stack, ESM_fromHere, TRUE );
Vdebug((1, error == EC_Normal, "Search succeeded" ));
Vdebug((1, error == EC_TagNotFound, "Search returned TagNotFound" ));
Vdebug((1, error == EC_IllegalCall, "Search returned IllegalCall" ));
Vdebug((1, error != EC_Normal
           && error != EC_TagNotFound
           && error != EC_IllegalCall, "Search returned error=%d", error ));

    if ( error == EC_Normal )
    {
	count++;
	cerr << "Found " << dec<< count;
	cerr << ". tag=(" << hex << stack.top()->getGTag() << ",";
	cerr << 	     hex << stack.top()->getGTag() << ") at p=";
	cerr << hex << stack.top() << " : ";
        cerr << stack.top()->getTag().getTagName() << dec << endl;
    }
    cout << "# Stack returned after single search:" << endl;
    printstack( stack );

    while ( (error=pobj->search( s_tag, stack, ESM_afterStackTop, TRUE ))
	    == EC_Normal )
    {
Vdebug((1, error == EC_Normal, "Search succeeded" ));

	count++;
	cerr << "Found " << dec << count;
	cerr << ". tag=(" << hex << stack.top()->getGTag() << ",";
	cerr << 	     hex << stack.top()->getGTag() << ") at p=";
	cerr << hex << stack.top() << " : ";
        cerr << stack.top()->getTag().getTagName() << dec << endl;
	cout << "# Stack returned after " << count << ". search:" << endl;
	printstack( stack );
    }
Vdebug((1, error == EC_TagNotFound, "Search returned TagNotFound" ));
Vdebug((1, error == EC_IllegalCall, "Search returned IllegalCall" ));
Vdebug((1, error != EC_Normal
           && error != EC_TagNotFound
           && error != EC_IllegalCall, "Search returned error=%d", error ));

    cout << "# Stack returned after all searches:" << endl;
    printstack( stack );
Edebug(());

    return error;
}


// ********************************************


E_Condition readObject( DcmObject **pobj,
                        char *filename,
                        E_GrpLenEncoding igltype )
{
    struct stat file_stat;
    stat( filename, &file_stat );
    T_VR_UL filelen = file_stat.st_size;

    T_VR_UL buflen = 1020;
    T_VR_UL packetlen = 200;

    iDicomStream buf( filename );
    iDicomStream *myin = new iDicomStream( buflen );
    *pobj = new DcmDataset( myin );

    char *buffer = new char[ packetlen ];
    BOOL generateMoreData = TRUE;
    T_VR_UL actlen = packetlen;
    T_VR_UL eofCounter = 0;

    while ( 1 )
    {
	cerr << "*";
	if ( generateMoreData && filelen > 0 )
	{
	    actlen = packetlen <= filelen ? packetlen : filelen;

	    buf.read( buffer, (int)actlen );

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
		{
		    myin->markBufferEOF();
		    cerr << endl;
		    cerr << "Info: end of input file in Buffer marked." << endl;
		}
	    }
	}

        if ( (*pobj)->readBlock( EXS_UNKNOWN, igltype ) == EC_Normal )
	{
	    cerr << endl;
	    cerr << " Info: end of data from readBlock() detected." << endl;
	    break;
	}
	if ( myin->eof() )
	{
	    eofCounter++;
	    if ( eofCounter > 2 )
	    {
		cerr << endl;
		cerr << " Warning: end of Buffer detected." << endl;
		break;
	    }
	}
    }
    cerr << endl;
    delete buffer;
    // kein: delete myin;
    return EC_Normal;
}


// ********************************************


E_Condition writeObject( DcmObject *pobj,
			 char *filename,
			 E_TransferSyntax xfer,
                         E_EncodingType enctype,
                         E_GrpLenEncoding ogltype )
{
    T_VR_UL buflen = 1020;
    T_VR_UL packetlen = 200;

    oDicomStream buf( filename );
    oDicomStream myout( buflen );

    char *buffer = new char[ packetlen ];
    BOOL generateMoreData = TRUE;

    while ( 1 )
    {
	cerr << "*";
	if (	generateMoreData
             && pobj->writeBlock( myout, xfer, enctype, ogltype ) == EC_Normal )
	{
	    generateMoreData = FALSE;
	    myout.markBufferEOF();
	    cerr << endl;
	    cerr << " Info: end of file in dicom dataset encountered" << endl;
	}

	T_VR_UL maxreadlen = 0;
	E_Condition errBuf;
	errBuf = myout.readBuffer( buffer, packetlen, &maxreadlen );
	if ( maxreadlen != 0 )
	    buf.write( buffer, (int)maxreadlen );
	else
	{
	    cerr << endl;
	    cerr << " Warning: myout.readBuffer() returned 0" << endl;
	}
	if ( errBuf == EC_EndOfBuffer )
	{
	    cerr << endl;
	    cerr << " Info: end of file from readBuffer() detected.";
	    break;
	}
    }
    cerr << endl;
    delete buffer;
    return EC_Normal;
}


// ********************************************


int main(int argc, char *argv[])
{
    cin.sync_with_stdio();
    cout.sync_with_stdio();
    cerr.sync_with_stdio();
    SetDebugLevel(( 2 ));

    if (argc <= 1)
    {
        cerr <<
        "usage: tstblock binary-in [binary-out [xfer [enctype [ogltype]]]]\n"
        "       tstblock binary-in [-s xtag ]\n"
        "    convert DICOM-data from one file to another\n"
        "    or print binary-in to stdout and search a Tag\n"
        "         xfer=(LI,LE,BE)=(0,2,3)               def=0\n"
        "         enctype=(ExplicitLen,UndefLen)=(0,1)  def=0\n"
        "         ogltype=(withoutGL,withGL)=(0,1)      def=0\n"
        "    -s xtag=(0..652)                           def=427\n"
        << endl;
        return 1;
    }
Bdebug((1, "tstblock:main()" ));

    char*	     ifname = NULL;
    char*	     ofname = NULL;
    E_TransferSyntax xfer = EXS_LittleEndianImplicit;
    E_EncodingType   enctype = EET_ExplicitLength;
    E_GrpLenEncoding ogltype = EGL_withoutGL;
    ETag	     xtag = ET_BitsAllocated0028;
    BOOL	     searchmode = FALSE;
    E_Condition      error = EC_Normal;

    if ( argc >= 6 )
        ogltype = (E_GrpLenEncoding)atoi( argv[6] );
    if ( argc >= 5 )
	enctype = (E_EncodingType)atoi( argv[4] );
    if ( argc >= 4 )
	xfer = (E_TransferSyntax)atoi( argv[3] );
    if ( argc >= 3 )
    {
	if ( strcmp( argv[2], "-s" ) == 0 )
	{
	    searchmode = TRUE;
	    if ( argc >= 4 )
		xtag = (ETag)atoi( argv[3] );
	    cerr << argv[0] << ":reading, printing " << argv[1];
	    cerr << " and searching for xtag=" << xtag << ".";
	    cerr << endl;
	}
	else
	{
	    ofname = argv[2];
	    cerr << argv[0] << ":converting " << argv[1] << " to " << argv[2];
	    cerr << " with xfer=" << xfer;
	    cerr << " and enctype=" << enctype << ".";
	    cerr << endl;
	}
    }
    else
    {
	cerr << argv[0] << ":reading and printing " << argv[1];
	cerr << endl;
    }

    ifname = argv[1];
    DcmObject *pobj;
/*
    // 1. Moeglichkeit, die Daten zu lesen:
    iDicomStream *myin = new iDicomStream( ifname );
    pobj = new DcmDataset( myin );
    pobj->read( EXS_UNKNOWN, EGL_withGL );
*/
    // 2. Moeglichkeit:
    E_Condition read_cond = readObject( &pobj, ifname, EGL_withGL );

    fflush( stdout );
    cout << "#readObject() returned error=" << read_cond << endl;
    pobj->print();
    cout << flush;

    if ( searchmode )
    {
	error = search( pobj, xtag );
    }

    if ( argc >= 3 && !searchmode )
    {
/*
        // 1. Moeglichkeit, die Daten zu schreiben:
        oDicomStream myout( ofname );
        pobj->write( myout, xfer, enctype, ogltype );
*/
        // 2. Moeglichkeit:
        writeObject( pobj, ofname, xfer, enctype, ogltype );

	fflush(stdout);
	BOOL boVeri = pobj->verify( TRUE );
	cerr << "#Verify returned with: (" << boVeri << ")" << endl;
	pobj->print();
    }
    DcmStack stk;
    pobj->searchErrors( stk );
    fflush( stdout );

    cout << "# Collected errors:" << endl;
    printstack( stk );

    delete pobj;
    cerr << "#Program ends." << endl;
Edebug(());

    return 0;
}
