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
** Last Update:		$Author: hewett $
** Update Date:		$Date: 1996-04-25 17:08:04 $
** Source File:		$Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/libsrc/dcdatset.cc,v $
** CVS/RCS Revision:	$Revision: 1.6 $
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


void DcmDataset::print(const int level)
{
    int i;
    cout << endl;
    for ( i=0; i<level; i++)
	cout << "    ";
    cout << "# Dicom-Data-Set" << endl;
    for ( i=0; i<level; i++)
	cout << "    ";
    cout << "# Used TransferSyntax: " << DcmXfer( Xfer ).getXferName();
    cout << endl;
    if ( !elementList->empty() )
    {
	DcmObject *dO;
	elementList->seek( ELP_first );
	do 
	{
	    dO = elementList->get();
	    dO->print( level + 1 );
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
			     const E_GrpLenEncoding gltype,
			     const Uint32 maxReadLength)
{
    Bdebug((3, "DcmDataset::read(xfer=%d,gltype=%d)", xfer, gltype ));


    errorFlag = inStream.GetError();

    if (errorFlag == EC_Normal && inStream.EndOfStream())
	errorFlag = EC_EndOfStream;
    else if (errorFlag == EC_Normal && fTransferState != ERW_ready )
    {
	if (fTransferState == ERW_init)
	{
	    if (xfer == EXS_Unknown)
		Xfer = checkTransferSyntax(inStream); // Notbehelf, falls xfer unbek.
	    else
		Xfer = xfer;

	    //	This is a problem since DcmItem::read needs the ERW_init state
	    //				fTransferState = ERW_inWork; 
	}
	// uebergebe Kontrolle an DcmItem
	errorFlag = DcmItem::read(inStream, Xfer, gltype, maxReadLength);

    } 

    if ( errorFlag == EC_Normal || errorFlag == EC_EndOfStream )
    {
	errorFlag = EC_Normal;
	this -> resolveAmbigous();

	if ( gltype == EGL_withoutGL )
	    removeGroupLengthElements(); // entferne alle Group Length El.
	fTransferState = ERW_ready;              // Dataset ist komplett
    }
    debug(( 3, "errorFlag=(%d)", errorFlag ));
    Edebug(());

    return errorFlag;
}


// ********************************


E_Condition DcmDataset::write(DcmStream & outStream,
			      const E_TransferSyntax oxfer,
			      const E_EncodingType enctype,
			      const E_GrpLenEncoding gltype)
{
    Bdebug((3, "DcmDataset::writeBlock(oxfer=%d,enctype=%d,gltype=%d)",
	    oxfer, enctype, gltype ));

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
		if ( gltype == EGL_withoutGL )
		{                      // entferne alle Group Length Elements
		    removeGroupLengthElements();
		}
		else if ( gltype == EGL_withGL )
		{                      // fuege alle Group Length Elements hinzu
		    addGroupLengthElements( newXfer, enctype );
		}
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
			errorFlag = dO->write(outStream, newXfer, 
					      enctype, gltype);
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
** Revision 1.6  1996-04-25 17:08:04  hewett
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

