/*
 *
 * Author: Gerd Ehlers		Created:  11-06-94
 *                              Modified: 02-15-95
 *
 * Module: dcutils.cc
 *
 * Purpose:
 * Implementation of some routines for accessing DICOM datasets
 *
 *
 * Last Update:   $Author: andreas $
 * Revision:	  $Revision: 1.2 $
 * Status:	  $State: Exp $
 *
 */

#include "osconfig.h"    /* make sure OS specific configuration is included first */


#include <stdlib.h>
#include <ctype.h>
#include "dcutils.h"
#include "dctk.h"


// **********************************************


#ifndef HAVE_ITOA

#include <stdio.h>

char *itoa(int l, char *str, int /*width*/ )
{
    sprintf(str, "%d", l);
    return str;
}

#endif


// **********************************************


char* skipWS( char *str )
{
    while (    *str != '\0'
	    && isspace( *str )
	  )
	str++;
    return str;
}


// **********************************************


BOOL getSingleValue( DcmObject *obj,
		     DcmTagKey searchtag,
		     Uint16 &returnVal)
{
    BOOL l_error = FALSE;
    DcmStack stack;

    if ( obj->search( searchtag, stack, ESM_fromHere, FALSE )
	 == EC_Normal )
    {
	DcmObject *searchedObj = stack.top();
	if ( searchedObj->getLength() > 0 )
	{
	    if ( searchedObj->ident() == EVR_US )
                ((DcmUnsignedShort*)searchedObj)->getUint16(returnVal);
	    else if ( searchedObj->ident() == EVR_IS )
	    {
		char *istr = NULL;
		((DcmIntegerString*)searchedObj)->getString(istr);
		returnVal = (Uint16)atoi( istr );
	    }
            else
                l_error = TRUE;
	}
	else
	    l_error = TRUE;
    }
    else
	l_error = TRUE;

    return l_error;
}


// **********************************************


BOOL getSingleValue( DcmObject *obj,
		     DcmTagKey searchtag,
		     Sint16 &returnVal)
{
    BOOL l_error = FALSE;
    DcmStack stack;

    if ( obj->search( searchtag, stack, ESM_fromHere, FALSE )
	 == EC_Normal )
    {
	DcmObject *searchedObj = stack.top();
	if ( searchedObj->getLength() > 0 )
	{
	    if ( searchedObj->ident() == EVR_SS )
                ((DcmSignedShort*)searchedObj)->getSint16(returnVal);
	    else if ( searchedObj->ident() == EVR_IS )
	    {
	      char *istr = NULL;
	      ((DcmIntegerString*)searchedObj)->getString(istr);
	      returnVal = (Sint16)atoi( istr );
	    }
            else
                l_error = TRUE;
	}
	else
	    l_error = TRUE;
    }
    else
	l_error = TRUE;

    return l_error;
}


// **********************************************


BOOL getSingleValue( DcmObject *obj,
		     DcmTagKey searchtag,
		     Uint32 &returnVal)
{
    BOOL l_error = FALSE;
    DcmStack stack;

    if ( obj->search( searchtag, stack, ESM_fromHere, FALSE )
	 == EC_Normal )
    {
	DcmObject *searchedObj = stack.top();
	if ( searchedObj->getLength() > 0 )
	{
	    if ( searchedObj->ident() == EVR_UL )
                ((DcmUnsignedLong*)searchedObj)->getUint32(returnVal);
	    else if ( searchedObj->ident() == EVR_IS )
	    {
		char *istr = NULL;
		((DcmIntegerString*)searchedObj)->getString(istr);
		returnVal = (Uint32)atol( istr );
	    }
            else
                l_error = TRUE;
	}
	else
	    l_error = TRUE;
    }
    else
    {
	l_error = TRUE;
    }
    return l_error;
}


// **********************************************


BOOL getSingleValue( DcmObject *obj,
		     DcmTagKey searchtag,
		     Sint32 &returnVal)
{
    BOOL l_error = FALSE;
    DcmStack stack;

    if ( obj->search( searchtag, stack, ESM_fromHere, FALSE )
	 == EC_Normal )
    {
	DcmObject *searchedObj = stack.top();
	if ( searchedObj->getLength() > 0 )
	{
	    if ( searchedObj->ident() == EVR_SL )
                ((DcmSignedLong*)searchedObj)->getSint32(returnVal);
	    else if ( searchedObj->ident() == EVR_IS )
	    {
		char *istr = NULL;
		((DcmIntegerString*)searchedObj)->getString(istr);
		returnVal = (Sint32)atol( istr );
	    }
            else
                l_error = TRUE;
	}
	else
	    l_error = TRUE;
    }
    else
    {
	l_error = TRUE;
    }
    return l_error;
}


// **********************************************


