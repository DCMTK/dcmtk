/*
 *
 *  Copyright (C) 2008-2012, OFFIS e.V. and ICSMED AG, Oldenburg, Germany
 *  Copyright (C) 2013-2015, J. Riesmeier, Oldenburg, Germany
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  Header file for class DRTDigitalSignaturesSequence
 *
 *  Generated automatically from DICOM PS 3.3-2015c
 *  File created on 2015-12-07 16:29:33
 *
 */


#ifndef DRTDSS_H
#define DRTDSS_H

#include "dcmtk/config/osconfig.h"     // make sure OS specific configuration is included first

#include "dcmtk/ofstd/oflist.h"        // for standard list class
#include "dcmtk/dcmrt/drttypes.h"      // module-specific helper class
#include "dcmtk/dcmrt/seq/drtdspcs.h"  // for DigitalSignaturePurposeCodeSequence


/** Interface class for DigitalSignaturesSequence (fffa,fffa)
 */
class DCMTK_DCMRT_EXPORT DRTDigitalSignaturesSequence
  : protected DRTTypes
{

  public:

    /** Item class
     */
    class DCMTK_DCMRT_EXPORT Item
      : protected DRTTypes
    {

      public:

      // --- constructors, destructor and operators ---

        /** (default) constructor
         *  @param emptyDefaultItem flag used to mark the empty default item
         */
        Item(const OFBool emptyDefaultItem = OFFalse);

        /** copy constructor
         *  @param copy item object to be copied
         */
        Item(const Item &copy);

        /** destructor
         */
        virtual ~Item();

        /** assigment operator
         *  @param copy item object to be copied
         */
        Item &operator=(const Item &copy);

      // --- general methods ---

        /** clear all internal member variables
         */
        void clear();

        /** check if item is empty
         *  @return OFTrue if item is empty, OFFalse otherwise
         */
        OFBool isEmpty();

        /** check if item is valid, i.e.\ not the empty default item
         *  @return OFTrue if item is valid, OFFalse otherwise
         */
        OFBool isValid() const;

      // --- input/output methods ---

        /** read elements from sequence item
         *  @param  item    reference to DICOM sequence item from which the elements should be read
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition read(DcmItem &item);

        /** write elements to sequence item
         *  @param  item    reference to DICOM sequence item to which the elements should be written
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition write(DcmItem &item);

      // --- get DICOM attribute values ---

        /** get CertificateOfSigner (0400,0115)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  count  stores number of items in the result array (if not NULL)
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getCertificateOfSigner(Uint8 *&value, unsigned long *count = NULL) const;

        /** get CertificateType (0400,0110)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getCertificateType(OFString &value, const signed long pos = 0) const;

        /** get CertifiedTimestamp (0400,0310)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  count  stores number of items in the result array (if not NULL)
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getCertifiedTimestamp(Uint8 *&value, unsigned long *count = NULL) const;

        /** get CertifiedTimestampType (0400,0305)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getCertifiedTimestampType(OFString &value, const signed long pos = 0) const;

        /** get DigitalSignatureDateTime (0400,0105)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getDigitalSignatureDateTime(OFString &value, const signed long pos = 0) const;

        /** get DigitalSignatureUID (0400,0100)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getDigitalSignatureUID(OFString &value, const signed long pos = 0) const;

        /** get MACIDNumber (0400,0005)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1)
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getMACIDNumber(Uint16 &value, const unsigned long pos = 0) const;

        /** get Signature (0400,0120)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  count  stores number of items in the result array (if not NULL)
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getSignature(Uint8 *&value, unsigned long *count = NULL) const;

      // --- get DICOM sequence attributes ---

        /** get DigitalSignaturePurposeCodeSequence (0400,0401)
         *  @return reference to sequence element
         */
        DRTDigitalSignaturePurposeCodeSequence &getDigitalSignaturePurposeCodeSequence()
            { return DigitalSignaturePurposeCodeSequence; }

        /** get DigitalSignaturePurposeCodeSequence (0400,0401)
         *  @return const reference to sequence element
         */
        const DRTDigitalSignaturePurposeCodeSequence &getDigitalSignaturePurposeCodeSequence() const
            { return DigitalSignaturePurposeCodeSequence; }

      // --- set DICOM attribute values ---

        /** set CertificateOfSigner (0400,0115)
         *  @param  value  array value to be set (data is copied)
         *  @param  count  number of items in the array
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setCertificateOfSigner(const Uint8 *value, const unsigned long count);

        /** set CertificateType (0400,0110)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (CS) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setCertificateType(const OFString &value, const OFBool check = OFTrue);

        /** set CertifiedTimestamp (0400,0310)
         *  @param  value  array value to be set (data is copied)
         *  @param  count  number of items in the array
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setCertifiedTimestamp(const Uint8 *value, const unsigned long count);

        /** set CertifiedTimestampType (0400,0305)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (CS) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setCertifiedTimestampType(const OFString &value, const OFBool check = OFTrue);

        /** set DigitalSignatureDateTime (0400,0105)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (DT) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setDigitalSignatureDateTime(const OFString &value, const OFBool check = OFTrue);

        /** set DigitalSignatureUID (0400,0100)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (UI) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setDigitalSignatureUID(const OFString &value, const OFBool check = OFTrue);

        /** set MACIDNumber (0400,0005)
         *  @param  value  value to be set (should be valid for this VR)
         *  @param  pos    index of the value to be set (0..vm-1), vm=1
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setMACIDNumber(const Uint16 value, const unsigned long pos = 0);

        /** set Signature (0400,0120)
         *  @param  value  array value to be set (data is copied)
         *  @param  count  number of items in the array
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setSignature(const Uint8 *value, const unsigned long count);

      private:

        /// internal flag used to mark the empty default item
        /*const*/ OFBool EmptyDefaultItem;

        /// CertificateOfSigner (0400,0115) vr=OB, vm=1, type=1
        DcmOtherByteOtherWord CertificateOfSigner;
        /// CertificateType (0400,0110) vr=CS, vm=1, type=1
        DcmCodeString CertificateType;
        /// CertifiedTimestamp (0400,0310) vr=OB, vm=1, type=3
        DcmOtherByteOtherWord CertifiedTimestamp;
        /// CertifiedTimestampType (0400,0305) vr=CS, vm=1, type=1C
        DcmCodeString CertifiedTimestampType;
        /// DigitalSignatureDateTime (0400,0105) vr=DT, vm=1, type=1
        DcmDateTime DigitalSignatureDateTime;
        /// DigitalSignaturePurposeCodeSequence (0400,0401) vr=SQ, vm=1, type=3
        DRTDigitalSignaturePurposeCodeSequence DigitalSignaturePurposeCodeSequence;
        /// DigitalSignatureUID (0400,0100) vr=UI, vm=1, type=1
        DcmUniqueIdentifier DigitalSignatureUID;
        /// MACIDNumber (0400,0005) vr=US, vm=1, type=1
        DcmUnsignedShort MACIDNumber;
        /// Signature (0400,0120) vr=OB, vm=1, type=1
        DcmOtherByteOtherWord Signature;

    };

  // --- constructors, destructor and operators ---

    /** (default) constructor
     *  @param emptyDefaultSequence internal flag used to mark the empty default sequence
     */
    DRTDigitalSignaturesSequence(const OFBool emptyDefaultSequence = OFFalse);

    /** copy constructor
     *  @param copy sequence object to be copied
     */
    DRTDigitalSignaturesSequence(const DRTDigitalSignaturesSequence &copy);

    /** destructor
     */
    virtual ~DRTDigitalSignaturesSequence();

    /** assigment operator
     *  @param copy sequence object to be copied
     */
    DRTDigitalSignaturesSequence &operator=(const DRTDigitalSignaturesSequence &copy);

  // --- general methods ---

    /** clear all internal member variables
     */
    void clear();

    /** check if sequence is empty
     *  @return OFTrue if sequence is empty, OFFalse otherwise
     */
    OFBool isEmpty();

    /** check if sequence is valid, i.e.\ not the empty default sequence
     *  @return OFTrue if sequence is valid, OFFalse otherwise
     */
    OFBool isValid() const;

    /** get number of items in the sequence
     *  @return number of items
     */
    unsigned long getNumberOfItems() const;

    /** goto first item in the sequence
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition gotoFirstItem();

    /** goto next item in the sequence
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition gotoNextItem();

    /** goto particular item in the sequence
     *  @param  num  number of the item to be selected (0..num-1)
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition gotoItem(const unsigned long num);

    /** get current item in the sequence
     *  @param  item  reference to item pointer (result variable)
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition getCurrentItem(Item *&item) const;

    /** get current item in the sequence
     *  @return reference to specified item if successful, empty default item otherwise
     */
    Item &getCurrentItem();

    /** get current item in the sequence
     *  @return const reference to specified item if successful, empty default item otherwise
     */
    const Item &getCurrentItem() const;

    /** get particular item in the sequence
     *  @param  num   number of the item to be retrieved (0..num-1)
     *  @param  item  reference to item pointer (result variable)
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition getItem(const unsigned long num, Item *&item);

    /** get particular item in the sequence
     *  @param  num  number of the item to be retrieved (0..num-1)
     *  @return reference to specified item if successful, empty default item otherwise
     */
    Item &getItem(const unsigned long num);

    /** get particular item in the sequence
     *  @param  num  number of the item to be retrieved (0..num-1)
     *  @return const reference to specified item if successful, empty default item otherwise
     */
    const Item &getItem(const unsigned long num) const;

    /** get particular item in the sequence
     *  @param  num  number of the item to be retrieved (0..num-1)
     *  @return reference to specified item if successful, empty default item otherwise
     */
    Item &operator[](const unsigned long num);

    /** get particular item in the sequence
     *  @param  num  number of the item to be retrieved (0..num-1)
     *  @return const reference to specified item if successful, empty default item otherwise
     */
    const Item &operator[](const unsigned long num) const;

    /** add new item to the end of this sequence
     *  @param  item  reference to new item pointer (result variable)
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition addItem(Item *&item);

    /** insert new item into the sequence
     *  @param  pos   position where the new item is to be inserted (0..num)
     *  @param  item  reference to new item pointer (result variable)
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition insertItem(const unsigned long pos, Item *&item);

    /** remove particular item from the sequence
     *  @param  pos  position of the item to be removed (0..num-1)
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition removeItem(const unsigned long pos);

  // --- input/output methods ---

    /** read sequence of items from dataset
     *  @param  dataset     reference to DICOM dataset from which the sequence should be read
     *  @param  card        cardinality (valid range for number of items)
     *  @param  type        value type (valid value: "1", "1C", "2" or something else which is not checked)
     *  @param  moduleName  optional module/sequence name to be printed (default: "RT object" if NULL)
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition read(DcmItem &dataset,
                     const OFString &card,
                     const OFString &type,
                     const char *moduleName = NULL);

    /** write sequence of items to dataset
     *  @param  dataset     reference to DICOM dataset to which the sequence should be written
     *  @param  card        cardinality (valid range for number of items)
     *  @param  type        value type (valid value: "1", "2" or something else which is not checked)
     *  @param  moduleName  optional module/sequence name to be printed (default: "RT object" if NULL)
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition write(DcmItem &dataset,
                      const OFString &card,
                      const OFString &type,
                      const char *moduleName = NULL);

  protected:

    /** goto particular item in the sequence
     *  @param  num       number of the item to be selected (0..num-1)
     *  @param  iterator  list iterator storing the position of the item
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition gotoItem(const unsigned long num,
                         OFListIterator(Item *) &iterator);

    /** goto particular item in the sequence
     *  @param  num       number of the item to be selected (0..num-1)
     *  @param  iterator  list iterator storing the position of the item
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition gotoItem(const unsigned long num,
                         OFListConstIterator(Item *) &iterator) const;

  private:

    /// internal flag used to mark the empty default sequence
    /*const*/ OFBool EmptyDefaultSequence;

    /// list of items in this sequence
    OFList<Item *>         SequenceOfItems;
    /// currently selected item
    OFListIterator(Item *) CurrentItem;
    /// empty default item
    Item                   EmptyItem;

};


#endif
