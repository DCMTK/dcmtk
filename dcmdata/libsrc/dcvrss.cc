/*
**
** Author: Gerd Ehlers      01.05.94 -- First Creation
**         Andreas Barth    05.12.95 -- new Stream class, unique value field
** Kuratorium OFFIS e.V.
**
** Module: dcvrss.cc
**
** Purpose:
** Implementation of class DcmSignedShort
**
** Last Update:		$Author: meichel $
** Update Date:		$Date: 1996-03-26 09:59:38 $
** Source File:		$Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/libsrc/dcvrss.cc,v $
** CVS/RCS Revision:	$Revision: 1.5 $
** Status:		$State: Exp $
**
** CVS/RCS Log at end of file
**
*/

#include <stdio.h>
#include <string.h>
#include <iostream.h>

#include "dcvrss.h"
#include "dcvm.h"
#include "dcdebug.h"


// ********************************


DcmSignedShort::DcmSignedShort(const DcmTag &tag, const Uint32 len)
: DcmElement(tag, len)
{
Bdebug((5, "dcvrsl:DcmSignedShort::DcmSignedShort(DcmTag&,len=%ld)",
           len ));
debug(( 8, "Object pointer this=0x%p", this ));

Edebug(());

}


// ********************************


DcmSignedShort::DcmSignedShort(const DcmSignedShort& old)
: DcmElement( old )
{
Bdebug((5, "dcvrsl:DcmSignedShort::DcmSignedShort(DcmSignedShort&)" ));
debug(( 8, "Object pointer this=0x%p", this ));

    if ( old.ident() != EVR_SS )
	{
        errorFlag = EC_IllegalCall;
        cerr << "Warning: DcmSignedShort: wrong use of Copy-Constructor"
             << endl;
    }
Edebug(());
}


// ********************************


DcmSignedShort::~DcmSignedShort(void)
{
Bdebug((5, "dcvrsl:DcmSignedShort::~DcmSignedShort()" ));
debug(( 8, "Object pointer this=0x%p", this ));

Edebug(());
}


// ********************************


void DcmSignedShort::print(const int level)
{
	if (valueLoaded())
	{
		Sint16 * sintVals = this -> get();

		if (!sintVals)
			printInfoLine( level, "(no value available)" );
		else
		{
			char *ch_words;
			char *tmp = ch_words = new char[Length*8/sizeof(Sint16)+2];

			for (unsigned long i=0; i<( Length/sizeof(Sint16) ); i++ )
			{
				sprintf( tmp, "%hd\\", *sintVals );
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


unsigned long DcmSignedShort::getVM()
{
    return Length / sizeof(Sint16);
}


// ********************************


E_Condition DcmSignedShort::put(const Sint16 * sintVal,
				const unsigned long numSints)
{
    errorFlag = EC_Normal;
    if (numSints)
    {
	if (sintVal)
	    errorFlag = this -> putValue(sintVal, 
					 sizeof(Sint16)*Uint32(numSints));
	else
	    errorFlag = EC_CorruptedData;
    }
    return errorFlag;
}


// ********************************


E_Condition DcmSignedShort::put(const Sint16 sintVal)
{
    Sint16 val = sintVal;
    errorFlag = this -> putValue(&val, sizeof(Sint16));
    return errorFlag;

    errorFlag = EC_Normal;
}


// ********************************


E_Condition DcmSignedShort::put(const Sint16 sintVal,
				const unsigned long position)
{
    Bdebug((2, "DcmSignedShort::put(slong=%ld,num=%ld)", sintVal, position));

    Sint16 val = sintVal;

    errorFlag = this -> changeValue(&val, sizeof(Sint16)*position,
				    sizeof(Sint16));
    return errorFlag;
}


// ********************************


E_Condition DcmSignedShort::put(const char * val)
{
    errorFlag = EC_Normal;
    if (val)
    {
	unsigned long vm = getVMFromString(val);
	Sint16 * field = new Sint16[vm];
	const char * s = val;
	    
	for(unsigned long i = 0; i < vm && errorFlag == EC_Normal; i++)
	{
	    char * value = getFirstValueFromString(s);
	    if (!value || sscanf(value, "%hd", &field[i]) != 1)
		errorFlag = EC_CorruptedData;
	    else if (value)
		delete[] value;
	}
	
	if (errorFlag == EC_Normal)
	    errorFlag = this -> put(field, vm);
	delete[] field;
    }
    return errorFlag;
}


// ********************************


E_Condition DcmSignedShort::get(Sint16 & sintVal, const unsigned long pos)
{
	sintVal = this -> get(pos);
	return errorFlag;
}


// ********************************


E_Condition DcmSignedShort::get(Sint16 * & sintVals)
{
	sintVals = this -> get();
	return errorFlag;
}


// ********************************


Sint16 * DcmSignedShort::get(void)
{
	Sint16 * sintVal = (Sint16 *)this -> getValue();

	if (errorFlag == EC_Normal)
		return sintVal;
	else
		return NULL;
}


// ********************************


Sint16 DcmSignedShort::get(const unsigned long position)
{
	Sint16 * sintVals = this -> get();

	if (sintVals && errorFlag == EC_Normal &&
		position < this -> getVM())
		return sintVals[position];
	else
		return Sint16(0);
}


// ********************************


E_Condition DcmSignedShort::verify(const BOOL autocorrect )
{
    errorFlag = EC_Normal;
    if ( Length % (sizeof(Sint16)) != 0 )
    {
		errorFlag = EC_CorruptedData;
		if (autocorrect)
		{
					    // auf gueltige Laenge kuerzen
			Length = Length - ( Length % (sizeof(Sint16)) );
		}
    }
    return errorFlag;
}


// ********************************

/*
** CVS/RCS Log:
** $Log: dcvrss.cc,v $
** Revision 1.5  1996-03-26 09:59:38  meichel
** corrected bug (deletion of const char *) which prevented compilation on NeXT
**
** Revision 1.4  1996/01/29 13:38:34  andreas
** - new put method for every VR to put value as a string
** - better and unique print methods
**
** Revision 1.3  1996/01/05 13:27:53  andreas
** - changed to support new streaming facilities
** - unique read/write methods for file and block transfer
** - more cleanups
**
**
**
*/

