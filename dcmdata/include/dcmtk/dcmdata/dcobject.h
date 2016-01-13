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
 *  Author:  Gerd Ehlers
 *
 *  Purpose:
 *  This file contains the interface to routines which provide
 *  DICOM object encoding/decoding, search and lookup facilities.
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
class DcmItem;
class DcmOutputStream;
class DcmInputStream;
class DcmWriteCache;
class DcmSpecificCharacterSet;

// include this file in doxygen documentation

/** @file dcobject.h
 *  @brief interface to DICOM object/dataset handling
 */

// Undefined Length Identifier now defined in dctypes.h

// Maximum number of read bytes for a Value Element
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
 *  data (e.g.\ stripping of padding blanks, removal of blanks in UIDs, etc).
 *  Default is enabled.
 */
extern DCMTK_DCMDATA_EXPORT OFGlobal<OFBool> dcmEnableAutomaticInputDataCorrection; /* default OFTrue */

/** This flag defines the handling of illegal odd-length attributes: If flag is
 *  true, odd lengths are respected (i.e.\ an odd number of bytes is read from
 *  the input stream.) After successful reading, padding to even number of bytes
 *  is enforced by adding a zero pad byte if dcmEnableAutomaticInputDataCorrection
 *  is true. Otherwise the odd number of bytes remains as read.
 *
 *  If flag is false, old (pre DCMTK 3.5.2) behaviour applies: The length field
 *  implicitly incremented and an even number of bytes is read from the stream.
 */
extern DCMTK_DCMDATA_EXPORT OFGlobal<OFBool> dcmAcceptOddAttributeLength; /* default OFTrue */

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
extern DCMTK_DCMDATA_EXPORT OFGlobal<OFBool> dcmEnableCP246Support; /* default OFTrue */

/** DCMTK releases up to 3.5.3 created a non-conforming byte stream
 *  as input to the MAC algorithm when creating or verifying digital signatures
 *  including compressed pixel data (i.e.\ signatures including attribute
 *  (7FE0,0010) in an encapsulated transfer syntax). This has been fixed
 *  in DCMTK 3.5.4, but this flag allows to revert to the old behavior
 *  in order to create or verify signatures that are compatible with older
 *  releases. Default is "off" (OFFalse).
 */
extern DCMTK_DCMDATA_EXPORT OFGlobal<OFBool> dcmEnableOldSignatureFormat; /* default OFFalse */

/** This flag defines whether the transfer syntax for uncompressed datasets
 *  is detected automatically.  The automatic detection has been introduced
 *  since there are (incorrectly encoded) DICOM dataset stored with a
 *  different transfer syntax than specified in the meta header.
 */
extern DCMTK_DCMDATA_EXPORT OFGlobal<OFBool> dcmAutoDetectDatasetXfer; /* default OFFalse */

/** This flag defines how non-standard VRs are treated by the parser when
 *  reading. The default is to treat data element with non-standard VR as
 *  unknown. If this flag is enabled, the parser will try to read the data
 *  element with Implicit VR Little Endian transfer syntax.
 */
extern DCMTK_DCMDATA_EXPORT OFGlobal<OFBool> dcmAcceptUnexpectedImplicitEncoding; /* default OFFalse */

/** This flag defines how the element's VR is treated by the parser when
 *  reading from a dataset with explicit VR encoding. By default, the
 *  VR from the dataset is used. If this flag is enabled and the VR of the
 *  tag is defined in the data dictionary, the parser will use the VR from
 *  the data dictionary (and ignore the one from the dataset). This flag is,
 *  therefore, useful for reading incorrectly encoded DICOM datasets.
 */
extern DCMTK_DCMDATA_EXPORT OFGlobal<OFBool> dcmPreferVRFromDataDictionary; /* default OFFalse */

/** This flag defines how the element's length field is interpreted when reading
 *  from a dataset with explicit VR encoding and the data dictionary doesn't
 *  agree with the VR from the dataset. By default, the length field is assumed
 *  to match the size of the VR in the dataset. If this flag is enabled and the
 *  tag is defined in the data dictionary, the parser will use the size for the
 *  VR from the data dictionary (and ignore the one from the dataset). This flag
 *  is, therefore, useful for reading incorrectly encoded DICOM datasets.
 */
extern DCMTK_DCMDATA_EXPORT OFGlobal<OFBool> dcmPreferLengthFieldSizeFromDataDictionary; /* default OFFalse */

/** This flag indicates, whether private attributes with implicit transfer
 *  syntax having a maximum length should be handled as sequences (ignoring
 *  any dictionary entries for that tag). This can happen, if for example
 *  a private creator element is illegally inserted with VR SQ
 *  (undefined length and implicit coding). The parser usually would then
 *  try to parse the element with VR=LO (private creator) with maximum
 *  length, which would lead to an error. The default behaviour is to
 *  rely on the dictionary.
 */
extern DCMTK_DCMDATA_EXPORT OFGlobal<OFBool> dcmReadImplPrivAttribMaxLengthAsSQ; /* default OFFalse */

/** This flag indicates, whether parsing errors during reading
 *  should be ignored, i.e.\ whether the parser should try to recover and
 *  parse the rest of the stream.
 *  This flag does not work for all parsing errors (at this time)
 *  making sense but was introduced afterwards.
 */
