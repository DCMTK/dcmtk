/*
**
** Author: Gerd Ehlers      26.04.94 -- Created
**         Andreas Barth    02.12.95 -- Modified for new stream classes
** Kuratorium OFFIS e.V.
**
** Module: dcfilefo.cc
**
** Purpose:
** Implementation of class DcmFileFormat
**
** Last Update:		$Author: andreas $
** Update Date:		$Date: 1996-04-12 13:17:22 $
** Source File:		$Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/libsrc/dcfilefo.cc,v $
** CVS/RCS Revision:	$Revision: 1.5 $
** Status:		$State: Exp $
**
** CVS/RCS Log at end of file
**
*/


#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include <stdlib.h>
#include <stdio.h>
//#include <memory.h>
#include <time.h>
#include <iostream.h>


#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#include "dcfilefo.h"
#include "dcitem.h"
#include "dcxfer.h"
#include "dcvrobow.h"
#include "dcvrui.h"
#include "dcvrul.h"
#include "dcvrus.h"
#include "dcvrae.h"
#include "dcvrsh.h"
#include "dcdebug.h"

#include "dcdeftag.h"
#include "dcuid.h"



// ********************************


DcmFileFormat::DcmFileFormat()
    : DcmSequenceOfItems(InternalUseTag)
{
    Bdebug((5, "dcfilefo:DcmFileFormat::DcmFileFormat()" ));
    debug(( 8, "Object pointer this=0x%p", this ));

    DcmMetaInfo *MetaInfo = new DcmMetaInfo();
    DcmSequenceOfItems::itemList->insert( MetaInfo );
    DcmDataset *Dataset = new DcmDataset();
    DcmSequenceOfItems::itemList->insert( Dataset );
    Edebug(());

}


// ********************************


DcmFileFormat::DcmFileFormat(DcmDataset * dataset)
    : DcmSequenceOfItems(InternalUseTag)
{
    Bdebug((5, "dcfilefo:DcmFileFormat::DcmFileFormat(DcmDataset*)" ));
    debug(( 8, "Object pointer this=0x%p", this ));

    DcmMetaInfo *MetaInfo = new DcmMetaInfo();
    DcmSequenceOfItems::itemList->insert( MetaInfo );
    DcmDataset *newDataset;

    if (dataset == (DcmDataset*)NULL) 
	newDataset = new DcmDataset();
    else
	newDataset = new DcmDataset( *dataset );
    DcmSequenceOfItems::itemList->insert( newDataset );
    Edebug(());

}


// ********************************


DcmFileFormat::DcmFileFormat(const DcmFileFormat &old)
    : DcmSequenceOfItems( old )
{
    Bdebug((5, "dcfilefo:DcmFileFormat::DcmFileFormat(DcmFileFormat&)" ));
    debug(( 8, "Object pointer this=0x%p", this ));

    debug(( 5, "ident()=%d", old.ident() ));

    Edebug(());
}


// ********************************


DcmFileFormat::~DcmFileFormat()
{
    Bdebug((5, "dcfilefo:DcmFileFormat::~DcmFileFormat()" ));
    debug(( 8, "Object pointer this=0x%p", this ));
    Edebug(());

}


// ********************************


void DcmFileFormat::print(int level)
{
    int i;
    cout << endl;
    for ( i=0; i<level; i++)
	cout << "    ";
    cout << "# Dicom-File-Format" << endl;
    if ( !itemList->empty() )
    {
	DcmObject *dO;
	itemList->seek( ELP_first );
	do 
	{
	    dO = itemList->get();
	    dO->print( level );
	} while ( itemList->seek( ELP_next ) );
    }
    else
    {
	for ( i=0; i<level; i++)
	    cout << "    ";
	cout << "# Dicom-File-Format has been erased";
	cout << endl;
    }
}


// ********************************


