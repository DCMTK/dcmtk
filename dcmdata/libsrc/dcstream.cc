/*
** Author: Andreas Barth	Created:	12.11.95
**				Modified:	21.11.95
** Kuratorium OFFIS e.V.
**
** Module: dcstream.cc
**
** Purpose:
**	implements streaming classes for file and buffer input/output
**
** Last Update:		$Author: andreas $
** Update Date:		$Date: 1997-04-18 08:02:10 $
** Source File:		$Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/libsrc/Attic/dcstream.cc,v $
** CVS/RCS Revision:	$Revision: 1.7 $
** Status:		$State: Exp $
**
** CVS/RCS Log at end of file
**
*/


#include "osconfig.h"

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#include "dcstream.h"
#include "dcbuf.h"

//
// CLASS DcmStream
//

DcmStream::DcmStream(const BOOL readMode, const BOOL randomAccess)
{
    fReadMode = readMode;
    fRandomAccess = randomAccess;
}

DcmStream::~DcmStream(void)
{
}



//
// CLASS DcmFileStream
//


Uint32 DcmFileStream::Avail(void)
{
    if (fErrorCond == EC_Normal)
    {
	if (fReadMode)	// no of bytes from postion to eof
	    return fNumBytes - (Uint32)ftell(fFile);
	else	// maximum number of bytes in file system
	    return Uint32(-1);
    }
    return 0;
}


E_Condition DcmFileStream::Avail(const Uint32 numBytes)
{
    E_Condition l_error = fErrorCond;
    if (fErrorCond == EC_Normal)
    {
	if (fReadMode &&
	    // Calculate no of bytes from postion to eof
	    numBytes > fNumBytes - (Uint32)ftell(fFile))
	    l_error = EC_EndOfStream;
    }
    return l_error;
}


void DcmFileStream::Close(void)
{
    if (fFile)
    {
	fclose(fFile);
	fFile = NULL;
    }
}


DcmFileStream::DcmFileStream(const char * filename, 
			     const BOOL readMode,
			     const BOOL randomAccess)
    : DcmStream(readMode, randomAccess)

{
    fErrorCond = EC_Normal;
    fFilename = NULL;

    if (filename)
    {
	fFilename = new char[strlen(filename)+1];
	if (fFilename)
	    strcpy(fFilename, filename);

	if (fReadMode)
	    fFile = fopen(filename, "rb");		// open read only
	else
	    fFile = fopen(filename, "wb");		// open write only

	if (!fFile)
	    fErrorCond = EC_InvalidStream;


    }
    else
	fErrorCond = EC_InvalidStream;

    if (fErrorCond != EC_Normal)
    {
	if (fFile)
	    fclose(fFile);
	fFile = NULL;
    }
    else
    {
	// Get number of bytes in file
	fseek(fFile, 0L, SEEK_END);
	fNumBytes = (Uint32)ftell(fFile);
	fseek(fFile, 0L, SEEK_SET);
    }

    fPutbackMode = FALSE;
    fNumPutbackBytes = 0;
}


DcmFileStream::~DcmFileStream(void)
{
    this -> Close();
    if (fFilename)
	delete fFilename;
}



BOOL DcmFileStream::EndOfStream(void)
{
    if (!fFile)
	fErrorCond = EC_IllegalCall;

    if (fErrorCond == EC_Normal)
    {
	BOOL ret = feof(fFile);
	if (!ret)
	    ret = fNumBytes <= this -> Tell();
	return ret;
    }
    else
	return FALSE;
}

BOOL DcmFileStream::Flush(void)
{
    if (fErrorCond != EC_InvalidStream && fFile)
	fflush(fFile); 
    return TRUE;
}


DcmStreamConstructor * DcmFileStream::NewConstructor(void)
{
    DcmFileStreamConstructor * newConstructor = 
	new DcmFileStreamConstructor(fFilename, fReadMode, fRandomAccess);

    if (newConstructor)
	return newConstructor;
    else
	return NULL;
}


BOOL DcmFileStream::MustFlush(void)
{
    return FALSE;
}


BOOL DcmFileStream::Putback(void)
{
    return this -> Putback(fNumPutbackBytes);
}


BOOL DcmFileStream::Putback(const Uint32 noBytes)
{
    BOOL result = FALSE;

    if (fPutbackMode && fFile)
    {
	// If stream has mode read, implement the Putback by setting
	// the file offset. 
	if (!fReadMode || noBytes > fNumPutbackBytes)
	    fErrorCond = EC_WrongStreamMode;

	else if (fErrorCond == EC_Normal)
	{
	    if (fseek(fFile, -Sint32(noBytes), SEEK_CUR))
	    {	
		fErrorCond = EC_InvalidStream;
	    }
	    else 
	    {
		fPutbackMode = FALSE;
		fNumPutbackBytes = 0;
		result = TRUE;
	    }
	}
    }
    return result;
}


