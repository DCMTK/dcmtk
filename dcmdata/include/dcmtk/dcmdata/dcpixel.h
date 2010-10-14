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
 *  Author:  Andreas Barth
 *
 *  Purpose: Interface of class DcmPixelData
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:15:41 $
 *  CVS/RCS Revision: $Revision: 1.40 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
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
class DcmRepresentationParameter
{
public:
    /// default constructor
    DcmRepresentationParameter() {}

    /// copy constructor
    DcmRepresentationParameter(const DcmRepresentationParameter&) {}

    /// destructor
    virtual ~DcmRepresentationParameter() {}

    /** this methods creates a copy of type DcmRepresentationParameter *
     *  it must be overweritten in every subclass.
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
class DcmRepresentationEntry
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
class DcmPixelData : public DcmPolymorphOBOW
{
private:
    friend class DcmRepresentationEntry;

    /// List of representations of pixel data
    DcmRepresentationList repList;

    /// Iterator to the last dummy element in representation lis
    DcmRepresentationListIterator repListEnd;

    /// Iterator to the original representation. if an uncompressed
    /// representation is used the iterator points to repList.end()
    DcmRepresentationListIterator original;

    /// current list element for some operations
    DcmRepresentationListIterator current;

    /// shows if an unecapsulated representation is stored
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
     */
    OFCondition findRepresentationEntry(
        const DcmRepresentationEntry & findEntry,
        DcmRepresentationListIterator & result);

    /** insert or replace a representation entry in the list
     */
    DcmRepresentationListIterator insertRepresentationEntry(
        DcmRepresentationEntry * repEntry);

    /** decode representation to unencapsulated format
     */
    OFCondition decode(
        const DcmXfer & fromType,
        const DcmRepresentationParameter * fromParam,
        DcmPixelSequence * fromPixSeq,
        DcmStack & pixelStack);

    /** encode to encapsulated format
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

    /** constructor
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

    /** set/change the current value representation of the uncompressed image representation, if any
     *  @param vr new value representation to be set.  All VRs except for OW (Other
     *    Word String) are treated as 8 bit data (OB).  This is particularily useful
     *    for unknown (UN) or unsupported VRs.
     *  @return status status, EC_Normal if successful, an error code otherwise
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
    virtual void print(STD_NAMESPACE ostream&out,
                       const size_t flags = 0,
                       const int level = 0,
                       const char *pixelFileName = NULL,
                       size_t *pixelCounter = NULL);

    /** tests if it is possible to write a specific representation
     *  Only existing representations are considered, since this
     *  method does not create a representation.
     */
    virtual OFBool canWriteXfer(const E_TransferSyntax newXfer,
                                const E_TransferSyntax oldXfer);

    /** returns length of representation conforming to the
     *  transfer syntax with tag, vr, ... It does not create a
     *  representation. If no conforming representation exists an
     *  error code is set and 0 returned.
     */
    virtual Uint32 calcElementLength(const E_TransferSyntax xfer,
                                     const E_EncodingType enctype);

    /** returns length of representation value field conforming to
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
    virtual OFCondition read(DcmInputStream & inStream,
                             const E_TransferSyntax ixfer,
                             const E_GrpLenEncoding glenc = EGL_noChange,
                             const Uint32 maxReadLength = DCM_MaxReadLength);

    /** writes a representation conforming to the transfer syntax
     *  It does not create a representation. If no conforming
     *  representation exists,  an error code is returned.
     *  The written representation is the new current representation
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
    virtual OFCondition writeXML(STD_NAMESPACE ostream&out,
                                 const size_t flags = 0);

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
     */
    OFBool canChooseRepresentation(
        const E_TransferSyntax repType,
        const DcmRepresentationParameter * repParam);

    /** choose a specific representation. if representation does not exist
     *  it is created (if possible).
     *  if repParam is zero, a representation is chosen or created that
     *  is equal to the default representation parameters
     */
    OFCondition chooseRepresentation(
        const E_TransferSyntax repType,
        const DcmRepresentationParameter * repParam,
        DcmStack & stack);

    /** Inserts an original encapsulated representation. current and original
     *  representations are changed, all old representations are deleted
     */
    void putOriginalRepresentation(
        const E_TransferSyntax repType,
        const DcmRepresentationParameter * repParam,
        DcmPixelSequence * pixSeq);

    /**insert an original unencapsulated
     *  representation. current and original representations are changed,
     *  all old representations are deleted. The array data is copied.
     */
    virtual OFCondition putUint8Array(
        const Uint8 * byteValue,
        const unsigned long length);

