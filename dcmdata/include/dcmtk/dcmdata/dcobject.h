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
 *  Author:  Gerd Ehlers
 *
 *  Purpose:
 *  This file contains the interface to routines which provide
 *  DICOM object encoding/decoding, search and lookup facilities.
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-29 10:57:17 $
 *  CVS/RCS Revision: $Revision: 1.70 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#ifndef DCOBJECT_H
#define DCOBJECT_H

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/ofstd/ofglobal.h"
#include "dcmtk/dcmdata/dcerror.h"
#include "dcmtk/dcmdata/dcxfer.h"
#include "dcmtk/dcmdata/dctag.h"
#include "dcmtk/dcmdata/dcstack.h"


// forward declarations
class DcmOutputStream;
class DcmInputStream;
class DcmWriteCache;

// Undefined Length Identifier now defined in dctypes.h

// Maxinum number of read bytes for a Value Element
const Uint32 DCM_MaxReadLength = 4096;

// Maximum length of tag and length in a DICOM element
const Uint32 DCM_TagInfoLength = 12;

// Optimum line length if not all data printed
const Uint32 DCM_OptPrintLineLength = 70;

// Optimum value length if not all data printed
const Uint32 DCM_OptPrintValueLength = 40;

// Optimum attribute name length (for tree output)
const Uint32 DCM_OptPrintAttributeNameLength = 35;

/** This flags defines whether automatic correction should be applied to input
 *  data (e.g. stripping of padding blanks, removal of blanks in UIDs, etc).
 *  Default is enabled.
 */
extern OFGlobal<OFBool> dcmEnableAutomaticInputDataCorrection; /* default OFTrue */

/** This flag defines the handling of illegal odd-length attributes: If flag is
 *  true, odd lengths are respected (i.e. an odd number of bytes is read from
 *  the input stream.) After successful reading, padding to even number of bytes
 *  is enforced by adding a zero pad byte if dcmEnableAutomaticInputDataCorrection
 *  is true. Otherwise the odd number of bytes remains as read.
 *
 *  If flag is false, old (pre DCMTK 3.5.2) behaviour applies: The length field
 *  implicitly incremented and an even number of bytes is read from the stream.
 */
extern OFGlobal<OFBool> dcmAcceptOddAttributeLength; /* default OFTrue */

/** This flag defines how UN attributes with undefined length are treated
 *  by the parser when reading. The default is to expect the content of the
 *  UN element (up to and including the sequence delimitation item)
 *  to be encoded in Implicit VR Little Endian, as described in CP 246.
 *  DCMTK expects the attribute to be encoded like a DICOM sequence, i.e.
 *  the content of each item is parsed as a DICOM dataset.
 *  If the flag is disabled old (pre DCMTK 3.5.4) behaviour applies: The
 *  attribute is treated as if it was an Explicit VR SQ element.
 *
 *  Note that the flag only affects the read behaviour but not the write
 *  behaviour - DCMTK will never write UN elements with undefined length.
 */
extern OFGlobal<OFBool> dcmEnableCP246Support; /* default OFTrue */

/** DCMTK releases up to 3.5.3 created a non-conforming byte stream
 *  as input to the MAC algorithm when creating or verifying digital signatures
 *  including compressed pixel data (i.e. signatures including attribute
 *  (7FE0,0010) in an encapsulated transfer syntax). This has been fixed
 *  in DCMTK 3.5.4, but this flag allows to revert to the old behavior
 *  in order to create or verify signatures that are compatible with older
 *  releases. Default is "off" (OFFalse).
 */
extern OFGlobal<OFBool> dcmEnableOldSignatureFormat; /* default OFFalse */

/** This flag defines whether the transfer syntax for uncompressed datasets
 *  is detected automatically.  The automatic detection has been introduced
 *  since there are (incorrectly encoded) DICOM dataset stored with a
 *  different transfer syntax than specified in the meta header.
 */
extern OFGlobal<OFBool> dcmAutoDetectDatasetXfer; /* default OFFalse */

/** This flag defines how non-standard VRs are treated by the parser when
 *  reading. The default is to treat data element with non-standard VR as
 *  unknown. If this flag is enabled, the parser will try to read the data
 *  element with Implicit VR Little Endian transfer syntax.
 */
extern OFGlobal<OFBool> dcmAcceptUnexpectedImplicitEncoding; /* default OFFalse */

