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
 * Last Update:   $Author: hewett $
 * Revision:	  $Revision: 1.1 $
 * Status:	  $State: Exp $
 *
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif


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
		       ETag searchtag,
		       T_VR_US &returnVal)
{
Bdebug((3, "dcmutils:getSingleValueUS(DcmObject*=%p,searchtag=%d,returnVal)",
           obj, (int)searchtag ));

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
                returnVal = ((DcmUnsignedShort*)searchedObj)->get( 0 );
	    else if ( searchedObj->ident() == EVR_IS )
	    {
		char *istr = ((DcmIntegerString*)searchedObj)->get();
		returnVal = (T_VR_US)atoi( istr );
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
		       ETag searchtag,
		       T_VR_SS &returnVal)
{
Bdebug((3, "dcmutils:getSingleValueSS(DcmObject*=%p,searchtag=%d,returnVal)",
           obj, (int)searchtag ));

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
                returnVal = ((DcmSignedShort*)searchedObj)->get( 0 );
	    else if ( searchedObj->ident() == EVR_IS )
	    {
		char *istr = ((DcmIntegerString*)searchedObj)->get();
		returnVal = (T_VR_SS)atoi( istr );
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
		       ETag searchtag,
		       T_VR_UL &returnVal)
{
Bdebug((3, "dcmutils:getSingleValueUL(DcmObject*=%p,searchtag=%d,returnVal)",
           obj, (int)searchtag ));

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
                returnVal = ((DcmUnsignedLong*)searchedObj)->get( 0 );
	    else if ( searchedObj->ident() == EVR_IS )
	    {
		char *istr = ((DcmIntegerString*)searchedObj)->get();
		returnVal = (T_VR_UL)atol( istr );
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
		       ETag searchtag,
		       T_VR_SL &returnVal)
{
Bdebug((3, "dcmutils:getSingleValueSL(DcmObject*=%p,searchtag=%d,returnVal)",
           obj, (int)searchtag ));

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
                returnVal = ((DcmSignedLong*)searchedObj)->get( 0 );
	    else if ( searchedObj->ident() == EVR_IS )
	    {
		char *istr = ((DcmIntegerString*)searchedObj)->get();
		returnVal = (T_VR_SL)atol( istr );
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
			       ETag searchtag,
			       char * &returnVal)
{
Bdebug((3, "dcmutils:getSingleValueByteString(DcmObject*=%p,searchtag=%d,"
           "returnVal)", obj, (int)searchtag ));

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
			       ETag searchtag,
			       ALT_CHAR *&returnVal)
{
Bdebug((3, "dcmutils:getSingleValueCharString(DcmObject*=%p,searchtag=%d,"
           "returnVal)", obj, (int)searchtag ));

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
		     ETag tag,
		     T_VR_US value)
{
Bdebug((3, "dcmutils:putSingleValue(DcmItem*=%p,tag=%d,USvalue=%hu)",
	   item, (int)tag, value ));

    BOOL l_error = FALSE;
    DcmTag localTag( tag );
    if ( item != (DcmItem*)NULL )
    {
	DcmElement *elem = (DcmElement*)NULL;
	if ( localTag.getVR() == EVR_US )
	{
	    elem = new DcmUnsignedShort( localTag );
            ((DcmUnsignedShort*)elem)->put( value );
	}
	else if ( localTag.getVR() == EVR_IS )
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
		     ETag tag,
		     T_VR_SS value)
{
Bdebug((3, "dcmutils:putSingleValue(DcmItem*=%p,tag=%d,SSvalue=%hd)",
	   item, (int)tag, value ));

    BOOL l_error = FALSE;
    DcmTag localTag( tag );
    if ( item != (DcmItem*)NULL )
    {
	DcmElement *elem = (DcmElement*)NULL;
	if ( localTag.getVR() == EVR_SS )
	{
	    elem = new DcmSignedShort( localTag );
            ((DcmSignedShort*)elem)->put( value );
	}
	else if ( localTag.getVR() == EVR_IS )
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
		     ETag tag,
		     T_VR_UL value)
{
Bdebug((3, "dcmutils:putSingleValue(DcmItem*=%p,tag=%d,ULvalue=%lu)",
	   item, (int)tag, value ));

    BOOL l_error = FALSE;
    DcmTag localTag( tag );
    if ( item != (DcmItem*)NULL )
    {
	DcmElement *elem = (DcmElement*)NULL;
	if ( localTag.getVR() == EVR_UL )
	{
	    elem = new DcmUnsignedLong( localTag );
            ((DcmUnsignedLong*)elem)->put( value );
	}
	else if ( localTag.getVR() == EVR_IS )
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
		     ETag tag,
		     T_VR_SL value)
{
Bdebug((3, "dcmutils:putSingleValue(DcmItem*=%p,tag=%d,SLvalue=%ld)",
	   item, (int)tag, value ));

    BOOL l_error = FALSE;
    DcmTag localTag( tag );
    if ( item != (DcmItem*)NULL )
    {
	DcmElement *elem = (DcmElement*)NULL;
	if ( localTag.getVR() == EVR_SL )
	{
	    elem = new DcmSignedLong( localTag );
            ((DcmSignedLong*)elem)->put( value );
	}
	else if ( localTag.getVR() == EVR_IS )
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
		     ETag tag,
		     char *value)
{
    BOOL l_error = FALSE;
    DcmTag localTag( tag );
    if ( item != (DcmItem*)NULL )
    {
	DcmElement *elem = (DcmElement*)NULL;
	if ( localTag.getVR() == EVR_AE )	      // DcmByteString()
	{
	    elem = new DcmApplicationEntity( localTag );
	    ((DcmApplicationEntity*)elem)->put( value );
	}
	else if ( localTag.getVR() == EVR_AS )
	{
	    elem = new DcmAgeString( localTag );
	    ((DcmAgeString*)elem)->put( value );
	}
	else if ( localTag.getVR() == EVR_CS )
	{
	    elem = new DcmCodeString( localTag );
	    ((DcmCodeString*)elem)->put( value );
	}
	else if ( localTag.getVR() == EVR_DA )
	{
	    elem = new DcmDate( localTag );
	    ((DcmDate*)elem)->put( value );
	}
	else if ( localTag.getVR() == EVR_DS )
	{
	    elem = new DcmDecimalString( localTag );
	    ((DcmDecimalString*)elem)->put( value );
	}
	else if ( localTag.getVR() == EVR_DT )
	{
	    elem = new DcmDateTime( localTag );
	    ((DcmDateTime*)elem)->put( value );
	}
	else if ( localTag.getVR() == EVR_IS )
	{
	    elem = new DcmIntegerString( localTag );
	    ((DcmIntegerString*)elem)->put( value );
	}
	else if ( localTag.getVR() == EVR_TM )
	{
	    elem = new DcmTime( localTag );
	    ((DcmTime*)elem)->put( value );
	}
	else if ( localTag.getVR() == EVR_UI )
	{
	    elem = new DcmUniqueIdentifier( localTag );
	    ((DcmUniqueIdentifier*)elem)->put( value );
	}
	else if ( localTag.getVR() == EVR_LO )	      // DcmCharString()
	{
	    elem = new DcmLongString( localTag );
	    ((DcmLongString*)elem)->put( value );
	}
	else if ( localTag.getVR() == EVR_LT )
	{
	    elem = new DcmLongText( localTag );
	    ((DcmLongText*)elem)->put( value );
	}
	else if ( localTag.getVR() == EVR_PN )
	{
	    elem = new DcmPersonName( localTag );
	    ((DcmPersonName*)elem)->put( value );
	}
	else if ( localTag.getVR() == EVR_SH )
	{
	    elem = new DcmShortString( localTag );
	    ((DcmShortString*)elem)->put( value );
	}
	else if ( localTag.getVR() == EVR_ST )
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


BOOL deleteAttribute( DcmItem *item, ETag searchtag )
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


