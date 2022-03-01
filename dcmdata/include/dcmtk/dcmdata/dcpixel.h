/*
 *
 *  Copyright (C) 1994-2021, OFFIS e.V.
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
 *  Purpose: Interface of class DcmPixelData
 *
 */

#ifndef DCPIXEL_H
#define DCPIXEL_H

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/dcmdata/dcvrpobw.h"
#include "dcmtk/ofstd/oflist.h"

class DcmCodec;
class DcmCodecList;
class DcmItem;
class DcmPixelData;
class DcmPixelSequence;
class DcmRepresentationEntry;
class DcmStack;

/** abstract base class for codec representation parameter sets.
 *  A codec parameter set subclass is implemented for each codec and passed to the codec
 *  by the encode() and decode() routines. It is supposed to contain data that may vary
 *  between compressed image representations using the same transfer syntax,
 *  for example the quality factor or compression factor for lossy compression.
 */
class DCMTK_DCMDATA_EXPORT DcmRepresentationParameter
{
public:
    /// default constructor
    DcmRepresentationParameter() {}

    /// copy constructor
    DcmRepresentationParameter(const DcmRepresentationParameter&) {}

    /// destructor
    virtual ~DcmRepresentationParameter() {}

    /** this methods creates a copy of type DcmRepresentationParameter *
     *  it must be overwritten in every subclass.
     *  @return copy of this object
     */
    virtual DcmRepresentationParameter *clone() const = 0;

    /** returns the class name as string.
     *  can be used in operator== as poor man's RTTI replacement.
     */
    virtual const char *className() const = 0;

    /** compares an object to another DcmRepresentationParameter.
     *  Implementation must make sure that classes are comparable.
     *  @param arg representation parameter to compare with
     *  @return true if equal, false otherwise.
     */
    virtual OFBool operator==(const DcmRepresentationParameter &arg) const = 0;
};


/** an object of this class maintains one compression "version"
 *  of a PixelData element within a DICOM dataset.  There can be
 *  multiple compressed versions in parallel, with different
 *  transfer syntaxes (compression algorithms) or representation
 *  parameters (e.g. compression factors).
 */
class DCMTK_DCMDATA_EXPORT DcmRepresentationEntry
{
    /** constructor
     *  @param rt transfer syntax
     *  @param rp pointer to representation parameter on heap,
     *     will be deleted when this object destructs.
     *  @param pixSeq pointer to pixel sequence on heap,
     *     will be deleted when this object destructs.
     */
    DcmRepresentationEntry(
        const E_TransferSyntax rt,
        const DcmRepresentationParameter * rp,
        DcmPixelSequence * pixSeq);

    /// copy constructor
    DcmRepresentationEntry(const DcmRepresentationEntry &oldEntry);

    /// destructor
    ~DcmRepresentationEntry();

    /// comparison operator
    OFBool operator==(const DcmRepresentationEntry& x) const;

    /// comparison operator
    OFBool operator!=(const DcmRepresentationEntry & x) const
    {
      return !(*this == x);
    }

private:
    /// private undefined copy assignment operator
    DcmRepresentationEntry &operator=(const DcmRepresentationEntry &);

    /// transfer syntax
    E_TransferSyntax repType;

    /// representation parameter for this pixel sequence
    DcmRepresentationParameter * repParam;

    /// the compressed pixel sequence itself
    DcmPixelSequence * pixSeq;

    friend class DcmPixelData;
};

/// list of DcmRepresentationEntry objects
typedef OFList<DcmRepresentationEntry *> DcmRepresentationList;

/// iterator for list of DcmRepresentationEntry objects
typedef OFListIterator(DcmRepresentationEntry *) DcmRepresentationListIterator;

/// const iterator for list of DcmRepresentationEntry objects
typedef OFListConstIterator(DcmRepresentationEntry *) DcmRepresentationListConstIterator;

/** The class DcmPixelData stores different pixel representations identified by
 * a type (the transfer syntax) and some representation parameters
 * The three unencapsulated transfer syntaxes belong to the same pixel
 * representation.
 * A type (or transfer syntax) conforms to a representation if
 * the type and the representation type are equal or both are unencapsulated.
 * If this is valid for the representation read or set by chooseRepresentation
 * then this representation is the conforming representation.
 * else a representation with the default parameter set defined in the
 * codec is the conforming representation.
 */
class DCMTK_DCMDATA_EXPORT DcmPixelData : public DcmPolymorphOBOW
{
private:
    friend class DcmRepresentationEntry;

