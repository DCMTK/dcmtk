/*
 *
 * Author: Gerd Ehlers      Created:  03-26-94
 *                          Modified: 07-17-95
 *
 * Module: dcstream.cc
 *
 * Purpose:
 * Binary streams inherited from the GNU IOSTREAM Library
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

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#include <string.h>
#include <errno.h>
#include <iostream.h>

#include "dcstream.h"
#include "dcswap.h"
#include "dcxfer.h"
#include "dcdebug.h"



// *****************************************************
// *** input *******************************************
// *****************************************************


iDicomStream::iDicomStream( const char *name,
			    int mode,
			    int prot )
{
    Bdebug((6, "dcstream:iDicomStream::iDicomStream(char*,mode=%d,prot=%d)",
           mode, prot));
    
    if (mode & DCM_InFile)
    {
	inFile = fopen(name, "rb");
	fileOperationOK = inFile != NULL;
    }
    else
    {
	inFile = NULL;
	fileOperationOK = FALSE;
    }
    if (!fileOperationOK)
        fileOperationErrno = errno;
      
    flagStreamFromFile = TRUE;
    machineByteOrder   = MachineByteOrder();
    dataByteOrder      = EBO_LittleEndian;
    mustSwap = ( machineByteOrder != dataByteOrder );
    EndOfFile = FALSE;
    BufLen = MIN_BUFFER_LENGTH;
    buffer = (char*)NULL;
    startBuf = endBuf = _gcount = 0;
    numChars = 0;
    _gposition = 0;
    Edebug(());

}


// *****************************************************


iDicomStream::iDicomStream( T_VR_UL bufferLength )
{
    Bdebug((6, "dcstream:iDicomStream::iDicomStream(bufferLength=%d)",
           bufferLength));

    flagStreamFromFile = FALSE;
    machineByteOrder   = MachineByteOrder();
    dataByteOrder      = EBO_LittleEndian;
    mustSwap = ( machineByteOrder != dataByteOrder );
    EndOfFile = FALSE;
    BufLen = bufferLength + MIN_BUFFER_LENGTH;
    buffer = new char[ BufLen ];
    startBuf = endBuf = _gcount = 0;
    numChars = 0;
    _gposition = 0;
    Edebug(());

}


// *****************************************************


iDicomStream::iDicomStream( const iDicomStream & /*newiDicom*/ )
{
    Bdebug((6, "dcstream:iDicomStream::iDicomStream(iDicomStream&)" ));

    flagStreamFromFile = FALSE;
    machineByteOrder   = MachineByteOrder();
    dataByteOrder      = EBO_LittleEndian;
    mustSwap = ( machineByteOrder != dataByteOrder );
    EndOfFile = FALSE;
    BufLen = MIN_BUFFER_LENGTH;
    buffer = (char*)NULL;
    startBuf = endBuf = _gcount = 0;
    numChars = 0;
    _gposition = 0;
    fprintf(stderr, "Warning: iDicomStream: use of Copy-Constructor not allowed\n");
    Edebug(());

}


// *****************************************************


iDicomStream::~iDicomStream()
{
    Bdebug((6, "dcstream:iDicomStream::~iDicomStream()" ));

    if (inFile)
        fclose(inFile);

    if ( buffer != (char*)NULL )
	delete buffer;
    Edebug(());

}


// *****************************************************


void iDicomStream::setDataByteOrder( E_TransferSyntax ts )
{
    dataByteOrder = ConvXferToByteOrder( ts );
    mustSwap = ( machineByteOrder != dataByteOrder );
}


// *****************************************************


void iDicomStream::setDataByteOrder( E_ByteOrder bo )
{
    dataByteOrder = bo;
    mustSwap = ( machineByteOrder != dataByteOrder );
}


// *****************************************************


