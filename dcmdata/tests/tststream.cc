/*
 *
 * Author: Gerd Ehlers		Created:  03-26-94
 *                              Modified: 11-23-94
 *
 * Module: streamtst.cc
 *
 * Purpose:
 * Test the Dicom binary streams
 *
 *
 * Last Update:   $Author: hewett $
 * Revision:      $Revision: 1.2 $
 * Status:	  $State: Exp $
 *
 */

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <stream.h>
#include "dctk.h"
#include "dcvr.h"
#include "dcdebug.h"
#include "dcstream.h"


E_Condition test_iDS( T_VR_UL buflen,
		      T_VR_UL packetlen,
		      T_VR_UL readlen,
		      T_VR_UL filelen )
{
    cerr << endl << "test_iDS: Begin" << endl;
    cerr << "param: packetlen=" << packetlen << " readlen=" << readlen;
    cerr << " buflen=" << buflen << " filelen=" << filelen << endl;

    iDicomStream *buf = new iDicomStream( buflen );

    char *buffer = new char[ packetlen ];
    char *printbuf = new char[ readlen ];
    BOOL generateMoreData = TRUE;
    T_VR_UL val = 0;
    T_VR_UL actlen = packetlen;

    cerr << "buffer: avail()=" << buf->avail();
    cerr << " buffered()=" << buf->buffered() << endl;

    while ( 1 )
    {
	cerr << "*" << endl;
	if ( generateMoreData && filelen > 0 )
	{
	    actlen = packetlen <= filelen ? packetlen : filelen;

	    cerr << endl << "Info: creating " << actlen << " bytes data.";
	    cerr << endl;
	    memset( buffer, 0, packetlen );
	    T_VR_UL i;
	    for ( i = 0; i < actlen; i++ )
		buffer[ i ] = (char)( val++ & 0xff );
	    generateMoreData = FALSE;

	    filelen -= actlen;
	}

	if ( !generateMoreData )
	{
	    if ( buf->fillBuffer( buffer, actlen ) == EC_BufferFull )
		cerr << "Warning: Buffer Full!" << endl;
	    else
	    {
		generateMoreData = TRUE;
		if ( filelen == 0 )
		{
		    buf->markBufferEOF();
		    cerr << "Info: end of input file detected." << endl;
		}
	    }
	}

	cerr << "#" << endl;
	if ( buf->eof() )
	    break;
	else
	{
	    T_VR_UL maxreadlen = ( readlen <= buf->buffered() )
				 ? readlen
				 : buf->buffered();
	    buf->read( printbuf, (int)maxreadlen );

	    T_VR_UL j = 0;
	    while ( j < maxreadlen )
	    {
		fprintf( stderr, " %2.2x", (unsigned char)printbuf[ j ] );
		j++;
		if ( j % 16 == 0 )
		    cerr << endl;
	    }
	    cerr << endl;
	}
    }
    cerr << "test_iDS: End" << endl;

    delete buf;
    delete buffer;
    delete printbuf;

    return EC_Normal;
}


// ********************************************


E_Condition test_oDS( T_VR_UL buflen,
		      T_VR_UL packetlen,
		      T_VR_UL readlen,
		      T_VR_UL filelen )
{
    cerr << endl << "test_oDS: Begin" << endl;
    cerr << "param: packetlen=" << packetlen << " readlen=" << readlen;
    cerr << " buflen=" << buflen << " filelen=" << filelen << endl;

    oDicomStream *buf = new oDicomStream( buflen );

    char *buffer = new char[ packetlen ];
    char *printbuf = new char[ readlen ];
    T_VR_UL val = 0;
    T_VR_UL actlen = packetlen;

    cerr << "buffer: avail()=" << buf->avail();
    cerr << " buffered()=" << buf->buffered() << endl;

    while ( 1 )
    {
	if ( filelen > 0 )
	{
	    cerr << "*";
	    actlen = buf->avail() <= filelen ? buf->avail() : filelen;
	    T_VR_UL restlen = actlen % packetlen;
	    actlen -= restlen;

	    cerr << endl << "Info: creating " << actlen << " bytes data.";
	    cerr << endl;
	    memset( buffer, 0, packetlen );
	    T_VR_UL i, k;
	    if ( restlen < filelen )
	    {
		for ( i = 0; i < actlen/packetlen; i++ )
		{
		    for ( k = 0; k < packetlen; k++ )
			buffer[ k ] = (char)( val++ & 0xff );

		    cerr << "*";
		    buf->write( buffer, packetlen );
		    if ( buf->pcount() != packetlen )
			cerr << "Warning: error in writing Buffer" << endl;
		}
	    }
	    else
	    {
		cerr << "Info: creating rest " << filelen << " bytes." << endl;
		for ( k = 0; k < filelen; k++ )
		    buffer[ k ] = (char)( val++ & 0xff );

		buf->write( buffer, filelen );
		filelen = 0;
	    }
	    filelen -= actlen;
	}
	if ( filelen == 0 )
	    buf->markBufferEOF();

	cerr << "#" << endl;
	{
	    T_VR_UL maxreadlen = 0;
	    E_Condition errBuf;
	    errBuf = buf->readBuffer( printbuf, readlen, &maxreadlen );

	    T_VR_UL j = 0;
	    while ( j < maxreadlen )
	    {
		fprintf( stderr, " %2.2x", (unsigned char)printbuf[ j ] );
		j++;
		if ( j % 16 == 0 )
		    cerr << endl;
	    }
	    cerr << endl;
	    if ( errBuf == EC_EndOfBuffer )
		break;
	}
    }
    cerr << "test_iDS: End" << endl;

    delete buf;
    delete buffer;
    delete printbuf;

    return EC_Normal;
}


// ********************************************


int main(int argc, char *argv[])
{
    cin.sync_with_stdio();
    cout.sync_with_stdio();
    cerr.sync_with_stdio();

    T_VR_UL buflen = 110;
    T_VR_UL packetlen = 100;
    T_VR_UL readlen = 80;
    T_VR_UL filelen = 510;

    if ( (argc > 5) || (argc == 2 && !strcmp( argv[1], "-h" )) )
    {
	cerr << "usage: " << argv[0];
	cerr << " [packetlen [readlen [buflen [filelen]]]]" << endl;
	cerr << "       test buffered streams" << endl;
	cerr << endl;
	cerr << "param: packetlen=" << packetlen << " readlen=" << readlen;
	cerr << " buflen=" << buflen << " filelen=" << filelen << endl;
	exit(1);
    }

    if ( argc >= 5 )
	filelen = (T_VR_UL)atoi( argv[4] );
    if ( argc >= 4 )
	buflen = (T_VR_UL)atoi( argv[3] );
    if ( argc >= 3 )
	readlen = (T_VR_UL)atoi( argv[2] );
    if ( argc >= 2 )
	packetlen = (T_VR_UL)atoi( argv[1] );

    test_iDS( buflen, packetlen, readlen, filelen );
    test_oDS( buflen, packetlen, readlen, filelen );

    exit(0);
}