void DcmFileStream::ReadBytes(void * bytes, const Uint32 length)
{
    if (length > 0 && fErrorCond == EC_Normal && fFile) 
    {
	if (!fReadMode)
	    fErrorCond = EC_WrongStreamMode;
	else
	{
	    if (1 != fread(bytes, length, 1, fFile))
		fErrorCond = EC_InvalidStream;
	    else
	    {
		fTransferredBytes = length;
		if (fPutbackMode)
		    fNumPutbackBytes += length;
	    }
	}
    }
}

void DcmFileStream::Seek(Sint32 offset)
{
    if (offset < 0 || Uint32(offset) > fNumBytes)
	fErrorCond = EC_InvalidStream;

    if (fErrorCond == EC_Normal && fFile)
    {
	if (fReadMode && fRandomAccess)
	{
	    if (fseek(fFile, (long)offset, SEEK_SET) != 0)
		fErrorCond = EC_InvalidStream;
	}
	else
	    fErrorCond = EC_WrongStreamMode;
    }
}


void DcmFileStream::SetPutbackMark(void)
{
    if (!fReadMode || !fFile)
	fErrorCond = EC_WrongStreamMode;
    else
	fPutbackMode = TRUE;
}


Uint32 DcmFileStream::Tell(void)
{
    if (fErrorCond == EC_Normal && fFile)
    {
	long where = ftell(fFile);
	if (where == -1L) {
	    fErrorCond = EC_InvalidStream;
	    return 0;
	} else {
	    return (Uint32)where;
	}
    }
    return 0;
}


void DcmFileStream::UnsetPutbackMark(void)
{
    if (!fReadMode || !fFile)
	fErrorCond = EC_WrongStreamMode;
    else
    {
	fPutbackMode = TRUE;
	fNumPutbackBytes = FALSE;
    }
}




void DcmFileStream::WriteBytes(const void * bytes, const Uint32 length)
{
    if (length > 0 && fErrorCond == EC_Normal && fFile)
    {
	if (fReadMode)
	    fErrorCond = EC_WrongStreamMode;
	else
	{
	    if (1 != fwrite(bytes, length, 1, fFile))
		fErrorCond = EC_InvalidStream;
	    else
		fTransferredBytes = length;
	}
    }
}

//
// CLASS DcmBufferStream
//

Uint32 DcmBufferStream::Avail(void)
{
    if (fErrorCond == EC_Normal || fErrorCond == EC_StreamNotifyClient)
    {
	if (fReadMode)
	    return fBuffer->AvailRead();
	else
	    return fBuffer->AvailWrite();
    }
    else
	return 0;
}


E_Condition DcmBufferStream::Avail(const Uint32 numBytes)
{
    E_Condition l_error = fErrorCond;
    if (fErrorCond == EC_Normal || fErrorCond == EC_StreamNotifyClient)
    {
	if (numBytes > fBuffer->GetLength())
	    l_error = EC_IllegalCall;
	else if (fReadMode && numBytes > fBuffer->AvailRead())
	{
	    if(fBuffer->EndOfBufferMarkSet())
		l_error = EC_EndOfStream;
	    else
		l_error = EC_StreamNotifyClient;
	}
	else if (!fReadMode && numBytes > fBuffer->AvailWrite())
	{
	    if (fBuffer->GetLength() == 0)
		l_error = EC_IllegalCall;
	    else
		l_error = EC_StreamNotifyClient;
	}
    }

    return l_error;
}


void DcmBufferStream::Close(void)
{
}


void DcmBufferStream::CopyFromBuffer(void * buffer)
{
    if (fErrorCond == EC_StreamNotifyClient)
	fErrorCond = EC_Normal;
    if (fErrorCond == EC_Normal && !fReadMode)
	fBuffer -> CopyFromBuffer(buffer);
    else if (fErrorCond == EC_Normal)
	fErrorCond = EC_WrongStreamMode;
}


DcmBufferStream::DcmBufferStream(const BOOL readMode) 
    : DcmStream(readMode, DCM_SequentialAccess)
{
    fErrorCond = EC_Normal;
    fBuffer = new DcmMemoryBuffer();
    if (!fBuffer)
	fErrorCond = EC_InvalidStream;

    if (fRandomAccess)
	fErrorCond = EC_WrongStreamMode;
}