E_Condition iDicomStream::fillBuffer( char *buf,
				      T_VR_UL len )
{
    Bdebug((4, "dcstream:iDicomStream::fillBuffer(char*,int len=%d)", len));

    E_Condition l_error = EC_Normal;
    if ( fromFile() || buf == (char*)NULL )
        l_error = EC_IllegalCall;
    else if ( avail() < len )
        l_error = EC_BufferFull;
    else if ( len > 0 )
    {
	EndOfFile = FALSE;
	T_VR_UL bufpart = (BufLen - endBuf);

	if ( bufpart < len )		    // Puffer ist zweigeteilt
	{
	    memcpy( &buffer[endBuf], buf, (int)bufpart );
	    memcpy( buffer, &buf[bufpart], (int)(len - bufpart) );
debug(( 4, "write 1. from val[%d..%d] into mem[%d..%d]",
	   0, bufpart-1, endBuf, endBuf+bufpart-1  ));
debug(( 4, "write 2. from val[%d..%d] into mem[%d..%d]",
	   bufpart, len-1, 0, len-bufpart-1  ));

	}
	else
	{
	    memcpy( &buffer[endBuf], buf, (int)len );
debug(( 4, "write from val[%d..%d] into mem[%d..%d]",
	   0, len-1, endBuf, endBuf+len-1 ));

	}
	endBuf = ( endBuf + len ) % BufLen;
	numChars += len;
debug(( 4, "number of buffered chars after filling Buffer=%d", buffered() ));

    }
Edebug(());

    return l_error;
}


// *****************************************************


iDicomStream& iDicomStream::readFromBuffer( char* val, int size )
{

    Bdebug((4, "dcstream:iDicomStream::readFromBuffer(char*,int size=%d)",
	   size));

    if (size > 0)
    {
	if ( buffered() > 0 )
	{
	    T_VR_UL len = (T_VR_UL)size <= buffered()
			  ? (T_VR_UL)size
			  : buffered();
	    T_VR_UL bufpart = (BufLen - startBuf);

	    if ( bufpart < (T_VR_UL)size )	      // Puffer ist zweigeteilt
	    {
		memcpy( val, &buffer[startBuf], (int)bufpart );
		memcpy( &val[bufpart], buffer, (int)(len - bufpart) );

debug(( 4, "read 1. from mem[%d..%d] into val[%d..%d]",
	   startBuf, startBuf+bufpart-1, 0, bufpart-1 ));
debug(( 4, "read 2. from mem[%d..%d] into val[%d..%d]",
	   0, len-bufpart-1, bufpart, len-1 ));

	    }
	    else
	    {
		memcpy( val, &buffer[startBuf], (int)len );

debug(( 4, "read from mem[%d..%d] into val[%d..%d]",
	   startBuf, startBuf+len-1, 0, len-1 ));

	    }
	    startBuf = ( startBuf + len ) % BufLen;
	    numChars -= len;
	    _gcount = len;
	    if ( len < (T_VR_UL)size )
		memset( &val[len], 0, (int)(size - len) );
	}
	else
	{
	    memset( val, 0, size );
	    _gcount = 0;
	}
    }
    _gposition += _gcount;
    debug(( 3, "number of buffered chars after read=%d", buffered() ));
    Edebug(());

    return *this;
}


// *****************************************************


iDicomStream& iDicomStream::readFromFile( char* val, int size )
{

    Bdebug((5, "dcstream:iDicomStream::readFromFile(char*,int size=%d)", size));

    if (size > 0)
    {
	if ( good() )
	{
	    fileOperationOK = fread(val, size, 1, inFile) == 1;
	    if (!fileOperationOK)
	    {
		_gcount = 0;
	        fileOperationErrno = errno;
	    }
	    else
	        _gcount = size;
	}
	    
    }
    Edebug(());

    return *this;
}


// *****************************************************


iDicomStream& iDicomStream::readSw( char* val, int val_width, int times )
{

    Bdebug((5, "dcstream:iDicomStream::readSw(char*,val_width=%d,times=%d)",
           val_width, times ));

    if ( val_width > 0  &&  times > 0  &&  val != (char*)NULL )
    {
	if ( fromFile() )
            readFromFile( val, val_width * times );
	else
            readFromBuffer( val, val_width * times );

        if ( mustSwap && val_width > 1 )
	{
debug(( 5, "must swap value" ));

            SwapN( val, val_width, times );
        }
Cdebug( val_width==2, (5, "val=[0x%4.4x]\t size=%d", *((T_VR_US*)val),
						  gcount()));
Cdebug( val_width==4, (5, "val=[0x%8.8x]\t size=%d", *((T_VR_UL*)val),
						  gcount()));
Cdebug( (val_width!=2)&(val_width!=4), (5, "val='%c'...\t size=%d", *val,
							  gcount()));
Cdebug( !good(), (5, "good()=(0x%x)", good() + 2*eof() ));

    }
Edebug(());

    return *this;
}