/** This flag indicates, whether private attributes with implicit transfer
 *  syntax having a maximum length should be handled as sequences (ignoring
 *  any dictionary entries for that tag). This can happen, if for example
 *  a private creator element is illegally inserted with VR SQ
 *  (undefined length and implicit coding). The parser usually would then
 *  try to parse the element with VR=LO (private creator) with maximum
 *  length, which would lead to an error. The default behaviour is to
 *  rely on the dictionary.
 */
extern OFGlobal<OFBool> dcmReadImplPrivAttribMaxLengthAsSQ; /* default OFFalse */

/** This flag indicates, whether parsing errors during reading
 *  should be ignored, ie whether the parser should try to recover and
 *  parse the rest of the stream.
 *  This flag does not work for all parsing errors (at this time)
 *  making sense but was introduced afterwards.
 */
extern OFGlobal<OFBool> dcmIgnoreParsingErrors; /* default OFFalse */

/** This flag indicates, whether parsing should stop after a certain
 *  element in the stream was parsed. This is especially useful for
 *  datasets containing garbage at the end, usually after the Pixel
 *  Data attribute. To prevent the parser for "stumbling" over that
 *  garbage, it is possible to tell the parser to stop after a
 *  specific element. The flag is only sensitive to elements on
 *  dataset level, ie. inside sequence any occurence of the specified
 *  tag is ignored. Caution: Note that if Pixel Data is chosen
 *  as stop element, any attributes behind will not be parsed, e. g.
 *  any digital signature attributes coming after.
 *  Default is (0xffff,0xffff), which means that the feature is
 *  disabled.
 */
extern OFGlobal<DcmTagKey> dcmStopParsingAfterElement; /* default OFTrue */

/** This flag influences behaviour when writing a dataset with items
 *  and sequences set to be encoded with explicit length. It is possible
 *  that the content of a sequence (or item) has an encoded length greater
 *  than the maximum 32-bit value that can be written to the sequence (item)
 *  length field. If this flag is enabled (OFTrue) then the encoding of the
 *  very sequence (item) is switched to undefined length encoding. Any
 *  contained items (sequences) will be encoded explicitly if possible.
 *  Default is OFTrue, i.e. encoding is switched to implicit if maximum
 *  size of length field is exceeded.
 */
extern OFGlobal<OFBool> dcmWriteOversizedSeqsAndItemsUndefined; /* default OFTrue */

/** This flag allows for ignoring the value of (0002,0000) File Meta Information
 *  Group Length which is useful in cases where this value is incorrect.  If the
 *  header length is ignored, the behavior is identical to the case when no value
 *  is available (i.e. all elements are read as long as the group number is 0x0002).
 */
extern OFGlobal<OFBool> dcmIgnoreFileMetaInformationGroupLength; /* default OFFalse */


/** Abstract base class for most classes in module dcmdata. As a rule of thumb,
 *  everything that is either a dataset or that can be identified with a DICOM
 *  attribute tag is derived from class DcmObject.
 */
class DcmObject
{
  public:

    /** constructor.
     *  Create new object from given tag and length.
     *  @param tag DICOM tag for the new element
     *  @param len value length for the new element
     */
    DcmObject(const DcmTag &tag, const Uint32 len = 0);

    /** copy constructor
     *  @param obj item to be copied
     */
    DcmObject(const DcmObject &obj);

    /// destructor
    virtual ~DcmObject();

    /** clone method
     *  @return deep copy of this object
     */
    virtual DcmObject *clone() const = 0;

    /** copy assignment operator
     *  @param obj object to be copied
     *  @return reference to this object
     */
    DcmObject &operator=(const DcmObject &obj);

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
    virtual OFCondition copyFrom(const DcmObject &rhs) = 0;

    /** return identifier for this class. Every class derived from this class
     *  returns a unique value of type enum DcmEVR for this call. This is used
     *  as a "poor man's RTTI" to correctly identify instances derived from
     *  this class even on compilers not supporting RTTI.
     *  @return type identifier of this class
     */
    virtual DcmEVR ident() const = 0;

    /** return the value representation assigned to this object.
     *  If object was read from a stream, this method returns the VR
     *  that was defined in the stream for this object. It is, therefore,
     *  possible that the VR does not match the one defined in the data
     *  dictionary for the tag assigned to this object.
     *  @return VR of this object
     */
    inline DcmEVR getVR() const { return Tag.getEVR(); }

