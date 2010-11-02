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
 *  Purpose: Interface of class DcmItem
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-11-02 15:31:06 $
 *  CVS/RCS Revision: $Revision: 1.83 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#ifndef DCITEM_H
#define DCITEM_H

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/ofstd/offile.h"       /* for offile_off_t */
#include "dcmtk/dcmdata/dctypes.h"
#include "dcmtk/dcmdata/dcobject.h"
#include "dcmtk/dcmdata/dclist.h"
#include "dcmtk/dcmdata/dcpcache.h"


// forward declaration
class DcmSequenceOfItems;
class DcmElement;


/** a class representing a list of DICOM elements in which each
 *  element has a different tag and elements are maintained in
 *  increasing order of tags. In particular, a sequence item.
 */
class DcmItem
  : public DcmObject
{
  public:

    /** default constructor
     */
    DcmItem();

    /** constructor.
     *  Create new item from given tag and length.
     *  @param tag DICOM tag for the new element
     *  @param len value length for the new element
     */
    DcmItem(const DcmTag &tag,
            const Uint32 len = 0);

    /** copy constructor
     *  @param old item to be copied
     */
    DcmItem(const DcmItem &old);

    /** assignment operator. Private creator cache is not copied
     *  as it is also the case for clone().
     *  @param the item to be copied
     */
    DcmItem &operator=(const DcmItem &obj);

    /** destructor
     */
    virtual ~DcmItem();

    /** clone method
     *  @return deep copy of this object
     */
    virtual DcmObject *clone() const
    {
      return new DcmItem(*this);
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

    /** get type identifier
     *  @return type identifier of this class (EVR_item)
     */
    virtual DcmEVR ident() const;

    /** get value multiplicity
     *  @return always returns 1 (according to the DICOM standard)
     */
    virtual unsigned long getVM();

    /** get cardinality of this item
     *  @return number of elements in this item
     */
    virtual unsigned long card() const;

    /** check if this element is a leaf node in a dataset tree.
     *  All subclasses of DcmElement except for DcmSequenceOfItems
     *  are leaf nodes, while DcmSequenceOfItems, DcmItem, DcmDataset etc.
     *  are not.
     *  @return true if leaf node, false otherwise.
     */
    virtual OFBool isLeaf() const { return OFFalse; }

    /** print all elements of the item to a stream
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

    /** calculate the length of this DICOM element when encoded with the
     *  given transfer syntax and the given encoding type for sequences.
     *  For elements, the length includes the length of the tag, length field,
     *  VR field and the value itself, for items and sequences it returns
     *  the length of the complete item or sequence including delimitation tags
     *  if applicable.
     *  If length encodig is set to be explicit and the total item size is
     *  larger than the available 32-bit length field, then undefined length
     *  is returned. If "dcmWriteOversizedSeqsAndItemsImplicit" is disabled,
     *  also the internal DcmObject errorFlag is set to EC_SeqOrItemContentOverflow
     *  in case the item content (excluding tag header etc.) is already too
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
     *  If length encodig is set to be explicit and the item content is larger
     *  than the available 32-bit length field, then undefined length is
     *  returned. If "dcmWriteOversizedSeqsAndItemsUndefined" is disabled,
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
    virtual OFBool canWriteXfer(const E_TransferSyntax newXfer,
                                const E_TransferSyntax oldXfer);

    /** This function reads the information of all attributes which
     *  are captured in the input stream and captures this information
     *  in elementList. Each attribute is represented as an element
     *  in this list. If not all information for an attribute could be
     *  read from the stream, the function returns EC_StreamNotifyClient.
     *  @param inStream      The stream which contains the information.
     *  @param ixfer         The transfer syntax which was used to encode
     *                       the information in inStream.
     *  @param glenc         Encoding type for group length; specifies
     *                       what will be done with group length tags.
     *  @param maxReadLength Maximum read length for reading an attribute value.
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition read(DcmInputStream &inStream,
                             const E_TransferSyntax ixfer,
                             const E_GrpLenEncoding glenc = EGL_noChange,
                             const Uint32 maxReadLength = DCM_MaxReadLength);

    /** write object to a stream
     *  @param outStream DICOM output stream
     *  @param oxfer output transfer syntax
     *  @param enctype encoding types (undefined or explicit length)
     *  @param wcache pointer to write cache object, may be NULL
     *  @return status, EC_Normal if successful, an error code otherwise
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

    /** insert a new element into the list of elements maintained by this item.
     *  The list of elements is always kept in ascending tag order.
     *  @param elem element to be inserted, must not be contained in this or
     *    any other item.  Will be deleted upon destruction of this item object.
     *  @param replaceOld if true, this element replaces any other element with
     *    the same tag which may already be contained in the item.  If false,
     *    insert fails if another element with the same tag is already present.
     *  @param checkInsertOrder if true, a warning message is sent to the console
     *    if the element is not inserted at the end of the list.  This is used
     *    in the read() method to detect datasets with out-of-order elements.
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition insert(DcmElement *elem,
                               OFBool replaceOld = OFFalse,
                               OFBool checkInsertOrder = OFFalse);

    /** access an element from the item. This method returns a pointer to one
     *  of the elements in the item, and not a copy.
     *  @param num index number of element, must be < card()
     *  @return pointer to element if found, NULL if num >= card()
     */
    virtual DcmElement *getElement(const unsigned long num);

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

    /** remove element from list. If found, the element is not deleted but
     *  returned to the caller who is responsible for further management of the
     *  DcmElement object.
     *  @param num index number of element, must be < card()
     *  @return pointer to DcmElement if found, NULL otherwise
     */
    virtual DcmElement *remove(const unsigned long num);

    /** remove element from list. If found, the element is not deleted but
     *  returned to the caller who is responsible for further management of the
     *  DcmElement object.
     *  @param elem pointer to element (as type DcmObject *) to be removed from list
     *  @return pointer to element (as type DcmElement *) if found, NULL otherwise
     */
    virtual DcmElement *remove(DcmObject *elem);

    /** remove element from list. If found, the element is not deleted but
     *  returned to the caller who is responsible for further management of the
     *  DcmElement object.
     *  @param tag attribute tag of element to be removed
     *  @return pointer to DcmElement if found, NULL otherwise
     */
    virtual DcmElement *remove(const DcmTagKey &tag);

    /** check if this item is empty
     *  @param normalize not used for this class
     *  @return true if item is empty, i.e. has no elements, false otherwise
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
    virtual OFCondition verify(const OFBool autocorrect = OFFalse );

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
     *  @return EC_Normal if found, EC_TagNotFound if not found, an error code is something
     *    went wrong.
     */
    virtual OFCondition search(const DcmTagKey &xtag,              // in
                               DcmStack &resultStack,              // inout
                               E_SearchMode mode = ESM_fromHere,   // in
                               OFBool searchIntoSub = OFTrue );    // in

    /** this method loads all attribute values maintained by this object and
     *  all sub-objects (in case of a container such as DcmDataset) into memory.
     *  After a call to this method, the file from which a dataset was read may safely
     *  be deleted or replaced. For large files, this method may obviously allocate large
     *  amounts of memory.
     *  @return EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition loadAllDataIntoMemory();

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
    virtual OFCondition computeGroupLengthAndPadding(const E_GrpLenEncoding glenc,
                                                     const E_PaddingEncoding padenc = EPD_noChange,
                                                     const E_TransferSyntax xfer = EXS_Unknown,
                                                     const E_EncodingType enctype = EET_ExplicitLength,
                                                     const Uint32 padlen = 0,
                                                     const Uint32 subPadlen = 0,
                                                     Uint32 instanceLength = 0);


    /** check if an element with the given attribute tag exists in the dataset
     *  @param key tag key to be searched
     *  @param searchIntoSub if true, do hierarchical search within sequences,
     *    if false only search through this dataset
     *  @return true if tag found, false otherwise
     */
    OFBool tagExists(const DcmTagKey &key,
                     OFBool searchIntoSub = OFFalse);

    /** check if an element with the given attribute tag exists in the dataset
     *  and has a non-empty value (i.e., length > 0)
     *  @param key tag key to be searched
     *  @param searchIntoSub if true, do hierarchical search within sequences,
     *    if false only search through this dataset
     *  @return true if tag found and element non-empty, false otherwise
     */
    OFBool tagExistsWithValue(const DcmTagKey &key,
                              OFBool searchIntoSub = OFFalse);

    /* --- findAndGet functions: find an element and get it or the value, respectively --- */

    /** find element and get a pointer to it (or copy it).
     *  Applicable to all DICOM value representations (VR).
     *  The result variable 'element' is automatically set to NULL if an error occurs.
     *  @param tagKey DICOM tag specifying the attribute to be searched for
     *  @param element variable in which the reference to (or copy of) the element is stored
     *  @param searchIntoSub flag indicating whether to search into sequences or not
     *  @param createCopy create a copy of the element if true, return a reference otherwise
     *  @return EC_Normal upon success, an error code otherwise.
     */
    OFCondition findAndGetElement(const DcmTagKey &tagKey,
                                  DcmElement *&element,
                                  const OFBool searchIntoSub = OFFalse,
                                  const OFBool createCopy = OFFalse);

    /** find all elements matching a particular tag and return references to them on a stack.
     *  This functions always performs a deep search (i.e. searches into sequence of items).
     *  @param tagKey DICOM tag specifying the attribute to be searched for
     *  @param resultStack stack where references to the elements are stored (added to).
     *    If no element is found, the stack is not modified (e.g. cleared).
     *  @return EC_Normal if at least one matching tag is found, an error code otherwise.
     */
    OFCondition findAndGetElements(const DcmTagKey &tagKey,
                                   DcmStack &resultStack);

    /** find element and get value as a reference to a C string.
     *  Applicable to the following VRs: AE, AS, CS, DA, DS, DT, IS, LO, LT, PN, SH, ST, TM, UI, UT
     *  Since the getString() routine is called internally the resulting string reference represents
     *  the (possibly multi-valued) value as stored in the dataset, i.e. no normalization is performed.
     *  The result variable 'value' is automatically set to NULL if an error occurs.
     *  @param tagKey DICOM tag specifying the attribute to be searched for
     *  @param value variable in which the reference to the element value is stored (might be NULL)
     *  @param searchIntoSub flag indicating whether to search into sequences or not
     *  @return EC_Normal upon success, an error code otherwise.
     */
    OFCondition findAndGetString(const DcmTagKey &tagKey,
                                 const char *&value,
                                 const OFBool searchIntoSub = OFFalse);

    /** find element and get value as a C++ string (only one component).
     *  Applicable to the following VRs: AE, AS, AT, CS, DA, DS, DT, FL, FD, IS, LO, LT, OB, OF, OW,
     *  PN, SH, SL, SS, ST, TM, UI, UL, US, UT
     *  Since the getOFString() routine is called internally the resulting string is normalized, i.e.
     *  leading and/or trailing spaces are removed according to the associated value representation,
     *  or the element value is converted to a character string (for non-string VRs) - see documentation
     *  in the corresponding header file.
     *  In contrast to the above and below function only the specified component (see parameter 'pos')
     *  is returned. The result variable 'value' is automatically set to an empty string if an error
     *  occurs.
     *  @param tagKey DICOM tag specifying the attribute to be searched for
     *  @param value variable in which the element value is stored
     *  @param pos index of the value in case of multi-valued elements (0..vm-1)
     *  @param searchIntoSub flag indicating whether to search into sequences or not
     *  @return EC_Normal upon success, an error code otherwise.
     */
    OFCondition findAndGetOFString(const DcmTagKey &tagKey,
                                   OFString &value,
                                   const unsigned long pos = 0,
                                   const OFBool searchIntoSub = OFFalse);

    /** find element and get value as a C++ string (all components).
     *  Applicable to the following VRs: AE, AS, AT, CS, DA, DS, DT, FL, FD, IS, LO, LT, OB, OF, OW,
     *  PN, SH, SL, SS, ST, TM, UI, UL, US, UT
     *  Since the getOFStringArray() routine is called internally the resulting string is normalized,
     *  i.e. leading and/or trailing spaces are removed according to the associated value representation
     *  or the element values are converted to character strings (for non-string VRs) - see documentation
     *  in the corresponding header file.
     *  The result variable 'value' is automatically set to an empty string if an error occurs.
     *  @param tagKey DICOM tag specifying the attribute to be searched for
     *  @param value variable in which the element value is stored
     *  @param searchIntoSub flag indicating whether to search into sequences or not
     *  @return EC_Normal upon success, an error code otherwise.
     */
    OFCondition findAndGetOFStringArray(const DcmTagKey &tagKey,
                                        OFString &value,
                                        const OFBool searchIntoSub = OFFalse);

    /** find element and get value as an unsigned 8-bit integer.
     *  Applicable to the following VRs: OB
     *  The result variable 'value' is automatically set to zero if an error occurs.
     *  @param tagKey DICOM tag specifying the attribute to be searched for
     *  @param value variable in which the element value is stored
     *  @param pos index of the value in case of multi-valued elements (0..vm-1)
     *  @param searchIntoSub flag indicating whether to search into sequences or not
     *  @return EC_Normal upon success, an error code otherwise.
     */
    OFCondition findAndGetUint8(const DcmTagKey &tagKey,
                                Uint8 &value,
                                const unsigned long pos = 0,
                                const OFBool searchIntoSub = OFFalse);

    /** find element and get value as an array of unsigned 8-bit integers.
     *  Applicable to the following VRs: OB
     *  The result variable 'value' is automatically set to NULL if an error occurs.
     *  @param tagKey DICOM tag specifying the attribute to be searched for
     *  @param value variable in which the reference to the element value is stored
     *  @param count stores number of items in the result array (if not NULL)
     *  @param searchIntoSub flag indicating whether to search into sequences or not
     *  @return EC_Normal upon success, an error code otherwise.
     */
    OFCondition findAndGetUint8Array(const DcmTagKey &tagKey,
                                     const Uint8 *&value,
                                     unsigned long *count = NULL,
                                     const OFBool searchIntoSub = OFFalse);

    /** find element and get value as an unsigned 16-bit integer.
     *  Applicable to the following VRs: OW, US
     *  The result variable 'value' is automatically set to zero if an error occurs.
     *  @param tagKey DICOM tag specifying the attribute to be searched for
     *  @param value variable in which the element value is stored
     *  @param pos index of the value in case of multi-valued elements (0..vm-1)
     *  @param searchIntoSub flag indicating whether to search into sequences or not
     *  @return EC_Normal upon success, an error code otherwise.
     */
    OFCondition findAndGetUint16(const DcmTagKey &tagKey,
                                 Uint16 &value,
                                 const unsigned long pos = 0,
                                 const OFBool searchIntoSub = OFFalse);

    /** find element and get value as an array of unsigned 16-bit integers.
     *  Applicable to the following VRs: AT, OW, US
     *  The result variable 'value' is automatically set to NULL if an error occurs.
     *  @param tagKey DICOM tag specifying the attribute to be searched for
     *  @param value variable in which the reference to the element value is stored
     *  @param count stores number of items in the result array (if not NULL)
     *  @param searchIntoSub flag indicating whether to search into sequences or not
     *  @return EC_Normal upon success, an error code otherwise.
     */
    OFCondition findAndGetUint16Array(const DcmTagKey &tagKey,
                                      const Uint16 *&value,
                                      unsigned long *count = NULL,
                                      const OFBool searchIntoSub = OFFalse);

    /** find element and get value as a signed 16-bit integer.
     *  Applicable to the following VRs: SS
     *  The result variable 'value' is automatically set to zero if an error occurs.
     *  @param tagKey DICOM tag specifying the attribute to be searched for
     *  @param value variable in which the element value is stored
     *  @param pos index of the value in case of multi-valued elements (0..vm-1)
     *  @param searchIntoSub flag indicating whether to search into sequences or not
     *  @return EC_Normal upon success, an error code otherwise.
     */
    OFCondition findAndGetSint16(const DcmTagKey &tagKey,
                                 Sint16 &value,
                                 const unsigned long pos = 0,
                                 const OFBool searchIntoSub = OFFalse);

    /** find element and get value as an array of signed 16-bit integers.
     *  Applicable to the following VRs: SS
     *  The result variable 'value' is automatically set to NULL if an error occurs.
     *  @param tagKey DICOM tag specifying the attribute to be searched for
     *  @param value variable in which the reference to the element value is stored
     *  @param count stores number of items in the result array (if not NULL)
     *  @param searchIntoSub flag indicating whether to search into sequences or not
     *  @return EC_Normal upon success, an error code otherwise.
     */
    OFCondition findAndGetSint16Array(const DcmTagKey &tagKey,
                                      const Sint16 *&value,
                                      unsigned long *count = NULL,
                                      const OFBool searchIntoSub = OFFalse);

    /** find element and get value as an unsigned 32-bit integer.
     *  Applicable to the following VRs: UL
     *  The result variable 'value' is automatically set to zero if an error occurs.
     *  @param tagKey DICOM tag specifying the attribute to be searched for
     *  @param value variable in which the element value is stored
     *  @param pos index of the value in case of multi-valued elements (0..vm-1)
     *  @param searchIntoSub flag indicating whether to search into sequences or not
     *  @return EC_Normal upon success, an error code otherwise.
     */
    OFCondition findAndGetUint32(const DcmTagKey &tagKey,
                                 Uint32 &value,
                                 const unsigned long pos = 0,
                                 const OFBool searchIntoSub = OFFalse);

    /** find element and get value as an array of unsigned 32-bit integers.
     *  Applicable to the following VRs: UL
     *  The result variable 'value' is automatically set to NULL if an error occurs.
     *  @param tagKey DICOM tag specifying the attribute to be searched for
     *  @param value variable in which the reference to the element value is stored
     *  @param count stores number of items in the result array (if not NULL)
     *  @param searchIntoSub flag indicating whether to search into sequences or not
     *  @return EC_Normal upon success, an error code otherwise.
     */
    OFCondition findAndGetUint32Array(const DcmTagKey &tagKey,
                                      const Uint32 *&value,
                                      unsigned long *count = NULL,
                                      const OFBool searchIntoSub = OFFalse);

    /** find element and get value as a signed 32-bit integer.
     *  Applicable to the following VRs: IS, SL
     *  The result variable 'value' is automatically set to zero if an error occurs.
     *  @param tagKey DICOM tag specifying the attribute to be searched for
     *  @param value variable in which the element value is stored
     *  @param pos index of the value in case of multi-valued elements (0..vm-1)
     *  @param searchIntoSub flag indicating whether to search into sequences or not
     *  @return EC_Normal upon success, an error code otherwise.
     */
    OFCondition findAndGetSint32(const DcmTagKey &tagKey,
                                 Sint32 &value,
                                 const unsigned long pos = 0,
                                 const OFBool searchIntoSub = OFFalse);

    /** find element and get value as an array of signed 32-bit integers.
     *  Applicable to the following VRs: SL
     *  The result variable 'value' is automatically set to NULL if an error occurs.
     *  @param tagKey DICOM tag specifying the attribute to be searched for
     *  @param value variable in which the reference to the element value is stored
     *  @param count stores number of items in the result array (if not NULL)
     *  @param searchIntoSub flag indicating whether to search into sequences or not
     *  @return EC_Normal upon success, an error code otherwise.
     */
    OFCondition findAndGetSint32Array(const DcmTagKey &tagKey,
                                      const Sint32 *&value,
                                      unsigned long *count = NULL,
                                      const OFBool searchIntoSub = OFFalse);

    /** find element and get value as a (signed) long integer.
     *  Applicable to the following VRs: IS, SL, SS, UL, US
     *  The result variable 'value' is automatically set to zero if an error occurs.
     *  @param tagKey DICOM tag specifying the attribute to be searched for
     *  @param value variable in which the element value is stored
     *  @param pos index of the value in case of multi-valued elements (0..vm-1)
     *  @param searchIntoSub flag indicating whether to search into sequences or not
     *  @return EC_Normal upon success, an error code otherwise.
     */
    OFCondition findAndGetLongInt(const DcmTagKey &tagKey,
                                  long int &value,
                                  const unsigned long pos = 0,
                                  const OFBool searchIntoSub = OFFalse);

    /** find element and get value as a 32-bit floating point.
     *  Applicable to the following VRs: FL, OF
     *  The result variable 'value' is automatically set to zero if an error occurs.
     *  @param tagKey DICOM tag specifying the attribute to be searched for
     *  @param value variable in which the element value is stored
     *  @param pos index of the value in case of multi-valued elements (0..vm-1)
     *  @param searchIntoSub flag indicating whether to search into sequences or not
     *  @return EC_Normal upon success, an error code otherwise.
     */
    OFCondition findAndGetFloat32(const DcmTagKey &tagKey,
                                  Float32 &value,
                                  const unsigned long pos = 0,
                                  const OFBool searchIntoSub = OFFalse);

    /** find element and get value as an array of 32-bit floating point values.
     *  Applicable to the following VRs: FL, OF
     *  The result variable 'value' is automatically set to NULL if an error occurs.
     *  @param tagKey DICOM tag specifying the attribute to be searched for
     *  @param value variable in which the reference to the element value is stored
     *  @param count stores number of items in the result array (if not NULL)
     *  @param searchIntoSub flag indicating whether to search into sequences or not
     *  @return EC_Normal upon success, an error code otherwise.
     */
    OFCondition findAndGetFloat32Array(const DcmTagKey &tagKey,
                                       const Float32 *&value,
                                       unsigned long *count = NULL,
                                       const OFBool searchIntoSub = OFFalse);

    /** find element and get value as a 64-bit floating point.
     *  Applicable to the following VRs: DS, FD
     *  The result variable 'value' is automatically set to zero if an error occurs.
     *  @param tagKey DICOM tag specifying the attribute to be searched for
     *  @param value variable in which the element value is stored
     *  @param pos index of the value in case of multi-valued elements (0..vm-1)
     *  @param searchIntoSub flag indicating whether to search into sequences or not
     *  @return EC_Normal upon success, an error code otherwise.
     */
    OFCondition findAndGetFloat64(const DcmTagKey &tagKey,
                                  Float64 &value,
                                  const unsigned long pos = 0,
                                  const OFBool searchIntoSub = OFFalse);

    /** find element and get value as an array of 64-bit floating point values.
     *  Applicable to the following VRs: FD
     *  The result variable 'value' is automatically set to NULL if an error occurs.
     *  @param tagKey DICOM tag specifying the attribute to be searched for
     *  @param value variable in which the reference to the element value is stored
     *  @param count stores number of items in the result array (if not NULL)
     *  @param searchIntoSub flag indicating whether to search into sequences or not
     *  @return EC_Normal upon success, an error code otherwise.
     */
    OFCondition findAndGetFloat64Array(const DcmTagKey &tagKey,
                                       const Float64 *&value,
                                       unsigned long *count = NULL,
                                       const OFBool searchIntoSub = OFFalse);

    /** looks up and returns a given sequence.
     *  Applicable to the following VRs: SQ, (pixelSQ)
     *  The result variable 'sequence' is automatically set to NULL if an error occurs
     *  (e.g. if 'seqTagKey' does not refer to a sequence attribute).
     *  @param seqTagKey DICOM tag specifying the sequence attribute to be searched for
     *  @param sequence variable in which the reference to (or copy of) the sequence is stored
     *  @param searchIntoSub flag indicating whether to search into sub-sequences or not
     *  @param createCopy create a copy of the sequence if true, return a reference otherwise
     *  @return EC_Normal upon success, an error otherwise.
     */
    OFCondition findAndGetSequence(const DcmTagKey &seqTagKey,
                                   DcmSequenceOfItems *&sequence,
                                   const OFBool searchIntoSub = OFFalse,
                                   const OFBool createCopy = OFFalse);

    /** looks up and returns a given sequence item, if it exists. Otherwise sets 'item'
     *  to NULL and returns EC_TagNotFound (specified sequence does not exist) or
     *  EC_IllegalParameter (specified item does not exist). Only the top-most level of
     *  the dataset/item is examined (i.e. no deep-search is performed).
     *  Applicable to the following VRs: SQ, (pixelSQ)
     *  @param seqTagKey DICOM tag specifying the sequence attribute to be searched for
     *  @param item variable in which the reference to (or copy of) the item is stored
     *  @param itemNum number of the item to be searched for (0..n-1, -1 for last)
     *  @param createCopy create a copy of the item if true, return a reference otherwise
     *  @return EC_Normal upon success, an error otherwise.
     */
    OFCondition findAndGetSequenceItem(const DcmTagKey &seqTagKey,
                                       DcmItem *&item,
                                       const signed long itemNum = 0,
                                       const OFBool createCopy = OFFalse);


    /* --- findOrCreate functions: find an element or create a new one --- */

    /** looks up the given sequence in the current dataset and returns the given item.
     *  If either the sequence or the item do not exist, they are created. If necessary,
     *  multiple empty items are inserted. Only the top-most level of the dataset/item
     *  is examined (i.e. no deep-search is performed).
     *  Applicable to the following VRs: SQ, (pixelSQ)
     *  @param seqTag DICOM tag specifying the sequence attribute to be searched for
     *    (or to be created)
     *  @param item variable in which the reference to the sequence item is stored
     *  @param itemNum number of the item to be searched for (0..n-1, -1 for last,
     *    -2 for append new)
     *  @return EC_Normal upon success, an error otherwise.
     */
    OFCondition findOrCreateSequenceItem(const DcmTag &seqTag,
                                         DcmItem *&item,
                                         const signed long itemNum = 0);

    /* --- findAndXXX functions: find an element and do something with it --- */

    /** find element, create a copy and insert it into the given destination dataset.
     *  This functions never performs a deep search (i.e. does not search into sequence
     *  of items). Empty elements are also copied. However, if the given tag is not
     *  found in the current dataset, EC_TagNotFound is returned and the destination
     *  dataset remains unchanged.
     *  Applicable to all DICOM value representations (VR).
     *  @param tagKey DICOM tag specifying the attribute to be searched for
     *  @param destItem destination dataset to which the copied element is inserted
     *  @param replaceOld flag indicating whether to replace an existing element or not
     *  @return EC_Normal upon success, an error code otherwise
     */
    OFCondition findAndInsertCopyOfElement(const DcmTagKey &tagKey,
                                           DcmItem *destItem,
                                           const OFBool replaceOld = OFTrue);

    /** find element, remove it from the dataset and free the associated memory.
     *  Applicable to all DICOM value representations (VR).
     *  @param tagKey DICOM tag specifying the attribute to be searched for
     *  @param allOccurrences flag indicating whether to delete all occurrences of the
     *    attribute tag or the first one only (implies 'searchIntoSub' to be true)
     *  @param searchIntoSub flag indicating whether to search into sequences or not
     *  @return EC_Normal upon success, an error code otherwise.
     */
    OFCondition findAndDeleteElement(const DcmTagKey &tagKey,
                                     const OFBool allOccurrences = OFFalse,
                                     const OFBool searchIntoSub = OFFalse);

    /** looks up the given sequence in the current dataset and deletes the given item.
     *  Applicable to the following VRs: SQ, (pixelSQ)
     *  @param seqTagKey DICOM tag specifying the sequence attribute to be searched for
     *  @param itemNum number of the item to be deleted (0..n-1, -1 for last)
     *  @return EC_Normal upon success, an error otherwise.
     */
    OFCondition findAndDeleteSequenceItem(const DcmTagKey &seqTagKey,
                                          const signed long itemNum);


    /* --- putAndInsert functions: put value and insert new element --- */

    /** create a new element, put specified value to it and insert the element into the dataset/item.
     *  Applicable to the following VRs: AE, AS, AT, CS, DA, DS, DT, FL, FD, IS, LO, LT, OB, OF, OW,
     *  PN, SH, SL, SS, ST, TM, UI, UL, US, UT
     *  @param tag DICOM tag specifying the attribute to be created
     *  @param value value to be set for the new element (might be empty or NULL)
     *  @param replaceOld flag indicating whether to replace an existing element or not
     *  @return EC_Normal upon success, an error code otherwise.
     */
    OFCondition putAndInsertString(const DcmTag &tag,
                                   const char *value,
                                   const OFBool replaceOld = OFTrue);

    /** create a new element, put specified value to it and insert the element into the dataset/item.
     *  Applicable to the following VRs: AE, AS, CS, DA, DS, DT, IS, LO, LT, PN, SH, ST, TM, UI, UT
     *  @param tag DICOM tag specifying the attribute to be created
     *  @param value value to be set for the new element (might be empty)
     *  @param replaceOld flag indicating whether to replace an existing element or not
     *  @return EC_Normal upon success, an error code otherwise.
     */
    OFCondition putAndInsertOFStringArray(const DcmTag &tag,
                                          const OFString &value,
                                          const OFBool replaceOld = OFTrue);

    /** create a new element, put specified value to it and insert the element into the dataset/item.
     *  Applicable to the following VRs: OB, ox (polymorph OB/OW or pixel data)
     *  @param tag DICOM tag specifying the attribute to be created
     *  @param value value to be set for the new element (might be NULL)
     *  @param count number of values (= bytes in this case) to be copied from 'value'
     *  @param replaceOld flag indicating whether to replace an existing element or not
     *  @return EC_Normal upon success, an error code otherwise.
     */
    OFCondition putAndInsertUint8Array(const DcmTag &tag,
                                       const Uint8 *value,
                                       const unsigned long count,
                                       const OFBool replaceOld = OFTrue);

    /** create a new element, put specified value to it and insert the element into the dataset/item.
     *  Applicable to the following VRs: US, xs (US or SS)
     *  @param tag DICOM tag specifying the attribute to be created
     *  @param value value to be set for the new element
     *  @param pos index of the value to be set (0..vm). A value can be appended to
     *    the end of or inserted within the existing value field.
     *  @param replaceOld flag indicating whether to replace an existing element or not
     *  @return EC_Normal upon success, an error code otherwise.
     */
    OFCondition putAndInsertUint16(const DcmTag &tag,
                                   const Uint16 value,
                                   const unsigned long pos = 0,
                                   const OFBool replaceOld = OFTrue);

    /** create a new element, put specified value to it and insert the element into the dataset/item.
     *  Applicable to the following VRs: AT, OW, US, ox (polymorph OB/OW or pixel data), xs (US or SS)
     *  @param tag DICOM tag specifying the attribute to be created
     *  @param value value to be set for the new element (might be NULL)
     *  @param count number of values (not bytes!) to be copied from 'value'
     *  @param replaceOld flag indicating whether to replace an existing element or not
     *  @return EC_Normal upon success, an error code otherwise.
     */
    OFCondition putAndInsertUint16Array(const DcmTag &tag,
                                        const Uint16 *value,
                                        const unsigned long count,
                                        const OFBool replaceOld = OFTrue);

    /** create a new element, put specified value to it and insert the element into the dataset/item.
     *  Applicable to the following VRs: SS, xs (US or SS)
     *  @param tag DICOM tag specifying the attribute to be created
     *  @param value value to be set for the new element
     *  @param pos index of the value to be set (0..vm). A value can be appended to
     *    the end of or inserted within the existing value field.
     *  @param replaceOld flag indicating whether to replace an existing element or not
     *  @return EC_Normal upon success, an error code otherwise.
     */
    OFCondition putAndInsertSint16(const DcmTag &tag,
                                   const Sint16 value,
                                   const unsigned long pos = 0,
                                   const OFBool replaceOld = OFTrue);

    /** create a new element, put specified value to it and insert the element into the dataset/item.
     *  Applicable to the following VRs: SS, xs (US or SS)
     *  @param tag DICOM tag specifying the attribute to be created
     *  @param value value to be set for the new element
     *  @param count number of values (not bytes!) to be copied from 'value'
     *  @param replaceOld flag indicating whether to replace an existing element or not
     *  @return EC_Normal upon success, an error code otherwise.
     */
    OFCondition putAndInsertSint16Array(const DcmTag &tag,
                                        const Sint16 *value,
                                        const unsigned long count,
                                        const OFBool replaceOld = OFTrue);

    /** create a new element, put specified value to it and insert the element into the dataset/item.
     *  Applicable to the following VRs: UL
     *  @param tag DICOM tag specifying the attribute to be created
     *  @param value value to be set for the new element
     *  @param pos index of the value to be set (0..vm). A value can be appended to
     *    the end of or inserted within the existing value field.
     *  @param replaceOld flag indicating whether to replace an existing element or not
     *  @return EC_Normal upon success, an error code otherwise.
     */
    OFCondition putAndInsertUint32(const DcmTag &tag,
                                   const Uint32 value,
                                   const unsigned long pos = 0,
                                   const OFBool replaceOld = OFTrue);

    /** create a new element, put specified value to it and insert the element into the dataset/item.
     *  Applicable to the following VRs: SL
     *  @param tag DICOM tag specifying the attribute to be created
     *  @param value value to be set for the new element
     *  @param pos index of the value to be set (0..vm). A value can be appended to
     *    the end of or inserted within the existing value field.
     *  @param replaceOld flag indicating whether to replace an existing element or not
     *  @return EC_Normal upon success, an error code otherwise.
     */
    OFCondition putAndInsertSint32(const DcmTag &tag,
                                   const Sint32 value,
                                   const unsigned long pos = 0,
                                   const OFBool replaceOld = OFTrue);

    /** create a new element, put specified value to it and insert the element into the dataset/item.
     *  Applicable to the following VRs: FL, OF
     *  @param tag DICOM tag specifying the attribute to be created
     *  @param value value to be set for the new element
     *  @param pos index of the value to be set (0..vm). A value can be appended to
     *    the end of or inserted within the existing value field.
     *  @param replaceOld flag indicating whether to replace an existing element or not
     *  @return EC_Normal upon success, an error code otherwise.
     */
    OFCondition putAndInsertFloat32(const DcmTag &tag,
                                    const Float32 value,
                                    const unsigned long pos = 0,
                                    const OFBool replaceOld = OFTrue);

    /** create a new element, put specified value to it and insert the element into the dataset/item.
     *  Applicable to the following VRs: FD
     *  @param tag DICOM tag specifying the attribute to be created
     *  @param value value to be set for the new element
     *  @param pos index of the value to be set (0..vm). A value can be appended to
     *    the end of or inserted within the existing value field.
     *  @param replaceOld flag indicating whether to replace an existing element or not
     *  @return EC_Normal upon success, an error code otherwise.
     */
    OFCondition putAndInsertFloat64(const DcmTag &tag,
                                    const Float64 value,
                                    const unsigned long pos = 0,
                                    const OFBool replaceOld = OFTrue);


    /* --- insertXXX functions: insert new element --- */

    /** create a new element (with no value) and insert it into the dataset/item.
     *  Applicable to the following VRs: AE, AS, AT, CS, DA, DS, DT, FL, FD, IS, LO, LT, OB, OF, OW,
     *  PN, SH, SL, SQ, SS, ST, TM, UI, UL, US, UT
     *  @param tag DICOM tag specifying the attribute to be created
     *  @param replaceOld flag indicating whether to replace an existing element or not
     *  @return EC_Normal upon success, an error code otherwise.
     */
    OFCondition insertEmptyElement(const DcmTag &tag,
                                   const OFBool replaceOld = OFTrue);

    /** looks up the given sequence in the current dataset and inserts the given item.
     *  If the sequence does not exist, it is created. If necessary, multiple empty items
     *  are inserted before the specified item position. Only the top-most level of the
     *  dataset/item is examined (i.e. no deep-search is performed).
     *  Applicable to the following VRs: SQ, (pixelSQ)
     *  @param seqTag DICOM tag specifying the sequence attribute to be searched for
     *    (or to be created)
     *  @param item item to be inserted into the sequence, must not be contained in this
     *    or any other sequence. Will be deleted upon destruction of the sequence object.
     *  @param itemNum position of the item (0..n-1, -1 = before last, -2 = after last)
     *  @return EC_Normal upon success, an error otherwise (delete 'item' manually!).
     */
    OFCondition insertSequenceItem(const DcmTag &seqTag,
                                   DcmItem *item,
                                   const signed long itemNum = -2);

  protected:

    /// the list of elements maintained by this object
    DcmList *elementList;

    /** flag used during suspended I/O. Indicates whether the last element
     *  was completely or only partially read/written during the last call
     *  to read/write.
     */
    OFBool lastElementComplete;

    /** used during reading. Contains the position in the stream where
     *  the item started (needed for calculating the remaining number of
     *  bytes available for a fixed-length item).
     */
    offile_off_t fStartPosition;

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
     *  @param bytesRead Contains in the end the amount of bytes which were
     *                   read from inStream.
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition readTagAndLength(DcmInputStream &inStream,       // inout
                                 const E_TransferSyntax xfer,    // in
                                 DcmTag &tag,                    // out
                                 Uint32 &length,                 // out
                                 Uint32 &bytesRead);             // out

    /** This function creates a new DcmElement object on the basis of the newTag
     *  and newLength information which was passed, inserts this new element into
     *  elementList, reads the actual data value which belongs to this element
     *  (attribute) from the inStream and also assigns this information to the
     *  object which was created at the beginning.
     *  @param inStream      The stream which contains the information.
     *  @param newTag        The tag of the element of which the information is
     *                       being read.
     *  @param newLength     The length of the information which is being read.
     *  @param xfer          The transfer syntax which was used to encode the
     *                       information in inStream.
     *  @param glenc         Encoding type for group length. Specifies what will
     *                       be done with group length tags.
     *  @param maxReadLength Maximum read length for reading the attribute value.
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition readSubElement(DcmInputStream &inStream,         // inout
                               DcmTag &newTag,                   // inout
                               const Uint32 newLength,           // in
                               const E_TransferSyntax xfer,      // in
                               const E_GrpLenEncoding glenc,     // in
                               const Uint32 maxReadLength = DCM_MaxReadLength);

    /** This function reads the first 6 bytes from the input stream and determines
     *  the transfer syntax which was used to code the information in the stream.
     *  The decision is based on two questions: a) Did we encounter a valid tag?
     *  and b) Do the last 2 bytes which were read from the stream represent a valid
     *  VR? In certain special cases, where the transfer syntax cannot be determined
     *  without doubt, we want to guess the most likely transfer syntax (see code).
     *  @param inStream The stream which contains the coded information.
     *  @return The transfer syntax which was determined.
     */
    E_TransferSyntax checkTransferSyntax(DcmInputStream &inStream);

    /** check whether the given tag requires some special handling regarding the VR
     *  (i.e. in case it is undefined and multiple values are possible). If required,
     *  the VR of the given element tag is then updated according to the DICOM
     *  standard, e.g. the VR of PixelPaddingValue (if undefined) is set to 'SS' or
     *  'US' depending on the value of PixelRepresentation.
     *  @param item dataset or item that can be used to lookup other element values
     *  @param tag tag of the element to be checked and updated (if required)
     */
    void checkAndUpdateVR(DcmItem &item,
                          DcmTag &tag);


  private:

    /** helper function for search(). May only be called if elementList is non-empty.
     *  Performs hierarchical search for given tag and pushes pointer of sub-element
     *  on result stack if found
     *  @param tag tag key to be searched
     *  @param resultStack upon successful return, pointer to element pushed onto this stack
     *  @param searchIntoSub flag indicating whether recursive search is desired
     *  @return EC_Normal if tag found and stack modified, EC_TagNotFound if tag not found
     *    and stack unmodified
     */
    OFCondition searchSubFromHere(const DcmTagKey &tag,          // in
                                  DcmStack &resultStack,         // inout
                                  OFBool searchIntoSub );        // in

    /** helper function that interprets the given pointer as a pointer to an
     *  array of two characters and checks whether these two characters form
     *  a valid standard DICOM VR.
     *  @param atposition pointer to array of (at least) two bytes interpreted as VR
     *  @return true if standard VR, false otherwise
     */
    static OFBool foundVR(const Uint8* atposition);

    /// cache for private creator tags and names
    DcmPrivateTagCache privateCreatorCache;
};

//
// SUPPORT FUNCTIONS
//

/** helper function for DICOM parser. Creates new DICOM element from given attribute tag
 *  @param newElement pointer to newly created element returned in this parameter upon success,
 *    NULL pointer otherwise
 *  @param tag attribute tag of the element to be created
 *  @param length attribute value length of the element to be created
 *  @param privateCreatorCache cache object for private creator strings in the current dataset
 *  @param readAsUN flag indicating whether parser is currently handling a UN element that
 *    must be read in implicit VR little endian; updated upon return
 *  @return EC_Normal upon success, an error code otherwise
 */
OFCondition newDicomElement(DcmElement *&newElement,
                            DcmTag &tag,
                            const Uint32 length,
                            DcmPrivateTagCache *privateCreatorCache,
                            OFBool& readAsUN);

/** helper function for DICOM parser. Creates new DICOM element from given attribute tag
 *  @param newElement pointer to newly created element returned in this parameter upon success,
 *    NULL pointer otherwise
 *  @param tag attribute tag of the element to be created
 *  @param length attribute value length of the element to be created
 *  @return EC_Normal upon success, an error code otherwise
 */
OFCondition newDicomElement(DcmElement *&newElement,
                            const DcmTag &tag,
                            const Uint32 length = 0);

/** helper function for DICOM parser. Creates new DICOM element from given attribute tag
 *  @param tag attribute tag of the element to be created
 *  @param length attribute value length of the element to be created
 *  @return pointer to newly created element returned in this parameter upon success,
 *    NULL pointer otherwise
 */
DcmElement *newDicomElement(const DcmTag &tag,
                            const Uint32 length = 0);

/** helper function for DcmElement::nextObject.
 *  hierarchically traverses all datasets/items after the position indicated by the call stack
 *  @param st stack
 *  @return EC_Normal upon success, an error code otherwise
 */
OFCondition nextUp(DcmStack &st);


#endif // DCITEM_H


/*
** CVS/RCS Log:
** $Log: dcitem.h,v $
** Revision 1.83  2010-11-02 15:31:06  joergr
** Added special handling for data elements that are associated with different
** VRs (according to the data dictinary) when read with an implicit transfer
** syntax, e.g. PixelPaddingValue or WaveformData.
**
** Revision 1.82  2010-10-14 13:15:41  joergr
** Updated copyright header. Added reference to COPYRIGHT file.
**
** Revision 1.81  2010-10-01 13:55:01  joergr
** Added new helper function findAndInsertCopyOfElement().
**
** Revision 1.80  2010-03-01 09:08:44  uli
** Removed some unnecessary include directives in the headers.
**
** Revision 1.79  2010-02-22 11:39:53  uli
** Remove some unneeded includes.
**
** Revision 1.78  2009-09-02 09:58:21  joergr
** Revised documentation of parameter "pos" for some putAndInsertXXX() functions
** in order to make clear what the possible range of values is.
**
** Revision 1.77  2009-08-07 14:40:38  joergr
** Enhanced isEmpty() method by checking whether the data element value consists
** of non-significant characters only.
**
** Revision 1.76  2009-03-25 10:22:09  joergr
** Added new method isEmpty() to DICOM object, item and sequence class.
**
** Revision 1.75  2009-03-05 14:07:56  onken
** Fixed typo.
**
** Revision 1.74  2009-03-05 13:35:47  onken
** Added checks for sequence and item lengths which prevents overflow in length
** field, if total length of contained items (or sequences) exceeds
** 32-bit length field. Also introduced new flag (default: enabled)
** for writing in explicit length mode, which allows for automatically
** switching encoding of only that very sequence/item to undefined
** length coding (thus permitting to actually write the file).
**
** Revision 1.73  2009-02-04 17:52:17  joergr
** Fixes various type mismatches reported by MSVC introduced with OFFile class.
**
** Revision 1.72  2008-12-12 11:44:40  onken
** Moved path access functions to separate classes
**
** Revision 1.71  2008-12-05 13:28:14  onken
** Splitted findOrCreatePath() function API for also offering a simple API
** for non-wildcard searches.
**
** Revision 1.70  2008-12-04 16:55:14  onken
** Changed findOrCreatePath() to also support wildcard as item numbers.
**
** Revision 1.69  2008-11-26 12:08:22  joergr
** Updated documentation of newDicomElement() in order to reflect the current
** implementation.
**
** Revision 1.68  2008-10-15 12:31:20  onken
** Added findOrCreatePath() functions which allow for finding or creating a
** hierarchy of sequences, items and attributes according to a given "path"
** string.
**
** Revision 1.67  2008-07-17 11:19:48  onken
** Updated copyFrom() documentation.
**
** Revision 1.66  2008-07-17 10:30:23  onken
** Implemented copyFrom() method for complete DcmObject class hierarchy, which
** permits setting an instance's value from an existing object. Implemented
** assignment operator where necessary.
**
** Revision 1.65  2008-06-23 12:09:13  joergr
** Fixed inconsistencies in Doxygen API documentation.
**
** Revision 1.64  2007/11/29 14:30:19  meichel
** Write methods now handle large raw data elements (such as pixel data)
**   without loading everything into memory. This allows very large images to
**   be sent over a network connection, or to be copied without ever being
**   fully in memory.
**
** Revision 1.63  2007/09/21 10:40:15  onken
** Changed foundVR() API and implementation to use Uint8* instead of char* to
** avoid calls to isalpha() with negative arguments (undef. behaviour/assertion)
**
** Revision 1.62  2007/06/29 14:17:49  meichel
** Code clean-up: Most member variables in module dcmdata are now private,
**   not protected anymore.
**
** Revision 1.61  2007/06/08 14:56:04  joergr
** Added new helper functions insertSequenceItem(), findAndDeleteSequenceItem().
** Replaced helper function findAndCopyElement() by new optional parameter
** 'createCopy' in various findAndGetXXX() functions.
**
** Revision 1.60  2007/03/09 10:38:13  joergr
** Added support for missing VRs (SL, SS, UL, SS) to insertEmptyElement().
**
** Revision 1.59  2007/02/19 15:04:34  meichel
** Removed searchErrors() methods that are not used anywhere and added
**   error() methods only in the DcmObject subclasses where really used.
**
** Revision 1.58  2006/12/15 14:18:07  joergr
** Added new method that checks whether a DICOM object or element is affected
** by SpecificCharacterSet (0008,0005).
**
** Revision 1.57  2006/12/13 13:58:14  joergr
** Added new optional parameter "checkAllStrings" to method containsExtended
** Characters().
**
** Revision 1.56  2006/08/15 15:49:56  meichel
** Updated all code in module dcmdata to correctly compile when
**   all standard C++ classes remain in namespace std.
**
** Revision 1.55  2006/05/30 15:01:52  joergr
** Modified comment of method containsExtendedCharacters().
**
** Revision 1.54  2006/05/11 08:54:00  joergr
** Moved checkForNonASCIICharacters() from application to library.
**
** Revision 1.53  2005/12/08 16:28:19  meichel
** Changed include path schema for all DCMTK header files
**
** Revision 1.52  2005/11/15 18:28:02  meichel
** Added new global flag dcmEnableUnknownVRConversion that enables the automatic
**   re-conversion of defined length UN elements read in an explicit VR transfer
**   syntax, if the real VR is defined in the data dictionary. Default is OFFalse,
**   i.e. to retain the previous behavior.
**
** Revision 1.51  2005/06/24 10:03:14  joergr
** Added support for internal VR "xs" to putAndInsertXXX() helper methods.
**
** Revision 1.50  2004/07/01 12:28:25  meichel
** Introduced virtual clone method for DcmObject and derived classes.
**
** Revision 1.49  2004/02/04 16:02:08  joergr
** Removed pointer declaration from parameter "resultStack" in method
** findAndGetElements().
**
** Revision 1.48  2003/10/08 10:24:40  joergr
** Added support for AT, OB, OF, OW, SL, SS, UL, US to putAndInsertString().
**
** Revision 1.47  2003/07/16 14:34:05  joergr
** Added new function findAndGetSequence().
**
** Revision 1.46  2003/06/26 09:17:25  onken
** Added commandline-application dcmodify.
**
** Revision 1.45  2003/06/12 13:33:46  joergr
** Fixed inconsistent API documentation reported by Doxygen.
**
** Revision 1.44  2003/06/02 17:16:23  joergr
** Added new helper function DcmItem::findAndCopyElement().
**
** Revision 1.43  2003/06/02 16:58:12  meichel
** Renamed local variables to avoid name clashes with STL
**
** Revision 1.42  2003/05/20 09:01:58  joergr
** Added new helper methods: findAndGetElement(), findAndGetUint32Array(),
** FindAndGetSint32Array(), findAndGetFloat64Array(), findAndDeleteElement().
** Enhanced findAndGetSequenceItem() and findOrCreateSequenceItem() by checking
** the return value of ident() - avoids crashes when applied to non-sequence
** elements.
**
** Revision 1.41  2002/12/09 09:31:15  wilkens
** Modified/Added doc++ documentation.
**
** Revision 1.40  2002/12/06 12:49:10  joergr
** Enhanced "print()" function by re-working the implementation and replacing
** the boolean "showFullData" parameter by a more general integer flag.
** Added doc++ documentation.
** Made source code formatting more consistent with other modules/files.
**
** Revision 1.39  2002/08/27 16:55:34  meichel
** Initial release of new DICOM I/O stream classes that add support for stream
**   compression (deflated little endian explicit VR transfer syntax)
**
** Revision 1.38  2002/08/02 08:42:44  joergr
** Added optional 'pos' parameter to the putAndInsertXXX() methods.
**
** Revision 1.37  2002/07/23 14:21:26  meichel
** Added support for private tag data dictionaries to dcmdata
**
** Revision 1.36  2002/06/26 15:47:40  joergr
** Added support for polymorp OB/OW value representation (e.g. pixel data) to
** putAndInsertUint8/16Array() methods.
**
** Revision 1.35  2002/05/17 09:58:14  meichel
** fixed bug in DcmItem which caused the parser to fail if the same attribute
**   tag appeared twice within one dataset (which is illegal in DICOM anyway).
**   Added console warning if the attributes read are not in ascending order.
**
** Revision 1.34  2002/04/25 10:06:46  joergr
** Added support for XML output of DICOM objects.
**
** Revision 1.33  2001/12/18 11:37:24  joergr
** Added helper method allowing to create and insert empty elements into an
** item/dataset.
**
** Revision 1.32  2001/11/16 15:54:38  meichel
** Adapted digital signature code to final text of supplement 41.
**
** Revision 1.31  2001/11/09 15:51:59  joergr
** Added new helper routines for managing sequences and items.
**
** Revision 1.30  2001/10/10 15:14:04  joergr
** Changed parameter DcmTagKey to DcmTag in DcmItem::putAndInsert... methods
** to support elements which are not in the data dictionary (e.g. private
** extensions).
**
** Revision 1.29  2001/10/02 11:46:45  joergr
** Added functions to get/put 8 bit values/arrays from/to an item/dataset.
**
** Revision 1.28  2001/10/01 15:01:14  joergr
** Introduced new general purpose functions to get/put DICOM element values
** from/to an item/dataset - removed some old and rarely used functions.
**
** Revision 1.27  2001/09/25 17:19:26  meichel
** Adapted dcmdata to class OFCondition
**
** Revision 1.26  2001/06/01 15:48:40  meichel
** Updated copyright header
**
** Revision 1.25  2000/11/07 16:56:06  meichel
** Initial release of dcmsign module for DICOM Digital Signatures
**
** Revision 1.24  2000/04/14 15:31:32  meichel
** Removed default value from output stream passed to print() method.
**   Required for use in multi-thread environments.
**
** Revision 1.23  2000/03/08 16:26:15  meichel
** Updated copyright header.
**
** Revision 1.22  2000/03/03 14:05:24  meichel
** Implemented library support for redirecting error messages into memory
**   instead of printing them to stdout/stderr for GUI applications.
**
** Revision 1.21  2000/02/10 10:50:51  joergr
** Added new feature to dcmdump (enhanced print method of dcmdata): write
** pixel data/item value fields to raw files.
**
** Revision 1.20  1999/03/31 09:24:40  meichel
** Updated copyright header in module dcmdata
**
** Revision 1.19  1998/07/15 15:48:48  joergr
** Removed several compiler warnings reported by gcc 2.8.1 with
** additional options, e.g. missing copy constructors and assignment
** operators, initialization of member variables in the body of a
** constructor instead of the member initialization list, hiding of
** methods by use of identical names, uninitialized member variables,
** missing const declaration of char pointers. Replaced tabs by spaces.
**
** Revision 1.18  1997/09/22 14:50:43  hewett
** - Added 2 simple methods to test for the existance of an attribute
**   to DcmItem class (tagExists and tagExistsWithValue).  This code
**   was part of dcmgpdir.cc but is more generally useful.
** - Added 2 methods to find an attribute and retrieve numeric values
**   to DcmItem class (findIntegerNumber and findRealNumber).  The old
**   method findLong is now marked as obsolete and reimplemented using
**   findIntegerNumber.
**
** Revision 1.17  1997/09/11 15:13:11  hewett
** Modified getOFString method arguments by removing a default value
** for the pos argument.  By requiring the pos argument to be provided
** ensures that callers realise getOFString only gets one component of
** a multi-valued string.
**
** Revision 1.16  1997/08/29 08:32:39  andreas
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
** Revision 1.15  1997/07/21 08:25:08  andreas
** - Replace all boolean types (BOOLEAN, CTNBOOLEAN, DICOM_BOOL, BOOL)
**   with one unique boolean type OFBool.
**
** Revision 1.14  1997/07/07 07:42:03  andreas
** - Changed parameter type DcmTag & to DcmTagKey & in all search functions
**   in DcmItem, DcmSequenceOfItems, DcmDirectoryRecord and DcmObject
**
** Revision 1.13  1997/05/30 06:45:42  andreas
** - fixed problem of inconsistent interfaces and implementation that the
**   syntax check of GNU C++ does not find.
**
** Revision 1.12  1997/05/27 13:48:28  andreas
** - Add method canWriteXfer to class DcmObject and all derived classes.
**   This method checks whether it is possible to convert the original
**   transfer syntax to an new transfer syntax. The check is used in the
**   dcmconv utility to prohibit the change of a compressed transfer
**   syntax to a uncompressed.
**
** Revision 1.11  1997/05/16 08:13:43  andreas
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
** - Deleted obsolete method DcmItem::calcHeaderLength because the
**   samce functionality is performed by DcmXfer::sizeofTagHeader
**
** Revision 1.10  1996/08/05 08:45:23  andreas
** new print routine with additional parameters:
**         - print into files
**         - fix output length for elements
** corrected error in search routine with parameter ESM_fromStackTop
**
** Revision 1.9  1996/07/17 12:38:58  andreas
** new nextObject to iterate a DicomDataset, DicomFileFormat, Item, ...
**
** Revision 1.8  1996/04/29 15:08:53  hewett
** Replaced DcmItem::findInt(...) with the more general DcmItem::findLong(...).
**
** Revision 1.7  1996/04/16 16:00:05  andreas
** - added const for Tag in newDicomElement
**
** Revision 1.6  1996/03/28 18:52:30  hewett
** Added 2 simple find&get methods (findString & findInt).
**
** Revision 1.5  1996/01/29 13:38:12  andreas
** - new put method for every VR to put value as a string
** - better and unique print methods
**
** Revision 1.4  1996/01/09 11:06:15  andreas
** New Support for Visual C++
** Correct problems with inconsistent const declarations
**
** Revision 1.3  1996/01/05 13:22:56  andreas
** - changed to support new streaming facilities
** - more cleanups
** - merged read / write methods for block and file transfer
**
*/