BOOL getSingleValue( DcmObject *obj,
		     DcmTagKey searchtag,
		     char * & returnVal)
{
    BOOL l_error = FALSE;
    DcmStack stack;

    if ( obj->search( searchtag, stack, ESM_fromHere, FALSE )
	 == EC_Normal )
    {
	DcmObject *searchedObj = stack.top();
        searchedObj->verify( TRUE );                   // erzwinge dealigning
	if (   (   searchedObj->ident() == EVR_AE
		|| searchedObj->ident() == EVR_AS
		|| searchedObj->ident() == EVR_CS
		|| searchedObj->ident() == EVR_DA
		|| searchedObj->ident() == EVR_DS
		|| searchedObj->ident() == EVR_DT
		|| searchedObj->ident() == EVR_IS
		|| searchedObj->ident() == EVR_TM
		|| searchedObj->ident() == EVR_UI
	       )
	    && searchedObj->getLength() > 0
	   )
	    ((DcmByteString*)searchedObj)->getString(returnVal);
	else if (   (	searchedObj->ident() == EVR_LO
		     || searchedObj->ident() == EVR_LT
		     || searchedObj->ident() == EVR_PN
		     || searchedObj->ident() == EVR_SH
		     || searchedObj->ident() == EVR_ST
		    )
		 && searchedObj->getLength() > 0
		)
		 ((DcmCharString*)searchedObj)->getString(returnVal);
	else
	    l_error = TRUE;
    }
    else
    {
	l_error = TRUE;
    }
    return l_error;
}


// **********************************************


BOOL putSingleValue( DcmItem *item,
		     DcmTagKey tag,
		     Uint16 value)
{
    BOOL l_error = FALSE;
    DcmTag localTag( tag );
    if ( item != (DcmItem*)NULL )
    {
	DcmElement *elem = (DcmElement*)NULL;
	if ( localTag.getEVR() == EVR_US )
	{
	    elem = new DcmUnsignedShort( localTag );
            ((DcmUnsignedShort*)elem)->putUint16( value );
	}
	else if ( localTag.getEVR() == EVR_IS )
	{
	    char buffer[12];
	    int intValue = value;
            char *StringValue = itoa( intValue, buffer, 10 );
	    elem = new DcmIntegerString( localTag );
	    ((DcmIntegerString*)elem)->putString( StringValue );
	}
	else
	{
	    l_error = TRUE;
	}
	item->insert( elem, TRUE );  // NULL-Elemente werden nicht eingefuegt
    }
    else
    {
	l_error = TRUE;
    }
    return l_error;
}


// **********************************************


BOOL putSingleValue( DcmItem *item,
		     DcmTagKey tag,
		     Sint16 value)
{
    BOOL l_error = FALSE;
    DcmTag localTag( tag );
    if ( item != (DcmItem*)NULL )
    {
	DcmElement *elem = (DcmElement*)NULL;
	if ( localTag.getEVR() == EVR_SS )
	{
	    elem = new DcmSignedShort( localTag );
            ((DcmSignedShort*)elem)->putSint16( value );
	}
	else if ( localTag.getEVR() == EVR_IS )
	{
	    char buffer[12];
	    int intValue = value;
	    char *StringValue = itoa( intValue, buffer, 10 );
	    elem = new DcmIntegerString( localTag );
	    ((DcmIntegerString*)elem)->putString( StringValue );
	}
	else
	{
	    l_error = TRUE;
	}
	item->insert( elem, TRUE );  // NULL-Elemente werden nicht eingefuegt
    }
    else
    {
	l_error = TRUE;
    }
    return l_error;
}


// **********************************************


BOOL putSingleValue( DcmItem *item,
		     DcmTagKey tag,
		     Uint32 value)
{
    BOOL l_error = FALSE;
    DcmTag localTag( tag );
    if ( item != (DcmItem*)NULL )
    {
	DcmElement *elem = (DcmElement*)NULL;
	if ( localTag.getEVR() == EVR_UL )
	{
	    elem = new DcmUnsignedLong( localTag );
            ((DcmUnsignedLong*)elem)->putUint32( value );
	}
	else if ( localTag.getEVR() == EVR_IS )
	{
	    char buffer[12];
	    int intValue = (int)value;
	    char *StringValue = itoa( intValue, buffer, 10 );
	    elem = new DcmIntegerString( localTag );
	    ((DcmIntegerString*)elem)->putString( StringValue );
	}
	else
	{
	    l_error = TRUE;
	}
	item->insert( elem, TRUE );  // NULL-Elemente werden nicht eingefuegt
    }
    else
    {
	l_error = TRUE;
    }
    return l_error;
}


// **********************************************