    /** insert an original unencapsulated
     *  representation. current and original representations are changed,
     *  all old representations are deleted. The array data is copied.
     */
    virtual OFCondition putUint16Array(
        const Uint16 * wordValue,
        const unsigned long length);

    /** create an empty Uint8 array of given number of words and set it
     */
    virtual OFCondition createUint8Array(
        const Uint32 numBytes,
        Uint8 * & bytes);

    /** create an empty Uint16 array of given number of words and set it
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

    /** get a specific exisiting Representation, creates no representation
     *  if repParam is NULL, then the representation conforming to the default
     *  presentationParameters (defined with the codec) is returned.
     */
    OFCondition getEncapsulatedRepresentation(
        const E_TransferSyntax repType,
        const DcmRepresentationParameter * repParam,
        DcmPixelSequence * & pixSeq);

    /** returns the representation identification (key) consisting of
     *  representation type and parameter of the original representation
     */
    void getOriginalRepresentationKey(
        E_TransferSyntax & repType,
        const DcmRepresentationParameter * & repParam);

    /** returns the representation identification (key) consisting of
     *  representation type and parameter of the current representation
     */
    void getCurrentRepresentationKey(
        E_TransferSyntax & repType,
        const DcmRepresentationParameter * & repParam);

    /** sets the representation identification parameter of the current
     *  representation
     */
    OFCondition setCurrentRepresentationParameter(
        const DcmRepresentationParameter * repParam);

    /** returns if a specific conforming  representation exists.
     */
    OFBool hasRepresentation(
        const E_TransferSyntax repType,
        const DcmRepresentationParameter * repParam = NULL);

    /** delete a representation. It is not possible to delete the
     *  original representation with this method
     */
    OFCondition removeRepresentation(
        const E_TransferSyntax repType,
        const DcmRepresentationParameter * repParam);

    /** removes all but the original representation
     */
    void removeAllButOriginalRepresentations();

    /** removes all but the current representation
     *  Makes the current representation original
     */
    void removeAllButCurrentRepresentations();

