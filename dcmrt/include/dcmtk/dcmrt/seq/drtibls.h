/*
 *
 *  Copyright (C) 2008-2012, OFFIS e.V. and ICSMED AG, Oldenburg, Germany
 *  Copyright (C) 2013-2023, J. Riesmeier, Oldenburg, Germany
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  Header file for class DRTIonBlockSequence
 *
 *  Generated automatically from DICOM PS 3.3-2023b
 *  File created on 2023-05-19 16:00:57
 *
 */


#ifndef DRTIBLS_H
#define DRTIBLS_H

#include "dcmtk/config/osconfig.h"     // make sure OS specific configuration is included first

#include "dcmtk/ofstd/oflist.h"        // for standard list class
#include "dcmtk/dcmrt/drttypes.h"      // module-specific helper class
#include "dcmtk/dcmrt/seq/drtbss.h"    // for BlockSlabSequence


/** Interface class for IonBlockSequence (300a,03a6)
 */
class DCMTK_DCMRT_EXPORT DRTIonBlockSequence
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

        /** assignment operator
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

        /** get AccessoryCode (300a,00f9)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getAccessoryCode(OFString &value, const signed long pos = 0) const;

        /** get BlockData (300a,0106)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getBlockData(OFString &value, const signed long pos = 0) const;

        /** get BlockData (300a,0106)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1)
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getBlockData(Float64 &value, const unsigned long pos = 0) const;

        /** get BlockData (300a,0106)
         *  @param  value  reference to variable in which the value(s) should be stored
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getBlockData(OFVector<Float64> &value) const;

        /** get BlockDivergence (300a,00fa)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getBlockDivergence(OFString &value, const signed long pos = 0) const;

        /** get BlockMountingPosition (300a,00fb)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getBlockMountingPosition(OFString &value, const signed long pos = 0) const;

        /** get BlockName (300a,00fe)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getBlockName(OFString &value, const signed long pos = 0) const;

        /** get BlockNumber (300a,00fc)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getBlockNumber(OFString &value, const signed long pos = 0) const;

        /** get BlockNumber (300a,00fc)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1)
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getBlockNumber(Sint32 &value, const unsigned long pos = 0) const;

        /** get BlockNumberOfPoints (300a,0104)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getBlockNumberOfPoints(OFString &value, const signed long pos = 0) const;

        /** get BlockNumberOfPoints (300a,0104)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1)
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getBlockNumberOfPoints(Sint32 &value, const unsigned long pos = 0) const;

        /** get BlockThickness (300a,0100)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getBlockThickness(OFString &value, const signed long pos = 0) const;

        /** get BlockThickness (300a,0100)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1)
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getBlockThickness(Float64 &value, const unsigned long pos = 0) const;

        /** get BlockTrayID (300a,00f5)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getBlockTrayID(OFString &value, const signed long pos = 0) const;

        /** get BlockType (300a,00f8)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getBlockType(OFString &value, const signed long pos = 0) const;

        /** get IsocenterToBlockTrayDistance (300a,00f7)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1)
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getIsocenterToBlockTrayDistance(Float32 &value, const unsigned long pos = 0) const;

        /** get MaterialID (300a,00e1)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getMaterialID(OFString &value, const signed long pos = 0) const;

        /** get NumberOfBlockSlabItems (300a,0440)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getNumberOfBlockSlabItems(OFString &value, const signed long pos = 0) const;

        /** get NumberOfBlockSlabItems (300a,0440)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1)
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getNumberOfBlockSlabItems(Sint32 &value, const unsigned long pos = 0) const;

      // --- get DICOM sequence attributes ---

        /** get BlockSlabSequence (300a,0441)
         *  @return reference to sequence element
         */
        DRTBlockSlabSequence &getBlockSlabSequence()
            { return BlockSlabSequence; }

        /** get BlockSlabSequence (300a,0441)
         *  @return const reference to sequence element
         */
        const DRTBlockSlabSequence &getBlockSlabSequence() const
            { return BlockSlabSequence; }

      // --- set DICOM attribute values ---

        /** set AccessoryCode (300a,00f9)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (LO) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setAccessoryCode(const OFString &value, const OFBool check = OFTrue);

        /** set BlockData (300a,0106)
         *  @param  value  value to be set (possibly multi-valued) or "" for no value
         *  @param  check  check 'value' for conformance with VR (DS) and VM (2-2n) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setBlockData(const OFString &value, const OFBool check = OFTrue);

        /** set BlockDivergence (300a,00fa)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (CS) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setBlockDivergence(const OFString &value, const OFBool check = OFTrue);

        /** set BlockMountingPosition (300a,00fb)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (CS) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setBlockMountingPosition(const OFString &value, const OFBool check = OFTrue);

        /** set BlockName (300a,00fe)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (LO) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setBlockName(const OFString &value, const OFBool check = OFTrue);

        /** set BlockNumber (300a,00fc)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (IS) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setBlockNumber(const OFString &value, const OFBool check = OFTrue);

        /** set BlockNumberOfPoints (300a,0104)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (IS) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setBlockNumberOfPoints(const OFString &value, const OFBool check = OFTrue);

        /** set BlockThickness (300a,0100)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (DS) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setBlockThickness(const OFString &value, const OFBool check = OFTrue);

        /** set BlockTrayID (300a,00f5)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (SH) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setBlockTrayID(const OFString &value, const OFBool check = OFTrue);

        /** set BlockType (300a,00f8)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (CS) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setBlockType(const OFString &value, const OFBool check = OFTrue);

        /** set IsocenterToBlockTrayDistance (300a,00f7)
         *  @param  value  value to be set (should be valid for this VR)
         *  @param  pos    index of the value to be set (0..vm-1), vm=1
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setIsocenterToBlockTrayDistance(const Float32 value, const unsigned long pos = 0);

        /** set MaterialID (300a,00e1)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (SH) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setMaterialID(const OFString &value, const OFBool check = OFTrue);

        /** set NumberOfBlockSlabItems (300a,0440)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (IS) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setNumberOfBlockSlabItems(const OFString &value, const OFBool check = OFTrue);

      private:

        /// internal flag used to mark the empty default item
        /*const*/ OFBool EmptyDefaultItem;

        /// AccessoryCode (300a,00f9) vr=LO, vm=1, type=3
        DcmLongString AccessoryCode;
        /// BlockData (300a,0106) vr=DS, vm=2-2n, type=1
        DcmDecimalString BlockData;
        /// BlockDivergence (300a,00fa) vr=CS, vm=1, type=1
        DcmCodeString BlockDivergence;
        /// BlockMountingPosition (300a,00fb) vr=CS, vm=1, type=1
        DcmCodeString BlockMountingPosition;
        /// BlockName (300a,00fe) vr=LO, vm=1, type=3
        DcmLongString BlockName;
        /// BlockNumber (300a,00fc) vr=IS, vm=1, type=1
        DcmIntegerString BlockNumber;
        /// BlockNumberOfPoints (300a,0104) vr=IS, vm=1, type=1
        DcmIntegerString BlockNumberOfPoints;
        /// BlockSlabSequence (300a,0441) vr=SQ, vm=1, type=1C
        DRTBlockSlabSequence BlockSlabSequence;
        /// BlockThickness (300a,0100) vr=DS, vm=1, type=1
        DcmDecimalString BlockThickness;
        /// BlockTrayID (300a,00f5) vr=SH, vm=1, type=3
        DcmShortString BlockTrayID;
        /// BlockType (300a,00f8) vr=CS, vm=1, type=1
        DcmCodeString BlockType;
        /// IsocenterToBlockTrayDistance (300a,00f7) vr=FL, vm=1, type=1
        DcmFloatingPointSingle IsocenterToBlockTrayDistance;
        /// MaterialID (300a,00e1) vr=SH, vm=1, type=2
        DcmShortString MaterialID;
        /// NumberOfBlockSlabItems (300a,0440) vr=IS, vm=1, type=3
        DcmIntegerString NumberOfBlockSlabItems;

    };

  // --- constructors, destructor and operators ---

    /** (default) constructor
     *  @param emptyDefaultSequence internal flag used to mark the empty default sequence
     */
    DRTIonBlockSequence(const OFBool emptyDefaultSequence = OFFalse);

    /** copy constructor
     *  @param copy sequence object to be copied
     */
    DRTIonBlockSequence(const DRTIonBlockSequence &copy);

    /** destructor
     */
    virtual ~DRTIonBlockSequence();

    /** assignment operator
     *  @param copy sequence object to be copied
     *  @return reference to this object
     */
    DRTIonBlockSequence &operator=(const DRTIonBlockSequence &copy);

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

    /** create and add new item to the end of this sequence
     *  @param  item  reference to new item pointer (result variable)
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition addItem(Item *&item);

    /** create and insert new item into the sequence
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