// *****************************************************


E_Condition iDicomStream::markBufferEOF()
{
    EndOfFile = TRUE;
    return EC_Normal;
}


// *****************************************************


T_VR_UL iDicomStream::buffered()
{
    return numChars;
}


// *****************************************************


T_VR_UL iDicomStream::avail()
{
    return (BufLen - numChars);
}


// *****************************************************


iDicomStream& iDicomStream::putback( char ch )
{
    if ( fromFile() )
    {
	fileOperationOK = ungetc((int)ch, inFile) != EOF;
	if (!fileOperationOK)
	    fileOperationErrno = errno;
    }
    else if ( avail() > 0 )
    {
	startBuf = ( BufLen + startBuf - 1 ) % BufLen;
	buffer[startBuf] = ch;
	numChars += 1;
    }
    _gcount -= 1;
    return *this;
}




// *****************************************************


void iDicomStream::seekg(long offset)
{
    fileOperationOK = fseek(inFile, offset, SEEK_SET) == 0;
    if (!fileOperationOK)
      fileOperationErrno = errno;
}

// *****************************************************


iDicomStream& iDicomStream::unget()
{
    if ( fromFile() )
    {
        fileOperationOK = ! fseek(inFile, -1L, SEEK_CUR);
	if (!fileOperationOK)
	    fileOperationErrno = errno;
    }
    else if ( BufLen - buffered() > 0 )
    {
	startBuf = ( BufLen + startBuf - 1 ) % BufLen;
	numChars += 1;
    }
    _gcount -= 1;
    return *this;
}


// *****************************************************


long iDicomStream::tellg()
{
    if ( fromFile() )
    {
	long val = ftell(inFile);
	fileOperationOK = val != -1L;
	if (!fileOperationOK)
	    fileOperationOK = errno;
	return val;
    }
    else
        return _gposition;
}


// *****************************************************


int iDicomStream::good()
{
    if ( fromFile() )
	return fileOperationOK;
    else return buffered();
}


// *****************************************************


int iDicomStream::eof()
{
    if ( fromFile() )
	return feof(inFile);
    else return ( buffered() == 0 && EndOfFile );
}


// *****************************************************
// *** output ******************************************
// *****************************************************



oDicomStream::oDicomStream( const char *name,
			    int mode,
			    int prot )
{
    Bdebug((6, "dcstream:oDicomStream::oDicomStream(char*,mode=%d,prot=%d)",
           mode, prot));

    if (mode & DCM_OutFile)
    {
	outFile = fopen(name, "wb");
	fileOperationOK = outFile != NULL;
    }
    else
    {
	outFile = NULL;
	fileOperationOK = FALSE;
    }
    if (!fileOperationOK)
        fileOperationErrno = errno;
      
    flagStreamIntoFile	 = TRUE;
    machineByteOrder   = MachineByteOrder();
    dataByteOrder      = EBO_LittleEndian;
    mustSwap = ( machineByteOrder != dataByteOrder );
    EndOfFile = FALSE;
    BufLen = MIN_BUFFER_LENGTH;
    buffer = (char*)NULL;
    startBuf = endBuf = _pcount = 0;
    numChars = 0;
    _pposition = 0;
    Edebug(());

}


// *****************************************************


oDicomStream::oDicomStream( T_VR_UL bufferLength )
{
    Bdebug((6, "dcstream:oDicomStream::oDicomStream(bufferLength=%d)",
           bufferLength));

    outFile = NULL;
    fileOperationOK = FALSE;
    fileOperationErrno = 0;
    flagStreamIntoFile = FALSE;
    machineByteOrder   = MachineByteOrder();
    dataByteOrder      = EBO_LittleEndian;
    mustSwap = ( machineByteOrder != dataByteOrder );
    EndOfFile = FALSE;
    BufLen = bufferLength + MIN_BUFFER_LENGTH;
    buffer = new char[ BufLen ];
    startBuf = endBuf = _pcount = 0;
    numChars = 0;
    _pposition = 0;
Edebug(());

}


