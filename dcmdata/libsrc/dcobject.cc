/*
 *
 * Author: Gerd Ehlers      Created:  03-27-94
 *                          Modified: 02-07-95
 *
 * Module: dcobject.cc
 *
 * Purpose:
 * Implementation of the base class object
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

#include <iostream.h>

#include "dcobject.h"
#include "dcvr.h"
#include "dcxfer.h"
#include "dcdebug.h"



// ****** public methods **********************************



DcmObject::DcmObject( DcmTag &tag )
{
Bdebug((6, "dcobject:DcmObject::DcmObject(DcmTag&)" ));
debug(( 8, "Object pointer this=0x%p", this ));

    Tag = new DcmTag( tag );    // copy-operator in DcmTag
    Length = 0L;
    iDS = (iDicomStream*)NULL;
    Xfer = EXS_UNKNOWN;
    offsetInFile = 0;
    valueInMemory = TRUE;
    valueModified = FALSE;
    errorFlag = EC_Normal;
    rdStat = wrStat = ERW_init;
    bytesRead = bytesWritten = 0;

    testConstructDestruct = 1; // for debugging

Edebug(());

}


// ********************************


DcmObject::DcmObject( DcmTag &tag, T_VR_UL len, iDicomStream *iDStream )
{
Bdebug((6, "dcobject:DcmObject::DcmObject(DcmTag&,len=%ld,*iDs)", len ));
debug(( 8, "Object pointer this=0x%p", this ));

    Tag = new DcmTag( tag );    // copy-operator in DcmTag
    Length = len;
    iDS = iDStream;
    Xfer = EXS_UNKNOWN;
    offsetInFile = 0;
    valueInMemory = TRUE;
    valueModified = FALSE;
    errorFlag = EC_Normal;
    rdStat = wrStat = ERW_init;
    bytesRead = bytesWritten = 0;

    testConstructDestruct = 1; // for debugging

Edebug(());

}


// ********************************


DcmObject::~DcmObject()
{
Bdebug((6, "dcobject:DcmObject::~DcmObject()" ));
debug(( 8, "Object pointer this=0x%p", this ));
debug(( 8, "Pointer to Tag=0x%p", Tag ));

    if (Tag != (DcmTag*)NULL)
	delete Tag;

    if ( testConstructDestruct == 1 )                   // for debugging
        testConstructDestruct = 2; // for debugging
    else
    {
debug(( 1, "Error: ~DcmObject called more than once (%d)",
           testConstructDestruct ));

        cerr << "Error: ~DcmObject called more than once ("
             << testConstructDestruct << ")" << endl;
        testConstructDestruct++;
    }
Edebug(());

}


// ********************************


E_TransferSyntax DcmObject::getOriginalXfer()
{
    return Xfer;
}


// ********************************


T_VR_US DcmObject::getGTag()
{
    T_VR_US grpTag = Tag->getGTag();
    errorFlag = Tag->error();
    return grpTag;
}


// ********************************


T_VR_US DcmObject::getETag()
{
    T_VR_US elemTag = Tag->getETag();
    errorFlag = Tag->error();
    return elemTag;
}


// ********************************


DcmTag& DcmObject::getTag()
{
    return *Tag;
}


// ********************************


EVR DcmObject::getVR()
{
    EVR vr = Tag->getVR();
    errorFlag = Tag->error();
    return vr;
}


// ********************************


E_Condition DcmObject::setVR( EVR /*vr*/ )
{
    return EC_Normal;
}


// ********************************


T_VR_UL DcmObject::getLength( E_TransferSyntax /*xfer*/, E_EncodingType /*enctype*/ )
{
    return Length;
}


// ********************************


E_Condition DcmObject::error()
{
    return errorFlag;
}


// ********************************


E_ReadWriteState DcmObject::readState()
{
    return rdStat;
}


// ********************************


E_ReadWriteState DcmObject::writeState()
{
    return wrStat;
}


// ********************************


E_Condition DcmObject::readBlockInit()
{
    rdStat = ERW_init;
    bytesRead = 0;
    return EC_Normal;
}


// ********************************


E_Condition DcmObject::writeBlockInit()
{
    wrStat = ERW_init;
    bytesWritten = 0;
    return EC_Normal;
}


// ********************************