    /** delete original representation and set new original representation.
     *  If the new representation does not exist, the original one is not
     *  deleted and an error code returns
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


/*
** CVS/RCS Log:
** $Log: dcpixel.h,v $
** Revision 1.40  2010-10-14 13:15:41  joergr
** Updated copyright header. Added reference to COPYRIGHT file.
**
** Revision 1.39  2010-07-02 12:34:42  uli
** Added comment explaining why an even buffer size is required.
**
** Revision 1.38  2010-02-22 11:39:54  uli
** Remove some unneeded includes.
**
** Revision 1.37  2009-11-17 16:36:51  joergr
** Added new method that allows for determining the color model of the
** decompressed image.
**
** Revision 1.36  2008-11-03 14:32:50  joergr
** Added method createValueFromTempFile() - overrides method in DcmElement.
**
** Revision 1.35  2008-07-17 11:19:49  onken
** Updated copyFrom() documentation.
**
** Revision 1.34  2008-07-17 10:30:23  onken
** Implemented copyFrom() method for complete DcmObject class hierarchy, which
** permits setting an instance's value from an existing object. Implemented
** assignment operator where necessary.
**
** Revision 1.33  2008-06-23 12:09:13  joergr
** Fixed inconsistencies in Doxygen API documentation.
**
** Revision 1.32  2008-05-29 10:46:13  meichel
** Implemented new method DcmPixelData::getUncompressedFrame
**   that permits frame-wise access to compressed and uncompressed
**   objects without ever loading the complete object into main memory.
**   For this new method to work with compressed images, all classes derived from
**   DcmCodec need to implement a new method decodeFrame(). For now, only
**   dummy implementations returning an error code have been defined.
**
** Revision 1.31  2007/11/29 14:30:19  meichel
** Write methods now handle large raw data elements (such as pixel data)
**   without loading everything into memory. This allows very large images to
**   be sent over a network connection, or to be copied without ever being
**   fully in memory.
**
** Revision 1.30  2007/06/29 14:17:49  meichel
** Code clean-up: Most member variables in module dcmdata are now private,
**   not protected anymore.
**
** Revision 1.29  2006/08/15 15:49:56  meichel
** Updated all code in module dcmdata to correctly compile when
**   all standard C++ classes remain in namespace std.
**
** Revision 1.28  2005/12/08 16:28:30  meichel
** Changed include path schema for all DCMTK header files
**
** Revision 1.27  2005/05/26 09:06:53  meichel
** Renamed isIconImage flag to alwaysUnencapsulated to clarify meaning.
**   Added public method DcmPixelData::setNonEncapsulationFlag() that allows
**   DcmCodec instances to enable the flag. Improved documentation.
**
** Revision 1.26  2004/07/01 12:28:25  meichel
** Introduced virtual clone method for DcmObject and derived classes.
**
** Revision 1.25  2004/04/07 13:55:56  meichel
** Compressed image datasets containing uncompressed icon images
**   are now correctly handled by the parser.
**
** Revision 1.24  2004/01/16 14:06:20  joergr
** Removed acknowledgements with e-mail addresses from CVS log.
**
** Revision 1.23  2003/06/12 18:21:24  joergr
** Modified code to use const_iterators where appropriate (required for STL).
**
** Revision 1.22  2003/06/12 14:03:24  joergr
** Fixed inconsistent API documentation reported by Doxygen.
**
** Revision 1.21  2003/06/02 16:55:34  meichel
** Cleaned up implementation of DcmRepresentationEntry, added doc++ comments
**
** Revision 1.20  2003/04/17 15:56:59  joergr
** Corrected API documentation of createUint8/16Array() methods.
**
** Revision 1.19  2003/04/01 12:35:12  joergr
** Added implementation of createUint8/16Array() methods to DcmPixelData.
** Required to work properly with chooseRepresentation() for pixel compression.
**
** Revision 1.18  2002/12/09 09:31:16  wilkens
** Modified/Added doc++ documentation.
**
** Revision 1.17  2002/12/06 12:49:12  joergr
** Enhanced "print()" function by re-working the implementation and replacing
** the boolean "showFullData" parameter by a more general integer flag.
** Added doc++ documentation.
** Made source code formatting more consistent with other modules/files.
**
** Revision 1.16  2002/08/27 16:55:37  meichel
** Initial release of new DICOM I/O stream classes that add support for stream
**   compression (deflated little endian explicit VR transfer syntax)
**
** Revision 1.15  2002/04/25 09:38:48  joergr
** Added support for XML output of DICOM objects.
**
** Revision 1.14  2001/09/25 17:18:34  meichel
** Updated abstract class DcmRepresentationParameter for use with dcmjpeg
**
** Revision 1.13  2001/06/01 15:48:42  meichel
** Updated copyright header
**
** Revision 1.12  2000/11/07 16:56:08  meichel
** Initial release of dcmsign module for DICOM Digital Signatures
**
** Revision 1.11  2000/09/27 08:19:55  meichel
** Minor changes in DcmCodec interface, required for future dcmjpeg module.
**
** Revision 1.10  2000/04/14 15:31:33  meichel
** Removed default value from output stream passed to print() method.
**   Required for use in multi-thread environments.
**
** Revision 1.9  2000/03/08 16:26:17  meichel
** Updated copyright header.
**
** Revision 1.8  2000/03/03 14:05:25  meichel
** Implemented library support for redirecting error messages into memory
**   instead of printing them to stdout/stderr for GUI applications.
**
** Revision 1.7  2000/02/10 10:50:52  joergr
** Added new feature to dcmdump (enhanced print method of dcmdata): write
** pixel data/item value fields to raw files.
**
** Revision 1.6  2000/02/03 16:28:10  joergr
** Fixed bug: encapsulated data (pixel items) have never been loaded using
** method 'loadAllDataIntoMemory'. Therefore, encapsulated pixel data was
** never printed with 'dcmdump'.
**
** Revision 1.5  1999/03/31 09:24:44  meichel
** Updated copyright header in module dcmdata
**
** Revision 1.4  1998/11/12 16:47:42  meichel
** Implemented operator= for all classes derived from DcmObject.
**
** Revision 1.3  1998/07/15 15:48:50  joergr
** Removed several compiler warnings reported by gcc 2.8.1 with
** additional options, e.g. missing copy constructors and assignment
** operators, initialization of member variables in the body of a
** constructor instead of the member initialization list, hiding of
** methods by use of identical names, uninitialized member variables,
** missing const declaration of char pointers. Replaced tabs by spaces.
**
** Revision 1.2  1997/07/24 13:08:24  andreas
** - Removed const for method DcmRepresentationParameter::copy
**
** Revision 1.1  1997/07/21 07:54:57  andreas
** - New environment for encapsulated pixel representations. DcmPixelData
**   can contain different representations and uses codecs to convert
**   between them. Codecs are derived from the DcmCodec class. New error
**   codes are introduced for handling of representations. New internal
**   value representation (only for ident()) for PixelData
**
*/