    /** check if this element is a string type, based on the VR.
     *  Since the check is based on the VR and not on the class,
     *  the result of this method is not a guarantee that the object
     *  can be safely casted to one of the string-VR subclasses.
     *  @return true if this object is a string VR, false otherwise
     */
    inline OFBool isaString() const { return Tag.getVR().isaString(); }

    /** check if this element is a leaf node in a dataset tree.
     *  All subclasses of DcmElement except for DcmSequenceOfItems
     *  are leaf nodes, while DcmSequenceOfItems, DcmItem, DcmDataset etc.
     *  are not.
     *  @return true if leaf node, false otherwise.
     */
    virtual OFBool isLeaf() const = 0;

    /** print object to a stream
     *  @param out output stream
     *  @param flags optional flag used to customize the output (see DCMTypes::PF_xxx)
     *  @param level current level of nested items. Used for indentation.
     *  @param pixelFileName not used (used in certain sub-classes of this class)
     *  @param pixelCounter not used (used in certain sub-classes of this class)
     */
    virtual void print(STD_NAMESPACE ostream &out,
                       const size_t flags = 0,
                       const int level = 0,
                       const char *pixelFileName = NULL,
                       size_t *pixelCounter = NULL) = 0;

    /** return the current transfer (read/write) state of this object.
     *  @return transfer state of this object
     */
    inline E_TransferState transferState() const { return fTransferState; }

    /** initialize the transfer state of this object. This method must be called
     *  before this object is written to a stream or read (parsed) from a stream.
     */
    virtual void transferInit(void);

    /** finalize the transfer state of this object. This method must be called
     *  when reading/writing this object from/to a stream has been completed.
     */
    virtual void transferEnd(void);

    /** return the group number of the attribute tag for this object
     *  @return group number of the attribute tag for this object
     */
    inline Uint16 getGTag() const { return Tag.getGTag(); }

    /** return the element number of the attribute tag for this object
     *  @return element number of the attribute tag for this object
     */
    inline Uint16 getETag() const { return Tag.getETag(); }

    /** return const reference to the attribute tag for this object
     *  @return const reference to the attribute tag for this object
     */
    inline const DcmTag &getTag() const { return Tag; }

    /** assign group tag (but not element tag) of the attribute tag for this object.
     *  This is sometimes useful when creating repeating group elements.
     *  @param gtag new attribute group tag
     */
    inline void setGTag(Uint16 gtag) { Tag.setGroup(gtag); }

