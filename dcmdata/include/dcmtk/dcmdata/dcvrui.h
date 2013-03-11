/*
 *
 *  Copyright (C) 1994-2013, OFFIS e.V.
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
 *  Author:  Gerd Ehlers, Andreas Barth
 *
 *  Purpose: Interface of class DcmUniqueIdentifier
 *
 */

#ifndef DCVRUI_H
#define DCVRUI_H

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmdata/dcbytstr.h"


/** a class representing the DICOM value representation 'Unique Identifier' (UI)
 */
class DCMTK_DCMDATA_EXPORT DcmUniqueIdentifier
  : public DcmByteString
{

  public:

    /** constructor.
     *  Create new element from given tag and length.
     *  @param tag DICOM tag for the new element
     *  @param len value length for the new element
     */
    DcmUniqueIdentifier(const DcmTag &tag,
                        const Uint32 len = 0);

    /** copy constructor
     *  @param old element to be copied
     */
    DcmUniqueIdentifier(const DcmUniqueIdentifier &old);

    /** destructor
     */
    virtual ~DcmUniqueIdentifier();

    /** assignment operator
     *  @param obj element to be assigned/copied
     *  @return reference to this object
     */
    DcmUniqueIdentifier &operator=(const DcmUniqueIdentifier &obj);

    /** clone method
     *  @return deep copy of this object
     */
    virtual DcmObject *clone() const
    {
      return new DcmUniqueIdentifier(*this);
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
     *  @return type identifier of this class (EVR_UI)
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

    /** print element to a stream.
     *  The output format of the value is a backslash separated sequence of string
     *  components. In case of a single component the UID number is mapped to the
     *  corresponding UID name (using "dcmFindNameOfUID()") if available. A "=" is
     *  used as a prefix to distinguish the UID name from the UID number.
     *  NB: This mapping of UID names only works for single-valued strings.
     *  @param out output stream
     *  @param flags optional flag used to customize the output (see DCMTypes::PF_xxx)
     *  @param level current level of nested items. Used for indentation.
     *  @param pixelFileName not used
     *  @param pixelCounter not used
     */
    virtual void print(STD_NAMESPACE ostream&out,
                       const size_t flags = 0,
                       const int level = 0,
                       const char *pixelFileName = NULL,
                       size_t *pixelCounter = NULL);

    /** get a copy of a particular string component
     *  @param stringVal variable in which the result value is stored
     *  @param pos index of the value in case of multi-valued elements (0..vm-1)
     *  @param normalize delete trailing NULL-byte(s) if OFTrue
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getOFString(OFString &stringVal,
                                    const unsigned long pos,
                                    OFBool normalize = OFTrue);

    /** set element value from the given character string.
     *  If the string starts with a "=" the subsequent characters are interpreted as a
     *  UID name and mapped to the corresponding UID number (using "dcmFindUIDFromName()")
     *  if possible. Otherwise the leading "=" is removed.
     *  NB: This mapping of UID names only works for single-valued input strings.
     *  @param stringVal input character string (possibly multi-valued)
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition putString(const char *stringVal);

    /** set element value from the given character string.
     *  If the string starts with a "=" the subsequent characters are interpreted as a
     *  UID name and mapped to the corresponding UID number (using "dcmFindUIDFromName()")
     *  if possible. Otherwise the leading "=" is removed.
     *  NB: This mapping of UID names only works for single-valued input strings.
     *  The length of the string has to be specified explicitly. The string can, therefore,
     *  also contain more than one NULL byte.
     *  @param stringVal input character string (possibly multi-valued)
     *  @param stringLen length of the string (number of characters without the trailing
     *    NULL byte)
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition putString(const char *stringVal,
                                  const Uint32 stringLen);

    /* --- static helper functions --- */

    /** check whether given string value conforms to the VR "UI" (Unique Identifier)
     *  and to the specified VM.
     *  @param value string value to be checked (possibly multi-valued)
     *  @param vm value multiplicity (according to the data dictionary) to be checked for.
     *    (See DcmElement::checkVM() for a list of valid values.)
     *  @return status of the check, EC_Normal if value is correct, an error code otherwise
     */
    static OFCondition checkStringValue(const OFString &value,
                                        const OFString &vm = "1-n");

  protected:

    /** convert currently stored string value to internal representation.
     *  It removes any leading, embedded and trailing space character and recomputes
     *  the string length. This manipulation attempts to correct problems with
     *  incorrectly encoded UIDs which have been observed in some images.
     *  @param length number of characters of the string value (optional)
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition makeMachineByteString(const Uint32 length = 0);
};


#endif // DCVRUI_H
