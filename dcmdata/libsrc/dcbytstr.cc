/*
**
** Author: Gerd Ehlers	    05.05.94 -- Created
**         Andreas Barth	01.12.95 -- New Stream and value storing
** Kuratorium OFFIS e.V.
**
** Module: dcbytstr.h
**
** Purpose:
** Implementation of class DcmByteString
**
** Last Update:		$Author: hewett $
** Update Date:		$Date: 1996-05-31 09:09:08 $
** Source File:		$Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/libsrc/dcbytstr.cc,v $
** CVS/RCS Revision:	$Revision: 1.8 $
** Status:		$State: Exp $
**
** CVS/RCS Log at end of file
**
*/

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream.h>


#include "dcbytstr.h"
#include "dcvr.h"
#include "dcdebug.h"



// ********************************


DcmByteString::DcmByteString(const DcmTag &tag,
			     const Uint32 len)
    : DcmElement(tag, len)
{
    Bdebug((6, "DcmByteString::DcmByteString(DcmTag&,len=%ld)",
	    len));
    realLength = len;
    paddingChar = ' ';
    maxLength = DCM_UndefinedLength;
    Edebug(());
}


// ********************************


DcmByteString::DcmByteString(const DcmByteString& old, const DcmEVR oldIdent)
    : DcmElement( old )
{
    Bdebug((6, "DcmByteString::DcmByteString(DcmObject&,oldIdent=%d)",
	    oldIdent ));

    paddingChar = ' ';
    realLength = old.realLength;
    maxLength = DCM_UndefinedLength;
    if (old.ident() != oldIdent)
    {
	errorFlag = EC_IllegalCall;
        cerr << "Warning: DcmByteString: wrong use of Copy-Constructor"
	     << endl;
    }
    Edebug(());
}


// ********************************


DcmByteString::~DcmByteString(void)
{
}


// ********************************


void DcmByteString::print(const int level)
{
    if (this -> valueLoaded())
    {
	const char * byteStringValue = this -> get();
	if (byteStringValue)
	{
	    char *tmp = new char[realLength + 3];
	    tmp[0] = '[';
	    strncpy( tmp+1, byteStringValue, realLength );
	    tmp[realLength + 1 ] = ']';
	    tmp[realLength + 2 ] = '\0';
	    printInfoLine(level, tmp);
	    delete tmp;
	}
	else
	    printInfoLine( level, "(no value available)" );
    }
    else
	printInfoLine( level, "(not loaded)" );
}


// ********************************


E_Condition DcmByteString::get(char * & byteStringValue)
{
    byteStringValue = (char *)this -> getValue();

    if (byteStringValue && fStringMode != DCM_MachineString)
	this -> makeMachineByteString();

    return errorFlag;
}


// ********************************


char * DcmByteString::get(void)
{
    char * value = (char *)this -> getValue();

    if (value && fStringMode != DCM_MachineString)
	this -> makeMachineByteString();

    return value;
}


// ********************************


Uint32 DcmByteString::getRealLength(void)
{
    switch (fStringMode)
    {
    case DCM_MachineString:
	break;

    case DCM_DicomString:
    case DCM_UnknownString:
	this -> makeMachineByteString();
    }

    return realLength;
}


// ********************************


unsigned long DcmByteString::getVM()
{
    const char * s = this -> get();
    unsigned long vm = 0;
    if (s == NULL || Length == 0)
	vm = 0;
    else
    {
	vm = 1;
	char c;
	while ((c = *s++) != 0)
	    if ( c == '\\' )
		vm++;
    }
    return vm;
}


// ********************************

E_Condition DcmByteString::makeDicomByteString(void)
{
    char * value = NULL;
    errorFlag = this -> get(value);

    if (value) 
    {
	if (realLength & 1)
	{
	    Length = realLength + 1;
	    value[realLength] = paddingChar;
	}
	else if (realLength < Length)
	    Length = realLength;

	value[Length] = '\0';
    }
    fStringMode = DCM_DicomString;
    return errorFlag;
}

// ********************************

E_Condition DcmByteString::makeMachineByteString(void)
{
    errorFlag = EC_Normal;
    char * value = NULL;
    value = (char *)this -> getValue();
    if (value)
    	realLength = strlen(value);
    else
    	realLength = 0;

    /* 
    ** This code removes extra padding chars at the end of
    ** a ByteString.  Trailing padding can cause problems
    ** when comparing strings.
    */
    if (realLength)
    {
	size_t i = 0;
	for(i = realLength;
	    i > 0 && (value[i-1] == paddingChar);
	    i--)
	    value[i-1] = '\0';

	realLength = (Uint32)i;
    }
    fStringMode = DCM_MachineString;
    return errorFlag;
}


