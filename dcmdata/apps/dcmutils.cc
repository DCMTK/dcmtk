/*
 *
 * Author: Gerd Ehlers		Created:  11-06-94
 *                              Modified: 02-15-95
 *
 * Module: dcmutils.cc
 *
 * Purpose:
 * Implementation of some routines for accessing DICOM datasets
 *
 *
 * Last Update:   $Author: andreas $
 * Revision:	  $Revision: 1.3 $
 * Status:	  $State: Exp $
 *
 */

#include "osconfig.h"    /* make sure OS specific configuration is included first */


#include <stdlib.h>
#include <ctype.h>
#include "dcmutils.h"
#include "dcvr.h"
#include "dcdebug.h"


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


BOOL getSingleValueUS( DcmObject *obj,
		       DcmTagKey searchtag,
		       Uint16 &returnVal)
{
Bdebug((3, "dcmutils:getSingleValueUS(DcmObject*=%p,searchtag=(%x,%x),returnVal)",
           obj, searchtag.getGroup(), searchtag.getElement() ));

//    returnVal = 0;
    BOOL l_error = FALSE;
    DcmStack stack;

    if ( obj->search( searchtag, stack, ESM_fromHere, FALSE )
	 == EC_Normal )
    {
	DcmObject *searchedObj = stack.top();
	if ( searchedObj->getLength() > 0 )
	{
	    if ( searchedObj->ident() == EVR_US )
                returnVal = ((DcmUnsignedShort*)searchedObj)->get((const unsigned long)0);
	    else if ( searchedObj->ident() == EVR_IS )
	    {
		const char *istr = ((DcmIntegerString*)searchedObj)->get();
		returnVal = (Uint16)atoi( istr );
	    }
            else
                l_error = TRUE;
debug(( 3, "returnVal=%hu found", returnVal ));

	}
	else
	    l_error = TRUE;
    }
    else
    {
debug(( 3, "no element with vr=\"US\" found" ));

	l_error = TRUE;
    }
Edebug(());

    return l_error;
}


// **********************************************


BOOL getSingleValueSS( DcmObject *obj,
		       DcmTagKey searchtag,
		       Sint16 &returnVal)
{
Bdebug((3, "dcmutils:getSingleValueSS(DcmObject*=%p,searchtag=(%x,%x),returnVal)",
           obj, searchtag.getGroup(), searchtag.getElement() ));

//    returnVal = 0;
    BOOL l_error = FALSE;
    DcmStack stack;

    if ( obj->search( searchtag, stack, ESM_fromHere, FALSE )
	 == EC_Normal )
    {
	DcmObject *searchedObj = stack.top();
	if ( searchedObj->getLength() > 0 )
	{
	    if ( searchedObj->ident() == EVR_SS )
                returnVal = ((DcmSignedShort*)searchedObj)->get((const unsigned long)0);
	    else if ( searchedObj->ident() == EVR_IS )
	    {
		const char *istr = ((DcmIntegerString*)searchedObj)->get();
		returnVal = (Sint16)atoi( istr );
	    }
            else
                l_error = TRUE;
debug(( 3, "returnVal=%hd found", returnVal ));

	}
	else
	    l_error = TRUE;
    }
    else
    {
debug(( 3, "no element with vr=\"SS\" found" ));

	l_error = TRUE;
    }
Edebug(());

    return l_error;
}


// **********************************************


BOOL getSingleValueUL( DcmObject *obj,
		       DcmTagKey searchtag,
		       Uint32 &returnVal)
{
Bdebug((3, "dcmutils:getSingleValueUL(DcmObject*=%p,searchtag=(%x,%x),returnVal)",
           obj, searchtag.getGroup(), searchtag.getElement() ));

//    returnVal = 0;
    BOOL l_error = FALSE;
    DcmStack stack;

    if ( obj->search( searchtag, stack, ESM_fromHere, FALSE )
	 == EC_Normal )
    {
	DcmObject *searchedObj = stack.top();
	if ( searchedObj->getLength() > 0 )
	{
	    if ( searchedObj->ident() == EVR_UL )
                returnVal = ((DcmUnsignedLong*)searchedObj)->get((const unsigned long)0);
	    else if ( searchedObj->ident() == EVR_IS )
	    {
		const char *istr = ((DcmIntegerString*)searchedObj)->get();
		returnVal = (Uint32)atol( istr );
	    }
            else
                l_error = TRUE;
debug(( 3, "returnVal=%lu found", returnVal ));

	}
	else
	    l_error = TRUE;
    }
    else
    {
debug(( 3, "no element with vr=\"UL\" found" ));

	l_error = TRUE;
    }
Edebug(());

    return l_error;
}


