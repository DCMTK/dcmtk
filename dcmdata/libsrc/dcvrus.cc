/*
 *
 *  Copyright (C) 1994-99, OFFIS
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
 *  Purpose: class DcmUnsignedShort
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 2000-03-03 14:05:41 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/libsrc/dcvrus.cc,v $
 *  CVS/RCS Revision: $Revision: 1.15 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
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
}


// ********************************


DcmUnsignedShort::DcmUnsignedShort(const DcmUnsignedShort& old)
    : DcmElement( old )
{
    if ( old.ident() != EVR_US )
    {
        errorFlag = EC_IllegalCall;
        CERR << "Warning: DcmUnsignedShort: wrong use of Copy-Constructor"
             << endl;
    }
}


// ********************************


DcmUnsignedShort::~DcmUnsignedShort(void)
{
}


// ********************************


void DcmUnsignedShort::print(ostream & out, const OFBool showFullData,
			     const int level, const char * /*pixelFileName*/,
                 size_t * /*pixelCounter*/)
{
    if (this -> valueLoaded())
    {
	Uint16 * uintVals;
	errorFlag = this -> getUint16Array(uintVals);

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
	    delete[] ch_words;
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


E_Condition DcmUnsignedShort::putUint16Array(const Uint16 * uintVal,
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


E_Condition DcmUnsignedShort::putUint16(const Uint16 uintVal,
					const unsigned long position)
{
    Uint16 val = uintVal;

    errorFlag = this -> changeValue(&val, sizeof(Uint16)*position,
				    sizeof(Uint16));
    return errorFlag;
}


// ********************************


E_Condition DcmUnsignedShort::putString(const char * val)
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
		errorFlag = this -> putUint16Array(field, vm);
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


E_Condition DcmUnsignedShort::getUint16(Uint16 & uintVal, 
					const unsigned long pos)
{
    Uint16 * uintVals = NULL;
    errorFlag = this -> getUint16Array(uintVals);

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


E_Condition DcmUnsignedShort::getUint16Array(Uint16 * & uintVals)
{
    uintVals = (Uint16 *)this -> getValue();
    return errorFlag;
}


// ********************************

E_Condition DcmUnsignedShort::verify(const OFBool autocorrect )
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
** Revision 1.15  2000-03-03 14:05:41  meichel
** Implemented library support for redirecting error messages into memory
**   instead of printing them to stdout/stderr for GUI applications.
**
** Revision 1.14  2000/02/10 10:52:26  joergr
** Added new feature to dcmdump (enhanced print method of dcmdata): write
** pixel data/item value fields to raw files.
**
** Revision 1.13  2000/02/02 14:33:03  joergr
** Replaced 'delete' statements by 'delete[]' for objects created with 'new[]'.
**
** Revision 1.12  1999/03/31 09:26:03  meichel
** Updated copyright header in module dcmdata
**
** Revision 1.11  1997/07/21 08:25:37  andreas
** - Replace all boolean types (BOOLEAN, CTNBOOLEAN, DICOM_BOOL, BOOL)
**   with one unique boolean type OFBool.
**
** Revision 1.10  1997/07/03 15:10:22  andreas
** - removed debugging functions Bdebug() and Edebug() since
**   they write a static array and are not very useful at all.
**   Cdebug and Vdebug are merged since they have the same semantics.
**   The debugging functions in dcmdata changed their interfaces
**   (see dcmdata/include/dcdebug.h)
**
** Revision 1.9  1997/04/18 08:10:54  andreas
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
** Revision 1.8  1996/08/05 08:46:25  andreas
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
*/