// ********************************

Uint8 * DcmByteString::newValueField(void)
{
    // This is correct since the relation Length >= realLength is true.

    Uint8 * value = NULL;
    if (Length & 1)
    {
	value = new Uint8[Length+2]; // protocol error: odd value length 
	if (value)
	    value[Length] = 0;
	Length++;		// make value length even
    }
    else
	value = new Uint8[Length+1];

    if (value)
	value[Length] = 0;

    return value;
}

// ********************************

void DcmByteString::postLoadValue(void)
{
    fStringMode = DCM_UnknownString;
}

// ********************************


E_Condition 
DcmByteString::put(const char * byteStringValue)
{
    errorFlag = EC_Normal;

    if (byteStringValue && byteStringValue[0] != '\0')
	this -> putValue(byteStringValue, strlen(byteStringValue));
    else
	this -> putValue(NULL, 0);

    fStringMode = DCM_UnknownString;
    this -> makeMachineByteString();

    return errorFlag;
}

// ********************************

E_Condition DcmByteString::verify(const BOOL autocorrect)
{
    Bdebug((3, "DcmByteString::verify(autocorrect=%d)", autocorrect));
    debug(( 3, "Tag=(0x%4.4x,0x%4.4x) \"%s\" \"%s\"",
	    getGTag(), getETag(),
	    DcmVR(getVR()).getVRName(), getTag().getTagName() ));

    char * value = NULL;
    errorFlag = this -> get(value);
    if (value != NULL && realLength != 0 )
    {
	char *tempstr = new char[ realLength + 1 ];
	unsigned long field = 0;
	unsigned long num = getVM();
	unsigned long pos = 0;
	unsigned long temppos = 0;
	char c;
	while (field < num )
	{
	    unsigned long fieldlen = 0;
	    while ((c = value[pos++]) != 0 && c != '\\')
	    {
		if (fieldlen < maxLength && autocorrect)
		    tempstr[temppos++] = c;
		fieldlen++;
	    }
	    if (fieldlen >= maxLength)
		errorFlag = EC_CorruptedData;

	    if (autocorrect)
		tempstr[temppos++] = c;      // c ist entweder '\\' oder NULL

	    field++;
	    if (pos > Length)
		break;
	}
	if (autocorrect)
	    this -> put(tempstr);
	delete tempstr;
    }

    Vdebug((3, errorFlag!=EC_Normal,
	    "Illegal values in Tag=(0x%4.4x,0x%4.4x) VM=%d",
	    getGTag(), getETag(), getVM() ));
    Edebug(());

    return errorFlag;
}

// ********************************

E_Condition DcmByteString::clear(void)
{
    errorFlag = DcmElement::clear();
    fStringMode = DCM_UnknownString;
    return errorFlag;
}


// ********************************

E_Condition DcmByteString::write(DcmStream & outStream,
				 const E_TransferSyntax oxfer,
				 const E_EncodingType enctype,
				 const E_GrpLenEncoding gltype)
{
    if (fTransferState == ERW_notInitialized)
	errorFlag = EC_IllegalCall;
    else
    {
	if (fTransferState == ERW_init)
	    this -> makeDicomByteString();

	errorFlag = DcmElement::write(outStream, oxfer, enctype, gltype);
    }
    return errorFlag;
}


/*
** CVS/RCS Log:
** $Log: dcbytstr.cc,v $
** Revision 1.8  1996-05-31 09:09:08  hewett
** The stripping of trailing padding characters has been restored (without
** the 8bit char removal bug).  Trailing padding characters are insignificant
** and if they are not removed problems arise with string comparisons since
** the dicom encoding rules require the addition of a padding character for
** odd length strings.
**
** Revision 1.7  1996/05/30 17:17:49  hewett
** Disabled erroneous code to strip trailing padding characters.
**
** Revision 1.6  1996/04/16 16:05:22  andreas
** - better support und bug fixes for NULL element value
**
** Revision 1.5  1996/03/11 13:17:23  hewett
** Removed get function for unsigned char*
**
** Revision 1.4  1996/01/09 11:06:42  andreas
** New Support for Visual C++
** Correct problems with inconsistent const declarations
** Correct error in reading Item Delimitation Elements
**
** Revision 1.3  1996/01/05 13:27:32  andreas
** - changed to support new streaming facilities
** - unique read/write methods for file and block transfer
** - more cleanups
**
**
*/

