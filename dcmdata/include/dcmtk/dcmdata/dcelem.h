/*
 *
 *  Copyright (C) 1994-2010, OFFIS e.V.
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
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-11-05 09:34:11 $
 *  CVS/RCS Revision: $Revision: 1.49 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#ifndef DCELEM_H
#define DCELEM_H

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmdata/dcobject.h"
#include "dcmtk/ofstd/ofstring.h"

// forward declarations
class DcmInputStreamFactory;
class DcmFileCache;
class DcmItem;

/** abstract base class for all DICOM elements
 */
class DcmElement
  : public DcmObject
{

  public:

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

    /** calculate the length of this DICOM element when encoded with the
     *  given transfer syntax and the given encoding type for sequences.
     *  For elements, the length includes the length of the tag, length field,
     *  VR field and the value itself, for items and sequences it returns
     *  the length of the complete item or sequence including delimitation tags
     *  if applicable. Never returns undefined length.
     *  @param xfer transfer syntax for length calculation
     *  @param enctype sequence encoding type for length calculation
     *  @return length of DICOM element
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
     *    (valid values: "1", "1-2", "1-3", "1-8", "1-99", "1-n", "2", "2-n", "2-2n",
     *                   "3", "3-n", "3-3n", "4", "6", "9", "16", "32"),
     *    interpreted as cardinality (number of items) for sequence attributes
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
     *  In case of VM > 1 the single values are separated by a backslash ('\').
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
     *  copy parameter is OFTrue; otherwise the value is erased in the DICOM element.
     *  @param copy if true, copy value field before detaching; if false, do not retain a copy.
     *  @return EC_Normal upon success, an error code otherwise
     */
    OFCondition detachValueField(OFBool copy = OFFalse);

    // PUT operations

    /** replace the element value by a copy of the given string (which is possibly multi-valued).
     *  Requires element to be of corresponding VR, otherwise an error is returned.
     *  @param stringValue new attribute value
     *  @return EC_Normal upon success, an error code otherwise
     */
    virtual OFCondition putOFStringArray(const OFString &stringValue);

    /** replace the element value by a copy of the given string (which is possibly multi-valued).
     *  Requires element to be of corresponding VR, otherwise an error is returned.
     *  @param val new attribute value
     *  @return EC_Normal upon success, an error code otherwise
     */
    virtual OFCondition putString(const char *val);

    /** insert into the element value a copy of the given Sint16 value. If the
     *  attribute is multi-valued, all other values remain untouched.
     *  Requires element to be of corresponding VR, otherwise an error is returned.
     *  @param val new value to be inserted
     *  @param pos position for insert operation. Value: pos <= getVM(), i.e.
     *  a value can be appended to the end of the current element or inserted within
     *  the existing value field.
     *  @return EC_Normal upon success, an error code otherwise
     */
    virtual OFCondition putSint16(const Sint16 val, const unsigned long pos = 0);

    /** insert into the element value a copy of the given Uint16 value. If the
     *  attribute is multi-valued, all other values remain untouched.
     *  Requires element to be of corresponding VR, otherwise an error is returned.
     *  @param val new value to be inserted
     *  @param pos position for insert operation. Value: pos <= getVM(), i.e.
     *  a value can be appended to the end of the current element or inserted within
     *  the existing value field.
     *  @return EC_Normal upon success, an error code otherwise
     */
    virtual OFCondition putUint16(const Uint16 val, const unsigned long pos = 0);

    /** insert into the element value a copy of the given Sint32 value. If the
     *  attribute is multi-valued, all other values remain untouched.
     *  Requires element to be of corresponding VR, otherwise an error is returned.
     *  @param val new value to be inserted
     *  @param pos position for insert operation. Value: pos <= getVM(), i.e.
     *  a value can be appended to the end of the current element or inserted within
     *  the existing value field.
     *  @return EC_Normal upon success, an error code otherwise
     */
    virtual OFCondition putSint32(const Sint32 val, const unsigned long pos = 0);

    /** insert into the element value a copy of the given Uint32 value. If the
     *  attribute is multi-valued, all other values remain untouched.
     *  Requires element to be of corresponding VR, otherwise an error is returned.
     *  @param val new value to be inserted
     *  @param pos position for insert operation. Value: pos <= getVM(), i.e.
     *  a value can be appended to the end of the current element or inserted within
     *  the existing value field.
     *  @return EC_Normal upon success, an error code otherwise
     */
    virtual OFCondition putUint32(const Uint32 val, const unsigned long pos = 0);

    /** insert into the element value a copy of the given Float32 value. If the
     *  attribute is multi-valued, all other values remain untouched.
     *  Requires element to be of corresponding VR, otherwise an error is returned.
     *  @param val new value to be inserted
     *  @param pos position for insert operation. Value: pos <= getVM(), i.e.
     *  a value can be appended to the end of the current element or inserted within
     *  the existing value field.
     *  @return EC_Normal upon success, an error code otherwise
     */
    virtual OFCondition putFloat32(const Float32 val, const unsigned long pos = 0);

    /** insert into the element value a copy of the given Float64 value. If the
     *  attribute is multi-valued, all other values remain untouched.
     *  Requires element to be of corresponding VR, otherwise an error is returned.
     *  @param val new value to be inserted
     *  @param pos position for insert operation. Value: pos <= getVM(), i.e.
     *  a value can be appended to the end of the current element or inserted within
     *  the existing value field.
     *  @return EC_Normal upon success, an error code otherwise
     */
    virtual OFCondition putFloat64(const Float64 val, const unsigned long pos = 0);

    /** insert into the element value a copy of the given DcmTagKey value. If the
     *  attribute is multi-valued, all other values remain untouched.
     *  Requires element to be of corresponding VR, otherwise an error is returned.
     *  @param attrTag new value to be inserted
     *  @param pos position for insert operation. Value: pos <= getVM(), i.e.
     *  a value can be appended to the end of the current element or inserted within
     *  the existing value field.
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
     *  @param frameSize frame size in bytes (without padding) returned in this parameter upon success
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

  protected:

    /** This function returns this element's value. The returned value
     *  corresponds to the byte ordering (little or big endian) that
     *  was passed.
     *  @param newByteOrder The byte ordering that shall be accounted
     *                      for (little or big endian).
     */
    void *getValue(const E_ByteOrder newByteOrder = gLocalByteOrder);

    /** insert into the element value a copy of the given raw value. If the
     *  attribute is multi-valued, all other values remain untouched.
     *  Only works for fixed-size VRs, not for strings.
     *  @param value new value to be inserted
     *  @param position position for insert operation. Value: pos <= getVM(), i.e.
     *  a value can be appended to the end of the current element or inserted within
     *  the existing value field.
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

    /** return the current byte order of the value field
     *  @return current byte order of the value field
     */
    E_ByteOrder getByteOrder() const { return fByteOrder; }

    /** set the current byte order of the value field
     *  @param val byte order of the value field
     */
    void setByteOrder(E_ByteOrder val) { fByteOrder = val; }

    /* --- static helper functions --- */

    /** check for correct value multiplicity (VM)
     *  @param vmNum value multiplicity of the value to be checked.
     *    For empty values (vmNum=0), the status of the check is always EC_Normal (i.e. no error).
     *  @param vmStr value multiplicity (according to the data dictionary) to be checked for.
     *    (valid values: "1", "1-2", "1-3", "1-8", "1-99", "1-n", "2", "2-n", "2-2n",
     *                   "3", "3-n", "3-3n", "4", "6", "9", "16", "32")
     *  @return status of the check, EC_ValueMultiplicityViolated in case of error
     */
    static OFCondition checkVM(const unsigned long vmNum,
                               const OFString &vmStr);

  private:

    /// current byte order of attribute value in memory
    E_ByteOrder fByteOrder;

    /// required information to load value later
    DcmInputStreamFactory *fLoadValue;

    /// value of the element
    Uint8 *fValue;
};


