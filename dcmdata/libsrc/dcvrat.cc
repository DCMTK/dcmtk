/*
**
** Author: Gerd Ehlers      27.04.94 -- First Creation
**         Andreas Barth    04.12.95 -- new Stream class, unique value field
** Kuratorium OFFIS e.V.
**
** Module: dcvrat.cc
**
** Purpose:
** Implementation of class DcmAttributeTag
**
** Last Update:		$Author: andreas $
** Update Date:		$Date: 1996-04-16 16:04:04 $
** Source File:		$Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/libsrc/dcvrat.cc,v $
** CVS/RCS Revision:	$Revision: 1.6 $
** Status:		$State: Exp $
**
** CVS/RCS Log at end of file
**
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
Bdebug((5, "dcvrat:DcmAttributeTag::DcmAttributeTag(DcmTag&,len=%ld)",
           len ));
debug(( 8, "Object pointer this=0x%p", this ));
Edebug(());
}


// ********************************


DcmAttributeTag::DcmAttributeTag( const DcmAttributeTag& old )
: DcmElement( old )
{
Bdebug((5, "dcvrat:DcmAttributeTag::DcmAttributeTag(DcmAttributeTag&)" ));
debug(( 8, "Object pointer this=0x%p", this ));

    if ( old.ident() != EVR_AT )

    {
        errorFlag = EC_IllegalCall;
        cerr << "Warning: DcmAttributeTag: wrong use of Copy-Constructor"
             << endl;
    }
Edebug(());

}


// ********************************


DcmAttributeTag::~DcmAttributeTag()
{
Bdebug((5, "dcvrat:DcmAttributeTag::~DcmAttributeTag()" ));
debug(( 8, "Object pointer this=0x%p", this ));

Edebug(());
}


// ********************************


void DcmAttributeTag::print(const int level)
{
    if (valueLoaded)
    {
	Uint16 * attributeTags = this -> get();

	if (!attributeTags)
	    printInfoLine( level, "(no value available)" );
	else
	{
	    char *ch_words;
	    char *tmp = ch_words = new char[ Length*6/sizeof(Uint16) + 9 ];
	    if (ch_words)
	    {
		for (unsigned long i=0; i<( Length/(2*sizeof(Uint16)) ); i++ )
		{
		    sprintf( tmp, "(%4.4x,%4.4x)\\", 
			     *attributeTags, *(attributeTags+1));
		    tmp += 12;
		    attributeTags += 2;
		}
			
		if ( Length > 0 )
		    tmp--;
			
		*tmp = '\0';
		printInfoLine( level, ch_words );
		delete ch_words;
	    }
	    else
		printInfoLine( level, "(no value available)" );
	}
    }
    else
	DcmObject::printInfoLine( level, "(not loaded)" );
}


// ********************************


unsigned long DcmAttributeTag::getVM()
{
    return (unsigned long)(Length / (2 * sizeof(Uint16)));
}


// ********************************


E_Condition DcmAttributeTag::put(const Uint16 * attrValue, 
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


E_Condition DcmAttributeTag::put(const DcmTagKey & attrTag)
{
    Uint16 attributeTag[2];
    attributeTag[0] = attrTag.getGroup();
    attributeTag[1] = attrTag.getElement();

    errorFlag = this -> putValue(attributeTag, 2*sizeof(Uint16));
	
    return errorFlag;
}


// ********************************


E_Condition DcmAttributeTag::put(const DcmTagKey &attrTag, 
				 const unsigned long position)
{
    Bdebug((2, "DcmAttributeTag::put(DcmTag&,=%ld)", position ));

    Uint16 attributeTag[2];
    attributeTag[0] = attrTag.getGroup();
    attributeTag[1] = attrTag.getElement();

    errorFlag = this -> changeValue(attributeTag, 
				    2*sizeof(Uint16)* Uint32(position), 
				    2*sizeof(Uint16));

    return errorFlag;
}


// ********************************


E_Condition DcmAttributeTag::put(const char * val)
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
		errorFlag = this -> put(field, vm);
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


E_Condition DcmAttributeTag::get(Uint16 * & attributeTags)
{
	attributeTags = (Uint16 *)this -> getValue();
	return errorFlag;
}


// ********************************


E_Condition DcmAttributeTag::get(DcmTag & attrTag, const unsigned long pos)
{
    Uint16 * attributeTags;
    errorFlag = this -> get(attributeTags);

    if (attributeTags && errorFlag == EC_Normal &&
	pos < Length / (2*sizeof(Uint16)))
    {
        DcmTag returnTag(attributeTags[2*pos], attributeTags[2*pos+1]);
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


Uint16 * DcmAttributeTag::get(void)
{
	Uint16 * attributeTags = (Uint16 *)this -> getValue();

	if (errorFlag == EC_Normal)
		return attributeTags;
	else
		return NULL;
}


// ********************************


DcmTag DcmAttributeTag::get(const unsigned long position)
{
    
    DcmTag attrTag;
    this -> get(attrTag, position);
    return attrTag;
}


// ********************************


E_Condition DcmAttributeTag::verify(const BOOL autocorrect)
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
** Revision 1.6  1996-04-16 16:04:04  andreas
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
**
**
*/
