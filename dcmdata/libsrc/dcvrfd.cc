/*
**
** Author: Gerd Ehlers      01.05.94 -- First Creation
**         Andreas Barth    05.12.95 -- new Stream class, unique value field
** Kuratorium OFFIS e.V.
**
** Module: dcvrfd.cc
**
** Purpose:
** Implementation of class DcmFloatingPointDouble
**
** Last Update:		$Author: meichel $
** Update Date:		$Date: 1996-03-26 09:59:35 $
** Source File:		$Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/libsrc/dcvrfd.cc,v $
** CVS/RCS Revision:	$Revision: 1.5 $
** Status:		$State: Exp $
**
** CVS/RCS Log at end of file
**
*/

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include <stdio.h>
#include <string.h>
#include <iostream.h>

#include "dcvrfd.h"
#include "dcvm.h"
#include "dcdebug.h"


// ********************************


DcmFloatingPointDouble::DcmFloatingPointDouble(const DcmTag &tag,
                                               const Uint32 len)
: DcmElement(tag, len)
{
Bdebug((5, "dcvrfd:DcmFloatingPointDouble::DcmFloatingPointDouble"
           "(DcmTag&,len=%ld)", len ));
debug(( 8, "Object pointer this=0x%p", this ));

Edebug(());

}


// ********************************


DcmFloatingPointDouble::DcmFloatingPointDouble(const DcmFloatingPointDouble& old)
: DcmElement( old )
{
Bdebug((5, "dcvrfd:DcmFloatingPointDouble::DcmFloatingPointDouble(DcmFloatingPointDouble&)" ));
debug(( 8, "Object pointer this=0x%p", this ));

    if ( old.ident() != EVR_FD ) 
	{
        errorFlag = EC_IllegalCall;
        cerr << "Warning: DcmFloatingPointDouble: wrong use of Copy-Constructor"
             << endl;
    }
Edebug(());
}


// ********************************


DcmFloatingPointDouble::~DcmFloatingPointDouble()
{
Bdebug((5, "dcvrfd:DcmFloatingPointDouble::~DcmFloatingPointDouble()" ));
debug(( 8, "Object pointer this=0x%p", this ));

Edebug(());
}


void DcmFloatingPointDouble::print(const int level)
{
	if (valueLoaded)
	{
		Float64 * doubleVals =  this -> get();

		if (!doubleVals)
			printInfoLine( level, "(no value available)" );
		else
		{
			char *ch_words;
			char *tmp = ch_words = new char[Length*26/sizeof(Float64)+8];

			for (unsigned long i=0; i<( Length/sizeof(Float64) ); i++ )
			{
				sprintf( tmp, "%g\\", *doubleVals );
				tmp += strlen(tmp);
				doubleVals++;
			}
			if ( Length > 0 )
				tmp--;
			*tmp = '\0';
			printInfoLine(level, ch_words);
			delete ch_words;
		}
    }
    else
		DcmObject::printInfoLine( level, "(not loaded)" );
}


// ********************************


unsigned long DcmFloatingPointDouble::getVM(void)
{
    return Length / sizeof(Float64);
}


// ********************************


E_Condition DcmFloatingPointDouble::put(const Float64 * doubleVal,
					const unsigned long numDoubles)
{
    errorFlag = EC_Normal;
    if (numDoubles)
    {
	if (doubleVal)
	    errorFlag = this -> putValue(doubleVal, 
					 sizeof(Float64)*Uint32(numDoubles));
	else
	    errorFlag = EC_CorruptedData;
    }
    return errorFlag;
}


// ********************************

E_Condition DcmFloatingPointDouble::put(const Float64 doubleVal )
{
    Float64 val = doubleVal;
    errorFlag = this -> putValue(&val, sizeof(Float64));
    return errorFlag;
}


// ********************************


E_Condition DcmFloatingPointDouble::put(const Float64 doubleVal,
                                        const unsigned long position )
{
    Bdebug((2, "DcmFloatingPointDouble::put(doubleval=%ld,position=%ld)", 
	    doubleVal, position));

    Float64 val = doubleVal;

    errorFlag = this -> changeValue(&val, sizeof(Float64)*position, 
				    sizeof(Float64));

    return errorFlag;
}


// ********************************


E_Condition DcmFloatingPointDouble::put(const char * val)
{
    errorFlag = EC_Normal;
    if (val)
    {
	unsigned long vm = getVMFromString(val);
	Float64 * field = new Float64[vm];
	const char * s = val;
	    
	for(unsigned long i = 0; i < vm && errorFlag == EC_Normal; i++)
	{
	    char * value = getFirstValueFromString(s);
	    if (!value || sscanf(value, "%lf", &field[i]) != 1)
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


E_Condition DcmFloatingPointDouble::get(Float64 * & doubleVals)
{
	doubleVals = this -> get();
	return errorFlag;
}


// ********************************


E_Condition DcmFloatingPointDouble::get(Float64 & doubleVal, 
										const unsigned long pos)
{
	doubleVal = this -> get(pos);
	return errorFlag;
}


// ********************************


Float64 * DcmFloatingPointDouble::get(void)
{
	Float64 * doubleVal = (Float64 *)this -> getValue();

	if (errorFlag == EC_Normal)
		return doubleVal;
	else
		return NULL;
}


// ********************************


Float64 DcmFloatingPointDouble::get(const unsigned long position)
{
	Float64 * doubleVals = this -> get();

	if (doubleVals && errorFlag == EC_Normal &&
		position < this -> getVM())
		return doubleVals[position];
	else
	{
		if (errorFlag == EC_Normal)
			errorFlag = EC_CorruptedData;
		return Float64(0.0);
	}
}


// ********************************


E_Condition DcmFloatingPointDouble::verify(const BOOL autocorrect )
{
    errorFlag = EC_Normal;
    if ( Length % (sizeof(Float64)) != 0 )
    {
		errorFlag = EC_CorruptedData;
		if (autocorrect)
					    // auf gueltige Laenge kuerzen
			Length = Length - (Length % (sizeof(Float64)));
    }
    return errorFlag;
}


// ********************************

/*
** CVS/RCS Log:
** $Log: dcvrfd.cc,v $
** Revision 1.5  1996-03-26 09:59:35  meichel
** corrected bug (deletion of const char *) which prevented compilation on NeXT
**
** Revision 1.4  1996/01/29 13:38:32  andreas
** - new put method for every VR to put value as a string
** - better and unique print methods
**
** Revision 1.3  1996/01/05 13:27:48  andreas
** - changed to support new streaming facilities
** - unique read/write methods for file and block transfer
** - more cleanups
**
**
**
*/