E_Condition DcmFileFormat::checkValue(DcmMetaInfo * metainfo,
				      DcmDataset * dataset,
				      const DcmTagKey & atagkey,
				      DcmObject* obj,
				      const E_TransferSyntax oxfer )
{
    Bdebug((2, "dcfilefo:DcmFileFormat::checkValue(*metainfo,*dataset,xtag=(%x,%x),*obj,oxfer=%d)",
	    atagkey.getGroup(), atagkey.getElement(), oxfer ));

    E_Condition l_error = EC_Normal;
    if ( metainfo != (DcmMetaInfo*)NULL && dataset != (DcmDataset*)NULL )
    {
	DcmStack stack;
        DcmTag tag( atagkey );
	if ( obj != (DcmObject*)NULL )
            tag = obj->getTag();

        DcmTagKey xtag = tag.getXTag();
	DcmElement *elem = (DcmElement*)obj;

	if ( xtag == DCM_MetaElementGroupLength ) {	// (0002,0000)
            if ( elem == (DcmElement*)NULL )
            {
                elem = new DcmUnsignedLong( tag );
                metainfo->insert( elem, TRUE );
            }
            Uint32 temp = 0;
            if ( elem->getLength() == 0 && elem->ident() == EVR_UL )
                ((DcmUnsignedLong*)elem)->put( &temp, 1 );
            // Laengenberechnung erfolgt in validateMetaInfo()
	} else if ( xtag == DCM_FileMetaInformationVersion ) {	// (0002,0001)
            if ( elem == (DcmElement*)NULL )
            {
                elem = new DcmOtherByteOtherWord( tag );
                metainfo->insert( elem, TRUE );
            }
            Uint8 version[2] = {0,1};
            if ( elem->getLength() == 0 && elem->ident() == EVR_OB )
                ((DcmOtherByteOtherWord*)elem)->put( version, 2 );

            // ueberpruefe Version des MetaHeaders
            Uint8 *currVers = ((DcmOtherByteOtherWord*)elem)->getBytes();
            if (((currVers[0] & version[0] & 0xff) == version[0]) &&
		((currVers[1] & version[1] & 0xff) == version[1]) ) {
		debug(( 2, "Version of MetaHeader is ok: 0x%2.2x%2.2x",
			currVers[1], currVers[0] ));

            } else {
                currVers[0] = currVers[0] | version[0]; // direkte Daten-
                currVers[1] = currVers[1] | version[1]; // Manipulation
                fprintf(stderr, "Warning: dcfilefo:"
                        " unknown Version of MetaHeader detected:");
                fprintf(stderr,
			" 0x%2.2x%2.2x supported: 0x%2.2x%2.2x",
			currVers[1], currVers[0],
			version[1],  version[0] );
                fprintf(stderr, "\n");
            }
	} else if ( xtag == DCM_MediaStorageSOPClassUID ) {	// (0002,0002)
	    if ( elem == (DcmElement*)NULL ) {
                elem = new DcmUniqueIdentifier( tag );
                metainfo->insert( elem, TRUE );
            }
            if ( elem->getLength() == 0 && elem->ident() == EVR_UI ) {
		if ( dataset->search( DCM_SOPClassUID, stack ) == EC_Normal ) {
		    const char *uid = ((DcmUniqueIdentifier*)stack.top())->get();
		    ((DcmUniqueIdentifier*)elem)->put( uid );
		    debug(( 2, "use SOPClassUID [%s]", uid ));

		} else {
		    ((DcmUniqueIdentifier*)elem)->put( 
			UID_PrivateGenericFileSOPClass );
		    debug(( 2, "No SOP Class UID in Dataset, using PrivateGenericFileSOPClass" ));
		}
	    }
	} else if ( xtag == DCM_MediaStorageSOPInstanceUID ) {	// (0002,0003)
	    if ( elem == (DcmElement*)NULL )
	    {
		elem = new DcmUniqueIdentifier( tag );
		metainfo->insert( elem, TRUE );
	    }
	    if ( elem->getLength() == 0 && elem->ident() == EVR_UI )
	    {
		if ( dataset->search( DCM_SOPInstanceUID, stack )
		     == EC_Normal )
		{
		    const char* uid = ((DcmUniqueIdentifier*)stack.top())->get();
		    ((DcmUniqueIdentifier*)elem)->put( uid );
		    debug(( 2, "use SOPInstanceUID [%s] from Dataset", uid ));

		}
		else
		{
		    char uid[128];

		    dcmGenerateUniqueIdentifer(uid);	// from dcuid.h 

		    ((DcmUniqueIdentifier*)elem)->put( uid );
		    debug(( 2, "use new generated SOPInstanceUID [%s]", uid ));

		}
	    }
	} else if ( xtag == DCM_TransferSyntaxUID ) {	// (0002,0010)
	    if ( elem == (DcmElement*)NULL )
	    {
		elem = new DcmUniqueIdentifier( tag );
		metainfo->insert( elem, TRUE );
	    }
	    if ( elem->ident() == EVR_UI )
	    {
		Vdebug((2, ((DcmUniqueIdentifier*)elem)->get() != (char*)NULL,
			"found old transfer-syntax: [%s]",
			((DcmUniqueIdentifier*)elem)->get() ));

		DcmXfer dcXfer( oxfer );
		const char *uid = dcXfer.getXferID();
		elem->put( uid );
		debug(( 2, "use new transfer-syntax [%s] on writing following Dataset",
			dcXfer.getXferName() ));

	    }
	} else if ( xtag == DCM_ImplementationClassUID ) {	// (0002,0012)
	    if ( elem == (DcmElement*)NULL )
	    {
		elem = new DcmUniqueIdentifier( tag );
		metainfo->insert( elem, TRUE );
	    }
	    if ( elem->ident() == EVR_UI )
	    {
		const char *uid = OFFIS_IMPLEMENTATION_CLASS_UID;
		((DcmUniqueIdentifier*)elem)->put( uid );
	    }

	} else if ( xtag == DCM_ImplementationVersionName ) {	// (0002,0013)
	    if ( elem == (DcmElement*)NULL )
	    {
		elem = new DcmShortString( tag );
		metainfo->insert( elem, TRUE );
	    }
	    if ( elem->ident() == EVR_SH )
	    {
		char *uid = OFFIS_DTK_IMPLEMENTATION_VERSION_NAME;
		((DcmShortString*)elem)->put( uid );
	    }

	} else if ( xtag == DCM_SourceApplicationEntityTitle ) {	// (0002,0016)
	    if ( elem == (DcmElement*)NULL )
	    {
		elem = new DcmApplicationEntity( tag );
		metainfo->insert( elem, TRUE );
	    }
	    cerr << "Error: dcfilefo: I don't know how to handle"
		" DCM_SourceApplicationEntityTitle!" << endl;

	} else if ( xtag == DCM_PrivateInformationCreatorUID ) {	// (0002,0100)
	    if ( elem == (DcmElement*)NULL )
	    {
		elem = new DcmUniqueIdentifier( tag );
		metainfo->insert( elem, TRUE );
	    }
	    cerr << "Error: dcfilefo: I don't know how to handle"
		" DCM_PrivateInformationCreatorUID!" << endl;

	} else if ( xtag == DCM_PrivateInformation ) {	// (0002,0102)
	    if ( elem == (DcmElement*)NULL )
	    {
		elem = new DcmOtherByteOtherWord( tag );
		metainfo->insert( elem, TRUE );
	    }
	    cerr << "Warning: dcfilefo: I don't know how to handle"
		" DCM_PrivateInformation!" << endl;

	} else {
	    cerr << "Warning: dcfilefo: I don't know how to handle "
		 << tag.getTagName() << endl;

	}
        if ( elem == (DcmElement*)NULL )
            l_error = EC_InvalidVR;
    } else {
        l_error = EC_IllegalCall;
    }
    Edebug(());

    return l_error;
}


