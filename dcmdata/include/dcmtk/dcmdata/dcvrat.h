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
 *  Purpose: Interface of class DcmAttributeTag
 *
 */


#ifndef DCVRAT_H
#define DCVRAT_H

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmdata/dcelem.h"


/** a class representing the DICOM value representation 'Attribute Tag' (AT)
 */
class DCMTK_DCMDATA_EXPORT DcmAttributeTag
  : public DcmElement
{

  public:

    /** constructor.
     *  Create new element from given tag and length.
     *  @param tag DICOM tag for the new element
     *  @param len value length for the new element
     */
    DcmAttributeTag(const DcmTag &tag,
                    const Uint32 len = 0);

    /** copy constructor
     *  @param old element to be copied
     */
    DcmAttributeTag(const DcmAttributeTag &old);

    /** destructor
     */
    virtual ~DcmAttributeTag();

    /** assignment operator
     *  @param obj element to be assigned/copied
     *  @return reference to this object
     */
    DcmAttributeTag &operator=(const DcmAttributeTag &obj);

    /** comparison operator that compares the normalized value of this object
     *  with a given object of the same type. The tag of the element is also
     *  considered as the first component that is compared, followed by the
     *  object types (VR, i.e. DCMTK'S EVR) and the comparison of all value
     *  components of the object, preferrably in the order declared in the
     *  object (if applicable).
     *  @param  rhs the right hand side of the comparison
     *  @return 0 if the object values are equal.
     *          -1 if either the value of the  first component that does not match
     *          is lower in this object than in rhs, or all compared components match
     *          but this object has fewer components than rhs. Also returned if rhs
     *          cannot be casted to this object type.
     *          1 if either the value of the first component that does not match
     *          is greater in this object than in rhs object, or all compared
     *          components match but the this component is longer.
     */
    virtual int compare(const DcmElement& rhs) const;


    /** clone method
     *  @return deep copy of this object
     */
    virtual DcmObject *clone() const
    {
      return new DcmAttributeTag(*this);
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
     *  @return type identifier of this class (EVR_AT)
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

    /** get value multiplicity
     *  @return number of tag value pairs (group,element)
     */
    virtual unsigned long getVM();

    /** print element to a stream.
     *  The output format of the value is a backslash separated sequence of group and
     *  element value pairs, e.g. "(0008,0020)\(0008,0030)"
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

    /** write object in XML format
     *  @param out output stream to which the XML document is written
     *  @param flags optional flag used to customize the output (see DCMTypes::XF_xxx)
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition writeXML(STD_NAMESPACE ostream &out,
                         const size_t flags = 0);

    /** get particular tag value
     *  @param tagVal reference to result variable (cleared in case of error)
     *  @param pos index of the value to be retrieved (0..vm-1)
     *  @return status status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getTagVal(DcmTagKey &tagVal,
                                  const unsigned long pos = 0);

    /** get reference to stored integer data.
     *  The array entries with an even-numbered index contain the group numbers
     *  and the odd entries contain the element numbers (see "putUint16Array()").
     *  The number of entries is twice as large as the return value of "getVM()".
     *  @param uintVals reference to result variable
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getUint16Array(Uint16 *&uintVals);

    /** get specified value as a character string.
     *  The output format is "(gggg,eeee)" where "gggg" is the hexa-decimal group
     *  number and "eeee" the hexa-decimal element number of the attribute tag.
     *  @param stringVal variable in which the result value is stored
     *  @param pos index of the value in case of multi-valued elements (0..vm-1)
     *  @param normalize not used
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getOFString(OFString &stringVal,
                                    const unsigned long pos,
                                    OFBool normalize = OFTrue);

    /** set particular tag value
     *  @param tagVal tag value to be set
     *  @param pos index of the value to be set (0 = first position)
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition putTagVal(const DcmTagKey &tagVal,
                                  const unsigned long pos = 0);

    /** set element value to given integer array data.
     *  The array entries with an even-numbered index are expected to contain the
     *  group numbers and the odd entries to contain the element numbers, e.g.
     *  {0x0008, 0x0020, 0x0008, 0x0030}. This function uses the same format as
     *  "getUint16Array()".
     *  @param uintVals unsigned integer data to be set
     *  @param numUints number of integer values to be set (should be even)
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition putUint16Array(const Uint16 *uintVals,
                                       const unsigned long numUints);

    /** set element value from the given character string.
     *  The input string is expected to be a backslash separated sequence of
     *  attribute tags, e.g. "(0008,0020)\(0008,0030)". This is the same format
     *  as used by "print()".
     *  @param stringVal input character string
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition putString(const char *stringVal);

    /** set element value from the given character string.
     *  The input string is expected to be a backslash separated sequence of
     *  attribute tags, e.g. "(0008,0020)\(0008,0030)". This is the same format
     *  as used by "print()".
     *  The length of the string has to be specified explicitly. The string can, therefore,
     *  also contain more than one NULL byte.
     *  @param stringVal input character string
     *  @param stringLen length of the string (number of characters without the trailing
     *    NULL byte)
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition putString(const char *stringVal,
                                  const Uint32 stringLen);

    /** check the currently stored element value
     *  @param autocorrect correct value length if OFTrue
     *  @return status, EC_Normal if value length is correct, an error code otherwise
     */
    virtual OFCondition verify(const OFBool autocorrect = OFFalse);

    /* --- static helper functions --- */

    /** check whether given string value conforms to the VR "AT" (Attribute Tag)
     *  and to the specified VM.
     *  @param value string value to be checked (possibly multi-valued)
     *  @param vm value multiplicity (according to the data dictionary) to be checked for.
     *    (See DcmElement::checkVM() for a list of valid values.)
     *  @return status of the check, EC_Normal if value is correct, an error code otherwise
     */
    static OFCondition checkStringValue(const OFString &value,
                                        const OFString &vm = "1-n");
};


#endif // DCVRAT_H
