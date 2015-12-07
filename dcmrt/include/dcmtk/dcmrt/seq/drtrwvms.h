/*
 *
 *  Copyright (C) 2008-2012, OFFIS e.V. and ICSMED AG, Oldenburg, Germany
 *  Copyright (C) 2013-2015, J. Riesmeier, Oldenburg, Germany
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  Header file for class DRTRealWorldValueMappingSequence
 *
 *  Generated automatically from DICOM PS 3.3-2015c
 *  File created on 2015-12-07 16:29:33
 *
 */


#ifndef DRTRWVMS_H
#define DRTRWVMS_H

#include "dcmtk/config/osconfig.h"     // make sure OS specific configuration is included first

#include "dcmtk/ofstd/oflist.h"        // for standard list class
#include "dcmtk/dcmrt/drttypes.h"      // module-specific helper class
#include "dcmtk/dcmrt/seq/drtmucs.h"   // for MeasurementUnitsCodeSequence
#include "dcmtk/dcmrt/seq/drtqds.h"    // for QuantityDefinitionSequence


/** Interface class for RealWorldValueMappingSequence (0040,9096)
 */
class DCMTK_DCMRT_EXPORT DRTRealWorldValueMappingSequence
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

        /** get LUTExplanation (0028,3003)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getLUTExplanation(OFString &value, const signed long pos = 0) const;

        /** get LUTLabel (0040,9210)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1), -1 for all components
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getLUTLabel(OFString &value, const signed long pos = 0) const;

        /** get RealWorldValueFirstValueMapped (0040,9216)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1)
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getRealWorldValueFirstValueMapped(Uint16 &value, const unsigned long pos = 0) const;

        /** get RealWorldValueIntercept (0040,9224)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1)
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getRealWorldValueIntercept(Float64 &value, const unsigned long pos = 0) const;

        /** get RealWorldValueLUTData (0040,9212)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1)
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getRealWorldValueLUTData(Float64 &value, const unsigned long pos = 0) const;

        /** get RealWorldValueLastValueMapped (0040,9211)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1)
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getRealWorldValueLastValueMapped(Uint16 &value, const unsigned long pos = 0) const;

        /** get RealWorldValueSlope (0040,9225)
         *  @param  value  reference to variable in which the value should be stored
         *  @param  pos    index of the value to get (0..vm-1)
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition getRealWorldValueSlope(Float64 &value, const unsigned long pos = 0) const;

      // --- get DICOM sequence attributes ---

        /** get MeasurementUnitsCodeSequence (0040,08ea)
         *  @return reference to sequence element
         */
        DRTMeasurementUnitsCodeSequence &getMeasurementUnitsCodeSequence()
            { return MeasurementUnitsCodeSequence; }

        /** get MeasurementUnitsCodeSequence (0040,08ea)
         *  @return const reference to sequence element
         */
        const DRTMeasurementUnitsCodeSequence &getMeasurementUnitsCodeSequence() const
            { return MeasurementUnitsCodeSequence; }

        /** get QuantityDefinitionSequence (0040,9220)
         *  @return reference to sequence element
         */
        DRTQuantityDefinitionSequence &getQuantityDefinitionSequence()
            { return QuantityDefinitionSequence; }

        /** get QuantityDefinitionSequence (0040,9220)
         *  @return const reference to sequence element
         */
        const DRTQuantityDefinitionSequence &getQuantityDefinitionSequence() const
            { return QuantityDefinitionSequence; }

      // --- set DICOM attribute values ---

        /** set LUTExplanation (0028,3003)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (LO) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setLUTExplanation(const OFString &value, const OFBool check = OFTrue);

        /** set LUTLabel (0040,9210)
         *  @param  value  value to be set (single value only) or "" for no value
         *  @param  check  check 'value' for conformance with VR (SH) and VM (1) if enabled
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setLUTLabel(const OFString &value, const OFBool check = OFTrue);

        /** set RealWorldValueFirstValueMapped (0040,9216)
         *  @param  value  value to be set (should be valid for this VR)
         *  @param  pos    index of the value to be set (0..vm-1), vm=1
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setRealWorldValueFirstValueMapped(const Uint16 value, const unsigned long pos = 0);

        /** set RealWorldValueIntercept (0040,9224)
         *  @param  value  value to be set (should be valid for this VR)
         *  @param  pos    index of the value to be set (0..vm-1), vm=1
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setRealWorldValueIntercept(const Float64 value, const unsigned long pos = 0);

        /** set RealWorldValueLUTData (0040,9212)
         *  @param  value  value to be set (should be valid for this VR)
         *  @param  pos    index of the value to be set (0..vm-1), vm=1-n
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setRealWorldValueLUTData(const Float64 value, const unsigned long pos = 0);

        /** set RealWorldValueLastValueMapped (0040,9211)
         *  @param  value  value to be set (should be valid for this VR)
         *  @param  pos    index of the value to be set (0..vm-1), vm=1
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setRealWorldValueLastValueMapped(const Uint16 value, const unsigned long pos = 0);

        /** set RealWorldValueSlope (0040,9225)
         *  @param  value  value to be set (should be valid for this VR)
         *  @param  pos    index of the value to be set (0..vm-1), vm=1
         *  @return status, EC_Normal if successful, an error code otherwise
         */
        OFCondition setRealWorldValueSlope(const Float64 value, const unsigned long pos = 0);

      private:

        /// internal flag used to mark the empty default item
        /*const*/ OFBool EmptyDefaultItem;

        /// LUTExplanation (0028,3003) vr=LO, vm=1, type=1
        DcmLongString LUTExplanation;
        /// LUTLabel (0040,9210) vr=SH, vm=1, type=1
        DcmShortString LUTLabel;
        /// MeasurementUnitsCodeSequence (0040,08ea) vr=SQ, vm=1, type=1
        DRTMeasurementUnitsCodeSequence MeasurementUnitsCodeSequence;
        /// QuantityDefinitionSequence (0040,9220) vr=SQ, vm=1, type=3
        DRTQuantityDefinitionSequence QuantityDefinitionSequence;
        /// RealWorldValueFirstValueMapped (0040,9216) vr=US/SS, vm=1, type=1
        DcmUnsignedShort RealWorldValueFirstValueMapped;
        /// RealWorldValueIntercept (0040,9224) vr=FD, vm=1, type=1C
        DcmFloatingPointDouble RealWorldValueIntercept;
        /// RealWorldValueLUTData (0040,9212) vr=FD, vm=1-n, type=1C
        DcmFloatingPointDouble RealWorldValueLUTData;
        /// RealWorldValueLastValueMapped (0040,9211) vr=US/SS, vm=1, type=1
        DcmUnsignedShort RealWorldValueLastValueMapped;
        /// RealWorldValueSlope (0040,9225) vr=FD, vm=1, type=1C
        DcmFloatingPointDouble RealWorldValueSlope;

    };

  // --- constructors, destructor and operators ---

    /** (default) constructor
     *  @param emptyDefaultSequence internal flag used to mark the empty default sequence
     */
    DRTRealWorldValueMappingSequence(const OFBool emptyDefaultSequence = OFFalse);

    /** copy constructor
     *  @param copy sequence object to be copied
     */
    DRTRealWorldValueMappingSequence(const DRTRealWorldValueMappingSequence &copy);

    /** destructor
     */
    virtual ~DRTRealWorldValueMappingSequence();

    /** assigment operator
     *  @param copy sequence object to be copied
     */
    DRTRealWorldValueMappingSequence &operator=(const DRTRealWorldValueMappingSequence &copy);

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