// **********************************************


BOOL getSingleValueSL( DcmObject *obj,
		       DcmTagKey searchtag,
		       Sint32 &returnVal)
{
Bdebug((3, "dcmutils:getSingleValueSL(DcmObject*=%p,searchtag=(%x,%x),returnVal)",
           obj, searchtag.getGroup(), searchtag.getElement() ));

//    returnVal = 0;
    BOOL l_error = FALSE;
    DcmStack stack;

    if ( obj->search( searchtag, stack, ESM_fromHere, FALSE )
	 == EC_Normal )
    {
	DcmObject *searchedObj = stack.top();
	if ( searchedObj->getLength() > 0 )
	{
	    if ( searchedObj->ident() == EVR_SL )
                returnVal = ((DcmSignedLong*)searchedObj)->get((const unsigned long)0);
	    else if ( searchedObj->ident() == EVR_IS )
	    {
		const char *istr = ((DcmIntegerString*)searchedObj)->get();
		returnVal = (Sint32)atol( istr );
	    }
            else
                l_error = TRUE;
debug(( 3, "returnVal=%ld found", returnVal ));

	}
	else
	    l_error = TRUE;
    }
    else
    {
debug(( 3, "no element with vr=\"SL\" found" ));

	l_error = TRUE;
    }
Edebug(());

    return l_error;
}


// **********************************************


BOOL getSingleValueByteString( DcmObject *obj,
			       DcmTagKey searchtag,
			       char * & returnVal)
{
Bdebug((3, "dcmutils:getSingleValueByteString(DcmObject*=%p,searchtag=(%x,%x),"
           "returnVal)", obj, searchtag.getGroup(), searchtag.getElement() ));

//    returnVal = "";
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
	    returnVal = ((DcmByteString*)searchedObj)->get();
	else if (   (	searchedObj->ident() == EVR_LO
		     || searchedObj->ident() == EVR_LT
		     || searchedObj->ident() == EVR_PN
		     || searchedObj->ident() == EVR_SH
		     || searchedObj->ident() == EVR_ST
		    )
		 && searchedObj->getLength() > 0
		)
		 returnVal = ((DcmCharString*)searchedObj)->get();
	else
	    l_error = TRUE;
debug(( 3, "returnVal=[%s]", returnVal ));

    }
    else
    {
debug(( 3, "no element with vr=\"Byte/CharString\" found" ));

	l_error = TRUE;
    }
Edebug(());

    return l_error;
}


// **********************************************


BOOL getSingleValueCharString( DcmObject *obj,
			       DcmTagKey searchtag,
			       const char *&returnVal)
{
Bdebug((3, "dcmutils:getSingleValueCharString(DcmObject*=%p,searchtag=(%x,%x),"
           "returnVal)", obj, searchtag.getGroup(), searchtag.getElement() ));

//    returnVal = "";
    BOOL l_error = FALSE;
    DcmStack stack;

    if ( obj->search( searchtag, stack, ESM_fromHere, FALSE )
	 == EC_Normal )
    {
	DcmObject *searchedObj = stack.top();
        searchedObj->verify( TRUE );                   // erzwinge dealigning
	if (   (   searchedObj->ident() == EVR_LO
		|| searchedObj->ident() == EVR_LT
		|| searchedObj->ident() == EVR_PN
		|| searchedObj->ident() == EVR_SH
		|| searchedObj->ident() == EVR_ST
	       )
	    && searchedObj->getLength() > 0
	   )
	    returnVal = ((DcmCharString*)searchedObj)->get();
	else
	    l_error = TRUE;
debug(( 3, "returnVal=[%s]", returnVal ));

    }
    else
    {
debug(( 3, "no element with vr=\"CharString\" found" ));

	l_error = TRUE;
    }
Edebug(());

    return l_error;
}


