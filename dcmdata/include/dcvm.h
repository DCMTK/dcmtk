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
 *  Update Date:      $Date: 1999-03-31 09:24:53 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/include/Attic/dcvm.h,v $
 *  CVS/RCS Revision: $Revision: 1.3 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
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
** Revision 1.3  1999-03-31 09:24:53  meichel
** Updated copyright header in module dcmdata
**
** Revision 1.2  1996/03/26 09:59:20  meichel
** corrected bug (deletion of const char *) which prevented compilation on NeXT
**
** Revision 1.1  1996/01/29 13:38:15  andreas
** - new put method for every VR to put value as a string
** - better and unique print methods
**
*/