    /// List of representations of pixel data
    DcmRepresentationList repList;

    /// Iterator to the last dummy element in representation list
    DcmRepresentationListIterator repListEnd;

    /// Iterator to the original representation. if an uncompressed
    /// representation is used the iterator points to repList.end()
    DcmRepresentationListIterator original;

    /// current list element for some operations
    DcmRepresentationListIterator current;

    /// shows if an unencapsulated representation is stored
    OFBool existUnencapsulated;

    /** this flag indicates that this pixel data element will be written
     *  in uncompressed (defined length) format even if the dataset
     *  itself is written in a compressed syntax where pixel data is normally
     *  written in encapsulated (undefined length) format.
     *  By default this flag is false, unless the dataset was read in an
     *  encapsulated transfer syntax and this pixel data element was already
     *  present in uncompressed format.
     */
    OFBool alwaysUnencapsulated;

    /// value representation of unencapsulated data
    DcmEVR unencapsulatedVR;

    /// in write function: pointer to current pixel sequence
    DcmPixelSequence * pixelSeqForWrite;

    /** check if this element should be written unencapsulated, even though an
     *  encapsulated transfer syntax is used. In other words, this checks if
     *  this pixel data element is on the main level on the dataset or not.
     *  @param xfer the transfer syntax that should be used
     */
    OFBool writeUnencapsulated(const E_TransferSyntax xfer);

    /** This function removes all pixel representations from the list
     *  of pixel representations except the one which was passed. Note
     *  that if parameter leaveInList equals repListEnd, all representations
     *  will be removed from the list.
     *  @param leaveInList Iterator to a representation which shall not
     *                     be removed from the list of representations.
     */
    void clearRepresentationList(
        DcmRepresentationListIterator leaveInList);

    /** find a conforming representation in the list of
     *  encapsulated representations
     */
    OFCondition findConformingEncapsulatedRepresentation(
        const DcmXfer & repType,
        const DcmRepresentationParameter * repParam,
        DcmRepresentationListIterator & result);

    /** find a representation entry and return an iterator to the found entry
     *  or the next element in the list. The condition returned can be EC_Normal
     *  if such an entry is found or EC_RepresentationNotFound. The pixSeq
     *  attribute in findEntry can be NULL, it is not needed for the find
     *  operation!
     *  @param findEntry the entry to find
     *  @param result the found entry
     *  @return EC_Normal if entry is found, EC_RepresentationNotFound otherwise.
     */
    OFCondition findRepresentationEntry(
        const DcmRepresentationEntry & findEntry,
        DcmRepresentationListIterator & result);

    /** insert or replace a representation entry in the list
     *  @param repEntry the DcmRepresentationEntry to be inserted
     *  @return the inserted entry
     */
    DcmRepresentationListIterator insertRepresentationEntry(
        DcmRepresentationEntry * repEntry);

    /** decode representation to unencapsulated format
     *  @param fromType transfer syntax to decode from
     *  @param fromParam representation parameter of current compressed
     *    representation, may be NULL.
     *  @param fromPixSeq compressed pixel sequence
     *  @param pixelStack stack pointing to the location of the pixel data
     *    element in the current dataset.
     *  @return EC_Normal if successful, an error code otherwise.
     */
    OFCondition decode(
        const DcmXfer & fromType,
        const DcmRepresentationParameter * fromParam,
        DcmPixelSequence * fromPixSeq,
        DcmStack & pixelStack);

    /** encode to encapsulated format
     *  @param fromType current transfer syntax of the compressed image
     *  @param fromParam current representation parameter of compressed data, may be NULL
     *  @param fromPixSeq compressed pixel sequence
     *  @param toType transfer syntax to compress to
     *  @param toParam representation parameter describing the desired
     *    new compressed representation (e.g. JPEG quality)
     *  @param pixelStack stack pointing to the location of the pixel data
     *    element in the current dataset.
     *  @return EC_Normal if successful, an error code otherwise.
     */
    OFCondition encode(
        const DcmXfer & fromType,
        const DcmRepresentationParameter * fromParam,
        DcmPixelSequence * fromPixSeq,
        const DcmXfer & toType,
        const DcmRepresentationParameter *toParam,
        DcmStack & pixelStack);

    /** set the current VR, which is always OB if the currently selected
     *  pixel representation is compressed, and may be OB or OW for uncompressed.
     */
    void recalcVR()
    {
        if (current == repList.end()) setTagVR(unencapsulatedVR);
        else setTagVR(EVR_OB);
    }

public:

