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
 *  Author:  Andreas Barth
 *
 *  Purpose: functions to derive VM from string
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 1999-03-31 09:25:44 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/libsrc/dcvm.cc,v $
 *  CVS/RCS Revision: $Revision: 1.4 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "osconfig.h"
#include <string.h>
#include "dcvm.h"

// get the number of values stored in string
unsigned long getVMFromString(const char * val)
{
    unsigned long vm = 1;
    if (val == NULL)
	vm = 0; 
    else
    {
	for(const char * s = strchr(val, '\\'); s; s = strchr(++s, '\\'))
	    vm++;
    }
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
** Revision 1.4  1999-03-31 09:25:44  meichel
** Updated copyright header in module dcmdata
**
** Revision 1.3  1996/04/29 13:35:27  savonge
** -checking NULL string in getVMFromString
**
** Revision 1.2  1996/03/26 09:59:33  meichel
** corrected bug (deletion of const char *) which prevented compilation on NeXT
**
** Revision 1.1  1996/01/29 13:38:30  andreas
** - new put method for every VR to put value as a string
** - better and unique print methods
**
*/

