/*
**
** Author: Andreas Barth
** Kuratorium OFFIS e.V.
**
** Module: dccodec.cc
**
** Purpose:
** Interface of abstract class DcmCodec
**
** Last Update:		$Author: andreas $
** Update Date:		$Date: 1997-07-21 07:55:04 $
** Source File:		$Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/libsrc/dccodec.cc,v $
** CVS/RCS Revision:	$Revision: 1.1 $
**
** CVS/RCS Log at end of file
**
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
** Revision 1.1  1997-07-21 07:55:04  andreas
** - New environment for encapsulated pixel representations. DcmPixelData
**   can contain different representations and uses codecs to convert
**   between them. Codecs are derived from the DcmCodec class. New error
**   codes are introduced for handling of representations. New internal
**   value representation (only for ident()) for PixelData
**
** 
*/
