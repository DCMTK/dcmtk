/*
 *
 *  Copyright (C) 2008-2012, OFFIS e.V. and ICSMED AG, Oldenburg, Germany
 *  Copyright (C) 2013-2017, J. Riesmeier, Oldenburg, Germany
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  Header file for class DRTOverrideSequence
 *
 *  Generated automatically from DICOM PS 3.3-2017e
 *  File created on 2017-12-05 09:30:54
 *
 */


#ifndef DRTOS_H
#define DRTOS_H

#include "dcmtk/config/osconfig.h"     // make sure OS specific configuration is included first

#include "dcmtk/ofstd/oflist.h"        // for standard list class
#include "dcmtk/dcmrt/drttypes.h"      // module-specific helper class
#include "dcmtk/dcmrt/seq/drtois.h"    // for OperatorIdentificationSequence


/** Interface class for OverrideSequence (3008,0060)
 */
class DCMTK_DCMRT_EXPORT DRTOverrideSequence
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
         *  @return reference to this object
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

        /** get OperatorsName (0008,1070)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getOperatorsName(OFString &value, const signed long pos = 0) const;

        /** get OverrideParameterPointer (3008,0062)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getOverrideParameterPointer(OFString &value, const signed long pos = 0) const;

        /** get OverrideReason (3008,0066)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getOverrideReason(OFString &value, const signed long pos = 0) const;

        /** get ParameterItemIndex (3008,0063)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getParameterItemIndex(OFString &value, const signed long pos = 0) const;

        /** get ParameterItemIndex (3008,0063)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1)
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getParameterItemIndex(Sint32 &value, const unsigned long pos = 0) const;

        /** get ParameterSequencePointer (3008,0061)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getParameterSequencePointer(OFString &value, const signed long pos = 0) const;

        /** get ParameterValueNumber (3008,0067)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1)
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getParameterValueNumber(Uint16 &value, const unsigned long pos = 0) const;

      // --- get DICOM sequence attributes ---

        /** get OperatorIdentificationSequence (0008,1072)
         *  @return reference to sequence element
         */
        DRTOperatorIdentificationSequence &getOperatorIdentificationSequence()
            { return OperatorIdentificationSequence; }

        /** get OperatorIdentificationSequence (0008,1072)
         *  @return const reference to sequence element
         */
        const DRTOperatorIdentificationSequence &getOperatorIdentificationSequence() const
            { return OperatorIdentificationSequence; }

      // --- set DICOM attribute values ---

        /** set OperatorsName (0008,1070)
         *  @param  value  value to be set (possibly multi-valued) or "" for no value
         *  @param  check  check 'value' for conformance with VR (PN) and VM (1-n) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setOperatorsName(const OFString &value, const OFBool check = OFTrue);

        /** set OverrideParameterPointer (3008,0062)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (AT) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setOverrideParameterPointer(const OFString &value, const OFBool check = OFTrue);

        /** set OverrideReason (3008,0066)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (ST) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setOverrideReason(const OFString &value, const OFBool check = OFTrue);

        /** set ParameterItemIndex (3008,0063)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (IS) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setParameterItemIndex(const OFString &value, const OFBool check = OFTrue);

        /** set ParameterSequencePointer (3008,0061)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (AT) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setParameterSequencePointer(const OFString &value, const OFBool check = OFTrue);

        /** set ParameterValueNumber (3008,0067)
         *  @param  value  value to be set (should be valid for this VR)
         *  @param  pos    index of the value to be set (0..vm-1), vm=1
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setParameterValueNumber(const Uint16 value, const unsigned long pos = 0);

      private:

        /// internal flag used to mark the empty default item
        /*const*/ OFBool EmptyDefaultItem;

        /// OperatorIdentificationSequence (0008,1072) vr=SQ, vm=1, type=3
        DRTOperatorIdentificationSequence OperatorIdentificationSequence;
        /// OperatorsName (0008,1070) vr=PN, vm=1-n, type=2
        DcmPersonName OperatorsName;
        /// OverrideParameterPointer (3008,0062) vr=AT, vm=1, type=1
        DcmAttributeTag OverrideParameterPointer;
        /// OverrideReason (3008,0066) vr=ST, vm=1, type=3
        DcmShortText OverrideReason;
        /// ParameterItemIndex (3008,0063) vr=IS, vm=1, type=1
        DcmIntegerString ParameterItemIndex;
        /// ParameterSequencePointer (3008,0061) vr=AT, vm=1, type=1
        DcmAttributeTag ParameterSequencePointer;
        /// ParameterValueNumber (3008,0067) vr=US, vm=1, type=3
        DcmUnsignedShort ParameterValueNumber;

    };

  // --- constructors, destructor and operators ---

    /** (default) constructor
     *  @param emptyDefaultSequence internal flag used to mark the empty default sequence
     */
    DRTOverrideSequence(const OFBool emptyDefaultSequence = OFFalse);

    /** copy constructor
     *  @param copy sequence object to be copied
     */
    DRTOverrideSequence(const DRTOverrideSequence &copy);

    /** destructor
     */
    virtual ~DRTOverrideSequence();

    /** assigment operator
     *  @param copy sequence object to be copied
     *  @return reference to this object
     */
    DRTOverrideSequence &operator=(const DRTOverrideSequence &copy);

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
    size_t getNumberOfItems() const;

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
    OFCondition gotoItem(const size_t num);

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
    OFCondition getItem(const size_t num, Item *&item);

    /** get particular item in the sequence
     *  @param  num  number of the item to be retrieved (0..num-1)
     *  @return reference to specified item if successful, empty default item otherwise
     */
    Item &getItem(const size_t num);

    /** get particular item in the sequence
     *  @param  num  number of the item to be retrieved (0..num-1)
     *  @return const reference to specified item if successful, empty default item otherwise
     */
    const Item &getItem(const size_t num) const;

    /** get particular item in the sequence
     *  @param  num  number of the item to be retrieved (0..num-1)
     *  @return reference to specified item if successful, empty default item otherwise
     */
    Item &operator[](const size_t num);

    /** get particular item in the sequence
     *  @param  num  number of the item to be retrieved (0..num-1)
     *  @return const reference to specified item if successful, empty default item otherwise
     */
    const Item &operator[](const size_t num) const;

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
    OFCondition insertItem(const size_t pos, Item *&item);

    /** remove particular item from the sequence
     *  @param  pos  position of the item to be removed (0..num-1)
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition removeItem(const size_t pos);

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
    OFCondition gotoItem(const size_t num,
                         OFListIterator(Item *) &iterator);

    /** goto particular item in the sequence
     *  @param  num       number of the item to be selected (0..num-1)
     *  @param  iterator  list iterator storing the position of the item
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition gotoItem(const size_t num,
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