    /** assign a new Value Representation (VR) to this object. This operation
     *  is only supported for very few subclasses derived from this class,
     *  in particular for classes handling pixel data which may either be
     *  of OB or OW value representation.
     *  @param vr value representation
     *  @return EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setVR(DcmEVR /*vr*/) { return EC_IllegalCall; }

    /** return value multiplicity of the current object
     *  @return value multiplicity of the current object
     */
    virtual unsigned long getVM() = 0;

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
                                     const E_EncodingType enctype) = 0;

    /** calculate the value length (without attribute tag, VR and length field)
     *  of this DICOM element when encoded with the given transfer syntax and
     *  the given encoding type for sequences. Never returns undefined length.
     *  @param xfer transfer syntax for length calculation
     *  @param enctype sequence encoding type for length calculation
     *  @return value length of DICOM element
     */
    virtual Uint32 getLength(const E_TransferSyntax xfer = EXS_LittleEndianImplicit,
                             const E_EncodingType enctype = EET_UndefinedLength) = 0;

    /** check if this DICOM object can be encoded in the given transfer syntax.
     *  @param newXfer transfer syntax in which the DICOM object is to be encoded
     *  @param oldXfer transfer syntax in which the DICOM object was read or created.
     *  @return true if object can be encoded in desired transfer syntax, false otherwise.
     */
    virtual OFBool canWriteXfer(const E_TransferSyntax newXfer,
                                const E_TransferSyntax oldXfer) = 0;

    /** read object from a stream.
     *  @param inStream DICOM input stream
     *  @param ixfer transfer syntax to use when parsing
     *  @param glenc handling of group length parameters
     *  @param maxReadLength attribute values larger than this value are skipped
     *    while parsing and read later upon first access if the stream type supports
     *    this.
     *  @return EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition read(DcmInputStream &inStream,
                             const E_TransferSyntax ixfer,
                             const E_GrpLenEncoding glenc = EGL_noChange,
                             const Uint32 maxReadLength = DCM_MaxReadLength) = 0;

    /** write object to a stream (abstract)
     *  @param outStream DICOM output stream
     *  @param oxfer output transfer syntax
     *  @param enctype encoding types (undefined or explicit length)
     *  @param wcache pointer to write cache object, may be NULL
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition write(DcmOutputStream &outStream,
                              const E_TransferSyntax oxfer,
                              const E_EncodingType enctype,
                              DcmWriteCache *wcache) = 0;

    /** write object in XML format to a stream
     *  @param out output stream to which the XML document is written
     *  @param flags optional flag used to customize the output (see DCMTypes::XF_xxx)
     *  @return status, always returns EC_Illegal Call
     */
    virtual OFCondition writeXML(STD_NAMESPACE ostream&out,
                                 const size_t flags = 0);

    /** special write method for creation of digital signatures (abstract)
     *  @param outStream DICOM output stream
     *  @param oxfer output transfer syntax
     *  @param enctype encoding types (undefined or explicit length)
     *  @param wcache pointer to write cache object, may be NULL
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition writeSignatureFormat(DcmOutputStream &outStream,
                                             const E_TransferSyntax oxfer,
                                             const E_EncodingType enctype,
                                             DcmWriteCache *wcache) = 0;

    /** returns true if the current object may be included in a digital signature
     *  @return true if signable, false otherwise
     */
    virtual OFBool isSignable() const;

    /** returns true if the object contains an element with Unknown VR at any nesting level
     *  @return true if the object contains an element with Unknown VR, false otherwise
     */
    virtual OFBool containsUnknownVR() const;

    /** check if this object contains non-ASCII characters
     *  @param checkAllStrings not used in this class
     *  @return always returns false, i.e. no extended characters used
     */
    virtual OFBool containsExtendedCharacters(const OFBool checkAllStrings = OFFalse);

    /** check if this object is affected by SpecificCharacterSet
     *  @return always returns false, i.e. not affected by SpecificCharacterSet
     */
    virtual OFBool isAffectedBySpecificCharacterSet() const;

    /** check if this object is empty
     *  @param normalize normalize value before checking (ignore non-significant characters)
     *  @return true if object is empty, i.e. has no value, false otherwise
     */
    virtual OFBool isEmpty(const OFBool normalize = OFTrue);

    /** clear (remove) attribute value
     *  @return EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition clear() = 0;

    /** check the currently stored element value
     *  @param autocorrect correct value length if OFTrue
     *  @return status, EC_Normal if value length is correct, an error code otherwise
     */
    virtual OFCondition verify(const OFBool autocorrect = OFFalse) = 0;

    /** this method is only used in container classes derived from this class,
     *  that is, DcmItem and DcmSequenceOfItems. It returns a pointer to the
     *  next object in the list AFTER the given object. If the caller passes NULL,
     *  a pointer to the first object in the list is returned. If the given object
     *  is not found, the given object is the last one in the list or the list is empty,
     *  NULL is returned.
     *  @param obj pointer to one object in the container; we are looking for the
     *    next entry after this one. NULL if looking for the first entry.
     *  @return pointer to next object in container or NULL if not found
     */
    virtual DcmObject *nextInContainer(const DcmObject *obj);

    /** this method enables a stack based, depth-first traversal of a complete
     *  hierarchical DICOM dataset (that is, classes derived from DcmItem or
     *  DcmSequenceOfItems). With each call of this method, the next object
     *  in the tree is located and marked on the stack.
     *  @param stack "cursor" for current position in the dataset. The stack
     *    will contain a pointer to each dataset, sequence, item and element
     *    from the main dataset down to the current element, and is updated
     *    upon each call to this method. An empty stack is equivalent to a stack
     *    containing a pointer to this object only.
     *  @param intoSub if true, the nextObject method will perform a hierarchical
     *    search through the dataset (depth-first), if false, only the current
     *    container object will be traversed (e.g., all elements of an item
     *    or all items of a sequence).
     *  @return EC_Normal if value length is correct, an error code otherwise
     */
    virtual OFCondition nextObject(DcmStack &stack,
                                   const OFBool intoSub);

    /** a complex, stack-based, hierarchical search method. It allows for a search
     *  for a DICOM object with a given attribute within a given container,
     *  hierarchically, from a starting position identified through a cursor stack.
     *  @param xtag the DICOM attribute tag we are searching for
     *  @param resultStack Depending on the search mode (see below), this parameter
     *     either serves as an input and output parameter, or as an output parameter
     *     only (the latter being the default). When used as an input parameter,
     *     the cursor stack defines the start position for the search within a
     *     hierarchical DICOM dataset. Upon successful return, the stack contains
     *     the position of the element found, in the form of a pointer to each dataset,
     *     sequence, item and element from the main dataset down to the found element.
     *  @param mode search mode, controls how the search stack is handled.
     *     In the default mode, ESM_fromHere, the stack is ignored on input, and
     *     the search starts in the object for which this method is called.
     *     In the other modes, the stack is used both as an input and an output
     *     parameter and defines the starting point for the search.
     *  @param searchIntoSub if true, the search will be performed hierarchically descending
     *    into the sequences and items of the dataset. If false, only the current container
     *    (sequence or item) will be traversed.
     *  @return EC_Normal if found, EC_TagNotFound if not found, an error code is something went wrong.
     */
    virtual OFCondition search(const DcmTagKey &xtag,
                               DcmStack &resultStack,
                               E_SearchMode mode = ESM_fromHere,
                               OFBool searchIntoSub = OFTrue);

    /** this method loads all attribute values maintained by this object and
     *  all sub-objects (in case of a container such as DcmDataset) into memory.
     *  After a call to this method, the file from which a dataset was read may safely
     *  be deleted or replaced. For large files, this method may obviously allocate large
     *  amounts of memory.
     *  @return EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition loadAllDataIntoMemory() = 0;

    /** return the current value of the Length field (which is different from the functionality
     *  of the public getLength method). Only needed for internal purposes and for checker tools
     *  that verify values against the length field.
     *  @return current value of length field
     */
    Uint32 getLengthField() const { return Length; }

 protected:

    /** print line indentation, e.g. a couple of spaces for each nesting level.
     *  Depending on the value of 'flags' other visualizations are also possible.
     *  @param out output stream
     *  @param flags used to customize the output (see DCMTypes::PF_xxx)
     *  @param level current level of nested items. Used for indentation.
     */
    void printNestingLevel(STD_NAMESPACE ostream &out,
                           const size_t flags,
                           const int level);

    /** print beginning of the info line.
     *  The default output is tag and value representation, though other
     *  visualizations are possible depending on the value of 'flags'.
     *  @param out output stream
     *  @param flags used to customize the output (see DCMTypes::PF_xxx)
     *  @param level current level of nested items. Used for indentation.
     *  @param tag optional tag used to print the data element information
     */
    void printInfoLineStart(STD_NAMESPACE ostream &out,
                            const size_t flags,
                            const int level,
                            DcmTag *tag = NULL);

    /** print end of the info line.
     *  The default output is length, value multiplicity and tag name, though
     *  other visualizations are possible depending on the value of 'flags'.
     *  @param out output stream
     *  @param flags used to customize the output (see DCMTypes::PF_xxx)
     *  @param printedLength number of characters printed after line start.
     *    Used for padding purposes.
     *  @param tag optional tag used to print the data element information
     */
    void printInfoLineEnd(STD_NAMESPACE ostream &out,
                          const size_t flags,
                          const unsigned long printedLength = 0xffffffff /*no padding*/,
                          DcmTag *tag = NULL);

    /** print given text with element information.
     *  Calls printInfoLineStart() and printInfoLineEnd() to frame the
     *  'info' text.
     *  @param out output stream
     *  @param flags used to customize the output (see DCMTypes::PF_xxx)
     *  @param level current level of nested items. Used for indentation.
     *  @param info text to be printed
     *  @param tag optional tag used to print the data element information
     *  @param isInfo optional flag indicating whether this text is really given for
     *    informational purposes only. Used to choose the correct output color.
     */
    virtual void printInfoLine(STD_NAMESPACE ostream &out,
                               const size_t flags,
                               const int level = 0,
                               const char *info = NULL,
                               DcmTag *tag = NULL,
                               const OFBool isInfo = OFTrue);

    /** static helper function that writes a given attribute tag to a binary
     *  output stream using the byte order indicated by the transfer syntax.
     *  @param outStream output stream
     *  @param tag tag to write to the stream
     *  @param oxfer transfer syntax defining the byte order
     *  @return EC_Normal if successful, an error code otherwise
     */
    static OFCondition writeTag(DcmOutputStream &outStream,
                                const DcmTag &tag,
                                const E_TransferSyntax oxfer);

    /** write tag, VR and length field to the given output stream
     *  @param outStream output stream
     *  @param oxfer transfer syntax for writing
     *  @param writtenBytes number of bytes written to stream returned in this parameter
     *  @return EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition writeTagAndLength(DcmOutputStream &outStream,
                                          const E_TransferSyntax oxfer, // in
                                          Uint32 &writtenBytes) const;  // out

    /** return the number of bytes needed to serialize the
     *  tag, VR and length information of the current object using the given
     *  transfer syntax.
     *  @param oxfer The transfer syntax used for encoding
     *  @return number of bytes, may be 8 or 12 depending on VR and transfer syntax.
     */
    virtual Uint32 getTagAndLengthSize(const E_TransferSyntax oxfer) const;

	/** return the DICOM attribute tag name for this object. If not known yet, will
	 *  be looked up in the dictionary and cached. Therefore, method is not const.
	 *  @return tag name for this attribute
	 */
	const char *getTagName() { return Tag.getTagName(); }

    /** set the VR for this attribute
     *  @param vr new VR for this attribute.
     */
    void setTagVR(DcmEVR vr) { Tag.setVR(vr); }

    /** return the current transfer state of this object during serialization/deserialization
     *  @return current transfer state of this object
     */
    E_TransferState getTransferState() const { return fTransferState; }

    /** set the current transfer state of this object during serialization/deserialization
     *  @param newState new transfer state of this object
     */
    void setTransferState(E_TransferState newState) { fTransferState = newState; }

    /** return the number of transferred bytes for this object during serialization/deserialization
     *  @return number of transferred bytes
     */
    Uint32 getTransferredBytes() const { return fTransferredBytes; }

    /** set the number of transferred bytes for this object during serialization/deserialization
     *  @param val number of transferred bytes
     */
    void setTransferredBytes(Uint32 val) { fTransferredBytes = val; }

    /** add to the number of transferred bytes for this object during serialization/deserialization
     *  @param val number of additional transferred bytes to add to existing value
     */
    void incTransferredBytes(Uint32 val) { fTransferredBytes += val; }

    /** set the current value of the Length field
     *  @param val new value of the Length field
     */
    void setLengthField(Uint32 val) { Length = val; }

