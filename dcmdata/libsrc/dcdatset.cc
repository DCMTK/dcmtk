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
** Update Date:		$Date: 1996-01-05 13:27:33 $
** Source File:		$Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/libsrc/dcdatset.cc,v $
** CVS/RCS Revision:	$Revision: 1.3 $
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


// Die folgende Zeile ist zu aktivieren, falls das Bestimmen der VR von
// Pixel-Daten Ueber die Auswertung des Datenelementes BitsAllocated (8 oder
// 16 Bit) erfolgen soll.
//#define RESOLVE_AMBIGOUS_VR_OF_PIXELDATA



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


void DcmDataset::print( int level )
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
    E_Condition l_error = EC_Normal;
    DcmStack stack;
    int count = 0;

    while((l_error=this->search(DCM_PixelData, stack, ESM_afterStackTop, TRUE))
	  == EC_Normal )
    {
	count++;
	DcmStack localStack;
	localStack.push(stack.elem(1));  // Zeiger auf Ebene ueber pixelObj
	DcmObject *pixelObj = stack.top();

#ifdef RESOLVE_AMBIGOUS_VR_OF_PIXELDATA
	if (this->search(DCM_BitsAllocated,
			 localStack,
			 ESM_fromStackTop,
			 FALSE )
	    == EC_Normal)
	{
	    DcmObject *allocatedObj = localStack.top();
	    Uint16 bits = 8;		   // default 8-Bit
	    if (allocatedObj->ident() == EVR_US &&
		allocatedObj->getLength() > 0)
		bits = *(((DcmUnsignedShort*)allocatedObj)->get());
	    else
                cerr << "Warning: Valuefield in Element(0028,0100) is empty"
		     << endl;
	    if (bits == 16)
		pixelObj->setVR(EVR_OW);
	    else
		pixelObj->setVR(EVR_OB);
	}
	else
	{	    // kein allocatedTag vorhanden
            cerr << "Warning: no Tag(0028,0100) Bits Allocated in Dataset found"
		 << endl;
	}
#else
	DcmXfer xferSyn (Xfer);	          
	if (xferSyn.isNotEncapsulated())  
	    pixelObj->setVR( EVR_OW );
	else
	    pixelObj->setVR( EVR_OB );
#endif
    }
    debug(( 1, "Found %d Elements with Pixel Data", count ));
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
** Revision 1.3  1996-01-05 13:27:33  andreas
** - changed to support new streaming facilities
** - unique read/write methods for file and block transfer
** - more cleanups
**
**
*/

