/*
**
** Author: Gerd Ehlers      01.05.94 -- First Creation
**         Andreas Barth    05.12.95 -- new Stream class, unique value field
** Kuratorium OFFIS e.V.
**
** Module: dcvrfl.cc
**
** Purpose:
** Implementation of class DcmFloatingPointSingle
**
** Last Update:		$Author: andreas $
** Update Date:		$Date: 1996-04-16 16:05:23 $
** Source File:		$Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/libsrc/dcvrfl.cc,v $
** CVS/RCS Revision:	$Revision: 1.6 $
** Status:		$State: Exp $
**
** CVS/RCS Log at end of file
**
*/

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include <stdio.h>
#include <string.h>
#include <iostream.h>

#include "dcvrfl.h"
#include "dcvm.h"
#include "dcdebug.h"


// ********************************


DcmFloatingPointSingle::DcmFloatingPointSingle(const DcmTag &tag,
											   const Uint32 len)
: DcmElement(tag, len)
{
Bdebug((5, "DcmFloatingPointSingle::DcmFloatingPointSingle(DcmTag&,len=%ld)",
		len ));
debug(( 8, "Object pointer this=0x%p", this ));

Edebug(());

}




// ********************************


DcmFloatingPointSingle::DcmFloatingPointSingle(const DcmFloatingPointSingle& old)
: DcmElement( old )
{
Bdebug((5, "dcvrfl:DcmFloatingPointSingle::DcmFloatingPointSingle(DcmFloatingPointSingle&)" ));
debug(( 8, "Object pointer this=0x%p", this ));

    if ( old.ident() != EVR_FL )
	{
        errorFlag = EC_IllegalCall;
        cerr << "Warning: DcmFloatingPointSingle: wrong use of Copy-Constructor"
             << endl;
    }
Edebug(());
}


// ********************************


DcmFloatingPointSingle::~DcmFloatingPointSingle()
{
Bdebug((5, "dcvrfl:DcmFloatingPointSingle::~DcmFloatingPointSingle()" ));
debug(( 8, "Object pointer this=0x%p", this ));

Edebug(());

}


// ********************************


void DcmFloatingPointSingle::print(const int level)
{
    if (valueLoaded)
    {
	Float32 * floatVals =  this -> get();

	if (!floatVals)
	    printInfoLine( level, "(no value available)" );
	else
	{
	    char *ch_words;
	    char *tmp = ch_words = new char[Length*26/sizeof(Float32)+8];

	    for (unsigned long i=0; i<( Length/sizeof(Float32) ); i++ )
	    {
		sprintf( tmp, "%g\\", *floatVals );
		tmp += strlen(tmp);
		floatVals++;
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


unsigned long DcmFloatingPointSingle::getVM()
{
    return Length / sizeof(Float32);
}


// ********************************


E_Condition DcmFloatingPointSingle::put(const Float32 * floatVal,
					const unsigned long numFloats)
{
    errorFlag = EC_Normal;
    if (numFloats)
    {
	if (floatVal)
	    errorFlag = this -> putValue(floatVal, 
					 sizeof(Float32)*Uint32(numFloats));
	else
	    errorFlag = EC_CorruptedData;
    }
    else
	this -> putValue(NULL, 0);

    return errorFlag;
}


// ********************************


E_Condition DcmFloatingPointSingle::put(const Float32 floatVal)
{
    Float32 val = floatVal;
    errorFlag = this -> putValue(&val, sizeof(Float32));
    return errorFlag;
}


// ********************************


E_Condition DcmFloatingPointSingle::put(const Float32 floatVal,
					const unsigned long position)
{
    Bdebug((2, "DcmFloatingPointSingle::put(floatval=%ld,position=%ld)", 
	    floatVal, position));

    Float32 val = floatVal;

    errorFlag = this -> changeValue(&val, sizeof(Float32)*position,
				    sizeof(Float32));
    return errorFlag;
}


// ********************************


E_Condition DcmFloatingPointSingle::put(const char * val)
{
    errorFlag = EC_Normal;
    if (val && val[0] != 0)
    {
	unsigned long vm = getVMFromString(val);
	if (vm)
	{
	    Float32 * field = new Float32[vm];
	    const char * s = val;
	    
	    for(unsigned long i = 0; i < vm && errorFlag == EC_Normal; i++)
	    {
		char * value = getFirstValueFromString(s);
		if (!value || sscanf(value, "%f", &field[i]) != 1)
		    errorFlag = EC_CorruptedData;
		else if (value)
		    delete[] value;
	    }
	
	    if (errorFlag == EC_Normal)
		errorFlag = this -> put(field, vm);
	    delete[] field;
	}
	else
	    errorFlag = this -> putValue(NULL, 0);
    }
    else
	errorFlag = this -> putValue(NULL, 0);

    return errorFlag;
}


// ********************************


E_Condition DcmFloatingPointSingle::get(Float32 * & singleVals)
{
	singleVals = (Float32 *)this -> getValue();
	return errorFlag;
}


// ********************************


E_Condition DcmFloatingPointSingle::get(Float32 & singleVal, 
					const unsigned long pos)
{
    Float32 * floatVals = NULL;
    errorFlag = this -> get(floatVals);

    if (floatVals && errorFlag == EC_Normal &&
	pos < this -> getVM())
	singleVal = floatVals[pos];
    else
    {
	errorFlag = EC_IllegalCall;
	singleVal = 0.0;
    }
    return errorFlag;
}


// ********************************


Float32 * DcmFloatingPointSingle::get(void)
{
	Float32 * floatVal = (Float32 *)this -> getValue();

	if (errorFlag == EC_Normal)
		return floatVal;
	else
		return NULL;
}


// ********************************


Float32 DcmFloatingPointSingle::get(const unsigned long position)
{
    Float32 floatVal = 0.0;
    errorFlag = this -> get(floatVal, position);
    return floatVal;
}


// ********************************


E_Condition DcmFloatingPointSingle::verify(const BOOL autocorrect)
{
    errorFlag = EC_Normal;
    if ( Length % (sizeof(Float32)) != 0 )
    {
		errorFlag = EC_CorruptedData;
		if (autocorrect)
					    // auf gueltige Laenge kuerzen
			Length = Length - ( Length % (sizeof(Float32)) );
    }
    return errorFlag;
}


// ********************************

/*
** CVS/RCS Log:
** $Log: dcvrfl.cc,v $
** Revision 1.6  1996-04-16 16:05:23  andreas
** - better support und bug fixes for NULL element value
**
** Revision 1.5  1996/03/26 09:59:36  meichel
** corrected bug (deletion of const char *) which prevented compilation on NeXT
**
** Revision 1.4  1996/01/29 13:38:32  andreas
** - new put method for every VR to put value as a string
** - better and unique print methods
**
** Revision 1.3  1996/01/05 13:27:49  andreas
** - changed to support new streaming facilities
** - unique read/write methods for file and block transfer
** - more cleanups
**
**
**
*/