    /** constructor.
     *  Create new element from given tag and length.
     *  @param tag attribute tag
     *  @param len length of the attribute value
     */
    DcmPixelData(const DcmTag & tag, const Uint32 len = 0);

    /** copy constructor
     *  @param pixelData element to be copied
     */
    DcmPixelData(const DcmPixelData & pixelData);

    /// destructor
    virtual ~DcmPixelData();

    /** copy assignment operator
     *  @param obj element to be copied
     */
    DcmPixelData &operator=(const DcmPixelData &obj);

    /** clone method
     *  @return deep copy of this object
     */
    virtual DcmObject *clone() const
    {
      return new DcmPixelData(*this);
    }

    /** comparison operator that compares the value of this element
     *  with a given element of the same type (e.g. an DcmPixelData with a
     *  DcmPixelData). The tag of the element is also considered as the first
     *  component that is compared, followed by the object types (VR, i.e. DCMTK'S EVR).
     *  The DcmPixelData implementation checks whether the uncompressed data of
     *  both objects are identical, and if not provided, if the compressed data
     *  of both objects is the same, by comparing the pixel items bytewise.
     *  @param  rhs the right hand side of the comparison
     *  @return 0 if the object values are equal.
     *          -1 is returned if rhs is considered greater than this object.
     *          1 is returned if rhs is considered smaller than this object.
     */
    virtual int compare(const DcmElement& rhs) const;

    /** virtual object copying. This method can be used for DcmObject
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

    /** set/change the current value representation of the uncompressed image representation, if any
     *  @param vr new value representation to be set.  All VRs except for OW (Other
     *    Word String) are treated as 8 bit data (OB).  This is particularly useful
     *    for unknown (UN) or unsupported VRs.
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setVR(DcmEVR vr);

    /** return identifier for this class. Every class derived from this class
     *  returns a unique value of type enum DcmEVR for this call. This is used
     *  as a "poor man's RTTI" to correctly identify instances derived from
     *  this class even on compilers not supporting RTTI.
     *  @return type identifier of this class
     */
    virtual DcmEVR ident() const { return EVR_PixelData; }

    /** print all elements of the item to a stream
     *  @param out output stream
     *  @param flags optional flag used to customize the output (see DCMTypes::PF_xxx)
     *  @param level current level of nested items. Used for indentation.
     *  @param pixelFileName optional filename used to write the raw pixel data file
     *  @param pixelCounter optional counter used for automatic pixel data filename creation
     */
    virtual void print(STD_NAMESPACE ostream &out,
                       const size_t flags = 0,
                       const int level = 0,
                       const char *pixelFileName = NULL,
                       size_t *pixelCounter = NULL);

    /** tests if it is possible to write a specific representation
     *  Only existing representations are considered, since this
     *  method does not create a representation.
     *  @param newXfer the syntax to be checked
     */
    virtual OFBool canWriteXfer(const E_TransferSyntax newXfer,
                                const E_TransferSyntax oldXfer);

    /** @copydoc DcmObject::calcElementLength()
     */
    virtual Uint32 calcElementLength(const E_TransferSyntax xfer,
                                     const E_EncodingType enctype);

    /** returns length of representation conforming to the transfer syntax
     *  @param xfer the transfer syntax
     *  @param enctype the encoding type
     *  @return returns length of representation value field conforming to
     *  given transfer syntax. It does not create a representation.
     *  If no conforming representation exists, an error code is set
     *  and 0 returned.
     */
    virtual Uint32 getLength(
        const E_TransferSyntax xfer  = EXS_LittleEndianImplicit,
        const E_EncodingType enctype = EET_UndefinedLength);

    /** initialize the transfer state of this object. This method must be called
     *  before this object is written to a stream or read (parsed) from a stream.
     */
    virtual void transferInit();

