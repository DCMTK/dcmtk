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
 * Revision:      $Revision: 1.1 $
 * Status:	  $State: Exp $
 *
 */


#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include <time.h>
#include <iostream.h>


//#ifdef SOLARIS2
//#include <sys/types.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
//#endif

#include "dcfilefo.h"
#include "dcitem.h"
#include "dcxfer.h"
#include "dcmodtbl.h"
#include "dcvrobow.h"
#include "dcvrui.h"
#include "dcvrul.h"
#include "dcvrus.h"
#include "dcvrae.h"
#include "dcvrsh.h"
#include "dcdebug.h"


extern char *VRTypesName[];
extern short DIM_OF_VRTypes;


#ifndef HAVE_GETHOSTID
#ifdef HAVE_SYSINFO

#include <sys/systeminfo.h>
static long gethostid(void)
{
    char buf[128];
    if (sysinfo(SI_HW_SERIAL, buf, 128) == -1) {
       perror("sysinfo");
       exit(1);
    }
#ifdef HAVE_STRTOUL
    return(strtoul(buf, NULL, 0));
#else
	long result;
	sscanf(buf, "%ld", &result);
	return result;
#endif
}

#else // !HAVE_SYSINFO

static long gethostid(void) { return 4711; }   // workaround for MSDOS

#endif // !HAVE_SYSINFO

#else // HAVE_GETHOSTID

#ifndef HAVE_UNISTD_H
extern "C" {
int gethostid();
int getpid();
}
#endif

#endif // HAVE_GETHOSTID


