/*
 *
 *  Copyright (C) 1994-2001, OFFIS
 *
 *  This software and supporting documentation were developed by
 *
 *    Kuratorium OFFIS e.V.
 *    Healthcare Information and Communication Systems
 *    Escherweg 2
 *    D-26121 Oldenburg, Germany
 *
 *  THIS SOFTWARE IS MADE AVAILABLE,  AS IS,  AND OFFIS MAKES NO  WARRANTY
 *  REGARDING  THE  SOFTWARE,  ITS  PERFORMANCE,  ITS  MERCHANTABILITY  OR
 *  FITNESS FOR ANY PARTICULAR USE, FREEDOM FROM ANY COMPUTER DISEASES  OR
 *  ITS CONFORMITY TO ANY SPECIFICATION. THE ENTIRE RISK AS TO QUALITY AND
 *  PERFORMANCE OF THE SOFTWARE IS WITH THE USER.
 *
 *  Module:  dcmdata
 *
 *  Author:  Gerd Ehlers
 *
 *  Purpose: Interface of class DcmItem
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2001-12-18 11:37:24 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/include/Attic/dcitem.h,v $
 *  CVS/RCS Revision: $Revision: 1.33 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#ifndef DCITEM_H
#define DCITEM_H

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include "ofconsol.h"
#include "dctypes.h"
#include "dcobject.h"
#include "dcvrui.h"
#include "dclist.h"
#include "dcstack.h"


/** a class representing a collection of DICOM elements
 */
class DcmItem : public DcmObject
{
private:

 // --- declarations to avoid compiler warnings

    DcmItem &operator=(const DcmItem &);

protected:
    DcmList * elementList;
    OFBool lastElementComplete;
    Uint32 fStartPosition;

    DcmObject*  copyDcmObject(DcmObject *oldObj);

    OFCondition readTagAndLength(DcmStream & inStream,           // inout
                                 const E_TransferSyntax newxfer, // in
                                 DcmTag   &tag,                  // out
                                 Uint32 & length,                // out
                                 Uint32 & bytesRead);            // out

    OFCondition readSubElement(DcmStream & inStream,             // inout
                               DcmTag &newTag,                   // inout
                               const Uint32 newLength,           // in
                               const E_TransferSyntax xfer,      // in
                               const E_GrpLenEncoding glenc,     // in
                               const Uint32 maxReadLength = DCM_MaxReadLength);

    OFCondition searchSubFromHere(const DcmTagKey &tag,          // in
                                  DcmStack &resultStack,         // inout
                                  OFBool searchIntoSub );        // in
    DcmObject * iterObject(const DcmObject * obj,
                           const E_ListPos pos);
    OFBool foundVR(char* atposition );
    E_TransferSyntax checkTransferSyntax(DcmStream & inStream);

public:
    DcmItem(); // create with an item tag
    DcmItem(const DcmTag &tag,
            const Uint32 len = 0);
    DcmItem( const DcmItem& old );
    virtual ~DcmItem();

    virtual DcmEVR ident(void) const;
    virtual OFBool isLeaf(void) const { return OFFalse; }
    virtual void print(ostream & out, const OFBool showFullData = OFTrue,
                       const int level = 0, const char *pixelFileName = NULL,
                       size_t *pixelCounter = NULL);
    virtual unsigned long getVM();
    virtual Uint32 calcElementLength(const E_TransferSyntax xfer,
                                     const E_EncodingType enctype);

    virtual Uint32 getLength(const E_TransferSyntax xfer
                             = EXS_LittleEndianImplicit,
                             const E_EncodingType enctype
                             = EET_UndefinedLength );

    virtual void transferInit();
    virtual void transferEnd();

    virtual OFBool canWriteXfer(const E_TransferSyntax newXfer,
                              const E_TransferSyntax oldXfer);

    virtual OFCondition read(DcmStream & inStream,
                             const E_TransferSyntax ixfer,
                             const E_GrpLenEncoding glenc = EGL_noChange,
                             const Uint32 maxReadLength
                             = DCM_MaxReadLength);

    virtual OFCondition write(DcmStream & outStream,
                              const E_TransferSyntax oxfer,
                              const E_EncodingType enctype
                              = EET_UndefinedLength);