DcmBufferStream::DcmBufferStream(const Uint32 length, 
				 const BOOL readMode)
    : DcmStream(readMode, DCM_SequentialAccess)
{
    fErrorCond = EC_Normal;
    if (length & 1)
    {
	fErrorCond = EC_InvalidStream;
	fBuffer = NULL;
    }
    else
    {
	fBuffer = new DcmMemoryBuffer(length);
	if (!fBuffer)
	    fErrorCond = EC_InvalidStream;

	if (fRandomAccess)
	    fErrorCond = EC_WrongStreamMode;
    }
}




DcmBufferStream::DcmBufferStream(void * buffer,
				 const Uint32 length, 
				 const BOOL readMode)
    : DcmStream(readMode, FALSE)
{
    fErrorCond = EC_Normal;
    if (length & 1)
    {
	fErrorCond = EC_InvalidStream;
	fBuffer = NULL;
    }
    else
    {
	fBuffer = new DcmMemoryBuffer(buffer, length);
	if (!fBuffer)
	    fErrorCond = EC_InvalidStream;

	if (fRandomAccess)
	    fErrorCond = EC_WrongStreamMode;
    }
}



DcmBufferStream::~DcmBufferStream(void)
{
    if (fBuffer)
	delete fBuffer;
}


BOOL DcmBufferStream::EndOfStream(void)
{
    if (fReadMode && 
	(fErrorCond == EC_Normal || fErrorCond == EC_StreamNotifyClient))
	return fBuffer->EndOfBuffer();
    else
	return FALSE;
}


void DcmBufferStream::FillBuffer(void * buffer, const Uint32 length)
{
    if (fErrorCond == EC_StreamNotifyClient)
	fErrorCond = EC_Normal;
    if (fErrorCond == EC_Normal && length & 1)
	fErrorCond = EC_InvalidStream;
    else if (fErrorCond == EC_Normal && fReadMode)
	fBuffer -> CopyIntoBuffer(buffer, length, length);
    else if (!fReadMode)
	fErrorCond = EC_WrongStreamMode;
}


BOOL DcmBufferStream::Flush(void)
{
    return FALSE;
}


void DcmBufferStream::GetBuffer(void * & buffer, Uint32 & length)
{
    if (fErrorCond == EC_StreamNotifyClient)
	fErrorCond = EC_Normal;
    if (fErrorCond == EC_Normal  && !fReadMode)
    {
	length = fBuffer -> GetFilled();
	fBuffer -> GetBuffer(buffer);
    }
    else
    {
	buffer = NULL;
	length = 0;
	if (fErrorCond == EC_Normal)
	    fErrorCond = EC_WrongStreamMode;
    }
}


Uint32 DcmBufferStream::GetBufferLength(void)
{
    if (fErrorCond == EC_Normal || fErrorCond == EC_StreamNotifyClient)
	return fBuffer -> GetFilled();
    else
	return 0;
}


DcmStreamConstructor * DcmBufferStream::NewConstructor(void)
{
    return NULL;
}



BOOL DcmBufferStream::MustFlush(void)
{
    return this -> GetBufferLength() != 0;
}


BOOL DcmBufferStream::Putback(void)
{
    if (fErrorCond == EC_Normal)
    {
	if (fReadMode)
	    return fBuffer -> Putback();
	else
	{
	    fErrorCond = EC_WrongStreamMode;
	    return FALSE;
	}
    }
    return FALSE;
}



BOOL DcmBufferStream::Putback(const Uint32 noBytes)
{
    if (fErrorCond == EC_Normal)
    {
	if (fReadMode)
	    return fBuffer -> Putback(noBytes);
	else
	{
	    fErrorCond = EC_WrongStreamMode;
	    return FALSE;
	}
    }
    return FALSE;
}

void DcmBufferStream::ReadBytes(void * bytes, const Uint32 length)
{
    if (length > 0 && fErrorCond == EC_Normal) 
    {
	if (!fReadMode)
	    fErrorCond = EC_WrongStreamMode;
	else
	{
	    if(fBuffer -> Read(bytes, length) == 0)
	    {
		fErrorCond = EC_StreamNotifyClient;
		fTransferredBytes = 0;
	    }
	    else
		fTransferredBytes = length;
	}
    }
    else if (length == 0)
	fTransferredBytes = 0;
}

void DcmBufferStream::ReleaseBuffer(void)
{
    if (fErrorCond == EC_Normal)
	fBuffer -> Release();
}
	

void DcmBufferStream::Seek(Sint32 /*offset*/)
{
    fErrorCond = EC_WrongStreamMode;
}


void DcmBufferStream::SetEndOfStream(void)
{
    if ((fErrorCond == EC_Normal || fErrorCond == EC_StreamNotifyClient) && 
	fReadMode)
	fBuffer->SetEndOfBuffer();
    else if (!fReadMode)
	fErrorCond = EC_WrongStreamMode;
}
		


