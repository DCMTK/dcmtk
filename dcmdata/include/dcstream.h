/*
 *
 *  Copyright (C) 1994-2001, OFFIS
 *
 *  This software and supporting documentation were developed by
 *
 *    Kuratorium OFFIS e.V.
 *    Healthcare Information and Communication Systems
 *    Escherweg 2
 *    D-26121 Oldenburg, Germany
 *
 *  THIS SOFTWARE IS MADE AVAILABLE,  AS IS,  AND OFFIS MAKES NO  WARRANTY
 *  REGARDING  THE  SOFTWARE,  ITS  PERFORMANCE,  ITS  MERCHANTABILITY  OR
 *  FITNESS FOR ANY PARTICULAR USE, FREEDOM FROM ANY COMPUTER DISEASES  OR
 *  ITS CONFORMITY TO ANY SPECIFICATION. THE ENTIRE RISK AS TO QUALITY AND
 *  PERFORMANCE OF THE SOFTWARE IS WITH THE USER.
 *
 *  Module:  dcmdata
 *
 *  Author:  Andreas Barth
 *
 *  Purpose: streaming classes for file and buffer input/output
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 2001-06-01 15:48:44 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/include/Attic/dcstream.h,v $
 *  CVS/RCS Revision: $Revision: 1.10 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
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
    OFBool fReadMode;           // Stream readable or writable
    OFBool fRandomAccess;       // Stream random or sequentiall Access
    Uint32 fTransferredBytes;   // no of bytes from last r/w operation
    E_Condition fErrorCond;     // Error Condition
    

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
    char * fFilename;           // Filename of file
    FILE * fFile;               // file for Input/Output
    Uint32 fNumBytes;           // Number of Bytes in file
    OFBool fPutbackMode;        // putback mark set ?
    Uint32 fNumPutbackBytes;    // number of bytes to putback

 // --- declarations to avoid compiler warnings
 
    DcmFileStream(const DcmFileStream &);
    DcmFileStream &operator=(const DcmFileStream &);
  

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
    
 // --- declarations to avoid compiler warnings
 
    DcmBufferStream(const DcmBufferStream &);
    DcmBufferStream &operator=(const DcmBufferStream &);
  

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

// --- declarations to avoid compiler warnings
 
    DcmFileStreamConstructor(const DcmFileStreamConstructor &);
    DcmFileStreamConstructor &operator=(const DcmFileStreamConstructor &);

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
** Revision 1.10  2001-06-01 15:48:44  meichel
** Updated copyright header
**
** Revision 1.9  2000/03/08 16:26:18  meichel
** Updated copyright header.
**
** Revision 1.8  1999/03/31 09:24:47  meichel
** Updated copyright header in module dcmdata
**
** Revision 1.7  1998/07/15 15:48:53  joergr
** Removed several compiler warnings reported by gcc 2.8.1 with
** additional options, e.g. missing copy constructors and assignment
** operators, initialization of member variables in the body of a
** constructor instead of the member initialization list, hiding of
** methods by use of identical names, uninitialized member variables,
** missing const declaration of char pointers. Replaced tabs by spaces.
**
** Revision 1.6  1997/09/11 15:13:11  hewett
** Modified getOFString method arguments by removing a default value
** for the pos argument.  By requiring the pos argument to be provided
** ensures that callers realise getOFString only gets one component of
** a multi-valued string.
**
** Revision 1.5  1997/07/21 08:25:11  andreas
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
*/

