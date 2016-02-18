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
 *  Author:  Gerd Ehlers, Andreas Barth
 *
 *  Purpose: Interface of class DcmByteString
 *
 */


#ifndef DCBYTSTR_H
#define DCBYTSTR_H

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmdata/dctypes.h"
#include "dcmtk/dcmdata/dcelem.h"
#include "dcmtk/ofstd/ofstring.h"


// include this file in doxygen documentation

/** @file dcbytstr.h
 *  @brief base class for all DICOM value representations storing a character string
 */

/** This flag defines whether the VR checker is actually used by the various
 *  checkStringValue() methods.  Since this checker is currently limited to
 *  ASCII and Latin-1, and the detection of the character set might fail for
 *  incorrectly encoded DICOM datasets, this check can be disabled globally.
 *  Please note, however, that other checks (i.e. VM and max. value length)
 *  are still performed if the optional parameters are set accordingly.
 *  By default, the VR checker is enabled (value: "OFTrue").
 */
extern DCMTK_DCMDATA_EXPORT OFGlobal<OFBool> dcmEnableVRCheckerForStringValues;  /* default: OFTrue */


/** base class for all DICOM value representations storing a character string
 */
class DCMTK_DCMDATA_EXPORT DcmByteString: public DcmElement
{

 public:

    /** constructor.
     *  Create new element from given tag and length.
     *  @param tag DICOM tag for the new element
     *  @param len value length for the new element
     */
    DcmByteString(const DcmTag &tag,
                  const Uint32 len = 0);

    /** copy constructor
     *  @param old element to be copied
     */
    DcmByteString(const DcmByteString &old);

    /** destructor
     */
    virtual ~DcmByteString();

    /** assignment operator
     *  @param obj element to be assigned/copied
     *  @return reference to this object
     */
    DcmByteString& operator=(const DcmByteString& obj);

    /** comparison operator that compares the normalized value of this object
     *  with a given object of the same type. The tag of the element is also
     *  considered as the first component that is compared, followed by the
     *  object types (VR, i.e. DCMTK'S EVR) and the comparison of all value
     *  components of the object, preferably in the order declared in the
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
      return new DcmByteString(*this);
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
     *  @return type identifier of this class (EVR_UNKNOWN)
     */
    virtual DcmEVR ident() const;

    /** clear the currently stored value
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition clear();

    /** get value multiplicity
     *  @return number of string components (separated by a backslash)
     */
    virtual unsigned long getVM();

    /** get length of the stored value.
     *  Trailing spaces (padding characters) are ignored for the "real" length.
     *  @return number of characters stored for the string value
     */
    Uint32 getRealLength();

    /** get DICOM length of the stored value.
     *  The string value is padded if required.  Therefore, the returned length
     *  always has an even value.
     *  @param xfer not used
     *  @param enctype not used
     *  @return number of characters stored in DICOM representation
     */
    virtual Uint32 getLength(const E_TransferSyntax xfer = EXS_LittleEndianImplicit,
                             const E_EncodingType enctype = EET_UndefinedLength);

    /** print element to a stream.
     *  The output format of the value is a backslash separated sequence of string
     *  components (if any).
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

    /** write data element to a stream
     *  @param outStream output stream
     *  @param oxfer transfer syntax used to write the data
     *  @param enctype flag, specifying the encoding with undefined or explicit length
     *  @param wcache pointer to write cache object, may be NULL
     */
    virtual OFCondition write(DcmOutputStream &outStream,
                              const E_TransferSyntax oxfer,
                              const E_EncodingType enctype,
                              DcmWriteCache *wcache);

    /** write data element to a stream as required for the creation of digital signatures
     *  @param outStream output stream
     *  @param oxfer transfer syntax used to write the data
     *  @param enctype flag, specifying the encoding with undefined or explicit length
     *  @param wcache pointer to write cache object, may be NULL
     */
    virtual OFCondition writeSignatureFormat(DcmOutputStream &outStream,
                                             const E_TransferSyntax oxfer,
                                             const E_EncodingType enctype,
                                             DcmWriteCache *wcache);

