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
** Update Date:		$Date: 1997-07-21 08:11:43 $
** Source File:		$Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/libsrc/dcvrobow.cc,v $
** CVS/RCS Revision:	$Revision: 1.18 $
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
#include "dcdeftag.h"
#include "dcswap.h"
#include "dcstream.h"
#include "dcvm.h"
#include "dcdebug.h"



// ********************************


DcmOtherByteOtherWord::DcmOtherByteOtherWord(const DcmTag &tag,
					     const Uint32 len)
    : DcmElement(tag, len)
{
}


// ********************************


DcmOtherByteOtherWord::DcmOtherByteOtherWord( const DcmOtherByteOtherWord& old )
    : DcmElement( old )
{
    if (old.ident() != EVR_OW && old.ident() != EVR_OB && 
	old.ident() != EVR_ox && old.ident() != EVR_UNKNOWN && 
	old.ident() != EVR_UN && old.ident() != EVR_PixelData &&
	old.ident() != EVR_OverlayData)
    {
	errorFlag = EC_IllegalCall;
        cerr << "Warning: DcmOtherByteOtherWord: wrong use of Copy-Constructor"
             << endl;
    }
}


// ********************************


DcmOtherByteOtherWord::~DcmOtherByteOtherWord()
{
}


// ********************************


E_Condition DcmOtherByteOtherWord::setVR(DcmEVR vr)
{
    Tag.setVR(vr);
    return EC_Normal;
}



// ********************************


DcmEVR DcmOtherByteOtherWord::ident(void) const
{
    return Tag.getEVR();
}


// ********************************


void DcmOtherByteOtherWord::print(ostream & out, const OFBool showFullData,
				  const int level )
{
    if (this -> valueLoaded())
    {
	const DcmEVR evr = Tag.getEVR();
	Uint16 * wordValues = NULL;
	Uint8 * byteValues = NULL;

	if (evr == EVR_OW)
	    this -> getUint16Array(wordValues);
	else
	    this -> getUint8Array(byteValues);

	errorFlag = EC_Normal;
	if (wordValues || byteValues)
	{
	    char *ch_words = NULL;;
	    char *tmp = NULL;
	    Uint32 maxCount = 0; 
	    Uint32 vrLength = 0;
	    if (evr == EVR_OW)
	    {
		vrLength = Length/sizeof(Uint16);
		maxCount = 
		    !showFullData && DCM_OptPrintLineLength/5 < vrLength ? 
		    DCM_OptPrintLineLength/5 : vrLength;
		tmp = ch_words = new char[maxCount*5+6];
	    }
	    else
	    {
		vrLength = Length;
		maxCount = 
		    !showFullData && DCM_OptPrintLineLength/3 < Length ? 
		    DCM_OptPrintLineLength/3 : Length;
		tmp = ch_words = new char[maxCount*3+6];
	    }
		
	    if (tmp)
	    {
		for (unsigned int i=0; i<maxCount; i++)
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

		if (maxCount > 0 )
		    tmp--;
		*tmp = '\0';
		if (maxCount < vrLength)
		    strcat(tmp, "...");
		    
		printInfoLine(out, showFullData, level, ch_words );
		delete[] ch_words;
	    }
	    else
		errorFlag = EC_MemoryExhausted;
	}
	else
	    printInfoLine(out, showFullData, 
				     level, "(no value available)" );
    }
    else
	printInfoLine(out, showFullData, level, "(not loaded)" );
}


// ********************************


E_Condition DcmOtherByteOtherWord::alignValue(void)
{
    errorFlag = EC_Normal;
    if ( Tag.getEVR() != EVR_OW && Length != 0L)
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

E_Condition DcmOtherByteOtherWord::putUint8Array(const Uint8 * byteValue,
						 const unsigned long numBytes)
{
    errorFlag = EC_Normal;
    if (numBytes)
    {
	if (byteValue && Tag.getEVR() == EVR_OB)
	{
	    errorFlag = putValue(byteValue, sizeof(Uint8)*Uint32(numBytes));
	    this -> alignValue();
	}
	else
	    errorFlag = EC_CorruptedData;
    }
    else
	this -> putValue(NULL, 0);

    return errorFlag;
}


// ********************************


E_Condition DcmOtherByteOtherWord::putUint16Array(const Uint16 * wordValue,
						  const unsigned long numWords)      
{
    errorFlag = EC_Normal;
    if (numWords)
    {
	if (wordValue && Tag.getEVR() == EVR_OW)
	    errorFlag = putValue(wordValue, sizeof(Uint16)*Uint32(numWords));
	else
	    errorFlag = EC_CorruptedData;
    }
    else
	errorFlag = this -> putValue(NULL, 0);

    return errorFlag;
}


// ********************************


E_Condition DcmOtherByteOtherWord::putString(const char * val)
{
    errorFlag = EC_Normal;
    if (val && val[0] != 0)
    {
	unsigned long vm = getVMFromString(val);
	if (vm)
	{
	    const DcmEVR evr = Tag.getEVR();
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
		char * value = getFirstValueFromString(s);
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
		    errorFlag = this -> putUint8Array(byteField, vm);
		else
		    errorFlag = this -> putUint16Array(wordField, vm);
	    }

	    if (evr != EVR_OW)
		delete[] byteField;
	    else
		delete[] wordField;
	}
	else
	    this -> putValue(NULL, 0);
    }
    else
	this -> putValue(NULL, 0);
    return errorFlag;
}


