/*
** Author: Andreas Barth	Created:	12.11.95
**							Modified:	21.11.95
** Kuratorium OFFIS e.V.
**
** Module: dcstream.h
**
** Purpose:
**	defines streaming classes for file and buffer input/output
**
** Last Update:		$Author: andreas $
** Update Date:		$Date: 1997-07-21 08:25:11 $
** Source File:		$Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/include/Attic/dcstream.h,v $
** CVS/RCS Revision:	$Revision: 1.5 $
** Status:		$State: Exp $
**
** CVS/RCS Log at end of file
**
*/

#ifndef DCSTREAM_H
#define DCSTREAM_H

#include "osconfig.h"

#include <stdio.h>
#include "dcerror.h"
#include "dctypes.h"

class DcmMemoryBuffer;

const OFBool DCM_ReadMode = OFTrue;
const OFBool DCM_WriteMode = OFFalse;

const OFBool DCM_RandomAccess = OFTrue;
const OFBool DCM_SequentialAccess = OFFalse;

class DcmStreamConstructor;

//
// CLASS DcmStream
//


class DcmStream
{

  protected:
	OFBool fReadMode;				// Stream readable or writable
	OFBool fRandomAccess;			// Stream random or sequentiall Access
	Uint32 fTransferredBytes;	// no of bytes from last r/w operation
	E_Condition fErrorCond;	   	// Error Condition
	

  public:

// CONSTRUCTOR

	DcmStream(const OFBool readMode, const OFBool randomAccess);
	virtual DcmStreamConstructor * NewConstructor(void) = 0;

// DESTRUCTOR
	
	virtual ~DcmStream(void);
	virtual void Close(void) = 0;
	

// LAST STREAM OPERATION


	// Get number of bytes of last r/w operation
	inline Uint32 TransferredBytes(void) const
	{ 
		return fTransferredBytes; 
	}

	// Get the number of bytes available in the stream
	virtual Uint32 Avail(void) = 0;

	// test Stream if number of Bytes are Available
	virtual E_Condition Avail(const Uint32 numBytes) = 0;


// FLUSHING STREAM

	virtual OFBool MustFlush(void) = 0;
	virtual OFBool Flush(void) = 0;


// ERROR HANDLING

	// Get Error Code
	inline E_Condition GetError(void) const
	{
		return fErrorCond;
	}

	inline OFBool Fail(void) const
	{
		return fErrorCond != EC_Normal;
	}
	

	// Reset error code if not fatal.
	inline void ClearError(void)
	{
			fErrorCond = EC_Normal;
	}	

// STREAM MODE

	// Is Stream readable or writeble
	inline OFBool IsReadMode(void) const
	{
		return fReadMode;
	}

	// Is Random Access possible ?
	inline OFBool HasRandomAccess(void) const
	{
		return fRandomAccess;
	}


// STREAM POSITION (FOR READING)

	// Get and Set stream position for next read operation
	// To use Seek functions the stream must be random
	virtual Uint32 Tell(void) = 0;
	virtual void Seek(Sint32 offset) = 0;
	
// HANDLING OF READ AND WRITE OPERATIONS

	virtual OFBool EndOfStream(void) = 0;

	// Putback of bytes. To use the putback routines, the stream must 
	// be in read mode and you must first set a putback mark. If you decide 
	// that no putback is necessary, you should unset the mark.
	virtual void SetPutbackMark(void) = 0;
	virtual void UnsetPutbackMark(void) = 0;
	virtual OFBool Putback(void) = 0;
	virtual OFBool Putback(const Uint32 noBytes) = 0;
	

	virtual void ReadBytes(void * bytes, const Uint32 length) = 0;

	virtual void WriteBytes(const void * bytes, const Uint32 length) = 0;
};


//
// CLASS DcmFileStream
//


class DcmFileStream : public DcmStream
{
  private:
	char * fFilename;			// Filename of file
	FILE * fFile;				// file for Input/Output
	Uint32 fNumBytes;			// Number of Bytes in file
	OFBool fPutbackMode;			// putback mark set ?
	Uint32 fNumPutbackBytes; 	// number of bytes to putback

  public:

// CONSTRUCTOR
	DcmFileStream(const char * filename, 
				  const OFBool readMode, 
				  const OFBool randomAccess = DCM_RandomAccess);
	virtual DcmStreamConstructor * NewConstructor(void);

// DESTRUCTOR

