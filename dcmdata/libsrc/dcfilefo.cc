/*
 *
 * Author: Gerd Ehlers      Created:  05-15-94
 *                          Modified: 02-15-95
 *
 * Module: dcfilefo.cc
 *
 * Purpose:
 * Implementation of the class DcmFileFormat
 *
 *
 * Last Update:   $Author: hewett $
 * Revision:      $Revision: 1.2 $
 * Status:	  $State: Exp $
 *
 */


#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
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
    : DcmSequenceOfItems( InternalUseTag )
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


DcmFileFormat::DcmFileFormat( DcmDataset *dataset )
    : DcmSequenceOfItems( InternalUseTag )
{
Bdebug((5, "dcfilefo:DcmFileFormat::DcmFileFormat(DcmDataset*)" ));
debug(( 8, "Object pointer this=0x%p", this ));

    DcmMetaInfo *MetaInfo = new DcmMetaInfo();
    DcmSequenceOfItems::itemList->insert( MetaInfo );
    DcmDataset *newDataset;

    if ( dataset == (DcmDataset*)NULL ) {
	newDataset = new DcmDataset();
    } else {
	newDataset = new DcmDataset( *dataset );
    }
    DcmSequenceOfItems::itemList->insert( newDataset );
Edebug(());

}


// ********************************


DcmFileFormat::DcmFileFormat( iDicomStream *iDStream )
    : DcmSequenceOfItems( InternalUseTag, 0, iDStream )
{
Bdebug((5, "dcfilefo:DcmFileFormat::DcmFileFormat(*iDS)" ));
debug(( 8, "Object pointer this=0x%p", this ));

    DcmMetaInfo *MetaInfo = new DcmMetaInfo( iDStream );
    DcmSequenceOfItems::itemList->insert( MetaInfo );
    DcmDataset *Dataset = new DcmDataset( iDStream );
    DcmSequenceOfItems::itemList->insert( Dataset );
Edebug(());

}


// ********************************


DcmFileFormat::DcmFileFormat( const DcmFileFormat &old )
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


DcmEVR DcmFileFormat::ident() const
{
    return EVR_fileFormat;
}


// ********************************


