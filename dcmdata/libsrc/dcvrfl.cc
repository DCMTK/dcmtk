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
** Update Date:		$Date: 1997-07-21 08:25:33 $
** Source File:		$Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/libsrc/dcvrfl.cc,v $
** CVS/RCS Revision:	$Revision: 1.12 $
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
}




// ********************************


DcmFloatingPointSingle::DcmFloatingPointSingle(const DcmFloatingPointSingle& old)
: DcmElement( old )
{
    if ( old.ident() != EVR_FL )
    {
        errorFlag = EC_IllegalCall;
        cerr << "Warning: DcmFloatingPointSingle: wrong use of Copy-Constructor"
             << endl;
    }
}


// ********************************


DcmFloatingPointSingle::~DcmFloatingPointSingle()
{
}


// ********************************


void DcmFloatingPointSingle::print(ostream & out, const OFBool showFullData,
				   const int level)
{
    if (this -> valueLoaded())
    {
	Float32 * floatVals;
	errorFlag =  this -> getFloat32Array(floatVals);

	if (!floatVals)
	    printInfoLine(out, showFullData, level, "(no value available)" );
	else
	{
	    const Uint32 valueLength = Length/sizeof(Float32);
	    const Uint32 maxCount =
		!showFullData && DCM_OptPrintLineLength/26 < valueLength ?
		DCM_OptPrintLineLength/26 : valueLength;
	    char *ch_words;
	    char *tmp = ch_words = new char[maxCount*26+8];

	    for (unsigned long i=0; i<maxCount; i++ )
	    {
		sprintf( tmp, "%g\\", *floatVals );
		tmp += strlen(tmp);
		floatVals++;
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


unsigned long DcmFloatingPointSingle::getVM()
{
    return Length / sizeof(Float32);
}


// ********************************


E_Condition DcmFloatingPointSingle::putFloat32Array(
    const Float32 * floatVal,
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

E_Condition DcmFloatingPointSingle::putFloat32(const Float32 floatVal,
					       const unsigned long position)
{
    Float32 val = floatVal;
    errorFlag = this -> changeValue(&val, sizeof(Float32)*position,
				    sizeof(Float32));
    return errorFlag;
}


// ********************************


E_Condition DcmFloatingPointSingle::putString(const char * val)
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
		errorFlag = this -> putFloat32Array(field, vm);
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


E_Condition DcmFloatingPointSingle::getFloat32Array(Float32 * & singleVals)
{
	singleVals = (Float32 *)this -> getValue();
	return errorFlag;
}


// ********************************


E_Condition DcmFloatingPointSingle::getFloat32(Float32 & singleVal, 
					       const unsigned long pos)
{
    Float32 * floatVals = NULL;
    errorFlag = this -> getFloat32Array(floatVals);

    if (floatVals && errorFlag == EC_Normal &&
	pos < this -> getVM())
	singleVal = floatVals[pos];
    else
    {
	errorFlag = EC_IllegalCall;
	singleVal = (Float32)0.0;
    }
    return errorFlag;
}


// ********************************

E_Condition DcmFloatingPointSingle::verify(const OFBool autocorrect)
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
** Revision 1.12  1997-07-21 08:25:33  andreas
** - Replace all boolean types (BOOLEAN, CTNBOOLEAN, DICOM_BOOL, BOOL)
**   with one unique boolean type OFBool.
**
** Revision 1.11  1997/07/03 15:10:13  andreas
** - removed debugging functions Bdebug() and Edebug() since
**   they write a static array and are not very useful at all.
**   Cdebug and Vdebug are merged since they have the same semantics.
**   The debugging functions in dcmdata changed their interfaces
**   (see dcmdata/include/dcdebug.h)
**
** Revision 1.10  1997/04/18 08:10:52  andreas
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
** Revision 1.9  1996/09/27 08:20:40  hewett
** Added explicit cast: (Float32)0.0
**
** Revision 1.8  1996/08/05 08:46:20  andreas
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