static long counterOfCurrentUID = 0;


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

    if ( dataset == (DcmDataset*)NULL )
    {
	newDataset = new DcmDataset();
    }
    else
    {
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


DcmFileFormat::DcmFileFormat( const DcmObject &oldObj )
    : DcmSequenceOfItems( InternalUseTag )
{
Bdebug((5, "dcfilefo:DcmFileFormat::DcmFileFormat(DcmObject&)" ));
debug(( 8, "Object pointer this=0x%p", this ));

    lastItemComplete = TRUE;
    itemList = new DcmList;
debug(( 5, "ident()=%d", oldObj.ident() ));

    if ( oldObj.ident() == EVR_fileFormat )
    {
        DcmFileFormat const *old = (DcmFileFormat const *)&oldObj;
	*Tag = *old->Tag;
	iDS = old->iDS;
	offsetInFile  = old->offsetInFile;
	valueInMemory = old->valueInMemory;
	valueModified = old->valueModified;
	Length = old->Length;
	Xfer = old->Xfer;
        if ( !old->itemList->empty() )
	{
	    DcmObject *oldDO;
	    DcmObject *newDO;
	    itemList->seek( ELP_first );
            old->itemList->seek( ELP_first );
	    do {
                oldDO = old->itemList->get();
		if ( oldDO->ident() == EVR_metainfo )
		    newDO = new DcmMetaInfo( *oldDO );
		else if ( oldDO->ident() == EVR_dataset )
		    newDO = new DcmDataset( *oldDO );
		else
		{
		    newDO = new DcmItem( oldDO->getTag() );
                    cerr << "Error: DcmFileFormat(): Element("
                         << hex << oldDO->getGTag() << "," << oldDO->getETag()
                         << dec
                         << ") found, which was not a MetaInfo or Dataset"
                         << endl;
		}
		itemList->insert( newDO, ELP_next );
            } while ( old->itemList->seek( ELP_next ) );
	}
    }
    else
    {
        cerr << "Warning: DcmFileFormat: wrong use of Copy-Constructor"
             << endl;
    }
Edebug(());

}


// ********************************


DcmFileFormat::DcmFileFormat( const DcmFileFormat &oldForm )
    : DcmSequenceOfItems( InternalUseTag )
{
Bdebug((5, "dcfilefo:DcmFileFormat::DcmFileFormat(DcmFileFormat&)" ));
debug(( 8, "Object pointer this=0x%p", this ));

    lastItemComplete = TRUE;
    itemList = new DcmList;
debug(( 5, "ident()=%d", oldForm.ident() ));

    if ( oldForm.ident() == EVR_fileFormat )
    {
        DcmFileFormat const *old = &oldForm;
	*Tag = *old->Tag;
	iDS = old->iDS;
	offsetInFile  = old->offsetInFile;
	valueInMemory = old->valueInMemory;
	valueModified = old->valueModified;
	Length = old->Length;
	Xfer = old->Xfer;
        if ( !old->itemList->empty() )
	{
	    DcmObject *oldDO;
	    DcmObject *newDO;
	    itemList->seek( ELP_first );
            old->itemList->seek( ELP_first );
	    do {
                oldDO = old->itemList->get();
		if ( oldDO->ident() == EVR_metainfo )
		    newDO = new DcmMetaInfo( *oldDO );
		else if ( oldDO->ident() == EVR_dataset )
		    newDO = new DcmDataset( *oldDO );
		else
		{
		    newDO = new DcmItem( oldDO->getTag() );
                    cerr << "Error: DcmFileFormat(): Element("
                         << hex << oldDO->getGTag() << "," << oldDO->getETag()
                         << dec
                         << ") found, which was not a MetaInfo or Dataset"
                         << endl;
		}
		itemList->insert( newDO, ELP_next );
            } while ( old->itemList->seek( ELP_next ) );
	}
    }
    else
    {
        cerr << "Warning: DcmFileFormat: wrong use of Copy-Constructor"
             << endl;
    }
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


EVR DcmFileFormat::ident() const
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
				       ETag xtag,
				       DcmObject* obj,
				       E_TransferSyntax oxfer )
{
Bdebug((2, "dcfilefo:DcmFileFormat::checkValue(*metainfo,*dataset,xtag=%d,*obj,oxfer=%d)",
	   xtag, oxfer ));

    E_Condition l_error = EC_Normal;
    if ( metainfo != (DcmMetaInfo*)NULL && dataset != (DcmDataset*)NULL )
    {
	DcmStack stack;
        DcmTag tag( xtag );
	if ( obj != (DcmObject*)NULL )
            tag = obj->getTag();

        ETag xtag = tag.getXTag();
	DcmElement *elem = (DcmElement*)obj;
	switch ( xtag )
	{
            case ET_GroupLength0002:              // (0002,0000)
		{
                    if ( elem == (DcmElement*)NULL )
                    {
                        elem = new DcmUnsignedLong( tag );
                        metainfo->insert( elem, TRUE );
                    }
                    T_VR_UL temp = 0;
                    if ( elem->getLength() == 0 && elem->ident() == EVR_UL )
                        ((DcmUnsignedLong*)elem)->put( &temp, 1 );
                    // Laengenberechnung erfolgt in validateMetaInfo()
		}
		break;

            case ET_FileMetaInformationVersion:   // (0002,0001)
		{
                    if ( elem == (DcmElement*)NULL )
                    {
                        elem = new DcmOtherByteOtherWord( tag );
                        metainfo->insert( elem, TRUE );
                    }
                    U_CHAR version[2] = {0,1};
                    if ( elem->getLength() == 0 && elem->ident() == EVR_OB )
                        ((DcmOtherByteOtherWord*)elem)->put( version, 2 );

                    // ueberpruefe Version des MetaHeaders
                    U_CHAR *currVers
                                 = ((DcmOtherByteOtherWord*)elem)->getBytes();
                    if (    ((currVers[0] & version[0] & 0xff) == version[0])
                         && ((currVers[1] & version[1] & 0xff) == version[1])
                       )
                    {
debug(( 2, "Version of MetaHeader is ok: 0x%2.2x%2.2x",
           currVers[1], currVers[0] ));

                    }
                    else
                    {
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
                }
                break;

            case ET_MediaStoredSOPClassUID:       // (0002,0002)
		if ( elem == (DcmElement*)NULL )
                {
                    elem = new DcmUniqueIdentifier( tag );
                    metainfo->insert( elem, TRUE );
                }
                if ( elem->getLength() == 0 && elem->ident() == EVR_UI )
		{
		    if ( dataset->search( ET_SOPClassUID, stack )
			 == EC_Normal )
		    {
			char *uid = ((DcmUniqueIdentifier*)stack.top())->get();
			((DcmUniqueIdentifier*)elem)->put( uid );
debug(( 2, "use SOPClassUID [%s]", uid ));

		    }
                    else
		    {
                        cerr << "Warning: dcfilefo: I can't find"
                                " ET_SOPClassUID in Dataset!" << endl;
                        l_error = EC_CorruptedData;
		    }
		}
		break;

            case ET_MediaStoredSOPInstanceUID:    // (0002,0003)
		if ( elem == (DcmElement*)NULL )
                {
                    elem = new DcmUniqueIdentifier( tag );
                    metainfo->insert( elem, TRUE );
                }
                if ( elem->getLength() == 0 && elem->ident() == EVR_UI )
		{
                    char *uid = (char*)NULL;
		    if ( dataset->search( ET_SOPInstanceUID, stack )
			 == EC_Normal )
		    {
                        uid = ((DcmUniqueIdentifier*)stack.top())->get();
                        ((DcmUniqueIdentifier*)elem)->put( uid );
debug(( 2, "use SOPInstanceUID [%s] from Dataset", uid ));

		    }
                    else
		    {
                        uid = new char[1024];
                        // time() get the seconds
                        //        since 00:00:00 GMT, Jan.  1,  1970
                        sprintf( uid, "%s.%s.%ld.%ld.%ld.%ld",
                                     DIC_EURO_UIDROOT,
                                     DIC_EURO_UIDROOT_INFIX,
                                     gethostid(),
                                     (long)getpid(),
                                     time(NULL),
                                     counterOfCurrentUID++ );
                        ((DcmUniqueIdentifier*)elem)->put( uid );
debug(( 2, "use new generated SOPInstanceUID [%s]", uid ));

                        delete uid;
		    }
		}
		break;

            case ET_TransferSyntaxUID:            // (0002,0010)
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
		    char *uid = dcXfer.getXferID();
		    ((DcmUniqueIdentifier*)elem)->put( uid );
debug(( 2, "use new transfer-syntax [%s] on writing following Dataset",
           dcXfer.getXferName() ));

		}
		break;

            case ET_ImplementationClassUID:       // (0002,0012)
		if ( elem == (DcmElement*)NULL )
                {
                    elem = new DcmUniqueIdentifier( tag );
                    metainfo->insert( elem, TRUE );
                }
                //old: if ( elem->getLength() == 0 && elem->ident() == EVR_UI )
                if ( elem->ident() == EVR_UI )
		{
		    char *uid = DIC_EURO_IMPLEMENTATIONCLASSUID;
		    ((DcmUniqueIdentifier*)elem)->put( uid );
		}
		break;

            case ET_ImplementationVersionName:    // (0002,0013)
		if ( elem == (DcmElement*)NULL )
                {
                    elem = new DcmShortString( tag );
                    metainfo->insert( elem, TRUE );
                }
                //old: if ( elem->getLength() == 0 && elem->ident() == EVR_SH )
                if ( elem->ident() == EVR_SH )
		{
		    char *uid = DIC_EURO_DTK_IMPLEMENTATIONVERSIONNAME;
		    ((DcmShortString*)elem)->put( uid );
		}
		break;

            case ET_SourceApplicationEntityTitle: // (0002,0016)
		if ( elem == (DcmElement*)NULL )
                {
                    elem = new DcmApplicationEntity( tag );
                    metainfo->insert( elem, TRUE );
                }
                cerr << "Error: dcfilefo: I don't know how to handle"
                        " ET_SourceApplicationEntityTitle!" << endl;
		break;

            case ET_PrivateInformationCreatorUID: // (0002,0100)
		if ( elem == (DcmElement*)NULL )
                {
                    elem = new DcmUniqueIdentifier( tag );
                    metainfo->insert( elem, TRUE );
                }
                cerr << "Error: dcfilefo: I don't know how to handle"
                        " ET_PrivateInformationCreatorUID!" << endl;
		break;

            case ET_PrivateInformation:           // (0002,0102)
		if ( elem == (DcmElement*)NULL )
                {
                    elem = new DcmOtherByteOtherWord( tag );
                    metainfo->insert( elem, TRUE );
                }
                cerr << "Warning: dcfilefo: I don't know how to handle"
                        " ET_PrivateInformation!" << endl;
		break;

	    default:
		break;
	}
        if ( elem == (DcmElement*)NULL )
            l_error = EC_InvalidVR;
    }
    else
        l_error = EC_IllegalCall;
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
    DcmModuleTable modtable( "DICOMFileMetaInformationHeader" );

    modtable.setModuleTable(0);
    if (    modtable.setModuleAttributeGroup(0) == EC_Normal
	 && metinf != (DcmMetaInfo*)NULL
	 && datset != (DcmDataset*)NULL
       )
    {
	DcmStack stack;
	short maxentries = modtable.getModuleAttributeMaxNum();
	for ( short i = 0; i < maxentries; i++ )
	{
debug(( 2, "testing table entry %d of %d", i+1, maxentries ));

	    ETag tab_xtag = modtable.getModuleAttribute_TagType( i )->XTag;
debug(( 2, "getModuleAttribute_TagType(%d)->XTag=%d", i, tab_xtag ));

	    switch ( modtable.getModuleAttribute_TagType( i )->Type )
	    {
		case EType_1:
		    if ( metinf->search( tab_xtag, stack, ESM_fromHere, FALSE )
			 != EC_Normal )
			;			// do nothing
		    checkValue( metinf, datset, tab_xtag, stack.top(), oxfer );
		    break;

		case EType_1C:
		    {
		    ETag tmp_x = modtable.getModuleAttribute_TagType(i-1)->XTag;
                    if ( metinf->search( tmp_x, stack, ESM_fromHere, FALSE )
                         == EC_Normal )
		    {
			if ( metinf->search( tab_xtag, stack, ESM_fromHere, FALSE )
			    != EC_Normal )
			;			// do nothing
			checkValue( metinf, datset, tab_xtag, stack.top(), oxfer );
		    }
		    }
		    break;

		case EType_3:
		    if ( tab_xtag == ET_ImplementationVersionName )
		    {
			if ( metinf->search( tab_xtag, stack, ESM_fromHere, FALSE )
			     != EC_Normal )
			    ;			// do nothing
			checkValue( metinf, datset, tab_xtag, stack.top(), oxfer );
		    }
		    break;

		default:
debug(( 2, "ETypeAttr of element(%d) was not EType_1, EType_1C or EType_3.",
	   i ));

		    break;
	    }
	}
debug(( 2, "DcmFileFormat:found %ld Elements in DcmMetaInfo metinf.",
           metinf->card() ));

	// berechne neue GroupLength
        if ( metinf->addGroupLengthElements( META_HEADER_DEFAULT_TRANSFERSYNTAX,
                                             EET_UndefinedLength ) != EC_Normal
           )
        {
            cerr << "Error: DcmFileFormat::validateMetaInfo(): group length"
                    " of Meta Information Header not adapted."
                 << endl;
        }
    }
    else
        l_error = EC_CorruptedData;
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
	 && metainfo->search( ET_TransferSyntaxUID, stack ) == EC_Normal
       )
    {
	DcmUniqueIdentifier *xferUI = (DcmUniqueIdentifier*)(stack.top());
        if ( xferUI->getTag().getXTag() == ET_TransferSyntaxUID )
	{
            char *xferid = xferUI->get();     // auslesen der ID
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


