/*
**
** Author: Andreas Barth    25.01.96
** Kuratorium OFFIS e.V.
**
** Module: dcvm.h
**
** Purpose:
** 	Functions to get the VM and values from a string
**
**
** Last Update:		$Author: meichel $
** Update Date:		$Date: 1996-03-26 09:59:20 $
** Source File:		$Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/include/Attic/dcvm.h,v $
** CVS/RCS Revision:	$Revision: 1.2 $
** Status:		$State: Exp $
**
** CVS/RCS Log at end of file
**
*/


#ifndef DCVM_H
#define DCVM_H

#include "osconfig.h"    /* make sure OS specific configuration is included first */

// get the number of values stored in string
unsigned long getVMFromString(const char * val);

// get first value stored in string, set the parameter to beginning of the
// next value
char * getFirstValueFromString(const char * & s);


#endif 

/*
** CVS/RCS Log:
** $Log: dcvm.h,v $
** Revision 1.2  1996-03-26 09:59:20  meichel
** corrected bug (deletion of const char *) which prevented compilation on NeXT
**
** Revision 1.1  1996/01/29 13:38:15  andreas
** - new put method for every VR to put value as a string
** - better and unique print methods
**
**
*/

