/*
**
** Author: Gerd Ehlers      01.05.94 -- First Creation
**         Andreas Barth    05.12.95 -- new Stream class, unique value field
** Kuratorium OFFIS e.V.
**
** Module: dcvris.cc
** 
** Purpose:
** Implementation of class DcmIntegerString
**
** Last Update:		$Author: andreas $
** Update Date:		$Date: 1997-05-30 06:45:00 $
** Source File:		$Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/libsrc/dcvris.cc,v $
** CVS/RCS Revision:	$Revision: 1.6 $
** Status:		$State: Exp $
**
** CVS/RCS Log at end of file
**
*/

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcvris.h"
#include "dcdebug.h"


// ********************************


DcmIntegerString::DcmIntegerString(const DcmTag &tag,
								   const Uint32 len)
: DcmByteString(tag, len)
{
Bdebug((5, "dcvris:DcmIntegerString::DcmIntegerString(DcmTag&,len=%ld)",
           len ));

    maxLength = 12;
Edebug(());

}


// ********************************


DcmIntegerString::DcmIntegerString( const DcmIntegerString& old )
: DcmByteString( old, EVR_IS )
{
Bdebug((5, "dcvris:DcmIntegerString::DcmIntegerString(DcmIntegerString&)" ));

    maxLength = 12;
Edebug(());

}


// ********************************


DcmIntegerString::~DcmIntegerString()
{
  Bdebug((5, "dcvris:DcmIntegerString::~DcmIntegerString()" ));
  Edebug(());

}


// ********************************

E_Condition DcmIntegerString::getSint32(Sint32 &val,
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
#if SIZEOF_LONG == 8
	      if (sscanf(str, "%d", &val) != 1)
#else
	      if (sscanf(str, "%ld", &val) != 1)
#endif
		l_error = EC_CorruptedData;
	    }
	  else
	    {
#if SIZEOF_LONG == 8
	      if (sscanf(str, "%d\\", &val) != 1)
#else
	      if (sscanf(str, "%ld\\", &val) != 1)
#endif
		l_error = EC_CorruptedData;
	    }
	}
    }
  return l_error;
}

// ********************************

/*
** CVS/RCS Log:
** $Log: dcvris.cc,v $
** Revision 1.6  1997-05-30 06:45:00  andreas
** - fixed scanf format problem leading to warnings on 64 bit machines.
**
** Revision 1.5  1997/05/12 09:57:02  andreas
** - warnings deletetd
**
** Revision 1.4  1997/05/12 07:38:27  andreas
** - new get-Methods for DcmDecimalString: getFloat64 and
**   DcmIntegerString: getSint32
**
** Revision 1.3  1996/01/05 13:27:49  andreas
** - changed to support new streaming facilities
** - unique read/write methods for file and block transfer
** - more cleanups
**
**
**
*/



