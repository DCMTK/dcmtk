/*
 *
 *  Copyright (C) 1994-2011, OFFIS e.V.
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  This software and supporting documentation were developed by
 *
 *    OFFIS e.V.
 *    R&D Division Health
 *    Escherweg 2
 *    D-26121 Oldenburg, Germany
 *
 *
 *  Module:  dcmdata
 *
 *  Author:  Gerd Ehlers, Joerg Riesmeier
 *
 *  Purpose: Interface of class DcmTime
 *
 */

#ifndef DCVRTM_H
#define DCVRTM_H

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmdata/dctypes.h"
#include "dcmtk/dcmdata/dcbytstr.h"
#include "dcmtk/ofstd/oftime.h"


/** a class representing the DICOM value representation 'Time' (TM)
 */
class DCMTK_DCMDATA_EXPORT DcmTime
  : public DcmByteString
{

  public:

    /** constructor.
     *  Create new element from given tag and length.
     *  @param tag DICOM tag for the new element
     *  @param len value length for the new element
     */
    DcmTime(const DcmTag &tag,
            const Uint32 len = 0);

    /** copy constructor
     *  @param old element to be copied
     */
    DcmTime(const DcmTime &old);

    /** destructor
     */
    virtual ~DcmTime();

    /** assignment operator
     *  @param obj element to be assigned/copied
     *  @return reference to this object
     */
    DcmTime &operator=(const DcmTime &obj);

    /** clone method
     *  @return deep copy of this object
     */
    virtual DcmObject *clone() const
    {
      return new DcmTime(*this);
    }

    /** Virtual object copying. This method can be used for DcmObject
     *  and derived classes to get a deep copy of an object. Internally
     *  the assignment operator is called if the given DcmObject parameter
     *  is of the same type as "this" object instance. If not, an error
     *  is returned. This function permits copying an object by value
     *  in a virtual way which therefore is different to just calling the
     *  assignment operator of DcmElement which could result in slicing
     *  the object.
     *  @param rhs - [in] The instance to copy from. Has to be of the same
     *                class type as "this" object
     *  @return EC_Normal if copying was successful, error otherwise
     */
    virtual OFCondition copyFrom(const DcmObject& rhs);

    /** get element type identifier
     *  @return type identifier of this class (EVR_TM)
     */
    virtual DcmEVR ident() const;

    /** check whether stored value conforms to the VR and to the specified VM
     *  @param vm value multiplicity (according to the data dictionary) to be checked for.
     *    (See DcmElement::checkVM() for a list of valid values.)
     *  @param oldFormat support old ACR/NEMA format if OFTrue (':' as a separator)
     *  @return status of the check, EC_Normal if value is correct, an error code otherwise
     */
    virtual OFCondition checkValue(const OFString &vm = "1-n",
                                   const OFBool oldFormat = OFFalse);

    /** get a copy of a particular string component
     *  @param stringValue variable in which the result value is stored
     *  @param pos index of the value in case of multi-valued elements (0..vm-1)
     *  @param normalize delete trailing spaces if OFTrue
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getOFString(OFString &stringValue,
                                    const unsigned long pos,
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
    OFCondition setCurrentTime(const OFBool seconds = OFTrue,
                               const OFBool fraction = OFFalse);

    /** set the element value to the given time
     *  @param timeValue time to be set (should be a valid time)
     *  @return EC_Normal upon success, an error code otherwise
     */
    OFCondition setOFTime(const OFTime &timeValue);

    /** get the current element value in OFTime format.
     *  Please note that the element value is expected to be in valid DICOM TM format
     *  ("[HH[MM[SS[.FFFFFF]]]]", "[HH[:MM[:SS[.FFFFFF]]]]" is also supported for reasons
     *  of backward compatibility). Since there is no time zone for the DICOM TM format
     *  local time is assumed (the time zone of 'timeValue' is set automatically).
     *  If this function fails the result variable 'timeValue' is cleared automatically.
     *  @param timeValue reference to OFTime variable where the result is stored
     *  @param pos index of the element component in case of value multiplicity (0..vm-1)
     *  @param supportOldFormat if OFTrue support old (prior V3.0) time format (see above)
     *  @return EC_Normal upon success, an error code otherwise
     */
    OFCondition getOFTime(OFTime &timeValue,
                          const unsigned long pos = 0,
                          const OFBool supportOldFormat = OFTrue);

    /** get the current element value in ISO time format.
     *  The ISO time format supported by this function is "HH:MM[:SS[.FFFFFF]]"
     *  where the brackets enclose optional parts. Please note that the element value
     *  is expected to be in valid DICOM TM format ("[HH[MM[SS[.FFFFFF]]]]",
     *  "[HH[:MM[:SS[.FFFFFF]]]]" is also supported for reasons of backward compatibility).
     *  If this function fails or the current element value is empty, the result variable
     *  'formattedTime' is cleared automatically. Please note that if the "Timezone Offset
     *  From UTC" attribute (0008,0201) is present, it applies to all TM attributes in the
     *  object. However, the time zone is not taken into account for the creation of the
     *  ISO formatted time. See also "getTimeZoneFromString()" below.
     *  @param formattedTime reference to string variable where the result is stored
     *  @param pos index of the element component in case of value multiplicity (0..vm-1)
     *  @param seconds add optional seconds (":SS") if OFTrue
     *  @param fraction add optional fractional part of a second (".FFFFFF") if OFTrue
     *   (requires parameter 'seconds' to be also OFTrue)
     *  @param createMissingPart if OFTrue create optional parts (seconds and/or fractional
     *   part of a seconds) if absent in the element value
     *  @param supportOldFormat if OFTrue support old (prior V3.0) time format (see above)
     *  @return EC_Normal upon success, an error code otherwise
     */
    OFCondition getISOFormattedTime(OFString &formattedTime,
                                    const unsigned long pos = 0,
                                    const OFBool seconds = OFTrue,
                                    const OFBool fraction = OFFalse,
                                    const OFBool createMissingPart = OFFalse,
                                    const OFBool supportOldFormat = OFTrue);

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
    static OFCondition getCurrentTime(OFString &dicomTime,
                                      const OFBool seconds = OFTrue,
                                      const OFBool fraction = OFFalse);

    /** get the specified OFTime value in DICOM format.
     *  The DICOM TM format supported by this function is "HHMM[SS[.FFFFFF]]" where
     *  the brackets enclose optional parts. If the current system time or parts of it
     *  are unavailable the corresponding values are set to "0" and an error code is
     *  returned.
     *  @param timeValue time to be converted to DICOM format
     *  @param dicomTime reference to string variable where the result is stored
     *  @param seconds add optional seconds ("SS") if OFTrue
     *  @param fraction add optional fractional part of a second (".FFFFFF") if OFTrue
     *   (requires parameter 'seconds' to be also OFTrue)
     *  @return EC_Normal upon success, an error code otherwise
     */
    static OFCondition getDicomTimeFromOFTime(const OFTime &timeValue,
                                              OFString &dicomTime,
                                              const OFBool seconds = OFTrue,
                                              const OFBool fraction = OFFalse);

    /** get the specified DICOM time value in OFTime format.
     *  Please note that the element value is expected to be in valid DICOM TM format
     *  ("[HH[MM[SS[.FFFFFF]]]]", "[HH[:MM[:SS[.FFFFFF]]]]" is also supported for reasons
     *  of backward compatibility). Since there is no time zone for the DICOM TM format
     *  local time is assumed (the time zone of 'timeValue' is set automatically).
     *  If this function fails the result variable 'timeValue' is cleared automatically.
     *  @param dicomTime string value in DICOM TM format to be converted to ISO format.
     *    An empty string is not regarded as valid input, since the time would be unknown.
     *  @param timeValue reference to OFTime variable where the result is stored
     *  @param supportOldFormat if OFTrue support old (prior V3.0) time format (see above)
     *  @return EC_Normal upon success, an error code otherwise
     */
    static OFCondition getOFTimeFromString(const OFString &dicomTime,
                                           OFTime &timeValue,
                                           const OFBool supportOldFormat = OFTrue);

    /** get the specified DICOM time value in ISO format.
     *  The ISO time format supported by this function is "HH:MM[:SS[.FFFFFF]]" where the
     *  brackets enclose optional parts. Please note that the specified value is expected
     *  to be in valid DICOM TM format ("[HH[MM[SS[.FFFFFF]]]]", "[HH[:MM[:SS[.FFFFFF]]]]"
     *  is also supported for reasons of backward compatibility). If this function fails
     *  or the specified DICOM time value is empty, the result variable 'formattedTime'
     *  is cleared automatically.
     *  @param dicomTime string value in DICOM TM format to be converted to ISO format
     *  @param formattedTime reference to string variable where the result is stored
     *  @param seconds add optional seconds (":SS") if OFTrue
     *  @param fraction add optional fractional part of a second (".FFFFFF") if OFTrue
     *   (requires parameter 'seconds' to be also OFTrue)
     *  @param createMissingPart if OFTrue create optional parts (seconds and/or fractional
     *   part of a seconds) if absent in the DICOM TM value
     *  @param supportOldFormat if OFTrue support old (prior V3.0) time format (see above)
     *  @return EC_Normal upon success, an error code otherwise
     */
    static OFCondition getISOFormattedTimeFromString(const OFString &dicomTime,
                                                     OFString &formattedTime,
                                                     const OFBool seconds = OFTrue,
                                                     const OFBool fraction = OFFalse,
                                                     const OFBool createMissingPart = OFFalse,
                                                     const OFBool supportOldFormat = OFTrue);

    /** get the specified DICOM time zone in number of hours format
     *  DICOM standard states that if the "Timezone Offset From UTC" attribute (0008,0201) is
     *  present it applies to all TM attributes in the object. This functions allows to convert
     *  the DICOM format ("&ZZZZ" where "&" is "+" or "-" and "ZZZZ" hours and minutes) to a
     *  floating point value, e.g. "+1.0" means plus one hour and "-2.5" minus two and a half
     *  hour, i.e. 2 hours and 30 minutes.
     *  The resulting 'timeZone' value can be used in conjuction with a OFTime object to convert
     *  the time to different time zones (e.g. to local time or UTC).
     *  @param dicomTimeZone string value in DICOM format ("&ZZZZ") to be converted
     *  @param timeZone reference to floating point variable where the resulting UTC offset is stored
     *  @return EC_Normal upon success, an error code otherwise
     */
    static OFCondition getTimeZoneFromString(const OFString &dicomTimeZone,
                                             double &timeZone);

    /** check whether given string value conforms to the VR "TM" (Time)
     *  and to the specified VM.
     *  @param value string value to be checked (possibly multi-valued)
     *  @param vm value multiplicity (according to the data dictionary) to be checked for.
     *    (See DcmElement::checkVM() for a list of valid values.)
     *  @param oldFormat support old ACR/NEMA time format if OFTrue (i.e. with ":" and "." delimiters)
     *  @return status of the check, EC_Normal if value is correct, an error code otherwise
     */
    static OFCondition checkStringValue(const OFString &value,
                                        const OFString &vm = "1-n",
                                        const OFBool oldFormat = OFFalse);
};


#endif // DCVRTM_H