// ********************************



E_Condition DcmFileFormat::validateMetaInfo(E_TransferSyntax oxfer)
{
    Bdebug((2, "dcfilefo:DcmFileFormat::validateMetaInfo(oxfer=%d)", oxfer ));

    E_Condition l_error = EC_Normal;
    DcmMetaInfo *metinf = getMetaInfo();
    DcmDataset *datset = getDataset();

    if (metinf != (DcmMetaInfo*)NULL && datset != (DcmDataset*)NULL) {
	DcmStack stack;

	/* DCM_MetaElementGroupLength */
	metinf->search(DCM_MetaElementGroupLength, stack, ESM_fromHere, FALSE );
	checkValue( metinf, datset, DCM_MetaElementGroupLength, stack.top(), oxfer );

	/* DCM_FileMetaInformationVersion */
	metinf->search(DCM_FileMetaInformationVersion, stack, ESM_fromHere, FALSE );
	checkValue( metinf, datset, DCM_FileMetaInformationVersion, stack.top(), oxfer );

	/* DCM_MediaStorageSOPClassUID */
	metinf->search(DCM_MediaStorageSOPClassUID, stack, ESM_fromHere, FALSE );
	checkValue( metinf, datset, DCM_MediaStorageSOPClassUID, stack.top(), oxfer );

	/* DCM_MediaStorageSOPInstanceUID */
	metinf->search(DCM_MediaStorageSOPInstanceUID, stack, ESM_fromHere, FALSE );
	checkValue( metinf, datset, DCM_MediaStorageSOPInstanceUID, stack.top(), oxfer );

	/* DCM_TransferSyntaxUID */
	metinf->search(DCM_TransferSyntaxUID, stack, ESM_fromHere, FALSE );
	checkValue( metinf, datset, DCM_TransferSyntaxUID, stack.top(), oxfer );

	/* DCM_ImplementationClassUID */
	metinf->search(DCM_ImplementationClassUID, stack, ESM_fromHere, FALSE );
	checkValue( metinf, datset, DCM_ImplementationClassUID, stack.top(), oxfer );

	/* DCM_ImplementationVersionName */
	metinf->search(DCM_ImplementationVersionName, stack, ESM_fromHere, FALSE );
	checkValue( metinf, datset, DCM_ImplementationVersionName, stack.top(), oxfer );

	debug(( 2, "DcmFileFormat:found %ld Elements in DcmMetaInfo metinf.",
		metinf->card() ));

	// berechne neue GroupLength
        if ( metinf->addGroupLengthElements( META_HEADER_DEFAULT_TRANSFERSYNTAX,
					     EET_UndefinedLength ) != EC_Normal ) {
            cerr << "Error: DcmFileFormat::validateMetaInfo(): group length"
		" of Meta Information Header not adapted."
		 << endl;
        }
    }
    else {
        l_error = EC_CorruptedData;
    }
    Edebug(());

    return l_error;
}



