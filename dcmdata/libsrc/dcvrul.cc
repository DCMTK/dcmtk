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
 *  Purpose: class DcmUnsignedLong
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2000-02-02 14:33:01 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/libsrc/dcvrul.cc,v $
 *  CVS/RCS Revision: $Revision: 1.13 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include <stdio.h>
#include <string.h>
#include <iostream.h>

#include "dcvrul.h"
#include "dcvm.h"
#include "dcdebug.h"


// ********************************


DcmUnsignedLong::DcmUnsignedLong(const DcmTag &tag, const Uint32 len)
: DcmElement(tag, len)
{
}


// ********************************


DcmUnsignedLong::DcmUnsignedLong(const DcmUnsignedLong& old)
: DcmElement( old )
{
    if ( old.ident() != EVR_UL )
	{
        errorFlag = EC_IllegalCall;
        cerr << "Warning: DcmUnsignedLong: wrong use of Copy-Constructor"
             << endl;
    }
}


// ********************************


DcmUnsignedLong::~DcmUnsignedLong(void)
{
}


// ********************************


void DcmUnsignedLong::print(ostream & out, const OFBool showFullData, 
			    const int level)
{
    if (this -> valueLoaded())
    {
	Uint32 * uintVals;
	errorFlag = this -> getUint32Array(uintVals);

	if (!uintVals)
	    printInfoLine(out, showFullData, level, "(no value available)" );
	else
	{
	    const Uint32 valueLength = Length/sizeof(Uint32);
	    const Uint32 maxCount =
		!showFullData && DCM_OptPrintLineLength/12 < valueLength ?
		DCM_OptPrintLineLength/12 : valueLength;
	    char *ch_words;
	    char *tmp = ch_words = new char[maxCount*12+8];

	    for (unsigned long i=0; i<maxCount; i++ )
	    {
#if SIZEOF_LONG == 8
		sprintf(tmp, "%u\\", *uintVals);
#else
		sprintf(tmp, "%lu\\", *uintVals);
#endif
		tmp += strlen(tmp);
		uintVals++;
	    }
	    if (maxCount > 0 )
		tmp--;
	    *tmp = '\0';

	    if (maxCount < valueLength)
		strcat(tmp, "...");

	    printInfoLine(out, showFullData, level, ch_words);
	    delete[] ch_words;
	}
    }
    else
	printInfoLine(out, showFullData, level, "(not loaded)");
}


// ********************************


unsigned long DcmUnsignedLong::getVM()
{
    return Length / sizeof(Uint32);
}


// ********************************


E_Condition DcmUnsignedLong::putUint32Array(const Uint32 * uintVal,
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
    else
	errorFlag = this -> putValue(NULL, 0);

    return errorFlag;
}


// ********************************


E_Condition DcmUnsignedLong::putUint32(const Uint32 uintVal,
				       const unsigned long position)
{
    Uint32 val = uintVal;
    errorFlag = this -> changeValue(&val, sizeof(Uint32)*position,
				    sizeof(Uint32));
    return errorFlag;
}


// ********************************


E_Condition DcmUnsignedLong::putString(const char * val)
{
    errorFlag = EC_Normal;
    if (val && val[0] != '\0')
    {
	unsigned long vm = getVMFromString(val);
	if (vm)
	{
	    Uint32 * field = new Uint32[vm];
	    const char * s = val;
	    
	    for(unsigned long i = 0; i < vm && errorFlag == EC_Normal; i++)
	    {
		char * value = getFirstValueFromString(s);
		if (!value || 
#if SIZEOF_LONG == 8
		    sscanf(value, "%u", &field[i]) != 1
#else
		    sscanf(value, "%lu", &field[i]) != 1
#endif
		    )
		    errorFlag = EC_CorruptedData;
		else if (value)
		    delete[] value;
	    }
	
	    if (errorFlag == EC_Normal)
		errorFlag = this -> putUint32Array(field, vm);
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


E_Condition DcmUnsignedLong::getUint32Array(Uint32 * & uintVals)
{
    uintVals = (Uint32 *)this -> getValue();
    return errorFlag;
}

// ********************************


E_Condition DcmUnsignedLong::getUint32(Uint32 & uintVal, 
				       const unsigned long pos)
{
    Uint32 * uintVals = NULL;
    errorFlag = this -> getUint32Array(uintVals);

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

E_Condition DcmUnsignedLong::verify(const OFBool autocorrect )
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
** Revision 1.13  2000-02-02 14:33:01  joergr
** Replaced 'delete' statements by 'delete[]' for objects created with 'new[]'.
**
** Revision 1.12  1999/03/31 09:26:01  meichel
** Updated copyright header in module dcmdata
**
** Revision 1.11  1997/07/21 08:25:36  andreas
** - Replace all boolean types (BOOLEAN, CTNBOOLEAN, DICOM_BOOL, BOOL)
**   with one unique boolean type OFBool.
**
** Revision 1.10  1997/07/03 15:10:21  andreas
** - removed debugging functions Bdebug() and Edebug() since
**   they write a static array and are not very useful at all.
**   Cdebug and Vdebug are merged since they have the same semantics.
**   The debugging functions in dcmdata changed their interfaces
**   (see dcmdata/include/dcdebug.h)
**
** Revision 1.9  1997/04/18 08:17:21  andreas
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
** Revision 1.8  1996/08/05 08:46:24  andreas
** new print routine with additional parameters:
**         - print into files
**         - fix output length for elements
** corrected error in search routine with parameter ESM_fromStackTop
**
** Revision 1.7  1996/05/20 13:27:53  andreas
** correct minor bug in print routine
**
** Revision 1.6  1996/04/16 16:05:26  andreas
** - better support und bug fixes for NULL element value
**
** Revision 1.5  1996/03/26 09:59:38  meichel
** corrected bug (deletion of const char *) which prevented compilation on NeXT
**
** Revision 1.4  1996/01/29 13:38:35  andreas
** - new put method for every VR to put value as a string
** - better and unique print methods
**
** Revision 1.3  1996/01/05 13:27:55  andreas
** - changed to support new streaming facilities
** - unique read/write methods for file and block transfer
** - more cleanups
**
**
**
*/
