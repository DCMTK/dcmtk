/*
 *
 * Author: Gerd Ehlers      Created:  06-03-94
 *                          Modified: 02-07-95
 *
 * Module: dcvrulup.cc
 *
 * Purpose:
 * Implementation of class DcmUnsignedLongOffset
 *
 *
 * Last Update:   $Author: hewett $
 * Revision:      $Revision: 1.1 $
 * Status:	  $State: Exp $
 *
 */



#include <stdio.h>
#include <string.h>
#include <iostream.h>

#include "dcvrul.h"
#include "dcvrulup.h"
#include "dcdebug.h"



// ********************************


DcmUnsignedLongOffset::DcmUnsignedLongOffset( DcmTag &tag )
    : DcmUnsignedLong( tag )
{
Bdebug((5, "dcvrulup:DcmUnsignedLongOffset::DcmUnsignedLongOffset(DcmTag&)" ));
debug(( 8, "Object pointer this=0x%p", this ));

    nextRecord = (DcmObject*)NULL;
Edebug(());

}


// ********************************


DcmUnsignedLongOffset::DcmUnsignedLongOffset( DcmTag &tag,
				  T_VR_UL len,
				  iDicomStream *iDStream )
    : DcmUnsignedLong( tag, len, iDStream )
{
Bdebug((5, "dcvrulup:DcmUnsignedLongOffset::DcmUnsignedLongOffset(DcmTag&,"
           "len=%ld,*iDS)", len ));
debug(( 8, "Object pointer this=0x%p", this ));

    nextRecord = (DcmObject*)NULL;
Edebug(());

}


// ********************************


DcmUnsignedLongOffset::DcmUnsignedLongOffset( const DcmObject &oldObj )
    : DcmUnsignedLong( oldObj )
{
Bdebug((5, "dcvrulup:DcmUnsignedLongOffset::DcmUnsignedLongOffset(DcmObject&)" ));
debug(( 8, "Object pointer this=0x%p", this ));

    if ( oldObj.ident() == EVR_up )
    {
	DcmUnsignedLongOffset const *old
				     = (DcmUnsignedLongOffset const *)&oldObj;
	nextRecord = old->nextRecord;
    }
    else
    {
	nextRecord = (DcmObject*)NULL;
	errorFlag = EC_IllegalCall;
        cerr << "Warning: DcmUnsignedLongOffset: wrong use of Copy-Constructor"
             << endl;
    }
Edebug(());

}


// ********************************


DcmUnsignedLongOffset::DcmUnsignedLongOffset( const DcmUnsignedLongOffset &newULup )
    : DcmUnsignedLong( newULup )
{
Bdebug((5, "dcvrulup:DcmUnsignedLongOffset::DcmUnsignedLongOffset("
           "DcmUnsignedLongOffset&)" ));
debug(( 8, "Object pointer this=0x%p", this ));

    if ( newULup.ident() == EVR_up )
    {
        DcmUnsignedLongOffset const *old = &newULup;
	nextRecord = old->nextRecord;
    }
    else
    {
	nextRecord = (DcmObject*)NULL;
	errorFlag = EC_IllegalCall;
        cerr << "Warning: DcmUnsignedLongOffset: wrong use of Copy-Constructor"
             << endl;
    }
Edebug(());

}


// ********************************


DcmUnsignedLongOffset::~DcmUnsignedLongOffset()
{
Bdebug((5, "dcvrulup:DcmUnsignedLongOffset::~DcmUnsignedLongOffset()" ));
debug(( 8, "Object pointer this=0x%p", this ));
Edebug(());

}


// ********************************


EVR DcmUnsignedLongOffset::ident() const
{
    return EVR_up;
}


// ********************************


void DcmUnsignedLongOffset::print( int level )
{
    if ( UnsigLongValue != (T_VR_UL*)NULL )
    {
	char *ch_words;
	char *tmp = ch_words = new char[ Length*14/sizeof(T_VR_UL) + 4 ];
	T_VR_UL *tattr = UnsigLongValue;
	unsigned int i;

	for ( i=0; i<( Length/sizeof(T_VR_UL) ); i++ )
	{
	    sprintf( tmp, "$%lu\\", *tattr );
	    tmp += strlen(tmp);
	    tattr++;
	}
	if ( Length > 0 )
	    tmp--;
	*tmp = '\0';
	DcmObject::printInfoLine( level, ch_words );
	delete ch_words;
    }
    else if ( valueInMemory == TRUE )
    {
	DcmObject::printInfoLine( level, "(no value available)" );
    }
    else
    {
	DcmObject::printInfoLine( level, "(not loaded)" );
    }
}


// ********************************


DcmObject* DcmUnsignedLongOffset::getNextRecord()
{
    errorFlag = EC_Normal;
    return nextRecord;
}


// ********************************


DcmObject* DcmUnsignedLongOffset::setNextRecord( DcmObject* record )
{
    errorFlag = EC_Normal;
    nextRecord = record;
    return record;
}


// ********************************


E_Condition DcmUnsignedLongOffset::clear()
{
    errorFlag = EC_Normal;
    if ( UnsigLongValue != (T_VR_UL*)NULL )
	delete UnsigLongValue;
    UnsigLongValue = (T_VR_UL*)NULL;
    Length = 0;
    nextRecord = (DcmObject*)NULL;
    return errorFlag;
}


// ********************************


E_Condition DcmUnsignedLongOffset::verify( BOOL autocorrect )
{
    errorFlag = EC_Normal;
    if ( Length % (sizeof(T_VR_UL)) != 0 )
    {
	errorFlag = EC_CorruptedData;
	if ( autocorrect == TRUE )
	{

	    Length = Length - ( Length % (sizeof(T_VR_UL)) );
					    // auf gueltige Laenge kuerzen
            valueModified = TRUE;
	}
    }
    if (    errorFlag == EC_Normal
	 && Length != 0
	 && UnsigLongValue != (T_VR_UL*)NULL
	 && *UnsigLongValue != 0
	 && nextRecord == (DcmObject*)NULL
       )
	errorFlag = EC_CorruptedData;
    return errorFlag;
}


// ********************************


