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
 *  Purpose: Interface of class DcmSequenceOfItems
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-11-05 09:34:11 $
 *  CVS/RCS Revision: $Revision: 1.58 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
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
class DcmSequenceOfItems : public DcmElement
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
     *    (valid values: "1", "1-2", "1-3", "1-8", "1-99", "1-n", "2", "2-n", "2-2n",
     *                   "3", "3-n", "3-3n", "4", "6", "9", "16", "32"),
     *     parameter used to specify the value multiplicity for non-sequence attributes
     *  @param oldFormat parameter not used for this VR (only for DA, TM, PN)
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
     *  If length encodig is set to be explicit and the total sequence size is
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
     *  If length encodig is set to be explicit and the total sequence size is
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

    /** check if this object contains non-ASCII characters at any nesting level
     *  @param checkAllStrings if true, also check elements with string values not affected
     *    by SpecificCharacterSet (0008,0005), default: only check PN, LO, LT, SH, ST, UT
     *  @return true if object contains non-ASCII characters, false otherwise
     */
    virtual OFBool containsExtendedCharacters(const OFBool checkAllStrings = OFFalse);

    /** check if this object is affected by SpecificCharacterSet at any nesting level.
     *  In detail, it is checked whether this object contains any data elements that
     *  according to their VR are affected by the SpecificCharacterSet (0008,0005)
     *  element. This is true for the following VRs: PN, LO, LT, SH, ST and UT
     *  @return true if object is affected by SpecificCharacterSet, false otherwise
     */
    virtual OFBool isAffectedBySpecificCharacterSet() const;

    /** get cardinality of this sequence
     *  @return number of items in this sequence
     */
    virtual unsigned long card();

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
     *  @param before indicates whether the new item should be inserted before or after the item identified by "where"
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