    /** get a copy of a particular string component
     *  @param stringVal variable in which the result value is stored
     *  @param pos index of the value in case of multi-valued elements (0..vm-1)
     *  @param normalize not used since string normalization depends on value representation
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getOFString(OFString &stringVal,
                                    const unsigned long pos,
                                    OFBool normalize = OFTrue);

    /** get entire element value as a character string.
     *  In case of VM > 1 the individual values are separated by a backslash ('\').
     *  @param stringVal variable in which the result value is stored
     *  @param normalize normalize each element value prior to concatenation
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getOFStringArray(OFString &stringVal,
                                         OFBool normalize = OFTrue);

    /** get a pointer to the current string value.
     *  This includes all string components and separators.
     *  NB: This method does not copy the stored value.
     *  @param stringVal reference to the pointer variable
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getString(char *&stringVal);

    /** get a pointer to the current string value.
     *  This includes all string components and separators. Since the length is returned
     *  separately, the string value can contain more than one NULL byte.
     *  NB: This method does not copy the stored value.
     *  @param stringVal reference to the pointer variable
     *  @param stringLen length of the string (number of characters without the trailing
     *    NULL byte)
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getString(char *&stringVal,
                                  Uint32 &stringLen);

    /** set element value from the given character string.
     *  The length of the given string is determined automatically by searching for the
     *  first NULL byte.
     *  @param stringVal input character string (possibly multi-valued)
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition putString(const char *stringVal);

    /** set element value from the given character string.
     *  The length of the string has to be specified explicitly. The string can, therefore,
     *  also contain more than one NULL byte.
     *  @param stringVal input character string (possibly multi-valued)
     *  @param stringLen length of the string (number of characters without the trailing
     *    NULL byte)
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition putString(const char *stringVal,
                                  const Uint32 stringLen);

    /** check the currently stored string value.
     *  Checks every string component for the maximum length specified for the particular
     *  value representation.
     *  @param autocorrect correct value and value component length if OFTrue
     *  @return status, EC_Normal if value length is correct, an error code otherwise
     */
    virtual OFCondition verify(const OFBool autocorrect = OFFalse);

    /** check if this element contains non-ASCII characters. Please note that this check
     *  is pretty simple and only works for single-byte character sets that do include
     *  the 7-bit ASCII codes, e.g. for the ISO 8859 family. In other words: All character
     *  codes below 128 are considered to be ASCII codes and all others are considered to
     *  be non-ASCII.
     *  @param checkAllStrings if true, also check elements with string values not affected
     *    by SpecificCharacterSet (0008,0005). By default, only check PN, LO, LT, SH, ST,
     *    UC and UT, i.e. none of the derived VR classes.
     *  @return true if element contains non-ASCII characters, false otherwise
     */
    virtual OFBool containsExtendedCharacters(const OFBool checkAllStrings = OFFalse);

    /** check if this element is affected by SpecificCharacterSet
     *  @return always returns false since none of the derived VR classes is affected by
     *    the SpecificCharacterSet (0008,0005) element
     */
    virtual OFBool isAffectedBySpecificCharacterSet() const;

    /** check if this object is empty
     *  @param normalize normalize value before checking (ignore non-significant characters)
     *  @return true if object is empty, i.e.\ has no value, false otherwise
     */
    virtual OFBool isEmpty(const OFBool normalize = OFTrue);

 protected:

    /// internal type used to specify the current string representation
    enum E_StringMode
    {
        /// string has internal representation (no padding)
        DCM_MachineString,
        /// string has DICOM representation (even length)
        DCM_DicomString,
        /// string has unknown representation (maybe multiple padding chars?)
        DCM_UnknownString
    };

    /** create a new value field (string buffer) of the previously defined size
     *  (member variable 'Length'). Also handles odd value length by allocating
     *  extra space for the padding character.
     *  This method is used by derived classes only.
     *  @return pointer to the newly created value field
     */
    virtual Uint8 *newValueField();

    /** method is called after the element value has been loaded.
     *  Can be used to correct the value before it is used for the first time.
     */
    virtual void postLoadValue();

    /** get current representation of the string value
     *  @return current representation of the string value
     */
    E_StringMode getStringMode() const { return fStringMode; }

