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
 *  Purpose: class DcmAttributeTag
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 1999-03-31 09:25:47 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/libsrc/dcvrat.cc,v $
 *  CVS/RCS Revision: $Revision: 1.12 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include <stdio.h>
#include <string.h>
#include <iostream.h>

#include "dcvrat.h"
#include "dcvm.h"
#include "dcdebug.h"



// ********************************


DcmAttributeTag::DcmAttributeTag(const DcmTag &tag, const Uint32 len)
: DcmElement(tag, len)
{
}


// ********************************


DcmAttributeTag::DcmAttributeTag( const DcmAttributeTag& old )
: DcmElement( old )
{
    if ( old.ident() != EVR_AT )
    {
        errorFlag = EC_IllegalCall;
        cerr << "Warning: DcmAttributeTag: wrong use of Copy-Constructor"
             << endl;
    }
}


// ********************************


DcmAttributeTag::~DcmAttributeTag()
{
}


// ********************************


void DcmAttributeTag::print(ostream & out, const OFBool showFullData,
			    const int level)
{
    if (this -> valueLoaded())
    {
	Uint16 * attributeTags;
	this -> getUint16Array(attributeTags);

	if (!attributeTags)
	    printInfoLine(out, showFullData, level, "(no value available)" );
	else
	{
	    const Uint32 valueLength = Length/sizeof(Uint16);
	    const Uint32 maxCount = 
		!showFullData && 
		DCM_OptPrintLineLength/6 <  valueLength ?
		DCM_OptPrintLineLength/6 : valueLength;
	    char *ch_words;
	    char *tmp = ch_words = new char[maxCount*6 + 9 ];
	    if (ch_words)
	    {
		for (unsigned long i=0; i< maxCount/2; i++ )
		{
		    sprintf( tmp, "(%4.4x,%4.4x)\\", 
			     *attributeTags, *(attributeTags+1));
		    tmp += 12;
		    attributeTags += 2;
		}
			
		if (maxCount> 0)
		    tmp--;
			
		*tmp = '\0';

		if (maxCount < valueLength)
		    strcat(tmp, "...");
		printInfoLine(out, showFullData, level, ch_words );
		delete ch_words;
	    }
	    else
		printInfoLine(out, showFullData, level, "(no value available)" );
	}
    }
    else
	printInfoLine(out, showFullData, level, "(not loaded)");
}


// ********************************


unsigned long DcmAttributeTag::getVM()
{
    return (unsigned long)(Length / (2 * sizeof(Uint16)));
}


// ********************************


E_Condition DcmAttributeTag::putUint16Array(const Uint16 * attrValue, 
					    const unsigned long tagNum)
{
    errorFlag = EC_Normal;
    if (tagNum)
    {
	if (attrValue)
	    errorFlag = this -> putValue(attrValue, 2*sizeof(Uint16)*Uint32(tagNum));
	else
	    errorFlag = EC_CorruptedData;
    }
    else
	errorFlag = this -> putValue(NULL, 0);

    return errorFlag;
}


// ********************************


E_Condition DcmAttributeTag::putTagVal(const DcmTagKey &attrTag, 
				       const unsigned long position)
{
    Uint16 attributeTag[2];
    attributeTag[0] = attrTag.getGroup();
    attributeTag[1] = attrTag.getElement();

    errorFlag = this -> changeValue(attributeTag, 
				    2*sizeof(Uint16)* Uint32(position), 
				    2*sizeof(Uint16));
    return errorFlag;
}


// ********************************


E_Condition DcmAttributeTag::putString(const char * val)
{
    errorFlag = EC_Normal;
    if (val && val[0] != 0)
    {
	unsigned long vm = getVMFromString(val);
	if (vm)
	{
	    Uint16 * field = new Uint16[2*vm];
	    const char * s = val;
	    
	    for(unsigned long i = 0; i < 2*vm && errorFlag == EC_Normal; i+=2)
	    {
		char * value = getFirstValueFromString(s);
		if (!value || sscanf(value, "(%hx,%hx)", &field[i], &field[i+1]) != 2)
		    errorFlag = EC_CorruptedData;
		else if (value)
		    delete[] value;
	    }
	
	    if (errorFlag == EC_Normal)
		errorFlag = this -> putUint16Array(field, vm);
	    delete[] field;
	}
	else
	    this -> putValue(NULL,0);
    }
    else
	this -> putValue(NULL, 0);

    return errorFlag;
}


// ********************************


E_Condition DcmAttributeTag::getUint16Array(Uint16 * & attributeTags)
{
	attributeTags = (Uint16 *)this -> getValue();
	return errorFlag;
}


// ********************************


E_Condition DcmAttributeTag::getTagVal(DcmTagKey & attrTag, const unsigned long pos)
{
    Uint16 * attributeTags;
    errorFlag = this -> getUint16Array(attributeTags);

    if (attributeTags && errorFlag == EC_Normal &&
	pos < Length / (2*sizeof(Uint16)))
    {
        DcmTagKey returnTag(attributeTags[2*pos], attributeTags[2*pos+1]);
        attrTag = returnTag;
    }
    else if (attributeTags == NULL)
	errorFlag = EC_IllegalCall;
    else if (errorFlag == EC_Normal)
        errorFlag = EC_CorruptedData;
        
    if (errorFlag != EC_Normal)
    {
	DcmTag returnTag;
	attrTag = returnTag;
    }
    return errorFlag;
}

// ********************************

E_Condition DcmAttributeTag::verify(const OFBool autocorrect)
{
    errorFlag = EC_Normal;
    if ( Length % (2*sizeof(Uint16)) != 0 )
    {
		errorFlag = EC_CorruptedData;
		if (autocorrect)
		{
					    // auf gueltige Laenge kuerzen
            Length = Length - (Length % (2*sizeof(Uint16)));
		}
    }
    return errorFlag;
}


// ********************************


/*
** CVS/RCS Log:
** $Log: dcvrat.cc,v $
** Revision 1.12  1999-03-31 09:25:47  meichel
** Updated copyright header in module dcmdata
**
** Revision 1.11  1997/07/21 08:25:32  andreas
** - Replace all boolean types (BOOLEAN, CTNBOOLEAN, DICOM_BOOL, BOOL)
**   with one unique boolean type OFBool.
**
** Revision 1.10  1997/07/03 15:10:08  andreas
** - removed debugging functions Bdebug() and Edebug() since
**   they write a static array and are not very useful at all.
**   Cdebug and Vdebug are merged since they have the same semantics.
**   The debugging functions in dcmdata changed their interfaces
**   (see dcmdata/include/dcdebug.h)
**
** Revision 1.9  1997/04/18 08:17:19  andreas
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
** Revision 1.8  1996/08/05 08:46:18  andreas
** new print routine with additional parameters:
**         - print into files
**         - fix output length for elements
** corrected error in search routine with parameter ESM_fromStackTop
**
** Revision 1.7  1996/05/20 13:27:50  andreas
** correct minor bug in print routine
**
** Revision 1.6  1996/04/16 16:04:04  andreas
** - new put parameter DcmTagKey for DcmAttributeTag elements
** - better support for NULL element value
**
** Revision 1.5  1996/03/26 09:59:34  meichel
** corrected bug (deletion of const char *) which prevented compilation on NeXT
**
** Revision 1.4  1996/01/29 13:38:31  andreas
** - new put method for every VR to put value as a string
** - better and unique print methods
**
** Revision 1.3  1996/01/05 13:27:46  andreas
** - changed to support new streaming facilities
** - unique read/write methods for file and block transfer
** - more cleanups
**
*/