// ********************************


E_Condition DcmOtherByteOtherWord::getUint8Array(Uint8 * & bytes)
{
    errorFlag = EC_Normal;
    if ( Tag.getEVR() != EVR_OW )
        bytes = (Uint8 *)this -> getValue();
    else
        errorFlag = EC_IllegalCall;
    return errorFlag;
}

// ********************************


E_Condition DcmOtherByteOtherWord::getUint16Array(Uint16 * & words)
{
    errorFlag = EC_Normal;
    if ( Tag.getEVR() == EVR_OW )
        words = (Uint16 *)this -> getValue();
    else
        errorFlag = EC_IllegalCall;
    return errorFlag;
}

// ********************************


E_Condition DcmOtherByteOtherWord::verify(const OFBool autocorrect)
{
    errorFlag = EC_Normal;
    if (autocorrect)
	errorFlag = alignValue();
    return errorFlag;
}


// ********************************

OFBool DcmOtherByteOtherWord::canWriteXfer(const E_TransferSyntax newXfer,
					 const E_TransferSyntax /*oldXfer*/)
{
    DcmXfer newXferSyn(newXfer);
    return Tag != DCM_PixelData || !newXferSyn.isEncapsulated();
}
    

// ********************************

E_Condition DcmOtherByteOtherWord::write(DcmStream & outStream,
					 const E_TransferSyntax oxfer,
					 const E_EncodingType enctype)
{

    if (fTransferState == ERW_notInitialized)
	errorFlag = EC_IllegalCall;
    else
    {
	if (fTransferState == ERW_init)
	    this -> alignValue();
	errorFlag = DcmElement::write(outStream, oxfer, enctype);
    }
    return errorFlag;
}

// ********************************

/*
** CVS/RCS Log:
** $Log: dcvrobow.cc,v $
** Revision 1.18  1997-07-21 08:11:43  andreas
** - Support for CP 14. PixelData and OverlayData can have VR OW or OB
**   (depending on the transfer syntax). New internal value
**   representation (only for ident()) for OverlayData.
** - New environment for encapsulated pixel representations. DcmPixelData
**   can contain different representations and uses codecs to convert
**   between them. Codecs are derived from the DcmCodec class. New error
**   codes are introduced for handling of representations. New internal
**   value representation (only for ident()) for PixelData
** - Replace all boolean types (BOOLEAN, CTNBOOLEAN, DICOM_BOOL, BOOL)
**   with one unique boolean type OFBool.
**
** Revision 1.17  1997/07/07 07:51:35  andreas
** - Changed type for Tag attribute in DcmObject from prointer to value
** - Enhanced (faster) byte swapping routine. swapIfNecessary moved from
**   a method in DcmObject to a general function.
**
** Revision 1.16  1997/07/03 15:10:15  andreas
** - removed debugging functions Bdebug() and Edebug() since
**   they write a static array and are not very useful at all.
**   Cdebug and Vdebug are merged since they have the same semantics.
**   The debugging functions in dcmdata changed their interfaces
**   (see dcmdata/include/dcdebug.h)
**
** Revision 1.15  1997/06/13 13:07:31  andreas
** - Corrected printing of OW values. The length of the output array was
**   computed incorrectly.
**
** Revision 1.14  1997/05/27 13:49:03  andreas
** - Add method canWriteXfer to class DcmObject and all derived classes.
**   This method checks whether it is possible to convert the original
**   transfer syntax to an new transfer syntax. The check is used in the
**   dcmconv utility to prohibit the change of a compressed transfer
**   syntax to a uncompressed.
**
** Revision 1.13  1997/05/22 16:54:20  andreas
** - Corrected wrong output length in print routine
**
** Revision 1.12  1997/05/16 08:31:28  andreas
** - Revised handling of GroupLength elements and support of
**   DataSetTrailingPadding elements. The enumeratio E_GrpLenEncoding
**   got additional enumeration values (for a description see dctypes.h).
**   addGroupLength and removeGroupLength methods are replaced by
**   computeGroupLengthAndPadding. To support Padding, the parameters of
**   element and sequence write functions changed.
**
** Revision 1.11  1997/04/18 08:17:20  andreas
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
** Revision 1.10  1997/03/26 17:15:59  hewett
** Added very preliminary support for Unknown VR (UN) described in
** Supplement 14.  WARNING: handling of unknown attributes with undefined
** length is not yet supported.
**
** Revision 1.9  1996/08/05 08:46:20  andreas
** new print routine with additional parameters:
**         - print into files
**         - fix output length for elements
** corrected error in search routine with parameter ESM_fromStackTop
**
** Revision 1.8  1996/05/06 10:22:47  meichel
** Copy constructor now handles case when VR=unknown.
**
** Revision 1.7  1996/04/16 16:05:24  andreas
** - better support und bug fixes for NULL element value
**
** Revision 1.6  1996/03/26 09:59:36  meichel
** corrected bug (deletion of const char *) which prevented compilation on NeXT
**
** Revision 1.5  1996/01/29 13:38:33  andreas
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
