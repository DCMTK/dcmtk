/*
**
** Author: Gerd Ehlers      01.05.94 -- First Creation
**         Andreas Barth    05.12.95 -- new Stream class, unique value field
** Kuratorium OFFIS e.V.
**
** Module: dcvrul.cc
**
** Purpose:
** Implementation of class DcmUnsignedLong
**
** Last Update:		$Author: andreas $
** Update Date:		$Date: 1996-01-05 13:27:55 $
** Source File:		$Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/libsrc/dcvrul.cc,v $
** CVS/RCS Revision:	$Revision: 1.3 $
** Status:		$State: Exp $
**
** CVS/RCS Log at end of file
**
*/

#include <stdio.h>
#include <string.h>
#include <iostream.h>

#include "dcvrul.h"
#include "dcdebug.h"


// ********************************


DcmUnsignedLong::DcmUnsignedLong(const DcmTag &tag, const Uint32 len)
: DcmElement(tag, len)
{
Bdebug((5, "dcvrsl:DcmUnsignedLong::DcmUnsignedLong(DcmTag&,len=%ld)",
           len ));
debug(( 8, "Object pointer this=0x%p", this ));

Edebug(());

}


// ********************************


DcmUnsignedLong::DcmUnsignedLong(const DcmUnsignedLong& old)
: DcmElement( old )
{
Bdebug((5, "dcvrsl:DcmUnsignedLong::DcmUnsignedLong(DcmUnsignedLong&)" ));
debug(( 8, "Object pointer this=0x%p", this ));

    if ( old.ident() != EVR_UL )
	{
        errorFlag = EC_IllegalCall;
        cerr << "Warning: DcmUnsignedLong: wrong use of Copy-Constructor"
             << endl;
    }
Edebug(());
}


// ********************************


DcmUnsignedLong::~DcmUnsignedLong(void)
{
Bdebug((5, "dcvrsl:DcmUnsignedLong::~DcmUnsignedLong()" ));
debug(( 8, "Object pointer this=0x%p", this ));

Edebug(());
}


// ********************************


void DcmUnsignedLong::print(const int level)
{
	if (valueLoaded())
	{
		Uint32 * uintVals = this -> get();

		if (!uintVals)
			printInfoLine( level, "(no value available)" );
		else
		{
			char *ch_words;
			char *tmp = ch_words = new char[Length*12/sizeof(Uint32)+4];

			for (unsigned long i=0; i<( Length/sizeof(Uint32) ); i++ )
			{
				sprintf( tmp, "%lu\\", *uintVals );
				tmp += strlen(tmp);
				uintVals++;
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


unsigned long DcmUnsignedLong::getVM()
{
    return Length / sizeof(Uint32);
}


// ********************************


E_Condition DcmUnsignedLong::put(const Uint32 * uintVal,
							   const unsigned long numUints)
{
    errorFlag = EC_Normal;
	if (numUints)
	{
		if (uintVal)
			errorFlag = this -> putValue(uintVal, 
										 sizeof(Uint32)*Uint32(numUints));
		else
			errorFlag = EC_CorruptedData;
	}
	return errorFlag;
}


// ********************************


E_Condition DcmUnsignedLong::put(const Uint32 uintVal)
{
	Uint32 val = uintVal;
	errorFlag = this -> putValue(&val, sizeof(Uint32));
	return errorFlag;

    errorFlag = EC_Normal;
}


// ********************************


E_Condition DcmUnsignedLong::put(const Uint32 uintVal,
							   const unsigned long position)
{
	Bdebug((2, "DcmUnsignedLong::put(slong=%ld,num=%ld)", uintVal, position));

	Uint32 val = uintVal;

	errorFlag = this -> changeValue(&val, sizeof(Uint32)*position,
									sizeof(Uint32));
	return errorFlag;
}


// ********************************


E_Condition DcmUnsignedLong::get(Uint32 * & uintVals)
{
	uintVals = this -> get();
	return errorFlag;
}

// ********************************


E_Condition DcmUnsignedLong::get(Uint32 & uintVal, const unsigned long pos)
{
	uintVal = this -> get(pos);
	return errorFlag;
}


// ********************************


Uint32 * DcmUnsignedLong::get(void)
{
	Uint32 * uintVal = (Uint32 *)this -> getValue();

	if (errorFlag == EC_Normal)
		return uintVal;
	else
		return NULL;
}


// ********************************


Uint32 DcmUnsignedLong::get(const unsigned long position)
{
	Uint32 * uintVals = this -> get();

	if (uintVals && errorFlag == EC_Normal &&
		position < this -> getVM())
		return uintVals[position];
	else
		return Uint32(0);
}


// ********************************


E_Condition DcmUnsignedLong::verify(const BOOL autocorrect )
{
    errorFlag = EC_Normal;
    if ( Length % (sizeof(Uint32)) != 0 )
    {
		errorFlag = EC_CorruptedData;
		if (autocorrect)
		{
					    // auf gueltige Laenge kuerzen
			Length = Length - ( Length % (sizeof(Uint32)) );
		}
    }
    return errorFlag;
}


// ********************************

/*
** CVS/RCS Log:
** $Log: dcvrul.cc,v $
** Revision 1.3  1996-01-05 13:27:55  andreas
** - changed to support new streaming facilities
** - unique read/write methods for file and block transfer
** - more cleanups
**
**
**
*/