// *****************************************************


oDicomStream::oDicomStream( const oDicomStream & /*newoDicom*/ )
{
    Bdebug((6, "dcstream:oDicomStream::oDicomStream(oDicomStream&)" ));

    outFile = NULL;
    fileOperationOK = FALSE;
    fileOperationErrno = 0;
    flagStreamIntoFile = FALSE;
    machineByteOrder   = MachineByteOrder();
    dataByteOrder      = EBO_LittleEndian;
    mustSwap = ( machineByteOrder != dataByteOrder );
    EndOfFile = FALSE;
    BufLen = MIN_BUFFER_LENGTH;
    buffer = (char*)NULL;
    startBuf = endBuf = _pcount = 0;
    numChars = 0;
    _pposition = 0;
    fprintf(stderr, "Warning: oDicomStream: use of Copy-Constructor not allowed\n");
Edebug(());

}


// *****************************************************


oDicomStream::~oDicomStream()
{
    Bdebug((6, "dcstream:oDicomStream::~oDicomStream()" ));

    if (outFile)
        fclose(outFile);

    if ( buffer != (char*)NULL )
	delete buffer;
    Edebug(());

}


// *****************************************************


void oDicomStream::close()
{
    fclose(outFile);
    fileOperationOK = FALSE;
}


// *****************************************************


void oDicomStream::setDataByteOrder( E_TransferSyntax ts )
{
    dataByteOrder = ConvXferToByteOrder( ts );
    mustSwap = ( machineByteOrder != dataByteOrder );
}


// *****************************************************


void oDicomStream::setDataByteOrder( E_ByteOrder bo )
{
    dataByteOrder = bo;
    mustSwap = ( machineByteOrder != dataByteOrder );
}


// *****************************************************


E_Condition oDicomStream::readBuffer( char* buf,
				      T_VR_UL size,
				      T_VR_UL *realsize )
{
Bdebug((4, "dcstream:oDicomStream::readBuffer(char*,size=%d)", size ));

    *realsize = 0;
    E_Condition l_error = EC_Normal;

    if ( intoFile() || buf == (char*)NULL )
        l_error = EC_IllegalCall;
    else
    {
	if ( buffered() > 0 && size > 0 )
	{
	    T_VR_UL len = (size <= buffered())
			  ? size
			  : buffered();
	    T_VR_UL bufpart = (BufLen - startBuf);

	    if ( bufpart < size )		// Puffer ist zweigeteilt
	    {
		memcpy( buf, &buffer[startBuf], (int)bufpart );
		memcpy( &buf[bufpart], buffer, (int)(len - bufpart) );
debug(( 4, "read 1. from mem[%d..%d] into val[%d..%d]",
	   startBuf, startBuf+bufpart-1, 0, bufpart-1 ));
debug(( 4, "read 2. from mem[%d..%d] into val[%d..%d]",
	   0, len-bufpart-1, bufpart, len-1 ));

	    }
	    else
	    {
		memcpy( buf, &buffer[startBuf], (int)len );
debug(( 4, "read from mem[%d..%d] into val[%d..%d]",
	   startBuf, startBuf+len-1, 0, len-1 ));

	    }
	    startBuf = ( startBuf + len ) % BufLen;
	    numChars -= len;
	    if ( len < size )
		memset( &buf[len], 0, (int)(size - len) );
	    *realsize = len;
	}
	if ( buffered() == 0 && EndOfFile )
            l_error = EC_EndOfBuffer;
    }
debug(( 4, "number of buffered chars after reading Buffer=%d", buffered() ));
Edebug(());

    return  l_error;
}


// *****************************************************


