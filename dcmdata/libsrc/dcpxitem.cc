/*
**
** Author: Gerd Ehlers	    01.11.94 -- Created
**         Andreas Barth    04.12.95 -- New Stream classes
** Kuratorium OFFIS e.V.
**
** Module: dcpxitem.cc
**
** Purpose:
** Implementation of class DcmPixelItem
**
**
** Last Update:		$Author: andreas $
** Update Date:		$Date: 1997-07-03 15:10:03 $
** Source File:		$Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/libsrc/dcpxitem.cc,v $
** CVS/RCS Revision:	$Revision: 1.5 $
** Status:		$State: Exp $
**
** CVS/RCS Log at end of file
**
*/

/*
 *
 * Author: Gerd Ehlers      Created:  11-01-94
 *                          Modified: 02-07-95
 *
 *
 * Last Update:   $Author: andreas $
 * Revision:      $Revision: 1.5 $
 * Status:	  $State: Exp $
 *
 */



#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream.h>

#include "dcpxitem.h"
#include "dcdebug.h"



// ********************************


DcmPixelItem::DcmPixelItem(const DcmTag &tag, const Uint32 len)
: DcmOtherByteOtherWord(tag, len)
{
	Tag -> setVR(EVR_OB);
}


// ********************************


DcmPixelItem::DcmPixelItem(const DcmPixelItem & old)
: DcmOtherByteOtherWord(old)
{
    if ( old.ident() != EVR_pixelItem )
    {
	errorFlag = EC_IllegalCall;
        cerr << "Warning: DcmPixelItem: wrong use of Copy-Constructor"
             << endl;
    }
    else
	Tag -> setVR(EVR_OB);
}


// ********************************


DcmPixelItem::~DcmPixelItem()
{
}

// ********************************

E_Condition DcmPixelItem::writeTagAndLength(DcmStream & outStream, 
					 const E_TransferSyntax oxfer,	
					 Uint32 & writtenBytes)	
{
    E_Condition l_error = outStream.GetError();
    if (l_error != EC_Normal)
	writtenBytes = 0;
    else
    {
	l_error = this -> writeTag(outStream, *Tag, oxfer);
	writtenBytes = 4;

	Uint32 valueLength = Length;
	DcmXfer outXfer(oxfer);
	const E_ByteOrder oByteOrder = outXfer.getByteOrder();
	if (oByteOrder == EBO_unknown)
	{
	    return EC_IllegalCall;
	}
	this -> swapIfNecessary(oByteOrder, gLocalByteOrder, 
				&valueLength, 4, 4);
	outStream.WriteBytes(&valueLength, 4); // 4 Byte Laenge
	writtenBytes += 4;
    }

    return l_error;
}


// ********************************

/*
** CVS/RCS Log:
** $Log: dcpxitem.cc,v $
** Revision 1.5  1997-07-03 15:10:03  andreas
** - removed debugging functions Bdebug() and Edebug() since
**   they write a static array and are not very useful at all.
**   Cdebug and Vdebug are merged since they have the same semantics.
**   The debugging functions in dcmdata changed their interfaces
**   (see dcmdata/include/dcdebug.h)
**
** Revision 1.4  1997/05/22 16:57:16  andreas
** - Corrected errors for writing of pixel sequences for encapsulated
**   transfer syntaxes.
**
** Revision 1.3  1996/01/05 13:27:41  andreas
** - changed to support new streaming facilities
** - unique read/write methods for file and block transfer
** - more cleanups
**
**
*/
