/*
**
** Author: Gerd Ehlers      05.05.94 -- First Creation
**         Andreas Barth    05.12.95 -- new Stream class, unique value field
** Kuratorium OFFIS e.V.
**
** Module: dcvrobow.cc
**
** Purpose:
** Implementation of class DcmOtherByteOtherWord
**
**
** Last Update:		$Author: andreas $
** Update Date:		$Date: 1996-01-29 13:38:33 $
** Source File:		$Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/libsrc/dcvrobow.cc,v $
** CVS/RCS Revision:	$Revision: 1.5 $
** Status:		$State: Exp $
**
** CVS/RCS Log at end of file
**
*/

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream.h>

#include "dcvrobow.h"
#include "dcswap.h"
#include "dcstream.h"
#include "dcvm.h"
#include "dcdebug.h"



// ********************************


DcmOtherByteOtherWord::DcmOtherByteOtherWord(const DcmTag &tag,
					     const Uint32 len)
    : DcmElement(tag, len)
{
    Bdebug((5, "DcmOtherByteOtherWord::DcmOtherByteOtherWord(DcmTag&,len=%ld)",
	    len ));
    debug(( 8, "Object pointer this=0x%p", this ));

    Edebug(());

}


// ********************************


DcmOtherByteOtherWord::DcmOtherByteOtherWord( const DcmOtherByteOtherWord& old )
    : DcmElement( old )
{
    Bdebug((5, "dcvrobow:DcmOtherByteOtherWord::DcmOtherByteOtherWord("
	    "DcmOtherByteOtherWord&)"));
    debug(( 8, "Object pointer this=0x%p", this ));

    if (old.ident() != EVR_OW && old.ident() != EVR_OB && old.ident() != EVR_ox)
    {
	errorFlag = EC_IllegalCall;
        cerr << "Warning: DcmOtherByteOtherWord: wrong use of Copy-Constructor"
             << endl;
    }
    Edebug(());

}


// ********************************


DcmOtherByteOtherWord::~DcmOtherByteOtherWord()
{
    Bdebug((5, "dcvrobow:DcmOtherByteOtherWord::~DcmOtherByteOtherWord()"));
    debug(( 8, "Object pointer this=0x%p", this ));

    Edebug(());
}


// ********************************


E_Condition DcmOtherByteOtherWord::setVR(DcmEVR vr)
{
    errorFlag = EC_Normal;
    Tag -> setVR(vr);
    return errorFlag;
}



// ********************************


DcmEVR DcmOtherByteOtherWord::ident(void) const
{
    return Tag->getEVR();
}


// ********************************


void DcmOtherByteOtherWord::print(const int level )
{
    if (this -> valueLoaded())
    {
	const DcmEVR evr = Tag -> getEVR();
	const Uint16 * wordValues = NULL;
	const Uint8 * byteValues = NULL;

	if (evr == EVR_OW)
	    wordValues = this -> getWords();
	else
	    byteValues = this -> getBytes();

	errorFlag = EC_Normal;
	if (wordValues || byteValues)
	{
	    char *ch_words = NULL;;
	    char *tmp = NULL;

	    if (evr == EVR_OW)
		tmp = ch_words = new char[Length*5+6];
	    else
		tmp = ch_words = new char[Length*3+6];
		
	    if (tmp)
	    {
		const size_t bytePerValue = 
		    (evr == EVR_OW ? sizeof(Uint16) : sizeof(Uint8));

		for (unsigned int i=0; i<Length/bytePerValue; i++)
		{
		    if (evr == EVR_OW)
		    {
			sprintf(tmp, "%4.4hx\\", *wordValues);
			tmp += 5;
			wordValues++;
		    }
		    else
		    {
			sprintf(tmp, "%2.2x\\", *byteValues);
			tmp += 3;
			byteValues++;
		    }
		}

		if ( Length > 0 )
		    tmp--;
		*tmp = '\0';
		printInfoLine( level, ch_words );
		delete[] ch_words;
	    }
	    else
		errorFlag = EC_MemoryExhausted;
	}
	else
	    DcmObject::printInfoLine( level, "(no value available)" );
    }
    else
	DcmObject::printInfoLine( level, "(not loaded)" );
}


// ********************************


E_Condition DcmOtherByteOtherWord::alignValue(void)
{
    errorFlag = EC_Normal;
    if ( Tag->getEVR() != EVR_OW && Length != 0L)
    {
	Uint8 * bytes = NULL;
	bytes = (Uint8 *)getValue(fByteOrder);
	if (bytes && (Length & 1) != 0)
	{
	    bytes[Length] = 0;
	    Length++;
	}
    }
    return errorFlag;
}


// ********************************

void DcmOtherByteOtherWord::postLoadValue(void)
{
    this -> alignValue();
}

// ********************************

