/*
 *
 *  Copyright (C) 1994-2001, OFFIS
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
 *  Author:  Gerd Ehlers
 *
 *  Purpose: Interface of class DcmTime
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2001-10-01 15:01:40 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/include/Attic/dcvrtm.h,v $
 *  CVS/RCS Revision: $Revision: 1.11 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#ifndef DCVRTM_H
#define DCVRTM_H

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include "dctypes.h"
#include "dcbytstr.h"



class DcmTime : public DcmByteString
{
  public:
    DcmTime(const DcmTag &tag, const Uint32 len = 0);
    DcmTime(const DcmTime& old);
    virtual ~DcmTime(void);

    DcmTime &operator=(const DcmTime &obj) { DcmByteString::operator=(obj); return *this; }

    virtual DcmEVR ident(void) const { return EVR_TM; }

    virtual OFCondition getOFString(
	    OFString & str,
	    const unsigned long pos,
	    OFBool normalize = OFTrue);

    virtual OFCondition getOFStringArray(
	    OFString & str,
	    OFBool normalize = OFTrue);

	/** set the element value to the current system time.
	 *  The DICOM TM format supported by this function is "HHMM[SS[.FFFFFF]]" where
	 *  the brackets enclose optional parts. If the current system time or parts of it
	 *  are unavailable the corresponding values are set to "0" and an error code is
	 *  returned.
	 *  @param seconds add optional seconds ("SS") if OFTrue
	 *  @param fraction add optional fractional part of a second (".FFFFFF") if OFTrue
	 *   (requires parameter 'seconds' to be also OFTrue)
	 *  @return EC_Normal upon success, an error code otherwise
	 */
    OFCondition setCurrentTime(
        const OFBool seconds = OFTrue,
        const OFBool fraction = OFFalse);

	/** get the current element value in ISO time format.
	 *  The ISO time format supported by this function is "HH:MM[:SS[.FFFFFF]]" where
	 *  the brackets enclose optional parts. Please note that the element value is
	 *  expected to be in valid DICOM TM format ("[HH[MM[SS[.FFFFFF]]]]"). If this function
	 *  fails the result variable 'formattedTime' is cleared automatically.
	 *  @param formattedTime reference to string variable where the result is stored
	 *  @param pos index of the element component in case of value multiplicity (0..vm-1)
	 *  @param seconds add optional seconds (":SS") if OFTrue
	 *  @param fraction add optional fractional part of a second (".FFFFFF") if OFTrue
	 *   (requires parameter 'seconds' to be also OFTrue)
	 *  @param createMissingPart if OFTrue create optional parts (seconds and/or fractional
	 *   part of a seconds) if absent in the element value
	 *  @return EC_Normal upon success, an error code otherwise
	 */
    OFCondition getISOFormattedTime(
        OFString &formattedTime,
        const unsigned long pos = 0,
        const OFBool seconds = OFTrue,
        const OFBool fraction = OFFalse,
        const OFBool createMissingPart = OFFalse);

    /* --- static helper functions --- */

	/** get the current system time.
	 *  The DICOM TM format supported by this function is "HHMM[SS[.FFFFFF]]" where
	 *  the brackets enclose optional parts. If the current system time or parts of it
	 *  are unavailable the corresponding values are set to "0" and an error code is
	 *  returned.
	 *  @param dicomTime reference to string variable where the result is stored
	 *  @param seconds add optional seconds ("SS") if OFTrue
	 *  @param fraction add optional fractional part of a second (".FFFFFF") if OFTrue
	 *   (requires parameter 'seconds' to be also OFTrue)
	 *  @return EC_Normal upon success, an error code otherwise
	 */
    static OFCondition getCurrentTime(
        OFString &dicomTime,
        const OFBool seconds = OFTrue,
        const OFBool fraction = OFFalse);

	/** get the specified DICOM time value in ISO format.
	 *  The ISO time format supported by this function is "HH:MM[:SS[.FFFFFF]]" where
	 *  the brackets enclose optional parts. Please note that the specified value is
	 *  expected to be in valid DICOM TM format ("[HH[MM[SS[.FFFFFF]]]]"). If this function
	 *  fails the result variable 'formattedTime' is cleared automatically.
	 *  @param dicomTime string value in DICOM TM format to be converted to ISO format
	 *  @param formattedTime reference to string variable where the result is stored
	 *  @param seconds add optional seconds (":SS") if OFTrue
	 *  @param fraction add optional fractional part of a second (".FFFFFF") if OFTrue
	 *   (requires parameter 'seconds' to be also OFTrue)
	 *  @param createMissingPart if OFTrue create optional parts (seconds and/or fractional
	 *   part of a seconds) if absent in the DICOM TM value
	 *  @return EC_Normal upon success, an error code otherwise
	 */
    static OFCondition getISOFormattedTimeFromString(
        const OFString &dicomTime,
        OFString &formattedTime,
        const OFBool seconds = OFTrue,
        const OFBool fraction = OFFalse,
        const OFBool createMissingPart = OFFalse);
};


#endif // DCVRTM_H

/*
** CVS/RCS Log:
** $Log: dcvrtm.h,v $
** Revision 1.11  2001-10-01 15:01:40  joergr
** Introduced new general purpose functions to get/set person names, date, time
** and date/time.
**
** Revision 1.10  2001/09/25 17:19:34  meichel
** Adapted dcmdata to class OFCondition
**
** Revision 1.9  2001/06/01 15:48:53  meichel
** Updated copyright header
**
** Revision 1.8  2000/03/08 16:26:26  meichel
** Updated copyright header.
**
** Revision 1.7  1999/03/31 09:25:08  meichel
** Updated copyright header in module dcmdata
**
** Revision 1.6  1998/11/12 16:47:56  meichel
** Implemented operator= for all classes derived from DcmObject.
**
** Revision 1.5  1997/09/11 15:13:17  hewett
** Modified getOFString method arguments by removing a default value
** for the pos argument.  By requiring the pos argument to be provided
** ensures that callers realise getOFString only gets one component of
** a multi-valued string.
**
** Revision 1.4  1997/08/29 08:32:45  andreas
** - Added methods getOFString and getOFStringArray for all
**   string VRs. These methods are able to normalise the value, i. e.
**   to remove leading and trailing spaces. This will be done only if
**   it is described in the standard that these spaces are not relevant.
**   These methods do not test the strings for conformance, this means
**   especially that they do not delete spaces where they are not allowed!
**   getOFStringArray returns the string with all its parts separated by \
**   and getOFString returns only one value of the string.
**   CAUTION: Currently getString returns a string with trailing
**   spaces removed (if dcmEnableAutomaticInputDataCorrection == OFTrue) and
**   truncates the original string (since it is not copied!). If you rely on this
**   behaviour please change your application now.
**   Future changes will ensure that getString returns the original
**   string from the DICOM object (NULL terminated) inclusive padding.
**   Currently, if you call getOF... before calling getString without
**   normalisation, you can get the original string read from the DICOM object.
**
** Revision 1.3  1996/01/05 13:23:10  andreas
** - changed to support new streaming facilities
** - more cleanups
** - merged read / write methods for block and file transfer
**
*/
