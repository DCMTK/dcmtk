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
 *  Update Date:      $Date: 2001-09-25 17:19:54 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/libsrc/Attic/dcstream.cc,v $
 *  CVS/RCS Revision: $Revision: 1.21 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "osconfig.h"

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#include <iostream.h>
#include <string.h>

#include "ofconsol.h"
#include "dcstream.h"
#include "dcbuf.h"

//
// CLASS DcmStream
//

DcmStream::DcmStream(const OFBool readMode, const OFBool randomAccess)
  : fReadMode(readMode),
    fRandomAccess(randomAccess),
    fTransferredBytes(0),
    fErrorCond(EC_Normal)
{
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
        if (fReadMode)  // no of bytes from postion to eof
            return fNumBytes - (Uint32)ftell(fFile);
        else    // maximum number of bytes in file system
            return Uint32(-1);
    }
    return 0;
}


OFCondition DcmFileStream::Avail(const Uint32 numBytes)
{
    OFCondition l_error = fErrorCond;
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
                             const OFBool readMode,
                             const OFBool randomAccess)
  : DcmStream(readMode, randomAccess),
    fFilename(NULL),
    fFile(NULL),
    fNumBytes(0),
    fPutbackMode(OFFalse),
    fNumPutbackBytes(0)
{
    fErrorCond = EC_Normal;

    if (filename)
    {
        fFilename = new char[strlen(filename)+1];
        if (fFilename)
            strcpy(fFilename, filename);

        if (fReadMode)
            fFile = fopen(filename, "rb");              // open read only
        else
            fFile = fopen(filename, "wb");              // open write only

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
}


DcmFileStream::~DcmFileStream(void)
{
    this -> Close();
    if (fFilename)
        delete[] fFilename;
}



OFBool DcmFileStream::EndOfStream(void)
{
    if (!fFile)
        fErrorCond = EC_IllegalCall;

    if (fErrorCond == EC_Normal)
    {
        OFBool ret = feof(fFile);
        if (!ret)
            ret = fNumBytes <= this -> Tell();
        return ret;
    }
    else
        return OFFalse;
}

OFBool DcmFileStream::Flush(void)
{
    if (fErrorCond != EC_InvalidStream && fFile)
        fflush(fFile); 
    return OFTrue;
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


OFBool DcmFileStream::MustFlush(void)
{
    return OFFalse;
}


OFBool DcmFileStream::Putback(void)
{
    return this -> Putback(fNumPutbackBytes);
}


OFBool DcmFileStream::Putback(const Uint32 noBytes)
{
    OFBool result = OFFalse;

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
                fPutbackMode = OFFalse;
                fNumPutbackBytes = 0;
                result = OFTrue;
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
            if (1 != fread(bytes, size_t(length), 1, fFile))
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
        fPutbackMode = OFTrue;
#ifdef DEBUG
    if (fNumPutbackBytes >0)
    {
      ofConsole.lockCerr() << "dcmdata warning: putback mark set twice in DcmFileStream, ignored." << endl;
      ofConsole.unlockCerr();
    }
#endif
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
        fPutbackMode = OFFalse;
        fNumPutbackBytes = 0;
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
            if (1 != fwrite(bytes, size_t(length), 1, fFile))
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


OFCondition DcmBufferStream::Avail(const Uint32 numBytes)
{
    OFCondition l_error = fErrorCond;
    if (fErrorCond == EC_Normal || fErrorCond == EC_StreamNotifyClient)
    {
       if (fReadMode && numBytes > fBuffer->AvailRead())
        {
            if(fBuffer->EndOfBufferMarkSet())
                l_error = EC_EndOfStream;
            else
                l_error = EC_StreamNotifyClient;
        }
        else if (!fReadMode && numBytes > fBuffer->AvailWrite())
        {
            if (fBuffer->GetLength() < numBytes)  // fail because the buffer is too small even if flushed
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


DcmBufferStream::DcmBufferStream(const OFBool readMode) 
  : DcmStream(readMode, DCM_SequentialAccess),
    fBuffer(NULL)
{
    fBuffer = new DcmMemoryBuffer();
    if (!fBuffer)
        fErrorCond = EC_InvalidStream;

    if (fRandomAccess)
        fErrorCond = EC_WrongStreamMode;
}

DcmBufferStream::DcmBufferStream(const Uint32 length, 
                                 const OFBool readMode)
  : DcmStream(readMode, DCM_SequentialAccess),
    fBuffer(NULL)
{
    if (length & 1)
        fErrorCond = EC_InvalidStream;
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
                                 const OFBool readMode)
   : DcmStream(readMode, OFFalse),
     fBuffer(NULL)
{
    if (length & 1)
        fErrorCond = EC_InvalidStream;
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


OFBool DcmBufferStream::EndOfStream(void)
{
    if (fReadMode && 
        (fErrorCond == EC_Normal || fErrorCond == EC_StreamNotifyClient))
        return fBuffer->EndOfBuffer();
    else
        return OFFalse;
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


OFBool DcmBufferStream::Flush(void)
{
    return OFFalse;
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



OFBool DcmBufferStream::MustFlush(void)
{
    return this -> GetBufferLength() != 0;
}


OFBool DcmBufferStream::Putback(void)
{
    if (fErrorCond == EC_Normal)
    {
        if (fReadMode)
            return fBuffer -> Putback();
        else
        {
            fErrorCond = EC_WrongStreamMode;
            return OFFalse;
        }
    }
    return OFFalse;
}



OFBool DcmBufferStream::Putback(const Uint32 noBytes)
{
    if (fErrorCond == EC_Normal)
    {
        if (fReadMode)
            return fBuffer -> Putback(noBytes);
        else
        {
            fErrorCond = EC_WrongStreamMode;
            return OFFalse;
        }
    }
    return OFFalse;
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

DcmStreamConstructor::DcmStreamConstructor(const OFBool readMode, 
                                           const OFBool randomAccess)
  : fReadMode(readMode),
    fRandomAccess(randomAccess)
{
}

DcmStreamConstructor::~DcmStreamConstructor(void)
{
}


//
// CLASS DcmFileStreamConstructor
//

DcmFileStreamConstructor::DcmFileStreamConstructor(const char * filename,
                                                   const OFBool readMode,
                                                   const OFBool randomAccess)
  : DcmStreamConstructor(readMode, randomAccess),
    fFilename(NULL)
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
        delete[] fFilename;
}


DcmStream * DcmFileStreamConstructor::NewDcmStream(void)
{
    DcmFileStream * newStream = new DcmFileStream(fFilename, 
                                                  fReadMode, fRandomAccess);
    if (newStream && newStream -> GetError() == EC_Normal)
        return newStream;
    else
    {
      delete newStream;
      return NULL;
    }
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
** Revision 1.21  2001-09-25 17:19:54  meichel
** Adapted dcmdata to class OFCondition
**
** Revision 1.20  2001/06/01 15:49:10  meichel
** Updated copyright header
**
** Revision 1.19  2000/04/14 15:55:07  meichel
** Dcmdata library code now consistently uses ofConsole for error output.
**
** Revision 1.18  2000/03/08 16:26:41  meichel
** Updated copyright header.
**
** Revision 1.17  2000/03/03 14:05:36  meichel
** Implemented library support for redirecting error messages into memory
**   instead of printing them to stdout/stderr for GUI applications.
**
** Revision 1.16  2000/02/29 12:21:11  meichel
** Fixed bug in dcmdata that could cause the parser to return
**   an EC_IllegalCall flag when parsing very small packets.
**
** Revision 1.15  2000/02/02 14:32:54  joergr
** Replaced 'delete' statements by 'delete[]' for objects created with 'new[]'.
**
** Revision 1.14  1999/03/31 09:25:40  meichel
** Updated copyright header in module dcmdata
**
** Revision 1.13  1998/10/20 08:20:21  meichel
** Closed some memory leaks in dcmdata and dcmnet libraries.
**
** Revision 1.12  1998/07/15 15:52:08  joergr
** Removed several compiler warnings reported by gcc 2.8.1 with
** additional options, e.g. missing copy constructors and assignment
** operators, initialization of member variables in the body of a
** constructor instead of the member initialization list, hiding of
** methods by use of identical names, uninitialized member variables,
** missing const declaration of char pointers. Replaced tabs by spaces.
**
** Revision 1.11  1998/03/25 16:16:06  meichel
** Corrected bug in DcmFileStream::UnsetPutbackMark. This bug caused
**   dcmdata to choke on files with metaheader where the value of
**   (0002,0010) TransferSyntaxUID was none of the supported UIDs,
**   because interpretation of the dataset started in the wrong file position.
**
** Revision 1.10  1997/07/24 13:10:53  andreas
** - Removed Warnings from SUN CC 2.0.1
**
** Revision 1.9  1997/07/21 08:25:30  andreas
** - Replace all boolean types (BOOLEAN, CTNBOOLEAN, DICOM_BOOL, BOOL)
**   with one unique boolean type OFBool.
**
** Revision 1.8  1997/07/04 09:27:48  meichel
** Now including <stream.h> in dcstream.cc,
** required for systems without <unistd.h>.
**
** Revision 1.7  1997/04/18 08:02:10  andreas
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
*/
