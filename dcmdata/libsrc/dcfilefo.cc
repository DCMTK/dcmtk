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
** Update Date:		$Date: 1997-07-03 15:09:57 $
** Source File:		$Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/libsrc/dcfilefo.cc,v $
** CVS/RCS Revision:	$Revision: 1.11 $
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
    DcmMetaInfo *MetaInfo = new DcmMetaInfo();
    DcmSequenceOfItems::itemList->insert( MetaInfo );
    DcmDataset *Dataset = new DcmDataset();
    DcmSequenceOfItems::itemList->insert( Dataset );
}


// ********************************


DcmFileFormat::DcmFileFormat(DcmDataset * dataset)
    : DcmSequenceOfItems(InternalUseTag)
{
    DcmMetaInfo *MetaInfo = new DcmMetaInfo();
    DcmSequenceOfItems::itemList->insert( MetaInfo );
    DcmDataset *newDataset;

    if (dataset == (DcmDataset*)NULL) 
	newDataset = new DcmDataset();
    else
	newDataset = new DcmDataset( *dataset );
    DcmSequenceOfItems::itemList->insert( newDataset );
}


// ********************************


DcmFileFormat::DcmFileFormat(const DcmFileFormat &old)
    : DcmSequenceOfItems( old )
{
}


// ********************************


DcmFileFormat::~DcmFileFormat()
{
}


// ********************************


void DcmFileFormat::print(ostream & out, const BOOL showFullData, 
			  const int level)
{
    int i;
    out << endl;
    for ( i=0; i<level; i++)
	out << "    ";
    out << "# Dicom-File-Format" << endl;
    if ( !itemList->empty() )
    {
	DcmObject *dO;
	itemList->seek( ELP_first );
	do 
	{
	    dO = itemList->get();
	    dO->print(out, showFullData, level);
	} while ( itemList->seek( ELP_next ) );
    }
    else
    {
	for ( i=0; i<level; i++)
	    out << "    ";
	out << "# Dicom-File-Format has been erased";
	out << endl;
    }
}


// ********************************