extern DCMTK_DCMDATA_EXPORT OFGlobal<OFBool> dcmIgnoreParsingErrors; /* default OFFalse */

/** This flag indicates, whether parsing should stop after a certain
 *  element in the stream was parsed. This is especially useful for
 *  datasets containing garbage at the end, usually after the Pixel
 *  Data attribute. To prevent the parser for "stumbling" over that
 *  garbage, it is possible to tell the parser to stop after a
 *  specific element. The flag is only sensitive to elements on
 *  dataset level, i.e. inside sequence any occurrence of the specified
 *  tag is ignored. Caution: Note that if Pixel Data is chosen
 *  as stop element, any attributes behind will not be parsed, e. g.
 *  any digital signature attributes coming after.
 *  Default is (0xffff,0xffff), which means that the feature is
 *  disabled.
 */
extern DCMTK_DCMDATA_EXPORT OFGlobal<DcmTagKey> dcmStopParsingAfterElement; /* default OFTrue */

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
extern DCMTK_DCMDATA_EXPORT OFGlobal<OFBool> dcmWriteOversizedSeqsAndItemsUndefined; /* default OFTrue */

/** This flag allows for ignoring the value of (0002,0000) File Meta Information
 *  Group Length which is useful in cases where this value is incorrect.  If the
 *  header length is ignored, the behavior is identical to the case when no value
 *  is available (i.e. all elements are read as long as the group number is 0x0002).
 */
extern DCMTK_DCMDATA_EXPORT OFGlobal<OFBool> dcmIgnoreFileMetaInformationGroupLength; /* default OFFalse */

/** This flag enables the replacement of a wrong delimitation item at the end of
 *  a sequence or item.  This is because there are incorrect systems that write
 *  a Sequence Delimitation Item (fffe,e0dd) at the end of an item or an Item
 *  Delimitation Item (fffe,e00d) at the end of a sequence.  By default, no
 *  delimitation items are replaced.
 */
extern DCMTK_DCMDATA_EXPORT OFGlobal<OFBool> dcmReplaceWrongDelimitationItem; /* default OFFalse */


/** Abstract base class for most classes in module dcmdata. As a rule of thumb,
 *  everything that is either a dataset or that can be identified with a DICOM
 *  attribute tag is derived from class DcmObject.
 */
class DCMTK_DCMDATA_EXPORT DcmObject
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
     *  @return true if leaf node, false otherwise
     */
    virtual OFBool isLeaf() const = 0;

    /** check if this element is nested in a sequence of items, i.e.\ not a
     *  top-level or stand-alone element
     *  @return true if this element is nested, false otherwise
     */
    virtual OFBool isNested() const;

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

    /** get root dataset/item (top-level) that contains this object. Internally,
     *  the list of parent pointers is followed in order to find the root. If
     *  this object has no parent item, a pointer to this object is returned
     *  instead.
     *  @return pointer to the root dataset/item (might be NULL)
     */
    DcmItem *getRootItem();

    /** get parent item of this object. In case of a top-level element, this is
     *  either the main dataset or the file meta-information. In case of a nested
     *  element, this is the surrounding item.
     *  @return pointer to the parent item of this object (might be NULL)
     */
    virtual DcmItem *getParentItem();

    /** get parent of this object. If this object is an element that has been
     *  inserted into a dataset/item, the parent is this particular dataset/item.
     *  If this object is an item that has been inserted into a sequence, the
     *  parent is this particular sequence. If this object has not been inserted
     *  into a dataset/item or sequence, NULL is returned.
     *  @return pointer to the parent of this object (might be NULL)
     */
    inline DcmObject *getParent() { return Parent; }

    /** get parent of this object. If this object is an element that has been
     *  inserted into a dataset/item, the parent is this particular dataset/item.
     *  If this object is an item that has been inserted into a sequence, the
     *  parent is this particular sequence. If this object has not been inserted
     *  into a dataset/item or sequence, NULL is returned.
     *  @return pointer to the parent of this object (might be NULL)
     */
    inline const DcmObject *getParent() const { return Parent; }

    /** set parent of this object. NULL means no parent.
     *  NB: This method is used by derived classes for internal purposes only.
     */
    inline void setParent(DcmObject *parent) { Parent = parent; }

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

    /** convert this object from the currently selected source character set to the
     *  currently selected destination character set (if affected by SpecificCharacterSet)
     *  @param converter character set converter to be used to convert the element values
     *  @return always returns EC_Normal, since there is nothing to do in this base class
     */
    virtual OFCondition convertCharacterSet(DcmSpecificCharacterSet &converter);

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
     *  of the public getLength() method). Only needed for internal purposes and for checker tools
     *  that verify values against the length field.
     *  @return current value of length field
     */
    Uint32 getLengthField() const { return Length; }

 protected:

    /** print line indentation, e.g.\ a couple of spaces for each nesting level.
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
     *  Calls printInfoLineStart() and printInfoLineEnd() to frame the 'info' text.
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
    class DCMTK_DCMDATA_EXPORT PrintHelper
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

    /// pointer to parent object if contained in a dataset/item (might be NULL)
    DcmObject *Parent;
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