void DcmFileFormat::print( int level )
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
	do {
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


E_Condition DcmFileFormat::checkValue( DcmMetaInfo *metainfo,
				       DcmDataset *dataset,
				       const DcmTagKey &atagkey,
				       DcmObject* obj,
				       E_TransferSyntax oxfer )
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
            T_VR_UL temp = 0;
            if ( elem->getLength() == 0 && elem->ident() == EVR_UL )
                ((DcmUnsignedLong*)elem)->put( &temp, 1 );
            // Laengenberechnung erfolgt in validateMetaInfo()
	} else if ( xtag == DCM_FileMetaInformationVersion ) {	// (0002,0001)
            if ( elem == (DcmElement*)NULL )
            {
                elem = new DcmOtherByteOtherWord( tag );
                metainfo->insert( elem, TRUE );
            }
            BYTE version[2] = {0,1};
            if ( elem->getLength() == 0 && elem->ident() == EVR_OB )
                ((DcmOtherByteOtherWord*)elem)->put( version, 2 );

            // ueberpruefe Version des MetaHeaders
            BYTE *currVers = ((DcmOtherByteOtherWord*)elem)->getBytes();
            if (    ((currVers[0] & version[0] & 0xff) == version[0])
                 && ((currVers[1] & version[1] & 0xff) == version[1]) ) {
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
                    cerr << "Warning: dcfilefo: I can't find"
                        " DCM_SOPClassUID in Dataset!" << endl;
                    l_error = EC_CorruptedData;
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
		    ((DcmUniqueIdentifier*)elem)->put( uid );
debug(( 2, "use new transfer-syntax [%s] on writing following Dataset",
           dcXfer.getXferName() ));

		}
	} else if ( xtag == DCM_ImplementationClassUID ) {	// (0002,0012)
		if ( elem == (DcmElement*)NULL )
                {
                    elem = new DcmUniqueIdentifier( tag );
                    metainfo->insert( elem, TRUE );
                }
                //old: if ( elem->getLength() == 0 && elem->ident() == EVR_UI )
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
                //old: if ( elem->getLength() == 0 && elem->ident() == EVR_SH )
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



E_Condition DcmFileFormat::validateMetaInfo( E_TransferSyntax oxfer )
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


E_TransferSyntax DcmFileFormat::lookForXfer( DcmMetaInfo* metainfo )
{
Bdebug((4, "dcfilefo:DcmFileFormat::lookForXfer(metainfo*)" ));

    E_TransferSyntax newxfer = EXS_UNKNOWN;
    DcmStack stack;
    if (    metainfo != (DcmMetaInfo*)NULL
	 && metainfo->search( DCM_TransferSyntaxUID, stack ) == EC_Normal
       )
    {
	DcmUniqueIdentifier *xferUI = (DcmUniqueIdentifier*)(stack.top());
        if ( xferUI->getTag().getXTag() == DCM_TransferSyntaxUID )
	{
            const char *xferid = xferUI->get();     // auslesen der ID
            DcmXfer localXfer( xferid );      // dekodieren in E_TransferSyntax
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


E_Condition DcmFileFormat::read( E_TransferSyntax xfer,
                                 E_GrpLenEncoding gltype )
{
Bdebug((3, "dcfilefo:DcmFileFormat::read(xfer=%d,gltype=%d)",
           xfer, gltype ));

    errorFlag = EC_Normal;
    valueModified = FALSE;
    E_TransferSyntax newxfer = xfer;

    if ( iDS == (iDicomStream*)NULL )
        errorFlag = EC_InvalidStream;
    else if ( iDS->eof() )
	errorFlag = EC_EndOfFile;
    else if ( !iDS->good() )
	errorFlag = EC_InvalidStream;
    else
    {
	if ( !itemList->empty() )   // Es sind schon vorher Daten gelesen worden
	    valueModified = TRUE;
	DcmSequenceOfItems::itemList->seek( ELP_last );
				    // Die neuen Daten werden ans Ende gehaengt

	DcmMetaInfo *MetaInfo = getMetaInfo();
	if ( MetaInfo == (DcmMetaInfo*)NULL )
	{
	    MetaInfo = new DcmMetaInfo( iDS );
	    DcmSequenceOfItems::itemList->insert( MetaInfo, ELP_first );
	}
        errorFlag = MetaInfo->read( xfer, gltype );
debug(( 3, "MetaInfo has been read." ));

	// lese aus MetaInfo() Tag(0002,0010) aus und bestimme xfer
	newxfer = lookForXfer( MetaInfo );

	if ( errorFlag == EC_Normal )
	{
	    DcmDataset *Dataset = getDataset();
	    if ( Dataset == (DcmDataset*)NULL )
	    {
		Dataset = new DcmDataset( iDS );
		DcmSequenceOfItems::itemList->seek( ELP_first );
		DcmSequenceOfItems::itemList->insert( Dataset, ELP_next );
	    }
            errorFlag = Dataset->read( newxfer, gltype );
	}
debug(( 3, "Dataset has been read." ));

    }
Edebug(());

    return errorFlag;
}  // DcmFileFormat::read()


// ********************************


E_Condition DcmFileFormat::write( oDicomStream &oDS,
				  E_TransferSyntax oxfer,
                                  E_EncodingType enctype,
                                  E_GrpLenEncoding gltype )
{
Bdebug((3, "dcfilefo:DcmFileFormat::write(&oDS,oxfer=%d,enctype=%d,"
           "gltype=%d)", oxfer, enctype, gltype ));

    errorFlag = EC_Normal;
    if ( oxfer == EXS_UNKNOWN || oxfer == EXS_BigEndianImplicit )
	errorFlag = EC_IllegalCall;
    else if ( !oDS.good() )
	errorFlag = EC_InvalidStream;
    else if ( itemList->empty() )
	errorFlag = EC_CorruptedData;
    else
    {
	validateMetaInfo( oxfer );
	DcmObject *dO;
	itemList->seek( ELP_first );
	do {
	    dO = itemList->get();
            errorFlag = dO->write( oDS, oxfer, enctype, gltype );
	    if ( errorFlag != EC_Normal )
		break;
	} while ( itemList->seek( ELP_next ) );

	valueModified = FALSE;
    }
    if ( oxfer == EXS_BigEndianImplicit )
    {
        cerr << "Error: DcmFileFormat::write() illegal TransferSyntax(BI)"
                " used" << endl;
    }
Edebug(());

    return errorFlag;
}


// ********************************


E_Condition DcmFileFormat::readBlock( E_TransferSyntax xfer,
                                      E_GrpLenEncoding gltype )
{
Bdebug((3, "dcfilefo:DcmFileFormat::readBlock(xfer=%d,gltype=%d)",
           xfer, gltype ));

    cerr << "Warning: illegal call of DcmFileFormat::readBlock()" << endl;
    errorFlag = EC_IllegalCall;

Edebug(());

    return errorFlag;
}


// ********************************


E_Condition DcmFileFormat::writeBlock( oDicomStream & /*oDS*/,
				       E_TransferSyntax oxfer,
                                       E_EncodingType enctype,
                                       E_GrpLenEncoding gltype )
{
Bdebug((3, "dcfilefo:DcmFileFormat::writeBlock(&oDS,oxfer=%d,enctype=%d,"
           "gltype=%d)", oxfer, enctype, gltype ));

    cerr << "Warning: illegal call of DcmFileFormat::writeBlock()" << endl;
    errorFlag = EC_IllegalCall;
Edebug(());

    return errorFlag;
}


// ********************************


E_Condition DcmFileFormat::insertItem( DcmItem* /*item*/,
                                       T_VR_UL /*where*/ )
{
    cerr << "Warning: illegal call of DcmFileFormat::insert(DcmItem*,T_VR_UL)"
         << endl;
    errorFlag = EC_IllegalCall;

    return errorFlag;
}


// ********************************


DcmItem* DcmFileFormat::remove( T_VR_UL /*num*/ )
{
    cerr << "Warning: illegal call of DcmFileFormat::remove(T_VR_UL)" << endl;
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


