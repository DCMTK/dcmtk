/*
**
** Author: Gerd Ehlers      01.05.94 
**         
** Kuratorium OFFIS e.V.
**
** Module: dcvrds.cc
**
** Purpose:
** Implementation of class DcmDecimalString
**
** Last Update:		$Author: andreas $
** Update Date:		$Date: 1997-07-03 10:22:45 $
** Source File:		$Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/libsrc/dcvrds.cc,v $
** CVS/RCS Revision:	$Revision: 1.5 $
** Status:		$State: Exp $
**
** CVS/RCS Log at end of file
**
*/

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcvrds.h"
#include "dcdebug.h"


// ********************************


DcmDecimalString::DcmDecimalString(const DcmTag &tag, const Uint32 len)
: DcmByteString(tag, len)
{
Bdebug((5, "dcvrds:DcmDecimalString::DcmDecimalString(DcmTag&,len=%ld)",
           len ));

    maxLength = 16;
Edebug(());

}


// ********************************


DcmDecimalString::DcmDecimalString( const DcmDecimalString &newDS )
: DcmByteString( newDS, EVR_DS )
{
Bdebug((5, "dcvrds:DcmDecimalString::DcmDecimalString(DcmDecimalString&)" ));

    maxLength = 16;
Edebug(());

}


// ********************************


DcmDecimalString::~DcmDecimalString()
{
Bdebug((5, "dcvrds:DcmDecimalString::~DcmDecimalString()" ));
Edebug(());

}


// ********************************

E_Condition DcmDecimalString::getFloat64(Float64 & val, 
					 const unsigned long pos)
{
  char *p;
  char *str = NULL;
  register unsigned long counter = pos;
  E_Condition l_error = getString(str);
  if (l_error == EC_Normal)
    {
      while (((p = strchr(str, '\\')) != NULL) && (counter > 0))
	{
	  str = p + 1;
	  counter--;
	}
      if (counter == 0)
	{
	  if (p == NULL)
	    {
	      if (sscanf(str, "%lf", &val) != 1)
		l_error = EC_CorruptedData;
	    }
	  else
	    {
	      if (sscanf(str, "%lf\\", &val) != 1)
		l_error = EC_CorruptedData;
	    }
	}
    }
  return l_error;
}

// ********************************

/*
** CVS/RCS Log:
** $Log: dcvrds.cc,v $
** Revision 1.5  1997-07-03 10:22:45  andreas
** - corrected Bug in DcmDecimalString::getFloat64 (Thanks to
**   Phil Liao <phil@eggroll.eeg.com>)
**
** Revision 1.4  1997/05/12 07:38:26  andreas
** - new get-Methods for DcmDecimalString: getFloat64 and
**   DcmIntegerString: getSint32
**
** Revision 1.3  1996/01/05 13:27:47  andreas
** - changed to support new streaming facilities
** - unique read/write methods for file and block transfer
** - more cleanups
**
**
**
*/


