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
 *  Purpose: class DcmSignedLong
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 2001-09-25 17:20:00 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/libsrc/dcvrsl.cc,v $
 *  CVS/RCS Revision: $Revision: 1.19 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include <stdio.h>
#include <string.h>
#include <iostream.h>

#include "dcvrsl.h"
#include "dcvm.h"
#include "dcdebug.h"


// ********************************


DcmSignedLong::DcmSignedLong(const DcmTag &tag, const Uint32 len)
: DcmElement(tag, len)
{
}


// ********************************


DcmSignedLong::DcmSignedLong(const DcmSignedLong& old)
: DcmElement( old )
{
}


// ********************************


DcmSignedLong::~DcmSignedLong(void)
{
}


// ********************************


void DcmSignedLong::print(ostream & out, const OFBool showFullData,
			  const int level, const char * /*pixelFileName*/,
		      size_t * /*pixelCounter*/)
{
    if (this -> valueLoaded())
    {
	Sint32 * sintVals;
	errorFlag = this -> getSint32Array(sintVals);

	if (!sintVals)
	    printInfoLine(out, showFullData, level, "(no value available)" );
	else
	{
	    const Uint32 valueLength = Length/sizeof(Sint32);
	    const Uint32 maxCount =
		!showFullData && DCM_OptPrintLineLength/12 < valueLength ?
		DCM_OptPrintLineLength/12 : valueLength;
	    char *ch_words;
	    char *tmp = ch_words = new char[maxCount*12+8];

	    for (unsigned long i=0; i<maxCount; i++ )
	    {
#if SIZEOF_LONG == 8
		sprintf(tmp, "%d\\", *sintVals);
#else
		sprintf(tmp, "%ld\\", *sintVals);
#endif
		tmp += strlen(tmp);
		sintVals++;
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
	printInfoLine(out, showFullData, level, "(not loaded)" );
}


// ********************************


unsigned long DcmSignedLong::getVM()
{
    return Length / sizeof(Sint32);
}


// ********************************


OFCondition DcmSignedLong::putSint32Array(const Sint32 * sintVal,
					  const unsigned long numSints)
{
    errorFlag = EC_Normal;
    if (numSints)
    {
	if (sintVal)
	    errorFlag = this -> putValue(sintVal, 
					 sizeof(Sint32)*Uint32(numSints));
	else
	    errorFlag = EC_CorruptedData;
    }
    else 
	errorFlag = this -> putValue(NULL, 0);
    return errorFlag;
}


// ********************************


OFCondition DcmSignedLong::putSint32(const Sint32 sintVal,
				     const unsigned long position)
{
    Sint32 val = sintVal;
    errorFlag = this -> changeValue(&val, sizeof(Sint32)*position,
				    sizeof(Sint32));
    return errorFlag;
}


// ********************************


OFCondition DcmSignedLong::putString(const char * val)
{
    errorFlag = EC_Normal;
    if (val && val[0] != 0)
    {
	unsigned long vm = getVMFromString(val);
	if (vm)
	{
	    Sint32 * field = new Sint32[vm];
	    const char * s = val;
	    
	    for(unsigned long i = 0; i < vm && errorFlag == EC_Normal; i++)
	    {
		char * value = getFirstValueFromString(s);
		if (!value || 
#if SIZEOF_LONG == 8
		    sscanf(value, "%d", &field[i]) != 1
#else
		    sscanf(value, "%ld", &field[i]) != 1
#endif
		   )
		    errorFlag = EC_CorruptedData;
		else if (value)
		    delete[] value;
	    }
	
	    if (errorFlag == EC_Normal)
		errorFlag = this -> putSint32Array(field, vm);
	    delete[] field;
	}
	else 
	    this -> putValue(NULL, 0);
    }
    else
	this -> putValue(NULL, 0);
    return errorFlag;
}


// ********************************


OFCondition DcmSignedLong::getSint32Array(Sint32 * & sintVals)
{
	sintVals = (Sint32 *)this -> getValue();
	return errorFlag;
}

// ********************************


OFCondition DcmSignedLong::getSint32(Sint32 & sintVal, 
				     const unsigned long pos)
{
    Sint32 * sintVals = NULL;
    errorFlag = this -> getSint32Array(sintVals);

    if (sintVals && errorFlag == EC_Normal &&
	pos < this -> getVM())
	sintVal = sintVals[pos];
    else
    {
	sintVal = 0;
	errorFlag = EC_IllegalCall;
    }
    return errorFlag;
}

// ********************************


OFCondition DcmSignedLong::verify(const OFBool autocorrect )
{
    errorFlag = EC_Normal;
    if ( Length % (sizeof(Sint32)) != 0 )
    {
		errorFlag = EC_CorruptedData;
		if (autocorrect)
		{
					    // auf gueltige Laenge kuerzen
			Length = Length - ( Length % (sizeof(Sint32)) );
		}
    }
    return errorFlag;
}


// ********************************

/*
** CVS/RCS Log:
** $Log: dcvrsl.cc,v $
** Revision 1.19  2001-09-25 17:20:00  meichel
** Adapted dcmdata to class OFCondition
**
** Revision 1.18  2001/06/01 15:49:19  meichel
** Updated copyright header
**
** Revision 1.17  2000/04/14 15:55:09  meichel
** Dcmdata library code now consistently uses ofConsole for error output.
**
** Revision 1.16  2000/03/08 16:26:50  meichel
** Updated copyright header.
**
** Revision 1.15  2000/03/03 14:05:40  meichel
** Implemented library support for redirecting error messages into memory
**   instead of printing them to stdout/stderr for GUI applications.
**
** Revision 1.14  2000/02/10 10:52:25  joergr
** Added new feature to dcmdump (enhanced print method of dcmdata): write
** pixel data/item value fields to raw files.
**
** Revision 1.13  2000/02/02 14:32:58  joergr
** Replaced 'delete' statements by 'delete[]' for objects created with 'new[]'.
**
** Revision 1.12  1999/03/31 09:25:58  meichel
** Updated copyright header in module dcmdata
**
** Revision 1.11  1997/07/21 08:25:34  andreas
** - Replace all boolean types (BOOLEAN, CTNBOOLEAN, DICOM_BOOL, BOOL)
**   with one unique boolean type OFBool.
**
** Revision 1.10  1997/07/03 15:10:17  andreas
** - removed debugging functions Bdebug() and Edebug() since
**   they write a static array and are not very useful at all.
**   Cdebug and Vdebug are merged since they have the same semantics.
**   The debugging functions in dcmdata changed their interfaces
**   (see dcmdata/include/dcdebug.h)
**
** Revision 1.9  1997/04/18 08:10:52  andreas
** - Corrected debugging code
** - The put/get-methods for all VRs did not conform to the C++-Standard
**   draft. Some Compilers (e.g. SUN-C++ Compiler, Metroworks
**   CodeWarrier, etc.) create many warnings concerning the hiding of
**   overloaded get methods in all derived classes of DcmElement.
**   So the interface of all value representation classes in the
**   library are changed rapidly, e.g.
**   OFCondition get(Uint16 & value, const unsigned long pos);
**   becomes
**   OFCondition getUint16(Uint16 & value, const unsigned long pos);
**   All (retired) "returntype get(...)" methods are deleted.
**   For more information see dcmdata/include/dcelem.h
**
** Revision 1.8  1996/08/05 08:46:21  andreas
** new print routine with additional parameters:
**         - print into files
**         - fix output length for elements
** corrected error in search routine with parameter ESM_fromStackTop
**
** Revision 1.7  1996/05/20 13:27:52  andreas
** correct minor bug in print routine
**
** Revision 1.6  1996/04/16 16:05:25  andreas
** - better support und bug fixes for NULL element value
**
** Revision 1.5  1996/03/26 09:59:37  meichel
** corrected bug (deletion of const char *) which prevented compilation on NeXT
**
** Revision 1.4  1996/01/29 13:38:33  andreas
** - new put method for every VR to put value as a string
** - better and unique print methods
**
** Revision 1.3  1996/01/05 13:27:53  andreas
** - changed to support new streaming facilities
** - unique read/write methods for file and block transfer
** - more cleanups
**
*/