    /** special write method for creation of digital signatures
     */
    virtual OFCondition writeSignatureFormat(DcmStream & outStream,
					 const E_TransferSyntax oxfer,
					 const E_EncodingType enctype
					 = EET_UndefinedLength);

    /** returns true if the object contains an element with Unknown VR at any nesting level
     *  @return true if the object contains an element with Unknown VR, false otherwise
     */
    virtual OFBool containsUnknownVR() const;

    virtual unsigned long card();
    virtual OFCondition insert(DcmElement* elem,
                               OFBool replaceOld = OFFalse);
    virtual DcmElement* getElement(const unsigned long num);

    // get next Object from position in stack. If stack empty
    // get next Object in this item. if intoSub true, scan
    // complete hierarchy, false scan only elements direct in this
    // item (not deeper).
    virtual OFCondition nextObject(DcmStack & stack, const OFBool intoSub);
    virtual DcmObject * nextInContainer(const DcmObject * obj);
    virtual DcmElement* remove(const unsigned long num);
    virtual DcmElement* remove(DcmObject* elem);
    virtual DcmElement* remove(const DcmTagKey & tag);
    virtual OFCondition clear();
    virtual OFCondition verify(const OFBool autocorrect = OFFalse );
    virtual OFCondition search(const DcmTagKey& xtag,              // in
                               DcmStack &resultStack,              // inout
                               E_SearchMode mode = ESM_fromHere,   // in
                               OFBool searchIntoSub = OFTrue );    // in
    virtual OFCondition searchErrors( DcmStack &resultStack );     // inout
    virtual OFCondition loadAllDataIntoMemory(void);

    virtual OFCondition computeGroupLengthAndPadding
                         (const E_GrpLenEncoding glenc,
                          const E_PaddingEncoding padenc = EPD_noChange,
                          const E_TransferSyntax xfer = EXS_Unknown,
                          const E_EncodingType enctype = EET_ExplicitLength,
                          const Uint32 padlen = 0,
                          const Uint32 subPadlen = 0,
                          Uint32 instanceLength = 0);

    /* simple tests for existance */
    OFBool tagExists(const DcmTagKey& key, OFBool searchIntoSub = OFFalse);
    OFBool tagExistsWithValue(const DcmTagKey& key, OFBool searchIntoSub = OFFalse);


    /* --- findAndGet functions: find an element and get the value --- */

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
    OFCondition findAndGetString(const DcmTagKey& tagKey,
                                 const char *&value,
                                 const OFBool searchIntoSub = OFFalse);

    /** find element and get value as a C++ string (only one component).
     *  Applicable to the following VRs: AE, AS, CS, DA, DS, DT, IS, LO, LT, PN, SH, ST, TM, UI, UT
     *  Since the getOFString() routine is called internally the resulting string is normalized, i.e.
     *  leading and/or trailing spaces are removed according to the associated value representation.
     *  In contrast to the above and below function only the specified component (see parameter 'pos')
     *  is returned.
     *  The result variable 'value' is automatically set to an empty string if an error occurs.
     *  @param tagKey DICOM tag specifying the attribute to be searched for
     *  @param value variable in which the element value is stored
     *  @param pos index of the value in case of multi-valued elements (0..vm-1)
     *  @param searchIntoSub flag indicating whether to search into sequences or not
     *  @return EC_Normal upon success, an error code otherwise.
     */
    OFCondition findAndGetOFString(const DcmTagKey& tagKey,
                                   OFString &value,
                                   const unsigned long pos = 0,
                                   const OFBool searchIntoSub = OFFalse);