	virtual ~DcmFileStream(void);
	virtual void Close(void);
	
// LAST STREAM OPERATION

	virtual Uint32 Avail(void);
	virtual E_Condition Avail(const Uint32 numBytes);

// STREAM POSITION (FOR READING)

	virtual Uint32 Tell(void);
	virtual void Seek(Sint32 offset);

// FLUSHING STREAM

	virtual OFBool MustFlush(void);
	virtual OFBool Flush(void);


// HANDLING OF READ AND WRITE OPERATIONS


	virtual OFBool EndOfStream(void);

	virtual void SetPutbackMark(void);
	virtual void UnsetPutbackMark(void);

	virtual OFBool Putback(void);
	virtual OFBool Putback(const Uint32 noBytes);
	
	virtual void ReadBytes(void * bytes, const Uint32 length);

	virtual void WriteBytes(const void * bytes, const Uint32 length);
};



//
// CLASS DcmBufferStream
//

class DcmBufferStream : public DcmStream
{
  private:
	DcmMemoryBuffer * fBuffer;
	
  public:

// CONSTRUCTOR

	DcmBufferStream(const OFBool readMode);
	DcmBufferStream(const Uint32 length, const OFBool readMode);
	DcmBufferStream(void * buffer, const Uint32 length,
					const OFBool readMode);

	virtual DcmStreamConstructor * NewConstructor(void);

// DESTRUCTOR

	virtual ~DcmBufferStream(void);
	virtual void Close(void);
	
// LAST STREAM OPERATION

	virtual Uint32 Avail(void);
	virtual E_Condition Avail(const Uint32 numBytes);

// STREAM POSITION (FOR READING)

	virtual Uint32 Tell(void);
	virtual void Seek(Sint32 offset);

// FLUSHING STREAM

	virtual OFBool MustFlush(void);
	virtual OFBool Flush(void);


// HANDLING OF READ AND WRITE OPERATIONS

	void SetEndOfStream();
	virtual OFBool EndOfStream(void);

	virtual void SetPutbackMark(void);
	virtual void UnsetPutbackMark(void);
	virtual OFBool Putback(void);
	virtual OFBool Putback(const Uint32 noBytes);

	virtual void ReadBytes(void * bytes, const Uint32 length);

	virtual void WriteBytes(const void * bytes, const Uint32 length);
	
	
// SPECIAL BUFFER HANDLING

	void GetBuffer(void * & buffer, Uint32 & length);
	void CopyFromBuffer(void * buffer);
	Uint32 GetBufferLength(void);
	void ReleaseBuffer(void);
	void SetBuffer(void * buffer, const Uint32 length);
	void FillBuffer(void * buffer, const Uint32 length);
};	


//
// CLASS DcmStreamConstructor
//

class DcmStreamConstructor
{
  protected:
	OFBool fReadMode;
	OFBool fRandomAccess;

  public:
	DcmStreamConstructor(const OFBool readMode, const OFBool randomAccess);
	virtual ~DcmStreamConstructor(void);

	virtual DcmStream * NewDcmStream(void) = 0;
	virtual DcmStreamConstructor * Copy(void) = 0;
};


//
// CLASS DcmFileStreamConstructor
//

class DcmFileStreamConstructor : public DcmStreamConstructor
{
  private:
	char * fFilename;

  public:
	DcmFileStreamConstructor(const char * fFilename, const OFBool readMode,
							 const OFBool randomAccess);
	virtual ~DcmFileStreamConstructor(void);

	virtual DcmStream * NewDcmStream(void);
	virtual DcmStreamConstructor * Copy(void);
};


#endif // DCSTREAM_H


/*
** CVS/RCS Log:
** $Log: dcstream.h,v $
** Revision 1.5  1997-07-21 08:25:11  andreas
** - Replace all boolean types (BOOLEAN, CTNBOOLEAN, DICOM_BOOL, BOOL)
**   with one unique boolean type OFBool.
**
** Revision 1.4  1997/04/18 08:01:59  andreas
** - Make the Declaration of DcmBuffer local to the DcmStream classes
**
** Revision 1.3  1996/01/05 13:23:00  andreas
** - changed to support new streaming facilities
** - more cleanups
** - merged read / write methods for block and file transfer
**
**
*/