#endif // DCELEM_H


/*
** CVS/RCS Log:
** $Log: dcelem.h,v $
** Revision 1.49  2010-11-05 09:34:11  joergr
** Added support for checking the value multiplicity "9" (see Supplement 131).
**
** Revision 1.48  2010-10-14 13:15:40  joergr
** Updated copyright header. Added reference to COPYRIGHT file.
**
** Revision 1.47  2010-04-23 15:28:17  joergr
** Specify an appropriate default value for the "vm" parameter of checkValue().
**
** Revision 1.46  2010-04-23 14:28:00  joergr
** Added new method to all VR classes which checks whether the stored value
** conforms to the VR definition and to the specified VM.
**
** Revision 1.45  2010-04-22 09:01:18  joergr
** Added support for further VM values ("1-8", "1-99", "16", "32") to be checked.
**
** Revision 1.44  2010-03-01 09:08:44  uli
** Removed some unnecessary include directives in the headers.
**
** Revision 1.43  2009-11-17 16:36:51  joergr
** Added new method that allows for determining the color model of the
** decompressed image.
**
** Revision 1.42  2009-08-03 09:05:29  joergr
** Added methods that check whether a given string value conforms to the VR and
** VM definitions of the DICOM standards.
**
** Revision 1.41  2009-05-11 16:05:45  meichel
** Minor fix in DcmElement::getUncompressedFrameSize for the rare case that
**   BitsAllocated is not 8 or 16. Also the method now returns the true frame
**   size without any pad byte.
**
** Revision 1.40  2009-02-04 17:52:17  joergr
** Fixes various type mismatches reported by MSVC introduced with OFFile class.
**
** Revision 1.39  2008-07-17 11:19:48  onken
** Updated copyFrom() documentation.
**
** Revision 1.38  2008-07-17 10:30:23  onken
** Implemented copyFrom() method for complete DcmObject class hierarchy, which
** permits setting an instance's value from an existing object. Implemented
** assignment operator where necessary.
**
** Revision 1.37  2008-06-23 12:09:13  joergr
** Fixed inconsistencies in Doxygen API documentation.
**
** Revision 1.36  2008-05-29 10:43:21  meichel
** Implemented new method createValueFromTempFile that allows the content of
**   a temporary file to be set as the new value of a DICOM element.
**   Also added a new method compact() that removes the value field if the
**   value field can still be reconstructed from file. For large attribute
**   value the file reference is now kept in memory even when the value has
**   been loaded once. Finally, added new helper method getUncompressedFrameSize
**   that computes the size of an uncompressed frame for a given dataset.
**
** Revision 1.35  2007/11/29 14:30:19  meichel
** Write methods now handle large raw data elements (such as pixel data)
**   without loading everything into memory. This allows very large images to
**   be sent over a network connection, or to be copied without ever being
**   fully in memory.
**
** Revision 1.34  2007/07/11 08:50:23  meichel
** Initial release of new method DcmElement::getPartialValue which gives access
**   to partial attribute values without loading the complete attribute value
**   into memory, if kept in file.
**
** Revision 1.33  2007/06/29 14:17:49  meichel
** Code clean-up: Most member variables in module dcmdata are now private,
**   not protected anymore.
**
** Revision 1.32  2007/06/07 09:01:15  joergr
** Added createUint8Array() and createUint16Array() methods.
**
** Revision 1.31  2006/10/13 10:12:02  joergr
** Fixed wrong formatting.
**
** Revision 1.30  2006/08/15 15:49:56  meichel
** Updated all code in module dcmdata to correctly compile when
**   all standard C++ classes remain in namespace std.
**
** Revision 1.29  2005/12/08 16:28:11  meichel
** Changed include path schema for all DCMTK header files
**
** Revision 1.28  2004/07/01 12:28:25  meichel
** Introduced virtual clone method for DcmObject and derived classes.
**
** Revision 1.27  2003/07/04 13:25:35  meichel
** Replaced forward declarations for OFString with explicit includes,
**   needed when compiling with HAVE_STD_STRING
**
** Revision 1.26  2002/12/09 09:31:14  wilkens
** Modified/Added doc++ documentation.
**
** Revision 1.25  2002/12/06 12:49:09  joergr
** Enhanced "print()" function by re-working the implementation and replacing
** the boolean "showFullData" parameter by a more general integer flag.
** Added doc++ documentation.
** Made source code formatting more consistent with other modules/files.
**
** Revision 1.24  2002/08/27 16:55:31  meichel
** Initial release of new DICOM I/O stream classes that add support for stream
**   compression (deflated little endian explicit VR transfer syntax)
**
** Revision 1.23  2002/04/25 10:06:09  joergr
** Added/modified getOFStringArray() implementation.
** Added support for XML output of DICOM objects.
**
** Revision 1.22  2001/09/25 17:19:25  meichel
** Adapted dcmdata to class OFCondition
**
** Revision 1.21  2001/06/01 15:48:39  meichel
** Updated copyright header
**
** Revision 1.20  2001/05/10 12:50:21  meichel
** Added protected createEmptyValue() method in class DcmElement.
**
** Revision 1.19  2000/11/07 16:56:06  meichel
** Initial release of dcmsign module for DICOM Digital Signatures
**
** Revision 1.18  2000/03/08 16:26:14  meichel
** Updated copyright header.
**
** Revision 1.17  2000/02/02 14:31:16  joergr
** Replaced 'delete' statements by 'delete[]' for objects created with 'new[]'.
**
** Revision 1.16  1999/03/31 09:24:37  meichel
** Updated copyright header in module dcmdata
**
** Revision 1.15  1998/11/12 16:47:38  meichel
** Implemented operator= for all classes derived from DcmObject.
**
** Revision 1.14  1998/07/15 15:48:47  joergr
** Removed several compiler warnings reported by gcc 2.8.1 with
** additional options, e.g. missing copy constructors and assignment
** operators, initialization of member variables in the body of a
** constructor instead of the member initialization list, hiding of
** methods by use of identical names, uninitialized member variables,
** missing const declaration of char pointers. Replaced tabs by spaces.
**
** Revision 1.13  1997/09/11 15:13:10  hewett
** Modified getOFString method arguments by removing a default value
** for the pos argument.  By requiring the pos argument to be provided
** ensures that callers realise getOFString only gets one component of
** a multi-valued string.
**
** Revision 1.12  1997/08/29 08:32:38  andreas
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
** Revision 1.11  1997/07/31 06:57:59  andreas
** new protected method swapValueField for DcmElement
**
** Revision 1.10  1997/07/21 07:57:53  andreas
** - New method DcmElement::detachValueField to give control over the
**   value field to the calling part (see dcelem.h)
** - Replace all boolean types (BOOLEAN, CTNBOOLEAN, DICOM_BOOL, BOOL)
**   with one unique boolean type OFBool.
**
** Revision 1.9  1997/05/27 13:48:26  andreas
** - Add method canWriteXfer to class DcmObject and all derived classes.
**   This method checks whether it is possible to convert the original
**   transfer syntax to an new transfer syntax. The check is used in the
**   dcmconv utility to prohibit the change of a compressed transfer
**   syntax to a uncompressed.
**
** Revision 1.8  1997/05/16 08:23:46  andreas
** - Revised handling of GroupLength elements and support of
**   DataSetTrailingPadding elements. The enumeratio E_GrpLenEncoding
**   got additional enumeration values (for a description see dctypes.h).
**   addGroupLength and removeGroupLength methods are replaced by
**   computeGroupLengthAndPadding. To support Padding, the parameters of
**   element and sequence write functions changed.
** - Added a new method calcElementLength to calculate the length of an
**   element, item or sequence. For elements it returns the length of
**   tag, length field, vr field, and value length, for item and
**   sequences it returns the length of the whole item. sequence including
**   the Delimitation tag (if appropriate).  It can never return
**   UndefinedLength.
**
** Revision 1.7  1997/04/18 08:13:28  andreas
** - The put/get-methods for all VRs did not conform to the C++-Standard
**   draft. Some Compilers (e.g. SUN-C++ Compiler, Metroworks
**   CodeWarrier, etc.) create many warnings concerning the hiding of
**   overloaded get methods in all derived classes of DcmElement.
**   So the interface of all value representation classes in the
**   library are changed rapidly, e.g.
**   OFCondition get(Uint16 & value, const unsigned long pos);
**   becomes
**   OFCondition getUint16(Uint16 & value, const unsigned long pos);
**   All (retired) "returntype get(...)" methods are deleted.
**   For more information see dcmdata/include/dcelem.h
**
** Revision 1.6  1996/07/17 12:38:58  andreas
** new nextObject to iterate a DicomDataset, DicomFileFormat, Item, ...
**
** Revision 1.5  1996/04/16 16:01:36  andreas
** - put methods for AttributeTag with DcmTagKey Parameter
** - better support for NULL values
**
** Revision 1.4  1996/03/12 15:31:56  hewett
** The base virtual get & put functions now support char*.
**
** Revision 1.3  1996/01/05 13:22:55  andreas
** - changed to support new streaming facilities
** - more cleanups
** - merged read / write methods for block and file transfer
**
*/
