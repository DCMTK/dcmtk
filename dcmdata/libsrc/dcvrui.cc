/*
**
** Author: Gerd Ehlers      01.05.94 -- First Creation
**         Andreas Barth    07.12.95 -- new Stream class, unique value field
** Kuratorium OFFIS e.V.
**
** Module: dcvrui.cc
** 
** Purpose:
** Implementation of class DcmUniqueIdentifier
**
** Last Update:		$Author: hewett $
** Update Date:		$Date: 1996-05-31 09:09:51 $
** Source File:		$Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/libsrc/dcvrui.cc,v $
** CVS/RCS Revision:	$Revision: 1.6 $
** Status:		$State: Exp $
**
** CVS/RCS Log at end of file
**
*/

#include <string.h>
#include <ctype.h>
#include "dcvrui.h"
#include "dcuid.h"
#include "dcdebug.h"


// ********************************


DcmUniqueIdentifier::DcmUniqueIdentifier(const DcmTag &tag,
                                         const Uint32 len)
: DcmByteString(tag, len)
{
Bdebug((5, "dcvrui:DcmUniqueIdentifier::DcmUniqueIdentifier"
           "(DcmTag&,len=%ld)", len ));

    paddingChar = '\0';
    maxLength = 64;
Edebug(());

}


// ********************************


DcmUniqueIdentifier::DcmUniqueIdentifier( const DcmUniqueIdentifier& old )
: DcmByteString( old, EVR_UI )
{
Bdebug((5, "dcvrui:DcmUniqueIdentifier::DcmUniqueIdentifier"
           "(DcmUniqueIdentifier&)" ));

    paddingChar = '\0';
    maxLength = 64;
Edebug(());

}


// ********************************


DcmUniqueIdentifier::~DcmUniqueIdentifier()
{
Bdebug((5, "dcvrui:DcmUniqueIdentifier::~DcmUniqueIdentifier()" ));
Edebug(());

}


// ********************************


void DcmUniqueIdentifier::print(const int level)
{
    if (this -> valueLoaded())
    {
	const char * uid = this -> get();
	if (uid)
	{
	    const char* symbol = dcmFindNameOfUID(uid);
	    char *tmp = NULL;

	    if ( symbol && *symbol != '\0' )
	    {
		tmp = new char[ strlen(symbol) + 3 ];
		tmp[0] = '=';
		strcpy( tmp+1, symbol );
	    }
	    else
	    {
		tmp = new char[ Length + 4 ];
		tmp[0] = '[';
		strncpy( tmp+1, uid, (int)Length );
		tmp[ Length + 1 ] = '\0';
		size_t t_len = strlen( tmp+1 );
		tmp[ t_len + 1 ] = ']';
		tmp[ t_len + 2 ] = '\0';
	    }
	    printInfoLine( level, tmp );
	    delete tmp;
	}
	else
	    printInfoLine( level, "(no value available)" );
    }
    else
	printInfoLine( level, "(not loaded)" );
}


// ********************************

E_Condition DcmUniqueIdentifier::put(const char * value)
{
    const char * uid = value;
    if (value && value[0] == '=')
	uid = dcmFindUIDFromName(&value[1]);

    return DcmByteString::put(uid);
}

// ********************************

E_Condition DcmUniqueIdentifier::makeMachineByteString(void)
{
    char * value = (char *)this -> getValue();
    if (value) {
    	int len = strlen(value);

	/*
	** Remove any leading, embedded, or trailing white space.
	** This manipulation attempts to correct problems with 
	** incorrectly encoded UIDs which have been observed in
	** some images.
	*/
	int k = 0;
	int i = 0;
	for (i=0; i<len; i++) {
	    if (!isspace(value[i])) {
		value[k] = value[i];
		k++;
	    }
	}
	value[k] = '\0';
    }

    return DcmByteString::makeMachineByteString();
}

// ********************************

/*
** CVS/RCS Log:
** $Log: dcvrui.cc,v $
** Revision 1.6  1996-05-31 09:09:51  hewett
** Unique Identifiers which are incorrectly encoded with leading, embedded
** or trailing white space are now corrected.
**
** Revision 1.5  1996/05/30 17:19:33  hewett
** Added a makeMachineByteString() method to strip and trailing whitespace
** from a UID.
**
** Revision 1.4  1996/01/29 13:38:34  andreas
** - new put method for every VR to put value as a string
** - better and unique print methods
**
**
*/


