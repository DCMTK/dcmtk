/*
**
** Author: Gerd Ehlers      01.05.94 -- First Creation
**         Andreas Barth    05.12.95 -- new Stream class, unique value field
** Kuratorium OFFIS e.V.
**
** Module: dcvrus.cc
**
** Purpose:
** Implementation of class DcmUnsignedShort
**
** Last Update:		$Author: andreas $
** Update Date:		$Date: 1996-08-05 08:46:25 $
** Source File:		$Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/libsrc/dcvrus.cc,v $
** CVS/RCS Revision:	$Revision: 1.8 $
** Status:		$State: Exp $
**
** CVS/RCS Log at end of file
**
*/

#include <stdio.h>
#include <string.h>
#include <iostream.h>

#include "dcvrus.h"
#include "dcdebug.h"
#include "dcvm.h"


// ********************************


DcmUnsignedShort::DcmUnsignedShort(const DcmTag &tag, const Uint32 len)
    : DcmElement(tag, len)
{
    Bdebug((5, "dcvrsl:DcmUnsignedShort::DcmUnsignedShort(DcmTag&,len=%ld)",
	    len ));
    debug(( 8, "Object pointer this=0x%p", this ));

    Edebug(());

}


// ********************************


DcmUnsignedShort::DcmUnsignedShort(const DcmUnsignedShort& old)
    : DcmElement( old )
{
    Bdebug((5, "dcvrsl:DcmUnsignedShort::DcmUnsignedShort(DcmUnsignedShort&)" ));
    debug(( 8, "Object pointer this=0x%p", this ));

    if ( old.ident() != EVR_US )
    {
        errorFlag = EC_IllegalCall;
        cerr << "Warning: DcmUnsignedShort: wrong use of Copy-Constructor"
             << endl;
    }
    Edebug(());
}


// ********************************


DcmUnsignedShort::~DcmUnsignedShort(void)
{
    Bdebug((5, "dcvrsl:DcmUnsignedShort::~DcmUnsignedShort()" ));
    debug(( 8, "Object pointer this=0x%p", this ));

    Edebug(());
}


// ********************************


void DcmUnsignedShort::print(ostream & out, const BOOL showFullData,
			     const int level)
{
    if (this -> valueLoaded())
    {
	Uint16 * uintVals = this -> get();

	if (!uintVals)
	    printInfoLine(out, showFullData, level, "(no value available)" );
	else
	{
	    const Uint32 valueLength = Length/sizeof(Uint16);
	    const Uint32 maxCount =
		!showFullData && DCM_OptPrintLineLength/8 < valueLength ?
		DCM_OptPrintLineLength/8 : valueLength;
	    char *ch_words;
	    char *tmp = ch_words = new char[maxCount*8+6];

	    for (unsigned long i=0; i<maxCount; i++ )
	    {
		sprintf( tmp, "%hu\\", *uintVals );
		tmp += strlen(tmp);
		uintVals++;
	    }
	    if (maxCount > 0)
		tmp--;
	    *tmp = '\0';

	    if (maxCount < valueLength)
		strcat(tmp, "...");

	    printInfoLine(out, showFullData, level, ch_words);
	    delete ch_words;
	}
    }
    else
	printInfoLine(out, showFullData, level, "(not loaded)" );
}


// ********************************


unsigned long DcmUnsignedShort::getVM()
{
    return Length / sizeof(Uint16);
}


// ********************************


E_Condition DcmUnsignedShort::put(const Uint16 * uintVal,
				  const unsigned long numUints)
{
    errorFlag = EC_Normal;
    if (numUints)
    {
	if (uintVal)
	    errorFlag = this -> putValue(uintVal, 
					 sizeof(Uint16)*Uint32(numUints));
	else
	    errorFlag = EC_CorruptedData;
    }
    else
	errorFlag = this -> putValue(NULL, 0);

    return errorFlag;
}


// ********************************


E_Condition DcmUnsignedShort::put(const Uint16 uintVal)
{
    Uint16 val = uintVal;
    errorFlag = this -> putValue(&val, sizeof(Uint16));
    return errorFlag;
}


// ********************************


E_Condition DcmUnsignedShort::put(const Uint16 uintVal,
				  const unsigned long position)
{
    Bdebug((2, "DcmUnsignedShort::put(slong=%ld,num=%ld)", uintVal, position));

    Uint16 val = uintVal;

    errorFlag = this -> changeValue(&val, sizeof(Uint16)*position,
				    sizeof(Uint16));
    return errorFlag;
}


// ********************************


E_Condition DcmUnsignedShort::put(const char * val)
{
    errorFlag = EC_Normal;
    if (val && val[0] != '\0')
    {
	unsigned long vm = getVMFromString(val);
	if (vm)
	{
	    Uint16 * field = new Uint16[vm];
	    const char * s = val;
	    
	    for(unsigned long i = 0; i < vm && errorFlag == EC_Normal; i++)
	    {
		char * value = getFirstValueFromString(s);
		if (!value || sscanf(value, "%hu", &field[i]) != 1)
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


E_Condition DcmUnsignedShort::get(Uint16 & uintVal, const unsigned long pos)
{
    Uint16 * uintVals = NULL;
    errorFlag = this -> get(uintVals);

    if (uintVals && errorFlag == EC_Normal &&
	pos < this -> getVM())
	uintVal = uintVals[pos];
    else
    {
	errorFlag = EC_IllegalCall;
	uintVal = 0;
    }
    return errorFlag;
}


// ********************************


E_Condition DcmUnsignedShort::get(Uint16 * & uintVals)
{
    uintVals = (Uint16 *)this -> getValue();
    return errorFlag;
}


// ********************************


Uint16 * DcmUnsignedShort::get(void)
{
    Uint16 * uintVal = (Uint16 *)this -> getValue();
    return uintVal;
}


// ********************************


Uint16 DcmUnsignedShort::get(const unsigned long position)
{
    Uint16 uintVal = 0;
    errorFlag = this -> get(uintVal, position);
    return uintVal;
}


// ********************************


E_Condition DcmUnsignedShort::verify(const BOOL autocorrect )
{
    errorFlag = EC_Normal;
    if ( Length % (sizeof(Uint16)) != 0 )
    {
	errorFlag = EC_CorruptedData;
	if (autocorrect)
	{
	    // auf gueltige Laenge kuerzen
	    Length = Length - ( Length % (sizeof(Uint16)) );
	}
    }
    return errorFlag;
}


// ********************************

/*
** CVS/RCS Log:
** $Log: dcvrus.cc,v $
** Revision 1.8  1996-08-05 08:46:25  andreas
** new print routine with additional parameters:
**         - print into files
**         - fix output length for elements
** corrected error in search routine with parameter ESM_fromStackTop
**
** Revision 1.7  1996/05/20 13:27:54  andreas
** correct minor bug in print routine
**
** Revision 1.6  1996/04/16 16:05:27  andreas
** - better support und bug fixes for NULL element value
**
** Revision 1.5  1996/03/26 09:59:39  meichel
** corrected bug (deletion of const char *) which prevented compilation on NeXT
**
** Revision 1.4  1996/01/29 13:38:35  andreas
** - new put method for every VR to put value as a string
** - better and unique print methods
**
** Revision 1.3  1996/01/05 13:27:56  andreas
** - changed to support new streaming facilities
** - unique read/write methods for file and block transfer
** - more cleanups
**
**
**
*/
