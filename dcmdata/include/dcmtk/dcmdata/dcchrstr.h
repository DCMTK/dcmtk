/*
 *
 *  Copyright (C) 1994-2015, OFFIS e.V.
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
 *  Author:  Andreas Barth
 *
 *  Purpose: Interface of class DcmCharString
 *
 */


#ifndef DCCHRSTR_H
#define DCCHRSTR_H

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

//
// This implementation does not support 16 bit character sets. Since 8 bit
// character sets are supported by the class DcmByteString the class
// DcmCharString is derived from DcmByteString without any extensions.
//
// If the extension for 16 bit character sets will be implemented, this class
// must be derived directly from DcmElement. This class is designed to support
// the value representations (LO, LT, PN, SH, ST, UC and UT). They are a problem
// because their value width (1, 2, ... bytes) is specified by the element
// SpecificCharacterSet (0008,0005) and an implementation must support
// different value widths that cannot be derived from the value representation.
//

#include "dcmtk/dcmdata/dcbytstr.h"


/** base class for DICOM elements with value representation LO, LT, PN, SH, ST, UC and UT
 */
class DCMTK_DCMDATA_EXPORT DcmCharString
  : public DcmByteString
{

  public:

    /** constructor.
     *  Create new element from given tag and length.
     *  @param tag DICOM tag for the new element
     *  @param len value length for the new element
     */
    DcmCharString(const DcmTag &tag,
                  const Uint32 len);

    /** copy constructor
     *  @param old element to be copied
     */
    DcmCharString(const DcmCharString &old);

    /** destructor
     */
    virtual ~DcmCharString();

    /** assignment operator
     *  @param obj element to be assigned/copied
     *  @return reference to this object
     */
    DcmCharString &operator=(const DcmCharString &obj);

    /** clone method
     *  @return deep copy of this object
     */
    virtual DcmObject *clone() const
    {
      return new DcmCharString(*this);
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

    /** check the currently stored string value.
     *  Checks every string component for the maximum length specified for the particular
     *  value representation.
     *  @param autocorrect correct value and value component length if OFTrue.
     *    NB: This parameter does currently nothing since it is unknown to this class
     *        whether a character consists of one or more bytes.  To be fixed in a future
     *        version when multi-byte character sets are (hopefully) supported.
     *  @return status, EC_Normal if value length is correct, an error code otherwise
     */
    virtual OFCondition verify(const OFBool autocorrect = OFFalse);

    /** check if this element contains non-ASCII characters. Please note that this check
     *  is pretty simple and only works for single-byte character sets that do include
     *  the 7-bit ASCII codes, e.g. for the ISO 8859 family. In other words: All character
     *  codes below 128 are considered to be ASCII codes and all others are considered to
     *  be non-ASCII.
     *  @param checkAllStrings not used in this class
     *  @return true if element contains non-ASCII characters, false otherwise
     */
    virtual OFBool containsExtendedCharacters(const OFBool checkAllStrings = OFFalse);

    /** check if this element is affected by SpecificCharacterSet
     *  @return always returns true since all derived VR classes are affected by the
     *    SpecificCharacterSet (0008,0005) element
     */
    virtual OFBool isAffectedBySpecificCharacterSet() const;

    /** convert this element value from the currently selected source character set to
     *  the currently selected destination character set
     *  @param converter character set converter to be used to convert the element value
     *  @return always returns EC_Normal, since nothing to do in this base class
     */
    virtual OFCondition convertCharacterSet(DcmSpecificCharacterSet &converter);

  protected:

    /** delimiter characters specifying when to switch back to the default character set
     *  (in case code extension techniques like ISO 2022 are used)
     *  @param characters delimiter characters to be used for character set conversion
     */
    void setDelimiterChars(const OFString &characters) { delimiterChars = characters; }

    /** get value of the SpecificCharacterSet element of the surrounding dataset/item
     *  @param charset reference to variable that will store the result value. The
     *    variable is not cleared in case of error!
     *  @return status, EC_Normal if successful (i.e. the element could be found),
     *    an error code otherwise. Typical error codes are:
     *    - EC_TagNotFound if the SpecificCharacterSet element could not be found
     *    - EC_CorruptedData if this object is not contained in a dataset/item
     */
    OFCondition getSpecificCharacterSet(OFString &charset);

  private:

    /// delimiter characters specifying when to switch back to the default character set
    /// (in case code extension techniques like ISO 2022 are used)
    OFString delimiterChars;
};


#endif // DCCHRSTR_H
