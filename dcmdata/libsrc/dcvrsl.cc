/*
**
** Author: Gerd Ehlers      01.05.94 -- First Creation
**         Andreas Barth    07.12.95 -- new Stream class, unique value field
** Kuratorium OFFIS e.V.
**
** Module: dcvrsl.cc
**
** Purpose:
** Implementation of class DcmSignedLong
**
** Last Update:		$Author: andreas $
** Update Date:		$Date: 1996-01-05 13:27:53 $
** Source File:		$Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/libsrc/dcvrsl.cc,v $
** CVS/RCS Revision:	$Revision: 1.3 $
** Status:		$State: Exp $
**
** CVS/RCS Log at end of file
**
*/

#include <stdio.h>
#include <string.h>
#include <iostream.h>

#include "dcvrsl.h"
#include "dcdebug.h"


// ********************************


DcmSignedLong::DcmSignedLong(const DcmTag &tag, const Uint32 len)
: DcmElement(tag, len)
{
Bdebug((5, "dcvrsl:DcmSignedLong::DcmSignedLong(DcmTag&,len=%ld)",
           len ));
debug(( 8, "Object pointer this=0x%p", this ));

Edebug(());

}


// ********************************


DcmSignedLong::DcmSignedLong(const DcmSignedLong& old)
: DcmElement( old )
{
Bdebug((5, "dcvrsl:DcmSignedLong::DcmSignedLong(DcmSignedLong&)" ));
debug(( 8, "Object pointer this=0x%p", this ));

    if ( old.ident() != EVR_SL )
	{
        errorFlag = EC_IllegalCall;
        cerr << "Warning: DcmSignedLong: wrong use of Copy-Constructor"
             << endl;
    }
Edebug(());
}


// ********************************


DcmSignedLong::~DcmSignedLong(void)
{
Bdebug((5, "dcvrsl:DcmSignedLong::~DcmSignedLong()" ));
debug(( 8, "Object pointer this=0x%p", this ));

Edebug(());
}


// ********************************


void DcmSignedLong::print(const int level)
{
	if (valueLoaded())
	{
		Sint32 * sintVals = this -> get();

		if (!sintVals)
			printInfoLine( level, "(no value available)" );
		else
		{
			char *ch_words;
			char *tmp = ch_words = new char[Length*12/sizeof(Sint32)+4];

			for (unsigned long i=0; i<( Length/sizeof(Sint32) ); i++ )
			{
				sprintf( tmp, "%ld\\", *sintVals );
				tmp += strlen(tmp);
				sintVals++;
			}
			if ( Length > 0 )
				tmp--;
			*tmp = '\0';
			printInfoLine(level, ch_words);
			delete ch_words;
		}
    }
    else
		printInfoLine( level, "(not loaded)" );
}


// ********************************


unsigned long DcmSignedLong::getVM()
{
    return Length / sizeof(Sint32);
}


// ********************************


E_Condition DcmSignedLong::put(const Sint32 * sintVal,
							   const unsigned long numSints)
{
    errorFlag = EC_Normal;
	if (numSints)
	{
		if (sintVal)
			errorFlag = this -> putValue(sintVal, 
										 sizeof(Sint32)*Uint32(numSints));
		else
			errorFlag = EC_CorruptedData;
	}
	return errorFlag;
}


// ********************************


E_Condition DcmSignedLong::put(const Sint32 sintVal)
{
	Sint32 val = sintVal;
	errorFlag = this -> putValue(&val, sizeof(Sint32));
	return errorFlag;

    errorFlag = EC_Normal;
}


// ********************************


E_Condition DcmSignedLong::put(const Sint32 sintVal,
							   const unsigned long position)
{
	Bdebug((2, "DcmSignedLong::put(slong=%ld,num=%ld)", sintVal, position));

	Sint32 val = sintVal;

	errorFlag = this -> changeValue(&val, sizeof(Sint32)*position,
									sizeof(Sint32));
	return errorFlag;
}


// ********************************


E_Condition DcmSignedLong::get(Sint32 * & sintVals)
{
	sintVals = this -> get();
	return errorFlag;
}

// ********************************


E_Condition DcmSignedLong::get(Sint32 & sintVal, const unsigned long pos)
{
	sintVal = this -> get(pos);
	return errorFlag;
}


// ********************************


Sint32 * DcmSignedLong::get(void)
{
	Sint32 * sintVal = (Sint32 *)this -> getValue();

	if (errorFlag == EC_Normal)
		return sintVal;
	else
		return NULL;
}


// ********************************


Sint32 DcmSignedLong::get(const unsigned long position)
{
	Sint32 * sintVals = this -> get();

	if (sintVals && errorFlag == EC_Normal &&
		position < this -> getVM())
		return sintVals[position];
	else
		return Sint32(0);
}


// ********************************


E_Condition DcmSignedLong::verify(const BOOL autocorrect )
{
    errorFlag = EC_Normal;
    if ( Length % (sizeof(Sint32)) != 0 )
    {
		errorFlag = EC_CorruptedData;
		if (autocorrect)
		{
					    // auf gueltige Laenge kuerzen
			Length = Length - ( Length % (sizeof(Sint32)) );
		}
    }
    return errorFlag;
}


// ********************************

/*
** CVS/RCS Log:
** $Log: dcvrsl.cc,v $
** Revision 1.3  1996-01-05 13:27:53  andreas
** - changed to support new streaming facilities
** - unique read/write methods for file and block transfer
** - more cleanups
**
**
**
*/