public:
    /** helper class to print a DcmObject to an ostream using operator<<
     */
    class PrintHelper
    {
      private:
        /** Undefined assignment operator. This is needed to work around a
         *  compiler warning on VC2008 with the highest warning level.
         */
        PrintHelper& operator=(PrintHelper &);

      public:
        /** construct a PrintHelper
         *  @param dcmobj DcmObject you want to print
         *  @param flags flags to use for DcmObject::print()
         *  @param level level to use for DcmObject::print()
         */
        explicit PrintHelper(DcmObject &dcmobj, size_t flags = 0, int level = 0)
          : dcmobj_(dcmobj), flags_(flags), level_(level)
        {}

        DcmObject &dcmobj_;
        const size_t flags_;
        const int level_;
    };

    /* member variables */

protected:

    /// error flag for this object.
    OFCondition errorFlag;

private:

    /// the DICOM attribute tag and VR for this object
    DcmTag Tag;

    /// the length of this attribute as read from stream, may be undefined length
    Uint32 Length;

    /// transfer state during read and write operations
    E_TransferState fTransferState;

    /// number of bytes already read/written during transfer
    Uint32 fTransferredBytes;

 }; // class DcmObject

/** Print a DcmObject::PrintHelper to an ostream.
 *  @param stream stream to print to
 *  @param obj object which will be print()ed
 *  @return the stream argument
 */
