/*
 *
 *  Copyright (C) 1994-2018, OFFIS e.V.
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
 *  Purpose: Interface of class DcmElement
 *
 */


#ifndef DCELEM_H
#define DCELEM_H

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmdata/dcobject.h"
#include "dcmtk/ofstd/ofstring.h"

// forward declarations
class DcmInputStreamFactory;
class DcmJsonFormat;
class DcmFileCache;
class DcmItem;

/** abstract base class for all DICOM elements
 */
class DCMTK_DCMDATA_EXPORT DcmElement
  : public DcmObject
{

  public:

    // be friend with "greater than" and "less than" operators that are defined
    // outside of this class
    friend OFBool operator< (const DcmElement& lhs, const DcmElement& rhs);
    friend OFBool operator> (const DcmElement& lhs, const DcmElement& rhs);
    friend OFBool operator<=(const DcmElement& lhs, const DcmElement& rhs);
    friend OFBool operator>=(const DcmElement& lhs, const DcmElement& rhs);

    /** constructor.
     *  Create new element from given tag and length.
     *  @param tag DICOM tag for the new element
     *  @param len value length for the new element
     */
    DcmElement(const DcmTag &tag,
               const Uint32 len = 0);

    /** copy constructor
     *  @param old element to be copied
     */
    DcmElement(const DcmElement &old);

    /** destructor
     */
    virtual ~DcmElement();

    /** assignment operator
     *  @param obj element to be assigned/copied
     *  @return reference to this object
     */
    DcmElement &operator=(const DcmElement &obj);

    /** comparison operator that compares the normalized value of this element
     *  with a given element of the same type (e.g. an DcmUnsignedShort with a
     *  DcmUnsignedShort). The tag of the element is also considered as the first
     *  component that is compared, followed by the object types (VR, i.e. DCMTK'S EVR).
     *  DcmElement's default implementation does only compare the tag and EVR while
     *  the derived classes implement the value comparisons by comparing all the
     *  components that make up the value, preferably in the order declared in
     *  the object (if applicable).
     *  @param  rhs the right hand side of the comparison
     *  @return 0 if the object values are equal.
     *    -1 if this element has fewer components than the rhs element.
     *    Also -1 if the value of the first component that does not match
     *    is lower in this object than in rhs. Also returned if rhs
     *    cannot be casted to this object type or both objects are of
     *    different VR (i.e. the DcmEVR returned by the element's ident()
     *    call are different).
     *    1 if either this element has more components than the rhs element, or
     *    if the first component that does not match is greater in this object
     *    than in rhs object.
     *    If the function is overwritten by derived classes, the behaviour might
     *    slightly change but all methods will return 0 on equality, and 1 or -1
     *    if different.
     */
    virtual int compare(const DcmElement& rhs) const =0;

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

    /** @copydoc DcmObject::calcElementLength()
     */
    virtual Uint32 calcElementLength(const E_TransferSyntax xfer,
                                     const E_EncodingType enctype);

    /** calculate the value length (without attribute tag, VR and length field)
     *  of this DICOM element when encoded with the given transfer syntax and
     *  the given encoding type for sequences. Never returns undefined length.
     *  @param xfer transfer syntax for length calculation
     *  @param enctype sequence encoding type for length calculation
     *  @return value length of DICOM element
     */
    virtual Uint32 getLength(const E_TransferSyntax /*xfer*/ = EXS_LittleEndianImplicit,
                             const E_EncodingType /*enctype*/ = EET_UndefinedLength)
    {
        return getLengthField();
    }

    /** check if this element is a leaf node in a dataset tree.
     *  All subclasses of DcmElement except for DcmSequenceOfItems
     *  are leaf nodes, while DcmSequenceOfItems, DcmItem, DcmDataset etc.
     *  are not.
     *  @return true if leaf node, false otherwise.
     */
    virtual OFBool isLeaf() const { return OFTrue; }

    /** check if value of this element is loaded into main memory
     *  @return true if value is present in memory, false if value still resides in file
     */
    inline OFBool valueLoaded() const { return fValue != NULL || getLengthField() == 0; }

    /** initialize the transfer state of this object. This method must be called
     *  before this object is written to a stream or read (parsed) from a stream.
     */
    virtual void transferInit();

    /** check if this DICOM object can be encoded in the given transfer syntax.
     *  @param newXfer transfer syntax in which the DICOM object is to be encoded
     *  @param oldXfer transfer syntax in which the DICOM object was read or created.
     *  @return true if object can be encoded in desired transfer syntax, false otherwise.
     */
    virtual OFBool canWriteXfer(const E_TransferSyntax newXfer,
                                const E_TransferSyntax oldXfer);

    /** This function reads the data value of an attribute which is
     *  captured in the input stream and captures this information
     *  in this. If not all information for an attribute could be
     *  read from the stream, the function returns EC_StreamNotifyClient.
     *  Note that if certain conditions are met, this function does
     *  not actually load the data value but creates and stores an object
     *  that enables us to load this information later.
     *  @param inStream      The stream which contains the information.
     *  @param ixfer         The transfer syntax which was used to encode
     *                       the information in inStream.
     *  @param glenc         Encoding type for group length; specifies what
     *                       will be done with group length tags.
     *  @param maxReadLength Maximum read length for reading an attribute value.
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition read(DcmInputStream &inStream,
                             const E_TransferSyntax ixfer,
                             const E_GrpLenEncoding glenc = EGL_noChange,
                             const Uint32 maxReadLength = DCM_MaxReadLength);

    /** This function writes this element's value to the outstream which was
     *  passed. When writing information, the byte ordering (little or big endian)
     *  of the transfer syntax which was passed will be accounted for. In case the
     *  outstream does not provide enough space for all bytes of the current
     *  element's value, only a certain part of the value will be written to the
     *  stream. This element's transfer state indicates if the all bytes of value
     *  have already been written to the stream (ERW_ready), if the writing is
     *  still in progress and more bytes need to be written to the stream
     *  (ERW_inWork) or if the writing of the bytes of this element's value has not
     *  even begun yet (ERW_init). The member variable fTransferredBytes indicates
     *  how many bytes (starting from byte 0) of this element's value have already
     *  been written to the stream. This function will return EC_Normal, if the
     *  entire value of this element has been written to the stream, it will return
     *  EC_StreamNotifyClient, if there is no more space in the buffer and _not_ all
     *  bytes of this element's value have been written, and it will return some
     *  other (error) value if there was an error.
     *  @param outStream The stream the information will be written to.
     *  @param oxfer The transfer syntax which shall be used.
     *  @param enctype encoding types (undefined or explicit length) (actually unused)
     *  @param wcache pointer to write cache object, may be NULL
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition write(DcmOutputStream &outStream,
                              const E_TransferSyntax oxfer,
                              const E_EncodingType enctype,
                              DcmWriteCache *wcache);

    /** write object in XML format
     *  @param out output stream to which the XML document is written
     *  @param flags optional flag used to customize the output (see DCMTypes::XF_xxx)
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition writeXML(STD_NAMESPACE ostream &out,
                                 const size_t flags = 0);

    /** write object in JSON format
     *  @param out output stream to which the JSON document is written
     *  @param format used to format and customize the output
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition writeJson(STD_NAMESPACE ostream &out,
                                  DcmJsonFormat &format);

    /** special write method for creation of digital signatures
     *  @param outStream DICOM output stream
     *  @param oxfer output transfer syntax
     *  @param enctype encoding types (undefined or explicit length)
     *  @param wcache pointer to write cache object, may be NULL
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition writeSignatureFormat(DcmOutputStream &outStream,
                                             const E_TransferSyntax oxfer,
                                             const E_EncodingType enctype,
                                             DcmWriteCache *wcache);

    /** clear (remove) attribute value
     *  @return EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition clear();

    /** check whether stored value conforms to the VR and to the specified VM
     *  @param vm value multiplicity (according to the data dictionary) to be checked for.
     *    (See DcmElement::checkVM() for a list of valid values.)
     *    Interpreted as cardinality (number of items) for sequence attributes.
     *  @param oldFormat support old ACR/NEMA format for certain VRs (DA, TM, PN) if OFTrue
     *  @return status of the check, EC_Normal if value is correct, an error code otherwise
     */
    virtual OFCondition checkValue(const OFString &vm = "1-n",
                                   const OFBool oldFormat = OFFalse);

    /** this method loads all attribute values maintained by this object and
     *  all sub-objects (in case of a container such as DcmDataset) into memory.
     *  After a call to this method, the file from which a dataset was read may safely
     *  be deleted or replaced. For large files, this method may obviously allocate large
     *  amounts of memory.
     *  @return EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition loadAllDataIntoMemory();

    // GET operations

    /** retrieve a single value of type Uint8. Requires element to be of corresponding VR,
     *  otherwise an error is returned.
     *  @param val value returned in this parameter upon success
     *  @param pos position in multi-valued attribute, default 0
     *  @return EC_Normal upon success, an error code otherwise
     */
    virtual OFCondition getUint8(Uint8 &val, const unsigned long pos = 0);

    /** retrieve a single value of type Sint16. Requires element to be of corresponding VR,
     *  otherwise an error is returned.
     *  @param val value returned in this parameter upon success
     *  @param pos position in multi-valued attribute, default 0
     *  @return EC_Normal upon success, an error code otherwise
     */
    virtual OFCondition getSint16(Sint16 &val, const unsigned long pos = 0);

    /** retrieve a single value of type Uint16. Requires element to be of corresponding VR,
     *  otherwise an error is returned.
     *  @param val value returned in this parameter upon success
     *  @param pos position in multi-valued attribute, default 0
     *  @return EC_Normal upon success, an error code otherwise
     */
    virtual OFCondition getUint16(Uint16 &val, const unsigned long pos = 0);

    /** retrieve a single value of type Sint32. Requires element to be of corresponding VR,
     *  otherwise an error is returned.
     *  @param val value returned in this parameter upon success
     *  @param pos position in multi-valued attribute, default 0
     *  @return EC_Normal upon success, an error code otherwise
     */
    virtual OFCondition getSint32(Sint32 &val, const unsigned long pos = 0);

    /** retrieve a single value of type Uint32. Requires element to be of corresponding VR,
     *  otherwise an error is returned.
     *  @param val value returned in this parameter upon success
     *  @param pos position in multi-valued attribute, default 0
     *  @return EC_Normal upon success, an error code otherwise
     */
    virtual OFCondition getUint32(Uint32 &val, const unsigned long pos = 0);

    /** retrieve a single value of type Float32. Requires element to be of corresponding VR,
     *  otherwise an error is returned.
     *  @param val value returned in this parameter upon success
     *  @param pos position in multi-valued attribute, default 0
     *  @return EC_Normal upon success, an error code otherwise
     */
    virtual OFCondition getFloat32(Float32 &val, const unsigned long pos = 0);

    /** retrieve a single value of type Float64. Requires element to be of corresponding VR,
     *  otherwise an error is returned.
     *  @param val value returned in this parameter upon success
     *  @param pos position in multi-valued attribute, default 0
     *  @return EC_Normal upon success, an error code otherwise
     */
    virtual OFCondition getFloat64(Float64 &val, const unsigned long pos = 0);

    /** retrieve a single value of type DcmTagKey. Requires element to be of corresponding VR,
     *  otherwise an error is returned.
     *  @param val value returned in this parameter upon success
     *  @param pos position in multi-valued attribute, default 0
     *  @return EC_Normal upon success, an error code otherwise
     */
    virtual OFCondition getTagVal(DcmTagKey &val, const unsigned long pos = 0);

    /** retrieve a single value of type string. Requires element to be of corresponding VR,
     *  otherwise an error is returned.
     *  @param str value returned in this parameter upon success
     *  @param pos position in multi-valued attribute
     *  @param normalize true if string is to be normalized (e.g. trailing space characters to be trimmed)
     *  @return EC_Normal upon success, an error code otherwise
     */
    virtual OFCondition getOFString(OFString &str,
                                    const unsigned long pos,
                                    OFBool normalize = OFTrue);

    /** get entire element value as a character string.
     *  In case of VM > 1 the individual values are separated by a backslash ('\').
     *  This method implements a general approach by concatenating the results of
     *  getOFString() for each value component. Derived class may implement more
     *  sophisticated methods.
     *  @param value variable in which the result value is stored
     *  @param normalize normalize each element value prior to concatenation
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getOFStringArray(OFString &value,
                                         OFBool normalize = OFTrue);

    /** get a pointer to the element value of the current element as type string.
     *  Requires element to be of corresponding VR, otherwise an error is returned.
     *  This method does not copy, but returns a pointer to the element value,
     *  which remains under control of this object and is valid only until the next
     *  read, write or put operation.
     *  @param val pointer to value returned in this parameter upon success
     *  @return EC_Normal upon success, an error code otherwise
     */
    virtual OFCondition getString(char *&val);        // for strings

    /** get a pointer to the element value of the current element as type string.
     *  Requires element to be of corresponding VR, otherwise an error is returned.
     *  This method does not copy, but returns a pointer to the element value,
     *  which remains under control of this object and is valid only until the next
     *  read, write or put operation.
     *  @param val pointer to value returned in this parameter upon success
     *  @param len length of the returned value (number of characters)
     *  @return EC_Normal upon success, an error code otherwise
     */
    virtual OFCondition getString(char *&val,
                                  Uint32 &len);

    /** get a pointer to the element value of the current element as type string.
     *  Requires element to be of corresponding VR, otherwise an error is returned.
     *  This method does not copy, but returns a pointer to the element value,
     *  which remains under control of this object and is valid only until the next
     *  read, write or put operation.
     *  @param val pointer to value returned in this parameter upon success
     *  @return EC_Normal upon success, an error code otherwise
     */
    virtual OFCondition getUint8Array(Uint8 *&val);   // for bytes

    /** get a pointer to the element value of the current element as type Sint16.
     *  Requires element to be of corresponding VR, otherwise an error is returned.
     *  This method does not copy, but returns a pointer to the element value,
     *  which remains under control of this object and is valid only until the next
     *  read, write or put operation.
     *  @param val pointer to value returned in this parameter upon success
     *  @return EC_Normal upon success, an error code otherwise
     */
    virtual OFCondition getSint16Array(Sint16 *&val);

    /** get a pointer to the element value of the current element as type Uint16.
     *  Requires element to be of corresponding VR, otherwise an error is returned.
     *  This method does not copy, but returns a pointer to the element value,
     *  which remains under control of this object and is valid only until the next
     *  read, write or put operation.
     *  @param val pointer to value returned in this parameter upon success
     *  @return EC_Normal upon success, an error code otherwise
     */
    virtual OFCondition getUint16Array(Uint16 *&val);

    /** get a pointer to the element value of the current element as type Sint32.
     *  Requires element to be of corresponding VR, otherwise an error is returned.
     *  This method does not copy, but returns a pointer to the element value,
     *  which remains under control of this object and is valid only until the next
     *  read, write or put operation.
     *  @param val pointer to value returned in this parameter upon success
     *  @return EC_Normal upon success, an error code otherwise
     */
    virtual OFCondition getSint32Array(Sint32 *&val);

    /** get a pointer to the element value of the current element as type Uint32.
     *  Requires element to be of corresponding VR, otherwise an error is returned.
     *  This method does not copy, but returns a pointer to the element value,
     *  which remains under control of this object and is valid only until the next
     *  read, write or put operation.
     *  @param val pointer to value returned in this parameter upon success
     *  @return EC_Normal upon success, an error code otherwise
     */
    virtual OFCondition getUint32Array(Uint32 *&val);

    /** get a pointer to the element value of the current element as type Float32.
     *  Requires element to be of corresponding VR, otherwise an error is returned.
     *  This method does not copy, but returns a pointer to the element value,
     *  which remains under control of this object and is valid only until the next
     *  read, write or put operation.
     *  @param val pointer to value returned in this parameter upon success
     *  @return EC_Normal upon success, an error code otherwise
     */
    virtual OFCondition getFloat32Array(Float32 *&val);

    /** get a pointer to the element value of the current element as type Float64.
     *  Requires element to be of corresponding VR, otherwise an error is returned.
     *  This method does not copy, but returns a pointer to the element value,
     *  which remains under control of this object and is valid only until the next
     *  read, write or put operation.
     *  @param val pointer to value returned in this parameter upon success
     *  @return EC_Normal upon success, an error code otherwise
     */
    virtual OFCondition getFloat64Array(Float64 *&val);

    /** detach the value field from the DICOM element (i.e., this object).
     *  After detaching the calling part of the application has total control
     *  over the element value, especially the value must be deleted from the
     *  heap after use. The DICOM element remains a copy of the value if the
     *  copy parameter is OFTrue; otherwise the value is erased in the DICOM
     *  element.
     *  @param copy if true, copy value field before detaching; if false, do not
     *    retain a copy.
     *  @return EC_Normal upon success, an error code otherwise
     */
    OFCondition detachValueField(OFBool copy = OFFalse);

    // PUT operations

    /** replace the element value by a copy of the given string (which is possibly
     *  multi-valued). Requires element to be of corresponding VR, otherwise an error
     *  is returned.
     *  @param val new attribute value
     *  @return EC_Normal upon success, an error code otherwise
     */
    virtual OFCondition putOFStringArray(const OFString &val);

    /** replace the element value by a copy of the given string (which is possibly
     *  multi-valued). Requires element to be of corresponding VR, otherwise an error
     *  is returned.
     *  @param val new attribute value
     *  @return EC_Normal upon success, an error code otherwise
     */
    virtual OFCondition putString(const char *val);

    /** replace the element value by a copy of the given string (which is possibly
     *  multi-valued). Requires element to be of corresponding VR, otherwise an error
     *  is returned.
     *  @param val new attribute value
     *  @param len length of the new attribute value (number of characters)
     *  @return EC_Normal upon success, an error code otherwise
     */
    virtual OFCondition putString(const char *val,
                                  const Uint32 len);

    /** insert into the element value a copy of the given Sint16 value. If the
     *  attribute is multi-valued, all other values remain untouched.
     *  Requires element to be of corresponding VR, otherwise an error is returned.
     *  @param val new value to be inserted
     *  @param pos position for insert operation. Value: pos <= getVM(), i.e. a value
     *    can be appended to the end of the current element or inserted within the
     *    existing value field.
     *  @return EC_Normal upon success, an error code otherwise
     */
    virtual OFCondition putSint16(const Sint16 val, const unsigned long pos = 0);

    /** insert into the element value a copy of the given Uint16 value. If the
     *  attribute is multi-valued, all other values remain untouched.
     *  Requires element to be of corresponding VR, otherwise an error is returned.
     *  @param val new value to be inserted
     *  @param pos position for insert operation. Value: pos <= getVM(), i.e. a value
     *    can be appended to the end of the current element or inserted within the
     *    existing value field.
     *  @return EC_Normal upon success, an error code otherwise
     */
    virtual OFCondition putUint16(const Uint16 val, const unsigned long pos = 0);

    /** insert into the element value a copy of the given Sint32 value. If the
     *  attribute is multi-valued, all other values remain untouched.
     *  Requires element to be of corresponding VR, otherwise an error is returned.
     *  @param val new value to be inserted
     *  @param pos position for insert operation. Value: pos <= getVM(), i.e. a value
     *    can be appended to the end of the current element or inserted within the
     *    existing value field.
     *  @return EC_Normal upon success, an error code otherwise
     */
    virtual OFCondition putSint32(const Sint32 val, const unsigned long pos = 0);

    /** insert into the element value a copy of the given Uint32 value. If the
     *  attribute is multi-valued, all other values remain untouched.
     *  Requires element to be of corresponding VR, otherwise an error is returned.
     *  @param val new value to be inserted
     *  @param pos position for insert operation. Value: pos <= getVM(), i.e. a value
     *    can be appended to the end of the current element or inserted within the
     *    existing value field.
     *  @return EC_Normal upon success, an error code otherwise
     */
    virtual OFCondition putUint32(const Uint32 val, const unsigned long pos = 0);

    /** insert into the element value a copy of the given Float32 value. If the
     *  attribute is multi-valued, all other values remain untouched.
     *  Requires element to be of corresponding VR, otherwise an error is returned.
     *  @param val new value to be inserted
     *  @param pos position for insert operation. Value: pos <= getVM(), i.e. a value
     *    can be appended to the end of the current element or inserted within the
     *    existing value field.
     *  @return EC_Normal upon success, an error code otherwise
     */
    virtual OFCondition putFloat32(const Float32 val, const unsigned long pos = 0);

    /** insert into the element value a copy of the given Float64 value. If the
     *  attribute is multi-valued, all other values remain untouched.
     *  Requires element to be of corresponding VR, otherwise an error is returned.
     *  @param val new value to be inserted
     *  @param pos position for insert operation. Value: pos <= getVM(), i.e. a value
     *    can be appended to the end of the current element or inserted within the
     *    existing value field.
     *  @return EC_Normal upon success, an error code otherwise
     */
    virtual OFCondition putFloat64(const Float64 val, const unsigned long pos = 0);

    /** insert into the element value a copy of the given DcmTagKey value. If the
     *  attribute is multi-valued, all other values remain untouched.
     *  Requires element to be of corresponding VR, otherwise an error is returned.
     *  @param attrTag new value to be inserted
     *  @param pos position for insert operation. Value: pos <= getVM(), i.e. a value
     *    can be appended to the end of the current element or inserted within the
     *    existing value field.
     *  @return EC_Normal upon success, an error code otherwise
     */
    virtual OFCondition putTagVal(const DcmTagKey &attrTag, const unsigned long pos = 0);

    /** replace the element value by a copy of the given Uint8 array (which is possibly multi-valued).
     *  Requires element to be of corresponding VR, otherwise an error is returned.
     *  @param vals new attribute value
     *  @param num number of values in array vals
     *  @return EC_Normal upon success, an error code otherwise
     */
    virtual OFCondition putUint8Array(const Uint8 *vals, const unsigned long num);

    /** replace the element value by a copy of the given Sint16 array (which is possibly multi-valued).
     *  Requires element to be of corresponding VR, otherwise an error is returned.
     *  @param vals new attribute value
     *  @param num number of values in array vals
     *  @return EC_Normal upon success, an error code otherwise
     */
    virtual OFCondition putSint16Array(const Sint16 *vals, const unsigned long num);

    /** replace the element value by a copy of the given Uint16 array (which is possibly multi-valued).
     *  Requires element to be of corresponding VR, otherwise an error is returned.
     *  @param vals new attribute value
     *  @param num number of values in array vals
     *  @return EC_Normal upon success, an error code otherwise
     */
    virtual OFCondition putUint16Array(const Uint16 *vals, const unsigned long num);

    /** replace the element value by a copy of the given Sint32 array (which is possibly multi-valued).
     *  Requires element to be of corresponding VR, otherwise an error is returned.
     *  @param vals new attribute value
     *  @param num number of values in array vals
     *  @return EC_Normal upon success, an error code otherwise
     */
    virtual OFCondition putSint32Array(const Sint32 *vals, const unsigned long num);

    /** replace the element value by a copy of the given Uint32 array (which is possibly multi-valued).
     *  Requires element to be of corresponding VR, otherwise an error is returned.
     *  @param vals new attribute value
     *  @param num number of values in array vals
     *  @return EC_Normal upon success, an error code otherwise
     */
    virtual OFCondition putUint32Array(const Uint32 *vals, const unsigned long num);

    /** replace the element value by a copy of the given Float32 array (which is possibly multi-valued).
     *  Requires element to be of corresponding VR, otherwise an error is returned.
     *  @param vals new attribute value
     *  @param num number of values in array vals
     *  @return EC_Normal upon success, an error code otherwise
     */
    virtual OFCondition putFloat32Array(const Float32 *vals, const unsigned long num);

    /** replace the element value by a copy of the given Float64 array (which is possibly multi-valued).
     *  Requires element to be of corresponding VR, otherwise an error is returned.
     *  @param vals new attribute value
     *  @param num number of values in array vals
     *  @return EC_Normal upon success, an error code otherwise
     */
    virtual OFCondition putFloat64Array(const Float64 *vals, const unsigned long num);

    /** Copy numBytes bytes of data from the attribute value in byteOrder byte order
     *  to targetBuffer, starting at byte offset offset of the attribute value.
     *  This method does not cause the complete attribute value to be read into
     *  main memory. Subsequent calls for the same partial value may cause repeated
     *  access to file if the attribute value is kept in file.
     *  @param targetBuffer pointer to target buffer, must not be NULL.
     *    Buffer size must be at least numBytes bytes.
     *  @param offset byte offset within the attribute value from where to start
     *    copying
     *  @param numBytes number of bytes to copy.
     *  @param cache file cache object that may be passed to multiple subsequent calls
     *    to this method for the same file; the file cache will then keep a file
     *    handle open, thus improving performance. Optional, may be NULL
     *  @param byteOrder byte order desired byte order of attribute value in memory buffer.
     *    Default is the local byte order of the machine.
     *  @return EC_Normal upon success, an error code otherwise
     */
    virtual OFCondition getPartialValue(void *targetBuffer,
                                        const Uint32 offset,
                                        Uint32 numBytes,
                                        DcmFileCache *cache = NULL,
                                        E_ByteOrder byteOrder = gLocalByteOrder);

    /** create an empty Uint8 array of given number of bytes and set it.
     *  All array elements are initialized with a value of 0 (using 'memzero').
     *  This method is only applicable to certain VRs, e.g. OB.
     *  @param numBytes number of bytes (8 bit) to be created
     *  @param bytes stores the pointer to the resulting buffer
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition createUint8Array(const Uint32 numBytes, Uint8 *&bytes);

    /** create an empty Uint16 array of given number of words and set it.
     *  All array elements are initialized with a value of 0 (using 'memzero').
     *  This method is only applicable to OW data.
     *  @param numWords number of words (16 bit) to be created
     *  @param words stores the pointer to the resulting buffer
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition createUint16Array(const Uint32 numWords, Uint16 *&words);

    /** replace the attribute value with the content of the given temporary file.
     *  The temporary file is not opened/loaded until the attribute value is accessed,
     *  very much like large attributes that remain in file during a read operation.
     *  @param factory a factory object that is able to create an input stream
     *    to the temporary file at the right location.
     *  @param length attribute value length, in bytes. Must be even length.
     *    The length is not checked against the real length of the temporary file,
     *    which might be written or completed after the call to this method.
     *  @param byteOrder byte order in the temporary file
     *  @return EC_Normal upon success, an error code otherwise.
     */
    virtual OFCondition createValueFromTempFile(DcmInputStreamFactory *factory,
                                                const Uint32 length,
                                                const E_ByteOrder byteOrder);

    /** remove the attribute value from memory if the attribute value can
     *  be loaded from file when needed again. Otherwise do nothing.
     */
    virtual void compact();

    /** compute uncompressed frame size of a single frame of this image.
     *  Note that the value returned by this method does not include the pad byte
     *  to even size needed for a buffer into which a frame is to be loaded.
     *  @param dataset dataset in which this pixel data element is contained
     *  @param frameSize frame size in bytes (without padding) returned in this
     *    parameter upon success, otherwise set to 0
     *  @return EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getUncompressedFrameSize(DcmItem *dataset,
                                                 Uint32 &frameSize) const;

    /** access single frame without decompressing or loading a complete
     *  multi-frame object. The frame is copied into the buffer passed by the caller
     *  which must be large enough to contain a complete frame.
     *  @param dataset pointer to DICOM dataset in which this pixel data object is
     *    located. Used to access rows, columns, samples per pixel etc.
     *  @param frameNo numer of frame, starting with 0 for the first frame.
     *  @param startFragment index of the compressed fragment that contains
     *    all or the first part of the compressed bitstream for the given frameNo.
     *    Upon successful return this parameter is updated to contain the index
     *    of the first compressed fragment of the next frame.
     *    When unknown, zero should be passed. In this case the decompression
     *    algorithm will try to determine the index by itself, which will always
     *    work if frames are decompressed in increasing order from first to last,
     *    but may fail if frames are decompressed in random order, multiple fragments
     *    per frame and multiple frames are present in the dataset, and the offset
     *    table is empty.
     *  @param buffer pointer to buffer allocated by the caller. The buffer
     *    must be large enough for one frame of this image.
     *  @param bufSize size of buffer, in bytes
     *  @param decompressedColorModel upon successful return, the color model
     *    of the decompressed image (which may be different from the one used
     *    in the compressed images) is returned in this parameter.
     *  @param cache file cache object that may be passed to multiple subsequent calls
     *    to this method for the same file; the file cache will then keep a file
     *    handle open, thus improving performance. Optional, may be NULL
     *  @return EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getUncompressedFrame(DcmItem *dataset,
                                             Uint32 frameNo,
                                             Uint32 &startFragment,
                                             void *buffer,
                                             Uint32 bufSize,
                                             OFString &decompressedColorModel,
                                             DcmFileCache *cache = NULL);

    /** determine color model of the decompressed image
     *  @param dataset pointer to DICOM dataset in which this pixel data object
     *    is located. Used to access photometric interpretation.
     *  @param decompressedColorModel upon successful return, the color model
     *    of the decompressed image (which may be different from the one used
     *    in the compressed images) is returned in this parameter
     *  @return EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getDecompressedColorModel(DcmItem *dataset,
                                                  OFString &decompressedColorModel);

    /** Determine if this element is universal matching.
     *  @param normalize normalize each element value. Defaults to OFTrue.
     *  @param enableWildCardMatching enable or disable wild card matching. Defaults to OFTrue,
     *    which means wild card matching is performed if the element's VR supports it. Set to
     *    OFFalse to force single value matching instead.
     *  @return returns OFTrue if element is empty or if enableWildCardMatching is enabled and
     *    the element contains only wildcard characters. Returns OFFalse otherwise.
     */
    virtual OFBool isUniversalMatch(const OFBool normalize = OFTrue,
                                    const OFBool enableWildCardMatching = OFTrue);

    /** perform attribute matching.
     *  Perform attribute matching on a candidate element using this element as the matching
     *  key.
     *  @note The given candidate element must refer to the same attribute kind, i.e. have the
     *    same tag and VR. The method will return OFFalse if it doesn't.
     *  @param candidate the candidate element to compare this element with.
     *  @param enableWildCardMatching enable or disable wild card matching. Defaults to OFTrue,
     *    which means wild card matching is performed if the element's VR supports it. Set to
     *    OFFalse to force single value matching instead.
     *  @return OFTrue if the candidate matches this element, OFFalse otherwise.
     */
    virtual OFBool matches(const DcmElement& candidate,
                           const OFBool enableWildCardMatching = OFTrue) const;

    /** perform combined attribute matching.
     *  Combine the given Attributes to one pair of matching key and candidate respectively
     *  and perform attribute matching on the result.
     *  @note The DICOM standard currently defines combined attribute matching for the VR
     *    DA in combination with TM, such that two attributes can be combined into a single
     *    attribute with VR=DT before matching against another pair of attributes with VR
     *    DA and TM. The method will return OFFalse if this element's VR is not DA or the
     *    given attributes are not of VR TM, DA and TM respectively.
     *  @param keySecond the second part of the matching key that will be combined with this
     *    element.
     *  @param candidateFirst the first part of the candidate that will be matched against this
     *    this element + keySecond.
     *  @param candidateSecond the second part of the candidate that will be combined with
     *    candidateFirst for matching against this elemement + keySecond.
     *  @return OFTrue if the combination of this elemement and keySecond match with the
     *    combination of candidateFirst and candidateSecond. OFFalse otherwise.
     */
    virtual OFBool combinationMatches(const DcmElement& keySecond,
                                      const DcmElement& candidateFirst,
                                      const DcmElement& candidateSecond) const;

    /** returns a pointer to the input stream, if available, NULL otherwise.
     *  In general, this pointer is available when the element is part of a dataset
     *  that has been read from a DICOM file, the file is not encoded in deflate
     *  transfer syntax, and the element value is large enough that loading the value
     *  has been postponed to the first read access. The DcmInputStreamFactory object
     *  can create an instance of a file stream seeked to the right position within
     *  the DICOM file from where the element value can be read.
     *  @return pointer to the input stream factory of the element, null if no object is available
     */
    inline const DcmInputStreamFactory* getInputStream() const
    {
        return fLoadValue;
    }

    /* --- static helper functions --- */

    /** scan string value for conformance with given value representation (VR)
     *  @param value string value to be scanned
     *  @param vr two-character identifier of the VR to be checked (lower case)
     *  @param pos position of the first character to be scanned in 'value'
     *  @param num number of characters to be scanned in 'value' (default: all)
     *  @return numeric identifier of the VR found, 16 in case of unknown VR
     */
    static int scanValue(const OFString &value,
                         const OFString &vr,
                         const size_t pos = 0,
                         const size_t num = OFString_npos);

    /** scan string value for conformance with given value representation (VR)
     *  @param vr two-character identifier of the VR to be checked (lower case)
     *  @param value string value to be scanned
     *  @param size number of characters to be scanned in 'value'
     *  @return numeric identifier of the VR found, 16 in case of unknown VR
     */
    static int scanValue(const OFString &vr,
                         const char* const value,
                         const size_t size);

    /** determine the number of values stored in a string, i.e.\ the value multiplicity (VM)
     *  @param str character string
     *  @param len length of the string (number of characters without the trailing NULL byte)
     *  @return number of values separated by backslash characters in the given string
     */
    static unsigned long determineVM(const char *str,
                                     const size_t len);

    /** get the first value stored in the given string.  The individual values are separated by
     *  a backslash.  Successive calls of this function allow for extracting all stored values.
     *  @param str character string
     *  @param pos position of the first character in the string to search from
     *  @param len length of the string (number of characters without the trailing NULL byte)
     *  @param val variable in which the result is stored (empty string in case of error)
     *  @return position to be used for the next search, identical to 'pos' in case of error
     */
    static size_t getValueFromString(const char *str,
                                     const size_t pos,
                                     const size_t len,
                                     OFString &val);

    /** check for correct value multiplicity (VM)
     *  @param vmNum value multiplicity of the value to be checked.
     *    For empty values (vmNum=0), the status of the check is always EC_Normal (i.e. no error).
     *  @param vmStr value multiplicity (according to the data dictionary) to be checked for.
     *    (valid values: "1", "1-2", "1-3", "1-8", "1-99", "1-n", "2", "2-n", "2-2n",
     *                   "3", "3-n", "3-3n", "4", "5", "5-n", "6", "7", "7-7n", "8", "9",
     *                   "16", "24", "32", "256")
     *  @return status of the check, EC_ValueMultiplicityViolated in case of error
     */
    static OFCondition checkVM(const unsigned long vmNum,
                               const OFString &vmStr);

  protected:

    /** This function returns this element's value. The returned value corresponds to the
    *   byte ordering (little or big endian) that was passed.
     *  @param newByteOrder The byte ordering that shall be accounted
     *                      for (little or big endian).
     */
    void *getValue(const E_ByteOrder newByteOrder = gLocalByteOrder);

    /** insert into the element value a copy of the given raw value. If the
     *  attribute is multi-valued, all other values remain untouched.
     *  Only works for fixed-size VRs, not for strings.
     *  @param value new value to be inserted
     *  @param position position for insert operation. Value: pos <= getVM(), i.e.\ a value
     *    can be appended to the end of the current element or inserted within the existing
     *    value field.
     *  @param num number of bytes for each value in the value field.
     *  @return EC_Normal upon success, an error code otherwise
     */
    OFCondition changeValue(const void *value,      // new Value
                            const Uint32 position,  // position in value array
                            const Uint32 num);      // number of new value bytes

    /** replace the element value by a copy of the given raw data block
     *  @param value new attribute value
     *  @param length length of new attribute value in bytes
     *  @return EC_Normal upon success, an error code otherwise
     */
    OFCondition putValue(const void *value,     // new value
                         const Uint32 length);  // number of new value bytes

    /** create a new, empty value field of given size.
     *  @param length size of new value field in bytes
     *  @return EC_Normal if successful, an error code otherwise.
     */
    OFCondition createEmptyValue(const Uint32 length); // number of new value bytes

    /** This function reads the data value of an attribute and stores the
     *  information which was read in this. The information is either read
     *  from the inStream or (if inStream is NULL) from a different stream
     *  which was created earlier and which is accessible through the fLoadValue
     *  member variable. Note that if not all information for an attribute
     *  could be read from the stream, the function returns EC_StreamNotifyClient.
     *  @param inStream The stream which contains the information.
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition loadValue(DcmInputStream *inStream = NULL);

    /** performs clean-up functions after loading an attribute value into main memory.
     *  In particular, if dcmEnableAutomaticInputDataCorrection is enabled, silently
     *  fixes odd length elements.
     */
    virtual void postLoadValue();

    /** This function creates a byte array of Length bytes and returns this
     *  array. In case Length is odd, an array of Length+1 bytes will be
     *  created and Length will be increased by 1.
     *  @return pointer to created byte array
     */
    virtual Uint8 *newValueField();

    /** swaps the content of the value field (if loaded) from big-endian to
     *  little-endian or back
     *  @param valueWidth width (in bytes) of each element value
     */
    void swapValueField(size_t valueWidth);

    /** write element start tag in XML format
     *  @param out output stream to which the XML start tag is written
     *  @param flags flag used to customize the output (not yet used)
     *  @param attrText extra attribute text to be added to the element tag
     */
    virtual void writeXMLStartTag(STD_NAMESPACE ostream &out,
                                  const size_t flags,
                                  const char *attrText = NULL);

    /** write element end tag in XML format
     *  @param out output stream to which the XML end tag is written
     *  @param flags flag used to customize the output (not yet used)
     */
    virtual void writeXMLEndTag(STD_NAMESPACE ostream &out,
                                const size_t flags);

    /** write element start tag in JSON format
     *  @param out output stream to which the JSON document is written
     *  @param format used to format the output
     */
    virtual void writeJsonOpener(STD_NAMESPACE ostream &out,
                                 DcmJsonFormat &format);

    /** write element end tag in JSON format
     *  @param out output stream to which the JSON document is written
     *  @param format used to format the output
     */
    virtual void writeJsonCloser(STD_NAMESPACE ostream &out,
                                 DcmJsonFormat &format);

    /** return the current byte order of the value field
     *  @return current byte order of the value field
     */
    E_ByteOrder getByteOrder() const { return fByteOrder; }

    /** set the current byte order of the value field
     *  @param val byte order of the value field
     */
    void setByteOrder(E_ByteOrder val) { fByteOrder = val; }

  private:

    /// current byte order of attribute value in memory
    E_ByteOrder fByteOrder;

    /// required information to load value later
    DcmInputStreamFactory *fLoadValue;

    /// value of the element
    Uint8 *fValue;
};

