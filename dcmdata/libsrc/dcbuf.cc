/*
 *
 *  Copyright (C) 1994-99, OFFIS
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
 *  Purpose: buffer class
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2000-03-02 12:49:52 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/libsrc/Attic/dcbuf.cc,v $
 *  CVS/RCS Revision: $Revision: 1.8 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "osconfig.h"
#include <string.h>
#include "dcbuf.h"
#include "dcdefine.h"


//
//      CLASS DcmBuffer
//

DcmBuffer::DcmBuffer(void)
  : fLength(0),
    fFilled(0),
    fBuffer(NULL),
    fSelfAllocated(OFFalse)
{
}

DcmBuffer::DcmBuffer(const Uint32 length)
  : fLength(length),
    fFilled(0),
    fBuffer(NULL),
    fSelfAllocated(OFTrue)
{
    fBuffer = new unsigned char[length];
}


DcmBuffer::DcmBuffer(void * buffer,
                     const Uint32 filled,
                     const Uint32 length)
  : fLength(length),
    fFilled(filled),
    fBuffer(NULL),
    fSelfAllocated(OFTrue)
{
    fBuffer = new unsigned char[length];
    memcpy(fBuffer, (unsigned char *)buffer, size_t(filled));
}


DcmBuffer::~DcmBuffer(void)
{
    if (fSelfAllocated)
        delete[] fBuffer;
}




//
//      CLASS DcmMemoryBuffer
//

Uint32 DcmMemoryBuffer::AvailRead(void) const
{
    if (fCurrent == fBuffers[BACKUP])
        return fCurrent->fFilled + fBuffers[MAIN]->fFilled - fIndex;
    else 
        return fCurrent->fFilled - fIndex;
}


// DcmMemoryBuffer::CopyIntoBuffer
// Copy buffer into new allocate internal buffer
// Caution: If the old buffer was not completly parsed, copy the rest
//                      into fBackup.

void DcmMemoryBuffer::CopyIntoBuffer(void * buffer, 
                                     const Uint32 filled,
                                     const Uint32 length)
{
    this -> InstallBackupBuffer();
    DcmBuffer * newBuffer = new DcmBuffer(buffer, filled, length);
    OFBool mustSetCurrent = fCurrent == fBuffers[MAIN] || fCurrent == NULL;
    delete fBuffers[MAIN];
    fBuffers[MAIN] = newBuffer;
    if (mustSetCurrent)
        fCurrent = fBuffers[MAIN];
}

DcmMemoryBuffer::DcmMemoryBuffer(void)
  : fCurrent(NULL),
    fIndex(0),
    fPosition(0),
    fPutbackBuffer(NULL),
    fPutbackIndex(0),
    fPutbackNumber(0),
    fEndOfBuffer(OFFalse),
    fLockedBuffer(OFFalse)
{
    this -> Initialize();
    fBuffers[MAIN] = new DcmBuffer;
    fCurrent = fBuffers[MAIN];
}

DcmMemoryBuffer::DcmMemoryBuffer(const Uint32 length)
  : fCurrent(NULL),
    fIndex(0),
    fPosition(0),
    fPutbackBuffer(NULL),
    fPutbackIndex(0),
    fPutbackNumber(0),
    fEndOfBuffer(OFFalse),
    fLockedBuffer(OFFalse)
{
    this -> Initialize();
    fBuffers[MAIN] = new DcmBuffer(length);
    fCurrent = fBuffers[MAIN];
}

DcmMemoryBuffer::DcmMemoryBuffer(void * buffer, const Uint32 length)
  : fCurrent(NULL),
    fIndex(0),
    fPosition(0),
    fPutbackBuffer(NULL),
    fPutbackIndex(0),
    fPutbackNumber(0),
    fEndOfBuffer(OFFalse),
    fLockedBuffer(OFFalse)
{
    this -> Initialize();
    fBuffers[MAIN] = new DcmBuffer;
    this -> SetBuffer(buffer, 0, length);
    fCurrent = fBuffers[MAIN];
}


DcmMemoryBuffer::~DcmMemoryBuffer(void)
{
    delete fBuffers[MAIN];
    delete fBuffers[BACKUP];
}


void DcmMemoryBuffer::Initialize(void)
{
    fBuffers[MAIN] = NULL;
    fBuffers[BACKUP] = new DcmBuffer;
    fCurrent = NULL;
    fIndex = 0;
    fPosition = 0;
    fPutbackBuffer = NULL;
    fPutbackIndex = 0;
    fPutbackNumber = 0;
    fEndOfBuffer = OFFalse;
    fLockedBuffer = OFFalse;
}



// DcmMemoryBuffer::InstallBackupBuffer
// The buffering scheme has 2 possible instances:
// 1. The information is in the main buffer
// 2. The information begins in the backup buffer
//    and ends in the main buffer
// The purpose of this method is to free the main buffer
// and store the not definitive parsed information in the backup buffer.
// This mean the every bytes after the fIndex must be put into the
// backup buffer. If we want to putback information then the bytes after
// the putback mark must be transferred to the backup buffer.

void DcmMemoryBuffer::InstallBackupBuffer(void)
{
    DcmBuffer * backup = fBuffers[BACKUP];
    DcmBuffer * main = fBuffers[MAIN];


    if (fCurrent == main && fPutbackBuffer != backup)
    {   
        // The information is in the main buffer and the putback
        // mark does not exist or point also to the main buffer 

        // Get the index from which bytes must be copied into
        // the backup buffer 
        const Uint32 copyIndex = 
            fPutbackBuffer ? fPutbackIndex : fIndex;

        if (backup->fLength < main->fFilled - copyIndex)
        {       
            // old backup buffer is not large enough to get all
            // information, so make a new one and copy the not
            // definitely parsed bytes into it.

            delete fBuffers[BACKUP];

            fBuffers[BACKUP] = 
                new DcmBuffer(&(*main)[copyIndex],
                              main->fFilled - copyIndex,
                              main->fFilled - copyIndex);
            backup = fBuffers[BACKUP];
        }
        else
        {
            // old backup buffer is big enough
            // Copy not definitely parsed information into it
            if (main->fFilled > copyIndex)              // check for number of bytes to be copied
            {
                memcpy(backup->fBuffer, &(*main)[copyIndex], 
                       size_t(main->fFilled - copyIndex));
            }
            backup->fFilled = main->fFilled - copyIndex;
        }
        // Set the new index due to the putback mark
        fIndex = fPutbackBuffer ? fIndex - fPutbackIndex : 0;
    }
    else if (fCurrent == backup || fPutbackBuffer == backup)
    {   
        // The backup buffer is currently used and must be expanded
        // There are two Possibilities:
        // The not parsed information is in the backup buffer or the
        // putback mark points to the backup buffer.


        // Calculate putback mark and parsing index after copying all
        // information into the backup buffer
        Uint32 saveIndex = 0;
        if (fPutbackBuffer)
        {
            // putback mark is set, so set the beginning or the 
            // backuped information to the mark.
                        
            saveIndex = fPutbackIndex;

            // Set the parse index 
            if (fCurrent == backup)
                fIndex -= saveIndex;
            else        // Index is in main buffer and must be set
                // into backup buffer.
                fIndex += (backup->fFilled - saveIndex);
        }
        else if (fCurrent == backup)
        {
            // not putback.
            saveIndex = fIndex;
            fIndex = 0;
        }

        const Uint32 newLength = 
            backup->fFilled + main->fFilled - saveIndex;

        if (backup->fLength < newLength)
        {       
            // we need a larger backup-buffer
            DcmBuffer * newBuffer = 
                new DcmBuffer(&(*backup)[saveIndex], 
                              backup->fFilled - saveIndex, newLength);

            delete fBuffers[BACKUP];

            // append main buffer to new backup buffer
            memcpy(&(*newBuffer)[newBuffer->fFilled], main->fBuffer,
                   size_t(main->fFilled));
            newBuffer->fFilled = newLength;
            fBuffers[BACKUP] = newBuffer;
            backup = newBuffer;
        }
        else 
        {
            // Shorten backup buffer 
            // CAUTION: The memory areas overlap, use a copy routine
            // that can handle overlapping memory
            if (saveIndex)
            {
                memmove(backup->fBuffer, &(*backup)[saveIndex], 
                        size_t(backup->fFilled - saveIndex));
                backup->fFilled -= saveIndex;
            }


            // Copy main buffer after backup buffer
            if (main->fFilled)
            {
                memcpy(&(*backup)[backup->fFilled], main->fBuffer,
                       size_t(main->fFilled));
                backup->fFilled += main->fFilled;
            }
        }
    }

    // parsing begins now in backup buffer
    fCurrent = backup;
    main->fFilled = 0;

    // if putback mark exists, set the mark to the beginning of the
    // backup buffer
    if (fPutbackBuffer)
    {
        fPutbackBuffer = backup;
        fPutbackIndex = 0;
    }
}


OFBool DcmMemoryBuffer::Putback(void)
{
    if (fPutbackBuffer)
    {
        fCurrent = fPutbackBuffer;
        fIndex = fPutbackIndex;
        fPosition -= fPutbackNumber;
        fPutbackBuffer = NULL;
        fPutbackIndex = 0;
        fPutbackNumber = 0;
        return OFTrue;
    }
    else
        return OFFalse;
}


OFBool DcmMemoryBuffer::Putback(const Uint32 number)
{
    OFBool result = OFFalse;
    if (fPutbackBuffer)
    {
        if (fCurrent == fPutbackBuffer && fIndex - number >= fPutbackIndex)
        {
            fIndex -= number;
            result = OFTrue;
        }
        else if (fCurrent != fPutbackBuffer) 
        {       // Putback is backup buffer, current buffer is main buffer
            if (fIndex >= number)
            {
                fIndex -= number;
                result = OFTrue;
            }
            else if (fPutbackBuffer->fFilled - fPutbackIndex >= number - fIndex)
            {
                fIndex = fPutbackBuffer->fFilled - number + fIndex;
                fCurrent = fPutbackBuffer;
                result = OFTrue;
            }
        }
    }

    if (result)
    {
        fPosition -= number;
        fPutbackBuffer = NULL;
        fPutbackIndex = 0;
        fPutbackNumber = 0;
    }
        
    return result;
}


OFBool DcmMemoryBuffer::Putback(void * content, const Uint32 length)
{
    OFBool result = this -> Putback(length);
    if (result)
    {
        unsigned char * bytes = (unsigned char *)content;
        if (fCurrent == fBuffers[MAIN])
            memcpy(&(*fCurrent)[fIndex], bytes, size_t(length));
        else
        {
            const Uint32 currLength = 
                fCurrent -> fFilled - fIndex <= length ? 
                fCurrent -> fFilled - fIndex : length;
            memcpy(&(*fCurrent)[fIndex], bytes, size_t(currLength));
            if (length != currLength)
                memcpy(fBuffers[MAIN] -> fBuffer, 
                       &bytes[currLength], 
                       size_t(length - currLength));
        }
    }           
    return result;
}


Uint32 DcmMemoryBuffer::Read(void * content,
                             const Uint32 length)
{
    Uint32 readBytes = 0;
    unsigned char * bytes = (unsigned char *)content;
        
    if (fCurrent->fFilled - fIndex < length)
    {
        // In the current buffer are not enough bytes to read

        if (fCurrent == fBuffers[BACKUP] && 
            fBuffers[MAIN]->fFilled + fCurrent->fFilled >= length + fIndex)
        {
            // Backup and main buffer have enough bytes
            const Uint32 restLength = fCurrent->fFilled - fIndex;
            if (restLength)
                memcpy(bytes, &(*fCurrent)[fIndex], size_t(restLength));
            fCurrent = fBuffers[MAIN];
            fIndex = length - restLength;
            memcpy(&bytes[restLength], fCurrent->fBuffer, size_t(fIndex));
            readBytes = length;
        }
    }
    else
    {
        // There are enough bytes to read in the current buffer
        memcpy(bytes, &(*fCurrent)[fIndex], size_t(length));
        fIndex += length;
        readBytes = length;
    }
    fPosition += readBytes;
    if (fPutbackBuffer)
        fPutbackNumber += readBytes;

    return readBytes;
}



// DcmMemoryBuffer::Release
// This method releases the internal buffer ptr. It must be
// called before using the buffer by the client if the buffer is an reference
// got from the client (see SetBuffer). If it is not called then
// the class DcmMemoryBuffer becomes invalid.
void DcmMemoryBuffer::Release(void)
{
    this -> InstallBackupBuffer();
    fLockedBuffer = OFFalse;
}


// DcmMemoryBuffer::SetBuffer
// Set the fBuffer variable to the buffer parameter and load a new buffer
void DcmMemoryBuffer::SetBuffer(void * buffer, 
                                const Uint32 filled,
                                const Uint32 length)
{
    if (!fLockedBuffer)
    {
        fBuffers[MAIN]->SetBuffer(buffer, filled, length);
        fLockedBuffer = OFTrue;
    }
}


Uint32 DcmMemoryBuffer::Write(const void * content,
                              const Uint32 length)
{
    Uint32 noCopyBytes;
    if (fCurrent->fFilled + length >= fCurrent->fLength)
        noCopyBytes = fCurrent->fLength - fCurrent->fFilled;
    else
        noCopyBytes = length;

    memcpy(&(*fCurrent)[fCurrent->fFilled], 
           (unsigned char*)content, 
           size_t(noCopyBytes));
    fCurrent->fFilled += noCopyBytes;

    fPosition += noCopyBytes;
    return noCopyBytes;
}


/*
** CVS/RCS Log:
** $Log: dcbuf.cc,v $
** Revision 1.8  2000-03-02 12:49:52  joergr
** Rewrote some memory related statements (memcpy, strcpy, etc.) to avoid
** warnings reported by BoundsChecker.
**
** Revision 1.7  2000/02/02 14:32:46  joergr
** Replaced 'delete' statements by 'delete[]' for objects created with 'new[]'.
**
** Revision 1.6  1999/03/31 09:25:15  meichel
** Updated copyright header in module dcmdata
**
** Revision 1.5  1998/07/15 15:51:45  joergr
** Removed several compiler warnings reported by gcc 2.8.1 with
** additional options, e.g. missing copy constructors and assignment
** operators, initialization of member variables in the body of a
** constructor instead of the member initialization list, hiding of
** methods by use of identical names, uninitialized member variables,
** missing const declaration of char pointers. Replaced tabs by spaces.
**
** Revision 1.4  1997/07/24 13:12:34  andreas
** - Removed Warnings from SUN CC 2.0.1
**
** Revision 1.3  1997/07/21 08:25:24  andreas
** - Replace all boolean types (BOOLEAN, CTNBOOLEAN, DICOM_BOOL, BOOL)
**   with one unique boolean type OFBool.
**
** Revision 1.2  1996/01/09 11:06:42  andreas
** New Support for Visual C++
** Correct problems with inconsistent const declarations
** Correct error in reading Item Delimitation Elements
**
** Revision 1.1  1996/01/05 13:27:31  andreas
** - changed to support new streaming facilities
** - unique read/write methods for file and block transfer
** - more cleanups
**
*/