static inline STD_NAMESPACE ostream& operator<<(STD_NAMESPACE ostream &stream, DcmObject::PrintHelper obj)
{
    obj.dcmobj_.print(stream, obj.flags_, obj.level_);
    return stream;
}

#endif // DCOBJECT_H


/*
 * CVS/RCS Log:
 * $Log: dcobject.h,v $
 * Revision 1.70  2010-10-29 10:57:17  joergr
 * Added support for colored output to the print() method.
 *
 * Revision 1.69  2010-10-14 13:15:41  joergr
 * Updated copyright header. Added reference to COPYRIGHT file.
 *
 * Revision 1.68  2010-06-18 08:17:57  uli
 * Added undefined assignment operator to PrintHelper to work around
 * a warning with VC2008.
 *
 * Revision 1.67  2010-03-01 09:08:44  uli
 * Removed some unnecessary include directives in the headers.
 *
 * Revision 1.66  2010-02-22 11:39:54  uli
 * Remove some unneeded includes.
 *
 * Revision 1.65  2009-11-04 09:58:07  uli
 * Switched to logging mechanism provided by the "new" oflog module
 *
 * Revision 1.64  2009-10-13 14:26:09  uli
 * Added the DcmObject::PrintHelper helper class. This class can be used to
 * print DcmObject instances with oflog's logging macros.
 *
 * Revision 1.63  2009-08-07 14:40:38  joergr
 * Enhanced isEmpty() method by checking whether the data element value consists
 * of non-significant characters only.
 *
 * Revision 1.62  2009-06-04 16:52:14  joergr
 * Added new parsing flag that allows for ignoring the value of File Meta
 * Information Group Length (0002,0000).
 *
 * Revision 1.61  2009-03-25 10:22:09  joergr
 * Added new method isEmpty() to DICOM object, item and sequence class.
 *
 * Revision 1.60  2009-03-05 14:07:56  onken
 * Fixed typo.
 *
 * Revision 1.59  2009-03-05 13:35:47  onken
 * Added checks for sequence and item lengths which prevents overflow in length
 * field, if total length of contained items (or sequences) exceeds
 * 32-bit length field. Also introduced new flag (default: enabled)
 * for writing in explicit length mode, which allows for automatically
 * switching encoding of only that very sequence/item to undefined
 * length coding (thus permitting to actually write the file).
 *
 * Revision 1.58  2009-02-11 13:16:32  onken
 * Added global parser flag permitting to stop parsing after a specific
 * element was parsed on dataset level (useful for removing garbage at
 * end of file).
 *
 * Revision 1.57  2009-02-04 17:54:30  joergr
 * Fixed various layout and formatting issues.
 *
 * Revision 1.56  2009-02-04 14:05:01  onken
 * Introduced global flag that, if enabled, tells the parser to continue
 * parsing if possible.
 *
 * Revision 1.55  2009-01-29 15:36:16  onken
 * Added global parsing option that allows for reading private attributes in
 * implicit encoding having a maximum length to be read as sequences instead
 * of relying on the dictionary.
 *
 * Revision 1.54  2009-01-06 16:28:11  joergr
 * Reworked print() output format for option PF_showTreeStructure.
 *
 * Revision 1.53  2009-01-05 15:32:23  joergr
 * Added global flag that allows for reading incorrectly encoded DICOM datasets
 * where particular data elements are encoded with a differing transfer syntax
 * (Implicit VR Little endian instead of Explicit VR encoding as declared).
 *
 * Revision 1.52  2008-09-24 13:33:53  joergr
 * Fixed typo in comment.
 *
 * Revision 1.51  2008-07-17 11:19:49  onken
 * Updated copyFrom() documentation.
 *
 * Revision 1.50  2008-07-17 10:30:23  onken
 * Implemented copyFrom() method for complete DcmObject class hierarchy, which
 * permits setting an instance's value from an existing object. Implemented
 * assignment operator where necessary.
 *
 * Revision 1.49  2008-04-17 14:48:39  meichel
 * Method DcmObject::getLengthField now public, needed by dcmcheck
 *
 * Revision 1.48  2007/11/29 14:30:19  meichel
 * Write methods now handle large raw data elements (such as pixel data)
 *   without loading everything into memory. This allows very large images to
 *   be sent over a network connection, or to be copied without ever being
 *   fully in memory.
 *
 * Revision 1.47  2007/06/29 14:17:49  meichel
 * Code clean-up: Most member variables in module dcmdata are now private,
 *   not protected anymore.
 *
 * Revision 1.46  2007/02/19 15:04:34  meichel
 * Removed searchErrors() methods that are not used anywhere and added
 *   error() methods only in the DcmObject subclasses where really used.
 *
 * Revision 1.45  2006/12/15 14:18:07  joergr
 * Added new method that checks whether a DICOM object or element is affected
 * by SpecificCharacterSet (0008,0005).
 *
 * Revision 1.44  2006/12/13 13:58:15  joergr
 * Added new optional parameter "checkAllStrings" to method containsExtended
 * Characters().
 *
 * Revision 1.43  2006/08/15 15:49:56  meichel
 * Updated all code in module dcmdata to correctly compile when
 *   all standard C++ classes remain in namespace std.
 *
 * Revision 1.42  2006/05/11 08:54:23  joergr
 * Moved checkForNonASCIICharacters() from application to library.
 *
 * Revision 1.41  2005/12/08 16:28:22  meichel
 * Changed include path schema for all DCMTK header files
 *
 * Revision 1.40  2005/12/02 08:49:17  joergr
 * Changed macro NO_XFER_DETECTION_FOR_DATASETS into a global option that can
 * be enabled and disabled at runtime.
 *
 * Revision 1.39  2005/11/24 12:50:57  meichel
 * Fixed bug in code that prepares a byte stream that is fed into the MAC
 *   algorithm when creating or verifying a digital signature. The previous
 *   implementation was non-conformant when signatures included compressed
 *   (encapsulated) pixel data because the item length was included in the byte
 *   stream, while it should not. The global variable dcmEnableOldSignatureFormat
 *   and a corresponding command line option in dcmsign allow to re-enable the old
 *   implementation.
 *
 * Revision 1.38  2005/05/10 15:27:14  meichel
 * Added support for reading UN elements with undefined length according
 *   to CP 246. The global flag dcmEnableCP246Support allows to revert to the
 *   prior behaviour in which UN elements with undefined length were parsed
 *   like a normal explicit VR SQ element.
 *
 * Revision 1.37  2004/07/01 12:28:25  meichel
 * Introduced virtual clone method for DcmObject and derived classes.
 *
 * Revision 1.36  2004/04/27 09:21:01  wilkens
 * Fixed a bug in dcelem.cc which occurs when one is serializing a dataset
 * (that contains an attribute whose length value is coded with 2 bytes) into
 * a given buffer. Although the number of available bytes in the buffer was
 * sufficient, the dataset->write(...) method would always return
 * EC_StreamNotifyClient to indicate that there are not sufficient bytes
 * available in the buffer. This code modification fixes the problem.
 *
 * Revision 1.35  2003/06/12 13:33:21  joergr
 * Fixed inconsistent API documentation reported by Doxygen.
 *
 * Revision 1.34  2002/12/06 12:49:11  joergr
 * Enhanced "print()" function by re-working the implementation and replacing
 * the boolean "showFullData" parameter by a more general integer flag.
 * Added doc++ documentation.
 * Made source code formatting more consistent with other modules/files.
 *
 * Revision 1.33  2002/08/27 16:55:35  meichel
 * Initial release of new DICOM I/O stream classes that add support for stream
 *   compression (deflated little endian explicit VR transfer syntax)
 *
 * Revision 1.32  2002/08/20 12:18:35  meichel
 * Changed parameter list of loadFile and saveFile methods in class
 *   DcmFileFormat. Removed loadFile and saveFile from class DcmObject.
 *
 * Revision 1.31  2002/07/08 14:45:20  meichel
 * Improved dcmdata behaviour when reading odd tag length. Depending on the
 *   global boolean flag dcmAcceptOddAttributeLength, the parser now either accepts
 *   odd length attributes or implements the old behaviour, i.e. assumes a real
 *   length larger by one.
 *
 * Revision 1.30  2002/04/25 09:38:47  joergr
 * Added support for XML output of DICOM objects.
 *
 * Revision 1.29  2002/04/11 12:23:46  joergr
 * Added new methods for loading and saving DICOM files.
 *
 * Revision 1.28  2001/11/16 15:54:39  meichel
 * Adapted digital signature code to final text of supplement 41.
 *
 * Revision 1.27  2001/09/25 17:19:27  meichel
 * Adapted dcmdata to class OFCondition
 *
 * Revision 1.26  2001/06/01 15:48:41  meichel
 * Updated copyright header
 *
 * Revision 1.25  2000/11/07 16:56:07  meichel
 * Initial release of dcmsign module for DICOM Digital Signatures
 *
 * Revision 1.24  2000/04/14 16:02:39  meichel
 * Global flag dcmEnableAutomaticInputDataCorrection now derived from OFGlobal
 *   and, thus, safe for use in multi-thread applications.
 *
 * Revision 1.23  2000/03/08 16:26:16  meichel
 * Updated copyright header.
 *
 * Revision 1.22  2000/03/03 14:05:24  meichel
 * Implemented library support for redirecting error messages into memory
 *   instead of printing them to stdout/stderr for GUI applications.
 *
 * Revision 1.21  2000/02/10 10:50:52  joergr
 * Added new feature to dcmdump (enhanced print method of dcmdata): write
 * pixel data/item value fields to raw files.
 *
 * Revision 1.20  2000/02/01 10:12:02  meichel
 * Avoiding to include <stdlib.h> as extern "C" on Borland C++ Builder 4,
 *   workaround for bug in compiler header files.
 *
 * Revision 1.19  1999/03/31 09:24:42  meichel
 * Updated copyright header in module dcmdata
 *
 *
 */
