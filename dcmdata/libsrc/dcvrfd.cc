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
** Last Update:		$Author: andreas $
** Update Date:		$Date: 1997-04-18 08:10:50 $
** Source File:		$Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/libsrc/dcvrfd.cc,v $
** CVS/RCS Revision:	$Revision: 1.9 $
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


void DcmFloatingPointDouble::print(ostream & out, const BOOL showFullData, 
				   const int level)
{
    if (this -> valueLoaded())
    {
	Float64 * doubleVals;
	errorFlag =  this -> getFloat64Array(doubleVals);

	if (!doubleVals)
	    printInfoLine(out, showFullData, level, "(no value available)" );
	else
	{
	    const Uint32 valueLength = Length/sizeof(Float64);
	    const Uint32 maxCount = 
		!showFullData && DCM_OptPrintLineLength/26 < valueLength ?
		DCM_OptPrintLineLength/26 : valueLength;
	    char *ch_words;
	    char *tmp = ch_words = new char[maxCount*26+8];

	    for (unsigned long i=0; i<maxCount; i++ )
	    {
		sprintf( tmp, "%g\\", *doubleVals );
		tmp += strlen(tmp);
		doubleVals++;
	    }
	    if (maxCount> 0 )
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


unsigned long DcmFloatingPointDouble::getVM(void)
{
    return Length / sizeof(Float64);
}


// ********************************


E_Condition DcmFloatingPointDouble::putFloat64Array(
    const Float64 * doubleVal,
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
    else
	this -> putValue(NULL, 0);

    return errorFlag;
}


// ********************************

E_Condition DcmFloatingPointDouble::putFloat64(const Float64 doubleVal,
					       const unsigned long position )
{
Bdebug((2, "DcmFloatingPointDouble::putFloat64(doubleval=%ld,position=%ld)", 
	    doubleVal, position));

    Float64 val = doubleVal;

    errorFlag = this -> changeValue(&val, sizeof(Float64)*position, 
				    sizeof(Float64));

Edebug(());
    return errorFlag;
}


// ********************************


E_Condition DcmFloatingPointDouble::putString(const char * val)
{
    errorFlag = EC_Normal;
    if (val && val[0] != 0)
    {
	unsigned long vm = getVMFromString(val);
	if (vm)
	{
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
		errorFlag = this -> putFloat64Array(field, vm);
	    delete[] field;
	}
	else 
	    this -> putValue(NULL, 0);
    }
    else
	this -> putValue(NULL,0);
    return errorFlag;
}


// ********************************


E_Condition DcmFloatingPointDouble::getFloat64Array(Float64 * & doubleVals)
{
	doubleVals =(Float64 *)this -> getValue();
	return errorFlag;
}


// ********************************


E_Condition DcmFloatingPointDouble::getFloat64(Float64 & doubleVal, 
					       const unsigned long pos)
{
    Float64 * doubleVals = NULL;
    errorFlag = this -> getFloat64Array(doubleVals);

    if (doubleVals && errorFlag == EC_Normal &&
	pos < this -> getVM())
	doubleVal = doubleVals[pos];
    else if (doubleVals == NULL)
	errorFlag = EC_IllegalCall;
    else if (errorFlag == EC_Normal)
	errorFlag = EC_CorruptedData;
	
    if (errorFlag != EC_Normal)
	doubleVal = 0.0;

    return errorFlag;
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
** Revision 1.9  1997-04-18 08:10:50  andreas
** - Corrected debugging code
** - The put/get-methods for all VRs did not conform to the C++-Standard
**   draft. Some Compilers (e.g. SUN-C++ Compiler, Metroworks
**   CodeWarrier, etc.) create many warnings concerning the hiding of
**   overloaded get methods in all derived classes of DcmElement.
**   So the interface of all value representation classes in the
**   library are changed rapidly, e.g.
**   E_Condition get(Uint16 & value, const unsigned long pos);
**   becomes
**   E_Condition getUint16(Uint16 & value, const unsigned long pos);
**   All (retired) "returntype get(...)" methods are deleted.
**   For more information see dcmdata/include/dcelem.h
**
** Revision 1.8  1996/08/05 08:46:19  andreas
** new print routine with additional parameters:
**         - print into files
**         - fix output length for elements
** corrected error in search routine with parameter ESM_fromStackTop
**
** Revision 1.7  1996/05/20 13:27:51  andreas
** correct minor bug in print routine
**
** Revision 1.6  1996/04/16 16:05:23  andreas
** - better support und bug fixes for NULL element value
**
** Revision 1.5  1996/03/26 09:59:35  meichel
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