    /** convert currently stored string value to internal representation.
     *  It removes any trailing space character and recomputes the string length.
     *  @param length number of characters of the string value (optional)
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition makeMachineByteString(const Uint32 length = 0);

    /** convert currently stored string value to DICOM representation.
     *  It removes trailing spaces apart from a possibly required single padding
     *  character (in case of odd string length).
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition makeDicomByteString();

    /** get a copy of the current string value.
     *  This includes all string components and separators.
     *  @param stringVal variable in which the result is stored
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition getStringValue(OFString &stringVal);

    /** get the maximum number of characters for each string component
     *  @return maximum number of characters for each string component
     */
    Uint32 getMaxLength() const { return maxLength; }

    /** get the end-of-string padding character
     *  @return end-of-string padding character
     */
    char getPaddingChar() const { return paddingChar; }

    /** set the end-of-string padding character
     *  @param c end-of-string padding character
     */
    void setPaddingChar(char c) { paddingChar = c; }

    /** set the maximum number of characters for each string component
     *  @param val maximum number of characters for each string component
     */
    void setMaxLength(Uint32 val) { maxLength = val; }

    /** set non-significant characters used to determine whether the value is empty
     *  @param characters non-significant characters used to determine whether the value is empty
     */
    void setNonSignificantChars(const OFString &characters) { nonSignificantChars = characters; }

    /* --- static helper functions --- */

    /** check if a given character string contains non-ASCII characters.
     *  Please note that this check is pretty simple and only works for single-byte character
     *  sets that do include the 7-bit ASCII codes, e.g. for the ISO 8859 family. In other
     *  words: All character codes below 128 are considered to be ASCII codes and all others
     *  are considered to be non-ASCII.
     *  @param stringVal character string to be checked
     *  @param stringLen length of the string (number of characters without the trailing
     *    NULL byte)
     * @return true if character string contains non-ASCII characters, false otherwise
     */
    static OFBool containsExtendedCharacters(const char *stringVal,
                                             const size_t stringLen);

    /** check whether given string value conforms to a certain VR and VM.
     *  @param value string value to be checked (possibly multi-valued)
     *  @param vm value multiplicity (according to the data dictionary) to be checked for.
     *    (See DcmElement::checkVM() for a list of valid values; "" for no check.)
     *  @param vr two-character identifier of the VR to be checked (lower case)
     *  @param vrID expected numeric identifier of the VR
     *  @param maxLen maximum number of characters allowed for a single value (0 = no check)
     *  @param charset character set (according to the value of the SpecificCharacterSet
     *    element) to be used for checking the string value. The default is ASCII (7-bit).
     *    Currently, the VR checker only supports ASCII (ISO_IR 6) and Latin-1 (ISO_IR 100).
     *    All other values disable the check of the value representation, e.g. "UNKNOWN".
     *  @return status of the check, EC_Normal if value is correct, an error code otherwise
     */
    static OFCondition checkStringValue(const OFString &value,
                                        const OFString &vm,
                                        const OFString &vr,
                                        const int vrID,
                                        const size_t maxLen = 0,
                                        const OFString &charset = "");

private:

    /// padding character used to adjust odd value length (e.g. a space)
    char paddingChar;

    /// maximum number of characters for each string component
    Uint32 maxLength;

    /// number of characters of the internal string representation
    Uint32 realLength;

    /// current representation of the string value
    E_StringMode fStringMode;

    /// non-significant characters used to determine whether the value is empty
    OFString nonSignificantChars;
};


/** @name string normalization flags.
 *  These flags can be used with normalizeString() to specify the extent of normalization.
 */
//@{

/// delete trailing spaces
const OFBool DELETE_TRAILING = OFTrue;
/// delete leading spaces
const OFBool DELETE_LEADING = OFTrue;
/// handle string as multi-valued (components separated by a backslash)
const OFBool MULTIPART = OFTrue;

//@}


/* Function to get part out of a String for VM > 1 */

/** normalize the given string value, i.e.\ remove leading and/or trailing padding
 *  @param string input and output string value to be normalized
 *  @param multiPart handle string as multi-valued if OFTrue
 *  @param leading delete leading spaces if OFTrue
 *  @param trailing delete trailing spaces if OFTrue
 *  @param paddingChar padding character to be removed (usually a space)
 */
DCMTK_DCMDATA_EXPORT void normalizeString(OFString &string,
                     const OFBool multiPart,
                     const OFBool leading,
                     const OFBool trailing,
                     const char paddingChar = ' ');


#endif // DCBYTSTR_H