/*
** CVS/RCS Log:
** $Log: dcsequen.h,v $
** Revision 1.58  2010-11-05 09:34:11  joergr
** Added support for checking the value multiplicity "9" (see Supplement 131).
**
** Revision 1.57  2010-10-14 13:15:42  joergr
** Updated copyright header. Added reference to COPYRIGHT file.
**
** Revision 1.56  2010-04-23 15:28:02  joergr
** Specify an appropriate default value for the "vm" parameter of checkValue().
**
** Revision 1.55  2010-04-23 14:27:30  joergr
** Added new method to all VR classes which checks whether the stored value
** conforms to the VR definition and to the specified VM.
**
** Revision 1.54  2010-03-01 09:08:44  uli
** Removed some unnecessary include directives in the headers.
**
** Revision 1.53  2010-02-22 11:39:54  uli
** Remove some unneeded includes.
**
** Revision 1.52  2009-08-07 14:40:39  joergr
** Enhanced isEmpty() method by checking whether the data element value consists
** of non-significant characters only.
**
** Revision 1.51  2009-03-25 10:22:09  joergr
** Added new method isEmpty() to DICOM object, item and sequence class.
**
** Revision 1.50  2009-03-05 14:07:56  onken
** Fixed typo.
**
** Revision 1.49  2009-03-05 13:35:48  onken
** Added checks for sequence and item lengths which prevents overflow in length
** field, if total length of contained items (or sequences) exceeds
** 32-bit length field. Also introduced new flag (default: enabled)
** for writing in explicit length mode, which allows for automatically
** switching encoding of only that very sequence/item to undefined
** length coding (thus permitting to actually write the file).
**
** Revision 1.48  2009-02-04 17:52:17  joergr
** Fixes various type mismatches reported by MSVC introduced with OFFile class.
**
** Revision 1.47  2008-12-12 11:44:40  onken
** Moved path access functions to separate classes
**
** Revision 1.46  2008-12-05 13:28:14  onken
** Splitted findOrCreatePath() function API for also offering a simple API
** for non-wildcard searches.
**
** Revision 1.45  2008-12-04 16:55:14  onken
** Changed findOrCreatePath() to also support wildcard as item numbers.
**
** Revision 1.44  2008-10-15 12:31:20  onken
** Added findOrCreatePath() functions which allow for finding or creating a
** hierarchy of sequences, items and attributes according to a given "path"
** string.
**
** Revision 1.43  2008-07-17 11:19:49  onken
** Updated copyFrom() documentation.
**
** Revision 1.42  2008-07-17 10:36:56  onken
** *** empty log message ***
**
** Revision 1.41  2008-07-17 10:30:23  onken
** Implemented copyFrom() method for complete DcmObject class hierarchy, which
** permits setting an instance's value from an existing object. Implemented
** assignment operator where necessary.
**
** Revision 1.40  2008-06-23 12:09:13  joergr
** Fixed inconsistencies in Doxygen API documentation.
**
** Revision 1.39  2007/11/29 14:30:19  meichel
** Write methods now handle large raw data elements (such as pixel data)
**   without loading everything into memory. This allows very large images to
**   be sent over a network connection, or to be copied without ever being
**   fully in memory.
**
** Revision 1.38  2007/06/29 14:17:49  meichel
** Code clean-up: Most member variables in module dcmdata are now private,
**   not protected anymore.
**
** Revision 1.37  2007/02/19 15:04:34  meichel
** Removed searchErrors() methods that are not used anywhere and added
**   error() methods only in the DcmObject subclasses where really used.
**
** Revision 1.36  2006/12/15 14:18:07  joergr
** Added new method that checks whether a DICOM object or element is affected
** by SpecificCharacterSet (0008,0005).
**
** Revision 1.35  2006/12/13 13:58:15  joergr
** Added new optional parameter "checkAllStrings" to method containsExtended
** Characters().
**
** Revision 1.34  2006/08/15 15:49:56  meichel
** Updated all code in module dcmdata to correctly compile when
**   all standard C++ classes remain in namespace std.
**
** Revision 1.33  2006/05/30 15:00:57  joergr
** Added missing method containsExtendedCharacters().
**
** Revision 1.32  2005/12/08 16:28:41  meichel
** Changed include path schema for all DCMTK header files
**
** Revision 1.31  2005/05/10 15:27:14  meichel
** Added support for reading UN elements with undefined length according
**   to CP 246. The global flag dcmEnableCP246Support allows to revert to the
**   prior behaviour in which UN elements with undefined length were parsed
**   like a normal explicit VR SQ element.
**
** Revision 1.30  2004/07/01 12:28:25  meichel
** Introduced virtual clone method for DcmObject and derived classes.
**
** Revision 1.29  2003/08/08 13:29:13  joergr
** Added new method insertAtCurrentPos() which allows for a much more efficient
** insertion (avoids re-searching for the correct position).
**
** Revision 1.28  2002/12/06 12:49:13  joergr
** Enhanced "print()" function by re-working the implementation and replacing
** the boolean "showFullData" parameter by a more general integer flag.
** Added doc++ documentation.
** Made source code formatting more consistent with other modules/files.
**
** Revision 1.27  2002/08/27 16:55:39  meichel
** Initial release of new DICOM I/O stream classes that add support for stream
**   compression (deflated little endian explicit VR transfer syntax)
**
** Revision 1.26  2002/04/25 09:43:56  joergr
** Added support for XML output of DICOM objects.
**
** Revision 1.25  2001/11/19 15:23:10  meichel
** Cleaned up signature code to avoid some gcc warnings.
**
** Revision 1.24  2001/11/16 15:54:39  meichel
** Adapted digital signature code to final text of supplement 41.
**
** Revision 1.23  2001/09/25 17:19:28  meichel
** Adapted dcmdata to class OFCondition
**
** Revision 1.22  2001/06/01 15:48:43  meichel
** Updated copyright header
**
** Revision 1.21  2000/11/07 16:56:09  meichel
** Initial release of dcmsign module for DICOM Digital Signatures
**
** Revision 1.20  2000/04/14 15:31:33  meichel
** Removed default value from output stream passed to print() method.
**   Required for use in multi-thread environments.
**
** Revision 1.19  2000/03/08 16:26:17  meichel
** Updated copyright header.
**
** Revision 1.18  2000/03/03 14:05:25  meichel
** Implemented library support for redirecting error messages into memory
**   instead of printing them to stdout/stderr for GUI applications.
**
** Revision 1.17  2000/02/10 10:50:53  joergr
** Added new feature to dcmdump (enhanced print method of dcmdata): write
** pixel data/item value fields to raw files.
**
** Revision 1.16  1999/03/31 09:24:46  meichel
** Updated copyright header in module dcmdata
**
** Revision 1.15  1998/11/12 16:47:44  meichel
** Implemented operator= for all classes derived from DcmObject.
**
** Revision 1.14  1998/07/15 15:48:52  joergr
** Removed several compiler warnings reported by gcc 2.8.1 with
** additional options, e.g. missing copy constructors and assignment
** operators, initialization of member variables in the body of a
** constructor instead of the member initialization list, hiding of
** methods by use of identical names, uninitialized member variables,
** missing const declaration of char pointers. Replaced tabs by spaces.
**
** Revision 1.13  1997/07/21 08:25:10  andreas
** - Replace all boolean types (BOOLEAN, CTNBOOLEAN, DICOM_BOOL, BOOL)
**   with one unique boolean type OFBool.
**
** Revision 1.12  1997/07/07 07:42:05  andreas
** - Changed parameter type DcmTag & to DcmTagKey & in all search functions
**   in DcmItem, DcmSequenceOfItems, DcmDirectoryRecord and DcmObject
**
** Revision 1.11  1997/05/27 13:48:29  andreas
** - Add method canWriteXfer to class DcmObject and all derived classes.
**   This method checks whether it is possible to convert the original
**   transfer syntax to an new transfer syntax. The check is used in the
**   dcmconv utility to prohibit the change of a compressed transfer
**   syntax to a uncompressed.
**
** Revision 1.10  1997/05/16 08:23:48  andreas
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
** Revision 1.9  1997/04/24 12:09:02  hewett
** Fixed DICOMDIR generation bug affecting ordering of
** patient/study/series/image records (item insertion into a sequence
** did produce the expected ordering).
**
** Revision 1.8  1996/08/05 08:45:28  andreas
** new print routine with additional parameters:
**         - print into files
**         - fix output length for elements
** corrected error in search routine with parameter ESM_fromStackTop
**
** Revision 1.7  1996/07/17 12:38:59  andreas
** new nextObject to iterate a DicomDataset, DicomFileFormat, Item, ...
**
** Revision 1.6  1996/01/29 13:38:14  andreas
** - new put method for every VR to put value as a string
** - better and unique print methods
**
** Revision 1.5  1996/01/24 09:34:56  andreas
** Support for 64 bit long
**
** Revision 1.4  1996/01/09 11:06:16  andreas
** New Support for Visual C++
** Correct problems with inconsistent const declarations
**
** Revision 1.3  1996/01/05 13:22:59  andreas
** - changed to support new streaming facilities
** - more cleanups
** - merged read / write methods for block and file transfer
**
*/
