/*
** 
** Author: Andreas Barth 	Created:  09.11.95
**                          Modified: 18.11.95
** Kuratorium OFFIS e.V.
**
** Module: dcbuf.h
** 
** Purpose:
** 	This file contains a Buffer Definition
** 
** 
** Last Update:		$Author: andreas $
** Update Date:		$Date: 1996-01-09 11:06:13 $
** Source File:		$Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/include/Attic/dcbuf.h,v $
** CVS/RCS Revision:	$Revision: 1.2 $
** Status:		$State: Exp $
**
** CVS/RCS Log at end of file
**
*/

#ifndef DCBUF_H
#define DCBUF_H

#include "osconfig.h"
#include "dctypes.h"
#include "dcdefine.h"


#define MAIN 	0
#define BACKUP	1

class DcmMemoryBuffer;

//
//	CLASS DcmBuffer
//	This class implements a buffer that can be filled and indexes

class DcmBuffer
{
    friend class DcmMemoryBuffer;

private:
    Uint32 fLength; 		// physical buffer length
    Uint32 fFilled;		// filled part of buffer
    unsigned char * fBuffer;	// content of buffer
    BOOL fSelfAllocated;		// buffer allocated by this object ?

public:
    // Create a new Buffer without any content
    DcmBuffer(void);

    // Create a new buffer and fill it with parameters
    DcmBuffer(void * buffer, 
	      const Uint32 filled, 
	      const Uint32 length);

    // Create a new buffer with given length
    DcmBuffer(const Uint32 length);

    //	Delete buffer 
    ~DcmBuffer(void);

	
    // Set content of current buffer 
    inline void SetBuffer(void * buffer, 
			  const Uint32 filled,
			  const Uint32 length)
    {
	fBuffer = (unsigned char *)buffer;
	fFilled = filled;
	fLength = length;
    }
	
    // Indexing of current buffer 
    inline unsigned char & operator[](const Uint32 index) const
    {
	return fBuffer[index];
    }

};

//
//	CLASS DcmMemoryBuffer
//

class DcmMemoryBuffer
{
private:
    DcmBuffer *fBuffers[2];		// main and backup buffer
    DcmBuffer * fCurrent;		// current buffer for parsing
    Uint32 fIndex;				// read index of current buffer
    Uint32 fPosition;			// Number of bytes r/w to buffer
    DcmBuffer * fPutbackBuffer;	// buffer ptr for Putback
    Uint32 fPutbackIndex;		// index in Putback buffer
    Uint32 fPutbackNumber;		// number of Bytes putbacked
    BOOL fEndOfBuffer;			// No more information in buffer ?
    BOOL fLockedBuffer;			// internal buffer locked ?	

    // Common initialization of Attributes
    void Initialize(void);

    // Copy not parsed parts of main buffer into backup buffer
    void InstallBackupBuffer(void);

public:
    // Create buffer without content
    DcmMemoryBuffer(void);

    // Create buffer with initialized content
    DcmMemoryBuffer(const Uint32 length);

    // Create buffer and set content ptr from parameter
    DcmMemoryBuffer(void * buffer, const Uint32 length);
	

    // Destroy buffer
    ~DcmMemoryBuffer(void);


    // Set the internal buffer parameter to the clients buffer
    void SetBuffer(void * buffer, 
		   const Uint32 filled,
		   const Uint32 length);

    // Release clients buffer for using by the client again
    // must be used before buffer is set with SetBuffer
    void Release(void);

    inline BOOL BufferLocked(void) const
    {
	return fLockedBuffer;
    }

    // Make a copy of buffer parameter and fill into the buffer
    void CopyIntoBuffer(void * buffer, 
			const Uint32 filled,
			const Uint32 length);

    // Get Ptr of the main buffer
    inline void GetBuffer(void * & buffer)
    {
	buffer = fBuffers[MAIN]->fBuffer;
	fBuffers[MAIN]->fFilled = 0;
    }

	
    // Copy content of the main buffer into the parameter
    inline void CopyFromBuffer(void * buffer)
    {
	memcpy(buffer, fBuffers[MAIN]->fBuffer, fBuffers[MAIN]->fFilled);
	fBuffers[MAIN]->fFilled = 0;
    }

    // Returns the main write buffer filled length
    inline Uint32 GetFilled(void) const
    {
	return fBuffers[MAIN]->fFilled;
    }

    // Return the main buffers allocated length
    inline Uint32 GetLength(void) const
    {
	return fBuffers[MAIN]->fLength;
    }

    // Returns number of r/w to buffer
    inline Uint32 GetPosition(void) const
    {
	return fPosition;
    }

    // Read length element from buffer an puxot in into content, returns
    // read elements (0 or length)
    Uint32 Read(void * content, const Uint32 length);

    // Write content into buffer, returns written components
    Uint32 Write(const void * content, const Uint32 length);

    // Returns number of available bytes for reading the buffer
    Uint32 AvailRead(void) const;

    // Returns number of available bytes for writing the buffer
    inline Uint32 AvailWrite(void) const
    { 
	return fCurrent->fLength - fCurrent->fFilled; 
    }

    //  Inform buffer, that everything is loaded
    inline void SetEndOfBuffer(void)
    {
	fEndOfBuffer = TRUE;
    }

    // Is buffer at an End?
    inline BOOL EndOfBuffer(void) const
    {
	return fEndOfBuffer && fIndex == fCurrent-> fFilled &&
	    (fCurrent != fBuffers[BACKUP] || fBuffers[MAIN] == 0);
    }

    // Is EndOfBuffer mark set ?
    inline BOOL EndOfBufferMarkSet(void) const
    {
	return fEndOfBuffer;
    }

    // Putback mechanism for the buffer
    // To use the putback routines, you must first set a putback mark. 
    // If you decide that no putback is necessary, you should unset the mark.

    inline void SetPutbackMark(void)
    {
	fPutbackBuffer = fCurrent;
	fPutbackIndex = fIndex;
	fPutbackNumber = 0;
    }

    inline void UnsetPutbackMark(void)
    {
	fPutbackBuffer = NULL;
	fPutbackIndex = 0;
	fPutbackNumber = 0;
    }

    BOOL Putback(const Uint32 number);
    BOOL Putback(void * bytes, const Uint32 number);
    BOOL Putback(void);
};

#endif

/*
** CVS/RCS Log:
** $Log: dcbuf.h,v $
** Revision 1.2  1996-01-09 11:06:13  andreas
** New Support for Visual C++
** Correct problems with inconsistent const declarations
**
** Revision 1.1  1996/01/05 13:22:51  andreas
** - changed to support new streaming facilities
** - more cleanups
** - merged read / write methods for block and file transfer
**
**
*/
