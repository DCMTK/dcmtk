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
 *  Purpose: class DcmPixelSequence
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 1999-03-31 09:25:36 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/libsrc/dcpixseq.cc,v $
 *  CVS/RCS Revision: $Revision: 1.15 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include <stdlib.h>
#include <stdio.h>
#include <iostream.h>

#include "dcpixseq.h"
#include "dcpxitem.h"
#include "dcitem.h"
#include "dcvr.h"
#include "dcdebug.h"

#include "dcdeftag.h"


// ********************************


DcmPixelSequence::DcmPixelSequence(const DcmTag &tag,
                                   const Uint32 len)
  : DcmSequenceOfItems(tag, len),
    xfer(EXS_Unknown)
{
    Tag.setVR(EVR_OB);
}


// ********************************


DcmPixelSequence::DcmPixelSequence(const DcmPixelSequence &old)
  : DcmSequenceOfItems(old),
    xfer(old.xfer)
{
    /* everything gets handled in DcmSequenceOfItems constructor */
}


// ********************************


DcmPixelSequence::~DcmPixelSequence()
{
}

// ********************************


DcmPixelSequence &DcmPixelSequence::operator=(const DcmPixelSequence &obj)
{
  DcmSequenceOfItems::operator=(obj);
  xfer = obj.xfer;
  return *this;
}

// ********************************


void DcmPixelSequence::print(ostream & out, const OFBool showFullData,
                             const int level )
{
    char *info = new char[200];
    const char *title = (char*)NULL;
    if ( Length == DCM_UndefinedLength)
        title = "PixelSequence";
    else
        title = "PixelSequence with explicit Length";

    sprintf( info, "(%s #=%ld)", title, (long)card() );
    printInfoLine(out, showFullData, level, info );
    delete info;

    if ( !itemList->empty() )
    {
        DcmObject *dO;
        itemList->seek( ELP_first );
        do {
            dO = itemList->get();
            dO->print(out, showFullData, level + 1);
        } while ( itemList->seek( ELP_next ) );
    }
    DcmTag delimItemTag( DCM_SequenceDelimitationItem );

    if ( Length == DCM_UndefinedLength )
        printInfoLine(out, showFullData, level, delimItemTag,
                                 0, "(SequenceDelimitationItem)");
    else
        printInfoLine(out, showFullData, level, delimItemTag,
                   0, "(SequenceDelimitationItem for re-enc.)" );
}


// ********************************


E_Condition DcmPixelSequence::makeSubObject(DcmObject * & subObject,
                                            const DcmTag & newTag,
                                            const Uint32 newLength)
{
    E_Condition l_error = EC_Normal;
    DcmObject * newObject = NULL;

    switch ( newTag.getEVR() )
    {
    case EVR_na:
        if ( newTag.getXTag() == DCM_Item )
            newObject = new DcmPixelItem(newTag, newLength);
        else if (newTag.getXTag() == DCM_SequenceDelimitationItem)
            l_error = EC_SequEnd;
        else if (newTag.getXTag() == DCM_ItemDelimitationItem)
            l_error = EC_ItemEnd;
        else
            l_error = EC_InvalidTag;
        break;

    default:
        newObject = new DcmPixelItem(newTag, newLength);
        l_error = EC_CorruptedData;
        break;
    }

    subObject = newObject;
    return l_error;
}

// ********************************


E_Condition DcmPixelSequence::insert(DcmPixelItem* item,
                                       unsigned long where)
{
    errorFlag = EC_Normal;
    if ( item != NULL )
    {
        itemList->seek_to( where );
        itemList->insert( item );
        Cdebug(3, where< itemList->card(), ("DcmPixelSequence::insert() item at position %d inserted", where ));
        Cdebug(3, where>=itemList->card(), ("DcmPixelSequence::insert() item at last position inserted" ));

    }
    else
        errorFlag = EC_IllegalCall;
    return errorFlag;
}


// ********************************


E_Condition DcmPixelSequence::getItem(DcmPixelItem * & item, 
                                        const unsigned long num)
{
    errorFlag = EC_Normal;
    item = (DcmPixelItem*)( itemList->seek_to(num) );  // liest Item aus Liste
    if ( item == NULL )
        errorFlag = EC_IllegalCall;
    return errorFlag;
}


// ********************************


E_Condition DcmPixelSequence::remove(DcmPixelItem * & item, 
                                     const unsigned long num)
{
    errorFlag = EC_Normal;
    item = (DcmPixelItem*)( itemList->seek_to(num) );  // liest Item aus Liste
    if ( item != (DcmPixelItem*)NULL )
    {
        itemList->remove();
    }
    else
        errorFlag = EC_IllegalCall;
    return errorFlag;
}


// ********************************


