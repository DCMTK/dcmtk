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
 *  Purpose: abstract class DcmCodec and the class DcmCodecStruct
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 2002-02-27 14:21:35 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/libsrc/dccodec.cc,v $
 *  CVS/RCS Revision: $Revision: 1.9 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "osconfig.h"    /* make sure OS specific configuration is included first */
#include "dccodec.h"
#include "oflist.h"
#include "ofthread.h"

// static member variables
OFList<DcmCodecList *> DcmCodecList::registeredCodecs;

#ifdef _REENTRANT
OFReadWriteLock DcmCodecList::codecLock;
#endif


DcmCodecList::DcmCodecList(
    const DcmCodec *aCodec,  
    const DcmRepresentationParameter *aDefaultRepParam,
    const DcmCodecParameter *aCodecParameter)
: codec(aCodec)
, defaultRepParam(aDefaultRepParam)
, codecParameter(aCodecParameter)
{
}

DcmCodecList::~DcmCodecList()
{
}

OFCondition DcmCodecList::registerCodec(
    const DcmCodec *aCodec,  
    const DcmRepresentationParameter *aDefaultRepParam,
    const DcmCodecParameter *aCodecParameter)
{
  if ((aCodec == NULL)||(aCodecParameter == NULL)) return EC_IllegalParameter;
#ifdef _REENTRANT
  if (! codecLock.initialized()) return EC_IllegalCall; // should never happen
#endif

  // acquire write lock on codec list.  Will block if some codec is currently active.
  OFCondition result = EC_Normal;
#ifdef _REENTRANT
  if (0 == codecLock.wrlock())
  {
#endif
    DcmCodecList *listEntry = new DcmCodecList(aCodec, aDefaultRepParam, aCodecParameter);
    if (listEntry)
    {
      // prevent codec from being registered twice
      OFListIterator(DcmCodecList *) first = registeredCodecs.begin();
      OFListIterator(DcmCodecList *) last = registeredCodecs.end();
      while (first != last)
      {
        if ((*first)->codec == aCodec)
        {
          // this codec is already registered.
          first = last;
          result = EC_IllegalCall;
        } else ++first;        
      }
      if (result.good()) registeredCodecs.push_back(listEntry); else delete listEntry;
    } else result = EC_MemoryExhausted;
#ifdef _REENTRANT
    codecLock.unlock();
  } else result = EC_IllegalCall;
#endif
  return result;
}

OFCondition DcmCodecList::deregisterCodec(const DcmCodec *aCodec)
{
  if (aCodec == NULL) return EC_IllegalParameter;
#ifdef _REENTRANT
  if (! codecLock.initialized()) return EC_IllegalCall; // should never happen
#endif
  // acquire write lock on codec list.  Will block if some codec is currently active.
  OFCondition result = EC_Normal;

#ifdef _REENTRANT
  if (0 == codecLock.wrlock())
  {
#endif
    OFListIterator(DcmCodecList *) first = registeredCodecs.begin();
    OFListIterator(DcmCodecList *) last = registeredCodecs.end();
    while (first != last)
    {
      if ((*first)->codec == aCodec)
      {
      	delete *first;
      	first = registeredCodecs.erase(first);
      } else ++first;
    }
#ifdef _REENTRANT
    codecLock.unlock();
  } else result = EC_IllegalCall;
#endif
  return result;
} 

OFCondition DcmCodecList::updateCodecParameter(
    const DcmCodec *aCodec,
    const DcmCodecParameter *aCodecParameter)
{
  if ((aCodec == NULL)||(aCodecParameter == NULL)) return EC_IllegalParameter;
#ifdef _REENTRANT
  if (! codecLock.initialized()) return EC_IllegalCall; // should never happen
#endif
  // acquire write lock on codec list.  Will block if some codec is currently active.
  OFCondition result = EC_Normal;

#ifdef _REENTRANT
  if (0 == codecLock.wrlock())
  {
#endif
    OFListIterator(DcmCodecList *) first = registeredCodecs.begin();
    OFListIterator(DcmCodecList *) last = registeredCodecs.end();
    while (first != last)
    {
      if ((*first)->codec == aCodec) (*first)->codecParameter = aCodecParameter;
      ++first;
    }
#ifdef _REENTRANT
    codecLock.unlock();
  } else result = EC_IllegalCall;
#endif
  return result;
}


OFCondition DcmCodecList::decode(
    const DcmXfer & fromType,
    const DcmRepresentationParameter * fromParam,
    DcmPixelSequence * fromPixSeq,
    DcmPolymorphOBOW& uncompressedPixelData,
    DcmStack & pixelStack)
{
#ifdef _REENTRANT
  if (! codecLock.initialized()) return EC_IllegalCall; // should never happen
#endif
  OFCondition result = EC_CannotChangeRepresentation;

  // acquire write lock on codec list.  Will block if some write lock is currently active.
#ifdef _REENTRANT
  if (0 == codecLock.rdlock())
  {
#endif
    E_TransferSyntax fromXfer = fromType.getXfer();
    OFListIterator(DcmCodecList *) first = registeredCodecs.begin();
    OFListIterator(DcmCodecList *) last = registeredCodecs.end();
    while (first != last)
    {
      if ((*first)->codec->canChangeCoding(fromXfer, EXS_LittleEndianExplicit))
      {
        result = (*first)->codec->decode(fromParam, fromPixSeq, uncompressedPixelData, (*first)->codecParameter, pixelStack);
        first = last;
      } else ++first;
    }
#ifdef _REENTRANT
    codecLock.unlock();
  } else result = EC_IllegalCall;
#endif
  return result;
}

