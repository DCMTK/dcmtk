/*
**
** Author: Gerd Ehlers      26.04.94 -- Created
**         Andreas Barth    02.12.95 -- Modified for new stream classes
** Kuratorium OFFIS e.V.
**
** Module: dcdatset.cc
**
** Purpose:
** Implementation of the class DcmDataset
**
**
** Last Update:		$Author: andreas $
** Update Date:		$Date: 1997-06-06 09:55:28 $
** Source File:		$Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/libsrc/dcdatset.cc,v $
** CVS/RCS Revision:	$Revision: 1.10 $
** Status:		$State: Exp $
**
** CVS/RCS Log at end of file
**
*/


#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include <stdio.h>
#include <stdlib.h>
#include <iostream.h>
#include <string.h>

#include "dcdatset.h"
#include "dcxfer.h"
#include "dcvrus.h"
#include "dcdebug.h"

#include "dcdeftag.h"


// ********************************


DcmDataset::DcmDataset()
    : DcmItem(ItemTag, DCM_UndefinedLength)
{
    Bdebug((5, "dcdatset:DcmDataset::DcmDataset()" ));
    debug(( 8, "Object pointer this=0x%p", this ));

    Xfer = EXS_Unknown;

    Edebug(());

}



// ********************************


DcmDataset::DcmDataset(const DcmDataset &old)
    : DcmItem( old )
{
    Bdebug((5, "dcdatset:DcmDataset::DcmDataset(DcmDataset&)" ));
    debug(( 8, "Object pointer this=0x%p", this ));
    debug(( 5, "ident()=%d", old.ident() ));

    Xfer = old.Xfer;

    Edebug(());
}


// ********************************


DcmDataset::~DcmDataset()
{
    Bdebug((5, "dcdatset:DcmDataset::~DcmDataset()" ));
    debug(( 8, "Object pointer this=0x%p", this ));
    Edebug(());

}


// ********************************


Uint32 DcmDataset::calcElementLength(const E_TransferSyntax xfer,
				     const E_EncodingType enctype)
{
    return DcmItem::getLength(xfer, enctype);
}

// ********************************


BOOL DcmDataset::canWriteXfer(const E_TransferSyntax newXfer,
			      const E_TransferSyntax oldXfer)
{
    register E_TransferSyntax originalXfer = Xfer;

    if (newXfer == EXS_Unknown)
	return FALSE;

    if (Xfer == EXS_Unknown)
	originalXfer = oldXfer;

    return DcmItem::canWriteXfer(newXfer, originalXfer);

}

// ********************************


void DcmDataset::print(ostream & out, const BOOL showFullData,
		       const int level)
{
    int i;
    out << endl;
    for ( i=0; i<level; i++)
	out << "    ";
    out << "# Dicom-Data-Set" << endl;
    for ( i=0; i<level; i++)
	out << "    ";
    out << "# Used TransferSyntax: " << DcmXfer( Xfer ).getXferName();
    out << endl;
    if ( !elementList->empty() )
    {
	DcmObject *dO;
	elementList->seek( ELP_first );
	do 
	{
	    dO = elementList->get();
	    dO->print(out, showFullData, level + 1 );
	} while ( elementList->seek( ELP_next ) );
    }
}


// ********************************


void DcmDataset::resolveAmbigous(void)
{
    /*
    ** Nothing to do.
    ** The handling of VR for PixelData is now done according
    ** to Correction Proposal 14 in DcmItem::readSubElement(...)
    */
}


// ********************************


E_Condition DcmDataset::read(DcmStream & inStream,
			     const E_TransferSyntax xfer,
			     const E_GrpLenEncoding glenc,
			     const Uint32 maxReadLength)
{
    Bdebug((3, "DcmDataset::read(xfer=%d,glenc=%d)", xfer, glenc ));


    errorFlag = inStream.GetError();

    if (errorFlag == EC_Normal && inStream.EndOfStream())
	errorFlag = EC_EndOfStream;
    else if (errorFlag == EC_Normal && fTransferState != ERW_ready )
    {
	if (fTransferState == ERW_init)
	{
	    if (xfer == EXS_Unknown)
		Xfer = checkTransferSyntax(inStream); 
	    else
		Xfer = xfer;

	    //	This is a problem since DcmItem::read needs the ERW_init state
	    //				fTransferState = ERW_inWork; 
	}
	// uebergebe Kontrolle an DcmItem
	errorFlag = DcmItem::read(inStream, Xfer, glenc, maxReadLength);

    } 

    if ( errorFlag == EC_Normal || errorFlag == EC_EndOfStream )
    {
	errorFlag = EC_Normal;
	this -> resolveAmbigous();

 	computeGroupLengthAndPadding(glenc, EPD_noChange, Xfer);
	fTransferState = ERW_ready;              // Dataset ist komplett
    }
    debug(( 3, "errorFlag=(%d)", errorFlag ));
    Edebug(());

    return errorFlag;
}