BOOL putSingleValue( DcmItem *item,
		     DcmTagKey tag,
		     Sint32 value)
{
    BOOL l_error = FALSE;
    DcmTag localTag( tag );
    if ( item != (DcmItem*)NULL )
    {
	DcmElement *elem = (DcmElement*)NULL;
	if ( localTag.getEVR() == EVR_SL )
	{
	    elem = new DcmSignedLong( localTag );
            ((DcmSignedLong*)elem)->putSint32( value );
	}
	else if ( localTag.getEVR() == EVR_IS )
	{
	    char buffer[12];
	    int intValue = (int)value;
	    char *StringValue = itoa( intValue, buffer, 10 );
	    elem = new DcmIntegerString( localTag );
	    ((DcmIntegerString*)elem)->putString( StringValue );
	}
	else
	{
	    l_error = TRUE;
	}
	item->insert( elem, TRUE );  // NULL-Elemente werden nicht eingefuegt
    }
    else
    {
	l_error = TRUE;
    }
    return l_error;
}


// **********************************************


BOOL putSingleValue( DcmItem *item,
		     DcmTagKey tag,
		     const char *value)
{
    BOOL l_error = FALSE;
    DcmTag localTag( tag );
    if ( item != (DcmItem*)NULL )
    {
	DcmElement *elem = (DcmElement*)NULL;
	if ( localTag.getEVR() == EVR_AE )	      // DcmByteString()
	{
	    elem = new DcmApplicationEntity( localTag );
	    ((DcmApplicationEntity*)elem)->putString( value );
	}
	else if ( localTag.getEVR() == EVR_AS )
	{
	    elem = new DcmAgeString( localTag );
	    ((DcmAgeString*)elem)->putString( value );
	}
	else if ( localTag.getEVR() == EVR_CS )
	{
	    elem = new DcmCodeString( localTag );
	    ((DcmCodeString*)elem)->putString( value );
	}
	else if ( localTag.getEVR() == EVR_DA )
	{
	    elem = new DcmDate( localTag );
	    ((DcmDate*)elem)->putString( value );
	}
	else if ( localTag.getEVR() == EVR_DS )
	{
	    elem = new DcmDecimalString( localTag );
	    ((DcmDecimalString*)elem)->putString( value );
	}
	else if ( localTag.getEVR() == EVR_DT )
	{
	    elem = new DcmDateTime( localTag );
	    ((DcmDateTime*)elem)->putString( value );
	}
	else if ( localTag.getEVR() == EVR_IS )
	{
	    elem = new DcmIntegerString( localTag );
	    ((DcmIntegerString*)elem)->putString( value );
	}
	else if ( localTag.getEVR() == EVR_TM )
	{
	    elem = new DcmTime( localTag );
	    ((DcmTime*)elem)->putString( value );
	}
	else if ( localTag.getEVR() == EVR_UI )
	{
	    elem = new DcmUniqueIdentifier( localTag );
	    ((DcmUniqueIdentifier*)elem)->putString( value );
	}
	else if ( localTag.getEVR() == EVR_LO )	      // DcmCharString()
	{
	    elem = new DcmLongString( localTag );
	    ((DcmLongString*)elem)->putString( value );
	}
	else if ( localTag.getEVR() == EVR_LT )
	{
	    elem = new DcmLongText( localTag );
	    ((DcmLongText*)elem)->putString( value );
	}
	else if ( localTag.getEVR() == EVR_PN )
	{
	    elem = new DcmPersonName( localTag );
	    ((DcmPersonName*)elem)->putString( value );
	}
	else if ( localTag.getEVR() == EVR_SH )
	{
	    elem = new DcmShortString( localTag );
	    ((DcmShortString*)elem)->putString( value );
	}
	else if ( localTag.getEVR() == EVR_ST )
	{
	    elem = new DcmShortText( localTag );
	    ((DcmShortText*)elem)->putString( value );
	}
	else
	    l_error = TRUE;
	item->insert( elem, TRUE );  // NULL-Elemente werden nicht eingefuegt
    }
    else
	l_error = TRUE;
    return l_error;
}


// **********************************************


BOOL deleteAttribute( DcmItem *item, DcmTagKey searchtag )
{
    BOOL l_error = FALSE;
    DcmStack stack;

    if ( item->search( searchtag, stack, ESM_fromHere, FALSE )
	 == EC_Normal )
    {
	DcmObject *searchedObj = stack.top();
	if ( item->remove( searchedObj ) != (DcmObject*)NULL )
	    delete searchedObj;
	else
	    l_error = TRUE;
    }
    else
	l_error = TRUE;
    return l_error;
}


// **********************************************


BOOL deleteAttribute( DcmItem *item, DcmObject *attribute )
{
    BOOL l_error = FALSE;
    if ( item->remove( attribute ) != (DcmObject*)NULL )
	delete attribute;
    else
	l_error = TRUE;
    return l_error;
}


// **********************************************