OFCondition DcmCodecList::encode(
    const E_TransferSyntax fromRepType,
    const DcmRepresentationParameter * fromParam, 
    DcmPixelSequence * fromPixSeq, 
    const E_TransferSyntax toRepType,
    const DcmRepresentationParameter * toRepParam,
    DcmPixelSequence * & toPixSeq,
    DcmStack & pixelStack)
{
  toPixSeq = NULL;
#ifdef _REENTRANT
  if (! codecLock.initialized()) return EC_IllegalCall; // should never happen
#endif
  OFCondition result = EC_CannotChangeRepresentation;

  // acquire write lock on codec list.  Will block if some write lock is currently active.
#ifdef _REENTRANT
  if (0 == codecLock.rdlock())
  {
#endif
    OFListIterator(DcmCodecList *) first = registeredCodecs.begin();
    OFListIterator(DcmCodecList *) last = registeredCodecs.end();
    while (first != last)
    {
      if ((*first)->codec->canChangeCoding(fromRepType, toRepType))
      {
        if (!toRepParam) toRepParam = (*first)->defaultRepParam;
        result = (*first)->codec->encode(fromRepType, fromParam, fromPixSeq, 
                 toRepParam, toPixSeq, (*first)->codecParameter, pixelStack);
        first = last;
      } else ++first;
    }
#ifdef _REENTRANT
    codecLock.unlock();
  } else result = EC_IllegalCall;
#endif

  return result;
}

OFCondition DcmCodecList::encode(
    const E_TransferSyntax fromRepType,
    const Uint16 * pixelData,
    const Uint32 length,
    const E_TransferSyntax toRepType,
    const DcmRepresentationParameter * toRepParam,
    DcmPixelSequence * & toPixSeq,
    DcmStack & pixelStack)
{
  toPixSeq = NULL;
#ifdef _REENTRANT
  if (! codecLock.initialized()) return EC_IllegalCall; // should never happen
#endif
  OFCondition result = EC_CannotChangeRepresentation;

  // acquire write lock on codec list.  Will block if some write lock is currently active.
#ifdef _REENTRANT
  if (0 == codecLock.rdlock())
  {
#endif
    OFListIterator(DcmCodecList *) first = registeredCodecs.begin();
    OFListIterator(DcmCodecList *) last = registeredCodecs.end();
    while (first != last)
    {
      if ((*first)->codec->canChangeCoding(fromRepType, toRepType))
      {
        if (!toRepParam) toRepParam = (*first)->defaultRepParam;
        result = (*first)->codec->encode(pixelData, length, toRepParam, toPixSeq, 
                 (*first)->codecParameter, pixelStack);
        first = last;
      } else ++first;
    }
#ifdef _REENTRANT
    codecLock.unlock();
  } else result = EC_IllegalCall;
#endif

  return result;
}

OFBool DcmCodecList::canChangeCoding(
    const E_TransferSyntax fromRepType,
    const E_TransferSyntax toRepType)
{
#ifdef _REENTRANT
  if (! codecLock.initialized()) return OFFalse; // should never happen
#endif
  OFBool result = OFFalse;

  // acquire write lock on codec list.  Will block if some write lock is currently active.
#ifdef _REENTRANT
  if (0 == codecLock.rdlock())
  {
#endif
    OFListIterator(DcmCodecList *) first = registeredCodecs.begin();
    OFListIterator(DcmCodecList *) last = registeredCodecs.end();
    while (first != last)
    {
      if ((*first)->codec->canChangeCoding(fromRepType, toRepType))
      {
      	result = OFTrue;
        first = last;
      } else ++first;
    }
#ifdef _REENTRANT
    codecLock.unlock();
  }
#endif

  return result;
}

/*
** CVS/RCS Log:
** $Log: dccodec.cc,v $
** Revision 1.9  2002-02-27 14:21:35  meichel
** Declare dcmdata read/write locks only when compiled in multi-thread mode
**
** Revision 1.8  2001/11/08 16:19:42  meichel
** Changed interface for codec registration. Now everything is thread-safe
**   and multiple codecs can be registered for a single transfer syntax (e.g.
**   one encoder and one decoder).
**
** Revision 1.7  2001/09/25 17:19:09  meichel
** Updated abstract class DcmCodecParameter for use with dcmjpeg.
**   Added new function deregisterGlobalCodec().
**
** Revision 1.6  2001/06/01 15:48:59  meichel
** Updated copyright header
**
** Revision 1.5  2000/09/27 08:19:57  meichel
** Minor changes in DcmCodec interface, required for future dcmjpeg module.
**
** Revision 1.4  2000/04/14 16:09:16  meichel
** Made function DcmCodec and related functions thread safe.
**   registerGlobalCodec() should not be called anymore from the constructor
**   of global objects.
**
** Revision 1.3  2000/03/08 16:26:30  meichel
** Updated copyright header.
**
** Revision 1.2  1999/03/31 09:25:18  meichel
** Updated copyright header in module dcmdata
**
** Revision 1.1  1997/07/21 07:55:04  andreas
** - New environment for encapsulated pixel representations. DcmPixelData
**   can contain different representations and uses codecs to convert
**   between them. Codecs are derived from the DcmCodec class. New error
**   codes are introduced for handling of representations. New internal
**   value representation (only for ident()) for PixelData
** 
*/
