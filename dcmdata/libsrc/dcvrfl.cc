/*
 *
 *  Copyright (C) 1994-2001, OFFIS
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
 *  Author:  Gerd Ehlers, Andreas Barth
 *
 *  Purpose: class DcmFloatingPointSingle
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 2001-06-01 15:49:17 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/libsrc/dcvrfl.cc,v $
 *  CVS/RCS Revision: $Revision: 1.19 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
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
}


// ********************************


DcmFloatingPointSingle::~DcmFloatingPointSingle()
{
}


// ********************************


void DcmFloatingPointSingle::print(ostream & out, const OFBool showFullData,
			  const int level, const char * /*pixelFileName*/,
		      size_t * /*pixelCounter*/)
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
	    delete[] ch_words;
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
** Revision 1.19  2001-06-01 15:49:17  meichel
** Updated copyright header
**
** Revision 1.18  2000/04/14 15:55:08  meichel
** Dcmdata library code now consistently uses ofConsole for error output.
**
** Revision 1.17  2000/03/08 16:26:47  meichel
** Updated copyright header.
**
** Revision 1.16  2000/03/03 14:05:39  meichel
** Implemented library support for redirecting error messages into memory
**   instead of printing them to stdout/stderr for GUI applications.
**
** Revision 1.15  2000/02/10 10:52:24  joergr
** Added new feature to dcmdump (enhanced print method of dcmdata): write
** pixel data/item value fields to raw files.
**
** Revision 1.14  2000/02/02 14:32:57  joergr
** Replaced 'delete' statements by 'delete[]' for objects created with 'new[]'.
**
** Revision 1.13  1999/03/31 09:25:52  meichel
** Updated copyright header in module dcmdata
**
** Revision 1.12  1997/07/21 08:25:33  andreas
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
*/

