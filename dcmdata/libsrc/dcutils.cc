/*
 *
 *  Copyright (C) 1994-2002, OFFIS
 *
 *  This software and supporting documentation were developed by
 *
 *    Kuratorium OFFIS e.V.
 *    Healthcare Information and Communication Systems
 *    Escherweg 2
 *    D-26121 Oldenburg, Germany
 *
 *  THIS SOFTWARE IS MADE AVAILABLE,  AS IS,  AND OFFIS MAKES NO  WARRANTY
 *  REGARDING  THE  SOFTWARE,  ITS  PERFORMANCE,  ITS  MERCHANTABILITY  OR
 *  FITNESS FOR ANY PARTICULAR USE, FREEDOM FROM ANY COMPUTER DISEASES  OR
 *  ITS CONFORMITY TO ANY SPECIFICATION. THE ENTIRE RISK AS TO QUALITY AND
 *  PERFORMANCE OF THE SOFTWARE IS WITH THE USER.
 *
 *  Module:  dcmdata
 *
 *  Author:  Gerd Ehlers
 *
 *  Purpose: Helper functions for accessing DICOM datasets
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 2002-11-27 15:12:15 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/libsrc/Attic/dcutils.cc,v $
 *  CVS/RCS Revision: $Revision: 1.14 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcutils.h"
#include "ofstring.h"
#include "dcitem.h"
#include "dcvrae.h"
#include "dcvras.h"
#include "dcvrcs.h"
#include "dcvrda.h"
#include "dcvrds.h"
#include "dcvrdt.h"
#include "dcvris.h"
#include "dcvrtm.h"
#include "dcvrui.h"
#include "dcvrlo.h"
#include "dcvrlt.h"
#include "dcvrpn.h"
#include "dcvrsh.h"
#include "dcvrst.h"
#include "dcvrut.h"
#include "dcvrss.h"
#include "dcvrus.h"
#include "dcvrsl.h"
#include "dcvrul.h"

#define INCLUDE_CSTDLIB
#define INCLUDE_CSTDIO
#define INCLUDE_CCTYPE
#include "ofstdinc.h"

// **********************************************


#ifndef HAVE_ITOA

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


/*
 * CVS/RCS Log:
 * $Log: dcutils.cc,v $
 * Revision 1.14  2002-11-27 15:12:15  meichel
 * Adapted module dcmdata to use of new header file ofstdinc.h
 *
 * Revision 1.13  2002/11/27 12:06:54  meichel
 * Adapted module dcmdata to use of new header file ofstdinc.h
 *
 * Revision 1.12  2002/08/27 16:55:58  meichel
 * Initial release of new DICOM I/O stream classes that add support for stream
 *   compression (deflated little endian explicit VR transfer syntax)
 *
 * Revision 1.11  2001/06/01 15:49:12  meichel
 * Updated copyright header
 *
 * Revision 1.10  2000/03/08 16:26:43  meichel
 * Updated copyright header.
 *
 * Revision 1.9  2000/02/29 11:49:30  meichel
 * Removed support for VS value representation. This was proposed in CP 101
 *   but never became part of the standard.
 *
 * Revision 1.8  2000/02/23 15:12:05  meichel
 * Corrected macro for Borland C++ Builder 4 workaround.
 *
 * Revision 1.7  2000/02/01 10:12:11  meichel
 * Avoiding to include <stdlib.h> as extern "C" on Borland C++ Builder 4,
 *   workaround for bug in compiler header files.
 *
 * Revision 1.6  1999/03/31 09:25:44  meichel
 * Updated copyright header in module dcmdata
 *
 *
 */
