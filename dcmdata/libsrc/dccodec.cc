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
 *  Update Date:      $Date: 2001-09-25 17:19:09 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/libsrc/dccodec.cc,v $
 *  CVS/RCS Revision: $Revision: 1.7 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include "dccodec.h"
#include "oflist.h"
#include "ofthread.h"

typedef OFList<const DcmCodecStruct *> DcmCodecList;
typedef OFListIterator(const DcmCodecStruct *) DcmCodecIterator;

static  DcmCodecList globalCodecList;
static  OFMutex globalCodecMutex;

/* this function assumes that globalCodecMutex is locked.
 */
static DcmCodecIterator findInsertionPoint(const E_TransferSyntax repType)
{
    DcmCodecIterator it(globalCodecList.begin());
    for (;
	 it != globalCodecList.end() && 
	     (*it)->getRepresentationType() < repType;
	 ++it)
	    ;
    return it;
}

void registerGlobalCodec(const DcmCodecStruct * codecStruct)
{
    if (!codecStruct) return;
    globalCodecMutex.lock();
    // search to a codec with this repType
    DcmCodecIterator it(findInsertionPoint(codecStruct->getRepresentationType()));
    if ((it != globalCodecList.end()) && ((*it)->getRepresentationType() == codecStruct->getRepresentationType()))
      it = globalCodecList.erase(it);
    globalCodecList.insert(it, codecStruct);
    globalCodecMutex.unlock();
}

void deregisterGlobalCodec(const DcmCodecStruct * codecStruct)
{
    if (! codecStruct) return;
    globalCodecMutex.lock();
    DcmCodecIterator first = globalCodecList.begin();
    DcmCodecIterator last = globalCodecList.end();
    while ((first != last) && (*first != codecStruct)) ++first;
    if (first != last) globalCodecList.erase(first);    
    globalCodecMutex.unlock();
}

const DcmCodecStruct * searchGlobalCodec(const E_TransferSyntax repType)
{
    globalCodecMutex.lock();
    if (globalCodecList.size())
    {
	DcmCodecIterator it = findInsertionPoint(repType);
	if (it != globalCodecList.end() && (*it)->getRepresentationType() == repType)
	{
          globalCodecMutex.unlock();
	  return *it;
	}
    }
    globalCodecMutex.unlock();
    return NULL;
}

 
/*
** CVS/RCS Log:
** $Log: dccodec.cc,v $
** Revision 1.7  2001-09-25 17:19:09  meichel
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