/** Checks whether left hand side element is smaller than right hand side
 *  element. Uses DcmElement's compare() method in order to perform the
 *  comparison. See DcmElement::compare() for details.
 *  @param lhs left hand side of the comparison
 *  @param rhs right hand side of the comparison
 *  @return OFTrue if lhs is smaller than rhs, OFFalse otherwise
 */
inline OFBool operator< (const DcmElement& lhs, const DcmElement& rhs)
{
  return ( lhs.compare(rhs) < 0 );
}

/** Checks whether left hand side element is greater than right hand side
 *  element. Uses DcmElement's compare() method in order to perform the
 *  comparison. See DcmElement::compare() for details.
 *  @param lhs left hand side of the comparison
 *  @param rhs right hand side of the comparison
 *  @return OFTrue if lhs is greater than rhs, OFFalse otherwise
 */
inline OFBool operator> (const DcmElement& lhs, const DcmElement& rhs)
{
    return rhs < lhs;
}

/** Checks whether left hand side element is smaller than or equal to right hand
 *  side element. Uses DcmElement's compare() method in order to perform the
 *  comparison. See DcmElement::compare() for details.
 *  @param lhs left hand side of the comparison
 *  @param rhs right hand side of the comparison
 *  @return OFTrue if lhs is smaller than rhs or both are equal, OFFalse
 *          otherwise
 */
inline OFBool operator<=(const DcmElement& lhs, const DcmElement& rhs)
{
    return !(lhs > rhs);
}

/** Checks whether left hand side element is greater than or equal to right hand
 *  side element. Uses DcmElement's compare() method in order to perform the
 *  comparison. See DcmElement::compare() for details.
 *  @param lhs left hand side of the comparison
 *  @param rhs right hand side of the comparison
 *  @return OFTrue if lhs is greater than rhs or both are equal, OFFalse
 *          otherwise
 */
inline OFBool operator>=(const DcmElement& lhs, const DcmElement& rhs)
{
    return !(lhs < rhs);
}

#endif // DCELEM_H