// **********************************************


BOOL putSingleValue( DcmItem *item,
		     DcmTagKey tag,
		     Uint16 value)
{
Bdebug((3, "dcmutils:putSingleValue(DcmItem*=%p,tag=(%x,%x),USvalue=%hu)",
	   item, tag.getGroup(), tag.getElement(), value ));

    BOOL l_error = FALSE;
    DcmTag localTag( tag );
    if ( item != (DcmItem*)NULL )
    {
	DcmElement *elem = (DcmElement*)NULL;
	if ( localTag.getEVR() == EVR_US )
	{
	    elem = new DcmUnsignedShort( localTag );
            ((DcmUnsignedShort*)elem)->put( value );
	}
	else if ( localTag.getEVR() == EVR_IS )
	{
	    char buffer[12];
	    int intValue = value;
            char *StringValue = itoa( intValue, buffer, 10 );
	    elem = new DcmIntegerString( localTag );
	    ((DcmIntegerString*)elem)->put( StringValue );
	}
	else
	{
debug(( 3, "tag is not EVR_US=%d or EVR_IS=%d", (int)EVR_US, (int)EVR_IS ));

	    l_error = TRUE;
	}
	item->insert( elem, TRUE );  // NULL-Elemente werden nicht eingefuegt
    }
    else
    {
debug(( 3, "no item available" ));

	l_error = TRUE;
    }
Edebug(());

    return l_error;
}


// **********************************************


BOOL putSingleValue( DcmItem *item,
		     DcmTagKey tag,
		     Sint16 value)
{
Bdebug((3, "dcmutils:putSingleValue(DcmItem*=%p,tag=(%x,%x),SSvalue=%hd)",
	   item, tag.getGroup(), tag.getElement(), value ));

    BOOL l_error = FALSE;
    DcmTag localTag( tag );
    if ( item != (DcmItem*)NULL )
    {
	DcmElement *elem = (DcmElement*)NULL;
	if ( localTag.getEVR() == EVR_SS )
	{
	    elem = new DcmSignedShort( localTag );
            ((DcmSignedShort*)elem)->put( value );
	}
	else if ( localTag.getEVR() == EVR_IS )
	{
	    char buffer[12];
	    int intValue = value;
	    char *StringValue = itoa( intValue, buffer, 10 );
	    elem = new DcmIntegerString( localTag );
	    ((DcmIntegerString*)elem)->put( StringValue );
	}
	else
	{
debug(( 3, "tag is not EVR_SS=%d or EVR_IS=%d", (int)EVR_SS, (int)EVR_IS ));

	    l_error = TRUE;
	}
	item->insert( elem, TRUE );  // NULL-Elemente werden nicht eingefuegt
    }
    else
    {
debug(( 3, "no item available" ));

	l_error = TRUE;
    }
Edebug(());

    return l_error;
}


// **********************************************


BOOL putSingleValue( DcmItem *item,
		     DcmTagKey tag,
		     Uint32 value)
{
Bdebug((3, "dcmutils:putSingleValue(DcmItem*=%p,tag=(%x,%x),ULvalue=%lu)",
	   item, tag.getGroup(), tag.getElement(), value ));

    BOOL l_error = FALSE;
    DcmTag localTag( tag );
    if ( item != (DcmItem*)NULL )
    {
	DcmElement *elem = (DcmElement*)NULL;
	if ( localTag.getEVR() == EVR_UL )
	{
	    elem = new DcmUnsignedLong( localTag );
            ((DcmUnsignedLong*)elem)->put( value );
	}
	else if ( localTag.getEVR() == EVR_IS )
	{
	    char buffer[12];
	    int intValue = (int)value;
	    char *StringValue = itoa( intValue, buffer, 10 );
	    elem = new DcmIntegerString( localTag );
	    ((DcmIntegerString*)elem)->put( StringValue );
	}
	else
	{
debug(( 3, "tag is not EVR_UL=%d or EVR_IS=%d", (int)EVR_UL, (int)EVR_IS ));

	    l_error = TRUE;
	}
	item->insert( elem, TRUE );  // NULL-Elemente werden nicht eingefuegt
    }
    else
    {
debug(( 3, "no item available" ));

	l_error = TRUE;
    }
Edebug(());

    return l_error;
}


