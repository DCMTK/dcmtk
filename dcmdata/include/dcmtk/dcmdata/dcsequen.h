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
 *  Author:  Gerd Ehlers, Andreas Barth
 *
 *  Purpose: Interface of class DcmSequenceOfItems
 *
 */


#ifndef DCSEQUEN_H
#define DCSEQUEN_H

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/ofstd/offile.h"       /* for offile_off_t */
#include "dcmtk/dcmdata/dcelem.h"
#include "dcmtk/dcmdata/dctag.h"
#include "dcmtk/dcmdata/dclist.h"
#include "dcmtk/dcmdata/dcstack.h"

/** class representing a DICOM Sequence of Items (SQ).
 *  This class is derived from class DcmElement (and not from DcmObject) despite the fact
 *  that sequences have no value field as such, they maintain a list of items. However,
 *  all APIs in class DcmItem and class DcmDataset accept DcmElements.
 *  This is ugly and causes some DcmElement API methods to be useless with DcmSequence.
 */
class DCMTK_DCMDATA_EXPORT DcmSequenceOfItems : public DcmElement
{
public:

    /** constructor
     *  @param tag attribute tag
     *  @param len length of the attribute value
     *  @param readAsUN flag indicating whether the sequence should be
     *  read (interpreted) as a UN element with Implicit VR Little Endian encoding
     */
    DcmSequenceOfItems(const DcmTag &tag,
                       const Uint32 len = 0,
                       OFBool readAsUN = OFFalse);

    /** copy constructor
     *  @param oldSeq element to be copied
     */
    DcmSequenceOfItems(const DcmSequenceOfItems& oldSeq);

    /// destructor
    virtual ~DcmSequenceOfItems();

    /** copy assignment operator
     *  @param obj element to be copied
     */
    DcmSequenceOfItems &operator=(const DcmSequenceOfItems &obj);