// ********************************

E_Condition DcmDataset::write(DcmStream & outStream,
			      const E_TransferSyntax oxfer,
			      const E_EncodingType enctype)
{
    return write(outStream, oxfer, enctype, EGL_recalcGL);
}

// ********************************

E_Condition DcmDataset::write(DcmStream & outStream,
			      const E_TransferSyntax oxfer,
			      const E_EncodingType enctype,
			      const E_GrpLenEncoding glenc,
			      const E_PaddingEncoding padenc,
			      const Uint32 padlen,
			      const Uint32 subPadlen,
			      Uint32 instanceLength)
{
    Bdebug((3, "DcmDataset::writeBlock(oxfer=%d,enctype=%d,glenc=%d)",
	    oxfer, enctype, glenc ));

    if (fTransferState == ERW_notInitialized)
	errorFlag = EC_IllegalCall;
    else
    {
	E_TransferSyntax newXfer = oxfer;
	if (newXfer == EXS_Unknown)
	    newXfer = Xfer;

	errorFlag = outStream.GetError();
	if (errorFlag == EC_Normal && fTransferState != ERW_ready)
	{
	    if (fTransferState == ERW_init)
	    {
  		computeGroupLengthAndPadding(glenc, padenc, newXfer, enctype,  
					     padlen, subPadlen, instanceLength);
		elementList->seek( ELP_first );
		fTransferState = ERW_inWork;
	    }

	    if (fTransferState == ERW_inWork)
	    {
		if (!elementList->empty())
		{
		    DcmObject *dO;
		    do 
		    {
			dO = elementList->get();
			errorFlag = dO->write(outStream, newXfer, enctype);
		    } while (errorFlag == EC_Normal &&
			     elementList->seek(ELP_next));
		}

		if ( errorFlag == EC_Normal )
		    fTransferState = ERW_ready;
	    }
	}
    }
    Edebug(());

    return errorFlag;
}


// ********************************


/*
** CVS/RCS Log:
** $Log: dcdatset.cc,v $
** Revision 1.10  1997-06-06 09:55:28  andreas
** - corrected error: canWriteXfer returns false if the old transfer syntax
**   was unknown, which causes several applications to prohibit the writing
**   of dataset.
**
** Revision 1.9  1997/05/27 13:48:57  andreas
** - Add method canWriteXfer to class DcmObject and all derived classes.
**   This method checks whether it is possible to convert the original
**   transfer syntax to an new transfer syntax. The check is used in the
**   dcmconv utility to prohibit the change of a compressed transfer
**   syntax to a uncompressed.
**
** Revision 1.8  1997/05/16 08:23:52  andreas
** - Revised handling of GroupLength elements and support of
**   DataSetTrailingPadding elements. The enumeratio E_GrpLenEncoding
**   got additional enumeration values (for a description see dctypes.h).
**   addGroupLength and removeGroupLength methods are replaced by
**   computeGroupLengthAndPadding. To support Padding, the parameters of
**   element and sequence write functions changed.
** - Added a new method calcElementLength to calculate the length of an
**   element, item or sequence. For elements it returns the length of
**   tag, length field, vr field, and value length, for item and
**   sequences it returns the length of the whole item. sequence including
**   the Delimitation tag (if appropriate).  It can never return
**   UndefinedLength.
**
** Revision 1.7  1996/08/05 08:46:08  andreas
** new print routine with additional parameters:
**         - print into files
**         - fix output length for elements
** corrected error in search routine with parameter ESM_fromStackTop
**
** Revision 1.6  1996/04/25 17:08:04  hewett
** Removed out-of-date comment about RESOLVE_AMBIGOUS_VR_OF_PIXELDATA.
**
** Revision 1.5  1996/03/13 14:44:23  hewett
** The DcmDataset::resolveAmbiguous() method was setting the VR of
** PixelData to OW if a non-encapsulated transfer syntax was in use.
** This should only be done if the transfer syntax is implicit.  Any
** explicit transfer syntax will carry the VR with the data.
** Solution: Delete the code in DcmDataset::resolveAmbiguous().
** The VR of PixelData is being correctly set in
** cmItem::readSubElement(...) according to Correction Proposal 14.
**
** Revision 1.4  1996/01/09 11:06:43  andreas
** New Support for Visual C++
** Correct problems with inconsistent const declarations
** Correct error in reading Item Delimitation Elements
**
** Revision 1.3  1996/01/05 13:27:33  andreas
** - changed to support new streaming facilities
** - unique read/write methods for file and block transfer
** - more cleanups
**
**
*/

