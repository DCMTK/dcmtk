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
 * Last Update:   $Author: hewett $
 * Revision:	  $Revision: 1.5 $
 * Status:	  $State: Exp $
 *
 */

#include "osconfig.h"    /* make sure OS specific configuration is included first */


#include <stdlib.h>
#include <ctype.h>
#include "ofstring.h"
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


OFBool getSingleValue(DcmObject *obj,
		      DcmTagKey searchtag,
		      Uint16 &returnVal)
{
    OFBool l_error = OFFalse;
    DcmStack stack;

    if ( obj->search( searchtag, stack, ESM_fromHere, OFFalse )
	 == EC_Normal )
    {
	DcmObject *searchedObj = stack.top();
	if ( searchedObj->getLength() > 0 )
	{
	    if ( searchedObj->ident() == EVR_US )
                ((DcmUnsignedShort*)searchedObj)->getUint16(returnVal);
	    else if ( searchedObj->ident() == EVR_IS )
	    {
		Sint32 val = 0;
		((DcmIntegerString*)searchedObj)->getSint32(val);
		returnVal = (Uint16)val;
	    }
            else
                l_error = OFTrue;
	}
	else
	    l_error = OFTrue;
    }
    else
	l_error = OFTrue;

    return l_error;
}


// **********************************************


OFBool getSingleValue( DcmObject *obj,
		     DcmTagKey searchtag,
		     Sint16 &returnVal)
{
    OFBool l_error = OFFalse;
    DcmStack stack;

    if ( obj->search( searchtag, stack, ESM_fromHere, OFFalse )
	 == EC_Normal )
    {
	DcmObject *searchedObj = stack.top();
	if ( searchedObj->getLength() > 0 )
	{
	    if ( searchedObj->ident() == EVR_SS )
                ((DcmSignedShort*)searchedObj)->getSint16(returnVal);
	    else if ( searchedObj->ident() == EVR_IS )
	    {
		Sint32 val = 0;
		((DcmIntegerString *)searchedObj)->getSint32(val);
		returnVal = (Sint16)val;
	    }
            else
                l_error = OFTrue;
	}
	else
	    l_error = OFTrue;
    }
    else
	l_error = OFTrue;

    return l_error;
}


// **********************************************


OFBool getSingleValue( DcmObject *obj,
		     DcmTagKey searchtag,
		     Uint32 &returnVal)
{
    OFBool l_error = OFFalse;
    DcmStack stack;

    if ( obj->search( searchtag, stack, ESM_fromHere, OFFalse )
	 == EC_Normal )
    {
	DcmObject *searchedObj = stack.top();
	if ( searchedObj->getLength() > 0 )
	{
	    if ( searchedObj->ident() == EVR_UL )
                ((DcmUnsignedLong*)searchedObj)->getUint32(returnVal);
	    else if ( searchedObj->ident() == EVR_IS )
	    {
		Sint32 val = 0;
		((DcmIntegerString *)searchedObj)->getSint32(val);
		returnVal = (Uint32)val;
	    }
            else
                l_error = OFTrue;
	}
	else
	    l_error = OFTrue;
    }
    else
    {
	l_error = OFTrue;
    }
    return l_error;
}


// **********************************************


OFBool getSingleValue( DcmObject *obj,
		     DcmTagKey searchtag,
		     Sint32 &returnVal)
{
    OFBool l_error = OFFalse;
    DcmStack stack;

    if ( obj->search( searchtag, stack, ESM_fromHere, OFFalse )
	 == EC_Normal )
    {
	DcmObject *searchedObj = stack.top();
	if ( searchedObj->getLength() > 0 )
	{
	    if ( searchedObj->ident() == EVR_SL )
                ((DcmSignedLong*)searchedObj)->getSint32(returnVal);
	    else if ( searchedObj->ident() == EVR_IS )
		((DcmIntegerString *)searchedObj)->getSint32(returnVal);
            else
                l_error = OFTrue;
	}
	else
	    l_error = OFTrue;
    }
    else
    {
	l_error = OFTrue;
    }
    return l_error;
}

// **********************************************