    /** comparison operator that compares the normalized value of this object
     *  with a given object of the same type. The tag of the element is also
     *  considered as the first component that is compared, followed by the
     *  object types (VR, i.e. DCMTK'S EVR) and the comparison of all value
     *  components of the object, preferably in the order declared in the
     *  object (if applicable). For sequences that means that all
     *  contained items are compared element by element, so this may be
     *  an expensive operation!
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

    /// returns current status flag
    inline OFCondition error() const { return errorFlag; }

    /** clone method
     *  @return deep copy of this object
     */
    virtual DcmObject *clone() const
    {
      return new DcmSequenceOfItems(*this);
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

    /** return identifier for this class. Every class derived from this class
     *  returns a unique value of type enum DcmEVR for this call. This is used
     *  as a "poor man's RTTI" to correctly identify instances derived from
     *  this class even on compilers not supporting RTTI.
     *  @return type identifier of this class
     */
    virtual DcmEVR ident() const { return EVR_SQ; }

    /** check if this element is a leaf node in a dataset tree.
     *  All subclasses of DcmElement except for DcmSequenceOfItems
     *  are leaf nodes, while DcmSequenceOfItems, DcmItem, DcmDataset etc.
     *  are not.
     *  @return true if leaf node, false otherwise.
     */
    virtual OFBool isLeaf() const { return OFFalse; }

    /** print object to a stream
     *  @param out output stream
     *  @param flags optional flag used to customize the output (see DCMTypes::PF_xxx)
     *  @param level current level of nested items. Used for indentation.
     *  @param pixelFileName not used (used in certain sub-classes of this class)
     *  @param pixelCounter not used (used in certain sub-classes of this class)
     */
    virtual void print(STD_NAMESPACE ostream&out,
                       const size_t flags = 0,
                       const int level = 0,
                       const char *pixelFileName = NULL,
                       size_t *pixelCounter = NULL);

    /** check whether stored value conforms to the VR and to the specified VM
     *  @param card cardinality (number of items) to be checked for.
     *    (See DcmElement::checkVM() for a list of valid values.)
     *     Parameter used to specify the value multiplicity for non-sequence attributes.
     *  @param oldFormat parameter not used for this VR (only for DA, TM)
     *  @return status of the check, EC_Normal if value is correct, an error code otherwise
     */
    virtual OFCondition checkValue(const OFString &card = "1-n",
                                   const OFBool oldFormat = OFFalse);

    /** get value multiplicity
     *  @return always returns 1 (according to the DICOM standard)
     */
    virtual unsigned long getVM() { return 1L; }

    /** This function takes care of group length and padding elements
     *  in the current element list according to what is specified in
     *  glenc and padenc. If required, this function does the following
     *  two things:
     *    a) it calculates the group length of all groups which are
     *       contained in this item and sets the calculated values
     *       in the corresponding group length elements and
     *    b) it inserts a corresponding padding element (or, in case
     *       of sequences: padding elements) with a corresponding correct
     *       size into the element list.
     *  @param glenc          Encoding type for group length; specifies what shall
     *                        be done with group length tags.
     *  @param padenc         Encoding type for padding; specifies what shall be
     *                        done with padding tags.
     *  @param xfer           The transfer syntax that shall be used.
     *  @param enctype        Encoding type for sequences; specifies how sequences
     *                        will be handled.
     *  @param padlen         The length up to which the dataset shall be padded,
     *                        if padding is desired.
     *  @param subPadlen      For sequences (ie sub elements), the length up to
     *                        which item shall be padded, if padding is desired.
     *  @param instanceLength Number of extra bytes added to the item/dataset
     *                        length used when computing the padding; this
     *                        parameter is for instance used to pass the length
     *                        of the file meta header from the DcmFileFormat to
     *                        the DcmDataset object.
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition computeGroupLengthAndPadding
                            (const E_GrpLenEncoding glenc,
                             const E_PaddingEncoding padenc = EPD_noChange,
                             const E_TransferSyntax xfer = EXS_Unknown,
                             const E_EncodingType enctype = EET_ExplicitLength,
                             const Uint32 padlen = 0,
                             const Uint32 subPadlen = 0,
                             Uint32 instanceLength = 0);

    /** calculate the length of this DICOM element when encoded with the
     *  given transfer syntax and the given encoding type for sequences.
     *  For elements, the length includes the length of the tag, length field,
     *  VR field and the value itself, for items and sequences it returns
     *  the length of the complete item or sequence including delimitation tags
     *  if applicable.
     *  If length encoding is set to be explicit and the total sequence size is
     *  larger than the available 32-bit length field, then undefined length
     *  is returned. If "dcmWriteOversizedSeqsAndItemsUndefined" is disabled,
     *  also the internal DcmObject errorFlag is set to EC_SeqOrItemContentOverflow
     *  in case the sequence content (excluding tag header etc.) is already too
     *  large.
     *  @param xfer transfer syntax for length calculation
     *  @param enctype sequence encoding type for length calculation
     *  @return length of DICOM element
     */
    virtual Uint32 calcElementLength(const E_TransferSyntax xfer,
                                     const E_EncodingType enctype);

    /** calculate the value length (without attribute tag, VR and length field)
     *  of this DICOM element when encoded with the given transfer syntax and
     *  the given encoding type for sequences.
     *  If length encoding is set to be explicit and the total sequence size is
     *  larger than the available 32-bit length field, then undefined length
     *  is returned. If "dcmWriteOversizedSeqsAndItemsImplicit" is disabled,
     *  also the internal DcmObject errorFlag is set to
     *  EC_SeqOrItemContentOverflow.
     *  @param xfer transfer syntax for length calculation
     *  @param enctype sequence encoding type for length calculation
     *  @return value length of DICOM element
     */
    virtual Uint32 getLength(const E_TransferSyntax xfer = EXS_LittleEndianImplicit,
                             const E_EncodingType enctype = EET_UndefinedLength);

    /** initialize the transfer state of this object. This method must be called
     *  before this object is written to a stream or read (parsed) from a stream.
     */
    virtual void transferInit();

    /** finalize the transfer state of this object. This method must be called
     *  when reading/writing this object from/to a stream has been completed.
     */
    virtual void transferEnd();

    /** check if this DICOM object can be encoded in the given transfer syntax.
     *  @param newXfer transfer syntax in which the DICOM object is to be encoded
     *  @param oldXfer transfer syntax in which the DICOM object was read or created.
     *  @return true if object can be encoded in desired transfer syntax, false otherwise.
     */
    virtual OFBool canWriteXfer(const E_TransferSyntax oldXfer,
                                const E_TransferSyntax newXfer);

    /** This function reads the information of all attributes which
     *  are captured in the input stream and captures this information
     *  in elementList. Each attribute is represented as an element
     *  in this list. If not all information for an attribute could be
     *  read from the stream, the function returns EC_StreamNotifyClient.
     *  @param inStream      The stream which contains the information.
     *  @param xfer         The transfer syntax which was used to encode
     *                       the information in inStream.
     *  @param glenc         Encoding type for group length; specifies
     *                       what will be done with group length tags.
     *  @param maxReadLength Maximum read length for reading an attribute value.
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition read(DcmInputStream &inStream,
                             const E_TransferSyntax xfer,
                             const E_GrpLenEncoding glenc = EGL_noChange,
                             const Uint32 maxReadLength = DCM_MaxReadLength);

    /** write object to a stream
     *  @param outStream DICOM output stream
     *  @param oxfer output transfer syntax
     *  @param enctype encoding types (undefined or explicit length)
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
    virtual OFCondition writeXML(STD_NAMESPACE ostream&out,
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

    /** returns true if the current object may be included in a digital signature
     *  @return true if signable, false otherwise
     */
    virtual OFBool isSignable() const;

    /** returns true if the object contains an element with Unknown VR at any nesting level
     *  @return true if the object contains an element with Unknown VR, false otherwise
     */
    virtual OFBool containsUnknownVR() const;

    /** check if this object contains non-ASCII characters at any nesting level. Please note
     *  that this check is pretty simple and only works for single-byte character sets that
     *  do include the 7-bit ASCII codes, e.g. for the ISO 8859 family. In other words: All
     *  character codes below 128 are considered to be ASCII codes and all others are
     *  considered to be non-ASCII.
     *  @param checkAllStrings if true, also check elements with string values not affected
     *    by SpecificCharacterSet (0008,0005). By default, only check PN, LO, LT, SH, ST,
     *    UC and UT.
     *  @return true if object contains non-ASCII characters, false otherwise
     */
    virtual OFBool containsExtendedCharacters(const OFBool checkAllStrings = OFFalse);

    /** check if this object is affected by SpecificCharacterSet at any nesting level.
     *  In detail, it is checked whether this object contains any data elements that
     *  according to their VR are affected by the SpecificCharacterSet (0008,0005)
     *  element. This is true for the following VRs: PN, LO, LT, SH, ST, UC and UT
     *  @return true if object is affected by SpecificCharacterSet, false otherwise
     */
    virtual OFBool isAffectedBySpecificCharacterSet() const;

    /** convert all element values that are contained in this item and that are affected
     *  by SpecificCharacterSet from the currently selected source character set to the
     *  currently selected destination character set
     *  @param converter character set converter to be used to convert the element values
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition convertCharacterSet(DcmSpecificCharacterSet &converter);

    /** get cardinality of this sequence
     *  @return number of items in this sequence
     */
    virtual unsigned long card() const;

    /** insert the given item at the start of the item list maintained by this sequence.
     *  Ownership of the item, which must be allocated on the heap, is transferred to the sequence.
     *  @param item pointer to DcmItem instance allocated on the heap, must not be NULL.
     *  @return EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition prepend(DcmItem *item);

    /** insert the given item at the given position within the item list maintained by this sequence.
     *  Ownership of the item, which must be allocated on the heap, is transferred to the sequence.
     *  @param item pointer to DcmItem instance allocated on the heap, must not be NULL.
     *  @param where index of the item after or before which the new item is to be inserted.
     *    Value must be < card() or equal to DCM_EndOfListIndex.
     *  @param before indicates whether the new item should be inserted before or after the item
     *    identified by "where"
     *  @return EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition insert(DcmItem *item,
                               unsigned long where = DCM_EndOfListIndex,
                               OFBool before = OFFalse);

    /** insert the given item at the end of the item list maintained by this sequence.
     *  Ownership of the item, which must be allocated on the heap, is transferred to the sequence.
     *  @param item pointer to DcmItem instance allocated on the heap, must not be NULL.
     *  @return EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition append(DcmItem *item);

    /** insert new item a current position.
     *  The current position is stored internally in the 'itemList' member variable.
     *  @param item new item to be inserted
     *  @param before flag indicating whether to insert the item before (OFFalse) or
     *    after (OFTrue) the current position
     *  @return status, EC_Normal upon success, an error code otherwise
     */
    virtual OFCondition insertAtCurrentPos(DcmItem *item,
                                           OFBool before = OFFalse);

    /** access an item from the sequence. This method returns a pointer to one
     *  of the items in the list, and not a copy.
     *  @param num index number of item, must be < card()
     *  @return pointer to item if found, NULL if num >= card()
     */
    virtual DcmItem *getItem(const unsigned long num);

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
    virtual OFCondition nextObject(DcmStack &stack, const OFBool intoSub);

    /** this method is only used in container classes,
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

    /** remove item from list. If found, the item is not deleted but
     *  returned to the caller who is responsible for further management of the
     *  DcmItem object.
     *  @param num index number of item, must be < card()
     *  @return pointer to DcmItem if found, NULL otherwise
     */
    virtual DcmItem *remove(const unsigned long num);

    /** remove item from list. If found, the item is not deleted but
     *  returned to the caller who is responsible for further management of the
     *  DcmItem object.
     *  @param item pointer to item to be removed from list
     *  @return pointer to item if found, NULL otherwise
     */
    virtual DcmItem *remove(DcmItem *item);

    /** check if this sequence is empty
     *  @param normalize not used for this class
     *  @return true if sequence is empty, i.e. has no items, false otherwise
     */
    virtual OFBool isEmpty(const OFBool normalize = OFTrue);

    /** clear (remove) attribute value
     *  @return EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition clear();

    /** check the currently stored element value
     *  @param autocorrect correct value length if OFTrue
     *  @return status, EC_Normal if value length is correct, an error code otherwise
     */
    virtual OFCondition verify(const OFBool autocorrect = OFFalse);

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
    virtual OFCondition search(const DcmTagKey &xtag,             // in
                               DcmStack &resultStack,             // inout
                               E_SearchMode mode = ESM_fromHere,  // in
                               OFBool searchIntoSub = OFTrue);    // in

    /** this method loads all attribute values maintained by this object and
     *  all sub-objects (in case of a container such as DcmDataset) into memory.
     *  After a call to this method, the file from which a dataset was read may safely
     *  be deleted or replaced. For large files, this method may obviously allocate large
     *  amounts of memory.
     *  @return EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition loadAllDataIntoMemory(void);

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

protected:

    /** This function reads tag and length information from inStream and
     *  returns this information to the caller. When reading information,
     *  the transfer syntax which was passed is accounted for. If the
     *  transfer syntax shows an explicit value representation, the data
     *  type of this object is also read from the stream. In general, this
     *  function follows the rules which are specified in the DICOM standard
     *  (see DICOM standard (year 2000) part 5, section 7) (or the corresponding
     *  section in a later version of the standard) concerning the encoding
     *  of a dataset.
     *  @param inStream  The stream which contains the information.
     *  @param xfer      The transfer syntax which was used to encode the
     *                   information in inStream.
     *  @param tag       Contains in the end the tag that was read.
     *  @param length    Contains in the end the length value that was read.
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition readTagAndLength(DcmInputStream &inStream,            // inout
                                         const E_TransferSyntax xfer,         // in
                                         DcmTag &tag,                         // out
                                         Uint32 &length);                     // out

    /** helper function for read(). Create sub-object (item, item delimiter or sequence delimiter) of the
     *  appropriate type depending on the tag.
     *  @param subObject upon success, a pointer to the newly created object is returned in this parameter
     *  @param newTag tag of the sub-object to be created
     *  @param newLength length of the sub-object to be created
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition makeSubObject(DcmObject *&subObject,
                                      const DcmTag &newTag,
                                      const Uint32 newLength);

    /** helper function for read(). Create sub-object (item or pixel item)
     *  and call read() for this sub-object.
     *  @param inStream      The stream which contains the information.
     *  @param newTag        attribute tag for sub-object
     *  @param newLength     length of the sub-object to be created
     *  @param xfer          The transfer syntax which was used to encode
     *                       the information in inStream.
     *  @param glenc         Encoding type for group length; specifies
     *                       what will be done with group length tags.
     *  @param maxReadLength Maximum read length for reading an attribute value.
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition readSubItem(DcmInputStream &inStream,                         // inout
                            const DcmTag &newTag,                             // in
                            const Uint32 newLength,                           // in
                            const E_TransferSyntax xfer,                      // in
                            const E_GrpLenEncoding glenc,                     // in
                            const Uint32 maxReadLength = DCM_MaxReadLength);  // in

    /** helper function for search(). May only be called if item list is non-empty.
     *  Performs hierarchical search for given tag and pushes pointer of sub-element
     *  on result stack if found
     *  @param tag tag key to be searched
     *  @param resultStack upon successful return, pointer to element pushed onto this stack
     *  @param searchIntoSub flag indicating whether recursive search is desired
     *  @return EC_Normal if tag found and stack modified, EC_TagNotFound if tag not found and stack unmodified
     */
    virtual OFCondition searchSubFromHere(const DcmTagKey &tag,               // in
                                          DcmStack &resultStack,              // inout
                                          const OFBool searchIntoSub);        // in

    /// the list of items maintained by this sequence object
    DcmList *itemList;

private:

    /** static helper method used in writeSignatureFormat().
     * This function resembles DcmObject::writeTagAndLength()
     * but only writes the tag, VR and reserved field.
     * @param outStream stream to write to
     * @param tag attribute tag
     * @param vr attribute VR as reported by getVR
     * @param oxfer output transfer syntax
     * @return EC_Normal if successful, an error code otherwise
     */
    static OFCondition writeTagAndVR(DcmOutputStream &outStream,
                                     const DcmTag &tag,
                                     DcmEVR vr,
                                     const E_TransferSyntax oxfer);

    /** flag used during suspended I/O. Indicates whether the last item
     *  was completely or only partially read/written during the last call
     *  to read/write.
     */
    OFBool lastItemComplete;

    /** used during reading. Contains the position in the stream where
     *  the sequence started (needed for calculating the remaining number of
     *  bytes available for a fixed-length sequence).
     */
    offile_off_t fStartPosition;

    /** true if this sequence has been instantiated while reading an UN element
     *  with undefined length
     */
    OFBool readAsUN_;

};


#endif // DCSEQUEN_H