oDicomStream& oDicomStream::writeIntoBuffer( const char* val, int size )
{
Bdebug((4, "dcstream:oDicomStream::writeIntoBuffer(char*,size=%d)", size ));

    if ( size > 0 && avail() > 0 )
    {
	EndOfFile = FALSE;
	T_VR_UL len = (T_VR_UL)size <= avail() ? (T_VR_UL)size : avail();
	T_VR_UL bufpart = (BufLen - endBuf);

	if ( bufpart < len )		    // Puffer ist zweigeteilt
	{
	    memcpy( &buffer[endBuf], val, (int)bufpart );
	    memcpy( buffer, &val[bufpart], (int)(len - bufpart) );
debug(( 4, "write 1. from val[%d..%d] into mem[%d..%d]",
	   0, bufpart-1, endBuf, endBuf+bufpart-1  ));
debug(( 4, "write 2. from val[%d..%d] into mem[%d..%d]",
	   bufpart, len-1, 0, len-bufpart-1  ));

	}
	else
	{
	    memcpy( &buffer[endBuf], val, (int)len );
debug(( 4, "write from val[%d..%d] into mem[%d..%d]",
	   0, len-1, endBuf, endBuf+len-1 ));

	}
	endBuf = ( endBuf + len ) % BufLen;
	numChars += len;
	_pcount = len;
    }
    else
    {
	_pcount = 0;
    }
    _pposition += _pcount;
debug(( 4, "number of buffered chars after write=%d", buffered() ));
Edebug(());

    return *this;
}


// *****************************************************


oDicomStream& oDicomStream::writeIntoFile( const char* val, int size )
{
Bdebug((5, "dcstream:oDicomStream::writeIntoFile(char*,size=%d)", size ));

    if (good())
    {
	if ((fileOperationOK = fwrite( val, size, 1, outFile)) == 1)
	  _pcount = size;
    }
    else
	_pcount = 0;
    _pposition += _pcount;
Edebug(());

    return *this;
}

// *****************************************************


oDicomStream& oDicomStream::writeSw( const char* val, int val_width, int times )
{
Bdebug((5, "dcstream:oDicomStream::writeSw(char*,val_width=%d,times=%d)",
           val_width, times ));
    if ( val_width > 0  &&  times > 0  &&  val != (char*)NULL )
    {
Cdebug( val_width==2,
        (5, "val=[0x%4.4x]\t size=%d", *((T_VR_US*)val), times*val_width));
Cdebug( val_width==4,
        (5, "val=[0x%8.8x]\t size=%d", *((T_VR_UL*)val), times*val_width));
Cdebug( (val_width!=2)&(val_width!=4),
        (5, "val='%c'...\t size=%d", *val, times*val_width));

	char *tempval = (char*)NULL;
	const char *cval;
        if ( mustSwap && val_width > 1 )
	{
debug(( 5, "must swap value" ));

            tempval = new char[ val_width*times ];
            memcpy( tempval, val, val_width*times );
            SwapN( tempval, val_width, times );
	    cval = tempval;

Cdebug( val_width==2,
        (5, "val=[0x%4.4x]\t size=%d", *((T_VR_US*)tempval), times*val_width));
Cdebug( val_width==4,
        (5, "val=[0x%8.8x]\t size=%d", *((T_VR_UL*)tempval), times*val_width));
Cdebug( (val_width!=2)&(val_width!=4),
        (5, "val='%c'...\t size=%d", *tempval, times*val_width));

	}
	else
	    cval = val;

	if ( intoFile() )
            writeIntoFile( cval, val_width * times );
	else
            writeIntoBuffer( cval, val_width * times );

Vdebug((5, rdstate(), "rdstate=(0x%x)",
	  rdstate() ));

	if ( tempval != (char*)NULL )
	    delete tempval;
    }
Edebug(());

    return *this;
}


// *****************************************************


E_Condition oDicomStream::markBufferEOF()
{
    EndOfFile = TRUE;
    return EC_Normal;
}


// *****************************************************


T_VR_UL oDicomStream::buffered()
{
    return numChars;
}


// *****************************************************


T_VR_UL oDicomStream::avail()
{
    return (BufLen - numChars);
}


// *****************************************************


long oDicomStream::tellp()
{
    if ( intoFile() )
    {
	long val = ftell(outFile);
	fileOperationOK = val != -1L;
	return val;
    }
    else
        return _pposition;
}


// *****************************************************


int oDicomStream::good()
{
    if ( intoFile() )
	return fileOperationOK;
    else return (( BufLen - buffered()) > 0);
}