OFBool getSingleValue( DcmObject *obj,
		     DcmTagKey searchtag,
		     char * & returnVal)
{
    OFBool l_error = OFFalse;
    DcmStack stack;

    if ( obj->search( searchtag, stack, ESM_fromHere, OFFalse )
	 == EC_Normal )
    {
	DcmObject *searchedObj = stack.top();
	searchedObj->verify(OFTrue);
	if (searchedObj->getLength() > 0)
	    l_error = 
		((DcmElement *)searchedObj)->getString(returnVal) != EC_Normal;
	else
	    l_error = OFTrue;
    }
    else
    {
	l_error = OFTrue;
    }
    return l_error;
}

// **********************************************


OFBool getSingleValue( DcmObject *obj,
		     DcmTagKey searchtag,
		     OFString & returnVal)
{
    OFBool l_error = OFFalse;
    DcmStack stack;

    if ( obj->search( searchtag, stack, ESM_fromHere, OFFalse )
	 == EC_Normal )
    {
	DcmObject *searchedObj = stack.top();
	if (searchedObj->getLength() > 0)
	    l_error = 
		((DcmElement *)searchedObj)->getOFStringArray(returnVal) != EC_Normal;
	else
	    l_error = OFTrue;
    }
    else
    {
	l_error = OFTrue;
    }
    return l_error;
}


// **********************************************


OFBool putSingleValue( DcmItem *item,
		     DcmTagKey tag,
		     Uint16 value)
{
    OFBool l_error = OFFalse;
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
	    l_error = OFTrue;
	}
	item->insert( elem, OFTrue );  // NULL-Elemente werden nicht eingefuegt
    }
    else
    {
	l_error = OFTrue;
    }
    return l_error;
}


// **********************************************


OFBool putSingleValue( DcmItem *item,
		     DcmTagKey tag,
		     Sint16 value)
{
    OFBool l_error = OFFalse;
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
	    l_error = OFTrue;
	}
	item->insert( elem, OFTrue );  // NULL-Elemente werden nicht eingefuegt
    }
    else
    {
	l_error = OFTrue;
    }
    return l_error;
}


// **********************************************


OFBool putSingleValue( DcmItem *item,
		     DcmTagKey tag,
		     Uint32 value)
{
    OFBool l_error = OFFalse;
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
	    l_error = OFTrue;
	}
	item->insert( elem, OFTrue );  // NULL-Elemente werden nicht eingefuegt
    }
    else
    {
	l_error = OFTrue;
    }
    return l_error;
}


// **********************************************


OFBool putSingleValue( DcmItem *item,
		     DcmTagKey tag,
		     Sint32 value)
{
    OFBool l_error = OFFalse;
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
	    l_error = OFTrue;
	}
	item->insert( elem, OFTrue );  // NULL-Elemente werden nicht eingefuegt
    }
    else
    {
	l_error = OFTrue;
    }
    return l_error;
}


// **********************************************


OFBool putSingleValue( DcmItem *item,
		     DcmTagKey tag,
		     OFString  & value)
{
    return putSingleValue(item, tag, value.c_str());
}

// **********************************************


OFBool putSingleValue( DcmItem *item,
		     DcmTagKey tag,
		     const char *value)
{
    OFBool l_error = OFFalse;
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
	else if ( localTag.getEVR() == EVR_UT )
	{
	    elem = new DcmUnlimitedText( localTag );
	    ((DcmUnlimitedText*)elem)->putString( value );
	}
	else if ( localTag.getEVR() == EVR_VS )
	{
	    elem = new DcmVirtualString( localTag );
	    ((DcmVirtualString*)elem)->putString( value );
	}
	else
	    l_error = OFTrue;
	item->insert( elem, OFTrue );  // NULL-Elemente werden nicht eingefuegt
    }
    else
	l_error = OFTrue;
    return l_error;
}


// **********************************************


OFBool deleteAttribute( DcmItem *item, DcmTagKey searchtag )
{
    OFBool l_error = OFFalse;
    DcmStack stack;

    if ( item->search( searchtag, stack, ESM_fromHere, OFFalse )
	 == EC_Normal )
    {
	DcmObject *searchedObj = stack.top();
	if ( item->remove( searchedObj ) != (DcmObject*)NULL )
	    delete searchedObj;
	else
	    l_error = OFTrue;
    }
    else
	l_error = OFTrue;
    return l_error;
}


// **********************************************


OFBool deleteAttribute( DcmItem *item, DcmObject *attribute )
{
    OFBool l_error = OFFalse;
    if ( item->remove( attribute ) != (DcmObject*)NULL )
	delete attribute;
    else
	l_error = OFTrue;
    return l_error;
}


// **********************************************