// **********************************************


BOOL putSingleValue( DcmItem *item,
		     DcmTagKey tag,
		     Sint32 value)
{
Bdebug((3, "dcmutils:putSingleValue(DcmItem*=%p,tag=(%x,%x),SLvalue=%ld)",
	   item, tag.getGroup(), tag.getElement(), value ));

    BOOL l_error = FALSE;
    DcmTag localTag( tag );
    if ( item != (DcmItem*)NULL )
    {
	DcmElement *elem = (DcmElement*)NULL;
	if ( localTag.getEVR() == EVR_SL )
	{
	    elem = new DcmSignedLong( localTag );
            ((DcmSignedLong*)elem)->put( value );
	}
	else if ( localTag.getEVR() == EVR_IS )
	{
	    char buffer[12];
	    int intValue = (int)value;
	    char *StringValue = itoa( intValue, buffer, 10 );
	    elem = new DcmIntegerString( localTag );
	    ((DcmIntegerString*)elem)->put( StringValue );
	}
	else
	{
debug(( 3, "tag is not EVR_SL=%d or EVR_IS=%d", (int)EVR_SL, (int)EVR_IS ));

	    l_error = TRUE;
	}
	item->insert( elem, TRUE );  // NULL-Elemente werden nicht eingefuegt
    }
    else
    {
debug(( 3, "no item available" ));

	l_error = TRUE;
    }
Edebug(());

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
	    ((DcmApplicationEntity*)elem)->put( value );
	}
	else if ( localTag.getEVR() == EVR_AS )
	{
	    elem = new DcmAgeString( localTag );
	    ((DcmAgeString*)elem)->put( value );
	}
	else if ( localTag.getEVR() == EVR_CS )
	{
	    elem = new DcmCodeString( localTag );
	    ((DcmCodeString*)elem)->put( value );
	}
	else if ( localTag.getEVR() == EVR_DA )
	{
	    elem = new DcmDate( localTag );
	    ((DcmDate*)elem)->put( value );
	}
	else if ( localTag.getEVR() == EVR_DS )
	{
	    elem = new DcmDecimalString( localTag );
	    ((DcmDecimalString*)elem)->put( value );
	}
	else if ( localTag.getEVR() == EVR_DT )
	{
	    elem = new DcmDateTime( localTag );
	    ((DcmDateTime*)elem)->put( value );
	}
	else if ( localTag.getEVR() == EVR_IS )
	{
	    elem = new DcmIntegerString( localTag );
	    ((DcmIntegerString*)elem)->put( value );
	}
	else if ( localTag.getEVR() == EVR_TM )
	{
	    elem = new DcmTime( localTag );
	    ((DcmTime*)elem)->put( value );
	}
	else if ( localTag.getEVR() == EVR_UI )
	{
	    elem = new DcmUniqueIdentifier( localTag );
	    ((DcmUniqueIdentifier*)elem)->put( value );
	}
	else if ( localTag.getEVR() == EVR_LO )	      // DcmCharString()
	{
	    elem = new DcmLongString( localTag );
	    ((DcmLongString*)elem)->put( value );
	}
	else if ( localTag.getEVR() == EVR_LT )
	{
	    elem = new DcmLongText( localTag );
	    ((DcmLongText*)elem)->put( value );
	}
	else if ( localTag.getEVR() == EVR_PN )
	{
	    elem = new DcmPersonName( localTag );
	    ((DcmPersonName*)elem)->put( value );
	}
	else if ( localTag.getEVR() == EVR_SH )
	{
	    elem = new DcmShortString( localTag );
	    ((DcmShortString*)elem)->put( value );
	}
	else if ( localTag.getEVR() == EVR_ST )
	{
	    elem = new DcmShortText( localTag );
	    ((DcmShortText*)elem)->put( value );
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


