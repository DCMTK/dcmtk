/*
 *
 *  Copyright (C) 2019, OFFIS e.V.
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
 *  Author:  Joerg Riesmeier
 *
 *  Purpose: Interface of class DcmSigned64bitVeryLong
 *
 */


#ifndef DCVRSV_H
#define DCVRSV_H

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmdata/dcelem.h"


/** a class representing the DICOM value representation 'Signed 64-bit Very Long' (SV)
 */
class DCMTK_DCMDATA_EXPORT DcmSigned64bitVeryLong
  : public DcmElement
{

 public:

    // Make friend with DcmItem which requires access to protected
    // constructor allowing construction using an explicit value length.
    friend class DcmItem;

    /** constructor.
     *  Create new element from given tag.
     *  @param tag DICOM tag for the new element
     */
    DcmSigned64bitVeryLong(const DcmTag &tag);

    /** copy constructor
     *  @param old element to be copied
     */
    DcmSigned64bitVeryLong(const DcmSigned64bitVeryLong &old);

    /** destructor
     */
    virtual ~DcmSigned64bitVeryLong();

    /** assignment operator
     *  @param obj element to be assigned/copied
     *  @return reference to this object
     */
    DcmSigned64bitVeryLong &operator=(const DcmSigned64bitVeryLong &obj);

    /** comparison operator that compares the normalized value of this object
     *  with a given object of the same type. The tag of the element is also
     *  considered as the first component that is compared, followed by the
     *  object types (VR, i.e. DCMTK'S EVR) and the comparison of all value
     *  components of the object, preferably in the order declared in the
     *  object (if applicable).
     *  @param  rhs the right hand side of the comparison
     *  @return 0 if the object values are equal.
     *    -1 if this element has fewer components than the rhs element.
     *    Also -1 if the value of the first component that does not match
     *    is lower in this object than in rhs. Also returned if rhs
     *    cannot be casted to this object type or both objects are of
     *    different VR (i.e. the DcmEVR returned by the element's ident()
     *    call are different).
     *    1 if either this element has more components than the rhs element, or
     *    if the first component that does not match is greater in this object than
     *    in rhs object.
     */
    virtual int compare(const DcmElement& rhs) const;

    /** clone method
     *  @return deep copy of this object
     */
    virtual DcmObject *clone() const
    {
      return new DcmSigned64bitVeryLong(*this);
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
     *  @return type identifier of this class (EVR_SV)
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
     *  @return number of values in this element
     */
    virtual unsigned long getVM();

    /** get number of values stored in this element
     *  @return number of values in this element
     */
    virtual unsigned long getNumberOfValues();

    /** print element to a stream.
     *  The output format of the value is a backslash separated sequence of numbers.
     *  @param out output stream
     *  @param flags optional flag used to customize the output (see DCMTypes::PF_xxx)
     *  @param level current level of nested items. Used for indentation.
     *  @param pixelFileName not used
     *  @param pixelCounter not used
     */
    virtual void print(STD_NAMESPACE ostream &out,
                       const size_t flags = 0,
                       const int level = 0,
                       const char *pixelFileName = NULL,
                       size_t *pixelCounter = NULL);

    /** get particular integer value
     *  @param sintVal reference to result variable (cleared in case of error)
     *  @param pos index of the value to be retrieved (0..vm-1)
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getSint64(Sint64 &sintVal,
                                  const unsigned long pos = 0);

    /** get reference to stored integer data.
     *  The number of entries can be determined by "getVM()".
     *  @param sintVals reference to result variable
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getSint64Array(Sint64 *&sintVals);

    /** get particular value as a character string
     *  @param stringVal variable in which the result value is stored
     *  @param pos index of the value in case of multi-valued elements (0..vm-1)
     *  @param normalize not used
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getOFString(OFString &stringVal,
                                    const unsigned long pos,
                                    OFBool normalize = OFTrue);

    /** set particular element value to given integer
     *  @param sintVal signed integer value to be set
     *  @param pos index of the value to be set (0 = first position)
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition putSint64(const Sint64 sintVal,
                                  const unsigned long pos = 0);

    /** set element value to given integer array data
     *  @param sintVals signed integer data to be set
     *  @param numSints number of integer values to be set
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition putSint64Array(const Sint64 *sintVals,
                                       const unsigned long numSints);

    /** set element value from the given character string.
     *  The input string is expected to be a backslash separated sequence of
     *  numeric characters, e.g. "-333\-22\-1\0\1\22\333".
     *  @param stringVal input character string
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition putString(const char *stringVal);

    /** set element value from the given character string.
     *  The input string is expected to be a backslash separated sequence of
     *  numeric characters, e.g. "-333\-22\-1\0\1\22\333".
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

  protected:

    /** constructor. Create new element from given tag and length.
     *  Only reachable from friend classes since construction with
     *  length different from 0 leads to a state with length being set but
     *  the element's value still being uninitialized. This can lead to crashes
     *  when the value is read or written. Thus the method calling this
     *  constructor with length > 0 must ensure that the element's value is
     *  explicitly initialized, too.
     *  @param tag DICOM tag for the new element
     *  @param len value length for the new element
     */
    DcmSigned64bitVeryLong(const DcmTag &tag,
                           const Uint32 len);
};


#endif // DCVRSV_H
