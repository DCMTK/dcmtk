/*
** Author: Andreas Barth    25.01.96
** Kuratorium OFFIS e.V.
**
** Module: dcvm.cc
**
** Purpose:
** 	Functions to get the VM and values from a string
**
**
** Last Update:		$Author: meichel $
** Update Date:		$Date: 1996-03-26 09:59:33 $
** Source File:		$Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/libsrc/dcvm.cc,v $
** CVS/RCS Revision:	$Revision: 1.2 $
** Status:		$State: Exp $
**
** CVS/RCS Log at end of file
**
*/


#include "osconfig.h"
#include <string.h>
#include "dcvm.h"

// get the number of values stored in string
unsigned long getVMFromString(const char * val)
{
    unsigned long vm = 1;
   
    for(const char * s = strchr(val, '\\'); s; s = strchr(++s, '\\'))
	vm++;

    return vm;
}

// get first value stored in string, set the parameter to beginning of the
// next value
char * getFirstValueFromString(const char * & s)
{
    if (s)
    {
	const char * p = strchr(s, '\\');
	const char * backup = s;
	size_t len = 0;
	if (p)
	{
	    len = p-s;
	    s += (len+1);
	}
	else
	{
	    len = strlen(s);
	    s = NULL;
	}

	char * result = new char[len+1];
	strncpy(result, backup, len);
	result[len] = '\0';
	return result;
    }
    else
	return NULL;
}


/*
** CVS/RCS Log:
** $Log: dcvm.cc,v $
** Revision 1.2  1996-03-26 09:59:33  meichel
** corrected bug (deletion of const char *) which prevented compilation on NeXT
**
** Revision 1.1  1996/01/29 13:38:30  andreas
** - new put method for every VR to put value as a string
** - better and unique print methods
**
**
*/

