/*
 * 
 * Author: Gerd Ehlers	    Created:  05-01-94
 *                          Modified: 02-07-95
 *
 * Module: dcvrui.cc
 * 
 * Purpose:
 * Implementation of class DcmUniqueIdentifier
 * 
 * 
 * Last Update:	  $Author: hewett $
 * Revision:      $Revision: 1.1 $
 * Status:        $State: Exp $
 *
 */


#include <string.h>
#include "dcvrui.h"
#include "dcmodtbl.h"
#include "dcdebug.h"



// ********************************


DcmUniqueIdentifier::DcmUniqueIdentifier( DcmTag &tag )
    : DcmByteString( tag )
{
Bdebug((5, "dcvrui:DcmUniqueIdentifier::DcmUniqueIdentifier(DcmTag&)" ));

    paddingChar = '\0';
    maxLength = 64;
Edebug(());

}


// ********************************


DcmUniqueIdentifier::DcmUniqueIdentifier( DcmTag &tag,
                                          T_VR_UL len,
                                          iDicomStream *iDStream )
    : DcmByteString( tag, len, iDStream )
{
Bdebug((5, "dcvrui:DcmUniqueIdentifier::DcmUniqueIdentifier"
           "(DcmTag&,len=%ld,*iDS)", len ));

    paddingChar = '\0';
    maxLength = 64;
Edebug(());

}


// ********************************


DcmUniqueIdentifier::DcmUniqueIdentifier( const DcmObject &oldObj )
    : DcmByteString( oldObj, EVR_UI )
{
Bdebug((5, "dcvrui:DcmUniqueIdentifier::DcmUniqueIdentifier(DcmObject&)" ));

    paddingChar = '\0';
    maxLength = 64;
Edebug(());

}


// ********************************


DcmUniqueIdentifier::DcmUniqueIdentifier( const DcmUniqueIdentifier &newUI )
    : DcmByteString( newUI, EVR_UI )
{
Bdebug((5, "dcvrui:DcmUniqueIdentifier::DcmUniqueIdentifier"
           "(DcmUniqueIdentifier&)" ));

    paddingChar = '\0';
    maxLength = 64;
Edebug(());

}


// ********************************


DcmUniqueIdentifier::~DcmUniqueIdentifier()
{
Bdebug((5, "dcvrui:DcmUniqueIdentifier::~DcmUniqueIdentifier()" ));
Edebug(());

}


// ********************************


EVR DcmUniqueIdentifier::ident() const
{
    return EVR_UI;
}


// ********************************


void DcmUniqueIdentifier::print( int level )
{
    if ( ByteStringValue != (char*)NULL )
    {
        DcmModuleTable nametable( ByteStringValue );
        char* symbol = nametable.getNameTableSymbol();
        char *tmp = (char*)NULL;

        if ( symbol != (char*)NULL && *symbol != '\0' )
        {
            tmp = new char[ strlen(symbol) + 3 ];
            tmp[0] = '=';
            strcpy( tmp+1, symbol );
        }
        else
        {
            tmp = new char[ realLength + 4 ];
            tmp[0] = '[';
            strncpy( tmp+1, ByteStringValue, (int)realLength );
            tmp[ realLength + 1 ] = '\0';
            T_VR_UL t_len = strlen( tmp+1 );
            tmp[ t_len + 1 ] = ']';
            tmp[ t_len + 2 ] = '\0';
        }
	DcmObject::printInfoLine( level, tmp );
	delete tmp;
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


