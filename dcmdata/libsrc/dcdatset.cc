/*
 *
 * Author: Gerd Ehlers      Created:  04-26-94
 *                          Modified: 02-07-95
 *
 * Module: dcdatset.cc
 *
 * Purpose:
 * Implementation of the class DcmDataset
 *
 *
 * Last Update:   $Author: hewett $
 * Revision:      $Revision: 1.2 $
 * Status:	  $State: Exp $
 *
 */


#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include <stdio.h>
#include <stdlib.h>
#include <iostream.h>
#include <string.h>

#include "dcdatset.h"
#include "dcitem.h"
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
    : DcmItem( ItemTag )
{
Bdebug((5, "dcdatset:DcmDataset::DcmDataset()" ));
debug(( 8, "Object pointer this=0x%p", this ));

Edebug(());

}


// ********************************


DcmDataset::DcmDataset( iDicomStream *iDStream )
    : DcmItem( ItemTag, UNDEF_LEN, iDStream )
{
Bdebug((5, "dcdatset:DcmDataset::DcmDataset(*iDS)" ));
debug(( 8, "Object pointer this=0x%p", this ));

Edebug(());

}


// ********************************


DcmDataset::DcmDataset( const DcmDataset &old )
    : DcmItem( old )
{
Bdebug((5, "dcdatset:DcmDataset::DcmDataset(DcmDataset&)" ));
debug(( 8, "Object pointer this=0x%p", this ));
debug(( 5, "ident()=%d", old.ident() ));

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


DcmEVR DcmDataset::ident() const
{
    return EVR_dataset;
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
	do {
	    dO = elementList->get();
	    dO->print( level + 1 );
	} while ( elementList->seek( ELP_next ) );
    }
}


// ********************************


T_VR_UL DcmDataset::resolveAmbigous()
{
    E_Condition l_error = EC_Normal;
    DcmStack stack;
    int count = 0;

    while ( (l_error=this->search( DCM_PixelData, stack, ESM_afterStackTop, TRUE ))
	    == EC_Normal )
    {
	count++;
	DcmStack localStack;
	localStack.push( stack.elem(1) );  // Zeiger auf Ebene ueber pixelObj
	DcmObject *pixelObj = stack.top();

#ifdef RESOLVE_AMBIGOUS_VR_OF_PIXELDATA
	if ( this->search( DCM_BitsAllocated,
			   localStack,
			   ESM_fromStackTop,
			   FALSE )
	     == EC_Normal )
	{
	    DcmObject *allocatedObj = localStack.top();
	    T_VR_US bits = 8;		   // default 8-Bit
	    if (    allocatedObj->ident() == EVR_US
		 && allocatedObj->getLength() > 0 )
		bits = *( ((DcmUnsignedShort*)allocatedObj)->get() );
	    else
                cerr << "Warning: Valuefield in Element(0028,0100) is empty"
                     << endl;
	    if ( bits == 16 )
		pixelObj->setVR( EVR_OW );
	    else
		pixelObj->setVR( EVR_OB );
	}
	else
	{	    // kein allocatedTag vorhanden
            cerr << "Warning: no Tag(0028,0100) Bits Allocated in Dataset found"
                 << endl;
	}
#else
	DcmXfer xferSyn ( Xfer );	     // diese Zeilen koennen auch nach
	if ( xferSyn.isNotEncapsulated() )   // dcvrobow.cc verlagert werden
	    pixelObj->setVR( EVR_OW );
	else
	    pixelObj->setVR( EVR_OB );
#endif
    }
    return count;
}


// ********************************


E_Condition DcmDataset::read( E_TransferSyntax xfer,
                              E_GrpLenEncoding gltype )
{
Bdebug((3, "dcdatset:DcmDataset::read(xfer=%d,gltype=%d)", xfer, gltype ));

    errorFlag = EC_Normal;
    if ( iDS == (iDicomStream*)NULL )
        errorFlag = EC_InvalidStream;
    else if ( iDS->eof() )
	errorFlag = EC_EndOfFile;
    else if ( !iDS->good() )
	errorFlag = EC_InvalidStream;
    else
    {
	E_TransferSyntax tmpxfer = checkTransferSyntax();
	DcmXfer tmpxferSyn( tmpxfer );
	DcmXfer xferSyn( xfer );

	if (	(    tmpxferSyn.isExplicitVR()
		  &&	xferSyn.isImplicitVR()
		)
	     || (    tmpxferSyn.isImplicitVR()
		  &&	xferSyn.isExplicitVR()
		)
	     || xferSyn.getXfer() == EXS_UNKNOWN
	   )
	{
            Xfer = tmpxferSyn.getXfer();  // benutze eigene, ermittelte xfer
	    if ( xferSyn.getXfer() != EXS_UNKNOWN )
                cerr << "Info: DcmDataset::read(): TransferSyntax of Dataset"
                        " is different from passed parameter xfer:"
			" ignoring xfer!"
		     << endl;
	}
	else
	    Xfer = xferSyn.getXfer();
                                          // uebergebe Kontrolle an DcmItem
        errorFlag = DcmItem::read( Xfer, gltype );

	T_VR_UL count = DcmDataset::resolveAmbigous();
debug(( 1, "Found %d Elements with Pixel Data", count ));

        count++;                          // turn Compiler Warning off
        if ( gltype == EGL_withoutGL )
            removeGroupLengthElements();  // entferne alle Group Length Elements
    } // else errorFlag
    valueModified = FALSE;
    if ( errorFlag == EC_EndOfFile )
	errorFlag = EC_Normal;
Edebug(());

    return errorFlag;
}