    /** find element and get value as a C++ string (all components).
     *  Applicable to the following VRs: AE, AS, CS, DA, DS, DT, IS, LO, LT, PN, SH, ST, TM, UI, UT
     *  Since the getOFStringArray() routine is called internally the resulting string is normalized,
     *  i.e. leading and/or trailing spaces are removed according to the associated value representation.
     *  The result variable 'value' is automatically set to an empty string if an error occurs.
     *  @param tagKey DICOM tag specifying the attribute to be searched for
     *  @param value variable in which the element value is stored
     *  @param searchIntoSub flag indicating whether to search into sequences or not
     *  @return EC_Normal upon success, an error code otherwise.
     */
    OFCondition findAndGetOFStringArray(const DcmTagKey& tagKey,
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
    OFCondition findAndGetUint8(const DcmTagKey& tagKey,
                                Uint8 &value,
                                const unsigned long pos = 0,
                                const OFBool searchIntoSub = OFFalse);

    /** find element and get value as an array of unsigned 8-bit integers.
     *  Applicable to the following VRs: OB
     *  The result variable 'value' is automatically set to NULL if an error occurs.
     *  @param tagKey DICOM tag specifying the attribute to be searched for
     *  @param value variable in which the element value is stored
     *  @param searchIntoSub flag indicating whether to search into sequences or not
     *  @return EC_Normal upon success, an error code otherwise.
     */
    OFCondition findAndGetUint8Array(const DcmTagKey& tagKey,
                                     Uint8 *&value,
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
    OFCondition findAndGetUint16(const DcmTagKey& tagKey,
                                 Uint16 &value,
                                 const unsigned long pos = 0,
                                 const OFBool searchIntoSub = OFFalse);

    /** find element and get value as an array of unsigned 16-bit integers.
     *  Applicable to the following VRs: AT, OW, US
     *  The result variable 'value' is automatically set to NULL if an error occurs.
     *  @param tagKey DICOM tag specifying the attribute to be searched for
     *  @param value variable in which the element value is stored
     *  @param searchIntoSub flag indicating whether to search into sequences or not
     *  @return EC_Normal upon success, an error code otherwise.
     */
    OFCondition findAndGetUint16Array(const DcmTagKey& tagKey,
                                      Uint16 *&value,
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
    OFCondition findAndGetSint16(const DcmTagKey& tagKey,
                                 Sint16 &value,
                                 const unsigned long pos = 0,
                                 const OFBool searchIntoSub = OFFalse);

    /** find element and get value as an array of signed 16-bit integers.
     *  Applicable to the following VRs: SS
     *  The result variable 'value' is automatically set to NULL if an error occurs.
     *  @param tagKey DICOM tag specifying the attribute to be searched for
     *  @param value variable in which the element value is stored
     *  @param searchIntoSub flag indicating whether to search into sequences or not
     *  @return EC_Normal upon success, an error code otherwise.
     */
    OFCondition findAndGetSint16Array(const DcmTagKey& tagKey,
                                      Sint16 *&value,
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
    OFCondition findAndGetUint32(const DcmTagKey& tagKey,
                                 Uint32 &value,
                                 const unsigned long pos = 0,
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
    OFCondition findAndGetSint32(const DcmTagKey& tagKey,
                                 Sint32 &value,
                                 const unsigned long pos = 0,
                                 const OFBool searchIntoSub = OFFalse);

    /** find element and get value as a 32-bit floating point.
     *  Applicable to the following VRs: FL
     *  The result variable 'value' is automatically set to zero if an error occurs.
     *  @param tagKey DICOM tag specifying the attribute to be searched for
     *  @param value variable in which the element value is stored
     *  @param pos index of the value in case of multi-valued elements (0..vm-1)
     *  @param searchIntoSub flag indicating whether to search into sequences or not
     *  @return EC_Normal upon success, an error code otherwise.
     */
    OFCondition findAndGetFloat32(const DcmTagKey& tagKey,
                                  Float32 &value,
                                  const unsigned long pos = 0,
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
    OFCondition findAndGetFloat64(const DcmTagKey& tagKey,
                                  Float64 &value,
                                  const unsigned long pos = 0,
                                  const OFBool searchIntoSub = OFFalse);

    /** looks up and returns a given sequence item, if it exists. Otherwise sets 'item'
     *  to NULL and returns EC_TagNotFound (specified sequence does not exist) or
     *  EC_IllegalParameter (specified item does not exist). Only the top-most level of
     *  the dataset/item is examined (i.e. no deep-search is performed).
     *  @param seqTagKey DICOM tag specifying the sequence attribute to be searched for
     *  @param item variable in which the reference to the sequence item is stored
     *  @param itemNum number of the item to be searched for (0..n-1, -1 for last)
     *  @return EC_Normal upon success, an error otherwise.
     */
    OFCondition findAndGetSequenceItem(const DcmTagKey &seqTagKey,
                                       DcmItem *&item,
                                       const signed long itemNum = 0);


    /* --- findOrCreate functions: find an element or create a new one --- */

    /** looks up the given sequence in the current dataset and returns the given item.
     *  If either the sequence or the item do not exist, they are created. If necessary,
     *  multiple empty items are inserted. Only the top-most level of the dataset/item
     *  is examined (i.e. no deep-search is performed).
     *  @param seqTag DICOM tag specifying the sequence attribute to be searched for or
     *    to be create respectively
     *  @param item variable in which the reference to the sequence item is stored
     *  @param itemNum number of the item to be searched for (0..n-1, -1 for last,
     *    -2 for append new)
     *  @return EC_Normal upon success, an error otherwise.
     */
    OFCondition findOrCreateSequenceItem(const DcmTag& seqTag,
                                         DcmItem *&item,
                                         const signed long itemNum = 0);


    /* --- putAndInsert functions: put value and insert new element --- */

    /** create a new element, put specified value to it and insert the element into the dataset/item.
     *  Applicable to the following VRs: AE, AS, CS, DA, DS, DT, FL, FD, IS, LO, LT, PN, SH, ST, TM, UI, UT
     *  @param tag DICOM tag specifying the attribute to be created
     *  @param value value to be set for the new element (might be empty or NULL)
     *  @param replaceOld flag indicating whether to replace an existing element or not
     *  @return EC_Normal upon success, an error code otherwise.
     */
    OFCondition putAndInsertString(const DcmTag& tag,
                                   const char *value,
                                   const OFBool replaceOld = OFTrue);

    /** create a new element, put specified value to it and insert the element into the dataset/item.
     *  Applicable to the following VRs: AE, AS, CS, DA, DS, DT, IS, LO, LT, PN, SH, ST, TM, UI, UT
     *  @param tag DICOM tag specifying the attribute to be created
     *  @param value value to be set for the new element (might be empty)
     *  @param replaceOld flag indicating whether to replace an existing element or not
     *  @return EC_Normal upon success, an error code otherwise.
     */
    OFCondition putAndInsertOFStringArray(const DcmTag& tag,
                                          const OFString &value,
                                          const OFBool replaceOld = OFTrue);

    /** create a new element, put specified value to it and insert the element into the dataset/item.
     *  Applicable to the following VRs: OB
     *  @param tag DICOM tag specifying the attribute to be created
     *  @param value value to be set for the new element (might be NULL)
     *  @param count number of values (= bytes in this case) to be copied from 'value'
     *  @param replaceOld flag indicating whether to replace an existing element or not
     *  @return EC_Normal upon success, an error code otherwise.
     */
    OFCondition putAndInsertUint8Array(const DcmTag& tag,
                                       const Uint8 *value,
                                       const unsigned long count,
                                       const OFBool replaceOld = OFTrue);

    /** create a new element, put specified value to it and insert the element into the dataset/item.
     *  Applicable to the following VRs: US
     *  @param tag DICOM tag specifying the attribute to be created
     *  @param value value to be set for the new element
     *  @param replaceOld flag indicating whether to replace an existing element or not
     *  @return EC_Normal upon success, an error code otherwise.
     */
    OFCondition putAndInsertUint16(const DcmTag& tag,
                                   const Uint16 value,
                                   const OFBool replaceOld = OFTrue);

    /** create a new element, put specified value to it and insert the element into the dataset/item.
     *  Applicable to the following VRs: AT, OW, US
     *  @param tag DICOM tag specifying the attribute to be created
     *  @param value value to be set for the new element (might be NULL)
     *  @param count number of values (not bytes!) to be copied from 'value'
     *  @param replaceOld flag indicating whether to replace an existing element or not
     *  @return EC_Normal upon success, an error code otherwise.
     */
    OFCondition putAndInsertUint16Array(const DcmTag& tag,
                                        const Uint16 *value,
                                        const unsigned long count,
                                        const OFBool replaceOld = OFTrue);

    /** create a new element, put specified value to it and insert the element into the dataset/item.
     *  Applicable to the following VRs: SS
     *  @param tag DICOM tag specifying the attribute to be created
     *  @param value value to be set for the new element
     *  @param replaceOld flag indicating whether to replace an existing element or not
     *  @return EC_Normal upon success, an error code otherwise.
     */
    OFCondition putAndInsertSint16(const DcmTag& tag,
                                   const Sint16 value,
                                   const OFBool replaceOld = OFTrue);

    /** create a new element, put specified value to it and insert the element into the dataset/item.
     *  Applicable to the following VRs: SS
     *  @param tag DICOM tag specifying the attribute to be created
     *  @param value value to be set for the new element
     *  @param count number of values (not bytes!) to be copied from 'value'
     *  @param replaceOld flag indicating whether to replace an existing element or not
     *  @return EC_Normal upon success, an error code otherwise.
     */
    OFCondition putAndInsertSint16Array(const DcmTag& tag,
                                        const Sint16 *value,
                                        const unsigned long count,
                                        const OFBool replaceOld = OFTrue);

    /** create a new element, put specified value to it and insert the element into the dataset/item.
     *  Applicable to the following VRs: UL
     *  @param tag DICOM tag specifying the attribute to be created
     *  @param value value to be set for the new element
     *  @param replaceOld flag indicating whether to replace an existing element or not
     *  @return EC_Normal upon success, an error code otherwise.
     */
    OFCondition putAndInsertUint32(const DcmTag& tag,
                                   const Uint32 value,
                                   const OFBool replaceOld = OFTrue);

    /** create a new element, put specified value to it and insert the element into the dataset/item.
     *  Applicable to the following VRs: SL
     *  @param tag DICOM tag specifying the attribute to be created
     *  @param value value to be set for the new element
     *  @param replaceOld flag indicating whether to replace an existing element or not
     *  @return EC_Normal upon success, an error code otherwise.
     */
    OFCondition putAndInsertSint32(const DcmTag& tag,
                                   const Sint32 value,
                                   const OFBool replaceOld = OFTrue);

    /** create a new element, put specified value to it and insert the element into the dataset/item.
     *  Applicable to the following VRs: FL
     *  @param tag DICOM tag specifying the attribute to be created
     *  @param value value to be set for the new element
     *  @param replaceOld flag indicating whether to replace an existing element or not
     *  @return EC_Normal upon success, an error code otherwise.
     */
    OFCondition putAndInsertFloat32(const DcmTag& tag,
                                    const Float32 value,
                                    const OFBool replaceOld = OFTrue);

    /** create a new element, put specified value to it and insert the element into the dataset/item.
     *  Applicable to the following VRs: FD
     *  @param tag DICOM tag specifying the attribute to be created
     *  @param value value to be set for the new element
     *  @param replaceOld flag indicating whether to replace an existing element or not
     *  @return EC_Normal upon success, an error code otherwise.
     */
    OFCondition putAndInsertFloat64(const DcmTag& tag,
                                    const Float64 value,
                                    const OFBool replaceOld = OFTrue);

    /** create a new element (with no value) and insert it into the dataset/item.
     *  Applicable to the following VRs: AE, AS, AT, CS, DA, DS, DT, FL, FD, IS, LO, OB, OW, LT, PN, SH, SQ, ST, TM, UI, UT
     *  @param tag DICOM tag specifying the attribute to be created
     *  @param replaceOld flag indicating whether to replace an existing element or not
     *  @return EC_Normal upon success, an error code otherwise.
     */
    OFCondition insertEmptyElement(const DcmTag& tag,
                                   const OFBool replaceOld = OFTrue);
};


//
// SUPPORT FUNCTIONS
//


// Function: newDicomElement
// creates a new DicomElement from a Tag.
//
// Input:
//   tag    : Tag of the new element
//   length : length of the element value
//
// Output:
//   newElement: point of a heap allocated new element. If the tag does not
//               describe a dicom element or has ambigious VR (e.g. EVR_ox)
//               a NULL pointer is returned.
//
// Result:
//   EC_Normal:     tag describes an element (possibly with ambiguous VR)
//   EC_InvalidTag: tag describes an item begin or an unknown element
//   EC_SequEnd:    tag describes a sequence delimitation element
//   EC_ItemEnd:    tag describes an item delmitation element
//   other: an error
OFCondition newDicomElement(DcmElement * & newElement,
                            const DcmTag & tag,
                            const Uint32 length = 0);



// Functions: newDicomElement
// creates a new DicomElement from a Tag. They differ from the above functions
// in not returning a condition.
DcmElement * newDicomElement(const DcmTag & tag,
                             const Uint32 length = 0);

// Function: nextUp
// pop Object from stack and get next Object in top of stack

OFCondition nextUp(DcmStack & stack);


#endif // DCITEM_H

/*
** CVS/RCS Log:
** $Log: dcitem.h,v $
** Revision 1.33  2001-12-18 11:37:24  joergr
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