    /** This function reads the data value of a pixel data attribute
     *  which is captured in the input stream and captures this
     *  information in this. This function takes into account, if the
     *  pixel data is captured in native (uncompressed) or encapsulated
     *  (compressed) format.
     *  @param inStream      The stream which contains the information.
     *  @param ixfer         The transfer syntax which was used to encode the
     *                       information in inStream.
     *  @param glenc         Encoding type for group length; specifies what
     *                       will be done with group length tags.
     *  @param maxReadLength Maximum read length for reading an attribute value.
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition read(DcmInputStream &inStream,
                             const E_TransferSyntax ixfer,
                             const E_GrpLenEncoding glenc = EGL_noChange,
                             const Uint32 maxReadLength = DCM_MaxReadLength);

    /** writes a representation conforming to the transfer syntax
     *  It does not create a representation. If no conforming
     *  representation exists,  an error code is returned.
     *  The written representation is the new current representation
     *  @param outStream the output stream
     *  @param oxfer the transfer syntax that should be used
     *  @param enctype encoding types (undefined or explicit length)
     *  @param pointer to write cache object, may be NULL
     *  @return EC_Normal on success, the error otherwise
     */
    virtual OFCondition write(
      DcmOutputStream &outStream,
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

    /** write object in JSON format to a stream
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
    virtual OFCondition writeSignatureFormat(
      DcmOutputStream &outStream,
      const E_TransferSyntax oxfer,
      const E_EncodingType enctype,
      DcmWriteCache *wcache);

    /** this method loads all attribute values maintained by this object and
     *  all sub-objects (in case of a container such as DcmDataset) into memory.
     *  After a call to this method, the file from which a dataset was read may safely
     *  be deleted or replaced. For large files, this method may obviously allocate large
     *  amounts of memory.
     *  @return EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition loadAllDataIntoMemory(void);


    /** finalize the transfer state of this object. This method must be called
     *  when reading/writing this object from/to a stream has been completed.
     */
    virtual void transferEnd();

    /** test if it is possible to choose the representation in the parameters
     * @param repType the representation type
     * @param repParam the representation parameters
     */
    OFBool canChooseRepresentation(
        const E_TransferSyntax repType,
        const DcmRepresentationParameter * repParam);

    /** choose a specific representation.
     *  @remarks if representation does not exist it is created (if possible).
     *    if repParam is zero, a representation is chosen or created that
     *    is equal to the default representation parameters
     *  @deprecated The direct call of this method by user code is deprecated.
     *    Use DcmDataset::chooseRepresentation() instead.
     *  @repType the representation type
     *  @repParam the representation parameters
     *  @stack the object stack
     */
    OFCondition chooseRepresentation(
        const E_TransferSyntax repType,
        const DcmRepresentationParameter * repParam,
        DcmStack & stack);

    /** Inserts an original encapsulated representation.
     *  @remarks current and original representations are changed,
     *  all old representations are deleted
     *  @param repType the representation type
     *  @param repParam the representation parameters
     *  @param pixSeq the pixel sequence
     */
    void putOriginalRepresentation(
        const E_TransferSyntax repType,
        const DcmRepresentationParameter * repParam,
        DcmPixelSequence * pixSeq);

    /** insert an original unencapsulated representation.
     *  @remarks current and original representations are changed,
     *  all old representations are deleted. The array data is copied.
     *  @param byteValue used to check for corrupted data
     *  @param length the length of the element to put
     */
    virtual OFCondition putUint8Array(
        const Uint8 * byteValue,
        const unsigned long length);

    /** insert an original unencapsulated representation.
     *  @remarks current and original representations are changed,
     *  all old representations are deleted. The array data is copied.
     *  @param wordValue new attribute value
     *  @param length number of values in array vals
     */
    virtual OFCondition putUint16Array(
        const Uint16 * wordValue,
        const unsigned long length);

    /** create an empty Uint8 array of given number of bytes and set it.
     *  All array elements are initialized with a value of 0 (using 'memset').
     *  This method is only applicable to certain VRs, e.g. OB.
     *  @param numBytes number of bytes (8 bit) to be created
     *  @param bytes stores the pointer to the resulting buffer
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition createUint8Array(
        const Uint32 numBytes,
        Uint8 * & bytes);

    /** create an empty Uint16 array of given number of words and set it.
     *  All array elements are initialized with a value of 0 (using 'memset').
     *  This method is only applicable to OW data.
     *  @param numWords number of words (16 bit) to be created
     *  @param words stores the pointer to the resulting buffer
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition createUint16Array(
        const Uint32 numWords,
        Uint16 * & words);

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
    virtual OFCondition createValueFromTempFile(
      DcmInputStreamFactory *factory,
      const Uint32 length,
      const E_ByteOrder byteOrder);

    /** get a specific existing Representation, creates no representation
     *  if repParam is NULL, then the representation conforming to the default
     *  representationParameters (defined with the codec) is returned.
     *  @param repType the transfer syntax
     *  @param repParam representation parameters
     *  @param pixSeq the found representation
     */
    OFCondition getEncapsulatedRepresentation(
        const E_TransferSyntax repType,
        const DcmRepresentationParameter * repParam,
        DcmPixelSequence * & pixSeq);

    /** returns the representation identification (key) consisting of
     *  representation type and parameter of the original representation
     *  @param repType the transfer syntax
     *  @param the representations parameters
     */
    void getOriginalRepresentationKey(
        E_TransferSyntax & repType,
        const DcmRepresentationParameter * & repParam);

    /** returns the representation identification (key) consisting of
     *  representation type and parameter of the current representation
     *  @param repType the transfer Syntax
     *  @param repParam the DcmRepresentationParameter
     */
    void getCurrentRepresentationKey(
        E_TransferSyntax & repType,
        const DcmRepresentationParameter * & repParam);

    /** sets the representation identification parameter of the current representation
     * @param repParam the representation Parameter
     * @return EC_Normal on success, EC_RepresentationNotFound if the representation was not found
     */
    OFCondition setCurrentRepresentationParameter(
        const DcmRepresentationParameter * repParam);

    /** checks if a specific conforming representation exists.
     * @param repType the representation Type
     * @param repParam the representation Parameter
     * @return OFTrue if a specific conforming representation exists, OFFalse otherwise
     */
    OFBool hasRepresentation(
        const E_TransferSyntax repType,
        const DcmRepresentationParameter * repParam = NULL);

    /** deletes a representation.
     * @remark It is not possible to delete the original representation with this method
     * @param repType the representation Type
     * @param repParam the representation Parameter
     * @return EC_NORMAL on success, the error otherwise
     */
    OFCondition removeRepresentation(
        const E_TransferSyntax repType,
        const DcmRepresentationParameter * repParam);

    /** removes all but the original representation
     */
    void removeAllButOriginalRepresentations();

    /** removes all but the current representation.
     *  Makes the current representation original.
     */
    void removeAllButCurrentRepresentations();

    /** delete original representation and set new original representation.
     *  If the new representation does not exist, the original one is not
     *  deleted and an error code returns
     *  @param repType the representation type
     *  @param repParam the representation parameter
     *  @return EC_Normal on success, the error code otherwise
     */
    OFCondition removeOriginalRepresentation(
        const E_TransferSyntax repType,
        const DcmRepresentationParameter * repParam);

    /** set or clear the flag that indicates that this pixel data element will be
     *  written in uncompressed (defined length) format even if the dataset
     *  itself is written in a compressed syntax where pixel data is normally
     *  written in encapsulated (undefined length) format. By default this flag
     *  is false, unless the dataset was read in an encapsulated transfer syntax
     *  and this pixel data element was already present in uncompressed format.
     *  This flag should never be enabled for pixel data elements on the main dataset
     *  level, only for pixel data elements within the icon image sequence or some
     *  other "private" place.
     *  @param flag new value of flag
     */
    void setNonEncapsulationFlag(OFBool flag);

    /** access single frame without decompressing or loading a complete
     *  multi-frame object. The frame is copied into the buffer passed by the caller
     *  which must be large enough to contain a complete frame.
     *  @param dataset pointer to DICOM dataset in which this pixel data object is
     *    located. Used to access rows, columns, samples per pixel etc.
     *  @param frameNo number of frame, starting with 0 for the first frame.
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
     *  @param bufSize size of buffer, in bytes. This number must be even so
     *    that the data can properly be converted to a different endianness.
     *  @param decompressedColorModel upon successful return, the color model
     *    of the decompressed image (which may be different from the one used
     *    in the compressed images) is returned in this parameter.
     *  @param cache file cache object that may be passed to multiple subsequent calls
     *    to this method for the same file; the file cache will then keep a file
     *    handle open, thus improving performance. Optional, may be NULL
     *  @return EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getUncompressedFrame(
        DcmItem *dataset,
        Uint32 frameNo,
        Uint32& startFragment,
        void *buffer,
        Uint32 bufSize,
        OFString& decompressedColorModel,
        DcmFileCache *cache=NULL);

    /** determine color model of the decompressed image
     *  @param dataset pointer to DICOM dataset in which this pixel data object
     *    is located. Used to access photometric interpretation.
     *  @param decompressedColorModel upon successful return, the color model
     *    of the decompressed image (which may be different from the one used
     *    in the compressed images) is returned in this parameter
     *  @return EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getDecompressedColorModel(
        DcmItem *dataset,
        OFString &decompressedColorModel);
};

#endif