// ********************************


E_Condition DcmDataset::readBlock( E_TransferSyntax xfer,
                                   E_GrpLenEncoding gltype )
{
Bdebug((3, "dcdatset:DcmDataset::readBlock(xfer=%d,gltype=%d)", xfer, gltype ));

    errorFlag = EC_Normal;
    if ( iDS == (iDicomStream*)NULL )
        errorFlag = EC_InvalidStream;
    else if ( iDS->eof() || iDS->fromFile() )
	errorFlag = EC_EndOfFile;
    else if ( !iDS->good() )
	errorFlag = EC_InvalidStream;
    else if ( readState() != ERW_ready )
    {
	if ( readState() == ERW_init )
	{
	    if ( xfer == EXS_UNKNOWN )
		Xfer = checkTransferSyntax(); // Notbehelf, falls xfer unbekannt
	    else
		Xfer = xfer;
	    rdStat = ERW_inWork;
	}
                                              // uebergebe Kontrolle an DcmItem
        errorFlag = DcmItem::readBlock( Xfer, gltype );

    } // else errorFlag
    valueModified = FALSE;
    if ( errorFlag == EC_Normal || errorFlag == EC_EndOfFile )
    {
	errorFlag = EC_Normal;
	T_VR_UL count = DcmDataset::resolveAmbigous();

debug(( 1, "Found %d Elements with Pixel Data", count ));

        count++;                         // turn Compiler Warning off
        if ( gltype == EGL_withoutGL )
            removeGroupLengthElements(); // entferne alle Group Length Elements
        rdStat = ERW_ready;              // Dataset ist komplett
    }
debug(( 3, "errorFlag=(%d)", errorFlag ));
Edebug(());

    return errorFlag;
}


// ********************************


E_Condition DcmDataset::write( oDicomStream &oDS,
                               E_TransferSyntax oxfer,
                               E_EncodingType enctype,
                               E_GrpLenEncoding gltype )
{
Bdebug((3, "dcdatset:DcmDataset::write(&oDS,oxfer=%d,enctype=%d,gltype=%d)",
           oxfer, enctype, gltype ));

    E_TransferSyntax outxfer = oxfer;
    errorFlag = EC_Normal;
    if ( oxfer == EXS_UNKNOWN || oxfer == EXS_BigEndianImplicit )
	errorFlag = EC_IllegalCall;
    else if ( !oDS.good() )
	errorFlag = EC_InvalidStream;
    else
    {
        if ( gltype == EGL_withoutGL )
            removeGroupLengthElements(); // entferne alle Group Length Elements
        else if ( gltype == EGL_withGL )
        {                              // fuege alle Group Length Elements hinzu
            addGroupLengthElements( oxfer, enctype );
        }

	if ( !elementList->empty() )
	{
	    DcmObject *dO;
	    elementList->seek( ELP_first );
	    do {
		dO = elementList->get();
/*
		if ( dO->getGTag() == 0x0002 )		// jetzt Aufgabe von
                    outxfer = EXS_LittleEndianExplicit; // DcmMetaset()
		else
		    outxfer = oxfer;
*/
                errorFlag = dO->write( oDS, outxfer, enctype, gltype );
	    } while ( elementList->seek( ELP_next ) && errorFlag == EC_Normal );
	}

	valueModified = FALSE;
    }
Edebug(());

    return errorFlag;
}


// ********************************


E_Condition DcmDataset::writeBlock( oDicomStream &oDS,
				    E_TransferSyntax oxfer,
                                    E_EncodingType enctype,
                                    E_GrpLenEncoding gltype )
{
Bdebug((3, "dcdatset:DcmDataset::writeBlock(&oDS,oxfer=%d,enctype=%d,gltype=%d)",
           oxfer, enctype, gltype ));

    errorFlag = EC_Normal;
    E_TransferSyntax outxfer = oxfer;
    if ( !oDS.good() || oDS.intoFile() )
	errorFlag = EC_InvalidStream;
    else
    {
	if ( writeState() != ERW_ready )
	{
	    if ( writeState() == ERW_init )
	    {
		wrStat = ERW_inWork;
                if ( gltype == EGL_withoutGL )
                {                      // entferne alle Group Length Elements
                    removeGroupLengthElements();
                }
                else if ( gltype == EGL_withGL )
                {                      // fuege alle Group Length Elements hinzu
                    addGroupLengthElements( oxfer, enctype );
                }
		elementList->seek( ELP_first );
	    }
	    if ( !elementList->empty() && writeState() != ERW_init )
	    {
		DcmObject *dO;
		do {
		    dO = elementList->get();
/*
		    if ( dO->getGTag() == 0x0002 )	    // jetzt Aufgabe von
                        outxfer = EXS_LittleEndianExplicit; // DcmMetaset()
		    else
			outxfer = oxfer;
*/
                    errorFlag = dO->writeBlock( oDS, outxfer, enctype, gltype );
		} while (    errorFlag == EC_Normal
			  && elementList->seek( ELP_next ) );
	    }

	    if ( errorFlag == EC_Normal )
		wrStat = ERW_ready;
	    valueModified = FALSE;
	}
    }
Edebug(());

    return errorFlag;
}


// ********************************