// ********************************


E_TransferSyntax DcmFileFormat::lookForXfer(DcmMetaInfo* metainfo)
{
    Bdebug((4, "dcfilefo:DcmFileFormat::lookForXfer(metainfo*)" ));

    E_TransferSyntax newxfer = EXS_Unknown;
    DcmStack stack;
    if (metainfo && metainfo->search(DCM_TransferSyntaxUID, stack) == EC_Normal
	)
    {
	DcmUniqueIdentifier *xferUI = (DcmUniqueIdentifier*)(stack.top());
        if ( xferUI->getTag().getXTag() == DCM_TransferSyntaxUID )
	{
            const char * xferid = xferUI->get();     // auslesen der ID
            DcmXfer localXfer(xferid);      // dekodieren in E_TransferSyntax
            newxfer = localXfer.getXfer();
	    debug(( 4, "detected xfer=%d=[%s] in MetaInfo",
		    newxfer,
		    localXfer.getXferName() ));
	}
    }
    Edebug(());

    return newxfer;
}


// ********************************


E_Condition DcmFileFormat::read(DcmStream & inStream,
				const E_TransferSyntax xfer,
				const E_GrpLenEncoding gltype,
				const Uint32 maxReadLength)
{
    Bdebug((3, "DcmFileFormat::read(xfer=%d,gltype=%d)",
	    xfer, gltype ));

    if (fTransferState == ERW_notInitialized)
	errorFlag = EC_IllegalCall;
    else
    {
	errorFlag = inStream.GetError();
	E_TransferSyntax newxfer = xfer;
	DcmDataset * dataset = NULL;

	if (errorFlag == EC_Normal && inStream.EndOfStream())
	    errorFlag = EC_EndOfStream;
	else if (errorFlag == EC_Normal && fTransferState != ERW_ready)
	{
	    // Die neuen Daten werden ans Ende gehaengt
	    itemList->seek( ELP_last );

	    DcmMetaInfo * metaInfo = getMetaInfo();
	    if (metaInfo == NULL && fTransferState == ERW_init)
	    {
		metaInfo = new DcmMetaInfo();
		itemList->insert(metaInfo, ELP_first );
	    }
			
	    if (metaInfo && metaInfo->transferState() != ERW_ready)
	    {
		errorFlag = metaInfo->read(inStream, xfer, 
					   gltype, maxReadLength );
		debug(( 3, "MetaInfo has been read." ));
	    }

	    // lese aus MetaInfo() Tag(0002,0010) aus und bestimme xfer
	    newxfer = lookForXfer(metaInfo );


	    if (errorFlag == EC_Normal && 
		(!metaInfo || metaInfo->transferState() == ERW_ready))
	    {
		dataset = getDataset();
		if (dataset == NULL && fTransferState == ERW_init)
		{
		    dataset = new DcmDataset();
		    itemList->seek (ELP_first);
		    itemList->insert(dataset, ELP_next);
		}

		if (dataset && dataset->transferState() != ERW_ready)
		{
		    errorFlag = dataset->read(inStream, newxfer, 
					      gltype, maxReadLength);
		    debug(( 3, "Dataset has been read." ));
		}
	    }
	}
	Edebug(());
	if (fTransferState == ERW_init)
	    fTransferState = ERW_inWork;
		
	if (dataset && dataset->transferState() == ERW_ready)
	    fTransferState = ERW_ready;
    }		

    return errorFlag;
}  // DcmFileFormat::read()


// ********************************


