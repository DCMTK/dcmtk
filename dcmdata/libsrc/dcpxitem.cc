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
 *  Author:  Gerd Ehlers, Andreas Barth
 *
 *  Purpose: class DcmPixelItem
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2000-02-03 16:31:26 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/libsrc/dcpxitem.cc,v $
 *  CVS/RCS Revision: $Revision: 1.10 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#ifdef HAVE_STDLIB_H
#ifndef  _BCB_4
/* workaround for bug in Borland C++ Builder 4 */
BEGIN_EXTERN_C
#endif
#include <stdlib.h>
#ifndef  _BCB_4
END_EXTERN_C
#endif
#endif

#include <string.h>
#include <stdio.h>
#include <iostream.h>

#include "dcpxitem.h"
#include "dcswap.h"
#include "dcdebug.h"

// ********************************


DcmPixelItem::DcmPixelItem(const DcmTag &tag, const Uint32 len)
: DcmOtherByteOtherWord(tag, len)
{
    Tag.setVR(EVR_pixelItem);
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
	l_error = this -> writeTag(outStream, Tag, oxfer);
	writtenBytes = 4;

	Uint32 valueLength = Length;
	DcmXfer outXfer(oxfer);
	const E_ByteOrder oByteOrder = outXfer.getByteOrder();
	if (oByteOrder == EBO_unknown)
	{
	    return EC_IllegalCall;
	}
	swapIfNecessary(oByteOrder, gLocalByteOrder, &valueLength, 4, 4);
	outStream.WriteBytes(&valueLength, 4); // 4 Byte Laenge
	writtenBytes += 4;
    }

    return l_error;
}

/*
** CVS/RCS Log:
** $Log: dcpxitem.cc,v $
** Revision 1.10  2000-02-03 16:31:26  joergr
** Fixed bug: encapsulated data (pixel items) have never been loaded using
** method 'loadAllDataIntoMemory'. Therefore, encapsulated pixel data was
** never printed with 'dcmdump'.
** Corrected bug that caused wrong calculation of group length for sequence
** of items (e.g. encapsulated pixel data).
**
** Revision 1.9  2000/02/01 10:12:09  meichel
** Avoiding to include <stdlib.h> as extern "C" on Borland C++ Builder 4,
**   workaround for bug in compiler header files.
**
** Revision 1.8  1999/03/31 09:25:37  meichel
** Updated copyright header in module dcmdata
**
** Revision 1.7  1998/11/12 16:48:19  meichel
** Implemented operator= for all classes derived from DcmObject.
**
** Revision 1.6  1997/07/07 07:52:29  andreas
** - Enhanced (faster) byte swapping routine. swapIfNecessary moved from
**   a method in DcmObject to a general function.
**
** Revision 1.5  1997/07/03 15:10:03  andreas
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
*/