void DcmBufferStream::SetBuffer(void * buffer, const Uint32 length)
{
    if (fErrorCond == EC_StreamNotifyClient)
	fErrorCond = EC_Normal;
    if (fErrorCond == EC_Normal && length & 1)
	fErrorCond = EC_InvalidStream;
    else if (fErrorCond == EC_Normal && !fBuffer -> BufferLocked())
	fBuffer -> SetBuffer(buffer, length, length);
    else if (fBuffer -> BufferLocked())
	fErrorCond = EC_InvalidStream;
}



void DcmBufferStream::SetPutbackMark(void)
{
    if (fErrorCond == EC_Normal && fReadMode)
	fBuffer -> SetPutbackMark();
    else if (!fReadMode)
	fErrorCond = EC_WrongStreamMode;
}


Uint32 DcmBufferStream::Tell(void)
{
    if (fErrorCond == EC_Normal || fErrorCond == EC_StreamNotifyClient)
    {
	if (fReadMode)
	    return fBuffer -> GetPosition();
	else
	{
	    fErrorCond = EC_WrongStreamMode;
	    return 0;
	}
    }
    return 0;
}


void DcmBufferStream::UnsetPutbackMark(void)
{
    if (fErrorCond == EC_Normal && fReadMode)
	fBuffer -> UnsetPutbackMark();
    else if (!fReadMode)
	fErrorCond = EC_Normal;
}
		


void DcmBufferStream::WriteBytes(const void * bytes, const Uint32 length)
{
    fTransferredBytes = 0;
    if (length > 0 && fErrorCond == EC_Normal)
    {
	if (fReadMode)
	    fErrorCond = EC_WrongStreamMode;
	else
	{
	    fTransferredBytes = fBuffer -> Write(bytes, length);
	    if (fTransferredBytes != length)
	    {
		if (fBuffer -> GetLength() == 0)
		    fErrorCond = EC_IllegalCall;
		else
		    fErrorCond = EC_StreamNotifyClient;
	    }
	}
    }
}



//
// CLASS DcmStreamConstructor
//

DcmStreamConstructor::DcmStreamConstructor(const BOOL readMode, 
					   const BOOL randomAccess)
{
    fReadMode = readMode;
    fRandomAccess = randomAccess;
}

DcmStreamConstructor::~DcmStreamConstructor(void)
{
}


//
// CLASS DcmFileStreamConstructor
//

DcmFileStreamConstructor::DcmFileStreamConstructor(const char * filename,
						   const BOOL readMode,
						   const BOOL randomAccess)
    : DcmStreamConstructor(readMode, randomAccess)
{
    if (filename)
    {
	fFilename = new char[strlen(filename)+1];
	if (fFilename)
	    strcpy(fFilename, filename);
    }
    else
	fFilename = NULL;
}


DcmFileStreamConstructor::~DcmFileStreamConstructor(void)
{
    if (fFilename)
	delete fFilename;
}


DcmStream * DcmFileStreamConstructor::NewDcmStream(void)
{
    DcmFileStream * newStream = new DcmFileStream(fFilename, 
						  fReadMode, fRandomAccess);
    if (newStream && newStream -> GetError() == EC_Normal)
	return newStream;
    else
	return NULL;
}

DcmStreamConstructor * 
DcmFileStreamConstructor::Copy(void)
{
    DcmFileStreamConstructor * copy = 
	new DcmFileStreamConstructor(fFilename, fReadMode, fRandomAccess);

    return copy;
}


/*
** CVS/RCS Log:
** $Log: dcstream.cc,v $
** Revision 1.7  1997-04-18 08:02:10  andreas
** - Make the Declaration of DcmBuffer local to the DcmStream classes
**
** Revision 1.6  1996/09/18 16:28:51  hewett
** Removed code which generated an error if DcmFileStream::Tell(void)
** was called when the stream was in write mode.  This behaviour was
** causing DcmDicomDir::write(...) to always fail.  In particular, this
** error was causing the program mknldir to write a DICOMDIR file
** containing only a meta-header.
**
** Revision 1.5  1996/01/23 17:29:24  andreas
** Support for old fashioned make without @^
** Support for machines that have mktemp but do not define it.
**
** Revision 1.4  1996/01/09 11:06:49  andreas
** New Support for Visual C++
** Correct problems with inconsistent const declarations
** Correct error in reading Item Delimitation Elements
**
** Revision 1.3  1996/01/05 13:27:42  andreas
** - changed to support new streaming facilities
** - unique read/write methods for file and block transfer
** - more cleanups
**
**
*/






