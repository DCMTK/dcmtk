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
 *  Author:  Andreas Barth
 *
 *  Purpose:
 *  Interface of class DcmPolymorphOBOW for Tags that can change their VR
 *  between OB and OW (e.g. Tag PixelData, OverlayData). This class shall
 *  not be used directly in applications. No identification exists.
 *
 */

#ifndef DCVRPOBW_H
#define DCVRPOBW_H

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/dcmdata/dcvrobow.h"

/** a class representing DICOM elements (such as uncompressed pixel data) that can
 *  be interpreted either as OB or as OW, i.e. are polymorph.
 */
class DCMTK_DCMDATA_EXPORT DcmPolymorphOBOW : public DcmOtherByteOtherWord
{
public:

    /** constructor
     *  @param tag attribute tag
     *  @param len length of the attribute value
     */
    DcmPolymorphOBOW(
        const DcmTag & tag,
        const Uint32 len = 0);

    /// copy constructor
    DcmPolymorphOBOW(
        const DcmPolymorphOBOW & old);

    /// destructor
    virtual ~DcmPolymorphOBOW();

    /** copy assignment operator
     *  @param obj element to be copied
     */
    DcmPolymorphOBOW &operator=(
        const DcmPolymorphOBOW &obj);

    /** clone method
     *  @return deep copy of this object
     */
    virtual DcmObject *clone() const
    {
      return new DcmPolymorphOBOW(*this);
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
    virtual OFCondition read(
        DcmInputStream & inStream,
        const E_TransferSyntax ixfer,
        const E_GrpLenEncoding glenc,
        const Uint32 maxReadLength);

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
    virtual OFCondition write(
      DcmOutputStream &outStream,
      const E_TransferSyntax oxfer,
      const E_EncodingType enctype,
      DcmWriteCache *wcache);

    /** special write method for creation of digital signatures
     *  @param outStream DICOM output stream
     *  @param oxfer output transfer syntax
     *  @param enctype encoding types (undefined or explicit length)
     *  @param wcache pointer to write cache object, may be NULL
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition writeSignatureFormat(
      DcmOutputStream &outStream,
      const E_TransferSyntax oxfer,
      const E_EncodingType enctype,
      DcmWriteCache *wcache);

    /** initialize the transfer state of this object. This method must be called
     *  before this object is written to a stream or read (parsed) from a stream.
     */
    virtual void transferInit();

    /** finalize the transfer state of this object. This method must be called
     *  when reading/writing this object from/to a stream has been completed.
     */
    virtual void transferEnd();

    /** get a pointer to the element value of the current element as type string.
     *  Requires element to be of corresponding VR, otherwise an error is returned.
     *  This method does not copy, but returns a pointer to the element value,
     *  which remains under control of this object and is valid only until the next
     *  read, write or put operation.
     *  @param val pointer to value returned in this parameter upon success
     *  @return EC_Normal upon success, an error code otherwise
     */
    virtual OFCondition getUint8Array(Uint8 *&val);   // for bytes

    /** get a pointer to the element value of the current element as type Uint16.
     *  Requires element to be of corresponding VR, otherwise an error is returned.
     *  This method does not copy, but returns a pointer to the element value,
     *  which remains under control of this object and is valid only until the next
     *  read, write or put operation.
     *  @param val pointer to value returned in this parameter upon success
     *  @return EC_Normal upon success, an error code otherwise
     */
    virtual OFCondition getUint16Array(Uint16 *&val);

    /** replace the element value by a copy of the given Uint8 array (which is possibly multi-valued).
     *  Requires element to be of corresponding VR, otherwise an error is returned.
     *  @param vals new attribute value
     *  @param num number of values in array vals
     *  @return EC_Normal upon success, an error code otherwise
     */
    virtual OFCondition putUint8Array(const Uint8 *vals, const unsigned long num);

    /** replace the element value by a copy of the given Uint16 array (which is possibly multi-valued).
     *  Requires element to be of corresponding VR, otherwise an error is returned.
     *  @param vals new attribute value
     *  @param num number of values in array vals
     *  @return EC_Normal upon success, an error code otherwise
     */
    virtual OFCondition putUint16Array(const Uint16 *vals, const unsigned long num);

    /** create an empty Uint8 array of given number of bytes and set it.
     *  All array elements are initialized with a value of 0 (using 'memzero').
     *  This method is only applicable to certain VRs, e.g. OB.
     *  @param numBytes number of bytes (8 bit) to be created
     *  @param bytes stores the pointer to the resulting buffer
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition createUint8Array(
        const Uint32 numBytes,
        Uint8 * & bytes);


    /** create an empty Uint16 array of given number of words and set it.
     *  All array elements are initialized with a value of 0 (using 'memzero').
     *  This method is only applicable to OW data.
     *  @param numWords number of words (16 bit) to be created
     *  @param words stores the pointer to the resulting buffer
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition createUint16Array(
        const Uint32 numWords,
        Uint16 * & words);

private:

    /** this flag is used during write operations in Implicit VR Little Endian
     *  and notes that the VR has temporarily been changed from OB to OW
     *  at the beginning to the write operation and needs to be changed back once the
     *  operation has finished.
     */
    OFBool changeVR;

    /// the current VR of value field, which can change over time
    DcmEVR currentVR;

};
#endif