E_Condition DcmObject::search( DcmTag &tag,
			       DcmStack &resultStack,
			       E_SearchMode mode,
			       BOOL searchIntoSub )
{
Bdebug((5, "dcobject:DcmObject::search(tag=(%4.4x,%4.4x),Stack&(%ld),mode=%d,"
           "sub=%d)", tag.getGTag(), tag.getETag(), resultStack.card(),
           mode, searchIntoSub ));
debug(( 5, "local Info: Tag=(%4.4x,%4.4x) \"%s\" p=%p",
           getGTag(), getETag(), DcmVR(getVR()).getVRName(), this ));

    E_Condition l_error = EC_TagNotFound;
/*
    if ( tag == *Tag )
    {
        resultStack.push( this );
        l_error = EC_Normal;
debug(( 4, "Element found with Tag=(%4.4x,%4.4x) \"%s\"",
	   getGTag(), getETag(), DcmVR(getVR()).getVRName() ));
    }
*/
Edebug(());

    return l_error;
}


// ********************************


E_Condition DcmObject::search( ETag etag,
			       DcmStack &resultStack,
			       E_SearchMode mode,
			       BOOL searchIntoSub )
{
Bdebug((5, "dcobject:DcmObject::search(etag=%d,Stack&,mode=%d,sub=%d)",
	   etag, mode, searchIntoSub ));

    DcmTag tag( etag );
    E_Condition l_error = search( tag, resultStack, mode, searchIntoSub );
Edebug(());

    return l_error;
}


// ********************************


E_Condition DcmObject::searchErrors( DcmStack &resultStack )
{
Bdebug((5, "dcobject:DcmObject::searchErrors(Stack&)" ));

    if ( errorFlag != EC_Normal )
	resultStack.push( this );
Edebug(());

    return errorFlag;
}



// ***********************************************************
// ****** protected methods **********************************
// ***********************************************************


void DcmObject::printInfoLine( int level, char *info )
{
    printInfoLine( level, *Tag, Length, info );
}


// ********************************


void DcmObject::printInfoLine( int level, DcmTag &tag, T_VR_UL length,
			       char *info )
{
    DcmVR vr( tag.getVR() );

    for ( int i=1; i<level; i++)
	printf("    ");
    printf( "%4.4x %4.4x %-5.5s %-38.1500s#%6ld,%3ld,%3d  %s\n",
               tag.getGTag(),
               tag.getETag(),
	       vr.getVRName(),
	       info,
	       length,
	       getVM(),
               tag.getMaxVM(),
               tag.getTagName()
             );
}


// ********************************


E_Condition DcmObject::writeTagAndLength( oDicomStream &oDS,            // in
					  E_TransferSyntax oxfer,	// in
					  T_VR_UL *written_bytes )	// out
{
Bdebug((4, "dcobject:DcmObject::writeTagAndLength(&oDS,oxfer=%d,*written_bytes)",
	   oxfer ));

    E_Condition l_error = EC_Normal;
    if ( !oDS.good() )
    {
	*written_bytes = 0;
        l_error = EC_InvalidStream;
    }
    else
    {
debug(( 4, "Tag (0x%4.4x,0x%4.4x) \"%s\" [0x%8.8x] \"%s\"",
	   Tag->getGTag(), Tag->getETag(),
	   DcmVR(Tag->getVR()).getVRName(),
	   Length, Tag->getTagName() ));

        oDS.setDataByteOrder( oxfer );
        oDS << Tag->getGTag();         // 2 Byte Laenge; Transfer Syntax !!
        oDS << Tag->getETag();         // 2 Byte Laenge; Transfer Syntax !!
	*written_bytes = 4;

        DcmXfer oxferSyn( oxfer );
        if ( oxferSyn.isExplicitVR() )
	{
	    EVR vr = DcmVR( Tag->getVR() ).getValidVR();
					// Umwandlung in gueltige VR
	    char *vrname = DcmVR( Tag->getVR() ).getValidVRName();
					// Umwandlung in gueltige Strings
            oDS.write( vrname, 2 );    // 2 Byte Laenge:VR als string
	    *written_bytes += 2;

#if defined(DCMCODE_OLD_EX_COMPATIBILITY)
            oDS << Length;             // 4 Byte Laenge; Transfer Syntax !!
	    *written_bytes += 4;
	    if ( vr == EVR_OB ) ;	// no compiler warning
#else
	    if ( vr == EVR_OB || vr == EVR_OW || vr == EVR_SQ )
	    {
		T_VR_US reserved = 0;
                oDS << reserved;       // 2 Byte Laenge; Transfer Syntax !!
                oDS << Length;         // 4 Byte Laenge; Transfer Syntax !!
		*written_bytes += 6;
	    }
	    else
	    {
		T_VR_US len_s = (T_VR_US)Length;   // funktioniert evtl. nicht
                oDS << len_s;          // 2 Byte Laenge; Transfer Syntax !!
		*written_bytes += 2;
	    }
#endif
        } // if ( oxferSyn.isExplicitVR() )
	else
	{
            oDS << Length;              // 4 Byte Laenge; Transfer Syntax !!
	    *written_bytes += 4;
	}
    }
Edebug(());

    return l_error;
}


// ********************************


