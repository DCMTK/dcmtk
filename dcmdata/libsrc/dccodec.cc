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
 *  Purpose: abstract class DcmCodec and the class DcmCodecStruct
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 1999-03-31 09:25:18 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/libsrc/dccodec.cc,v $
 *  CVS/RCS Revision: $Revision: 1.2 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include "dccodec.h"
#include "oflist.h"

typedef OFList<DcmCodecStruct *> DcmCodecList;
typedef OFListIterator(DcmCodecStruct *) DcmCodecIterator;


static  DcmCodecList * globalCodecList;
/* must not have a default value!
   Ansi C++ guarantees that this pointer is initialized to 0
   prior to any other initialization, e.g. prior to all constructor calls. */

static DcmCodecIterator findInsertionPoint(const E_TransferSyntax repType)
{
    DcmCodecIterator it(globalCodecList->begin());
    for (;
	 it != globalCodecList->end() && 
	     (*it)->getRepresentationType() < repType;
	 ++it)
	    ;
    return it;
}

void registerGlobalCodec(DcmCodecStruct * codecStruct)
{
    if (!codecStruct) return;

    // create global codec list if it does not exist
    if (!globalCodecList)
	globalCodecList = new DcmCodecList();

    // search to a codec with this repType
    DcmCodecIterator it(
	findInsertionPoint(
	    codecStruct->getRepresentationType()));

    if ((*it)->getRepresentationType() == 
	codecStruct->getRepresentationType())
	it = globalCodecList->erase(it);
    globalCodecList->insert(it, codecStruct);
}

const DcmCodecStruct * searchGlobalCodec(const E_TransferSyntax repType)
{
    if (globalCodecList)
    {
	DcmCodecIterator it = findInsertionPoint(repType);
	if (it != globalCodecList->end() && 
	    (*it)->getRepresentationType() == repType)
	    return *it;
    }
    return NULL;
}

 
/*
** CVS/RCS Log:
** $Log: dccodec.cc,v $
** Revision 1.2  1999-03-31 09:25:18  meichel
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