E_Condition DcmPixelSequence::remove(DcmPixelItem* item)
{
    errorFlag = EC_IllegalCall;
    if ( !itemList->empty() && item != NULL )
    {
        DcmObject *dO;
        itemList->seek( ELP_first );
        do {
            dO = itemList->get();
            if ( dO == item )
            {
                itemList->remove();         // entfernt Element aus Liste,
                // aber loescht es nicht
                errorFlag = EC_Normal;
                break;
            }
        } while ( itemList->seek( ELP_next ) );
    }
    return errorFlag;
}

// ********************************

E_Condition DcmPixelSequence::changeXfer(const E_TransferSyntax newXfer)
{
    if (xfer == EXS_Unknown || canWriteXfer(newXfer, xfer))
    {
        xfer = newXfer;
        return EC_Normal;
    }
    else
        return EC_IllegalCall;
}
        

// ********************************

OFBool DcmPixelSequence::canWriteXfer(const E_TransferSyntax newXfer,
                                      const E_TransferSyntax oldXfer)
{
    DcmXfer newXferSyn(newXfer);

    return newXferSyn.isEncapsulated() && 
        newXfer == oldXfer && oldXfer == xfer;
}

// ********************************

E_Condition DcmPixelSequence::read(DcmStream & inStream,
                                   const E_TransferSyntax ixfer,
                                   const E_GrpLenEncoding glenc,
                                   const Uint32 maxReadLength)
{
    E_Condition l_error = changeXfer(ixfer);
    if (l_error == EC_Normal)
        return DcmSequenceOfItems::read(inStream, ixfer, glenc, maxReadLength);
    else
        return l_error;
}

// ********************************

E_Condition DcmPixelSequence::write(DcmStream & outStream,
                                      const E_TransferSyntax oxfer,
                                      const E_EncodingType /*enctype*/)
{
    E_Condition l_error = changeXfer(oxfer);
    if (l_error == EC_Normal)
        return DcmSequenceOfItems::write(outStream, oxfer, EET_UndefinedLength);
    else
        return l_error;
}


/*
** CVS/RCS Log:
** $Log: dcpixseq.cc,v $
** Revision 1.15  1999-03-31 09:25:36  meichel
** Updated copyright header in module dcmdata
**
** Revision 1.14  1998/11/12 16:48:18  meichel
** Implemented operator= for all classes derived from DcmObject.
**
** Revision 1.13  1998/07/15 15:52:05  joergr
** Removed several compiler warnings reported by gcc 2.8.1 with
** additional options, e.g. missing copy constructors and assignment
** operators, initialization of member variables in the body of a
** constructor instead of the member initialization list, hiding of
** methods by use of identical names, uninitialized member variables,
** missing const declaration of char pointers. Replaced tabs by spaces.
**
** Revision 1.12  1997/07/21 08:19:33  andreas
** - New environment for encapsulated pixel representations. DcmPixelData
**   can contain different representations and uses codecs to convert
**   between them. Codecs are derived from the DcmCodec class. New error
**   codes are introduced for handling of representations. New internal
**   value representation (only for ident()) for PixelData
** - Replace all boolean types (BOOLEAN, CTNBOOLEAN, DICOM_BOOL, BOOL)
**   with one unique boolean type OFBool.
**
** Revision 1.11  1997/07/07 07:51:36  andreas
** - Changed type for Tag attribute in DcmObject from prointer to value
** - Enhanced (faster) byte swapping routine. swapIfNecessary moved from
**   a method in DcmObject to a general function.
**
** Revision 1.10  1997/07/03 15:10:02  andreas
** - removed debugging functions Bdebug() and Edebug() since
**   they write a static array and are not very useful at all.
**   Cdebug and Vdebug are merged since they have the same semantics.
**   The debugging functions in dcmdata changed their interfaces
**   (see dcmdata/include/dcdebug.h)
**
** Revision 1.9  1997/06/06 09:55:30  andreas
** - corrected error: canWriteXfer returns false if the old transfer syntax
**   was unknown, which causes several applications to prohibit the writing
**   of dataset.
**
** Revision 1.8  1997/05/27 13:49:01  andreas
** - Add method canWriteXfer to class DcmObject and all derived classes.
**   This method checks whether it is possible to convert the original
**   transfer syntax to an new transfer syntax. The check is used in the
**   dcmconv utility to prohibit the change of a compressed transfer
**   syntax to a uncompressed.
**
** Revision 1.7  1997/05/26 15:08:15  andreas
** - Corrected typo in write method
**
** Revision 1.6  1997/05/22 16:57:15  andreas
** - Corrected errors for writing of pixel sequences for encapsulated
**   transfer syntaxes.
**
** Revision 1.5  1996/08/05 08:46:15  andreas
** new print routine with additional parameters:
**         - print into files
**         - fix output length for elements
** corrected error in search routine with parameter ESM_fromStackTop
**
** Revision 1.4  1996/01/29 13:38:29  andreas
** - new put method for every VR to put value as a string
** - better and unique print methods
**
** Revision 1.3  1996/01/05 13:27:40  andreas
** - changed to support new streaming facilities
** - unique read/write methods for file and block transfer
** - more cleanups
**
*/