E_Condition DcmOtherByteOtherWord::put(const Uint8 * byteValue,
				       const unsigned long numBytes)
{
    errorFlag = EC_Normal;
    if (numBytes)
    {
	if (Tag->getEVR() != EVR_OW)
	{
	    if (byteValue)
	    {
		errorFlag = putValue(byteValue, sizeof(Uint8)*Uint32(numBytes));
		this -> alignValue();
	    }
	    else
		errorFlag = EC_CorruptedData;
	}
	else
	    errorFlag = EC_IllegalCall;
    }
    return errorFlag;
}


// ********************************


E_Condition DcmOtherByteOtherWord::put(const Uint16 * wordValue,
				       const unsigned long numWords)      
{
    errorFlag = EC_Normal;
    if (numWords)
    {
	if (Tag->getEVR() == EVR_OW)
	{
	    if (wordValue)
		errorFlag = putValue(wordValue, sizeof(Uint16)*Uint32(numWords));
	    else
		errorFlag = EC_CorruptedData;
	}
	else
	    errorFlag = EC_IllegalCall;
    }
    return errorFlag;
}


// ********************************


E_Condition DcmOtherByteOtherWord::put(const char * val)
{
    errorFlag = EC_Normal;
    if (val)
    {
	unsigned long vm = getVMFromString(val);
	const DcmEVR evr = Tag -> getEVR();
	Uint16 * wordField = NULL;
	Uint8 * byteField = NULL;

	if (evr == EVR_OW)
	    wordField = new Uint16[vm];
	else
	    byteField = new Uint8[vm];

	const char * s = val;
	Uint16 intVal = 0;
	    
	for(unsigned long i = 0; i < vm && errorFlag == EC_Normal; i++)
	{
	    const char * value = getFirstValueFromString(s);
	    if (value) 
	    {
		if (sscanf(value, "%hx", &intVal) != 1)
		    errorFlag = EC_CorruptedData;
		else if (evr != EVR_OW)
		    byteField[i] = Uint8(intVal);
		else
		    wordField[i] = Uint16(intVal);
		delete[] value;
	    }
	    else 
		errorFlag = EC_CorruptedData;
	}
	


	if (errorFlag == EC_Normal)
	{
	    if (evr != EVR_OW)
		errorFlag = this -> put(byteField, vm);
	    else
		errorFlag = this -> put(wordField, vm);
	}

	if (evr != EVR_OW)
	    delete[] byteField;
	else
	    delete[] wordField;
    }
    return errorFlag;
}


// ********************************


E_Condition DcmOtherByteOtherWord::get(Uint8 * & bytes)
{
    bytes = this -> getBytes();
    return errorFlag;
}

// ********************************


Uint8 * DcmOtherByteOtherWord::getBytes()
{
    errorFlag = EC_Normal;
    if ( Tag->getEVR() != EVR_OW )
    {
	Uint8 * value = (Uint8 *)this -> getValue();
	return value;
    }
    else
    {
	errorFlag = EC_IllegalCall;
	return NULL;
    }
}


// ********************************


E_Condition DcmOtherByteOtherWord::get(Uint16 * & words)
{
    words = this -> getWords();
    return errorFlag;
}

// ********************************


Uint16 * DcmOtherByteOtherWord::getWords(void)
{
    errorFlag = EC_Normal;
    if ( Tag->getEVR() == EVR_OW )
    {
	Uint16 * value = (Uint16 *)this -> getValue();
	return value;
    }
    else
    {
	errorFlag = EC_IllegalCall;
	return NULL;
    }
}


// ********************************


E_Condition DcmOtherByteOtherWord::verify(const BOOL autocorrect)
{
    errorFlag = EC_Normal;
    if (autocorrect)
	errorFlag = alignValue();
    return errorFlag;
}


// ********************************

E_Condition DcmOtherByteOtherWord::write(DcmStream & outStream,
					 const E_TransferSyntax oxfer,
					 const E_EncodingType enctype,
					 const E_GrpLenEncoding gltype)
{

    if (fTransferState == ERW_notInitialized)
	errorFlag = EC_IllegalCall;
    else
    {
	if (fTransferState == ERW_init)
	    this -> alignValue();
	errorFlag = DcmElement::write(outStream, oxfer, enctype, gltype);
    }
    return errorFlag;
}

// ********************************

/*
** CVS/RCS Log:
** $Log: dcvrobow.cc,v $
** Revision 1.5  1996-01-29 13:38:33  andreas
** - new put method for every VR to put value as a string
** - better and unique print methods
**
** Revision 1.4  1996/01/09 11:06:50  andreas
** New Support for Visual C++
** Correct problems with inconsistent const declarations
** Correct error in reading Item Delimitation Elements
**
** Revision 1.3  1996/01/05 13:27:51  andreas
** - changed to support new streaming facilities
** - unique read/write methods for file and block transfer
** - more cleanups
**
** Revision 1.2  1995/11/23 17:03:07  hewett
** Updated for loadable data dictionary.  Some cleanup (more to do).
**
**
*/
