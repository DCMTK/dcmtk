/*
**
** Author: Gerd Ehlers      03.06.94 -- First Creation
**         Andreas Barth    07.12.95 -- new Stream class, unique value field
** Kuratorium OFFIS e.V.
**
** Module: dcvrulup.cc
**
** Purpose:
** Implementation of class DcmUnsignedLongOffset
**
** Last Update:		$Author: andreas $
** Update Date:		$Date: 1996-04-12 13:17:23 $
** Source File:		$Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/libsrc/dcvrulup.cc,v $
** CVS/RCS Revision:	$Revision: 1.6 $
** Status:		$State: Exp $
**
** CVS/RCS Log at end of file
**
*/

#include <stdio.h>
#include <string.h>
#include <iostream.h>

#include "dcvrulup.h"
#include "dcdebug.h"


// ********************************


DcmUnsignedLongOffset::DcmUnsignedLongOffset(const DcmTag &tag,
											 const Uint32 len)
: DcmUnsignedLong(tag, len)
{
Bdebug((5, "DcmUnsignedLongOffset::DcmUnsignedLongOffset(DcmTag&,"
           "len=%ld)", len ));
debug(( 8, "Object pointer this=0x%p", this ));

    nextRecord = (DcmObject*)NULL;
Edebug(());

}


// ********************************


DcmUnsignedLongOffset::DcmUnsignedLongOffset(const DcmUnsignedLongOffset& old)
: DcmUnsignedLong(old)
{
	Bdebug((5, "dcvrulup:DcmUnsignedLongOffset::DcmUnsignedLongOffset("
			"DcmUnsignedLongOffset&)" ));
	debug(( 8, "Object pointer this=0x%p", this ));

    if ( old.ident() == EVR_up ) 
		nextRecord = old.nextRecord;
	else 
	{
		nextRecord = (DcmObject*)NULL;
		errorFlag = EC_IllegalCall;
		cerr << "Warning: DcmUnsignedLongOffset: wrong use of Copy-Constructor"
			<< endl;
	}
	Edebug(());

}


// ********************************


DcmUnsignedLongOffset::~DcmUnsignedLongOffset()
{
Bdebug((5, "dcvrulup:DcmUnsignedLongOffset::~DcmUnsignedLongOffset()" ));
debug(( 8, "Object pointer this=0x%p", this ));
Edebug(());
}


// ********************************


DcmEVR DcmUnsignedLongOffset::ident() const
{
    return EVR_up;
}


// ********************************


void DcmUnsignedLongOffset::print(const int level)
{
    if (valueLoaded())
    {
	Uint32 * uintVals =  this -> get();

	if (!uintVals)
	    printInfoLine( level, "(no value available)" );
	else
	{
	    char *ch_words;
	    char *tmp = ch_words = new char[Length*14/sizeof(Uint32)+4];

	    for (unsigned long i=0; i<( Length/sizeof(Uint32) ); i++ )
	    {
		sprintf( tmp, "$%lu\\", (unsigned long)(*uintVals));
		tmp += strlen(tmp);
		uintVals++;
	    }
	    if ( Length > 0 )
		tmp--;
	    *tmp = '\0';
	    printInfoLine(level, ch_words);
	    delete ch_words;
	}
    }
    else
	printInfoLine( level, "(not loaded)" );
}


// ********************************


DcmObject* DcmUnsignedLongOffset::getNextRecord()
{
    errorFlag = EC_Normal;
    return nextRecord;
}


// ********************************


DcmObject* DcmUnsignedLongOffset::setNextRecord( DcmObject* record )
{
    errorFlag = EC_Normal;
    nextRecord = record;
    return record;
}


// ********************************


E_Condition DcmUnsignedLongOffset::clear(void)
{
	DcmUnsignedLong::clear();
    nextRecord = NULL;
    return errorFlag;
}


// ********************************


E_Condition DcmUnsignedLongOffset::verify(const BOOL autocorrect)
{
	errorFlag = DcmUnsignedLong::verify(autocorrect);
    if (errorFlag == EC_Normal && 
		Length != 0 && this->get() != NULL && *(this->get()) != 0 &&
		nextRecord == NULL)
		errorFlag = EC_CorruptedData;
    return errorFlag;
}


// ********************************


/*
** CVS/RCS Log:
** $Log: dcvrulup.cc,v $
** Revision 1.6  1996-04-12 13:17:23  andreas
** Minor changes to support DEC ALPHA and DEC MIPS
**
** Revision 1.5  1996/03/11 13:09:44  hewett
** Corrected ambiguous use of this->get(0).
**
** Revision 1.4  1996/01/09 11:06:50  andreas
** New Support for Visual C++
** Correct problems with inconsistent const declarations
** Correct error in reading Item Delimitation Elements
**
** Revision 1.3  1996/01/05 13:27:56  andreas
** - changed to support new streaming facilities
** - unique read/write methods for file and block transfer
** - more cleanups
**
**
**
*/