E_Condition DcmFileFormat::write(DcmStream & outStream,
				 const E_TransferSyntax oxfer,
				 const E_EncodingType enctype,
				 const E_GrpLenEncoding gltype )
{
    Bdebug((3, "DcmFileFormat::write(oxfer=%d,enctype=%d,"
	    "gltype=%d)", oxfer, enctype, gltype ));
    if (fTransferState == ERW_notInitialized)
	errorFlag = EC_IllegalCall;
    else
    {
	E_TransferSyntax outxfer = oxfer;
	if (outxfer == EXS_Unknown)
	{
	    DcmDataset * dataset = this -> getDataset();
	    if (dataset)
		outxfer = dataset->getOriginalXfer();
	}

	errorFlag = outStream.GetError();
	if (outxfer == EXS_Unknown || outxfer == EXS_BigEndianImplicit)
	    errorFlag = EC_IllegalCall;
	else if (itemList->empty())
	    errorFlag = EC_CorruptedData;
	else if (errorFlag == EC_Normal && fTransferState != ERW_ready)
	{
	    if (fTransferState == ERW_init)
	    {
		validateMetaInfo(outxfer);
		itemList->seek( ELP_first );
		fTransferState = ERW_inWork;
	    }

	    if (fTransferState == ERW_inWork)
	    {
		DcmObject * dO;
		do 
		{
		    dO = itemList->get();
		    if (dO -> transferState() != ERW_ready)
			errorFlag = dO->write(outStream, outxfer, 
					      enctype, gltype);
		} while (errorFlag == EC_Normal && itemList->seek(ELP_next));

		if (errorFlag == EC_Normal)
		    fTransferState = ERW_ready;
	    }
	}

	if ( outxfer == EXS_BigEndianImplicit )
	{
	    cerr << "Error: DcmFileFormat::write() illegal TransferSyntax(BI)"
		" used" << endl;
	}
    }
    Edebug(());

    return errorFlag;
}



// ********************************


E_Condition DcmFileFormat::insertItem(DcmItem* /*item*/,
				      const unsigned long /*where*/ )
{
    cerr << "Warning: illegal call of DcmFileFormat::insert(DcmItem*,Uin32)"
	 << endl;
    errorFlag = EC_IllegalCall;

    return errorFlag;
}


// ********************************


DcmItem* DcmFileFormat::remove(const unsigned long /*num*/ )
{
    cerr << "Warning: illegal call of DcmFileFormat::remove(Uint32)" << endl;
    errorFlag = EC_IllegalCall;

    return (DcmItem*)NULL;
}


// ********************************


DcmItem* DcmFileFormat::remove( DcmItem* /*item*/ )
{
    cerr << "Warning: illegal call of DcmFileFormat::remove(DcmItem*)" << endl;
    errorFlag = EC_IllegalCall;

    return (DcmItem*)NULL;
}


// ********************************


E_Condition DcmFileFormat::clear()
{
    Bdebug((2, "dcfilefo:DcmFileFormat::clear()"));

    cerr << "Warning: illegal call of DcmFileFormat::clear()" << endl;
    errorFlag = EC_IllegalCall;
    Edebug(());

    return errorFlag;
}


// ********************************


DcmMetaInfo* DcmFileFormat::getMetaInfo()
{
    errorFlag = EC_Normal;
    DcmMetaInfo *meta = (DcmMetaInfo*)NULL;
    if (    itemList->seek_to( 0 ) != (DcmItem*)NULL
	    && itemList->get()->ident() == EVR_metainfo )
	meta = (DcmMetaInfo*)( itemList->get() );
    else
	errorFlag = EC_IllegalCall;
    return meta;
}


// ********************************


DcmDataset* DcmFileFormat::getDataset()
{
    errorFlag = EC_Normal;
    DcmDataset *data = (DcmDataset*)NULL;
    if (    itemList->seek_to( 1 ) != (DcmItem*)NULL
	    && itemList->get()->ident() == EVR_dataset )
	data = (DcmDataset*)( itemList->get() );
    else
	errorFlag = EC_IllegalCall;
    return data;
}


// ********************************


/*
** CVS/RCS Log:
** $Log: dcfilefo.cc,v $
** Revision 1.5  1996-04-12 13:17:22  andreas
** Minor changes to support DEC ALPHA and DEC MIPS
**
** Revision 1.4  1996/03/11 14:48:04  hewett
** When creating a DICOM file, if a SOP Class UID is present in the
** data set then it will be used.  Otherwise, a private UID will now be used
** in the meta header attribute MediaStorageSOPClass UID.  Previously this
** attribute was left empty (an error for type 1 attributes).
**
** Revision 1.3  1996/01/05 13:27:37  andreas
** - changed to support new streaming facilities
** - unique read/write methods for file and block transfer
** - more cleanups
**
**
*/