E_Condition DcmFileFormat::checkValue(DcmMetaInfo * metainfo,
				      DcmDataset * dataset,
				      const DcmTagKey & atagkey,
				      DcmObject* obj,
				      const E_TransferSyntax oxfer )
{
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
                ((DcmUnsignedLong*)elem)->putUint32Array( &temp, 1 );
            // Laengenberechnung erfolgt in validateMetaInfo()
	} else if ( xtag == DCM_FileMetaInformationVersion ) {	// (0002,0001)
            if ( elem == (DcmElement*)NULL )
            {
                elem = new DcmOtherByteOtherWord( tag );
                metainfo->insert( elem, TRUE );
            }
            Uint8 version[2] = {0,1};
            if ( elem->getLength() == 0 && elem->ident() == EVR_OB )
                ((DcmOtherByteOtherWord*)elem)->putUint8Array( version, 2 );

            // ueberpruefe Version des MetaHeaders
            Uint8 *currVers;
	    l_error = ((DcmOtherByteOtherWord*)elem)->getUint8Array(currVers);
            if (((currVers[0] & version[0] & 0xff) == version[0]) &&
		((currVers[1] & version[1] & 0xff) == version[1]) ) {
		debug(2, ( "DcmFileFormat::checkValue() Version of MetaHeader is ok: 0x%2.2x%2.2x",
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
		    char *uid = NULL;
		    l_error = ((DcmUniqueIdentifier*)stack.top())->getString(uid);
		    ((DcmUniqueIdentifier*)elem)->putString( uid );
		    debug(2, ( "DcmFileFormat::checkValue() use SOPClassUID [%s]", uid ));

		} else {
		    ((DcmUniqueIdentifier*)elem)->putString( 
			UID_PrivateGenericFileSOPClass );
		    debug(2, ( "DcmFileFormat::checkValue() No SOP Class UID in Dataset, using PrivateGenericFileSOPClass" ));
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
		    char* uid = NULL;
		    l_error =((DcmUniqueIdentifier*)stack.top())->getString(uid);
		    ((DcmUniqueIdentifier*)elem)->putString( uid );
		    debug(2, ( "DcmFileFormat::checkValue() use SOPInstanceUID [%s] from Dataset", uid ));

		}
		else
		{
		    char uid[128];

		    dcmGenerateUniqueIdentifer(uid);	// from dcuid.h 

		    ((DcmUniqueIdentifier*)elem)->putString( uid );
		    debug(2, ( "DcmFileFormat::checkValue() use new generated SOPInstanceUID [%s]", uid ));

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
#ifdef DEBUG
char * uidtmp = NULL;
((DcmUniqueIdentifier*)elem)->getString(uidtmp);
Cdebug(2,  uidtmp != (char*)NULL,
       ( "DcmFileFormat::checkValue() found old transfer-syntax: [%s]",uidtmp ));
#endif
		DcmXfer dcXfer( oxfer );
		const char *uid = dcXfer.getXferID();
		elem->putString( uid );
		debug(2,( "DcmFileFormat::checkValue() use new transfer-syntax [%s] on writing following Dataset",
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
		((DcmUniqueIdentifier*)elem)->putString( uid );
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
		((DcmShortString*)elem)->putString( uid );
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
    return l_error;
}


// ********************************



E_Condition DcmFileFormat::validateMetaInfo(E_TransferSyntax oxfer)
{
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

	debug(2, ( "DcmFileFormat: found %ld Elements in DcmMetaInfo metinf.",
		metinf->card() ));

	// berechne neue GroupLength
        if (metinf->computeGroupLengthAndPadding
	     (EGL_withGL, EPD_noChange, META_HEADER_DEFAULT_TRANSFERSYNTAX, 
	      EET_UndefinedLength) != EC_Normal ) {
            cerr << "Error: DcmFileFormat::validateMetaInfo(): group length"
		" of Meta Information Header not adapted."
		 << endl;
        }
    }
    else {
        l_error = EC_CorruptedData;
    }
    return l_error;
}



// ********************************


E_TransferSyntax DcmFileFormat::lookForXfer(DcmMetaInfo* metainfo)
{
    E_TransferSyntax newxfer = EXS_Unknown;
    DcmStack stack;
    if (metainfo && metainfo->search(DCM_TransferSyntaxUID, stack) == EC_Normal
	)
    {
	DcmUniqueIdentifier *xferUI = (DcmUniqueIdentifier*)(stack.top());
        if ( xferUI->getTag().getXTag() == DCM_TransferSyntaxUID )
	{
            char * xferid = NULL;
	    xferUI->getString(xferid);     // auslesen der ID
            DcmXfer localXfer(xferid);      // dekodieren in E_TransferSyntax
            newxfer = localXfer.getXfer();
	    debug(4, ( "DcmFileFormat::lookForXfer() detected xfer=%d=[%s] in MetaInfo",
		    newxfer,
		    localXfer.getXferName() ));
	}
    }
    return newxfer;
}


// ********************************


Uint32 DcmFileFormat::calcElementLength(const E_TransferSyntax xfer,
					const E_EncodingType enctype)
{
    return getMetaInfo() -> calcElementLength(xfer, enctype) + 
	getDataset() -> calcElementLength(xfer, enctype);
}

// ********************************


BOOL DcmFileFormat::canWriteXfer(const E_TransferSyntax newXfer,
				 const E_TransferSyntax oldXfer)
{
    DcmDataset * dataset = getDataset();

    if (dataset)
	return dataset -> canWriteXfer(newXfer, oldXfer);
    else
	return FALSE;
}
	   

// ********************************


E_Condition DcmFileFormat::read(DcmStream & inStream,
				const E_TransferSyntax xfer,
				const E_GrpLenEncoding glenc,
				const Uint32 maxReadLength)

{
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
		errorFlag = metaInfo->read(inStream, xfer, glenc,
					   maxReadLength );
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
		    errorFlag = dataset->read(inStream, newxfer, glenc,
					      maxReadLength);
		}
	    }
	}
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
				 const E_EncodingType enctype)
{
    return write(outStream, oxfer, enctype, EGL_recalcGL, EPD_noChange);
}


// ********************************

E_Condition DcmFileFormat::write(DcmStream & outStream,
				 const E_TransferSyntax oxfer,
				 const E_EncodingType enctype,
				 const E_GrpLenEncoding glenc,
				 const E_PaddingEncoding padenc,
				 const Uint32 padlen,
				 const Uint32 subPadlen,
				 Uint32 instanceLength)
{
    if (fTransferState == ERW_notInitialized)
	errorFlag = EC_IllegalCall;
    else
    {
	DcmDataset * dataset = this -> getDataset();
	DcmMetaInfo * metainfo = this -> getMetaInfo();

	E_TransferSyntax outxfer = oxfer;
	if (outxfer == EXS_Unknown && dataset)
	    outxfer = dataset->getOriginalXfer();

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
		errorFlag = 
		    metainfo -> write(outStream, outxfer, enctype);
		instanceLength += 
		    metainfo -> calcElementLength(outxfer, enctype);

		if (errorFlag == EC_Normal)
		    errorFlag = dataset -> write(outStream, outxfer, enctype, 
						 glenc, padenc, padlen, 
						 subPadlen, instanceLength);
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
    cerr << "Warning: illegal call of DcmFileFormat::clear()" << endl;
    errorFlag = EC_IllegalCall;
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


DcmDataset* DcmFileFormat::getAndRemoveDataset()
{
    errorFlag = EC_Normal;
    DcmDataset *data = (DcmDataset*)NULL;
    if (    itemList->seek_to( 1 ) != (DcmItem*)NULL
	    && itemList->get()->ident() == EVR_dataset )
    {
	data = (DcmDataset*)( itemList->remove() );
	DcmDataset *Dataset = new DcmDataset();
	DcmSequenceOfItems::itemList->insert(Dataset, ELP_last);
    }
    else
	errorFlag = EC_IllegalCall;
    return data;
}


// ********************************


/*
** CVS/RCS Log:
** $Log: dcfilefo.cc,v $
** Revision 1.11  1997-07-03 15:09:57  andreas
** - removed debugging functions Bdebug() and Edebug() since
**   they write a static array and are not very useful at all.
**   Cdebug and Vdebug are merged since they have the same semantics.
**   The debugging functions in dcmdata changed their interfaces
**   (see dcmdata/include/dcdebug.h)
**
** Revision 1.10  1997/05/27 13:48:59  andreas
** - Add method canWriteXfer to class DcmObject and all derived classes.
**   This method checks whether it is possible to convert the original
**   transfer syntax to an new transfer syntax. The check is used in the
**   dcmconv utility to prohibit the change of a compressed transfer
**   syntax to a uncompressed.
**
** Revision 1.9  1997/05/16 08:23:54  andreas
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
** Revision 1.8  1997/04/18 08:17:17  andreas
** - The put/get-methods for all VRs did not conform to the C++-Standard
**   draft. Some Compilers (e.g. SUN-C++ Compiler, Metroworks
**   CodeWarrier, etc.) create many warnings concerning the hiding of
**   overloaded get methods in all derived classes of DcmElement.
**   So the interface of all value representation classes in the
**   library are changed rapidly, e.g.
**   E_Condition get(Uint16 & value, const unsigned long pos);
**   becomes
**   E_Condition getUint16(Uint16 & value, const unsigned long pos);
**   All (retired) "returntype get(...)" methods are deleted.
**   For more information see dcmdata/include/dcelem.h
**
** Revision 1.7  1996/08/05 08:46:11  andreas
** new print routine with additional parameters:
**         - print into files
**         - fix output length for elements
** corrected error in search routine with parameter ESM_fromStackTop
**
** Revision 1.6  1996/07/29 15:47:00  andreas
** add method getAndRemoveDataset to remove connection of dataset from fileformat
**
** Revision 1.5  1996/04/12 13:17:22  andreas
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
