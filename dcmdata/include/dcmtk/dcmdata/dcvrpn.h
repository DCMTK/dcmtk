/*
 *
 *  Copyright (C) 1994-2016, OFFIS e.V.
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
 *  Purpose: Interface of class DcmPersonName
 *
 */


#ifndef DCVRPN_H
#define DCVRPN_H

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmdata/dcchrstr.h"


/** a class representing the DICOM value representation 'Person Name' (PN)
 */
class DCMTK_DCMDATA_EXPORT DcmPersonName
  : public DcmCharString
{

  public:

    /** constructor.
     *  Create new element from given tag and length.
     *  @param tag DICOM tag for the new element
     *  @param len value length for the new element
     */
    DcmPersonName(const DcmTag &tag,
                  const Uint32 len = 0);

    /** copy constructor
     *  @param old element to be copied
     */
    DcmPersonName(const DcmPersonName &old);

    /** destructor
     */
    virtual ~DcmPersonName();

    /** assignment operator
     *  @param obj element to be assigned/copied
     *  @return reference to this object
     */
    DcmPersonName &operator=(const DcmPersonName &obj);

    /** clone method
     *  @return deep copy of this object
     */
    virtual DcmObject *clone() const
    {
      return new DcmPersonName(*this);
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
     *  @return type identifier of this class (EVR_PN)
     */
    virtual DcmEVR ident() const;

    /** check whether stored value conforms to the VR and to the specified VM
     *  @param vm value multiplicity (according to the data dictionary) to be checked for.
     *    (See DcmElement::checkVM() for a list of valid values.)
     *  @param oldFormat parameter not used for this VR (only for DA, TM)
     *  @return status of the check, EC_Normal if value is correct, an error code otherwise
     */
    virtual OFCondition checkValue(const OFString &vm = "1-n",
                                   const OFBool oldFormat = OFFalse);

    /** get a copy of a particular string component
     *  @param stringVal variable in which the result value is stored
     *  @param pos index of the value in case of multi-valued elements (0..vm-1)
     *  @param normalize delete leading and trailing spaces if OFTrue
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getOFString(OFString &stringVal,
                                    const unsigned long pos,
                                    OFBool normalize = OFTrue);

    /** get name components from the element value.
     *  The DICOM PN consists of up to three component groups separated by a "=". The
     *  supported format is "[CG0[=CG1[=CG2]]]" where the brackets enclose optional
     *  parts and CG0 is an alphabetic representation, CG1 an ideographic representation,
     *  and CG2 a phonetic representation of the name.
     *  Each component group may consist of up to five components separated by a "^".
     *  The format is "[lastName[^firstName[^middleName[^namePrefix[^nameSuffix]]]]";
     *  each component group and each component might be empty.
     *  If this function fails the result variables are cleared automatically. If the
     *  format is valid but does not comply with the above described scheme ("=" and "^")
     *  the full person name is returned in the 'lastName' variable.
     *  @param lastName reference to string variable where the "last name" is stored
     *  @param firstName reference to string variable where the "first name" is stored
     *  @param middleName reference to string variable where the "middle name" is stored
     *  @param namePrefix reference to string variable where the "name prefix" is stored
     *  @param nameSuffix reference to string variable where the "name suffix" is stored
     *  @param pos index of the element component in case of value multiplicity (0..vm-1)
     *  @param componentGroup index of the component group (0..2) to be used, see above
     *  @return EC_Normal upon success, an error code otherwise
     */
    OFCondition getNameComponents(OFString &lastName,
                                  OFString &firstName,
                                  OFString &middleName,
                                  OFString &namePrefix,
                                  OFString &nameSuffix,
                                  const unsigned long pos = 0,
                                  const unsigned int componentGroup = 0);

    /** get current element value as a formatted/readable name.
     *  The current element value is expected to be in DICOM PN format as described above.
     *  The output format is "[namePrefix][ firstName][ middleName][ lastName][, nameSuffix]";
     *  the delimiters (" " and ", ") are only inserted if required.
     *  If this function fails the result variable 'formattedName' is cleared automatically.
     *  @param formattedName reference to string variable where the result is stored
     *  @param pos index of the element component in case of value multiplicity (0..vm-1)
     *  @param componentGroup index of the component group (0..2) to be used, see above
     *  @return EC_Normal upon success, an error code otherwise
     */
    OFCondition getFormattedName(OFString &formattedName,
                                 const unsigned long pos = 0,
                                 const unsigned int componentGroup = 0);


    /** put element value from specified name components.
     *  The stored format is "[lastName[^firstName[^middleName[^namePrefix[^nameSuffix]]]]]",
     *  i.e. a DICOM Person Name (PN). Component groups are not (yet) supported.
     *  If this function fails the currently stored value is not modified.
     *  @param lastName reference to string variable where the "last name" is stored
     *  @param firstName reference to string variable where the "first name" is stored
     *  @param middleName reference to string variable where the "middle name" is stored
     *  @param namePrefix reference to string variable where the "name prefix" is stored
     *  @param nameSuffix reference to string variable where the "name suffix" is stored
     *  @return EC_Normal upon success, an error code otherwise
     */
    OFCondition putNameComponents(const OFString &lastName,
                                  const OFString &firstName,
                                  const OFString &middleName,
                                  const OFString &namePrefix,
                                  const OFString &nameSuffix);

    /** write object in XML format
     *  @param out output stream to which the XML document is written
     *  @param flags optional flag used to customize the output (see DCMTypes::XF_xxx)
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition writeXML(STD_NAMESPACE ostream &out,
                         const size_t flags = 0);

    /* --- static helper functions --- */

    /** get name components from specified DICOM person name.
     *  The DICOM PN consists of up to three component groups separated by a "=". The
     *  supported format is "[CG0[=CG1[=CG2]]]" where the brackets enclose optional
     *  parts and CG0 is an alphabetic representation, CG1 an ideographic representation,
     *  and CG2 a phonetic representation of the name.
     *  Each component group may consist of up to five components separated by a "^".
     *  The format is "[lastName[^firstName[^middleName[^namePrefix[^nameSuffix]]]]";
     *  each component group and each component might be empty.
     *  If this function fails the result variables are cleared automatically. If the
     *  format is valid but does not comply with the above described scheme ("=" and "^")
     *  the full person name is returned in the 'lastName' variable.
     *  @param dicomName string value in DICOM PN format to be split into components
     *  @param lastName reference to string variable where the "last name" is stored
     *  @param firstName reference to string variable where the "first name" is stored
     *  @param middleName reference to string variable where the "middle name" is stored
     *  @param namePrefix reference to string variable where the "name prefix" is stored
     *  @param nameSuffix reference to string variable where the "name suffix" is stored
     *  @param componentGroup index of the component group (0..2) to be used, see above
     *  @return EC_Normal upon success, an error code otherwise
     */
    static OFCondition getNameComponentsFromString(const OFString &dicomName,
                                                   OFString &lastName,
                                                   OFString &firstName,
                                                   OFString &middleName,
                                                   OFString &namePrefix,
                                                   OFString &nameSuffix,
                                                   const unsigned int componentGroup = 0);

    /** get single component group from specified DICOM person name.
     *  The DICOM PN consists of up to three component groups separated by a "=". The
     *  supported format is "[CG0[=CG1[=CG2]]]" where the brackets enclose optional
     *  parts and CG0 is an alphabetic representation, CG1 an ideographic representation,
     *  and CG2 a phonetic representation of the name. Each component group might be empty.
     *  The returned component group will contain component delimiters ("^") as they are
     *  stored within the very component group, i.e. superfluous component delimiters are
     *  not removed.
     *  @param allCmpGroups string value in DICOM PN format to component group from
     *  @param groupNo index of the component group (0..2) to be extracted
     *  @param cmpGroup reference to string variable where selected component group shall be
     *    stored
     *  @return EC_Normal upon success, an error code otherwise. Especially, if a component
     *    group exists (always for group 0, for group 1 and 2 depending on whether
     *    corresponding "=" is present) and is empty, EC_Normal is returned.
     */
    static OFCondition getComponentGroup(const OFString &allCmpGroups,
                                         const unsigned int groupNo,
                                         OFString &cmpGroup);

    /** get specified DICOM person name as a formatted/readable name.
     *  The specified 'dicomName' is expected to be in DICOM PN format as described above.
     *  The output format is "[namePrefix][ firstName][ middleName][ lastName][, nameSuffix]";
     *  the delimiters (" " and ", ") are only inserted if required.
     *  If this function fails the result variable 'formattedName' is cleared automatically.
     *  @param dicomName string value in DICOM PN format to be converted to readable format
     *  @param formattedName reference to string variable where the result is stored
     *  @param componentGroup index of the component group (0..2) to be used, see above
     *  @return EC_Normal upon success, an error code otherwise
     */
    static OFCondition getFormattedNameFromString(const OFString &dicomName,
                                                  OFString &formattedName,
                                                  const unsigned int componentGroup = 0);

    /** get formatted/readable name from specified name components.
     *  The output format is "[namePrefix][ firstName][ middleName][ lastName][, nameSuffix]";
     *  the delimiters (" " and ", ") are only inserted if required.
     *  If this function fails the result variable 'formattedName' is cleared automatically.
     *  @param lastName reference to string variable where the "last name" is stored
     *  @param firstName reference to string variable where the "first name" is stored
     *  @param middleName reference to string variable where the "middle name" is stored
     *  @param namePrefix reference to string variable where the "name prefix" is stored
     *  @param nameSuffix reference to string variable where the "name suffix" is stored
     *  @param formattedName reference to string variable where the result is stored
     *  @return always returns EC_Normal
     */
    static OFCondition getFormattedNameFromComponents(const OFString &lastName,
                                                      const OFString &firstName,
                                                      const OFString &middleName,
                                                      const OFString &namePrefix,
                                                      const OFString &nameSuffix,
                                                      OFString &formattedName);

    /** get DICOM Person Name (PN) from specified name components.
     *  The output format is "[lastName[^firstName[^middleName[^namePrefix[^nameSuffix]]]]]".
     *  Component groups are not (yet) supported.
     *  If this function fails the result variable 'dicomName' is cleared automatically.
     *  @param lastName reference to string variable where the "last name" is stored
     *  @param firstName reference to string variable where the "first name" is stored
     *  @param middleName reference to string variable where the "middle name" is stored
     *  @param namePrefix reference to string variable where the "name prefix" is stored
     *  @param nameSuffix reference to string variable where the "name suffix" is stored
     *  @param dicomName reference to string variable where the result is stored
     *  @return always returns EC_Normal
     */
    static OFCondition getStringFromNameComponents(const OFString &lastName,
                                                   const OFString &firstName,
                                                   const OFString &middleName,
                                                   const OFString &namePrefix,
                                                   const OFString &nameSuffix,
                                                   OFString &dicomName);

    /** check whether given string value conforms to the VR "PN" (Person Name)
     *  and to the specified VM.
     *  @param value string value to be checked (possibly multi-valued)
     *  @param vm value multiplicity (according to the data dictionary) to be checked for.
     *    (See DcmElement::checkVM() for a list of valid values.)
     *  @param charset character set (according to the value of the SpecificCharacterSet
     *    element) to be used for checking the string value. The default is ASCII (7-bit).
     *    Currently, the VR checker only supports ASCII (ISO_IR 6) and Latin-1 (ISO_IR 100).
     *    All other values disable the check of the value representation, e.g. "UNKNOWN".
     *  @return status of the check, EC_Normal if value is correct, an error code otherwise
     */
    static OFCondition checkStringValue(const OFString &value,
                                        const OFString &vm = "1-n",
                                        const OFString &charset = "");
};


#endif // DCVRPN_H
